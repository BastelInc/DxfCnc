//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "GCodeObject.h"
#include "Object.h"
#include "Resource.h"
#include "stdlib.h"
#include <FL/Fl_Value_Slider.H>
#include <math.h>
#include <sys/stat.h>

#include "DxfCnc.h"
#include "GCodeDown.h"
#include "GlDisplay.h"
#include "Serialcom.h"
#include "Settings.h"

//-------------------------------------------------------------
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef struct FloatPoint {
  float a;
  float x;
  float y;
  float z;
} FloatPoint;

#define CURVE_SECTION_MM 0.5

/*static FloatPoint current_units;
static FloatPoint target_units;
static float feedrate = 0.0;
*/
static long ActualFeddrate = 0;
static long courfeedrate = 0;
int bHideRapidTrans = 1;
int AB_Swap = 0;
int gDrehachse = 0;
int gNAuswahl = 0;
static char abs_mode = true; // 0 = incremental; 1 = absolute

double search_string(char key, char instruction[], int string_size);

char has_command(char key, char instruction[], int string_size);
/*
void set_target2(float x, float y, float z)
{
  target_units.x = x;
  target_units.y = y;
  target_units.z = z;
  current_units = target_units;
}

void set_position2(float x, float y, float z)
{
//  current_units.x = x;
//  current_units.y = y;
//  current_units.z = z;
}
*/

//-------------------------------------------------------------
static char GcodeStr[255];
//-------------------------------------------------------------
// look for the number that appears after the char key and return it
double search_string(char key, char instruction[], int string_size)
{
  char temp[10] = "";

  for(uint8_t i = 0; i < string_size; i++) {
    if(toupper(instruction[i]) == key) {
      i++;
      int k = 0;
      while(i < string_size && k < 10) {
        if(instruction[i] == 0 || (k > 3 && instruction[i] == ' '))
          break;

        temp[k] = toupper(instruction[i]);
        i++;
        k++;
      }
      return strtod(temp, NULL);
    }
  }

  return 0;
}

//-------------------------------------------------------------
// look for the command if it exists.
char has_command(char key, char instruction[], int string_size)
{
  for(uint8_t i = 0; i < string_size; i++) {
    if(toupper(instruction[i]) == key)
      return true;
  }

  return false;
}

//-------------------------------------------------------------
bool CObjectManager::GCodeEinlesen(void)
{
#ifndef DREITABLE
  fxAufloesung = (float)gPa.Aufloesung;
  if(fxAufloesung < 1 || fxAufloesung > 5000)
    fxAufloesung = 100.0;
  if(Zoll)
    fxAufloesung *= 25.4f;

  //  memset(&Symbole,0,sizeof(Symbole));
  //  SymAnz     = 0;
  //  ungetCount = 0;
  CNCFreigeben();
  SaveFreigeben();
  LayersFreigeben(true);
  gStopp = false;
  int Index;
  FloatPoint Current_Pos;
  Current_Pos.a = 0.0;
  Current_Pos.x = 0.0;
  Current_Pos.y = 0.0;
  Current_Pos.z = 0.0;
  int StartNewObject = 1;
  gDrehachse = 0;
  int Durchmesser = gPa.Maschine[eDrehachse] ? gPa.Maschine[eDurchmesser] : 0;
  AB_Swap = gPa.Maschine[eXYSwap];

  int Color = 3;
  int G0Feddrate = 6000; // 100 mm /Sec
  int G1Feddrate = 180;
  int M3_4PWM = 255;
  int MPWM = 255;
  int Nmr=0;
  int bLaserMode = 0;
  int LaserPower = 100;
  struct stat fileinfo;
  float curve_section = CURVE_SECTION_MM;
  stat(m_FilePfadName, &fileinfo);
#ifdef WIN32
  time = fileinfo.st_mtime;
#else
  time = fileinfo.st_mtim.tv_sec;
#endif
  //  SetBusyDisplay(st.st_size,"Gcode einlesen");
  ActualFeddrate = courfeedrate = gPa.Tempo[Tempo_Verfahr];
  //  uint16_t Lastcode = 0;

  int f = open(m_FilePfadName, O_RDONLY);

  char* buff = (char*)malloc(fileinfo.st_size + 1);
  int len = read(f, buff, fileinfo.st_size);
  if(len == (int)fileinfo.st_size) {
    buff[len] = '\0';
  }
  int LineFeeds = 0;
  for(int t = 0; t < len; t++) {
    if(buff[t] == '\n') {
      LineFeeds++;
    }
  }

  close(f);

  //int Linien = 0;
  int Fpos = 0;

  if(CNCInfoCnt >= AnzahlCNC2 - 1)
    gStopp = true;
  else {
    CGCodeLineObject* pGCodeObject = NULL;
    int p = 0;
    while(p < len) {
      int tlen = 0;
      while(p < len) {
        if(buff[p] == '\r') {
          p++;
          continue;
        }
        if(buff[p] == '\n') {
          p++;
          break;
        }
        if(tlen < (sizeof(GcodeStr) - 2)) {
          GcodeStr[tlen++] = buff[p++];
        } else {
          p++; // Extrem lange Zeile
        }
      }
      if(tlen == 0)
        Fpos = p;
      GcodeStr[tlen++] = '\0';

      if(tlen) {
        char* instruction = GcodeStr;
        int size = strlen(GcodeStr);
        if(instruction[0] == '/' || instruction[0] == ';') {
          // printf("ok\n");
          continue;
        }
        if (instruction[0]=='N') {
          Nmr = atoi(&instruction[1]);
        } else {
          Nmr++;
        }

        // init baby!
        FloatPoint fp;
        fp.a = 0.0;
        fp.x = 0.0;
        fp.y = 0.0;
        fp.z = 0.0;

        uint16_t code = 0;
        ;
        // did we get a gcode?
        if(has_command('G', instruction, size)) {
          code = (int)search_string('G', instruction, size);
          // Get co-ordinates if required by the code type given
          switch(code) {
          case 0:
          case 1:
          case 2:
          case 3:
            if(abs_mode) {
              // we do it like this to save time. makes curves better.
              // eg. if only x and y are specified, we dont have to waste time
              // looking up z.
              if(AB_Swap) {
                if(has_command('B', instruction, size))
                  fp.a = search_string('B', instruction, size);
                else
                  fp.a = Current_Pos.a;
              } else {
                if(has_command('A', instruction, size))
                  fp.a = search_string('A', instruction, size);
                else
                  fp.a = Current_Pos.a;
              }
              if(has_command('X', instruction, size))
                fp.x = search_string('X', instruction, size);
              else
                fp.x = Current_Pos.x;

              if(has_command('Y', instruction, size))
                fp.y = search_string('Y', instruction, size);
              else
                fp.y = Current_Pos.y;

              if(has_command('Z', instruction, size))
                fp.z = search_string('Z', instruction, size);
              else
                fp.z = Current_Pos.z;
            } else {
              if(AB_Swap) {
                fp.a = search_string('B', instruction, size) + Current_Pos.a;
              } else {
                fp.a = search_string('A', instruction, size) + Current_Pos.a;
              }
              fp.x = search_string('X', instruction, size) + Current_Pos.x;
              fp.y = search_string('Y', instruction, size) + Current_Pos.y;
              fp.z = search_string('Z', instruction, size) + Current_Pos.z;
            }

            if(code == 0) {
              // adjust if we have a specific G1Feddrate.
              if(has_command('F', instruction, size))
                G0Feddrate = search_string('F', instruction, size);
              ActualFeddrate = G0Feddrate;
            } else {
              if(has_command('F', instruction, size))
                G1Feddrate = search_string('F', instruction, size);
              ActualFeddrate = G1Feddrate;
            }

            if(bLaserMode == 1) {
              if(has_command('S', instruction, size)) {
                M3_4PWM = (search_string('S', instruction, size));
                LaserPower = M3_4PWM;
              } else {
                // Turn Laser On in Move G1 G2 G3
                // Off in Eilgang G0
                LaserPower = (code == 0) ? 0 : M3_4PWM;
              }
            } else if(bLaserMode == 3) {
              LaserPower = M3_4PWM;
              bLaserMode++;
            } else if(bLaserMode == 4) {
              LaserPower = 0;
              bLaserMode++;
            }
            break;
          }

          // do something!
          if(code < 4 || !StartNewObject) {
            if(has_command('S', instruction, size)) {
              M3_4PWM = (search_string('S', instruction, size))/4.0;
              LaserPower = M3_4PWM;
              bLaserMode = 1;
            }

            if(StartNewObject) {
              if(CNCInfoCnt <= AnzahlCNC2 - 1) {
                CNCInfo[CNCInfoCnt] =
                  new CGCodeLineObject(this, Current_Pos.x * 100.0, Current_Pos.y * 100.0,
                                       Current_Pos.z * 100.0 - Durchmesser/2, Current_Pos.a * 100.0, (char*)"GCode", Color);
                pGCodeObject = (CGCodeLineObject*)CNCInfo[CNCInfoCnt];
                CNCInfoCnt++;
                if(pGCodeObject) {
                  WorkLayer = pGCodeObject->m_pLayer;
                  AktTiefe = Color;
                }
              }
            }
          }
          if(gPa.Maschine[eDurchmesser]) {
            if(fp.z)
              gPa.Maschine[eDurchmesser] = 0;
          }
          if(gDrehachse == 0) {
            if(fp.a)
              gDrehachse = 1;
          }
          switch(code) {
          // Rapid Positioning
          // Linear Interpolation
          // these are basically the same thing.
          case 0:
          case 1:
            // set our target.
          {
            // finally move.
            // return StartMove(ActualFeddrate);
            if(pGCodeObject) {
              //Linien++;
              if(AB_Swap) {
                pGCodeObject->AddVertex((long)(fp.y * 100.0), (long)(fp.x * -100.0),
                                        (long)(fp.z * 100.0), (long)(fp.a * 100.0), LaserPower, code,
                                        (code == 0) ? G0Feddrate : G1Feddrate, Fpos,Nmr);
              } else {
                pGCodeObject->AddVertex((long)(fp.x * 100.0), (long)(fp.y * 100.0),
                                        (long)(fp.z * 100.0), (long)(fp.a * 100.0), LaserPower, code,
                                        (code == 0) ? G0Feddrate : G1Feddrate, Fpos,Nmr);
              }
            }
            StartNewObject = 0;
            Current_Pos.a = fp.a;
            Current_Pos.x = fp.x;
            Current_Pos.y = fp.y;
            Current_Pos.z = fp.z;
          }
          break;

          // Clockwise arc
          case 2:
          // Counterclockwise arc
          case 3: {
            FloatPoint cent;
            // Centre coordinates are always relative
            cent.x = search_string('I', instruction, size) + Current_Pos.x;
            cent.y = search_string('J', instruction, size) + Current_Pos.y;

            float radius, aX, aY, bX, bY;

            aX = (Current_Pos.x - cent.x);
            aY = (Current_Pos.y - cent.y);
            bX = (fp.x - cent.x);
            bY = (fp.y - cent.y);

            float angleA, angleB;

            if(code == 2) { // Clockwise
              angleA = atan2(bY, bX);
              angleB = atan2(aY, aX);
            } else { // Counterclockwise
              angleA = atan2(aY, aX);
              angleB = atan2(bY, bX);
            }

            if(angleB <= (angleA + 0.001))
              angleB += 2 * M_PI;

            float angle = angleB - angleA;
            radius = sqrt(aX * aX + aY * aY);

            float perimeter = radius * angle;

            int steps, s, step;

            steps = (int)ceil(perimeter / curve_section);

            FloatPoint newPoint = Current_Pos; // fp;
            float zd = (fp.z - Current_Pos.z) / steps;
            for(s = 1; s <= steps; s++) {

              step = (code == 3) ? s : steps - s; // Work backwards for CW

              newPoint.x = cent.x + radius * cos(angleA + angle * ((float)step / steps));

              newPoint.y = cent.y + radius * sin(angleA + angle * ((float)step / steps));
              newPoint.z += zd;
              // set_target(newPoint.x, newPoint.y, fp.z);
              if(pGCodeObject) {
                //Linien++;
                if(AB_Swap) {
                  pGCodeObject->AddVertex((long)(newPoint.y * 100.0), (long)(newPoint.x * -100.0),
                                          (long)(newPoint.z * 100.0), (long)(newPoint.a * 100.0), LaserPower, 1,
                                          G1Feddrate, Fpos,Nmr);
                } else {
                  pGCodeObject->AddVertex((long)(newPoint.x * 100.0), (long)(newPoint.y * 100.0),
                                          (long)(newPoint.z * 100.0), (long)(newPoint.a * 100.0), LaserPower, 1,
                                          G1Feddrate, Fpos,Nmr);
                }
              }
            }
            Current_Pos = newPoint; // set_target(fp.x, fp.y, fp.z);
            // return StartCircularMove(ActualFeddrate,angleA,angleB,radius);
          }
          break;

          // Dwell
          case 4:
            // delay((int)search_string('P', instruction, size));
            break;

          // Inches for Units
          case 20:
            break;
          // mm for Units
          case 21:
            break;
          // go home.
          case 28:
            //            Nullen = 0;
            //            if (has_command('X', instruction, size)) Nullen |=
            //            0x03; if (has_command('Y', instruction, size))
            //            Nullen |= 0x0C; if (has_command('Z', instruction,
            //            size)) Nullen |= 0x30; RunMode= REFERENZLAUF;
            //            SetSpeed(6000);
            //            HandshakeCode='0';
            //            if (Nullen==0) Nullen =0x3F;
            //            MovingPendent=1;
            //            return;
            break;

          case 61: // G61   Exact stop check, modal   M   T   Can be canceled
          // with G64. The non-modal version is G09.
          case 62: // G62   Automatic corner override   M   T
          case 64: // G64   Default cutting mode (cancel exact stop check
            // mode)   M   T   Cancels G61.
            break;

          // Absolute Positioning
          case 90:
            abs_mode = true;
            break;

          // Incremental Positioning
          case 91:
            abs_mode = false;
            break;

          // Set Relative Home
          case 92:

            //              if(AB_Swap) {
            //                if(has_command('B', instruction, size))
            //                  Current_Pos.a = search_string('B', instruction, size);
            //              } else {
            //                if(has_command('A', instruction, size))
            //                  Current_Pos.a = search_string('A', instruction, size);
            //              }
            //              if(has_command('X', instruction, size))
            //                Current_Pos.x = search_string('X', instruction, size);
            //
            //              if(has_command('Y', instruction, size))
            //                Current_Pos.y = search_string('Y', instruction, size);
            //
            //              if(has_command('Z', instruction, size))
            //                Current_Pos.z = search_string('Z', instruction, size);

            if(has_command('A', instruction, size) || has_command('Y', instruction, size) ||
               has_command('Y', instruction, size) || has_command('Z', instruction, size)) {
              //        SetOffsetActualPos(
              //          search_string('A', instruction, size),
              //          search_string('X', instruction, size),
              //          search_string('Y', instruction, size),
              //          search_string('Z', instruction, size));
            }
//              if (has_command('A', instruction,size)){
//                 gCNCPos[Ap] = ((search_string('A', instruction, size))*100.0);
//              }
//              else if (has_command('B', instruction,size)){
//                 gCNCPos[Ap] = ((search_string('B', instruction, size))*100.0);
//              }
            StartNewObject = 1;

            break;

          default:
            printf("huh? G %d\n", code);
          }
        } else if(has_command('M', instruction, size)) { // find us an m
          // code.
          code = search_string('M', instruction, size);
          switch(code) {
          case 0:
            // todo: stop program
            break;

          case 1:
            // todo: optional stop
            break;

          case 2:
            // todo: program end
            break;

          case 3:
          // M3: Spindle On, Clockwise (CNC specific)
          //        break;
          case 4:
            // M4: Spindle On, Counter-Clockwise (CNC specific)
          {
            if(has_command('S', instruction, size)) {
              M3_4PWM = (search_string('S', instruction, size));
              LaserPower = M3_4PWM;
              bLaserMode = 1;
            } else if(has_command('P', instruction, size)) {
              M3_4PWM = (search_string('P', instruction, size)) * 255.0 / 100.0;
              // SetLaserPower(M3_4PWM);
              bLaserMode = 1;
            }
            // Spindle(M3_4PWM );
            MPWM = M3_4PWM;
          }
          break;
          case 5:
            // M5: Spindle Off (CNC specific)
            // Spindle(0);
            MPWM = 0;
            LaserPower = 0;
            break;

          case 6: // M6: Tool change
            StartNewObject = 1;
            if(has_command('T', instruction, size)) {
              Color = (search_string('T', instruction, size));
            } else {
              Color++;
            }
            break;

          case 7: // M7: Mist Coolant On (CNC specific)
          case 8: // M8: Flood Coolant On (CNC specific)
          case 9: // M9: Coolant Off (CNC specific)
            break;
          // set max extruder speed, 0-255 PWM
          case 100:
            // extruder_speed = (int)(search_string('P', instruction, size));
            break;

          // turn extruder on, forward
          case 101:
            // extruder_set_direction(1);
            // extruder_set_speed(extruder_speed);
            break;

          // turn extruder on, reverse
          case 102:
            // extruder_set_direction(0);
            // extruder_set_speed(extruder_speed);
            break;

          // turn extruder off
          case 103:
            // extruder_set_speed(0);
            break;

          // custom code for temperature control
          case 104:
            /*extruder_set_temperature((int)search_string('P', instruction,
              size));

              //warmup if we're too cold.
              while (extruder_get_temperature() < extruder_target_celsius)
              {
              extruder_manage_temperature();
              printf("T: %d\n",extruder_get_temperature());
              delay(1000);
              }
              */
            break;
          // turn fan on
          case 106:
            // extruder_set_cooler(255);
            break;

          // turn fan off
          case 107:
            // extruder_set_cooler(0);
            break;
          case 82:
            // M82: Set extruder to absolute mode
            break;
          case 109:
            // M109: Set Extruder Temperature and Wait
            break;

          // custom code for temperature reading
          case 105:
            printf("ok T:500.0 /0.0 B:0.0 /0.0\n"); // CNC Model
            //        return;
            // printf("ok T:%d\n",201);//extruder_get_temperature());
            break;
          case 114:
            printf("ok\n");
            //        SendCurrentPos();
            break;
          case 115:
            break;
          case 551: // m_Select FFF Printer Mode
            bLaserMode = 2;
            break;
          case 552: // m_Select Laser Printer Mode
            bLaserMode = 1;
            break;
          case 553: // m_Select CNC Printer Mode
            bLaserMode = 0;
            break;
          case 999:
            //         GCodeMode =0;
            break;
          case 1005:
            // printf("Firmware Version: Bastel.Inc 1.1.0\nRelease Date Jul 23
            // 1019\n");
            printf("Firmware Version: Snapmaker-Base-2.2\nRelease Date: Mar "
                   "27 2018\n");
            break;
          case 1006:
            break;
          case 10000:
            break;
          default:
            printf("Huh? M %d\n", code);
            break;
          }
        } else if(has_command('F', instruction, size)) {
          G1Feddrate = search_string('F', instruction, size);
          ActualFeddrate = G1Feddrate;
        } else if(has_command('S', instruction, size)) {
          MPWM = M3_4PWM = (search_string('S', instruction, size));
        }
        //        if ((CNCInfoCnt & 0x3F)==0)
        //        AdjustBusyDisplay(ftell(stream));
      }
    }
  }

  tDialogCtlItem* pCtlItem = gGcodeTab->pGetDlgItem(IDC_GCODEPosStart);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
    if (gNAuswahl==0)((Fl_Value_Slider*)pCtlItem->pWndObject)->value(0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->maximum(0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->minimum(Nmr);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->precision(0);

    //    //((Fl_Value_Slider*)pCtlItem->pWndObject)->labelsize(8);
    //    ((Fl_Value_Slider*)pCtlItem->pWndObject)->scrollvalue(0,10,0,10);
  }
  pCtlItem = gGcodeTab->pGetDlgItem(IDC_GCODEPosEnd);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
    if (gNAuswahl==0) ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(Nmr);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->maximum(0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->minimum(Nmr);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->precision(-1);
    //    //((Fl_Value_Slider*)pCtlItem->pWndObject)->labelsize(8);
    //    ((Fl_Value_Slider*)pCtlItem->pWndObject)->scrollvalue(0,10,0,10);
  }
  if (gNAuswahl==0) {
    char  str[60];
    sprintf(str,"EN=%d",Nmr);
    gGcodeTab->SetDlgItemText(IDC_GCODEEndTxt,str);
    sprintf(str,"SN=0");
    gGcodeTab->SetDlgItemText(IDC_GCODEStartTxt,str);
  }
#ifdef BLOCKSIMULATION
  if (gBlockTab) {
    pCtlItem = gBlockTab->pGetDlgItem(IDC_Fortschritt);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(Nmr);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->maximum(0);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->minimum(Nmr);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->precision(-1);
      //    //((Fl_Value_Slider*)pCtlItem->pWndObject)->labelsize(8);
      //    ((Fl_Value_Slider*)pCtlItem->pWndObject)->scrollvalue(0,10,0,10);
    }
  }
#endif
  //  if   (!gStopp) SetBusyStatus("Ok");
  //  else          SetBusyStatus("Abbruch");
  free(buff);
  //  fclose(stream);
#endif
  return true;
}

//-------------------------------------------------------------

CGCodeLineObject::CGCodeLineObject(CObjectManager* ObjSource,
                                   ClipperLib::cInt x,
                                   ClipperLib::cInt y,
                                   ClipperLib::cInt z,
                                   ClipperLib::cInt a,
                                   const char* lpLayer,
                                   unsigned int color)
  : CObject(ObjSource, x, y, z, lpLayer, color)
{
  Art = GCodeLinine;
  m_Path.clear(); // = (CNCPOINT *) malloc(m_ArraySize *sizeof(CNCPOINT));
  // m_ClosedLoop=0;
  m_SelectIx = -1;
  // m_Projektion=0;
  // m_Insider=NULL;
  // LastAttr=0;
  m_Konstruktion =FORM;
}

//-------------------------------------------------------------
CGCodeLineObject::~CGCodeLineObject()
{
  m_Path.clear();
  //  free (m_Path);
}

//-------------------------------------------------------------
void CGCodeLineObject::AddVertex(ClipperLib::cInt x,
                                 ClipperLib::cInt y,
                                 ClipperLib::cInt z,
                                 ClipperLib::cInt a,
                                 int S,
                                 int Gcode,
                                 int mmSec100,
                                 int FPos,
                                 int Nmr)
{
  GcodePoint p;
  // static int anzahl=0;
  //  if (m_ClosedLoop && m_Path.size()>1 && x==m_Path[0].X && y == m_Path[0].Y
  //  && z == m_Path[0].Z) {
  //    // macht ev. noralisieren falsch wenn Punkt doppelt
  //    if (LastAttr==0) return;
  //  }

  if(z < -400000) {
    z = 0;
  }

  //  if (LastAttr>100 ||LastAttr<-100) {
  //    int a= LastAttr;
  //    LastAttr=0;
  //    //anzahl++;
  //    AddBulgeVertex(x,y,z,a);
  //    LastAttr=Attr;
  //    return;
  //  }
  //  LastAttr=Attr;
  p.N   = Nmr;
  p.A = a;
  p.X = x;
  p.Y = y;
  p.Z = z;

  p.S = S;
  p.Gcode = Gcode;
  p.mmSec100 = mmSec100;
  p.FPos = FPos;
  m_Path.push_back(p);
  if(x > 10000) {
    x = 0;
  }
  if(y > 10000) {
    y = 0;
  }
  if (gPa.Maschine[eDrehachse]) {
    if(m_Xmin > 0) {
      x = -2400;
      z = -2400;
    } else {
      x = 2400;
      z = 2400;
    }
  }

  if(x < m_Xmin)
    m_Xmin = x;
  if(x > m_Xmax)
    m_Xmax = x;
  if(y < m_Ymin)
    m_Ymin = y;
  if(y > m_Ymax)
    m_Ymax = y;
  if(z < m_Zmin)
    m_Zmin = z;
  if(z > m_Zmax)
    m_Zmax = z;

  if(x < gObjectManager->MinMaxleft)
    gObjectManager->MinMaxleft = x;
  if(x > gObjectManager->MinMaxright)
    gObjectManager->MinMaxright = x;
  if(y < gObjectManager->MinMaxtop)
    gObjectManager->MinMaxtop = y;
  if(y > gObjectManager->MinMaxbottom)
    gObjectManager->MinMaxbottom = y;
  if(z < gObjectManager->MinMaxfront)
    gObjectManager->MinMaxfront = z;
  if(z > gObjectManager->MinMaxback)
    gObjectManager->MinMaxback = z;
}

//-------------------------------------------------------------
#if 0
int circVboId = 0;
int posLoc = 0;
// https://stackoverflow.com/questions/25279009/how-to-draw-a-circle-using-vbo-in-es2-0/25321141#25321141
void DrawArc()
{
  // Number of segments the circle is divided into.
  const unsigned DIV_COUNT = 32;

  if(circVboId == 0) {
    // Will use a triangle fan rooted at the origin to draw the circle. So one additional
    // point is needed for the origin, and another one because the first point is repeated
    // as the last one to close the circle.
    GLfloat* coordA = new GLfloat[(DIV_COUNT + 2) * 2];

    // Origin.
    unsigned coordIdx = 0;
    coordA[coordIdx++] = 0.0f;
    coordA[coordIdx++] = 0.0f;

    // Calculate angle increment from point to point, and its cos/sin.
    float angInc = 2.0f * M_PI / static_cast<float>(DIV_COUNT);
    float cosInc = cos(angInc);
    float sinInc = sin(angInc);

    // Start with vector (1.0f, 0.0f), ...
    coordA[coordIdx++] = 1.0f;
    coordA[coordIdx++] = 0.0f;

    // ... and then rotate it by angInc for each point.
    float xc = 1.0f;
    float yc = 0.0f;
    for(unsigned iDiv = 1; iDiv < DIV_COUNT; ++iDiv) {
      float xcNew = cosInc * xc - sinInc * yc;
      yc = sinInc * xc + cosInc * yc;
      xc = xcNew;

      coordA[coordIdx++] = xc;
      coordA[coordIdx++] = yc;
    }

    // Repeat first point as last point to close circle.
    coordA[coordIdx++] = 1.0f;
    coordA[coordIdx++] = 0.0f;

    GLuint vboId = 0;
    glGenBuffers(1, &circVboId);
    glBindBuffer(GL_ARRAY_BUFFER, circVboId);
    glBufferData(GL_ARRAY_BUFFER, (DIV_COUNT + 2) * 2 * sizeof(GLfloat), coordA, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] coordA;
  }
  glBindBuffer(GL_ARRAY_BUFFER, circVboId);
  glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posLoc);

  glDrawArrays(GL_TRIANGLE_FAN, 0, DIV_COUNT + 2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#endif
//-------------------------------------------------------------
int GcodeNmr=0;

#define FASTERSINCOS
int ElementWorkCounter = 0;
int ElementWorkEnd = 0;
//int ElementCounter = 0;
int ElementStart = 0;
int ElementEnd = 0;
void CGCodeLineObject::ExecuteDrawObject(int mode)
{
#ifndef DREITABLE
  if(m_Path.size() < 2)
    return;
  if(this == IncludeList[0].m_ObjectManager->CNCInfo[0]) {
    //ElementCounter = 0;
    ElementStart = gGcodeTab->GetDlgItemInt(IDC_GCODEPosStart, NULL, 0);
    ElementEnd   = gGcodeTab->GetDlgItemInt(IDC_GCODEPosEnd, NULL, 0) + 1;
    if((mode & Art) == Art) {
      ElementWorkCounter=0;
#ifdef BLOCKSIMULATION
      if (gBlockTab) {
        ElementWorkEnd = gBlockTab-> GetDlgItemFloat(IDC_Fortschritt) + 1;
      }
#endif
    }
  }
  int Durchmesser = gPa.Maschine[eDrehachse] ? gPa.Maschine[eDurchmesser] : 0;

  if(mode == Draw) {
    int PWM = 1255;
    int GCode = -1;
    int StartDraw = 1;
    int StartPunkt = 1;
    int ShowVertex = 0;
    int i;
    float Y_LateralLast, X_AxisPosLast, Z_RadialLast, A_RotationLast;
    float Y_LateralNeu, X_AxisPosNeu, Z_RadialNeu, A_RotationNeu;
    float xl, yl, zl;
    float radius = Durchmesser / 2; // m_Path[i].Z);
    float slices = radius / 15;
    if(slices < 0) {
      if(slices > -7)
        slices = -7;
      if(slices < -100)
        slices = -100;
    } else {
      if(slices < 7)
        slices = 7;
      if(slices > 100)
        slices = 100;
    }
    radius = glZCNCToScreen(radius); // m_Path[i].Z);
    float part = (2.0 * M_PI) / slices;
    for(i = 0; i < m_Path.size(); i++) {
      //ElementCounter++;

      // Positionen
//      if (m_Path[i].N == 502) {
//        GcodeNmr = m_Path[i].N;
//      }
      if(gDrehachse == 0) {
        X_AxisPosNeu = glXCNCToScreen(m_Path[i].X);
        Y_LateralNeu = glYCNCToScreen(m_Path[i].Y);
        Z_RadialNeu = glZCNCToScreen(m_Path[i].Z);
      } else {
        if(1) { // AB_Swap) {
          Y_LateralNeu = glYCNCToScreen(m_Path[i].Y);
          X_AxisPosNeu = glXCNCToScreen(m_Path[i].X);
        } else {
          X_AxisPosNeu = glYCNCToScreen(m_Path[i].Y);
          Y_LateralNeu = glXCNCToScreen(m_Path[i].X);
        }
        Z_RadialNeu = 0;
        A_RotationNeu = (float)m_Path[i].A / (36000.0 / (2.0 * M_PI));
        //        EnterCriticalSection(&SerialComm::s_critical_section);
        //        // Nullpunkt soll oben
        //        A_RotationNeu -= gCNCPos[Ap] / (36000.0 / (2.0 * M_PI));
        //        LeaveCriticalSection(&SerialComm::s_critical_section);
        A_RotationNeu *= -1.0;
        A_RotationNeu -= M_PI / 2;
        if(Durchmesser == 0)
          radius = glZCNCToScreen(m_Path[i].Z);
        // if(m_Path[i].Z)radius = glZCNCToScreen(m_Path[i].Z);

        float perimeter = m_Path[i].Z * (2.0 * M_PI) / 100.0;
        slices = (int)ceil(perimeter / CURVE_SECTION_MM);
        part = (2.0 * M_PI) / slices;
      }

      // Show detector
      //if(ElementCounter < ElementStart || ElementCounter > ElementEnd) {
      if(m_Path[i].N < ElementStart || m_Path[i].N > ElementEnd) {
        if(StartDraw == 0) {
          glEnd();
          StartDraw = 1;
        }
        ShowVertex = 0;
      } else {
        if(m_Path[i].Gcode != GCode) {
          GCode = m_Path[i].Gcode;
          if(StartDraw == 0) {
            glEnd();
            StartDraw = 1;
          }
        }
        ShowVertex = GCode == 1 ? 1 : !bHideRapidTrans;
      }
      // Farbe und Liniebreite
      if(ShowVertex) {
        if(StartDraw) {
          if(GCode == 0) { // m_Path[i].Gcode==0) {
            glColor4f(0.6, 0.6, 0.6, 0.7);
            glLineWidth(1);
          } else {
            glLineWidth((ETiefe == AktTiefe) ? 3 : 1);
            if(IsDone()) {
              DWORD clr = GetRGBColor();
              glColor4f((clr >> 0 & 0xFF) / 255.0, (clr >> 8 & 0xFF) / 255.0, (clr >> 16 & 0xFF) / 255.0,1.0);
            } else {
              DWORD clr = GetRGBColor();
              float transparecy = 1.0;
              PWM = m_Path[i].S;
              if(gPa.Maschine[ePowerPercent]) {
                transparecy = _max(60, _min(PWM * gPa.Maschine[ePowerPercent] / 100, 255)) / 255.0;
              }
              // glColor4f(PWM / 256.0, PWM / 256.0, 1.0 - PWM / 256.0,transparecy);
              float power = transparecy;
              glColor4f((float)(clr >> 0 & 0xFF) * power, (clr >> 8 & 0xFF) * power,
                        (float)(clr >> 16 & 0xFF) * power, transparecy);
            }
          }
          glBegin(GL_LINE_STRIP); // GL_LINES);
          if(i == 0) {
            X_AxisPosLast = X_AxisPosNeu;
            Y_LateralLast = Y_LateralNeu;
            Z_RadialLast = Z_RadialNeu;
            A_RotationLast = A_RotationNeu;
          }
          StartPunkt = 1;
          StartDraw = 0;
        }
        // Ausgabe
        float wDiff = A_RotationNeu - A_RotationLast;
        if(gDrehachse == 0) {
          if(StartPunkt) {
            glVertex3f(X_AxisPosLast, Y_LateralLast, Z_RadialLast);
            StartPunkt = 0;
          }

          xl = X_AxisPosNeu;
          yl = Y_LateralNeu;
          zl = Z_RadialNeu;
          glVertex3f(xl, yl, zl);
        } else {
          if(wDiff == 0 || radius == 0) {
            if(StartPunkt) {
              // Letzter Punkt wenn vorher verborgen
              float yc = sin(A_RotationLast) * radius - cos(A_RotationLast) *Y_LateralLast;
              float xc = cos(A_RotationLast) * radius + sin(A_RotationLast) *Y_LateralLast;
              yl = /*Y_LateralNeu +*/ xc;
              xl = X_AxisPosLast;
              zl = Z_RadialLast - yc;
              glVertex3f(xl, yl, zl);
              StartPunkt=0;
            }
            // Erster Punkt wenn kein Bogen
            float yc = sin(A_RotationNeu) * radius - cos(A_RotationNeu) *Y_LateralNeu;
            float xc = cos(A_RotationNeu) * radius + sin(A_RotationNeu) *Y_LateralNeu;
            yl = /*Y_LateralNeu +*/ xc;
            xl = X_AxisPosNeu;
            zl = Z_RadialNeu - yc;// - Durchmesser/2;
            glVertex3f(xl, yl, zl);
          } else {
            float yDiff = X_AxisPosNeu - X_AxisPosLast;
            float xDiff = Y_LateralNeu - Y_LateralLast;
            float zDiff = Z_RadialNeu - Z_RadialLast;
            int n = (int)fabs((wDiff / part)) + 1;
            if(n > 37) {
              n = 37;
              float rounds = fabs(wDiff / (M_PI*2.0));
              if (rounds>=1) n *= rounds;
            }
            wDiff = wDiff / n;
            xDiff = xDiff / n;
            yDiff = yDiff / n;
            zDiff = zDiff / n;
#ifdef FASTERSINCOS
            float cosInc = cos(wDiff);
            float sinInc = sin(wDiff);
            // StartWinkel Position
            float yc = sin(A_RotationLast) * radius;
            float xc = cos(A_RotationLast) * radius;
#endif
            slices = n;
            n++; // Ersten und letzten Punkt auch rechnen
            while(n) {

#ifdef FASTERSINCOS
#else
              float yc = sin(A_RotationLast) * radius;
              float xc = cos(A_RotationLast) * radius;
#endif

              float yln = Y_LateralLast + xc;
              float xln = X_AxisPosLast;
              float zln = Z_RadialLast - yc;// - Durchmesser/2;
              // Erster Punkt kann gleich dem vergangenem letzen Punk sein
              if(xl != xln || yl != yln || zl != zln) {
                glVertex3f(xln, yln, zln);
              }
              xl = xln;
              yl = yln;
              zl = zln;
              if(A_RotationNeu == A_RotationLast) {
                break;
              }
              Y_LateralLast += xDiff;
              X_AxisPosLast += yDiff;
              Z_RadialLast += zDiff;
              A_RotationLast += wDiff;
              n--;
#ifdef FASTERSINCOS
              float xcNew = cosInc * xc - sinInc * yc;
              yc = sinInc * xc + cosInc * yc;
              xc = xcNew;
#endif
            }
          }
        }
      }
      A_RotationLast = A_RotationNeu;
      Y_LateralLast = Y_LateralNeu;
      X_AxisPosLast = X_AxisPosNeu;
      Z_RadialLast = Z_RadialNeu;
    }
    if(StartDraw == 0) {
      glEnd();
    }
  } else if((mode & Art) == Art) {
    ClipperLib::cInt A1, X1, Y1, Z1;
    Y1 = m_Path[0].Y;
    A1 = m_Path[0].A;
    X1 = m_Path[0].X;
    Z1 = m_Path[0].Z;

    if(mode & LaserGravur)
      SpindleWarteBis(0);
    if(gDrehachse) {
      Positionier3DWarteBis(A1, X1, Y1, Z1+Durchmesser/2, (int)(m_Path[0].mmSec100 * gPa.Maschine[eGcodeRapid] / 60));
    } else {
      PositionierWarteBis(X1, Y1, Z1, (int)(m_Path[0].mmSec100 * gPa.Maschine[eGcodeRapid] / 60));
    }
    // if (AbhebenwennNoetig(X1,Y1)==0) return;
    // PositionierWarteBis(X1,Y1,Z1,(int)Pa.Tempo[Tempo_Eintauch]);
  } else
    return;

  ClipperLib::cInt X1 = 0, Y1 = 0, Z1 = 0, A1 = 0;
  if(ETiefe == AktTiefe) {
    int index = 0;
    int runden = 1;
    if(mode == Draw) {
      glBegin(GL_LINE_STRIP);
      DWORD clr = GetRGBColor();
      glColor3f((clr >> 0 & 0xFF) / 355.0, (clr >> 8 & 0xFF) / 355.0, (clr >> 16 & 0xFF) / 355.0);
    }
    while(runden) {
      if(mode == Draw) {
        if(0 && !gDrehachse) {
          float x, y, z;
          y = glYCNCToScreen(m_Path[index].Y);
          if(gDrehachse)
            x = glXCNCToScreen(m_Path[index].A);
          else
            x = glXCNCToScreen(m_Path[index].X);
          z = glZCNCToScreen(m_Path[index].Z);
          glVertex3f(x, y, z);
        }
      } else if((mode & Art) == Art) {
        Y1 = m_Path[index].Y;
        X1 = m_Path[index].X;
        Z1 = m_Path[index].Z;
        A1 = m_Path[index].A;
        int PWM = m_Path[index].S;
        if(mode & LaserGravur)
          SpindleWarteBis(PWM);
        //        if(gPa.Maschine[ePowerPercent])
        //          SpindleWarteBis(1 + _min(PWM * gPa.Maschine[ePowerPercent] / 100, 254));
        GcodeNmr = m_Path[index].N;
        if(gDrehachse) {
          Positionier3DWarteBis(
            A1, X1, Y1, Z1+Durchmesser/2, (int)(m_Path[index].mmSec100 * gPa.Maschine[eGcodeFeed] / 60));
        } else {
          PositionierWarteBis(X1, Y1, Z1, (int)(m_Path[index].mmSec100 * gPa.Maschine[eGcodeFeed] / 60));
        }
#ifdef BLOCKSIMULATION
        ElementWorkCounter++;
        if(ElementWorkCounter > ElementWorkEnd) {
          runden = 0;
        }
#endif
      }
      if(runden == 1) {
        if(index < (m_Path.size() - 1)) {
          index++;
        } else {
          runden = 0;
        }

      }
    }
    if(mode == Draw) {
      glEnd();
    }
  }
  if((mode & Art) == Art) {
    if(gPa.Maschine[ePowerPercent])
      SpindleWarteBis(1); // 1 Releais nicht schalten
    if(!gStopp && gOkCount > 0) {
      usleep(15000);
    }
    if(!gStopp)
      SetDone(1);
    while(GetAntwort() != 'H' && !gStopp)
      ;
    // PositionierWarteBis(X1,Y1,Pa.Tiefe[0],(int)(m_G0Feddrate*GcodeRapid/100));
  }
#endif
}





//-------------------------------------------------------------

//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "CncSetup.h"
#include "DxfCnc.h"
#include "GlDisplay.h"
#include "Object.h"
#include "Resource.h"
#include "Serialcom.h"
#include "Settings.h"
#include <math.h>

#ifndef WIN32

#endif
// bool  gStopp;
int SollCommand;
int SollV1=0;
int SollV2=0;
int SollV3=0;
int SollV4=0;
int SollV5=0;
int SollTempo = 1200;
#ifdef VIRTUALCNC
double VIRTUALV1 =0;
double VIRTUALV2 =0 ;
double VIRTUALV3 =4900;;
double VIRTUALV4 =0;
double VIRTUALTempo = 1200;
#endif
bool gCNCEiche = false;
bool gCNCAktiv = false;
char gCNCCode;
char gCNC_HALT;
// int         gOkCount=0;
int gOkMax = 1;
int gOkModus = 0;
int gExecuteMode = 0;

double gCNCPos[4];
extern SerialComm* gpSerialComm;

//-------------------------------------------------------------
#ifdef WIN32
DWORD exec_thread;
#else
pthread_t exec_thread;
#endif
void* ExecThreadProc(void* lpThreadParameter)
{
  fprintf(stderr,"ExecThreadProc Start\n");

  //gCNCAktiv = true;
  int Mode = (long)lpThreadParameter;
#ifdef BLOCKSIMULATION
  if (AktTiefe && AktTiefe <= 6 && gBlockSimulation && gBlockSimulation->m_rgTiefe) {
    gBlockSimulation->StartSimulation((float)gPa.WerkzeugDuchmesser[AktTiefe]/100.0,gPa.WerkzeugForm[AktTiefe]);
    gCNCPos[Ap] = ((long)fabs(gCNCPos[Ap]) % 36000);
  }
#endif
  SendToCNC('G', 90); // Absolut Mode
  WarteAufHaltAntwort();
  if(Mode == NullPunkt) {
    int diffX = abs(gPa.Ref[Xp] - gCNCPos[Xp]);
    int diffY = abs(gPa.Ref[Yp] - gCNCPos[Yp]);
    int diffZ = abs(gPa.Ref[Zp] - gCNCPos[Zp]);
    if((diffX < 3) && (diffY < 3)) {
      Positionier(0, 0, gPa.Tiefe[0], (int)gPa.Tempo[0]);
      while(GetAntwort() != 'H' && !gStopp)
        ;
    } else {
      if(gCNCPos[Zp] < 5000) {
        PositionierAbs(gCNCPos[Xp], gCNCPos[Yp], 5000, (int)gPa.Tempo[0]);
        while(GetAntwort() != 'H' && !gStopp)
          ;
      }
      Positionier(0, 0, gPa.Tiefe[0] + 1000, (int)gPa.Tempo[0]);
      while(GetAntwort() != 'H' && !gStopp)
        ;
    }
  } else if(Mode == FahreGrenzenAb) {
    CNCINT X = gObjectManager->MinMaxleft;
    CNCINT Y = gObjectManager->MinMaxtop;
    CNCINT Z = 1000;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, (int)gPa.Tempo[Tempo_Verfahr]);
    Z = 100;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, (int)gPa.Tempo[Tempo_Eintauch]);
    Y = gObjectManager->MinMaxbottom;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, gPa.Maschine[eArbeitsTemp]);
    X = gObjectManager->MinMaxright;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, gPa.Maschine[eArbeitsTemp]);
    Y = gObjectManager->MinMaxtop;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, gPa.Maschine[eArbeitsTemp]);
    X = gObjectManager->MinMaxleft;
    if(!gStopp)
      PositionierWarteBis(X, Y, Z, gPa.Maschine[eArbeitsTemp]);

  } else if(Mode == WerkzeugEinrichten) {
    int diffX = abs(gPa.WerzeugSensor[Xp] - gCNCPos[Xp]);
    int diffY = abs(gPa.WerzeugSensor[Yp] - gCNCPos[Yp]);
    int diffZ = abs(gPa.WerzeugSensor[Zp] - gCNCPos[Zp]);
    if(gCNCPos[Zp] < 8000) {
      PositionierAbs(gCNCPos[Xp], gCNCPos[Yp], 8000, (int)gPa.Tempo[0]);
      while(GetAntwort() != 'H' && !gStopp)
        ;
    }
    PositionierAbs(gPa.WerzeugSensor[Xp], gPa.WerzeugSensor[Yp], 8000, (int)gPa.Tempo[0]);
    char ch = ' ';
    do {
      ch = GetAntwort();
    } while(ch != 'H' && ch != 'W' && ch != 'N' && !gStopp);
  } else {
    WarteAufHaltAntwort();
    if(gObjectManager->m_InUse == 0) {
      // erste verwendung der Zeichnung
      // gBastelUtils.WriteProfileString("CNCFile","LastFile",gObjectManager->m_FilePfadName);
      if(prefs)
        prefs->set("LastFile", gObjectManager->m_FilePfadName);

      SaveWorkValues(gObjectManager->m_WorkPfadName);
    } else if(ParamModify) {
      // Sichere die verwendeten Parameter
      SaveWorkValues(gObjectManager->m_WorkPfadName);
    }
    if(Mode & LaserGravur) {
      SendToCNC('M', 552, 0, 0); // Laser Mode On
    }
    //    SendToCNC('V',0);
    ParamModify = 0;
    gObjectManager->m_InUse++;
    gObjectManager->Execute(Mode);
    if(Mode & LaserGravur) {
      SendToCNC('M', 553, 0, 0); // Laser Mode Off
    }
    SendToCNC('E', 0);
    SendToCNC('V', 0);
  }
#ifdef BLOCKSIMULATION
  if (gBlockSimulation && gBlockSimulation->m_RunSimulation) {
    gBlockSimulation->m_RunSimulation = 0;
    if (gObjectManager->m_InUse>0) gObjectManager->m_InUse--;
  }
#endif
  gCNCAktiv = false;
  gStopp = true;
  Show_ExecuteState();
  Sleep(50);
  fprintf(stderr,"ExecThreadProc Ende\n");
  exec_thread=0;
  return NULL;
}

//-------------------------------------------------------------
void StartExecuteThread(int Mode)
{
  gOkCount = 0;
  if(gCNCAktiv)
    return;
  gExecuteMode = Mode;
  gStopp = false;
  gOkMax = 1;
#ifdef BLOCKSIMULATION
  if (gBlockSimulation && gBlockSimulation->m_rgTiefe==NULL) {
#endif
    if(!gCNCEiche) {
      SendToCNC('R', 0, 0, 0);
      if(WarteAufHaltAntwort() == 'H')
        gCNCEiche = true;
    }
#ifdef BLOCKSIMULATION
  } else {
    gCNCEiche = true;
  }
#endif
  SendToCNC('F', 0, 0, 0);
  if(gCNCEiche) {
    int t =10;
    while (t && exec_thread!=0) {
      Sleep(50);
      t--;
    }

    gCNCAktiv = true;
#ifdef WIN32
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExecThreadProc, (void*)Mode, 0, &exec_thread);
#else
    pthread_create(&exec_thread, NULL, ExecThreadProc, (void*)(long)Mode);
#endif
  }
  Show_ExecuteState();
  return;
}
//-------------------------------------------------------------
void TestforCNCCommands()
{
  static int XPos = -1;
  static int YPos = -1;
  static int ZPos = -1;
  static int x, y;
  if(XPos != gCNCPos[Xp] || YPos != gCNCPos[Yp] || ZPos != gCNCPos[Zp]) {
    XPos = gCNCPos[Xp];
    YPos = gCNCPos[Yp];
    ZPos = gCNCPos[Zp];
    gNewSelection = 0;
  }
  static char str[100];
  static tMessage msg;

  if(1) {
    sprintf(str, "Ok%d %6.2f  y%6.2f  z%6.2f %c", gOkCount, (float)XPos / 100.0, (float)YPos / 100.0,
            (float)ZPos / 100.0, gCNCCode);
    SetStatusText(str, 2, 0);
    if(gToolAdjustTab->visible()) {
      char s[32];
      sprintf(s, "%03.2f", (float)XPos / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugX, s);
      sprintf(s, "%03.2f", (float)YPos / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugY, s);
      sprintf(s, "%03.2f", (float)ZPos / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugZ, s);
      if(gCNCCode == 'N')
        gToolAdjustTab->deactivate();
      else
        gToolAdjustTab->activate();
    }
  }
  if(gCNC_HALT == 'H') {
    gCNCEiche = true;
  }
  if(gCNCCode == 'I') {
    gCNCEiche = false;
  } else if(gCNCCode == 'x') {
    msg.Value = IDM_SetXNull;
    msg.receiver = gNullpunktTab;
    Fl::awake(PostMessage_cb, (void*)&msg);
  } else if(gCNCCode == 'y') {
    msg.Value = IDM_SetYNull;
    msg.receiver = gNullpunktTab;
    Fl::awake(PostMessage_cb, (void*)&msg);
  } else if(gCNCCode == 'z') {
    msg.Value = IDM_SetZNull;
    msg.receiver = gNullpunktTab;
    Fl::awake(PostMessage_cb, (void*)&msg);
  } else if(gCNCCode == 'W') {
    gPa.WerzeugSensor[Xp] = gCNCPos[Xp];
    gPa.WerzeugSensor[Yp] = gCNCPos[Yp];
    gPa.WerzeugSensor[Zp] = gCNCPos[Zp];
    // msg.Value   = IDC_WekzeugSet;
    // msg.receiver= gToolBox;
    // Fl::awake(PostMessage_cb,(void*)&msg);
  }
  if(gNewPos) {
    gNewPos = 0;
    extern DXF3DDisplay* gWrk3DSheet;
    if(gWrk3DSheet->visible()) {
      // gWrk3DSheet->invalidate();
      gWrk3DSheet->redraw();
    }
  }
  usleep(10000);
}
//-------------------------------------------------------------
void Exec_PauseMs(int msec)
{
  //if (gBlockSimulation==NULL || gBlockSimulation->m_RunSimulation==0) {
#ifdef BLOCKSIMULATION
  if (gSimulationModus!=eModusFast) {
#endif
    DWORD t = GetTickCount();
    do {
      usleep(10000);
      Fl::lock();
      Fl::check();
      Fl::unlock();
    } while((t + msec) > GetTickCount());
#ifdef BLOCKSIMULATION
  }
#endif
}

//-------------------------------------------------------------
bool SendToCNC(int Command, float V1, float V2, float V3, float V4, float V5)
{
  static bool NoRecursive = 0;
  if(NoRecursive == 0) {
    NoRecursive = 1;
    int res = 0;
    if(Command != 'T')
      gCNC_HALT = 0;

    static float CuA;
    static float CuX;
    static float CuY;
    static float CuZ;
    static int CuT;
    static int Laser;
    unsigned char ComBuff[64];
    memset(&ComBuff, 0, sizeof(ComBuff));
    if(Command != 'E' && Command != 't' && Command != 'T' && Command != 'P' && Command != 'D' && Command != 'C' && Command != 'K' &&
       Command != 'N' && Command != 'F' && Command != 27 && Command != 'w' && Command != 'x' && Command != 'y' &&
       Command != 'z' && Command != 'W' && Command != 'X' && Command != 'Y' && Command != 'Z') {
      SollV1 = -1;
      SollV2 = -1;
      SollV3 = -1;
      SollV4 = -1;
    }
    switch(Command) {
    case 'B':
      sprintf((char*)ComBuff, "M999\n"); // Bastel+GCode
      break;
    case 'G':
      if(V1 == 90 || V1 == 91)
        sprintf((char*)ComBuff, "G%d\n", (int)V1);
      else
        sprintf((char*)ComBuff, "M1000\n"); // GCode
      break;
    case 'F':
      sprintf((char*)ComBuff, "f\n"); // Notstop Freigabe
      break;
    case '\033':
      SollV1 = gCNCPos[Xp];
      SollV2 = gCNCPos[Yp];
      SollV3 = gCNCPos[Zp];
      sprintf((char*)ComBuff, "\033\n"); // Notstop
      if(gpSerialComm && gOnline) {
        int len = strlen((char*)&ComBuff);
        res = gpSerialComm->SendMsg(ComBuff, len);
      }
      gOkCount = 0;
      NoRecursive = 0;
      memset(&ComBuff, 0, sizeof(ComBuff));
      break;
    case 'w': {
      gOkMax = 1;
      SollV1 += V1;
      SollV2 += V2;
      CuX = V1 / 100.0;
      CuY = V2 / 100.0;
      ;
      sprintf((char*)ComBuff, "G91\nG1 X%.2f Y%.2f\n", CuX, CuY);
      gCNCCode = 0;
    }
    break;
    case 'x': {
      gOkMax = 1;
      SollV1 += V1;
      CuX = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G91\nG1 X%.2fF%d\n", CuX,6000);
      gCNCCode = 0;
    }
    break;
    case 'y': {
      gOkMax = 1;
      SollV2 += V1;
      CuY = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G91\nG1 Y%.2fF%d\n", CuY,6000);
      gCNCCode = 0;
    }
    break;
    case 'z': {
      gOkMax = 1;
      SollV3 += V1;
      CuZ = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G91\nG1 Z%.2fF%d\n", CuZ,6000);
      gCNCCode = 0;
    }
    break;
    case 'W': {
      gOkMax = 1;
      SollV1 = V1;
      SollV2 = V2;
      CuX = V1 / 100.0;
      CuY = V2 / 100.0;
      ;
      sprintf((char*)ComBuff, "G90\nG0 X%.2f Y%.2f\n", CuX, CuY);
      gCNCCode = 0;
    }
    break;
    case 'X': {
      gOkMax = 1;
      SollV1 = V1;
      CuX = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G90\nG0 X%.2f\n", CuX);
      gCNCCode = 0;
    }
    break;
    case 'Y': {
      gOkMax = 1;
      SollV2 = V1;
      CuY = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G90\nG0 Y%.2f\n", CuY);
      gCNCCode = 0;
    }
    break;
    case 'Z': {
      gOkMax = 1;
      SollV3 = V1;
      CuZ = V1 / 100.0;
      ;
      sprintf((char*)ComBuff, "G90\nG0 Z%.2f\n", CuZ);
      gCNCCode = 0;
    }
    break;
    case 'N':
      SollV3 = 10000;
      sprintf((char*)ComBuff, "G90\nG0 Z100.0\n"); // Z Abheben
      break;
    case 'L':
      if (gLaserEnable) Laser = V1;
      break;
    case 'E':
      if(V1)
        sprintf((char*)ComBuff, "M3 P%d S%d\n",(int)V1, (int)V1);
      else
        sprintf((char*)ComBuff, "M5\n");
      gSpindleOn = V1;
      //      if (gOnline && gSaugerAuto && gSettingsTab) {
      //        gSettingsTab->WindowProc(WM_COMMAND,IDM_Sauger,gSpindleOn ?1:-1);
      //      }
      break;
    case 'V': // Dust
      if(gOnline && gSaugerAuto && gSettingsTab) {
        gSettingsTab->WindowProc(WM_COMMAND, IDM_Sauger, V1); // gSpindleOn ?1:-1);
      }
      break;
    case 'M':
      // SendToCNC('M',552,0,0);
      if(V1 == 552)
        Laser = 0;
      else if(V1 == 553)
        Laser = -1;
      sprintf((char*)ComBuff, "M%d\n", (int)V1);
      break;

    case 'R':
      sprintf((char*)ComBuff, "G90\nG21\nG28 X Y Z\n");
      //sprintf((char*)ComBuff, "$H\n");
      SollTempo = 1000;
      gCNCCode = 0;
      break;
    case 'T':
      EnterCriticalSection(&SerialComm::s_critical_section);
      SollTempo = V1;
      LeaveCriticalSection(&SerialComm::s_critical_section);
      CuT = (V1 * 60) / 100; // mm / Min
      break;
    case 't':
      SollTempo = V1;
      CuT = (V1 * 60) / 100; // mm / Min
      sprintf((char*)ComBuff, "T%6d\n", (int)V1);
      if(gpSerialComm && gOnline) {
        int len = strlen((char*)&ComBuff);
        res = gpSerialComm->SendMsg(ComBuff, len);
      }
      printf("%s", (char*)&ComBuff);
      NoRecursive = 0;
      memset(&ComBuff, 0, sizeof(ComBuff));
      break;
    case 'A':
      Command = 'P';
      V1 = CuX * 100.0;
      V2 = CuY * 100.0;
    case 'D':
    case 'P':
      SollCommand = Command;
      if(SollV1 == V1 && SollV2 == V2 && SollV3 == V3 && SollV4 == V4) {
        //printf("Redundant Pos\n");
      } else {
        EnterCriticalSection(&SerialComm::s_critical_section);
#ifdef BLOCKSIMULATION
        EnterCriticalSection(&CBlockSimulation::s_critical_section);
#endif
        SollV1 = V1;
        SollV2 = V2;
        SollV3 = V3;
        SollV4 = V4;
#ifdef BLOCKSIMULATION
        LeaveCriticalSection(&CBlockSimulation::s_critical_section);
#endif
        LeaveCriticalSection(&SerialComm::s_critical_section);
        V1 /= 100.0f;
        V2 /= 100.0f;
        V3 /= 100.0f;
        V4 /= 100.0f;
        {
          float diffX = (CuX - V1);
          float diffY = (CuY - V2);
          float diffZ = (CuZ - V3);
          float diffA = (CuA - V4);
          float qsumX = pow(diffX, 2);
          float qsumY = pow(diffY, 2);
          float qsumZ = pow(diffZ, 2);
          float qsumA = pow(diffA, 2);
          float diff = sqrt(qsumX + qsumY + qsumZ + (qsumA/3.0));
          // Wieviel Vorrat Senden
          float time = diff / CuT * 60;
          if(gOkModus) {
            gOkMax = gOkModus;
          } else {
            if(diff > 1) { // Bei Strecken laeeger als 1 mm
              gOkMax = 1;
            } else {
              gOkMax = 3;
            }
          }
        }
        CuX = V1;
        CuY = V2;
        CuZ = V3;
        CuA = V4;
        if(Command == 'D') {
          if(Laser >= 0) {
            sprintf((char*)ComBuff, "G1 A%.2f X%.2f Y%.2f Z%.2f F%d S%d\n", CuA, CuX, CuY, CuZ, CuT, Laser);
          } else {
            sprintf((char*)ComBuff, "G1 A%.2f X%.2f Y%.2f Z%.2f F%d\n", CuA, CuX, CuY, CuZ, CuT);

          }
        } else {
          if(gPa.Tempo[Tempo_Verfahr] == SollTempo) {
            if(Laser >= 0) {
              sprintf((char*)ComBuff, "G0 X%.2f Y%.2f Z%.2f F%d S0\n", CuX, CuY, CuZ, CuT);
            } else {
              sprintf((char*)ComBuff, "G0 X%.2f Y%.2f Z%.2f F%d\n", CuX, CuY, CuZ, CuT);
            }
          } else {
            if(Laser >= 0) {
              sprintf((char*)ComBuff, "G1 X%.2f Y%.2f Z%.2f F%d S%d\n", CuX, CuY, CuZ, CuT, Laser);
            } else {
              sprintf((char*)ComBuff, "G1 X%.2f Y%.2f Z%.2f F%d\n", CuX, CuY, CuZ, CuT);
            }
          }
        }
        gCNCCode = 0;
#ifdef VIRTUALCNC
        VIRTUALV1 =SollV1;
        VIRTUALV2 =SollV2;
        VIRTUALV3 =SollV3;
        VIRTUALV4 =SollV4;
        VIRTUALTempo = SollTempo;
#endif
      }
      break;
    case 'C':
    case 'K':
      SollCommand = Command;
      if(SollV1 == V1 && SollV2 == V2 && SollV3 == V3 && SollV4 == V4 && SollV5 == V5) {
        printf("Redundant Pos\n");
      } else {
        SollV1 = V1;
        SollV2 = V2;
        SollV3 = V3;
        SollV4 = V4;
        SollV5 = V5;
        V1 /= 100.0f;
        V2 /= 100.0f;
        V3 /= 100.0f;
        V4 /= 100.0f;
        V5 /= 100.0f;
        if(gOkModus) {
          gOkMax = gOkModus;
        } else {
          float diffX = (CuX - V1);
          float diffY = (CuY - V2);
          float diffZ = (CuZ - V3);
          float qsumX = pow(diffX, 2);
          float qsumY = pow(diffY, 2);
          float qsumZ = pow(diffZ, 2);
          float diff = sqrt(qsumX + qsumY + qsumZ);
          // Wieviel Vorrat Senden
          float time = diff / CuT * 60;
          if(time > 1.0) {
            // if (OkMax>1) OkMax --;
            gOkMax = 1;
          } else {
            // if (OkMax<2) OkMax ++;
            gOkMax = 2;
          }
        }
        CuX = V1;
        CuY = V2;
        CuZ = V3;
        sprintf((char*)ComBuff, "G2 X%.2f Y%.2f Z%.2f I%.2f J%.2f F%d\n", CuX, CuY, CuZ, V4, V5, CuT);
        if(Command == 'C')
          ComBuff[1] = '3';
        gCNCCode = 0;
      }
      break;
    }
    if(gExecuteMode == FahreGrenzenAb) {
      gOkMax = 1;
    }
    int len = strlen((char*)&ComBuff);
    if(len > 1) {
      if(gpSerialComm && gOnline) {
        res = gpSerialComm->SendMsg(ComBuff, len);
      } else {
        //  printf(ComBuff);
      }
      gOkCount++;
      // printf("%s",(char*)&ComBuff);
      // SetStatusText((char*)&ComBuff,0,0);
      NoRecursive = 0;
      return 1;
    }
    NoRecursive = 0;
  }
  return 0;
}
//-------------------------------------------------------------
/*bool GetRelativePosition(CNCINT * pX,CNCINT*  pY,CNCINT*  pZ)
{
  CNCINT TempX,TempY,TempZ;
  if (GetXYZPosition())
  {
    if (gPa.Drehen==1)
    {
      TempX = gCNCPos[Yp]-gPa.Ref[Yp];
      TempY = gPa.Ref[Xp]-gCNCPos[Xp];
    }
    else if (gPa.Drehen==10)
    {
      TempX = gPa.Ref[Yp]-gCNCPos[Yp];
      TempY = gCNCPos[Xp]-gPa.Ref[Xp];
    }
    else
    {
      TempX = gCNCPos[Xp]-gPa.Ref[Xp];
      TempY = gCNCPos[Yp]-gPa.Ref[Yp];
    }
    TempZ = gCNCPos[Zp]-gPa.Ref[Zp];
    if (pX) *pX = TempX;
    if (pY) *pY = TempY;
    if (pZ) *pZ = TempZ;
  }
  else gStopp = true;
  return ! gStopp;
}*/
//-------------------------------------------------------------
bool AbhebenwennNoetig(CNCINT Xn, CNCINT Yn)
{
  CNCINT TempX, TempY;
  Exec_PauseMs(5);
  int w = 10;
  while(gOkCount > 1 && w > 0) {
    // ganz kurze Strecke: 2.tes 'Ok' abwarten
    Exec_PauseMs(50);
    w--;
  }
  if(gOkCount <= 1) {
    if(gPa.Drehen == 1) {
      TempX = gCNCPos[Yp] - gPa.Ref[Yp];
      TempY = gPa.Ref[Xp] - gCNCPos[Xp];
    } else if(gPa.Drehen == 10) {
      TempX = gPa.Ref[Yp] - gCNCPos[Yp];
      TempY = gCNCPos[Xp] - gPa.Ref[Xp];
    } else {
      TempX = gCNCPos[Xp] - gPa.Ref[Xp];
      TempY = gCNCPos[Yp] - gPa.Ref[Yp];
    }
    // nur abheben wenn eine andere Position angefahren wird
    // if (Xn != TempX  || Yn != TempY)
    int x = abs(Xn - TempX);
    int y = abs(Yn - TempY);
    if(x > 10 || y > 10) {
      // if (CNCPos[Zp] >= Pa.Ref[Zp])
      if(gCNCPos[Zp] <= gPa.Ref[Zp]) {
        // Z Werkzeug heben
        Positionier(TempX, TempY, gPa.Tiefe[0], (int)gPa.Tempo[Tempo_Verfahr]); // Tempo_Eintauch]);
        while(GetAntwort() != 'H' && !gStopp)
          ;
      }
      // XY anfahren
      Positionier(Xn, Yn, gPa.Tiefe[0], (int)gPa.Tempo[Tempo_Verfahr]);
      while(GetAntwort() != 'H' && !gStopp)
        ;
    }
  } else
    gStopp = true;
  return !gStopp;
}
//-------------------------------------------------------------
void Positionier(CNCINT X1, CNCINT Y1, CNCINT Z1, int Tmp)
{
  gCNC_HALT = 0;
  if(!gStopp) {
    SendToCNC('T', Tmp, 0, 0);
    Exec_PauseMs(20);
    if(gPa.Drehen == 1)
      SendToCNC('P', gPa.Ref[Xp] - Y1, X1 + gPa.Ref[Yp], gPa.Ref[Zp] + Z1);
    else if(gPa.Drehen == 10)
      SendToCNC('P', gPa.Ref[Xp] + Y1, gPa.Ref[Yp] - X1, gPa.Ref[Zp] + Z1);
    else
      SendToCNC('P', X1 + gPa.Ref[Xp], Y1 + gPa.Ref[Yp], gPa.Ref[Zp] + Z1);
  }
}
//-------------------------------------------------------------
void PositionierWarteBis(CNCINT X1, CNCINT Y1, CNCINT Z1, int Tmp)
{
  Positionier(X1, Y1, Z1, Tmp);
  while(GetAntwort() != 'H' && !gStopp)
    ;
}
//-------------------------------------------------------------
void Positionier3DWarteBis(CNCINT A1, CNCINT X1, CNCINT Y1, CNCINT Z1, int Tmp)
{
  gCNC_HALT = 0;
  if(!gStopp) {
    SendToCNC('T', Tmp, 0, 0);
    Exec_PauseMs(20);
    SendToCNC('D', X1 + gPa.Ref[Xp], Y1 + gPa.Ref[Yp], gPa.Ref[Zp] + Z1, A1);
  }
  while(GetAntwort() != 'H' && !gStopp)
    ;
}
//-------------------------------------------------------------
void PositionierBogenWarteBis(CNCINT X1, CNCINT Y1, CNCINT Z1, CNCINT I, CNCINT J, int Tmp)
{
  gCNC_HALT = 0;
  if(!gStopp) {
    SendToCNC('T', abs(Tmp), 0, 0);
    Exec_PauseMs(20);// Hier spinnts, weil zweimal gesendet wird

    /// ZTemp
    if(gPa.Drehen == 1)
      SendToCNC(Tmp > 0 ? 'K' : 'C', gPa.Ref[Xp] - Y1, X1 + gPa.Ref[Yp], gPa.Ref[Zp] + Z1, I, J);
    else if(gPa.Drehen == 10)
      SendToCNC(Tmp > 0 ? 'K' : 'C', gPa.Ref[Xp] + Y1, gPa.Ref[Yp] - X1, gPa.Ref[Zp] + Z1, I, J);
    else
      SendToCNC(Tmp > 0 ? 'K' : 'C', X1 + gPa.Ref[Xp], Y1 + gPa.Ref[Yp], gPa.Ref[Zp] + Z1, I, J);
  }
  while(GetAntwort() != 'H' && !gStopp)
    ;
}
//-------------------------------------------------------------
void SpindleWarteBis(int Tmp)
{
  if(!gStopp)
    SendToCNC('L', Tmp, 0, 0);
  // if (! gStopp) SendToCNC('E',Tmp,0,0);
  while(GetAntwort() != 'H' && !gStopp)
    ;
}
//-------------------------------------------------------------
void PositionierAbs(CNCINT X1, CNCINT Y1, CNCINT Z1, int Tmp)
{
  gCNC_HALT = 0;
  if(!gStopp) {
    SendToCNC('T', Tmp, 0, 0);
    Exec_PauseMs(20);
    SendToCNC('P', X1, Y1, Z1);
  }
}
//-------------------------------------------------------------
void CNCAbheben(void)
{
  int Zi = (int)gPa.Ref[Zp];
  if(Zi < 8500)
    Zi += 1500;
  else
    Zi = 1500;
  SendToCNC('T', gPa.Tempo[Tempo_Verfahr], 0, 0);
  SendToCNC('A', gCNCPos[Xp], gCNCPos[Yp], Zi);
}
//-------------------------------------------------------------
char GetAntwort(void)
{
  char ch;
  int mb;
#ifdef BLOCKSIMULATION
  if (gBlockSimulation && gBlockSimulation->m_RunSimulation==0) {
#endif
    if(ifEscape()) {
      gStopp = true;
      SendToCNC('\033', 0, 0, 0); // SoftSTOP);
      Abbruch = false;
    }
#ifdef BLOCKSIMULATION
  }
#endif
  gCNCCode = (gOkCount < gOkMax) ? 'H' : 'R';
  if(gCNCCode == 'H')
    gCNC_HALT = 'H';
  ch = gCNCCode;
  if(ch == 'N')
    gStopp = true;
  return ch;
}
//-------------------------------------------------------------
char WarteAufHaltAntwort(void)
{
  char ch;
  do {
    bool res = Abbruch;
    res = ifEscape();
    if(res) {
      gStopp = true;
      SendToCNC('\033', 0, 0, 0);
      Abbruch = false;
    }
    ch = gCNC_HALT;
    gCNCCode = (gOkCount == 0) ? 'H' : 'R';
    if(gCNCCode == 'H')
      gCNC_HALT = 'H';
    Exec_PauseMs(10);
  } while(ch != 'H' && !gStopp);

  return ch;
}
//-------------------------------------------------------------

//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include  "Settings.h"
#include  "BastelUtils.h"
#include  <FL/Enumerations.H>
int     gLookAtIx=0;
int     LookAtTimeOut=0;
int     SpeichernTimeOut=0;
int     gOkCount;
int     gSpindleOn=0;
int     gLaserEnable=0;
patype gPa =          {
  {100,-200,-200,-200, -200,  -20,  -20},
  {3000,250,250,250,250,250,250,150,1000 },
  {-100,200,200,200, 200,  10,  10},
  {315,65,80,100,120,15,100},         //WerkzeugDuchmesser[7]
  {1,0,0,0,1,3,1},                    //WerkzeugForm[7]
  {1,100,20,60,400, 100,100,100,1000,0,0,4000},// Maschine[12]

/*  eRaster,
  eBlockLaengeS,
  eBlockLaengeE,
  eBlockBreiteS,
  eBlockBreiteE,
  eBlockHoeheS,
  eBlockHoeheE,
  eBlockDurchmesser,
  eOberflaeche,
  eKoerper
*/
//  {10,-100,16200,-100,10100, 0,150,2500,FL_RED,FL_DARK_GREEN},// Block[10]
  {10,0,0,0,0,0,0,0,FL_RED,FL_DARK_GREEN},// Block[10]
  { 100,100,100     },
  {29300,10000,19500},
  { 100,100,100     },
  100,
  0,
  0,
/*  {0,0,0,       // Center
   10,12,200,   // Eye
   10,33,2,     // Rotation
   30,100,100,  // persp,Light,Zoom
   100,300,1000,// LightPos
   130,100,10,  // Light Color
   20,60,100}   // Back,Gray,Transp  LookAt[]
   */
  //eSpindleRel=0,eSpindlePWM,eFocusPower,eLaserPower,eLaserSpeed,eGcodeRapid,eGcodeFeed,ePowerPercent,eArbeitsTemp
};
int    gLookAt[21];
int LookAtDefault[21]={
  0,0,0,       // Center
   10,10,150,   // Eye
   -67,0,26,     // Rotation
   30,100,100,  // persp,Light,Zoom
   150,150,250, // LightPos
   130,100,10,  // Light Color
   16,0,100}   // Back,Gray,Transp  LookAt[]
;

const patype PaMax = {{500,0,   0,   0,   0,   0,   0},{6000,2000,2000,2000,2000,2000,2000,2000,2000},{0,1500,1500,1500,1500,1500,1500},
{1000,1000,1000,1000,1000,1000,1000},       //WerkzeugDuchmesser[7]
{3,3,3,3,3,3,3},                             //WerkzeugForm[7]
{22300,20000,9500 },{22300,20000,9500},{22300,20000,9500 },200,1,1,
};
const patype PaMin = {{0,-1500,-1500,-1500,-1500,-1500,-1500},{ 21, 21, 21, 21, 21, 21, 21,  21,  21},{-500,0,   0,   0,   0,   0,   0},
{5,5,5,5,5,5,5},                   //WerkzeugDuchmesser[7]
{0,0,0,0,0,0,0},                    //WerkzeugForm[7]
{100,100,100      },{   10,  10, 10 },{100,100,100      },10,0,0,
};

int Power[5] = {24,24,22,31,33};

int	   Korektur;
const char  InitKey[NUM_INITPARAMS][2][16] =
{
  {"Tiefe","Heben"},
  {"Tiefe","Tiefe1"},
  {"Tiefe","Tiefe2"},
  {"Tiefe","Tiefe3"},
  {"Tiefe","Tiefe4"},
  {"Tiefe","Tiefe5"},
  {"Tiefe","Tiefe6"},
  {"Tempo","Verfahr"},
  {"Tempo","Arbeits1"},
  {"Tempo","Arbeits2"},
  {"Tempo","Arbeits3"},
  {"Tempo","Arbeits4"},
  {"Tempo","Arbeits5"},
  {"Tempo","Arbeits6"},
  {"Tempo","Eintauch"},
  {"Tempo","Bohren"},
  {"Stufen","Stufe0"},
  {"Stufen","Stufe1"},
  {"Stufen","Stufe2"},
  {"Stufen","Stufe3"},
  {"Stufen","Stufe4"},
  {"Stufen","Stufe5"},
  {"Stufen","Stufe6"},
  {"Werkzeug","Duchmesser0"},
  {"Werkzeug","Duchmesser1"},
  {"Werkzeug","Duchmesser2"},
  {"Werkzeug","Duchmesser3"},
  {"Werkzeug","Duchmesser4"},
  {"Werkzeug","Duchmesser5"},
  {"Werkzeug","Duchmesser6"},
  {"Werkzeug","Form0"},
  {"Werkzeug","Form1"},
  {"Werkzeug","Form2"},
  {"Werkzeug","Form3"},
  {"Werkzeug","Form4"},
  {"Werkzeug","Form5"},
  {"Werkzeug","Form6"},
  {"Maschine","SpindleRel"},
  {"Maschine","SpindlePWM"},
  {"Maschine","FocusPower"},
  {"Maschine","LaserPower"},
  {"Maschine","LaserSpeed"},
  {"Maschine","GcodeRapid"},
  {"Maschine","GcodeFeed"},
  {"Maschine","PowerPercent"},
  {"Maschine","ArbeitsTemp"},
  {"Maschine","Drehachse"},
  {"Maschine","XYSwap"},
  {"Maschine","Durchmesser"},
  {"Block","Raster"},
  {"Block","BlockLaengeS"},
  {"Block","BlockLaengeE"},
  {"Block","BlockBreiteS"},
  {"Block","BlockBreiteE"},
  {"Block","BlockHoeheS"},
  {"Block","BlockHoeheE"},
  {"Block","BlockDurchmes"},
  {"Block","OberflaecheCl"},
  {"Block","KoerperColor"},
  {"Nullpunkt_X"},
  {"Nullpunkt_Y"},
  {"Nullpunkt_Z"},
  {"Anschlag_X"},
  {"Anschlag_Y"},
  {"Anschlag_Z"},
  {"WerzeugSensor_X"},
  {"WerzeugSensor_Y"},
  {"WerzeugSensor_Z"},
  {"Inc_pro_mm"},
  {"Drehen"},
  {"Aufraumen"}
};
const char  LookAtInitKey[21][2][16] = {

  {"LookAt0","CenterX"},
  {"LookAt0","CenterY"},
  {"LookAt0","CenterZ"},
  {"LookAt0","EyeX"},
  {"LookAt0","EyeY"},
  {"LookAt0","EyeZ"},
  {"LookAt0","RotationX"},
  {"LookAt0","RotationY"},
  {"LookAt0","RotationZ"},
  {"LookAt0","Perspective"},
  {"LookAt0","Light"},
  {"LookAt0","Zoom"},
  {"LookAt0","LightPosX"},
  {"LookAt0","LightPosY"},
  {"LookAt0","LightPosZ"},
  {"LookAt0","LightRed"},
  {"LookAt0","LightGreen"},
  {"LookAt0","LightBlue"},
  {"LookAt0","Background"},
  {"LookAt0","GrayOut"},
  {"LookAt0","AlphaTransp"}
};
//-------------------------------------------------------------
//-------------------------------------------------------------
void InitSpeichern(void)
{
  LookAtTimeOut = 100;

  for (int i=0; i < NUM_LOCALPARAMS; i++)
  {
    char s[20];
    sprintf(s,"%d",(int)gPa.Tiefe[i]);
    gBastelUtils.WriteProfileString(InitKey[i][0],InitKey[i][1],s);
  }
  if (prefs) {
    for (int ix=NUM_LOCALPARAMS;ix < NUM_INITPARAMS; ix++) {
      prefs->set(InitKey[ix][0],gPa.Tiefe[ix]);

    }
  }
  if (gLookAtIx>=0 && gLookAtIx<4)LookAtSpeichern(gLookAtIx);
  gBastelUtils.SaveIniFile();
//  int num = Fl::screen_num(100,100);
//  gBastelUtils.WriteProfileInt("Display","Fullscreen",MainWindow->fullscreen_active ()?1:0);
}

//-------------------------------------------------------------
void SaveWorkValues(char const * path){
   gBastelUtils.OverwriteIniFileNmae(path);
   InitSpeichern();

/* // ueberschreibt ganzes File
  FILE *  ftemp     = fopen(path, "wt");
  if (ftemp!= NULL) {
    const char * MainKey=NULL;
    for (int i=0; i < NUM_INITPARAMS; i++)
    {
      char str[256];
      if (MainKey==NULL || strcmp(MainKey,InitKey[i][0])) {
        MainKey = InitKey[i][0];
        fprintf (ftemp ,"[%s]\n",InitKey[i][0]);
      }
      fprintf (ftemp,"%s=%d\n",InitKey[i][1],(int)gPa.Tiefe[i]);
    }
    fclose (ftemp);
  }*/
}
//-------------------------------------------------------------
/*void LoadAllWorkValues(char * path){
  FILE *  ftemp     = fopen(path, "rt");
  if (ftemp!= NULL) {
    char MainKey[64];
    char buf[_MAX_PATH *2];
    int cont=0;
    while (fgets (buf, sizeof (buf),ftemp)) {
      if (buf[0] == '[') {
        char * end = strrchr(buf,']');
        if (end) {
          * end = '\0';
          strcpy(MainKey,&buf[1]);
        }
      } else {
        char * val = strrchr(buf,'=');
        if (val) {
          *val = '\0';
          val++;
          for (int i=cont; i < NUM_INITPARAMS-18; i++)
          {
            if (strcmp(InitKey[i][0],MainKey)==0) {
              if (strcmp(InitKey[i][1],buf)==0) {
                gPa.Tiefe[i]=atol(val);
                if (i==cont+1) cont = i;
                break;
              }
            }
          }
        }
      }
    }
    fclose (ftemp);
  }
}*/
//-------------------------------------------------------------
void LookAtSpeichern(int num)
{
  LookAtTimeOut = 100;
  gLookAt[0] = (int)(center[0]    * 100.0);
  gLookAt[1] = (int)(center[1]    * 100.0);
  gLookAt[2] = (int)(center[2]    * 100.0);
  gLookAt[3] = (int)(eye[0]       * 100.0);
  gLookAt[4] = (int)(eye[1]       * 100.0);
  gLookAt[5] = (int)(eye[2]       * 100.0);
  gLookAt[6] = (int)RotateVal[0];
  gLookAt[7] = (int)RotateVal[1];
  gLookAt[8] = (int)RotateVal[2];
  gLookAt[9] = (int)Perspective;
  gLookAt[10] = (int)LightColor[1]*100.0;
  gLookAt[11] = (int)(Zoom*100.0);
  gLookAt[12] = (int)(LightPos[0] * 100.0);
  gLookAt[13] = (int)(LightPos[1] * 100.0);
  gLookAt[14] = (int)(LightPos[2] * 100.0);
  gLookAt[15] = (int)(LightColor[0] * 100.0);
  gLookAt[16] = (int)(LightColor[1] * 100.0);
  gLookAt[17] = (int)(LightColor[2] * 100.0);
  gLookAt[18] = (int)(gBackground * 100.0);
  gLookAt[19] = (int)(gGrayOut    * 100.0);
  gLookAt[20] = (int)(gAlphaTransp* 100.0);
  gBastelUtils.OverwriteIniFileNmae(gGetIniName());
  struct stat statbuf;
  if (stat(gBastelUtils.m_IniFile, &statbuf)!=0) {
    FILE * f_IniFile = fopen(gGetIniName(), "wt");
    if(f_IniFile) {
      fclose(f_IniFile);
    }
  }

  char k[20];
  sprintf(k,"LookAt%d",num);
  for (int i=0; i < ((sizeof(gLookAt) / sizeof(gLookAt[0]))); i++) {
    char s[20];
    sprintf(s,"%d",(int)gLookAt[i]);
    gBastelUtils.WriteProfileString(k,LookAtInitKey[i][1],s);
  }
  gBastelUtils.WriteProfileInt("LookAt","LookAtIndex",gLookAtIx);
  gBastelUtils.SaveIniFile();
}

//-------------------------------------------------------------
void LookAtLaden(int num)
{
  char s[240],s2[240];
  char k[20];
  if (num>=0) {
  gBastelUtils.ExternModify();
  LookAtTimeOut = 2;
  sprintf(k,"LookAt%d",num);
  for (int ix=0; ix < ((sizeof(gLookAt) / sizeof(gLookAt[0]))); ix++) {
    sprintf(s2,"%d",(int)gLookAt[ix]);
    gBastelUtils.GetProfileString(k,LookAtInitKey[ix][1],s2,s,sizeof(s));
    gLookAt[ix] = atol (s);
  }
  RotateVal[0]=  (float)gLookAt[6];
  RotateVal[1]=  (float)gLookAt[7];
  RotateVal[2]=  (float)gLookAt[8];
} else {
  memcpy(&gLookAt,&LookAtDefault,sizeof(gLookAt));
}
  center[0]   =  (float)gLookAt[0]/100.0;
  center[1]   =  (float)gLookAt[1]/100.0;
  center[2]   =  (float)gLookAt[2]/100.0;
  eye[0]      =  (float)gLookAt[3]/100.0;
  eye[1]      =  (float)gLookAt[4]/100.0;
  eye[2]      =  (float)gLookAt[5]/100.0;
  RotateValSoll[0]=(float)gLookAt[6];
  RotateValSoll[1]=(float)gLookAt[7];
  RotateValSoll[2]=(float)gLookAt[8];
  Perspective =  (float)gLookAt[9];
  //LightColor[0]= (float)gLookAt[10]/100.0;
  //LightColor[1]= (float)gLookAt[10]/100.0;
  //LightColor[2]= (float)gLookAt[10]/100.0;
  Zoom        =  (float)gLookAt[11]/100.0;
  LightPos[0] =  (float)gLookAt[12]/100.0;
  LightPos[1] =  (float)gLookAt[13]/100.0;
  LightPos[2] =  (float)gLookAt[14]/100.0;
  LightColor[0]= (float)gLookAt[15]/100.0;
  LightColor[1]= (float)gLookAt[16]/100.0;
  LightColor[2]= (float)gLookAt[17]/100.0;
  gBackground =  (float)gLookAt[18]/100.0;
  gGrayOut    =  (float)gLookAt[19]/100.0;
  gAlphaTransp=  (float)gLookAt[20]/100.0;
  Deep=0;
  gLookClip=0;

  glDisplay_redraw=1;
}

//-------------------------------------------------------------
void LoadWorkValues(const char * WorkFileName)
{
  struct stat statbuf;
  if (stat(WorkFileName, &statbuf)==0) {
    gBastelUtils.OverwriteIniFileNmae(WorkFileName);
  } else {
    extern int gIselRs232Mode;
#ifndef DREITABLE
    gBastelUtils.SetIniFile(gIselRs232Mode?"IselDxf.ini":"GoCnc.ini");
#else
    gBastelUtils.SetIniFile("DXF3Table.ini");
#endif
  }
  //LoadAllWorkValues(gBastelUtils.m_IniFile);

  char s[240],s2[240];
  int * Werte = (int *)&gPa;

  //for (int ix=0;ix < NUM_INITPARAMS-18; ix++)
  for (int ix=0;ix < NUM_LOCALPARAMS; ix++)
  {
    sprintf(s2,"%d",(int)Werte[ix]);
    char str[200];
    strcpy(str,InitKey[ix][0]);
    strcat(str,"-");
    strcat(str,InitKey[ix][1]);
    gBastelUtils.GetProfileString(InitKey[ix][0],InitKey[ix][1],s2,s,sizeof(s));
//strcpy(s,s2);
    Werte[ix] = atol (s);
  }

  if (prefs) {
    for (int ix=NUM_LOCALPARAMS;ix < NUM_INITPARAMS; ix++) {
      prefs->get(InitKey[ix][0],gPa.Tiefe[ix],gPa.Tiefe[ix]);
    }
  }


  gLookAtIx = gBastelUtils.GetProfileInt("LookAt","LookAtIndex",gLookAtIx);
  LookAtLaden(gLookAtIx);
}

//-------------------------------------------------------------
#ifdef DREITABLE
// Dummy funktonen wegen DexfCNC
bool AbhebenwennNoetig(CNCINT Xn,CNCINT Yn){return 1;}
//-------------------------------------------------------------
void Positionier(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp){}
//-------------------------------------------------------------
void PositionierWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp ) {}
//-------------------------------------------------------------
void PositionierBogenWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,CNCINT I,CNCINT J,int Tmp){}
//-------------------------------------------------------------
void SpindleWarteBis(int Tmp){}
//-------------------------------------------------------------
void PositionierAbs(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp){}
//-------------------------------------------------------------
void CNCAbheben(void){}
//----------------------------------------------- --------------
char WarteAufHaltAntwort(void){return  'H';}
//-------------------------------------------------------------
bool SendToCNC(int Command,float V1,float V2,float V3,float V4,float V5){return 1;};
//-------------------------------------------------------------
char   GetAntwort(void){return ' ';};
//-------------------------------------------------------------
#endif

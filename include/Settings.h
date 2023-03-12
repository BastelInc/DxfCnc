#ifndef __Settings_h__
#define __Settings_h__
#include <FL/Fl_Preferences.H>

extern float Zoom;
extern float Perspective;
extern float Deep;
extern float LightPos[3];
extern float LightColor[3];
extern float eye[3];
extern float center[3];
extern float RotateVal [3];
extern float RotateValSoll[3];
extern float gBackground;
extern int   gLookClip;
extern int   glDisplay_redraw;
extern float gGrayOut;
extern float gAlphaTransp;
extern int   gDreiDFace;
extern float fxAufloesung;
extern int   ParamModify;
extern Fl_Preferences  * prefs;

//-------------------------------------------------------------
typedef long CNCINT;

extern int   LookAtTimeOut;
extern int   SpeichernTimeOut;
extern const char * gGetIniName();
extern int   gLookAtIx;
void LookAtSpeichern(int num);
void LookAtLaden(int num);
void LoadWorkValues(const char * WorkFileName);
void InitSpeichern(void);
void SaveWorkValues(const char * path);

#define NUM_INITPARAMS  71  //84
#define NUM_LOCALPARAMS NUM_INITPARAMS - 12
typedef struct
{
  int  Tiefe[7],Tempo[9],Stufe[7],
  WerkzeugDuchmesser[7],WerkzeugForm[7],
  Maschine[12],Block[10],
  Ref[3],Anschlag[3],WerzeugSensor[3],
  Aufloesung,Drehen,Aufraumen;
} patype;
extern  int    gLookAt[21];
enum {
  eSpindleRel=0,eSpindlePWM,eFocusPower,eLaserPower,eLaserSpeed,eGcodeRapid,eGcodeFeed,
  ePowerPercent,eArbeitsTemp,eDrehachse,eXYSwap,eDurchmesser
};
enum {
  eRaster,
  eBlockLaengeS,
  eBlockLaengeE,
  eBlockBreiteS,
  eBlockBreiteE,
  eBlockHoeheS,
  eBlockHoeheE,
  eBlockDurchmesser,
  eOberflaeche,
  eKoerper
};
extern patype gPa;
extern const patype PaMax;
extern const patype PaMin;
extern int Power[5];


//-------------------------------------------------------------
extern int gOkCount;
extern int gSpindleOn;
extern int gLaserEnable;
extern unsigned int gFarbe256[256];

/*
#define  Draw        0
#define  Linie	     1
#define  Kreisbogen  4
#define  Buchstaben  8
#define  Punkt       16
#define  DreiDFace   32
#define  GCodeLinine 64
#define  LaserGravur 128
#define  Weggemacht 256
#define  NullPunkt  512
#define  WerkzeugEinrichten  1024
#define  FahreGrenzenAb  2048
*/
#define  Tiefe6 6

#define  Xp    0
#define  Yp    1
#define  Zp    2
#define  Ap    3
#define  Tempo_Verfahr  0
#define  Tmpo_Arbeits1  1
#define  Tempo_Eintauch 7
#define  Tempo_Bohren   8

extern bool   SendToCNC(int Command,float V1=0,float V2=0,float V3=0,float V4=0,float V5=0);
extern char   GetAntwort(void);
//extern bool   GetRelativePosition(CNCINT * pX,CNCINT*  pY,CNCINT*  pZ);
extern bool   AbhebenwennNoetig(CNCINT Xn,CNCINT Yn);
extern bool   TestPosition(CNCINT X1,CNCINT Y1,CNCINT Z1);

//extern bool	  GetXYZPosition(void);
extern void   Positionier(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
extern void   PositionierWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
extern void   Positionier3DWarteBis(CNCINT A1,CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp );
extern void   PositionierBogenWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,CNCINT I,CNCINT J,int Tmp);
extern void   PositionierAbs(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
extern void   SpindleWarteBis(int Tmp);
extern void   TestforCNCCommands();
extern void   StartExecuteThread(int Mode);
extern void   Show_ExecuteState(void);

extern char   WarteAufHaltAntwort(void);

#define ErsterBuchstabe   32
#define LetzterBuchstabe  132
typedef struct
{
  unsigned char x;
  unsigned char y;
} bvector;
typedef struct
{
  unsigned char Ofs;
  unsigned char Br;
  unsigned char Si;
} Bkorrekt;
extern  Bkorrekt Korrektur[101];
extern  bvector * Zeichen[101];
extern  void SettingsShow(int show);
extern  void ShowTab(int key);
extern  void SetStatusText(const char * pnt,int num,int style);

//-------------------------------------------------------------



#endif

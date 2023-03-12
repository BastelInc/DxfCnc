#if !defined(AFX_DXFCNCSETUP_H__ACE6FB5C_CE3A_4C96_97BB_586024AECFBC__INCLUDED_)
#define AFX_DXFCNCSETUP_H__ACE6FB5C_CE3A_4C96_97BB_586024AECFBC__INCLUDED_

#include "FltkWindow.h"
#include "FltkDialogBox.h"
#include "FingerScroll.h"
#include "Layers.h"
#include <FL/Fl_Choice.H>

extern const tDialogCtlItem rg_ToolBox  [];
void ShowTab(int id);


class BastelTabs : public Fl_Group {
public:
  Fl_Widget* value() {
    Fl_Widget* v = 0;
    Fl_Widget*const* a = array();
    for (int i=children(); i--;) {
      Fl_Widget* o = *a++;
      if (v) o->hide();
      else if (o->visible()) v = o;
      else if (!i) {o->show(); v = o;}
    }
    return v;
  };

  int value(Fl_Widget *newvalue) {
    Fl_Widget*const* a = array();
    int ret = 0;
    for (int i=children(); i--;) {
      Fl_Widget* o = *a++;
      if (o == newvalue) {
        if (!o->visible()) ret = 1;
        o->show();
      } else {
        o->hide();
      }
    }
    return ret;
  };
  int handle(int e){
  //fprintf(stderr,"handle %d\n",e);
    if (e==FL_PUSH) {
      focus(value());
    }
    return Fl_Group ::handle(e);
  }

  BastelTabs(int X,int Y,int W, int H, const char *l):Fl_Group(X,Y,W,H,l){;};
  ~BastelTabs(){;};
};




class CToolBox : public CFltkDialogBox
{
public:
  CToolBox(int X, int Y, int W, int H, const char *L);
  ~CToolBox();

public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  BOOL SetSelectionColor(int nIDDlgItem, int iColor);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};


class COptionsBox : public CFltkDialogBox
{
public:
  COptionsBox(int X, int Y, int W, int H, const char *L);
  ~COptionsBox();

public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  BOOL SetSelectionColor(int nIDDlgItem, int iColor);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};


class CDxfLayer: public CFltkDialogBox
{
public:
  CDxfLayer(int X, int Y, int W, int H, const char *L);
  ~CDxfLayer();
public:
  //CFingerScroll  *  m_FingerScroll;
  //bool virtual ShowLayerButtons(int neu);
  virtual int  WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int  InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void Show(void);
};

class CDxfColorTab: public CFltkDialogBox
{
public:
  CDxfColorTab(int X, int Y, int W, int H, const char *L);
  ~CDxfColorTab();
public:
  Fl_Choice    * m_LayerAuswahl;
  Fl_Menu_Item * p_LayerListe;
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CCalculatorTab: public CFltkDialogBox
{
public:
  CCalculatorTab(int X, int Y, int W, int H, const char *L);
  ~CCalculatorTab();
public:
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CDXFtoCNCTab: public CFltkDialogBox
{
public:
  CDXFtoCNCTab(int X, int Y, int W, int H, const char *L);
  ~CDXFtoCNCTab();
public:
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};


class CNullpunktTab: public CFltkDialogBox
{
public:
  CNullpunktTab(int X, int Y, int W, int H, const char *L);
  ~CNullpunktTab();
public:
  void        FlashGreen(int id);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CToolAdjustTab: public CFltkDialogBox
{
public:
  CToolAdjustTab(int X, int Y, int W, int H, const char *L);
  ~CToolAdjustTab();
public:
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CSettingsTab: public CFltkDialogBox
{
public:
  CSettingsTab(int X, int Y, int W, int H, const char *L);
  ~CSettingsTab();
public:
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CLaserTab: public CFltkDialogBox
{
public:
  CLaserTab(int X, int Y, int W, int H, const char *L);
  ~CLaserTab();
public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CGcodeTab: public CFltkDialogBox
{
public:
  CGcodeTab(int X, int Y, int W, int H, const char *L);
  ~CGcodeTab();
public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CPerspektive: public CFltkDialogBox
{
public:
  CPerspektive(int X, int Y, int W, int H, const char *L);
  ~CPerspektive();
public:
  BOOL SetSelectionColor(int nIDDlgItem, int iColor);
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

class CDXFOptimize: public CFltkDialogBox
{
public:
  CDXFOptimize(int X, int Y, int W, int H, const char *L);
  ~CDXFOptimize();
public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

#define M_2PI  6.28318530718
#ifndef M_PI
#define M_PI   3.14159265359
#define M_PI_2 1.57079632680
#define M_PI_4 0.78539816340f
#endif
#define M_PI_8 0.39269908170f
#define M_PI_6 0.523598776f
#define GRAD360 57.2957795131f
#define CBWNDEXTRA 4

#define CNC

extern int   gStopp;
extern float  fxAufloesung;
extern void   CloseComm(void);
extern void   CNCAbheben(void);
extern bool   gCNCAktiv;
extern int    gXPos;
extern int    gYPos;
extern int    gZPos;
extern int    gNewPos;
extern int    gOkCount;
extern int    gOkModus;
extern BOOL   Abbruch;
extern int    AktTiefe;
extern  RECT  Auswahl;
extern  bool  Zoll;
extern  char  LeiterLayer[30];
extern  char  BohrenLayer[30];
//#define MAXNUMLAYER 32
//extern char   gszLayer[MAXNUMLAYER][120];
extern int    gLayerAnz;
extern int    AktLayer;
extern LAYERTYPE WorkLayer;
extern  int   Auge[18];
extern  int   DCodes[18];
extern  int   Bohrers[8];

/*extern  int   LaserPower;
extern  int   LaserSpeed;
extern  int   GcodeRapid;
extern  int   GcodeFeed;
extern  int   PowerPercent;
extern  int   ArbeitsTemp;
*/
extern  int   gSpindleOn;

extern  int   gExecuteMode;
#define FixFang 16
#define Fraserb 17
//extern int Fraeserbreite;
/*
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
*/
union MarkeT
{
  long int l;
  POINTS p;
};

typedef long CNCINT;

typedef struct
{
  CNCINT x;
  CNCINT y;
  CNCINT z;
  int  Attr;
} CNCPOINT;

#define  Sel                 1
#define  Active              2
#define  UnSel	             0
#define  Done                8
#define  DisplayUpdate    0x10
#define  DisplayTest      0x20

/*typedef struct
{
  CNCPOINT  S;
  union
  {
    CNCPOINT  E;
    LPSTR Ps;
  };
  int Radius;
  char ETiefe;
  char Art;
  char Select;
  char Layer;
} CNCObjT;
typedef CNCObjT * CNCObjP;
*/

extern double	gCNCPos[4];
/*
typedef struct
{
  int SIndex;
  char Vertauschen;
//	char Abheben;
  char Gefunden;
} CNCSortT;
*/
//#define  AnzahlCNC   50000

//typedef CNCObjP  CNCInfoArrT[AnzahlCNC];
//typedef CNCSortT CNCSortArrT[AnzahlCNC];


//extern DWORD gFarbe256[256];
//
//#define  Draw        0
//#define  Linie	     1
//#define  Kreisbogen  4
//#define  Buchstaben  8
//#define  Punkt       16
//#define  DreiDFace   32
//#define  GCodeLinine 64
//#define  LaserGravur 128
//#define  Weggemacht 256
//#define  NullPunkt  512
//#define  WerkzeugEinrichten  1024
//#define  FahreGrenzenAb  2048
//#define  Tiefe6 6
//
//#define  Xp    0
//#define  Yp    1
//#define  Zp    2
//#define  Tempo_Verfahr  0
//#define  Tmpo_Arbeits1  1
//#define  Tempo_Eintauch 7
//#define  Tempo_Bohren   8

extern bool	  gCNCEiche;
extern char	  gCNCCode;
extern int    gNewSelection;
//extern patype gPa;
//extern void   InitLaden(void);
//extern void   SaveWorkValues(LPCSTR path);
//extern void   LoadWorkValues(LPCSTR path);
extern BOOL   ifEscape(void);
//extern void	  InitSpeichern(void);
//extern void   LookAtSpeichern(int num);
//extern void   LookAtLaden(int num);

//extern bool   SendToCNC(int Command,float V1=0,float V2=0,float V3=0,float V4=0,float V5=0);
//extern char   GetAntwort(void);
////extern bool   GetRelativePosition(CNCINT * pX,CNCINT*  pY,CNCINT*  pZ);
//extern bool   AbhebenwennNoetig(CNCINT Xn,CNCINT Yn);
//extern bool   TestPosition(CNCINT X1,CNCINT Y1,CNCINT Z1);
//
////extern bool	  GetXYZPosition(void);
//extern void   Positionier(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
//extern void   PositionierWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
//extern void   PositionierBogenWarteBis(CNCINT X1,CNCINT Y1,CNCINT Z1,CNCINT I,CNCINT J,int Tmp);
//extern void   PositionierAbs(CNCINT X1,CNCINT Y1,CNCINT Z1,int Tmp);
//extern void   SpindleWarteBis(int Tmp);
//extern void   TestforCNCCommands();
//extern void   StartExecuteThread(int Mode);
//extern void   Show_ExecuteState(void);
//
//extern char   WarteAufHaltAntwort(void);
//extern void   DXFInitLaden(void);
//extern void   DXFExtraSpeichern(void);
extern void ResetScanStatus(void);
extern void IncrementFileScanStatus(void);
extern void IncrementDirScanStatus(void);


#endif

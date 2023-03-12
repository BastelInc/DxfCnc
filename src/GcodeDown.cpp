#include "CVisual_Pad.h"
#include "CncSetup.h"
#include "FileFolder.h"
#include "FingerScroll.h"
#include "FltkWindow.h"
#include "GCodeDown.h"
#include "GlDisplay.h"
#include "Object.h"
#include "Resource.h"
#include "Serialcom.h"
#include "BastelUtils.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#if LONMSG
#include "LonMessage.h"
#endif
#include "CommSnapMaker.h"
#include "ProgressStatus.h"

#define ROLF_DEBUG
#undef TOOLSCROLL

ReseorceInit* R;
Fl_Preferences* prefs = NULL;
Fl_Double_Window* MainWindow;
Fl_Button* FullScreen;
Fl_Button* Exit;
Fl_Button* Scheme;
CToolBox* gToolBox = NULL;
COptionsBox* gOptionsBox = NULL;
// Fl_Tabs         * SettingsTabs=0;
BastelTabs* SettingsTabs = 0;
CDxfLayer* gDxfLayer = NULL;
CDxfColorTab* gDxfColorTab = NULL;
CCalculatorTab* gCalculatorTab = NULL;
CNullpunktTab* gNullpunktTab = NULL;
CToolAdjustTab* gToolAdjustTab = NULL;
CSettingsTab* gSettingsTab = NULL;
CLaserTab* gLaserTab = NULL;
CGcodeTab* gGcodeTab = NULL;
CPerspektive* gPerspektive = NULL;
CDXFOptimize* gDXFOptimize = NULL;
CDXFtoCNCTab* gDXFtoCNCTab = NULL;

#ifdef NATIVE_FILECHOOSER
Fl_Input* gFileExplorer = NULL;
#else
CTreeView* gFileExplorer;
#endif
CTreeView* pDXFTree;
CTVItem* pTreeitem_found = NULL;
CObjectManager* gObjectManager = 0;
DXF3DDisplay* gWrk3DSheet = 0;
Fl_Window* gEditemain = 0;
Fl_Box* gStatus1 = 0;
Fl_Box* gStatus2 = 0;
int StatusUpdate;
int SerialInitDelay = 0;
Fl_Progress* BusyStatus = 0;
extern Fl_Window* Editemain(int X, int Y, int W, int H, const char* L);
SerialComm* gpSerialComm = NULL;
#if LONMSG
CUDP_Comm* gLonService = NULL;
#endif
char evt_txt[256] = { 0 };

int gIselRs232Mode = 0;
int gOnline = 0;
int gSaugerAuto = 0;
int ScanFileTree = -1;
int gDebugMode = 0;
int gAppRun = 1;
int gbModify = 0;
int xTool = 256;
int yTool = 486;
int yStatus = 22;
int yTab = 150;
int MainPosX = 100;
int MainPosY = 17;
int Mainwidth = 1000;
int Mainheight = 18 * 45;
int xPerspektive = 84;
#ifdef WIN32
#include <DBT.h>
// extern WPARAM ComRemovewParam;
// extern LPARAM ComRemovelParam;
#else
#include <signal.h>
#endif
//-------------------------------------------------------------
//-------------------------------------------------------------
void* IniPortProc()
{
  char str[30];
  // gBastelUtils.GetProfileString("Profiler","Comm","COM9",str,sizeof(str));
  prefs->get("Comm", str, "COM9", sizeof(str));
  if(gpSerialComm)
    delete gpSerialComm;
  gpSerialComm = new SerialComm(str);
  if(gpSerialComm) {
    gToolBox->SetButtonImage(IDC_OnLine, imOnline);
    gToolBox->SetDlgItemText(IDC_OnLine, "ON");
    gToolBox->ShowDlgItem(IDC_OnLine, 1);
    SendToCNC('B', 0, 0, 0);
    gToolBox->EnableDlgItem(IDC_Isel, 0);
    gToolBox->EnableDlgItem(IDC_GoCnc, 0);
  }
  SetBusyStatus(szMGMCommStatusText);
  return 0;
}
//-------------------------------------------------------------
void GlTimer_CB(void* data)
{
  DXF3DDisplay* pWrk3DSheet = (DXF3DDisplay*)data;
  static time_t statbuf_mtime = 0;
  static ClipperLib::cInt sLookAt[10];

  /*
    static   time_t statbufS_mtime=0;
    static   time_t statbufS_mtime=0;
    struct stat statbufV;
    struct stat statbufS;
    stat(szvertexshaderFile, &statbufV);
    stat(szfragmentshaderFile, &statbufS);
    if (statbufV_mtime != statbufV.st_mtime ||
        statbufS_mtime != statbufS.st_mtime ) {
      if (statbufV_mtime!=0) {
        pWrk3DSheet->InitOpenGL();
        pWrk3DSheet->invalidate();
        pWrk3DSheet->redraw();
      }
      statbufV_mtime = statbufV.st_mtime;
      statbufS_mtime = statbufS.st_mtime;
    }
    */

  if(LookAtTimeOut)
    LookAtTimeOut--;
  struct stat statbuf;
  stat(gBastelUtils.m_IniFile, &statbuf);
  if(statbuf_mtime != statbuf.st_mtime) {
    if(LookAtTimeOut == 0 && statbuf_mtime != 0) {
      gBastelUtils.ExternModify();
      LookAtLaden(gLookAtIx);
    }
    statbuf_mtime = statbuf.st_mtime;
  }

  /*  struct stat statbuf;
    stat(gBastelUtils.m_IniFile, &statbuf);
    if (statbuf_mtime != statbuf.st_mtime) {
      if (statbuf_mtime!=0) {
        //InitLaden();
        LookAtLaden(gLookAtIx);
        if (memcmp(&sLookAt,&gPa.LookAt,sizeof(sLookAt))!=0) {
          memcpy(&sLookAt,&gPa.LookAt,sizeof(sLookAt));
          center[0]   =  (float)gPa.LookAt[0]/10.0;
          center[1]   =  (float)gPa.LookAt[1]/10.0;
          center[2]   =  (float)gPa.LookAt[2]/10.0;
          eye[0]      =  (float)gPa.LookAt[3]/100.0;
          eye[1]      =  (float)gPa.LookAt[4]/100.0;
          eye[2]      =  (float)gPa.LookAt[5]/100.0;
          RotateVal[0]=  (float)gPa.LookAt[6]*2.0;
          RotateVal[1]=  (float)gPa.LookAt[7]*2.0;
          RotateVal[2]=  (float)gPa.LookAt[8]*2.0;
          Perspective =  (float)gPa.LookAt[9];
          LightColor[0]= (float)gPa.LookAt[10]/100.0;
          Zoom        =  (float)gPa.LookAt[11]/100.0;
        }
        pWrk3DSheet->redraw();
        if (gDxfColorTab->visible()) gDxfColorTab->show();
        Show_ExecuteState();
      } else {
        memcpy(&sLookAt,&gPa.LookAt,sizeof(sLookAt));
      }
      statbuf_mtime = statbuf.st_mtime;
    }
  */
  // static   time_t time=0;
  //  static int fsize=0;
  //  if (gObjectManager&& gObjectManager->m_InUse==0) {
  //    if (stat(gObjectManager->m_FilePfadName, &statbuf)==0){
  //    if (gObjectManager->time != statbuf.st_mtime) {
  //      if (fsize==statbuf.st_size) {
  //        if (gObjectManager->FileOpen(NULL,1)) {
  ////        gObjectManager->time = statbuf.st_mtime;
  //        }
  //      }
  //      fsize=statbuf.st_size;
  //    }
  //    }
  //  }
  static int fsize = 0;
  if(gObjectManager && gObjectManager->m_InUse == 0) {
    if(evt_txt[0]) { // von Drag and Drop oder vom Zweiter Instance
      char* pnt = strchr(evt_txt, '\n');
      if(pnt)
        *pnt = '\0';
      if(strncmp(evt_txt, "file://", 7) == 0) {
        if(gObjectManager->FileOpen(&evt_txt[7], 1)) {
          memset(evt_txt, 0, sizeof(evt_txt));
        }
      } else {
        if(gObjectManager->FileOpen(evt_txt, 1)) {
          memset(evt_txt, 0, sizeof(evt_txt));
        }
      }
    } else if(stat(gObjectManager->m_FilePfadName, &statbuf) == 0) {
      if(gObjectManager->time != statbuf.st_mtime) {
        if(fsize == statbuf.st_size) { // muss zweites mal gleich sein
          gObjectManager->FileOpen(NULL, 1);
        }
        fsize = statbuf.st_size;
      }
    }
  }

  static float degTimer = 0;
  static int HaltTimer = 0;
#ifdef MOUSSERVICE
  if(gObject_redraw) {
    pWrk3DSheet->redraw();
    gObject_redraw = 0;
  }
#endif
  if(0) {
    degTimer += 0.1;
    if(degTimer > 360)
      degTimer = 0;
    RotateVal[0] = degTimer;
    pWrk3DSheet->redraw();
  }
#ifdef VIRTUALCNC
  if(!gOnline) {
    extern int SollV1;
    extern int SollV2;
    extern int SollV3;
    extern int SollV4;
    extern int SollTempo;
    extern char gCNC_HALT;
    EnterCriticalSection(&SerialComm::s_critical_section);
    float xd = gCNCPos[Xp] - SollV1;
    float yd = gCNCPos[Yp] - SollV2;
    float zd = gCNCPos[Zp] - SollV3;
    float ad = (gCNCPos[Ap] - SollV4)/3.0;
    float w;
    if(gPa.Maschine[eDrehachse]) {
      w = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2) + pow(ad, 2));
    } else {
      w = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2));
    }
    float tempo = 1 + _min(100, w / 2);
    if(w > (SollTempo / 20.0))
      tempo = 10+ SollTempo / 20.0;
    else
      tempo = w;
    if(w>3.0) {
      gCNCPos[Xp] -= (int)(tempo * xd / w);
      gCNCPos[Yp] -= (int)(tempo * yd / w);
      gCNCPos[Zp] -= (int)(tempo * zd / w);
      gCNCPos[Ap] -= (int)(tempo * ad / w);
      gCNCCode = 'R';
      gNewPos = 1;
      pWrk3DSheet->redraw();
      glDisplay_redraw = 0;
      HaltTimer=100;
    } else {
      if(gCNCCode!= 'H' && gOkCount)
        gOkCount--;
      if (HaltTimer){
        HaltTimer--;
      } else {
        gCNC_HALT = gCNCCode = 'H';
      }
    }
    LeaveCriticalSection(&SerialComm::s_critical_section);
  }
  if(glDisplay_redraw) {
    glDisplay_redraw = 0;
    pWrk3DSheet->redraw();
  }
#endif
#ifdef WIN32
  Fl::repeat_timeout(0.001, GlTimer_CB, data);
#else
  Fl::repeat_timeout(0.02, GlTimer_CB, data);
#endif
}
//---------------------------------------------------------------
#ifdef NATIVE_FILECHOOSER
void PickFile_CB(Fl_Widget*, void*)
{
  // Create native chooser
  Fl_Native_File_Chooser native;
  native.title("Pick a file");
  native.type(Fl_Native_File_Chooser::BROWSE_FILE);
  native.filter("Dxf\t*.dxf\n"
                "Gcode\t*.{gcode,gcd}\n"
                "Text\t*.{txt,rtf}\n");
  native.preset_file(gFileExplorer->value());
  // Show native chooser
  switch(native.show()) {
  case -1:
    fprintf(stderr, "ERROR: %s\n", native.errmsg());
    break; // ERROR
  case 1:
    fprintf(stderr, "*** CANCEL\n");
    fl_beep();
    break; // CANCEL
  default: // PICKED FILE
    if(native.filename()) {
      if(gObjectManager)
        gObjectManager->FileOpen(native.filename());
      gFileExplorer->value(native.filename());
    } else {
      gFileExplorer->value("NULL");
    }
    break;
  }
}
#else

#ifdef FLTREE
int ShowFileExplorer(LPCSTR szpathstr, LPCSTR szExtention)
{
  gFileExplorer->clear();
  if(szExtention == NULL) {
    char* ending;
    ending = strrchr((char*)szpathstr, '.');
    if(ending)
      strcpy(gszExtention, ending);
    else
      strcpy(gszExtention, ".");
  } else
    strcpy(gszExtention, szExtention);

#ifdef WIN32
  DWORD dwLogicalDrives = GetLogicalDrives();
  CTVItem* pTVItem1 = 0;
  for(int i = 0; i < 26; i++) {
    if(dwLogicalDrives & 1) {
      char str[200];
      str[0] = 'A' + (char)i;
      str[1] = ':';
      str[2] = '\0';
      int ddrivetype = GetDriveType(str);
      CTVItem* pTVItem2 = (CTVItem*)new CFolder(gFileExplorer, str, imFolderClose);
      int image = imDrive;
      if(ddrivetype == DRIVE_CDROM) {
        image = imCompactDsik;
      } else if(ddrivetype == DRIVE_REMOVABLE) {
        image = iExtmemory;
      }
      if(pTVItem1 == 0) {
        gFileExplorer->add("Root", pTVItem2);
      } else {
        pTVItem1->add(gFileExplorer->prefs(), str, pTVItem2);
      }
      CTVItem* pTVItem3 = (CTVItem*)new CFolder(gFileExplorer, "Dummy", imFolderClose);
      pTVItem2->add(gFileExplorer->prefs(), "", pTVItem3);
      pTVItem2->close();
    }
    dwLogicalDrives >>= 1;
  }
  if(pTVItem1)
    pTVItem1->close();
#else
  CTVItem* pTVItem1 = (CTVItem*)new CFolder(gFileExplorer, szpathstr, imFolderClose);
  if(pTVItem1) {
    gFileExplorer->add("Root", pTVItem1);
    CTVItem* pTVItem2 = (CTVItem*)new CFolder(gFileExplorer, "Dummy", imFolderClose);
    pTVItem1->add(gFileExplorer->prefs(), "", pTVItem2);
    pTVItem1->close();
  }
#endif
  gFileExplorer->show();
  return 1;
}
#else
int ShowFileExplorer(LPCSTR szpathstr, LPCSTR szExtention)
{
  if(szExtention == NULL) {
    char* ending;
    ending = strrchr((char*)szpathstr, '.');
    if(ending)
      strcpy(gszExtention, ending);
    else
      strcpy(gszExtention, ".");
  } else
    strncpy(gszExtention, szExtention, sizeof(gszExtention));
  if(gFileExplorer->m_pack) {
    gFileExplorer->m_pack->clear();
    gFileExplorer->m_pack->begin();
    gFileExplorer->m_TVItem = NULL;

#ifdef WIN32
    DWORD dwLogicalDrives = GetLogicalDrives();
    CTVItem* pTVItem1 = 0;
    for(int i = 0; i	 < 26; i++) {
      if(dwLogicalDrives & 1) {
        char str[200];
        str[0] = 'A' + (char)i;
        str[1] = ':';
        str[2] = '\0';
        int ddrivetype = GetDriveType(str);
        int image = imFolderClose; // imDrive;
        /*      if (ddrivetype == DRIVE_CDROM) {
                image = imCompactDsik;
              } else if  (ddrivetype == DRIVE_REMOVABLE) {
                image =  iExtmemory;
              }
        */
        CTVItem* parent = NULL;
        CTVItem* pTVItem1 = (CTVItem*)new CFolder(gFileExplorer, parent, 0);
        pTVItem1->SetImage(imFolderClose);

        if(pTVItem1) {
          pTVItem1->m_lParam = (void*)(long)1;
          pTVItem1->SetText(str);
          pTVItem1->m_NumChild = INSERTCHILDREN;
          pTVItem1->m_SortKey = 0xFFFFFF00;
          gFileExplorer->InsertChildItem((CTVItem*)pTVItem1, INSERT_END);
          // IncrementDirScanStatus();
        }
      }
      dwLogicalDrives >>= 1;
    }

#else
    CTVItem* parent = NULL;
    CTVItem* pTVItem1 = (CTVItem*)new CFolder(gFileExplorer, parent, 0);
    pTVItem1->SetImage(imFolderClose);

    if(pTVItem1) {
      pTVItem1->m_lParam = (void*)(long)1;
      pTVItem1->SetText("home");
      pTVItem1->m_NumChild = INSERTCHILDREN;
      pTVItem1->m_SortKey = 0xFFFFFF00;
      gFileExplorer->InsertChildItem((CTVItem*)pTVItem1, INSERT_END);
      // IncrementDirScanStatus();
    }

/*
  CTVItem* pTVItem1 =  (CTVItem*) new CFolder(gFileExplorer,szpathstr,imFolderClose);
  if (pTVItem1)
  {
    gFileExplorer->add("Root",pTVItem1);
    CTVItem* pTVItem2 =  (CTVItem*) new CFolder(gFileExplorer,"Dummy",imFolderClose);
    pTVItem1->add(gFileExplorer->prefs(),"",pTVItem2);
    pTVItem1->close();
  }
  */
#endif
    int xx = gFileExplorer->x();
    int xy = gFileExplorer->y();
    int xw = gFileExplorer->w();
    int xh = gFileExplorer->h();
    gFileExplorer->m_pack->end();
    //  gFileExplorer->m_pack->resize(xx,xy,gFileExplorer->m_pack->w()/*xw-22*/,xh);
    gFileExplorer->show();
    gFileExplorer->redraw();
    return gFileExplorer->m_pack->children();
  }
  gFileExplorer->show();
  return 1;
}
#endif
#endif
//-------------------------------------------------------------
int OutputDebugInfo::level = 2;
int OutputDebugInfo::debug = 1;
CBastelUtils gBastelUtils;
//-------------------------------------------------------------
int FltkMessageBox(LPCSTR lpText, LPCSTR lpCaption, int Type)
{
  int ret = 0;
  if((Type & 0xF) == MB_OK) {
    fl_message(lpText, lpCaption);
    return 0;
  } else if((Type & 0xf) == MB_YESNOCANCEL) {
    ret = fl_choice("%s\n%s", "Nein", "Ja", "Abbruch", lpCaption, lpText);
    if(ret == 1)
      return IDYES;
    else if(ret == 0)
      return IDNO;
    return IDCANCEL;
  } else {
    ret = fl_choice("%s\n%s", "Nein", "Ja", NULL, lpCaption, lpText);
    return (ret == 1) ? IDYES : IDNO;
  }
}
//---------------------------------------------------------------
void SetStatusText(LPCSTR pnt, int num, int style)
{
  //  static char StatusTxt0[80];
  static char StatusTxt1[80];
  static char StatusTxt2[80];
  //  static char StatusTxt3[80];
  //  if (num==0) {
  //    if (gStatus0) {
  //      strncpy(StatusTxt0,pnt,sizeof(StatusTxt0));
  //      gStatus0->label(StatusTxt0);
  //    }
  //  } else
  if(num == 1) {
    if(gStatus1) {
      strncpy(StatusTxt1, pnt, sizeof(StatusTxt1));
      gStatus1->label(StatusTxt1);
    }
  } else if(num == 2) {
    if(gStatus2) {
      strncpy(StatusTxt2, pnt, sizeof(StatusTxt2));
      gStatus2->label(StatusTxt2);
    }
    //  } else if (num==3) {
    //    if (BusyStatus)
    //    {
    //    strncpy(StatusTxt3,pnt,sizeof(StatusTxt3));
    //      BusyStatus->label(StatusTxt3);
    //    }
  }
  StatusUpdate = 1;
}
//---------------------------------------------------------------
void cbScheme(Fl_Button*, void*)
{
  Fl::scheme("base");
  Fl::foreground(240, 240, 240);
  Fl::background(92, 91, 86);
  Fl::background2(51, 51, 51);
  Fl::set_color(FL_SELECTION_COLOR, 222, 177, 1);
}
//---------------------------------------------------------------
void cbFullScreen(Fl_Button*, void*)
{
  /*
    Fl::scheme("xp");
    Fl::foreground(10,10,10);
    Fl::background2(151,151,151);
    Fl::background(239, 235, 222);
  */
  if(MainWindow->fullscreen_active())
    MainWindow->fullscreen_off();
  else
    MainWindow->fullscreen();
}
//---------------------------------------------------------------
void cbExit(Fl_Widget*, void*)
{
  if(gpSerialComm)
    SendToCNC('G', 0, 0, 0);
  if(prefs) // && LonTreeMaster && ConnectionsBoxH)
  {
    if(gEditemain)
      yTab = gEditemain->h();
    Mainwidth = MainWindow->w();
    Mainheight = MainWindow->h();
    MainPosX = MainWindow->x();
    MainPosY = MainWindow->y();
    if(MainWindow->fullscreen_active() == 0) {
      if(gEditemain)
        prefs->set("Editemain", yTab);
      prefs->set("Mainwidth", Mainwidth);
      prefs->set("Mainheight", Mainheight);
      prefs->set("MainPosX", MainPosX);
      prefs->set("MainPosY", MainPosY);
      prefs->set("Fullscreen", 0);
    } else {
      prefs->set("Fullscreen", 1);
    }
    prefs->set("IselRs232Mode", gIselRs232Mode);

    prefs->flush();
  }

  if(gpSerialComm)
    delete(gpSerialComm);
  if(gbModify) {
    int ret = fl_choice("Data modyify, you want to quit?", "Nein", "Ja", NULL);
    if(ret == 1)
      exit(0);
  } else
    exit(0);
}
//---------------------------------------------------------------
void cbMainWindowExit(Fl_Widget*, void*)
{
  if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
    return; // ignore Escape
  cbExit(NULL, NULL);
}

//---------------------------------------------------------------
void PostMessage_cb(void* p)
{
  tMessage* msg = (tMessage*)p;
  if(msg->receiver) {
    // ruft CFltkDialogBox::MainProc
    Fl_Callback* cb = (Fl_Callback*)msg->receiver->callback();
    if(cb)
      cb(msg->receiver, (void*)(long)msg->Value);
  }
}
//---------------------------------------------------------------
void SetIniFile(LPCSTR FileName)
{
  char* pnt = getenv("HOME");
  if(pnt) {
    strcpy(CBastelUtils::s_IniPath, pnt);
    strcat(CBastelUtils::s_IniPath, "/.");
  } else {
#ifdef WIN32
    if(!pnt)
      pnt = getenv("HOME_PATH");
    if(!pnt)
      pnt = getenv("UserProfile");
    strcpy(CBastelUtils::s_IniPath, pnt);
    strcat(CBastelUtils::s_IniPath, "/.");
#else
    strcpy(CBastelUtils::s_IniPath, "");
#endif
  }
  strcat(CBastelUtils::s_IniPath, "bastel");
  if(FileName == NULL)
    gBastelUtils.SetIniFile("IselDxfcnc.ini");
  else
    gBastelUtils.SetIniFile(FileName);
}
//---------------------------------------------------------------

#ifdef WIN32

/* static int mbcs2utf(const char *s, int l, char *dst, unsigned dstlen) */
static int mbcs2utf(const char* s, int l, char* dst)
{
  static wchar_t* mbwbuf;
  unsigned dstlen = 0;
  if(!s)
    return 0;
  dstlen = (l * 6) + 6;
  mbwbuf = (wchar_t*)malloc(dstlen * sizeof(wchar_t));
  l = (int)mbstowcs(mbwbuf, s, l);
  /* l = fl_unicode2utf(mbwbuf, l, dst); */
  l = fl_utf8fromwc(dst, dstlen, mbwbuf, l);
  dst[l] = 0;
  free(mbwbuf);
  return l;
}

/*
Modifikation in FL_win32.cxx
typedef int (*WM_DevicechangeCB)(HWND,WPARAM, LPARAM);
WM_DevicechangeCB  gDevicechangeCB=NULL;

  case WM_DEVICECHANGE:
    if (gDevicechangeCB!=NULL){
      gDevicechangeCB(hWnd, wParam, lParam);
    }

*/
typedef int (*WM_DevicechangeCB)(HWND, WPARAM, LPARAM);
extern WM_DevicechangeCB gDevicechangeCB;

int DevicechangeCB(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  if(wParam == DBT_DEVNODES_CHANGED) {
    if(gCNCAktiv == 0) {
      gOnline = !gOnline;
    }
  } else if(wParam == DBT_DEVICEREMOVECOMPLETE || wParam == DBT_DEVICEARRIVAL) {
    DWORD dwData = (DWORD)lParam;
    DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*)dwData;
    if(hdr && hdr->dbch_size >= sizeof(DEV_BROADCAST_PORT_A)) {
      if(hdr->dbch_devicetype == DBT_DEVTYP_PORT) {
        PDEV_BROADCAST_PORT_A p = (PDEV_BROADCAST_PORT_A)lParam;
        int slen = hdr->dbch_size - (3 * sizeof(DWORD));
        if(slen > 4) {
          char devstr[30];
          int l = fl_utf8fromwc(devstr, sizeof(devstr), (wchar_t*)p->dbcp_name, slen);
          char str[30];
          gBastelUtils.GetProfileString("Profiler", "Comm", "COM1", str, sizeof(str));
          if(strncmp(devstr, str, 4) == 0) {
            if(wParam == DBT_DEVICEREMOVECOMPLETE) {
              gOnline = 0;
              gStopp = true;
            } else {
              gOnline = 1;
            }
          }
        }
      } else if(hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
        DEV_BROADCAST_VOLUME* vp = (DEV_BROADCAST_VOLUME*)lParam;
        int slen = hdr->dbch_size - (3 * sizeof(DWORD));
        if(slen >= 8) {
          ULONG m = vp->dbcv_unitmask;
          if(wParam == DBT_DEVICEREMOVECOMPLETE) {
            ScanFileTree = -1;
          } else {
            ScanFileTree = m;
          }
        }
      }
    }
  }
  return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  int rc, i;
  char** argv;
#ifdef ROLF_DEBUG
  // If we are using compiling in debug mode, open a console window so
  // we can see any printf's, etc...
  //
  // While we can detect if the program was run from the command-line -
  // look at the CMDLINE environment variable, it will be "WIN" for
  // programs started from the GUI - the shell seems to run all WIN32
  // applications in the background anyways...

  AllocConsole();
  freopen("conin$", "r", stdin);
  freopen("conout$", "w", stdout);
  freopen("conout$", "w", stderr);
#endif
  argv = (char**)malloc(sizeof(char*) * (__argc + 1));
  i = 0;
  while(i < __argc) {
    int l;
    unsigned dstlen;
    if(__wargv) {
      for(l = 0; __wargv[i] && __wargv[i][l]; l++) {
      }; // is this just wstrlen???
      dstlen = (l * 5) + 1;
      argv[i] = (char*)malloc(dstlen);
      //    ar[i][fl_unicode2utf(__wargv[i], l, ar[i])] = 0;
      dstlen = fl_utf8fromwc(argv[i], dstlen, __wargv[i], l);
      argv[i][dstlen] = 0;
    } else {
      for(l = 0; __argv[i] && __argv[i][l]; l++) {
      };
      dstlen = (l * 5) + 1;
      argv[i] = (char*)malloc(dstlen);
      //      ar[i][mbcs2utf(__argv[i], l, ar[i], dstlen)] = 0;
      argv[i][mbcs2utf(__argv[i], l, argv[i])] = 0;
    }
    i++;
  }
  argv[__argc] = 0;
  // Run the standard main entry point function...
  gDevicechangeCB = DevicechangeCB;
#else

int main(int argc, char** argv)
{
  // Fl::use_high_res_GL(1);
  int i = 0;
#endif
  if(!prefs) {
    prefs = new Fl_Preferences(Fl_Preferences::USER, "fltk.org", "dxfcnc");
  }
  prefs->get("ToolWidth", xTool, xTool);
  prefs->get("Editemain", yTab, yTab);
  prefs->get("Mainwidth", Mainwidth, Mainwidth);
  prefs->get("Mainheight", Mainheight, Mainheight);
  prefs->get("MainPosX", MainPosX, MainPosX);
  prefs->get("MainPosY", MainPosY, MainPosY);
  prefs->get("IselRs232Mode", gIselRs232Mode, gIselRs232Mode);
  prefs->get("SaugerAuto", gSaugerAuto, gSaugerAuto);
  prefs->get("TouchScreen", bTouchScreen, bTouchScreen);
  int bfullscreen = 0;
  prefs->get("Fullscreen", bfullscreen, bfullscreen);

  i = 0;
  SetIniFile(gIselRs232Mode ? "IselDxf.ini" : "GoCnc.ini");
//    Fl::set_font(FL_FREE_FONT,"-*-medium-*-normal-*-12-*-*-*-*-*-*-*");
//    Fl::set_font(FL_FREE_FONT,"-microsoft-comic sans ms-medium-*-normal-*-12-240-*-*-*-*-*-*");
#define FONTFACE FL_HELVETICA //  FL_COURIER
#undef FONTSIZE
#define FONTSIZE 12
  fl_font(FONTFACE, FONTSIZE);
  R = new ReseorceInit();
  if(1) {
    Fl::get_system_colors();
    Fl::scheme("gtk+");
    Fl::foreground(240, 240, 240);
    Fl::background(92, 91, 86);
    Fl::background2(86, 85, 80);
    Fl::set_color(FL_SELECTION_COLOR, 241, 176, 0);
  } else {
    // Fl::scheme("xp");
    Fl::scheme("gleam");
    Fl::foreground(255, 225, 0); // 140);
    Fl::background(32, 31, 26);
    Fl::background2(26, 25, 20);
    Fl::set_color(FL_SELECTION_COLOR, 241, 176, 0);
  }

  int num = Fl::screen_num(MainPosX + 100, MainPosY + 100);

  if(bfullscreen) {
    Mainwidth = Fl::w();
    Mainheight = Fl::h();
    int X, Y;
    Fl::screen_xywh(X, Y, Mainwidth, Mainheight, MainPosX + 100, MainPosY + 100);
    printf("Mainwidth =%d Mainheight=%d ", Mainwidth, Mainheight);
  }
#if LONMSG
  gLonService = new CUDP_Comm();
  extern void LonMessage(unsigned char* msg);
  gLonService->m_Callback = LonMessage;
#endif
  // Mainheight = 480;
  // Mainwidth = 848;

  int PerspektiveTool = 1; // Mainwidth < 850; // Raspi LCD
                           //  if (Mainheight < 768) Mainheight = 768;
  MainWindow = new Fl_Double_Window(Mainwidth, Mainheight, "DxfCnc");
  if(MainWindow) {
    Zoll = gBastelUtils.GetProfileInt("DXFExtra", "Zoll", 0) != 0;

    MainWindow->position(MainPosX, MainPosY);
    MainWindow->icon(&ReseorceInit::ic_AppIcon_xpm);
    MainWindow->callback(cbMainWindowExit);
    // if (gBastelUtils.GetProfileInt("Display","Fullscreen",1))  {
    if(bfullscreen) {
      MainWindow->fullscreen();
    }

    // int ToolHeight =  yTool;
    int MenuHeight = 48;
    int ToolHeight = Mainheight - yStatus - MenuHeight;
    int border = 2;

    gToolBox = new CToolBox(0, 0, Mainwidth, MenuHeight, 0);
    if(gToolBox) {
      int s = (gToolBox->InitWindow(MainWindow, 0, 0, 0, Mainwidth, MenuHeight, 0));
      gToolBox->end();
    }

#ifdef TOOLSCROLL
    CFingerScroll* ToolPart1 = new CFingerScroll(0, MenuHeight, xTool, 10, 0);
    Fl_Pack* ToolPart = ToolPart1->m_pack;
#else
    //      Fl_Scroll* ToolPart1= new Fl_Scroll(0,MenuHeight,xTool,Mainheight-yStatus-MenuHeight*2-3,0);
    //      ToolPart1->resizable(0);
    //      ToolPart1->type (0);
    Fl_Pack* ToolPart = new Fl_Pack(0, MenuHeight, xTool, 10, 0);
#endif

    ToolPart->end();
    ToolPart->spacing(0);
    int xTool2 = xTool;
    /*      gToolBox = new CToolBox(border,MenuHeight,xTool2-border,yTool-MenuHeight,0);
          if (gToolBox )
          {
            int s =  (gToolBox
       ->InitWindow(ToolPart,0,border,MenuHeight,xTool-border,yTool-MenuHeight,PerspektiveTool)); yTool  = (s   >> 16)+
       MenuHeight*2; xTool2 = xTool  = (s & 0xFFFF)+ border*2;; printf("xTool=%d\n",xTool); gToolBox
       ->size(xTool-border,yTool-MenuHeight); gToolBox ->end();
          }
          ToolPart->size(xTool,10);
          ToolPart->add(gToolBox );
    */
    int yTool3 = 2 + MenuHeight;
    gOptionsBox = new COptionsBox(border, yTool3, xTool2, ToolHeight, "1");
    if(gOptionsBox) {
      int s = gOptionsBox->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
      // ToolHeight  = (s   >> 16);
      xTool2 = xTool = (s & 0xFFFF) + border * 2;
      ;
      int my = Mainheight - yStatus - MenuHeight * 2 - 3 - yTool;
      if(ToolHeight < my)
        ToolHeight = my;
      ToolPart->size(xTool - border, ToolHeight); /// yTool-MenuHeight);

      gOptionsBox->size(xTool2, ToolHeight);
      gOptionsBox->end();
    }

    gDxfColorTab = new CDxfColorTab(border, yTool3, xTool2, ToolHeight, "1");
    if(gDxfColorTab) {
      int s = gDxfColorTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
      // ToolHeight  = (s   >> 16);
      xTool2 = xTool = (s & 0xFFFF) + border * 2;
      ;
      int my = Mainheight - yStatus - MenuHeight * 2 - 3 - yTool;
      if(ToolHeight < my)
        ToolHeight = my;
      ToolPart->size(xTool - border, ToolHeight); /// yTool-MenuHeight);

      gDxfColorTab->size(xTool2, ToolHeight);
      gDxfColorTab->end();
    }

    SettingsTabs = new BastelTabs(border, yTool3, xTool2, ToolHeight, 0); // Fl_Tabs(border,yTool3,xTool2,ToolHeight,0);
    if(SettingsTabs) {
      SettingsTabs->add(gOptionsBox);
      SettingsTabs->add(gDxfColorTab);

      gCalculatorTab = new CCalculatorTab(border, yTool3, xTool2, ToolHeight, "2");
      if(gCalculatorTab) {
        gCalculatorTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gCalculatorTab->end();
        gCalculatorTab->size(xTool2, ToolHeight);
      }

      gDxfLayer = new CDxfLayer(border, yTool3, xTool2, ToolHeight, "2");
      if(gDxfLayer) {
        gDxfLayer->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gDxfLayer->end();
        gDxfLayer->size(xTool2, ToolHeight);
      }

      gNullpunktTab = new CNullpunktTab(border, yTool3, xTool2, ToolHeight, "2");
      if(gNullpunktTab) {
        gNullpunktTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gNullpunktTab->end();
        gNullpunktTab->size(xTool2, ToolHeight);
      }
      gToolAdjustTab = new CToolAdjustTab(border, yTool3, xTool2, ToolHeight, "3");
      if(gToolAdjustTab) {
        gToolAdjustTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gToolAdjustTab->end();
        gToolAdjustTab->size(xTool2, ToolHeight);
      }
      gSettingsTab = new CSettingsTab(border, yTool3, xTool2, ToolHeight, "4");
      if(gSettingsTab) {
        gSettingsTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gSettingsTab->end();
        gSettingsTab->size(xTool2, ToolHeight);
      }
      gLaserTab = new CLaserTab(border, yTool3, xTool2, ToolHeight, "5");
      if(gLaserTab) {
        gLaserTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gLaserTab->end();
        gLaserTab->size(xTool2, ToolHeight);
      }
      gGcodeTab = new CGcodeTab(border, yTool3, xTool2, ToolHeight, "6");
      if(gGcodeTab) {
        gGcodeTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gGcodeTab->end();
        gGcodeTab->size(xTool2, ToolHeight);
      }
      gDXFOptimize = new CDXFOptimize(border, yTool3, xTool2, ToolHeight, "6");
      if(gDXFOptimize) {
        gDXFOptimize->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gDXFOptimize->end();
        gDXFOptimize->size(xTool2, ToolHeight);
      }

      gDXFtoCNCTab = new CDXFtoCNCTab(border, yTool3, xTool2, ToolHeight, "6");
      if(gDXFtoCNCTab) {
        gDXFtoCNCTab->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0);
        gDXFtoCNCTab->end();
        gDXFtoCNCTab->size(xTool2, ToolHeight);
      }

      // if (PerspektiveTool) {
      gPerspektive = new CPerspektive(border, yTool3, xTool2, ToolHeight, "7"); //-yStatus
      if(gPerspektive) {
        int s = gPerspektive->InitWindow(SettingsTabs, 0, border, yTool3, xTool2, ToolHeight, 0); // PerspektiveTool);
        xPerspektive = s & 0xFFFF;
        gPerspektive->end();
        gPerspektive->size(xTool2, ToolHeight);
        // gPerspektive->resize(Mainwidth-xPerspektive,0,xPerspektive,Mainheight-0);//-yStatus
      }
      //}

#ifdef NATIVE_FILECHOOSER
      gFileExplorer = new Fl_Input(border, yTool3, xTool2, ToolHeight, "7"); //"Filename");
#else
#ifdef FLTREE
      gFileExplorer = new CTreeView(border, yTool3, xTool2, ToolHeight, 0);
      if(gFileExplorer) {
        gFileExplorer->callback(TreeCallback, 0);
        gFileExplorer->selectmode(FL_TREE_SELECT_SINGLE);
#if FLTK_ABI_VERSION >= 10304
        gFileExplorer->item_reselect_mode(FL_TREE_SELECTABLE_ALWAYS);
#endif
        gFileExplorer->showroot(0);
        // pFileExplorer->end();
      }
#else
      gFileExplorer = new CTreeView(border, yTool3, xTool2, ToolHeight, 0);
      if(gFileExplorer) {
      }
#endif
#endif

      pDXFTree = new CTreeView(border, yTool3, xTool2, ToolHeight, "8");
      if(pDXFTree) {
        pDXFTree->size(xTool2, ToolHeight);
      }

      SettingsTabs->end();
      ToolPart->add(SettingsTabs);
      //        ToolPart1->end();
      //        ToolPart1->scrollbar.value (0,yTool+ToolHeight,0,yTool+ToolHeight);
      //        ToolPart1->size(xTool,ToolPart1->h());
    }
    xPerspektive = 0;
    if(gPerspektive == NULL) {
      // Wenn nicht als ToolObjet dann rechts am Rand
      gPerspektive = new CPerspektive(Mainwidth - xPerspektive, 0, xPerspektive, Mainheight - 0, 0); //-yStatus
      if(gPerspektive) {
        int s = gPerspektive->InitWindow(MainWindow, 0, Mainwidth - xPerspektive, 0, xPerspektive, Mainheight - 0, 0);
        xPerspektive = s & 0xFFFF;
        gPerspektive->end();
        gPerspektive->resize(Mainwidth - xPerspektive, 0, xPerspektive, Mainheight - 0); //-yStatus
      }
    }

    Fl_Group* StatusPart = new Fl_Group(0, Mainheight - yStatus, Mainwidth - xTool, yStatus, 0);
    if(StatusPart) {
      StatusPart->box(FL_THIN_UP_FRAME);
      int yStatus1 = yStatus - 4;
      int breite0 = 0;
      int breite3 = Mainwidth / 6;
      int breite2 = Mainwidth / 3;
      int breite1 = Mainwidth - breite2 - breite3 - xPerspektive;
      int ypos3 = Mainheight - yStatus + 2;
      gStatus1 = new Fl_Box(breite0, ypos3, breite1, yStatus1, "File");
      gStatus1->box(FL_THIN_DOWN_BOX);
      gStatus2 = new Fl_Box(breite0 + breite1, ypos3, breite2, yStatus1, "Selection");
      gStatus2->box(FL_THIN_DOWN_BOX);

      BusyStatus = new Fl_Progress(breite0 + breite1 + breite2, ypos3, breite3 - 2, yStatus1, "Progress");
      //        BusyStatus->box(FL_THIN_DOWN_BOX);
      StatusPart->end();
    }

    Fl_Group* WorkPart = new Fl_Group(xTool, MenuHeight, Mainwidth - xTool - xPerspektive,
        Mainheight - yStatus - MenuHeight, 0); // Fl_Group(0,0,xTool,Mainheight-yStatus,0);
    if(WorkPart) {
      WorkPart->box(FL_DOWN_FRAME);
      {
        gObjectManager = new CObjectManager();
        gWrk3DSheet = new DXF3DDisplay(
            xTool, MenuHeight, Mainwidth - xTool - xPerspektive, Mainheight - yStatus - MenuHeight, NULL);
        gWrk3DSheet->when(FL_WHEN_RELEASE);
        gWrk3DSheet->end();
      }
      WorkPart->end();
    }
    MainWindow->resizable(StatusPart);
    MainWindow->resizable(WorkPart);
    ToolPart->resizable(MainWindow);

    MainWindow->end();
    // if (Mainheight > 800)
    // ShowTab(IDM_DisplayAdj);// NULLPUNKT);
    ShowTab(IDM_GCode); // NULLPUNKT);
    Show_ExecuteState();
  }

  char* pnt2 = getenv("HOME");
  if(pnt2 == 0) {
    if(!pnt2)
      pnt2 = getenv("HOME_PATH");
    if(!pnt2)
      pnt2 = getenv("UserProfile");
  }
  ScanFileTree = -1;
#ifdef WIN32
  MainWindow->show(__argc, argv);
#else
  MainWindow->show(argc, argv);
#endif
  Fl::set_color(FL_SELECTION_COLOR, 255, 176, 0);
  {
    char str[256];
    // gBastelUtils.GetProfileString("CNCFile","LastFile","/home/rolf",str,sizeof(str));
    prefs->get("LastFile", str, ".", sizeof(str));

    // extern void load_file(const char *newfile, int ipos);
    // if (gEditemain )load_file(str,1);
    gStatus1->label(str);
    if(gObjectManager)
      gObjectManager->FileOpen(str, 1);
  }

  // MainWindow ->redraw_overlay();

  // extern void InitFingerTouchService(void);
  if(bTouchScreen) {
    InitFingerTouchService();
  }
  gAppRun = 1;
  Fl::lock();
  void* next_message;
  int ret = 0; // Fl::run();
  // Fuehre die FLTK-Schleife aus und verarbeite Thread-Nachrichten
#ifdef WIN32
  while(Fl ::wait(10) > 0)
#else
  while(Fl ::wait() > 0)
#endif
  {
    if((next_message = Fl ::thread_message()) != NULL) {
      tMessage* pmsg = (tMessage*)next_message;
      if(pmsg->receiver) {
        ((CFltkDialogBox*)pmsg->receiver)->WindowProc(WM_COMMAND, pmsg->Code, pmsg->Value);
      }
      if(next_message)
        free(next_message);
      next_message = NULL;
    }
    if(SerialInitDelay)
      SerialInitDelay--;
    if(gOnline && gpSerialComm && gpSerialComm->IsRunning() == 0) {
      if(SerialInitDelay == 0) {
        gToolBox->SetButtonImage(IDC_OnLine, imOffline);
        gToolBox->ShowDlgItem(IDC_OnLine, 1);
        gToolBox->SetDlgItemText(IDC_OnLine, "OFF-line");
        usleep(10000);
        delete(gpSerialComm);
        gpSerialComm = NULL;
        printf("IniPortProc\n");
        IniPortProc();
        SerialInitDelay = 100;
      }
    } else if(gOnline && gpSerialComm == NULL) {
      IniPortProc();
    } else if(!gOnline && gpSerialComm != NULL) {
      delete(gpSerialComm);
      gpSerialComm = NULL;
      gToolBox->SetButtonImage(IDC_OnLine, imOffline);
      gToolBox->ShowDlgItem(IDC_OnLine, 1);
      gToolBox->SetDlgItemText(IDC_OnLine, "OFF");
      SetBusyStatus(szMGMCommStatusText);
      gToolBox->EnableDlgItem(IDC_Isel, 1);
      gToolBox->EnableDlgItem(IDC_GoCnc, 1);
    } else if(gNewPos) {
      // Position nachtragen updatedrwaw()
      gNewPos = 0;
      TestforCNCCommands();
      if(gWrk3DSheet->visible()) {
        // gWrk3DSheet->invalidate();
        gWrk3DSheet->redraw();
      }
    }
#ifdef NATIVE_FILECHOOSER
#else
    if(ScanFileTree) {
#ifdef WIN32
      //        if (gWrk3DSheet->visible()) {
      //        ShowFileExplorer("","");
      //    } else {
      ShowFileExplorer("", "*.nc;*.gcode;*.cnc;*.dxf");
      //  }
#else
      strncpy(gszHomeFilePath, pnt2, sizeof(gszHomeFilePath));
      // if (gWrk3DSheet->visible()) {
      //  ShowFileExplorer("Home","");
      //} else {
      ShowFileExplorer("home", ".nc;*.gcode;*.cnc");
      //}
#endif
#ifdef WIN32
      if(ScanFileTree != -1) {
        int m = ScanFileTree;
        char drive[10] = { 0 };
        drive[0] = 'A' - 1;
        while(m) {
          m >>= 1;
          drive[0]++;
        }
        drive[1] = ':';
        SelectTreeItem(gFileExplorer, drive);
        /*if (wParam == DBT_DEVICEREMOVECOMPLETE) {
        printf("Device %c Removed \n",(char)drive[0]);
        } else {
        printf("Device %c Inserted \n",(char)drive[0]);
      }*/

      } else
#endif
      {
      }
      gFileExplorer->redraw();
      ScanFileTree = 0;
    }
#endif

    /*if (wParam) {
      if (wParam == DBT_DEVNODES_CHANGED) {
        wParam =0;
      } else if (wParam == DBT_DEVICEREMOVECOMPLETE
        || wParam == DBT_DEVICEARRIVAL) {
        DWORD dwData = (DWORD) lParam;
        DEV_BROADCAST_HDR * hdr = (DEV_BROADCAST_HDR *)dwData;
        if (hdr && hdr->dbch_size >= sizeof(DEV_BROADCAST_PORT_A)) {
          if (hdr->dbch_devicetype ==  DBT_DEVTYP_PORT) {
          PDEV_BROADCAST_PORT_A p = (PDEV_BROADCAST_PORT_A)lParam;
            int slen = hdr->dbch_size - (3 * sizeof(DWORD));
            if (slen>4) {
              char devstr[20];
              if (p->dbcp_name[1]=='\0') {
                for (int i=0;i<slen/2;i++) {
                  devstr[i] = p->dbcp_name[i*2];
                }
              } else {
                memcpy(&devstr[0],(char *)p->dbcp_name,_min(slen,20));
              }
              char str[30];
              gBastelUtils.GetProfileString("Profiler","Comm","COM1",str,sizeof(str));
              if (strncmp(devstr,str,4)==0){
                if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                  lParam=0;
                  gOnline=0;
                  gStopp=true;
                  if (gpSerialComm != NULL) {
                    delete(gpSerialComm);
                    gpSerialComm=NULL;
                    gToolBox->SetButtonImage(IDC_OnLine,imOffline);
                    gToolBox->ShowDlgItem(IDC_OnLine,1);
                    gToolBox->SetDlgItemText(IDC_OnLine,"OFF");
                    SetBusyStatus(szMGMCommStatusText);
                    gToolBox->EnableDlgItem(IDC_Isel,1);
                    gToolBox->EnableDlgItem(IDC_GoCnc,1);
                  }
                } else {
                  gOnline=1;
                  //IniPortProc();
                }
              }
            }

          }
        } else {
          if (wParam == DBT_DEVICEREMOVECOMPLETE) {
            gOnline=0;
          } else {
            gOnline=1;
          }
        }
      } else {
    }
    / * if (wParam == DBT_DEVNODES_CHANGED) {
    } else if (wParam == DBT_DEVICEARRIVAL) {
    } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
        int devTypeD = lParam;
        if (devTypeD == DBT_DEVTYP_PORT) {
          ComRemove++;
        }
    } else {
    }* /
      wParam=0;
    }*/
    if(BusyStatus) {
      // gStatus0->redraw();
      gStatus1->redraw();
      gStatus2->redraw();
      UpdateDisplay();
      // BusyStatus->redraw();
    }
    usleep(10000);
  }
#if LONMSG
  if(gLonService)
    delete gLonService;
#endif
  if(gpSerialComm) {
    SendToCNC('G', 0, 0, 0);
    delete(gpSerialComm);
  }
  return ret;
}

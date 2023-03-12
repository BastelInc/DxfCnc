#ifndef _DXFCNC_INCLUDE_
#define _DXFCNC_INCLUDE_

//#undef  NATIVE_FILECHOOSER
//#define  NATIVE_FILECHOOSER

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Gl_Window.H>

#ifdef NATIVE_FILECHOOSER
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#endif

#include "BastelUtils.h"
#include "CncSetup.h"
#include "BlockModel.h"
#include "Settings.h"
#include "TreeView.h"
void PostMessage_cb(void *p);
extern Fl_Double_Window * MainWindow;
extern int  FltkMessageBox(LPCSTR lpText,LPCSTR lpCaption,int Type);
extern int  gAppRun;
extern int  gOnline;
extern int  gSaugerAuto;
extern int  gIselRs232Mode;
extern int  bTouchScreen;
extern void SetIniFile(LPCSTR FileName);
extern CToolBox    * gToolBox;
extern COptionsBox * gOptionsBox;
extern BastelTabs  * SettingsTabs;
//extern Fl_Tabs     * SettingsTabs;
extern CDxfColorTab  * gDxfColorTab;
extern CCalculatorTab* gCalculatorTab;
extern CNullpunktTab  * gNullpunktTab;
extern CToolAdjustTab * gToolAdjustTab;
extern CSettingsTab  * gSettingsTab;
extern CDxfLayer    * gDxfLayer;
extern CLaserTab  * gLaserTab;
extern CGcodeTab  * gGcodeTab;
extern CBlockTab  * gBlockTab;
extern CPerspektive* gPerspektive;
extern CDXFOptimize *gDXFOptimize;
extern CDXFtoCNCTab *gDXFtoCNCTab;
#ifdef NATIVE_FILECHOOSER
extern Fl_Input * gFileExplorer;
void PickFile_CB(Fl_Widget*, void*);
#else
extern CTreeView   * gFileExplorer;
#endif
extern Fl_Box      * gStatus0;
extern Fl_Box      * gStatus1;
extern Fl_Box      * gStatus2;
extern Fl_Progress * BusyStatus;
extern Fl_Window   * gEditemain;


#endif

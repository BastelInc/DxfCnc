//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "BastelUtils.h"
#include "CncSetup.h"
#include "DxfCnc.h"
#include "FileFolder.h"
#include "FltkDialogBox.h"
#include "GlDisplay.h"
#include "Layers.h"
#include "Object.h"
#include "ProgressStatus.h"
#include "Resource.h"
#include "Serialcom.h"
#include "Settings.h"
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <ctype.h>
#include <math.h>
#ifdef LONMSG
#include "LonMessage.h"
#endif
extern SerialComm* gpSerialComm;
extern CTreeView* pDXFTree;
extern int bHideRapidTrans;
extern int gNAuswahl;
#define TOUCHSCREEN
//#undef TOUCHSCREEN

extern DXF3DDisplay* gWrk3DSheet;
extern int Geometrie;

#ifdef GCODEDOWN
#define MENUEW 52
#define MENUEH 25
const tDialogCtlItem rg_ToolBox[] = {

  { eDialogBox, "Tool", IDC_DialogBox, 4, 0, 70, 240, WS_BORDER | WS_TABSTOP }, // Dialog
  { BUTTON "File", IDM_FILEOPEN, MENUEW * 0, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "Nullp.k", IDM_NULLPUNKT, MENUEW * 1, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "GCode", IDM_GCode, MENUEW * 2, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "Reload", IDM_UPDATE, MENUEW * 3, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },

  { BUTTON "", IDM_ZoomGanz, MENUEW * 4, 1, MENUEW / 2, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "", IDM_Zoom1, MENUEW * 4 + MENUEW / 2, 1, MENUEW / 2, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "Top", IDC_ShowTop, MENUEW * 5, 1, MENUEW / 2, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "Front", IDC_ShowFront, MENUEW * 5 + MENUEW/2, 1, MENUEW / 2, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "Def", IDC_GetDefault, MENUEW * 6, 1, MENUEW / 2, MENUEH, WS_TABSTOP, NULL },

  { BUTTON "Stopp", IDM_STOPP, MENUEW * 6+MENUEW /2, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "", IDC_OnLine, MENUEW * 7+MENUEW /2, 1, MENUEW/2, MENUEH, WS_TABSTOP, NULL },
  { BUTTON "", IDC_Exit, MENUEW * 8, 1, MENUEW*2/3, MENUEH, WS_TABSTOP, NULL }, { 0 }
};
#else
//#define MENUEW 49
//#define MENUEH 25
//const tDialogCtlItem rg_ToolBox[] = {
//  { eDialogBox, "Tool", IDC_DialogBox, 4, 0, 70, 240, WS_BORDER | WS_TABSTOP }, // Dialog
//  { BUTTON "File", IDM_FILEOPEN, MENUEW * 0, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "Nullp.k", IDM_NULLPUNKT, MENUEW * 1, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "DXF-C", IDM_DXFColor, MENUEW * 2, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "UpDt.", IDM_UPDATE, MENUEW * 3, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  //  { BUTTON "Modify\nDXF"     ,IDM_OPTIMIZEDXF   ,MENUEW*3,1,MENUEW,MENUEH,WS_TABSTOP,NULL},
//  { BUTTON "Config", IDM_DXFSPEZIAL, MENUEW * 4, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "Layers", IDM_LAYER, MENUEW * 5, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "Display", IDM_DisplayAdj, MENUEW * 6, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  //  { BUTTON "Save\nParam"     ,IDM_FILESAVE      ,MENUEW*7,1,MENUEW,MENUEH,WS_TABSTOP,NULL},
//  { BUTTON "Options", IDM_TOPDTOOL, MENUEW * 7, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "Conn", IDC_OnLine, MENUEW * 8, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "Stopp", IDM_STOPP, MENUEW * 9, 1, MENUEW, MENUEH, WS_TABSTOP, NULL },
//  { BUTTON "", IDC_Exit, MENUEW * 10, 1, MENUEW, MENUEH, WS_TABSTOP, NULL }, { 0 }
//};
#define MENUEW 54
#define MENUEH 21

const tDialogCtlItem rg_ToolBox  []= {

  { eDialogBox,"Tool",IDC_DialogBox, 4,  0,70,240,WS_BORDER| WS_TABSTOP},                       // Dialog
  { BUTTON "Open",IDM_FILEOPEN,2+MENUEW*0,2+MENUEH*0,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},
  { BUTTON "Save\nParam",IDM_FILESAVE,2+MENUEW*1,2+MENUEH*0,MENUEW,MENUEH,WS_TABSTOP,NULL},
  { BUTTON "",IDC_Exit,2+MENUEW*2,2+MENUEH*0,MENUEW,MENUEH,WS_TABSTOP,NULL},

  { BUTTON "Up\ndtate",IDM_UPDATE,2+MENUEW*0,2+MENUEH*1,MENUEW,MENUEH,WS_TABSTOP,NULL},
  { BUTTON "Block\nDefiniton",IDM_GBlock,2+MENUEW*1,2+MENUEH*1,MENUEW,MENUEH,WS_TABSTOP,NULL},
  { BUTTON "Display",IDM_DisplayAdj,2+MENUEW*2,2+MENUEH*1,MENUEW,MENUEH,WS_TABSTOP,NULL},
//  { BUTTON "Fulls"           ,IDC_ModeFullscreen,2+MENUEW*2,2+MENUEH*1,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},

  { BUTTON "Modify\nDXF",IDM_OPTIMIZEDXF,2+MENUEW*0,2+MENUEH*2,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP| WS_GROUP,NULL},
//  { BUTTON "ISEL"            ,IDC_Isel          ,  4, 2+MENUEH*2,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "DXFtoCNC",IDC_DXFtoCNCTab,2+MENUEW*1,2+MENUEH*2,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "Conn",IDC_OnLine,2+MENUEW*2,2+MENUEH*2,MENUEW,MENUEH,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},

  { BUTTON "Config",IDM_DXFSPEZIAL,2+MENUEW*0,2+MENUEH*3,MENUEW,MENUEH,WS_GROUP | WS_TABSTOP,NULL},
  { BUTTON "Layers",IDM_LAYER,2+MENUEW*1,2+MENUEH*3,MENUEW,MENUEH,WS_TABSTOP,NULL},
  { BUTTON "Tree\nSel",IDM_ShowDXFTree,2+MENUEW*2,2+MENUEH*3,MENUEW,MENUEH,WS_TABSTOP,NULL},

  { BUTTON "GCode",IDM_GCode,2+MENUEW*0,2+MENUEH*4,MENUEW,MENUEH,WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "Nullp.k",IDM_NULLPUNKT,2+MENUEW*1,2+MENUEH*4,MENUEW,MENUEH,WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "Laser",IDM_LASER,2+MENUEW*2,2+MENUEH*4,MENUEW,MENUEH,WS_TABSTOP| WS_GROUP,NULL},

  { BUTTON "Adjust",IDM_Adjust,2+MENUEW*0,2+MENUEH*5,MENUEW,MENUEH,WS_GROUP |WS_TABSTOP,NULL},
  { BUTTON "DXF-C",IDM_DXFColor,2+MENUEW*1,2+MENUEH*5,MENUEW,MENUEH,WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "*TOP*",IDM_TOPDTOOL,2+MENUEW*2,2+MENUEH*5,MENUEW,MENUEH,WS_TABSTOP,NULL},
  {0}
};


#endif

const tDialogCtlItem rg_OptionsBox[] = {

  { eDialogBox, "Tool", IDC_DialogBox, 4, 0, 70, 240, WS_BORDER | WS_TABSTOP }, // Dialog
  { BUTTON "File", IDM_FILEOPEN, 4, 2, 52, 28, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "Save\nParam", IDM_FILESAVE, 58, 2, 52, 28, WS_TABSTOP, NULL },
  { BUTTON "", IDC_Exit, 112, 2, 52, 28, WS_TABSTOP, NULL },

  { BUTTON "Up\ndtate", IDM_UPDATE, 4, 32, 52, 28, WS_TABSTOP, NULL },
  { BUTTON "Reset", IDM_Refernezfahrt, 58, 32, 52, 28, WS_TABSTOP, NULL },
  { BUTTON "Display", IDM_DisplayAdj, 112, 32, 52, 28, WS_TABSTOP, NULL },
  //{ BUTTON "Fulls"           ,IDC_ModeFullscreen,112, 32,52,28,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},

  { BUTTON "Modify\nDXF", IDM_OPTIMIZEDXF, 4, 62, 52, 28, BS_AUTORADIOBUTTON | WS_TABSTOP | WS_GROUP, NULL },
  //  { BUTTON "ISEL"            ,IDC_Isel          ,  4, 62,52,28,BS_AUTORADIOBUTTON | WS_TABSTOP| WS_GROUP,NULL},
  { BUTTON "DXFtoCNC", IDC_DXFtoCNCTab, 58, 62, 52, 28, BS_AUTORADIOBUTTON | WS_TABSTOP | WS_GROUP, NULL },
  { BUTTON "Conn", IDC_OnLine, 112, 62, 52, 28, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },

  { BUTTON "Config", IDM_DXFSPEZIAL, 4, 92, 52, 28, WS_GROUP | WS_TABSTOP, NULL },
  { BUTTON "Layers", IDM_LAYER, 58, 92, 52, 28, WS_TABSTOP, NULL },
  { BUTTON "Tree\nSel", IDM_ShowDXFTree, 112, 92, 52, 28, WS_TABSTOP, NULL },

  { BUTTON "GCode", IDM_GCode, 4, 122, 52, 28, WS_TABSTOP | WS_GROUP, NULL },
  { BUTTON "Nullp.k", IDM_NULLPUNKT, 58, 122, 52, 28, WS_TABSTOP | WS_GROUP, NULL },
  { BUTTON "Laser", IDM_LASER, 112, 122, 52, 28, WS_TABSTOP | WS_GROUP, NULL },

  { BUTTON "Adjust", IDM_Adjust, 4, 152, 52, 28, WS_GROUP | WS_TABSTOP, NULL },
  { BUTTON "DXF-C", IDM_DXFColor, 58, 152, 52, 28, WS_TABSTOP | WS_GROUP, NULL },
  { BUTTON "*TOP*", IDM_TOPDTOOL, 112, 152, 52, 28, WS_TABSTOP, NULL }, { 0 }
};

const tDialogCtlItem rg_DxfLayer[] = { {
    eDialogBox, "Layer", IDC_DialogBox - 1, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { BUTTON "All", IDC_LayerAll, 5, 2, 158, 18, BS_AUTOCHECKBOX | WS_TABSTOP, NULL }, { 0 }
};

DWORD Farbe[9] = { FL_GRAY, FL_RED, FL_YELLOW, FL_GREEN, FL_CYAN, FL_BLUE, FL_WHITE, FL_LIGHT1, FL_LIGHT3 };
//-------------------------------------------------------------
const tDialogCtlItem rg_DxfColorTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 1, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { AUTORADIOBUTTON "Bohren", IDM_BOHREN, 4, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Fraesen", IDM_FRAESEN, 44, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Spindle", IDM_SPINDLE, 84, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "STOPP", IDM_STOPP, 124, 3, 40, 24, WS_TABSTOP, NULL },

  { AUTORADIOBUTTON "", IDM_Tiefe1, 6, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe2, 32, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe3, 58, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe4, 84, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe5, 110, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe6, 136, 34, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },

#ifdef TOUCHSCREEN
  /*  { CTEXT           "Tempo mm / Sec",-1,    4, 61,42, 24,ES_LEFT|ES_MULTILINE},
    { eCounter,       "",id_ArbeitsTempT,    48, 61,114, 24,WS_TABSTOP| WS_CLIENTEDGE,NULL},
    { CTEXT           "Tiefe  mm",-1,         4, 90,42, 24,ES_LEFT|ES_MULTILINE},
    { eCounter,       "",id_EintauchTiefe1T, 48, 90,114, 24,WS_TABSTOP| WS_CLIENTEDGE,NULL},
    { CTEXT           "Stufe Tiefe mm",-1  ,  4,120,42, 24,ES_LEFT|ES_MULTILINE},
    { eCounter,       "",id_StufenTiefe1T,   48,120,114, 24,WS_TABSTOP| WS_CLIENTEDGE,NULL},
    //{ CTEXT           "Durchm. mm",-1     ,   4,150,42, 24,ES_LEFT|ES_MULTILINE},
    { eCounter,       "",id_FraeserDurchmT,  82,150,80, 24,WS_TABSTOP| WS_CLIENTEDGE,NULL},
  //  { CTEXT           "Drehzahl\nAlu -2ZÃ¤",-1     ,     4,180,42, 24,ES_LEFT|ES_MULTILINE},
  //  { eCounter,       "",IDC_Spindle,        82,180,80, 24,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
    { BUTTON    "TempoCalculator", IDM_Calculator ,82,180,40,22,WS_TABSTOP,NULL},
  */
  { CTEXT "", IDM_LAYER, 58, 60, 100, 18, ES_LEFT | ES_MULTILINE },
  { CTEXT "Tempo mm / Sec", -1, 4, 83, 70, 18, ES_LEFT | ES_MULTILINE },
  { BUTTON "TempoCalculator", IDM_Calculator, 78, 81, 80, 18, WS_TABSTOP, NULL },
  { eInputNum, "", id_ArbeitsTempT, 14, 102, 144, 26, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Tiefe  mm", -1, 14, 132, 144, 11, ES_LEFT | ES_MULTILINE },
  { eInputNum, "", id_EintauchTiefe1T, 14, 142, 144, 26, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Stufe Tiefe mm", -1, 14, 172, 144, 11, ES_LEFT | ES_MULTILINE },
  { eInputNum, "", id_StufenTiefe1T, 14, 182, 144, 26, WS_TABSTOP | WS_CLIENTEDGE, NULL },
//  { BUTTON "TempoCalculator", IDM_Calculator ,45,182,80,22,WS_TABSTOP,NULL},

#else
  { CTEXT "Tempo mm / Sec", -1, 4, 56, 42, 18, ES_LEFT | ES_MULTILINE },
  { EDITTEXT "", id_ArbeitsTempT, 8, 184, 34, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Tiefe  mm", -1, 41, 56, 42, 18, ES_LEFT | ES_MULTILINE },
  { EDITTEXT "", id_EintauchTiefe1T, 48, 184, 34, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Stufe Tiefe mm", -1, 84, 56, 42, 18, ES_LEFT | ES_MULTILINE },
  { EDITTEXT "", id_StufenTiefe1T, 88, 184, 34, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Durchm. mm", -1, 124, 56, 42, 18, ES_LEFT | ES_MULTILINE },
  { EDITTEXT "", id_FraeserDurchmT, 128, 184, 34, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },

  { eSliderCtl, "", id_ArbeitsTemp, 15, 76, 20, 106, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "", id_EintauchTiefe1, 55, 76, 20, 106, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "", id_StufenTiefe1, 95, 76, 20, 106, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "", id_FraeserDurchm, 135, 76, 20, 106, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

#endif
  { 0 }
};
//-------------------------------------------------------------

const tDialogCtlItem rg_CalculatorTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 1, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { BUTTON "Apply", IDM_ApplyValues, 44, 3, 80, 24, WS_TABSTOP, NULL },
  { BUTTON "Back", IDM_DXFColor, 124, 3, 40, 24, WS_TABSTOP, NULL },

  { CTEXT "Tempo mm / Sec", -1, 4, 31, 42, 24, ES_RIGHT | ES_MULTILINE },
  { CTEXT "", id_ArbeitsTempT, 48, 31, 114, 24, ES_LEFT, NULL },
  { CTEXT "Stufe Tiefe mm", -1, 4, 58, 42, 24, ES_RIGHT | ES_MULTILINE },
  { CTEXT "", id_StufenTiefe1T, 48, 58, 114, 24, ES_LEFT, NULL },
  { CTEXT "", IDC_FraeserType, 48, 86, 114, 24, ES_LEFT | ES_MULTILINE },
  { CTEXT "", IDC_Material, 48, 112, 114, 24, ES_LEFT | ES_MULTILINE },
  { CTEXT "Durchm. mm", -1, 4, 140, 42, 24, ES_RIGHT | ES_MULTILINE },
  { eInputNum, "", id_FraeserDurchmT, 48, 140, 114, 24, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "", IDM_MaxSpindleRPM, 48, 166, 114, 14, ES_LEFT, NULL },
  { CTEXT "Drehzahl", -1, 4, 180, 42, 24, ES_LEFT | ES_MULTILINE },
  { eInputNum, "", IDC_Spindle, 48, 180, 114, 24, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP }, { 0 }
};
//-------------------------------------------------------------

const tDialogCtlItem rg_DXFtoCNCTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 1, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { AUTORADIOBUTTON "Bohren", IDM_BOHREN, 4, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Fraesen", IDM_FRAESEN, 44, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Spindle", IDM_SPINDLE, 84, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "STOPP", IDM_STOPP, 124, 3, 40, 24, WS_TABSTOP, NULL },
  { CTEXT "Tempo mm / Sec", -1, 4, 30, 42, 22, ES_RIGHT | ES_MULTILINE },
  { eInputNum, "", id_ArbeitsTempT, 48, 30, 114, 22, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Tiefe  mm", -1, 4, 52, 42, 22, ES_LEFT | ES_MULTILINE },
  { eInputNum, "", id_EintauchTiefe1T, 48, 52, 114, 22, WS_TABSTOP | WS_CLIENTEDGE, NULL },

  { CTEXT "Stufe Tiefe mm", -1, 4, 74, 42, 22, ES_RIGHT | ES_MULTILINE },
  { eInputNum, "", id_StufenTiefe1T, 48, 74, 114, 22, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "", IDC_FraeserType, 48, 96, 114, 22, ES_LEFT | ES_MULTILINE },
  { CTEXT "", IDC_Material, 48, 118, 114, 22, ES_LEFT | ES_MULTILINE },
  { CTEXT "Durchm. mm", -1, 4, 140, 42, 22, ES_RIGHT | ES_MULTILINE },
  { eInputNum, "", id_FraeserDurchmT, 48, 140, 114, 22, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "", IDM_MaxSpindleRPM, 48, 162, 114, 14, ES_LEFT, NULL },

  { AUTORADIOBUTTON "Auf dem\nStrich", IDC_AufdemStrich, 4, 162, 52, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Gegenlauf", IDC_Gegenlauf, 57, 162, 52, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Gleichlauf", IDC_Gleichlauf, 110, 162, 52, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { CTEXT "Drehzahl", -1, 4, 184, 42, 22, ES_LEFT | ES_MULTILINE },
  { eInputNum, "", IDC_Spindle, 48, 184, 114, 22, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP }, { 0 }
};
//-------------------------------------------------------------
const tDialogCtlItem rg_NullpunktTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 2, 4, 0, 80, 220,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 219, WS_DLGFRAME, NULL },
  { AUTORADIOBUTTON "GeZu\nXY 0", IDC_GehZuNULL, 4, 3, 44, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Grenzen\nAbfahren", IDM_FAHRGRENZEN, 55, 3, 44, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "STOPP", IDM_STOPP, 110, 3, 56, 24, WS_TABSTOP, NULL },

  { BUTTON "Reset", IDM_Refernezfahrt, 66, 32, 96, 24, WS_TABSTOP, NULL },

//  { AUTORADIOBUTTON "Drehen",id_Drehen,    4, 32,80,18,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP,NULL},
//  { AUTORADIOBUTTON "Spiegeln",ID_AufKopf,84, 32,80,18,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP,NULL},
#ifdef TOUCHSCREEN
  { BUTTON "Adjust", IDM_Adjust, 4, 60, 44, 24, WS_TABSTOP, NULL },
  { BUTTON "XY Ubernehmen", id_XYNull, 66, 60, 96, 24, WS_TABSTOP, NULL },
  { CTEXT "X Null", -1, 4, 88, 58, 24, ES_RIGHT },
  { eInputNum, "", id_XWerNullT, 66, 88, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { CTEXT "Y Null", -1, 4, 112, 58, 24, ES_RIGHT },
  { eInputNum, "", id_YWerNullT, 66, 112, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { BUTTON "Z Ubernehmen", id_ZNull, 66, 142, 96, 24, WS_TABSTOP, NULL },
  { CTEXT "Z Null", -1, 4, 169, 58, 24, ES_RIGHT },
  { eInputNum, "", id_ZWerNullT, 66, 169, 96, 24, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Sensor XYZ", -1, 4, 200, 58, 12, ES_RIGHT },
#else
  { BUTTON "XY Ubernehmen", id_XYNull, 4, 76, 98, 21, WS_TABSTOP, NULL },
  { CTEXT "X Null", -1, 4, 103, 32, 8, ES_RIGHT },
  { EDITTEXT "", id_XWerNullT, 41, 98, 58, 21, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { CTEXT "Y Null", -1, 4, 125, 32, 8, ES_RIGHT },
  { EDITTEXT "", id_YWerNullT, 41, 121, 58, 18, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { BUTTON "Z Ubernehmen", id_ZNull, 4, 146, 98, 21, WS_TABSTOP, NULL },
  { CTEXT "Z Null", -1, 4, 172, 32, 8, ES_RIGHT },
  { EDITTEXT "", id_ZWerNullT, 41, 169, 58, 21, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Tempo mm / Min", -1, 114, 56, 42, 18, ES_LEFT | ES_MULTILINE },
  { eSliderCtl, "", id_ArbeitsTemp, 125, 76, 20, 106, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { CTEXT "Sensor XYZ", -1, 5, 193, 32, 12, ES_RIGHT },
#endif
  { CTEXT "", IDC_WekzeugX, 60, 200, 28, 12, WS_TABSTOP, NULL },
  { CTEXT "", IDC_WekzeugY, 93, 200, 28, 12, WS_TABSTOP, NULL },
  { CTEXT "", IDC_WekzeugZ, 126, 200, 28, 12, WS_TABSTOP, NULL },

  { 0 }
};
//-------------------------------------------------------------
#define BSIZE 26
#define XCOL1 4
#define XCOL2 (XCOL1 + (BSIZE * 1))
#define XCOL3 (XCOL1 + (BSIZE * 2))
#define XCOL4 (XCOL1 + (BSIZE * 3))
#define XCOL5 (XCOL1 + (BSIZE * 4))

#define XCOLZ (XCOL1 + (BSIZE * 5 + 6))

#define YROW1 50
#define YROW2 (YROW1 + (BSIZE * 1))
#define YROW3 (YROW1 + (BSIZE * 2))
#define YROW4 (YROW1 + (BSIZE * 3))
#define YROW5 (YROW1 + (BSIZE * 4))

#define YROW6 (YROW1 + (BSIZE * 5) + 8)

const tDialogCtlItem rg_ToolAdjustTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 2, 4, 0, 80, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { BUTTON "GOTO\nORIGIN", IDC_GehZuNULL, 4, 3, 35, 24, WS_TABSTOP, NULL },
  { BUTTON "SET X,Y\nORIGIN", id_XYNull, 39, 3, 35, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "SET Z\nORIGIN", id_ZNull, 74, 3, 35, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { BUTTON "TOOL ADJ.", IDC_AdjustHeight, 110, 3, 56, 24, WS_TABSTOP, NULL },
  { CTEXT "", IDC_WekzeugX, 4, 28, 48, 18, WS_BORDER | WS_TABSTOP, NULL },
  { CTEXT "", IDC_WekzeugY, 55, 28, 48, 18, WS_BORDER | WS_TABSTOP, NULL },
  { CTEXT "", IDC_WekzeugZ, 120, 28, 48, 18, WS_BORDER | WS_TABSTOP, NULL },

  { CONTROL "@+53<<", IDC_MoveXYMP10, XCOL1, YROW1, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52<<", IDC_MoveYP10, XCOL3, YROW1, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+51<<", IDC_MoveXYPP10, XCOL5, YROW1, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { CONTROL "@+53<", IDC_MoveXYMP1, XCOL2, YROW2, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52<", IDC_MoveYP1, XCOL3, YROW2, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+51<", IDC_MoveXYPP1, XCOL4, YROW2, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { CONTROL "@+5<<", IDC_MoveXM10, XCOL1, YROW3, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+5<", IDC_MoveXM1, XCOL2, YROW3, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+5||", IDM_STOPP, XCOL3, YROW3, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+5>", IDC_MoveXP1, XCOL4, YROW3, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+5>>", IDC_MoveXP10, XCOL5, YROW3, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { CONTROL "@+51>", IDC_MoveXYMM1, XCOL2, YROW4, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52>", IDC_MoveYM1, XCOL3, YROW4, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+53>", IDC_MoveXYPM1, XCOL4, YROW4, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { CONTROL "@+51>>", IDC_MoveXYMM10, XCOL1, YROW5, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52>>", IDC_MoveYM10, XCOL3, YROW5, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+53>>", IDC_MoveXYPM10, XCOL5, YROW5, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { CONTROL "@+52|<", IDC_MoveZP10, XCOLZ, YROW1, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52<-", IDC_MoveZP1, XCOLZ, YROW2, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52->", IDC_MoveZM1, XCOLZ, YROW4, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },
  { CONTROL "@+52>|", IDC_MoveZM10, XCOLZ, YROW5, BSIZE, BSIZE, WS_TABSTOP | WS_MAXIMIZE, NULL },

  { AUTORADIOBUTTON "10mm", IDC_Move1000, 4, YROW6, 40, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "1mm", IDC_Move0100, 44, YROW6, 40, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "0.1mm", IDC_Move0010, 84, YROW6, 40, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  {
    AUTORADIOBUTTON "0.01mm", IDC_Move0001, 124, YROW6, 41, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },

  { 0 }
};
//-------------------------------------------------------------
const tDialogCtlItem rg_SettingsTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { AUTORADIOBUTTON "RS232", IDC_Isel, 5, 3, 44, 24, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  {
    AUTORADIOBUTTON "Sauger", IDM_Sauger, 55, 3, 44, 24,
    BS_AUTORADIOBUTTON | WS_TABSTOP | BS_AUTOCHECKBOX | WS_DISABLED, NULL
  },
  { BUTTON "STOPP", IDM_STOPP, 110, 3, 54, 24, WS_TABSTOP, NULL },
#ifdef TOUCHSCREEN
  { CTEXT "Positonieren\nmm/Min", -1, 4, 34, 64, 24, ES_RIGHT },
  { eInputNum, "", id_VerfahrTempT, 66, 34, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },

  { CTEXT "Eintauch Tempo\nmm/Min", -1, 4, 67, 64, 24, ES_CENTER },
  {
    eInputNum, "", id_EintauchTempo1T, 66, 67, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/,
    NULL
  },

  { CTEXT "Bohren Tempo\nmm/Min", -1, 4, 100, 64, 24, ES_CENTER },
  { eInputNum, "", id_BohrTempo1T, 66, 100, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },

  { CTEXT "Abheben (Alle)\nmm", -1, 4, 133, 64, 24, ES_CENTER },
  { eInputNum, "", id_HebenT, 66, 133, 96, 24, WS_TABSTOP | WS_CLIENTEDGE /*| ES_AUTOHSCROLL*/, NULL },
#else
  { CTEXT "Positonieren", -1, 4, 34, 64, 8, ES_RIGHT },
  { EDITTEXT "", id_VerfahrTempT, 14, 44, 47, 21, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { CTEXT "mm/Min", -1, 67, 45, 30, 18, ES_LEFT | ES_MULTILINE },

  { CTEXT "Eintauch Tempo", -1, 4, 67, 64, 8, ES_CENTER },
  { EDITTEXT "", id_EintauchTempo1T, 14, 77, 47, 21, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { CTEXT "mm/Min", -1, 67, 78, 30, 18, ES_LEFT | ES_MULTILINE },

  { CTEXT "Bohren Tempo", -1, 4, 100, 64, 8, ES_CENTER },
  { EDITTEXT "", id_BohrTempo1T, 14, 110, 47, 21, WS_TABSTOP | WS_CLIENTEDGE /*ES_CENTER | ES_AUTOHSCROLL*/, NULL },
  { CTEXT "mm/Min", -1, 67, 111, 30, 18, ES_LEFT | ES_MULTILINE },

  { CTEXT "Abheben (Alle)", -1, 4, 133, 64, 8, ES_CENTER },
  { EDITTEXT "", id_HebenT, 14, 143, 47, 21, WS_TABSTOP | WS_CLIENTEDGE /*| ES_AUTOHSCROLL*/, NULL },
  { CTEXT "mm", -1, 67, 144, 30, 8, ES_LEFT },

  { eSliderCtl, "Tempo", id_VerfahrTemp, 108, 44, 25, 122, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Spindle", IDC_Spindle, 138, 44, 25, 122, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
#endif

  {
    AUTORADIOBUTTON "Aufrauemen", id_Aufrauemen, 34, 165, 60, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL },
  {
    AUTORADIOBUTTON "AutoSauger", IDC_SaugerAuto, 100, 165, 60, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },

  { CTEXT "Msg Prefetch", -1, 4, 189, 40, 18, ES_CENTER },
  {
    AUTORADIOBUTTON "Auto", IDC_GcodeModusAuto, 40, 189, 39, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "No Stock", IDC_GcodeModus1, 80, 189, 39, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "5 Cmnd's", IDC_GcodeModus5, 120, 189, 39, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },

  { 0 }
};

//-------------------------------------------------------------
const tDialogCtlItem rg_LaserTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },

  { AUTORADIOBUTTON "START", IDM_LASERENGRAVE, 5, 3, 40, 24, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  { BUTTON "STOPP", IDM_STOPP, 124, 3, 40, 24, WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe1, 6, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe2, 32, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe3, 58, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe4, 84, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe5, 110, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "", IDM_Tiefe6, 136, 30, 26, 22, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },

  { CTEXT "LASER POWER", -1, 10, 54, 54, 8, ES_RIGHT },
  {
    AUTORADIOBUTTON "Focus  ", IDC_FocusPowerOnF, 7, 69, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "Work", IDC_LaserPowerOnF, 42, 69, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  { AUTORADIOBUTTON "OFF", IDC_PowerOff, 77, 69, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  { eSliderCtl, "Focus", IDC_FocusPower, 10, 90, 20, 82, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Work", IDC_LaserPower, 45, 90, 20, 82, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { CTEXT "SPEED", -1, 120, 54, 29, 8, ES_RIGHT },
  { eSliderCtl, "Speed", IDC_LaserSpeed, 123, 90, 20, 82, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { EDITTEXT "", IDC_FocusPowerT, 7, 177, 29, 21, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { EDITTEXT "", IDC_LaserPowerT, 42, 177, 29, 21, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { EDITTEXT "", IDC_LaserSpeedT, 120, 177, 29, 21, WS_TABSTOP | WS_CLIENTEDGE, NULL }, { 0 }
};

//-------------------------------------------------------------
const tDialogCtlItem rg_GcodeTab[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },
  { AUTORADIOBUTTON "RUN", IDM_RUNGCODE, 4, 3, 40, 24, BS_AUTORADIOBUTTON | WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Spindle", IDM_SPINDLE, 44, 3, 40, 24, BS_AUTORADIOBUTTON |  BS_AUTOCHECKBOX |WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Laser", IDM_LASERENGRAVE,84, 3, 40, 24, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },

  { BUTTON "STOPP", IDM_STOPP,124, 3, 40, 24, WS_TABSTOP, NULL },

  { AUTORADIOBUTTON "",IDM_Tiefe1, 6,30,26,22,BS_AUTORADIOBUTTON |  WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "",IDM_Tiefe2,32,30,26,22,BS_AUTORADIOBUTTON |  WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "",IDM_Tiefe3,58,30,26,22,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "",IDM_Tiefe4,84,30,26,22,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "",IDM_Tiefe5,110,30,26,22,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "",IDM_Tiefe6,136,30,26,22,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},

  { CTEXT "GCODE MODIFY PERCENT", -1, 6, 55, 124, 8, ES_RIGHT },
  {
    AUTORADIOBUTTON "G0 Rapid", IDC_RapidOverwrite, 4, 67, 39, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "G1 Feed", IDC_FeedOverwrite, 44, 67, 39, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "Laser Power", IDC_PowerOverwrite, 84, 67, 39, 18,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  { eSliderCtl, "Rapid", IDC_GcodeRapid, 13, 90, 20, 37, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Feed", IDC_GcodeFeed, 53, 90, 20, 37, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Power", IDC_PowerPercent, 93, 90, 20, 37, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  //  { CTEXT    "PERCENT OF GC",-1, 14,163,110,8,ES_RIGHT},
  { EDITTEXT "", IDC_GcodeRapidT, 4, 131, 39, 16, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { EDITTEXT "", IDC_GcodeFeedT, 44, 131, 39, 16, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { EDITTEXT "", IDC_PowerPercentT, 84, 131, 39, 16, WS_TABSTOP | WS_CLIENTEDGE, NULL },

  { AUTORADIOBUTTON "AB+XY Swap", IDC_AB_SWAP, 4, 155, 39, 22, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  {
    AUTORADIOBUTTON "Drehachse", IDC_Drehachse, 44, 155, 39, 22, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "Hide G0", IDC_HideRapidTrans, 84, 155, 39, 22, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  { CTEXT "DIAMETER", -1, 1, 180, 44, 24, ES_RIGHT },
  { eInputNum, "", IDC_Durchmesser, 44, 180, 80, 24, WS_TABSTOP | WS_CLIENTEDGE, NULL },

//  { eSliderCtl, "Start", IDC_GCODEPosStart, 126,55, 18, 146,BS_AUTOCHECKBOX | WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
//  { eSliderCtl, "Ende",  IDC_GCODEPosEnd, 146, 55, 18, 146, BS_AUTOCHECKBOX | WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { CTEXT "N-Ende", -1, 126, 55, 38, 8, ES_RIGHT },
  { CTEXT "N-Start", -1, 126,172, 38, 8, ES_RIGHT },
  { EDITTEXT    "", IDC_GCODEEndTxt, 126,67, 38, 18, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { EDITTEXT    "", IDC_GCODEStartTxt, 126,183, 38, 18, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Start", IDC_GCODEPosStart, 126,90, 18, 80, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Ende",  IDC_GCODEPosEnd, 146, 90, 18, 80,  WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { 0 }
};


#define SLIDERH 50
#define ZOOM_Y 2         // 112
#define FIXP_Y 114 - 84  // 140
#define CENT_Y 266 - 84  //  65
#define EYE__Y2 326 - 84 //  65
#define ROTA_Y 386 - 84  //  65
#define PERSPY 436 - 84  //  65
#define LIGHTP_Y 520 - 87
#define LIGHTC_Y 584 - 90

const tDialogCtlItem rg_Perspektive[] = { {
    eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192,
    WS_BORDER | WS_TABSTOP
  }, // Dialog

  { CTEXT "Centre", -1, 2, CENT_Y - 8, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_CenterX, 2, CENT_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_CenterY, 17, CENT_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_CenterZ, 32, CENT_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  //  { CTEXT           "Eye",        -1,            2,EYE__Y2-8 ,45,      8,ES_CENTER},
  //  { eSliderCtl,     "X",        IDC_EyeX       , 2,EYE__Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
  //  { eSliderCtl,     "Y",        IDC_EyeY       ,17,EYE__Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
  //  { eSliderCtl,     "Z",        IDC_EyeZ       ,32,EYE__Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
  //
  { CTEXT "3Tafel", -1, 2, EYE__Y2 - 8, 45, 8, ES_CENTER },
  { eSliderCtl, "Z", IDC_EyeX, 2, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Gr", IDC_EyeY, 17, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Tr", IDC_EyeZ, 32, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { CTEXT "Rotation", -1, 2, ROTA_Y - 8, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_RotationX, 2, ROTA_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_RotationY, 17, ROTA_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_RotationZ, 32, ROTA_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { CTEXT "Perspective", -1, 2, PERSPY + 0, 45, 8, ES_CENTER },
  { eSliderCtl, "", IDC_Perspective, 2, PERSPY + 8, 45, 14, WS_CLIENTEDGE | WS_HSCROLL | WS_TABSTOP },

  //  { BUTTON          "",         IDM_ZoomMax,    2,ZOOM_Y+  0,45,28,WS_TABSTOP,NULL},
  //  { BUTTON          "",         IDM_Zoom2,      2,ZOOM_Y+ 28,45,28,WS_TABSTOP,NULL},
  { BUTTON "", IDM_ZoomGanz, 2, ZOOM_Y + 0, 45, 28, WS_TABSTOP, NULL },
  //  { BUTTON          ""   ,      IDM_Zoom1,      2,ZOOM_Y+ 84,45,28,WS_TABSTOP,NULL},
  { BUTTON "GetDefault", IDC_GetDefault, 2, FIXP_Y + 0, 45, 28, WS_TABSTOP, NULL },
  { BUTTON "Top", IDC_ShowTop, 2, FIXP_Y + 28, 45, 28, WS_TABSTOP, NULL },
  { BUTTON "Bottom", IDC_ShowBottom, 2, FIXP_Y + 56, 45, 12, WS_TABSTOP, NULL },
  { BUTTON "EyeDef", IDC_ShowEyeDef, 2, FIXP_Y + 68, 45, 12, WS_TABSTOP, NULL },
  { BUTTON "Left", IDC_ShowLeft, 2, FIXP_Y + 80, 45, 12, WS_TABSTOP, NULL },
  { BUTTON "Right", IDC_ShowRight, 2, FIXP_Y + 92, 45, 12, WS_TABSTOP, NULL },
  { BUTTON "Front", IDC_ShowFront, 2, FIXP_Y + 104, 45, 12, WS_TABSTOP, NULL },
  { BUTTON "Back", IDC_ShowBack, 2, FIXP_Y + 116, 45, 12, WS_TABSTOP, NULL },
  //  { BUTTON       "SetDefault",	IDC_SetDefault ,2,FIXP_Y+128,45,12,WS_TABSTOP|WS_DISABLED,NULL},

  { AUTORADIOBUTTON "1", IDC_LookAt1, 2, PERSPY + 22, 11, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL },
  {
    AUTORADIOBUTTON "2", IDC_LookAt2, 2 + 11, PERSPY + 22, 11, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "3", IDC_LookAt3, 2 + 22, PERSPY + 22, 11, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "4", IDC_LookAt4, 2 + 33, PERSPY + 22, 11, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },

  {
    AUTORADIOBUTTON "Clip", IDC_LookClip, 2, PERSPY + 34, 45, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "3DFace", IDC_Show3DFace, 2, PERSPY + 47, 45, 12, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,
    NULL
  },
  {
    AUTORADIOBUTTON "Background", IDC_Background, 2, PERSPY + 60, 45, 12,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },

  { CTEXT "LightPos", -1, 2, LIGHTP_Y - 8, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_LightPosX, 2, LIGHTP_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_LightPosY, 17, LIGHTP_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_LightPosZ, 32, LIGHTP_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { CTEXT "Light Val", -1, 2, LIGHTC_Y - 8, 45, 8, ES_CENTER },
  { eSliderCtl, "Po", IDC_LightColorR, 2, LIGHTC_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Am", IDC_LightColorG, 17, LIGHTC_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Bk", IDC_LightColorB, 32, LIGHTC_Y, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { 0 }
};

#define WERASTERX 164 / 6
#define TOP 2
#define WERASTERY 27
#define WEWIDTH WERASTERX
#define WEHEIGHT WERASTERY

#define SLIDERH 50
#define ZOOM_Y 2 //    2  // 112
//#define FIXP_Y 18//  114  // 140
#define CENT_Y2 70  //  266  //  65
#define EYE__Y 132  //  326  //  65
#define ROTA_Y2 132 //  386  //  65
#define PERSPY2 186 //  436  //  65

#define LIGHTP_Y2 70
#define LIGHTC_Y2 132
// const tDialogCtlItem  rg_Perspektive2[]=
//{
//  { eDialogBox,"Tool",IDC_DialogBox-3, 4,  2,90,192,WS_BORDER| WS_TABSTOP},                        // Dialog
//
//  { BUTTON          "Top",	    IDC_ShowTop    ,1+ WERASTERX*0,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          "Bottom",	IDC_ShowBottom ,1+ WERASTERX*1,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          "Left",	    IDC_ShowLeft   ,1+ WERASTERX*2,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          "Right",	IDC_ShowRight  ,1+ WERASTERX*3,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          "Front",	IDC_ShowFront  ,1+ WERASTERX*4,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          "Back",	    IDC_ShowBack   ,1+ WERASTERX*5,TOP+WERASTERY*0 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//
//  { BUTTON          "",         IDM_ZoomGanz,   1+ WERASTERX*0,TOP+WERASTERY*1 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON          ""       ,  IDM_Zoom1,      1+ WERASTERX*1,TOP+WERASTERY*1 ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL},
//  { BUTTON       "Get Default",	IDC_GetDefault ,1+ WERASTERX*2,TOP+WERASTERY*1
//  ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL}, { BUTTON          "EyeDef",	IDC_ShowEyeDef ,1+ WERASTERX*3,TOP+WERASTERY*1
//  ,WEWIDTH,WEHEIGHT,WS_TABSTOP,NULL}, { BUTTON       "",            IDC_ModeFullscreen,1+ WERASTERX*4,TOP+WERASTERY*1
//  ,WEWIDTH,WEHEIGHT,BS_AUTORADIOBUTTON | WS_TABSTOP,NULL}, { AUTORADIOBUTTON "Touch Screen",	IDC_TouchScreen,1+
//  WERASTERX*5,TOP+WERASTERY*1 ,WEWIDTH,WEHEIGHT,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
//
//  { CTEXT           "Centre",     -1,            5+ WERASTERX*0,CENT_Y2-9 ,45        ,8,ES_CENTER},
//  { eSliderCtl,     "X",        IDC_CenterX    , 5+ WERASTERX*0,CENT_Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "Y",        IDC_CenterY    ,20+ WERASTERX*0,CENT_Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "Z",        IDC_CenterZ    ,35+ WERASTERX*0,CENT_Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//  { CTEXT           "Rotation",   -1,            5+ WERASTERX*0,ROTA_Y2-9 ,45,      8,ES_CENTER},
//  { eSliderCtl,     "X",        IDC_RotationX  , 5+ WERASTERX*0,ROTA_Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "Y",        IDC_RotationY  ,20+ WERASTERX*0,ROTA_Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "Z",        IDC_RotationZ  ,35+ WERASTERX*0,ROTA_Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//  { CTEXT           "Eye",        -1,            2+ WERASTERX*2,EYE__Y2-9 ,45,      8,ES_CENTER},
//  { eSliderCtl,     "X",        IDC_EyeX       , 2+ WERASTERX*2,EYE__Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "Y",        IDC_EyeY       ,17+ WERASTERX*2,EYE__Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "Z",        IDC_EyeZ       ,32+ WERASTERX*2,EYE__Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//
//
//
//
////  { AUTORADIOBUTTON "1",     	IDC_LookAt1    ,5+ WERASTERX*2    ,CENT_Y +0,11,12,BS_AUTORADIOBUTTON |
/// BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, /  { AUTORADIOBUTTON "2",     	IDC_LookAt2    ,5+ WERASTERX*2+11 ,CENT_Y
///+0,11,12,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
////  { AUTORADIOBUTTON "3",     	IDC_LookAt3    ,5+ WERASTERX*2+22 ,CENT_Y +0,11,12,BS_AUTORADIOBUTTON |
/// BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, /  { AUTORADIOBUTTON "4",     	IDC_LookAt4    ,5+ WERASTERX*2+33 ,CENT_Y
///+0,11,12,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
//
//
//  { AUTORADIOBUTTON "Clip", 	IDC_LookClip   ,5+ WERASTERX*2    ,CENT_Y2    ,45,12,BS_AUTORADIOBUTTON |
//  BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, { AUTORADIOBUTTON "3DFace", 	IDC_Show3DFace ,5+ WERASTERX*2    ,CENT_Y2
//  +13,45,12,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
////  { AUTORADIOBUTTON "Background",IDC_Background,5+ WERASTERX*2    ,CENT_Y2 +34,45,16,BS_AUTORADIOBUTTON |
/// BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
//  { CTEXT        "Perspective",   -1,            5+ WERASTERX*2,CENT_Y2 +26,45,      8,ES_CENTER},
//  { eSliderCtl,      "",        IDC_Perspective, 5+ WERASTERX*2,CENT_Y2 +34,45,14,WS_CLIENTEDGE| WS_HSCROLL |
//  WS_TABSTOP},
//
//
//  { CTEXT           "LightPos", -1,             5+ WERASTERX*4+2,LIGHTP_Y2-9 ,45,      8,ES_CENTER},
//  { eSliderCtl,     "X",        IDC_LightPosX  ,5+ WERASTERX*4+2,LIGHTP_Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "Y",        IDC_LightPosY  ,5+ WERASTERX*4+17,LIGHTP_Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "Z",        IDC_LightPosZ  ,5+ WERASTERX*4+32,LIGHTP_Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//  { CTEXT           "Ambiente", -1,             5+ WERASTERX*2+2,LIGHTC_Y2-9 ,45,      8,ES_CENTER},
//  { eSliderCtl,     "Tr",      IDC_Transparency,5+ WERASTERX*2+2,LIGHTC_Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "Gr",        IDC_GrayOut   ,5+ WERASTERX*2+17,LIGHTC_Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "Bk",        IDC_BackValue ,5+ WERASTERX*2+32,LIGHTC_Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//
//
//  { CTEXT           "Light Val", -1,             5+ WERASTERX*4+2,LIGHTC_Y2-9 ,45,      8,ES_CENTER},
//  { eSliderCtl,     "R",         IDC_LightColorR,5+ WERASTERX*4+2,LIGHTC_Y2   ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL |
//  WS_TABSTOP}, { eSliderCtl,     "G",         IDC_LightColorG,5+ WERASTERX*4+17,LIGHTC_Y2   ,15,SLIDERH,WS_CLIENTEDGE|
//  WS_VSCROLL | WS_TABSTOP}, { eSliderCtl,     "B",         IDC_LightColorB,5+ WERASTERX*4+32,LIGHTC_Y2
//  ,15,SLIDERH,WS_CLIENTEDGE| WS_VSCROLL | WS_TABSTOP},
//
//  { AUTORADIOBUTTON "1",     	IDC_LookAt1    ,10+ WERASTERX*0,PERSPY2 ,WEWIDTH,WEHEIGHT-4,BS_AUTORADIOBUTTON |
//  BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, { AUTORADIOBUTTON "2",     	IDC_LookAt2    ,10+ WERASTERX*1,PERSPY2
//  ,WEWIDTH,WEHEIGHT-4,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
//  { AUTORADIOBUTTON "3",     	IDC_LookAt3    ,10+ WERASTERX*2,PERSPY2 ,WEWIDTH,WEHEIGHT-4,BS_AUTORADIOBUTTON |
//  BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, { AUTORADIOBUTTON "4",     	IDC_LookAt4    ,10+ WERASTERX*3,PERSPY2
//  ,WEWIDTH,WEHEIGHT-4,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
//  { BUTTON       "Save",	    IDC_SetDefault ,10+ WERASTERX*4+10,PERSPY2 ,WEWIDTH,WEHEIGHT-4,WS_TABSTOP,NULL},
//
///*
//
//  { BUTTON          "",         IDM_ZoomMax,    2,ZOOM_Y+  0,45,28,WS_TABSTOP,NULL},
//  { BUTTON          "",         IDM_Zoom2,      2,ZOOM_Y+ 28,45,28,WS_TABSTOP,NULL},
//  { BUTTON          ""  ,       IDM_ZoomGanz,   2,ZOOM_Y+ 56,45,28,WS_TABSTOP,NULL},
//  { BUTTON          ""   ,      IDM_Zoom1,      2,ZOOM_Y+ 84,45,28,WS_TABSTOP,NULL},
//  { BUTTON       "GetDefault",	IDC_GetDefault ,2,FIXP_Y+  0,45,28,WS_TABSTOP,NULL},
//  { BUTTON          "Top",	    IDC_ShowTop    ,2,FIXP_Y+ 28,45,28,WS_TABSTOP,NULL},
//  { BUTTON          "Bottom",	IDC_ShowBottom ,2,FIXP_Y+ 56,45,12,WS_TABSTOP,NULL},
//  { BUTTON          "EyeDef",	IDC_ShowEyeDef ,2,FIXP_Y+ 68,45,12,WS_TABSTOP,NULL},
//  { BUTTON          "Left",	    IDC_ShowLeft   ,2,FIXP_Y+ 80,45,12,WS_TABSTOP,NULL},
//  { BUTTON          "Right",	IDC_ShowRight  ,2,FIXP_Y+ 92,45,12,WS_TABSTOP,NULL},
//  { BUTTON          "Front",	IDC_ShowFront  ,2,FIXP_Y+104,45,12,WS_TABSTOP,NULL},
//  { BUTTON          "Back",	    IDC_ShowBack   ,2,FIXP_Y+116,45,12,WS_TABSTOP,NULL},
//  { BUTTON       "SetDefault",	IDC_SetDefault ,2,FIXP_Y+128,45,12,WS_TABSTOP|WS_DISABLED,NULL},
//*/
//
//  {0}
//};
const tDialogCtlItem rg_Perspektive2[] = {
  { eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192, WS_BORDER | WS_TABSTOP }, // Dialog
  { CONTROL "", -1, 2, 0, 164, 300, WS_DLGFRAME, NULL },

  { BUTTON "Top", IDC_ShowTop, 3+ WERASTERX * 0, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Bottom", IDC_ShowBottom, 3+ WERASTERX * 1, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Left", IDC_ShowLeft, 3+ WERASTERX * 2, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Right", IDC_ShowRight, 3+ WERASTERX * 3, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Front", IDC_ShowFront, 3+ WERASTERX * 4, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Back", IDC_ShowBack, 3+ WERASTERX * 5, TOP + WERASTERY * 0, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },

  { BUTTON "", IDM_ZoomGanz, 3+ WERASTERX * 0, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "", IDM_Zoom1, 3+ WERASTERX * 1, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Get Default", IDC_GetDefault, 3+ WERASTERX * 2, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "EyeDef", IDC_ShowEyeDef, 3+ WERASTERX * 3, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT, WS_TABSTOP, NULL },
  {
    BUTTON "", IDC_ModeFullscreen, 3+ WERASTERX * 4, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT,
    BS_AUTORADIOBUTTON | WS_TABSTOP, NULL
  },
  {
    AUTORADIOBUTTON "Touch Screen", IDC_TouchScreen, 3+ WERASTERX * 5, TOP + WERASTERY * 1, WEWIDTH, WEHEIGHT,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },

  { CTEXT "Centre", -1, 5 + WERASTERX * 0, CENT_Y2 - 9, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_CenterX, 5 + WERASTERX * 0, CENT_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_CenterY, 20 + WERASTERX * 0, CENT_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_CenterZ, 35 + WERASTERX * 0, CENT_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { CTEXT "Rotation", -1, 5 + WERASTERX * 0, ROTA_Y2 - 9, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_RotationX, 5 + WERASTERX * 0, ROTA_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_RotationY, 20 + WERASTERX * 0, ROTA_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_RotationZ, 35 + WERASTERX * 0, ROTA_Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  {
    BUTTON "Move XY", IDC_MausMoveXY, 1 + WERASTERX * 4, CENT_Y2 + WERASTERY * 0, WEWIDTH * 2, WEHEIGHT, WS_TABSTOP,
    NULL
  },
  {
    BUTTON "Zoom MZ", IDC_MausMovez, 1 + WERASTERX * 4, CENT_Y2 + WERASTERY * 1, WEWIDTH * 2, WEHEIGHT, WS_TABSTOP,
    NULL
  },
  {
    BUTTON "RotateXY", IDC_RotateXY, 1 + WERASTERX * 4, CENT_Y2 + WERASTERY * 2, WEWIDTH * 2, WEHEIGHT, WS_TABSTOP,
    NULL
  },
  {
    BUTTON "SelectXY", IDC_SelectXY, 1 + WERASTERX * 4, CENT_Y2 + WERASTERY * 3, WEWIDTH * 2, WEHEIGHT, WS_TABSTOP,
    NULL
  },

  //  { AUTORADIOBUTTON "1",     	IDC_LookAt1    ,5+ WERASTERX*2    ,CENT_Y +0,11,12,BS_AUTORADIOBUTTON |
  //  BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, { AUTORADIOBUTTON "2",     	IDC_LookAt2    ,5+ WERASTERX*2+11 ,CENT_Y
  //  +0,11,12,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
  //  { AUTORADIOBUTTON "3",     	IDC_LookAt3    ,5+ WERASTERX*2+22 ,CENT_Y +0,11,12,BS_AUTORADIOBUTTON |
  //  BS_AUTOCHECKBOX|WS_TABSTOP,NULL}, { AUTORADIOBUTTON "4",     	IDC_LookAt4    ,5+ WERASTERX*2+33 ,CENT_Y
  //  +0,11,12,BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX|WS_TABSTOP,NULL},

  {
    AUTORADIOBUTTON "Clip", IDC_LookClip, 5 + WERASTERX * 2, CENT_Y2, 45, 12,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  {
    AUTORADIOBUTTON "3DFace", IDC_Show3DFace, 5 + WERASTERX * 2, CENT_Y2 + 13, 45, 12,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  //  { AUTORADIOBUTTON "Background",IDC_Background,5+ WERASTERX*2    ,CENT_Y2 +34,45,16,BS_AUTORADIOBUTTON |
  //  BS_AUTOCHECKBOX|WS_TABSTOP,NULL},
  { CTEXT "Perspective", -1, 5 + WERASTERX * 2, CENT_Y2 + 26, 45, 8, ES_CENTER },
  { eSliderCtl, "", IDC_Perspective, 5 + WERASTERX * 2, CENT_Y2 + 34, 45, 14, WS_CLIENTEDGE | WS_HSCROLL | WS_TABSTOP },

  { CTEXT "Ambiente", -1, 5 + WERASTERX * 2 + 2, LIGHTC_Y2 - 9, 45, 8, ES_CENTER },
  {
    eSliderCtl, "Tr", IDC_Transparency, 5 + WERASTERX * 2 + 2, LIGHTC_Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "Gr", IDC_GrayOut, 5 + WERASTERX * 2 + 17, LIGHTC_Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "Bk", IDC_BackValue, 5 + WERASTERX * 2 + 32, LIGHTC_Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },

  { CTEXT "Eye", -1, 3 + WERASTERX * 0, EYE__Y2 - 9, 45, 8, ES_CENTER },
  { eSliderCtl, "X", IDC_EyeX, 4 + WERASTERX * 0, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Y", IDC_EyeY, 19 + WERASTERX * 0, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },
  { eSliderCtl, "Z", IDC_EyeZ, 34 + WERASTERX * 0, EYE__Y2, 15, SLIDERH, WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { CTEXT "LightPos", -1, 8 + WERASTERX * 2 + 2, EYE__Y2 - 9, 45, 8, ES_CENTER },
  {
    eSliderCtl, "X", IDC_LightPosX, 7 + WERASTERX * 2 + 2, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "Y", IDC_LightPosY, 7 + WERASTERX * 2 + 17, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "Z", IDC_LightPosZ, 7 + WERASTERX * 2 + 32, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },

  { CTEXT "Light Val", -1, 6 + WERASTERX * 4 + 2, EYE__Y2 - 9, 45, 8, ES_CENTER },
  {
    eSliderCtl, "R", IDC_LightColorR, 6 + WERASTERX * 4 + 2, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "G", IDC_LightColorG, 6 + WERASTERX * 4 + 17, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },
  {
    eSliderCtl, "B", IDC_LightColorB, 6 + WERASTERX * 4 + 32, EYE__Y2, 15, SLIDERH,
    WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP
  },

  {
    AUTORADIOBUTTON "1", IDC_LookAt1, 10 + WERASTERX * 0, PERSPY2, WEWIDTH, WEHEIGHT - 4,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  {
    AUTORADIOBUTTON "2", IDC_LookAt2, 10 + WERASTERX * 1, PERSPY2, WEWIDTH, WEHEIGHT - 4,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  {
    AUTORADIOBUTTON "3", IDC_LookAt3, 10 + WERASTERX * 2, PERSPY2, WEWIDTH, WEHEIGHT - 4,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  {
    AUTORADIOBUTTON "4", IDC_LookAt4, 10 + WERASTERX * 3, PERSPY2, WEWIDTH, WEHEIGHT - 4,
    BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP, NULL
  },
  { BUTTON "Save", IDC_SetDefault, 10 + WERASTERX * 4 + 10, PERSPY2, WEWIDTH, WEHEIGHT - 4, WS_TABSTOP, NULL },

  /*

    { BUTTON          "",         IDM_ZoomMax,    2,ZOOM_Y+  0,45,28,WS_TABSTOP,NULL},
    { BUTTON          "",         IDM_Zoom2,      2,ZOOM_Y+ 28,45,28,WS_TABSTOP,NULL},
    { BUTTON          ""  ,       IDM_ZoomGanz,   2,ZOOM_Y+ 56,45,28,WS_TABSTOP,NULL},
    { BUTTON          ""   ,      IDM_Zoom1,      2,ZOOM_Y+ 84,45,28,WS_TABSTOP,NULL},
    { BUTTON       "GetDefault",	IDC_GetDefault ,2,FIXP_Y+  0,45,28,WS_TABSTOP,NULL},
    { BUTTON          "Top",	    IDC_ShowTop    ,2,FIXP_Y+ 28,45,28,WS_TABSTOP,NULL},
    { BUTTON          "Bottom",	IDC_ShowBottom ,2,FIXP_Y+ 56,45,12,WS_TABSTOP,NULL},
    { BUTTON          "EyeDef",	IDC_ShowEyeDef ,2,FIXP_Y+ 68,45,12,WS_TABSTOP,NULL},
    { BUTTON          "Left",	    IDC_ShowLeft   ,2,FIXP_Y+ 80,45,12,WS_TABSTOP,NULL},
    { BUTTON          "Right",	IDC_ShowRight  ,2,FIXP_Y+ 92,45,12,WS_TABSTOP,NULL},
    { BUTTON          "Front",	IDC_ShowFront  ,2,FIXP_Y+104,45,12,WS_TABSTOP,NULL},
    { BUTTON          "Back",	    IDC_ShowBack   ,2,FIXP_Y+116,45,12,WS_TABSTOP,NULL},
    { BUTTON       "SetDefault",	IDC_SetDefault ,2,FIXP_Y+128,45,12,WS_TABSTOP|WS_DISABLED,NULL},
  */

  { 0 }
};

#define ORASTERX 80
#define ORASTERY 22
#define OWIDTH 76
#define OHEIGHT 21

const tDialogCtlItem rg_DXFOptimize[] = {
  { eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192, WS_BORDER | WS_TABSTOP }, // Dialog
  { BUTTON "Select All", IDC_SelectionAll, 1 + ORASTERX * 0, TOP + ORASTERY * 0, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Select Non", IDC_SelectionNone, 1 + ORASTERX * 1, TOP + ORASTERY * 0, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Set Active", IDC_Activate, 1 + ORASTERX * 0, TOP + ORASTERY * 1, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Set InAcivte", IDC_DeAcitvate, 1 + ORASTERX * 1, TOP + ORASTERY * 1, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },

  { BUTTON "Make Polygones", IDC_OptimizeDXF, 1 + ORASTERX * 0, TOP + ORASTERY * 2, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Make Drills", IDC_MakeDrills, 1 + ORASTERX * 1, TOP + ORASTERY * 2, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "FlipDXF", IDC_FlipDXF, 1 + ORASTERX * 0, TOP + ORASTERY * 3, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "SaveDXF", IDC_SaveDXF, 1 + ORASTERX * 1, TOP + ORASTERY * 3, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Mod Red", IDC_SetColorRed, 1 + ORASTERX * 0, TOP + ORASTERY * 4, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Mod Yellow", IDC_SetColorYellow, 1 + ORASTERX * 1, TOP + ORASTERY * 4, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Mod Green", IDC_SetColorGreen, 1 + ORASTERX * 0, TOP + ORASTERY * 5, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Mod Cyan", IDC_SetColorCyan, 1 + ORASTERX * 1, TOP + ORASTERY * 5, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Mod Blue", IDC_SetColorBlue, 1 + ORASTERX * 0, TOP + ORASTERY * 6, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  {
    BUTTON "Mod Magenta", IDC_SetColorMagenta, 1 + ORASTERX * 1, TOP + ORASTERY * 6, OWIDTH, OHEIGHT, WS_TABSTOP,
    NULL
  },
  { BUTTON "Expand 0.15", IDC_ExpandDXF015, 1 + ORASTERX * 0, TOP + ORASTERY * 7, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Expand 1.00", IDC_ExpandDXF100, 1 + ORASTERX * 1, TOP + ORASTERY * 7, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Shrink 0.15", IDC_ShrinkDXF015, 1 + ORASTERX * 0, TOP + ORASTERY * 8, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { BUTTON "Shrink 1.00", IDC_ShrinkDXF100, 1 + ORASTERX * 1, TOP + ORASTERY * 8, OWIDTH, OHEIGHT, WS_TABSTOP, NULL },
  { 0 }

};

int AktModus = 0;
// int         AktTiefe   =-1;
int ParamModify = 0;
// int         gSpindleOn  = 0;
int LaserState = IDC_PowerOff;
/*int         SpindlePWM=0;
int         FocusPower=20;
int         LaserPower=60;
int         LaserSpeed= 400;

int GcodeRapid      = 100;
int GcodeFeed       = 100;
int PowerPercent    = 100;
int ArbeitsTemp     = 1000;
*/

// unsigned int AktLayer=0xFFFFFFFF;
// int          WorkLayer=0;
//-------------------------------------------------------------
void InitLaden(void)
{
  if(gObjectManager)
    LoadWorkValues(gObjectManager->GetIniName());

  if(gDxfColorTab) {
    if(gDxfColorTab->visible())
      gDxfColorTab->Show();
  }
  if(gNullpunktTab) {
    if(gNullpunktTab->visible())
      gNullpunktTab->Show();
  }
  if(gToolAdjustTab) {
    if(gToolAdjustTab->visible())
      gToolAdjustTab->Show();
  }
  if(gSettingsTab) {
    if(gSettingsTab->visible())
      gSettingsTab->Show();
  }
  if(gLaserTab) {
    if(gLaserTab->visible())
      gLaserTab->Show();
  }
  if(gGcodeTab) {
    if(gGcodeTab->visible())
      gGcodeTab->Show();
  }
#ifdef BLOCKSIMULATION
  if(gBlockTab) {
    if(gBlockTab->visible())
      gBlockTab->Show();
  }
#endif
}

//-------------------------------------------------------------
void Show_ExecuteState()
{
  gToolBox->EnableDlgItem(IDM_FILEOPEN, !gCNCAktiv);
  gToolBox->EnableDlgItem(IDM_UPDATE, !gCNCAktiv);
  gToolBox->EnableDlgItem(IDM_FILESAVE, !gCNCAktiv && ParamModify);
  gToolBox->EnableDlgItem(IDM_Refernezfahrt, !gCNCAktiv);
  // gToolBox->EnableDlgItem(       IDM_DXFSPEZIAL,    !gCNCAktiv);
  gToolBox->EnableDlgItem(IDM_Adjust, !gCNCAktiv);
  gToolBox->EnableDlgItem(IDM_NULLPUNKT, !gCNCAktiv);
  //  gToolBox->EnableDlgItem(       IDM_LASER,         !gCNCAktiv);
  //  gToolBox->EnableDlgItem(       IDM_GCode,         !gCNCAktiv);
  // gToolBox->EnableDlgItem(       IDM_DXFColor,      !gCNCAktiv);
  //  gToolBox->EnableDlgItem(       IDM_ShowDXFTree,   !gCNCAktiv);

  gSettingsTab->EnableDlgItem(IDC_Spindle, gPa.Maschine[eSpindleRel]);

  gDxfColorTab->EnableDlgItem(IDM_SPINDLE, gPa.Maschine[eSpindleRel]);
  gGcodeTab->EnableDlgItem(IDM_SPINDLE, gPa.Maschine[eSpindleRel]);
  gDxfColorTab->EnableDlgItem(IDM_FRAESEN, gSpindleOn || !gPa.Maschine[eSpindleRel]);
  gDxfColorTab->EnableDlgItem(IDM_BOHREN, gSpindleOn || !gPa.Maschine[eSpindleRel]);
  gGcodeTab->EnableDlgItem(IDM_RUNGCODE, (gSpindleOn || !gPa.Maschine[eSpindleRel])||gLaserEnable);

  gDXFtoCNCTab->EnableDlgItem(IDM_SPINDLE, gPa.Maschine[eSpindleRel]);
  gDXFtoCNCTab->EnableDlgItem(IDM_FRAESEN, gSpindleOn | !gPa.Maschine[eSpindleRel]);
  gDXFtoCNCTab->EnableDlgItem(IDM_BOHREN, gSpindleOn | !gPa.Maschine[eSpindleRel]);
  gDXFtoCNCTab->CheckDlgButton(IDM_SPINDLE, gSpindleOn);
  gDXFtoCNCTab->CheckDlgButton(IDM_FRAESEN, gCNCAktiv && (gExecuteMode & (Linie | Buchstaben | Kreisbogen)) != 0);
  gDXFtoCNCTab->CheckDlgButton(IDM_BOHREN, gCNCAktiv && (gExecuteMode & Punkt) != 0);

  gNullpunktTab->CheckDlgButton(IDC_AdjustHeight, gCNCAktiv && (gExecuteMode == WerkzeugEinrichten));
  gNullpunktTab->CheckDlgButton(IDM_FAHRGRENZEN, gCNCAktiv && (gExecuteMode == FahreGrenzenAb));
  gDxfColorTab->CheckDlgButton(IDM_SPINDLE, gSpindleOn);
  gDxfColorTab->CheckDlgButton(IDM_FRAESEN, gCNCAktiv && (gExecuteMode & (Linie | Buchstaben | Kreisbogen)) != 0);
  gDxfColorTab->CheckDlgButton(IDM_BOHREN, gCNCAktiv && (gExecuteMode & Punkt) != 0);
  gGcodeTab->CheckDlgButton(IDM_RUNGCODE, gCNCAktiv && (gExecuteMode & GCodeLinine) != 0);
  gLaserTab->CheckDlgButton(IDM_LASER, gCNCAktiv && (gExecuteMode & LaserGravur) != 0);
  gNullpunktTab->CheckDlgButton(IDC_GehZuNULL, gCNCAktiv && (gExecuteMode & NullPunkt) != 0);

  /*  gDxfColorTab->CheckDlgButton(  IDM_STOPP,         gStopp);
    gLaserTab->CheckDlgButton(     IDM_STOPP,         gStopp);
    gGcodeTab->CheckDlgButton(     IDM_STOPP,         gStopp);
    gNullpunktTab->CheckDlgButton( IDM_STOPP,         gStopp);
  */
}
//-------------------------------------------------------------
void StopCNC()
{
  if(gStopp) {
    SendToCNC('N');
  } else {
    gStopp = true;
    SendToCNC('\033');
  }
  Show_ExecuteState();

  /*  Fl_Scroll * scroll =  (Fl_Scroll * )gToolBox->parent();
    if (scroll) {
      scroll = (Fl_Scroll * )scroll->parent();
      if (scroll) {
        scroll->scroll_to(0,0);
      }
    }
  */
}

//-------------------------------------------------------------
void ShowTab(int id)
{
  static int rec = 0;
  if(rec)
    return;
  rec++;
  Fl::lock();
  Fl::check();
  Fl::unlock();
  AktModus = id;
  // gToolBox->CheckRadioButton(IDM_DXFColor,IDM_TOPDTOOL,AktModus );

  /*
    if(id == IDM_TOPDTOOL) {
    //StopCNC();
        {
          Fl_Scroll * scroll =  (Fl_Scroll * )gToolBox->parent();
          if (scroll) {
            scroll = (Fl_Scroll * )scroll->parent();
            if (scroll) {
              scroll->scroll_to(0,0);
            }
          }
        }
    } else {
      Fl_Scroll * scroll =  (Fl_Scroll * )gToolBox->parent();
      if (scroll) {
        int h = scroll->h();
        int n = scroll->children();
        scroll = (Fl_Scroll * )scroll->parent();
        if (scroll) {
          int h2 = scroll->h();
          int h1 = gToolBox->h();
          int scroll_max  = h-h2;
          if (scroll_max  >0)
            scroll->scroll_to(0,scroll_max  );
        }
      }
    }
  */

  gToolBox->CheckDlgButton(IDM_FILEOPEN, 0);
  gToolBox->CheckDlgButton(IDM_TOPDTOOL, 0);
  gToolBox->CheckDlgButton(IDM_LAYER, 0);
  gToolBox->CheckDlgButton(IDM_DXFSPEZIAL, 0);
  gToolBox->CheckDlgButton(IDM_NULLPUNKT, 0);
  gToolBox->CheckDlgButton(IDM_DXFColor, 0);
  gToolBox->CheckDlgButton(IDM_DisplayAdj, 0);
  gToolBox->CheckDlgButton(IDM_LASER, 0);
  gToolBox->CheckDlgButton(IDM_GCode, 0);
  gToolBox->CheckDlgButton(IDM_GBlock, 0);


  switch(id) {
  case 'f':
  case IDM_FILEOPEN:
    gToolBox->CheckDlgButton(IDM_FILEOPEN, 1);
    SettingsTabs->value(gFileExplorer);

#ifdef NATIVE_FILECHOOSER
    extern int ShowFileExplorer(LPCSTR szpathstr, LPCSTR szExtention);
#else
    extern int ShowFileExplorer(LPCSTR szpathstr, LPCSTR szExtention);
    if(gObjectManager && gObjectManager->m_FilePfadName[0] != '\0') {
      SelectTreeItem(gFileExplorer, gObjectManager->m_FilePfadName);
      Fl::focus(gFileExplorer);
    }
#endif
    break;
  case 'o':
  case IDM_TOPDTOOL:
    gToolBox->CheckDlgButton(IDM_TOPDTOOL, 1);
    SettingsTabs->value(gOptionsBox);
    break;
  case IDM_ShowDXFTree:
    if(SettingsTabs->value() == pDXFTree) {
      gObjectManager->Selection(-1);
      // gWrk3DSheet->invalidate();
      gWrk3DSheet->redraw();
      pDXFTree->redraw();
    } else {
      SettingsTabs->value(pDXFTree);
    }
    break;

  case 'l':
  case IDM_LAYER:
    gToolBox->CheckDlgButton(IDM_LAYER, 1);
    SettingsTabs->value(gDxfLayer);
    gDxfLayer->Show();
    break;
  case 'c':
  case IDM_DXFSPEZIAL:
    gToolBox->CheckDlgButton(IDM_DXFSPEZIAL, 1);
    SettingsTabs->value(gSettingsTab);
    gSettingsTab->Show();
    break;
  case 'n':
  case IDM_NULLPUNKT:
    gToolBox->CheckDlgButton(IDM_NULLPUNKT, 1);
    SettingsTabs->value(gNullpunktTab);
    gNullpunktTab->Show();
    break;

  case IDM_Adjust:
    SettingsTabs->value(gToolAdjustTab);
    gToolAdjustTab->Show();
    break;

  case IDM_LASER:
    gToolBox->CheckDlgButton(IDM_LASER, 1);
    SettingsTabs->value(gLaserTab);
    gLaserTab->Show();
    break;
  case IDM_GCode:
    gToolBox->CheckDlgButton(IDM_GCode, 1);
    SettingsTabs->value(gGcodeTab);
    gGcodeTab->Show();
    break;

#ifdef BLOCKSIMULATION
  case IDM_GBlock:
    gToolBox->CheckDlgButton(IDM_GBlock,1);
    SettingsTabs->value(gBlockTab);
    gBlockTab->Show();
    break;
#endif
  /*  case IDM_Tiefe1:
    case IDM_Tiefe2:
    case IDM_Tiefe3:
    case IDM_Tiefe4:
    case IDM_Tiefe5:
    case IDM_Tiefe6:*/
  case 'x':
  case IDM_DXFColor:
    // fl_beep(FL_BEEP_NOTIFICATION);
    gToolBox->CheckDlgButton(IDM_DXFColor, 1);
    SettingsTabs->value(gDxfColorTab);
    gDxfColorTab->Show();
    if(gWrk3DSheet->visible()) {
      // gWrk3DSheet->CNCFreigeben();
      // gWrk3DSheet->invalidate();
      gWrk3DSheet->redraw();
    }
    break;
  case IDM_Calculator:
    SettingsTabs->value(gCalculatorTab);
    gCalculatorTab->Show();
    break;
  case 'd':
  case IDM_DisplayAdj:
    gToolBox->CheckDlgButton(IDM_DisplayAdj, 1);
    SettingsTabs->value(gPerspektive);
    gPerspektive->Show();
    break;
  case IDM_OPTIMIZEDXF:
    SettingsTabs->value(gDXFOptimize);
    gDXFOptimize->Show();
    break;
  case IDC_DXFtoCNCTab:
    SettingsTabs->value(gDXFtoCNCTab);
    AktTiefe = Tiefe6;
    gDXFtoCNCTab->Show();
    break;
  }
  Fl::lock();
  Fl::check();
  Fl::unlock();
  //  gToolBox->CheckRadioButton(IDM_Tiefe1,IDM_TOOLBAR,id);
  Fl::lock();
  Fl::check();
  Fl::unlock();
  rec--;
}

//-------------------------------------------------------------
CDxfLayer::CDxfLayer(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  m_NumDialogCtlItem = 0;
//  I Heard It Through The Grapevine
#ifdef FINGERSCROLL
  gLayerFingerScroll = NULL;
#else
#endif
  const tDialogCtlItem* pCtlItem = rg_DxfLayer;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_DxfLayer, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CDxfLayer::~CDxfLayer()
{
}

//-------------------------------------------------------------
int CDxfLayer::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  int buttonh = (maxxy >> 16);
#ifdef FINGERSCROLL
  gLayerFingerScroll = new CFingerScroll(left+5, buttonh + 2, width, height - buttonh - 2, "");
  gLayerFingerScroll->type(Fl_Scroll::VERTICAL);
  gLayerFingerScroll->scrollbar_size(22);
  /*  int n = 0;
    for (int y=0; y<16; y++) {
      for (int x=0; x<3; x++) {
        char buf[20];
        sprintf(buf,"%d",n++);
        Fl_Button* b = new Fl_Button(x*75,y*25+(y>=8?5*75:0),75,25);
        b->copy_label(buf);
        b->color(n);
        b->labelcolor(FL_WHITE);
      }
    }
    //  Drawing drawing(0,8*25,5*75,5*75,0);
    */
  gLayerFingerScroll->m_pack->end();
  // gLayerFingerScroll->resizable(parent());
  resizable(gLayerFingerScroll);
#else
  gLayerTree = new CTreeView(left, buttonh + 2, width, height - buttonh - 2, "");

  //  gLayerTree = ne w   Fl_Tree (4,13+h*i,w*4,h*(20-i)-20,"");
  //  gLayerTree ->type(Fl_Scroll::VERTICAL);
  // gLayerTree ->scrollbar_size(22);
  //  gLayerTree ->CreateTreeView();
  // gLayerTree ->labelcolor(FL_YELLOW);
  // gLayerTree->show();
  gLayerTree->end();
#endif

  return maxxy;
}
//-------------------------------------------------------------
void CDxfLayer::Show(void)
{
  show();
  redraw();
}

//-------------------------------------------------------------
/*
  void  cbDxfLayerButton(Fl_Widget* item, void* idc) {
  long i = (long)idc;
  Fl_Light_Button * b = (Fl_Light_Button *)item;
  if (b) {
    int mask = 1 << i;
    if (b->value()) AktLayer = AktLayer |  mask;
    else            AktLayer = AktLayer & ~mask;
  }
  if (gWrk3DSheet) gWrk3DSheet->redraw();
}
 */
//-------------------------------------------------------------
/*bool CDxfLayer::ShowLayerButtons(int neu)
{
  int i=0;

  if (m_FingerScroll) {
    if (neu) {
    int w = m_FingerScroll->w() - 25;
    m_FingerScroll->m_pack->resize(m_FingerScroll->x(),m_FingerScroll->y(),w,m_FingerScroll->h());
    m_FingerScroll->m_pack->clear();
    m_FingerScroll->m_pack->begin();
    for (i=0; i< gLayerAnz; i++) {
      Fl_Light_Button * b = new Fl_Light_Button(0,1+i*35,12,35);
      b->align(Fl_Align(FL_ALIGN_CLIP)|FL_ALIGN_INSIDE|FL_ALIGN_CENTER| FL_ALIGN_WRAP);
      b->copy_label(gszLayer[i]);
      int mask = 1 << i;
      b->value((AktLayer&mask)?1:0);
      b->callback(cbDxfLayerButton,(void*)(long)i);
    }
    m_FingerScroll->m_pack->end();
    m_FingerScroll->scroll_to(0,0);
    m_FingerScroll->redraw();
    } else {

      for (i=0; i< gLayerAnz; i++) {
        Fl_Light_Button * b = (Fl_Light_Button *)m_FingerScroll->m_pack->child(i);
        if (b) {
          b->copy_label(gszLayer[i]);
          int mask = 1 << i;
          b->value((AktLayer&mask)?1:0);
        }
      }

    }
  }
  return 0;
}
void gShowLayerButtons(int newi){
  gDxfLayer->ShowLayerButtons(1);
  return;
}

*/
//-------------------------------------------------------------
int CDxfLayer::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int ShowResult = 0;
  if(uMsg == WM_COMMAND) {
    switch(wParam) {
    case IDM_STOPP:
      StopCNC();
      break;
    case IDC_LayerAll: {
      if(AktLayer)
        AktLayer = 0x0;
      else
        AktLayer = 1;
      gShowLayerButtons(0);
      gWrk3DSheet->redraw();
    }
    break;
    }
  }
  return 0;
}
//-------------------------------------------------------------

//-------------------------------------------------------------
CDxfColorTab ::CDxfColorTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  m_NumDialogCtlItem = 0;
  const tDialogCtlItem* pCtlItem = rg_DxfColorTab;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_DxfColorTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
  m_LayerAuswahl = NULL;
  p_LayerListe = NULL;
}

//-------------------------------------------------------------
CDxfColorTab::~CDxfColorTab()
{
}

//-------------------------------------------------------------
BOOL CDxfColorTab::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}

//-------------------------------------------------------------
int CDxfColorTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);

  tDialogCtlItem* pCtlItem = pGetDlgItem(id_ArbeitsTemp);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
    //((Fl_Value_Slider*)pCtlItem->pWndObject)->box(FL_FLAT_BOX);
    //((Fl_Value_Slider*)pCtlItem->pWndObject)->labelsize(8);
    //((Fl_Value_Slider*)pCtlItem->pWndObject)->scrollvalue(0,10,0,10);
  }
  pCtlItem = pGetDlgItem(id_EintauchTiefe1);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(id_StufenTiefe1);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(id_FraeserDurchm);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }

  pCtlItem = pGetDlgItem(IDM_LAYER);
  if(pCtlItem && pCtlItem->pWndObject) {
    int r = ((Fl_Counter*)pCtlItem->pWndObject)->x();
    int t = ((Fl_Counter*)pCtlItem->pWndObject)->y();
    int h = ((Fl_Counter*)pCtlItem->pWndObject)->h();
    int w = ((Fl_Counter*)pCtlItem->pWndObject)->w();
    m_LayerAuswahl = new Fl_Choice(r, t, w, h, "WorkLayer");
    ;
    // choice->menu(FraeserChoices);
  }

  SetButtonImage(IDM_Tiefe1, 19);
  SetButtonImage(IDM_Tiefe2, 20);
  SetButtonImage(IDM_Tiefe3, 21);
  SetButtonImage(IDM_Tiefe4, 22);
  SetButtonImage(IDM_Tiefe5, 23);
  SetButtonImage(IDM_Tiefe6, 24);

  SetButtonImage(IDM_BOHREN, 6);
  SetButtonImage(IDM_FRAESEN, 4);


  return maxxy;
}
//-------------------------------------------------------------
void CDxfColorTab::Show(void)
{
  char s[32];
  if(AktTiefe && AktTiefe <= 6) {
    sprintf(s, "%1.1f", (float)gPa.Tempo[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(id_ArbeitsTempT, s);
    tDialogCtlItem* pCtlItem = pGetDlgItem(id_ArbeitsTemp);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Tempo[AktTiefe], PaMin.Tempo[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Tempo[AktTiefe]);
    }
    SetBusyDisplay(PaMax.Tempo[AktTiefe], "PaMax");

#ifdef TOUCHSCREEN
    sprintf(s, "%1.2f", (float)gPa.Tiefe[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(id_EintauchTiefe1T, s);
    pCtlItem = pGetDlgItem(id_EintauchTiefe1T);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Counter*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
      if(gPa.Tiefe[AktTiefe] < -100) {
        ((Fl_Counter*)pCtlItem->pWndObject)->step(0.1, 1.0);
      } else {
        ((Fl_Counter*)pCtlItem->pWndObject)->step(0.01, 0.2);
      }
      ((Fl_Counter*)pCtlItem->pWndObject)->range(PaMax.Tiefe[AktTiefe], PaMin.Tiefe[AktTiefe]);
      //      ((Fl_Counter*)pCtlItem->pWndObject)->value(gPa.Tiefe[AktTiefe]);
    }
#else
    sprintf(s, "%1.2f", (float)gPa.Tiefe[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(id_EintauchTiefe1T, s);
    pCtlItem = pGetDlgItem(id_EintauchTiefe1);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(5);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Tiefe[AktTiefe], PaMin.Tiefe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Tiefe[AktTiefe]);
    }
#endif
    // Stufe Tiefe
    sprintf(s, "%1.2f", (float)gPa.Stufe[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(id_StufenTiefe1T, s);

    pCtlItem = pGetDlgItem(id_StufenTiefe1);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Stufe[AktTiefe], PaMin.Stufe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Stufe[AktTiefe]);
    }

    // Fraeser Durchmesser
    sprintf(s, "%1.2f", (float)gPa.WerkzeugDuchmesser[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(id_FraeserDurchmT, s);

    pCtlItem = pGetDlgItem(id_FraeserDurchm);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)
      ->range(PaMax.WerkzeugDuchmesser[AktTiefe], PaMin.WerkzeugDuchmesser[AktTiefe]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.WerkzeugDuchmesser[AktTiefe]);
    }

    pCtlItem = pGetDlgItem(id_StufenTiefe1T);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Counter*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
      ((Fl_Counter*)pCtlItem->pWndObject)->step(0.1);
      ((Fl_Counter*)pCtlItem->pWndObject)->lstep(1.0);
      ((Fl_Counter*)pCtlItem->pWndObject)->range(PaMin.Stufe[AktTiefe], PaMax.Stufe[AktTiefe]);
    }

    pCtlItem = pGetDlgItem(id_FraeserDurchmT);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Counter*)pCtlItem->pWndObject)->type(FL_SIMPLE_COUNTER);
      ((Fl_Counter*)pCtlItem->pWndObject)->step(1);
      //((Fl_Counter*)pCtlItem->pWndObject)->lstep(1.0);
      ((Fl_Counter*)pCtlItem->pWndObject)->range(PaMin.Stufe[AktTiefe], PaMax.Stufe[AktTiefe]);
    }

    pCtlItem = pGetDlgItem(IDC_Spindle);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Counter*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
      ((Fl_Counter*)pCtlItem->pWndObject)->step(100);
      ((Fl_Counter*)pCtlItem->pWndObject)->lstep(1000);
      ((Fl_Counter*)pCtlItem->pWndObject)->range(300, 20000);
    }
  } else {
    SetDlgItemText(id_ArbeitsTempT, "err");
    SetDlgItemText(id_EintauchTiefe1T, "err");
    SetDlgItemText(id_StufenTiefe1T, "err");
    //    SetDlgItemText(id_EintauchTempo1T,"err");
    //    SetDlgItemText(id_BohrTempo1T,"err");
  }
  // static Fl_Menu_Item * pChoices=NULL;
  if(p_LayerListe)
    free(p_LayerListe);

  int gLayerAnz = 0;
  CLayers* pLayer = gpLayers;
  while(pLayer) {
    gLayerAnz++;
    pLayer = pLayer->m_NextLayer;
  }

  p_LayerListe = (Fl_Menu_Item*)malloc(sizeof(Fl_Menu_Item) * (gLayerAnz + 1));
  if(p_LayerListe) {
    memset(p_LayerListe, 0, sizeof(Fl_Menu_Item) * (gLayerAnz + 1));
    int aktiveLayers = 0;
    int selindex = -1;

    CLayers* pLayer = gpLayers;
    for(int i = 0; i < gLayerAnz; i++) {
      if(pLayer) {
        p_LayerListe[aktiveLayers].text = pLayer->m_Name;
        p_LayerListe[aktiveLayers].shortcut_ = i;
        // if (WorkLayer==pLayer) selindex = aktiveLayers;
        if(strcmp(szWorkLayer, pLayer->m_Name) == 0) {
          selindex = aktiveLayers;
          WorkLayer = pLayer;
        }
        p_LayerListe[aktiveLayers].callback_ = CFltkDialogBox::cbUniversal;
        p_LayerListe[aktiveLayers].user_data_ = (void*)IDM_LAYER;
        aktiveLayers++;
        pLayer = pLayer->m_NextLayer;
      }
    }

    /*      for (int i=0;i< gLayerAnz;i++) {
            int OLayer = 1 << i;
            if ((AktLayer&OLayer)==OLayer) {
              p_LayerListe[aktiveLayers].text = gszLayer[i];
              p_LayerListe[aktiveLayers].shortcut_ = i;
              if (WorkLayer==i) selindex = aktiveLayers;
              p_LayerListe[aktiveLayers].callback_ = CFltkDialogBox::cbUniversal;
              p_LayerListe[aktiveLayers].user_data_=(void*)IDM_LAYER;
              aktiveLayers++;
            }
          }*/
    m_LayerAuswahl->menu(p_LayerListe);
    m_LayerAuswahl->value(selindex);
  }
  CheckRadioButton(IDM_Tiefe1, IDM_TOOLBAR, IDM_Tiefe1+AktTiefe-1);

  show();
  redraw();
}
//-------------------------------------------------------------
int roundf(double v)
{
  return (int)(v + 0.5);
}
int CDxfColorTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int ShowResult = 0;
  if(uMsg == WM_COMMAND) {
    char str[32];
    float f;
    static bool recursion = 0;
    switch(wParam) {
    case IDM_FRAESEN: {
      StartExecuteThread(Linie | Buchstaben | Kreisbogen);
    }
    break;
    case IDM_SPINDLE:
      if(!gSpindleOn) {
        SendToCNC('E', (_max(1, gPa.Maschine[eSpindlePWM]) * 255) / 100);
      } else {
        SendToCNC('E', 0);
      }
      SendToCNC('V', gSpindleOn ? 1 : -1);
      Show_ExecuteState();
      break;
    case IDM_STOPP:
      StopCNC();
      break;
    case IDM_BOHREN: {
      StartExecuteThread(gExecuteMode = Punkt);
    }
    break;
    case id_ArbeitsTemp: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(id_ArbeitsTemp, NULL, 0);
      if(v > 0 && v < 100000) {
        gPa.Tempo[AktTiefe] = v; // * 100;
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Tempo[AktTiefe]);
        sprintf(str, "%1.0f", (float)gPa.Tempo[AktTiefe] / (float)gPa.Aufloesung);
        SetDlgItemText(id_ArbeitsTempT, str);
        ParamModify = 1;
      }
    }
    break;
    case id_ArbeitsTempT:
      GetDlgItemText(id_ArbeitsTempT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        if(f < 0.1) {
          f = 0.1;
          sprintf(str, "%1.2f", f);
          SetDlgItemText(id_ArbeitsTempT, str);
        }
        gPa.Tempo[AktTiefe] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Tempo[AktTiefe]);
        ParamModify = 1;

        recursion = 1;
        SetDlgItemInt(id_ArbeitsTemp, gPa.Tempo[AktTiefe] / 1, 1);
        recursion = 0;
      }
      break;
    case id_EintauchTiefe1: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(id_EintauchTiefe1, NULL, 1);
      //~ if (v> 0  && v < 10000)
      if(v > -10000 && v <= 0) {
        gPa.Tiefe[AktTiefe] = v * 1;
        sprintf(str, "%1.2f", (float)gPa.Tiefe[AktTiefe] / (float)gPa.Aufloesung);
        SetDlgItemText(id_EintauchTiefe1T, str);
        ShowResult = 1;
        ParamModify = 1;
      }
    }
    break;
    case id_EintauchTiefe1T:
      GetDlgItemText(id_EintauchTiefe1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tiefe[AktTiefe] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
        recursion = 1;
        SetDlgItemInt(id_EintauchTiefe1, gPa.Tiefe[AktTiefe] / 1, 1);
        ShowResult = 1;
        recursion = 0;
      }
      break;
    case id_StufenTiefe1: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(id_StufenTiefe1, NULL, 1);
      if(v > 0 && v < 10000) {
        gPa.Stufe[AktTiefe] = v;
        sprintf(str, "%1.2f", (float)v / (float)gPa.Aufloesung);
        SetDlgItemText(id_StufenTiefe1T, str);
        ParamModify = 1;
        ShowResult = 1;
      }
    }
    break;
    case id_StufenTiefe1T:
      GetDlgItemText(id_StufenTiefe1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        if(f < 0.0) {
          f = 0.0;
          sprintf(str, "%1.2f", f);
          SetDlgItemText(id_StufenTiefe1T, str);
        }
        //(CNCINT)f;
        ParamModify = 1;
        ShowResult = 1;
        recursion = 1;
        SetDlgItemInt(id_StufenTiefe1, f * (float)gPa.Aufloesung, 1);
        gPa.Stufe[AktTiefe] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        recursion = 0;
      }
      break;
    case id_FraeserDurchm: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(id_FraeserDurchm, NULL, 1);
      if(v > 0 && v < 10000) {
        gPa.WerkzeugDuchmesser[AktTiefe] = v * 1;
        sprintf(str, "%1.2f", (float)v / (float)gPa.Aufloesung);
        SetDlgItemText(id_FraeserDurchmT, str);
        ParamModify = 1;
        ShowResult = 1;
      }
    }
    break;
    case id_FraeserDurchmT:
      GetDlgItemText(id_FraeserDurchmT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.WerkzeugDuchmesser[AktTiefe] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
        recursion = 1;
        SetDlgItemInt(id_FraeserDurchm, f * (float)gPa.Aufloesung, 1);
        ShowResult = 1;
        recursion = 0;
      }
      //      break;
      /*    case IDC_Material:
            if (wParam==IDC_Material){
              iMaterial = ((Fl_Choice *) lParam )->value();
            }
          case IDC_FraeserType:
            if (wParam==IDC_FraeserType){
              iFraeserType = ((Fl_Choice *) lParam )->value();
            }
          case IDC_Spindle:
            GetDlgItemText(IDC_Spindle,str,20);
            if (sscanf(str,"%f",&f)==1)
            {
              float Durchmesser = gPa.WerkzeugDuchmesser[AktTiefe]/100.0;
              if (iFraeserType && iMaterial <= Stahl && Durchmesser <12.3&&Durchmesser >= 1.0 ) {
                // Vorschub mm / Zahn / Um
                float Vorschub = Zahnvorschub[iMaterial][(int)Durchmesser]*iFraeserType*f/60.0;
                gPa.Tempo[AktTiefe]  = (CNCINT)roundf(Vorschub*(float)gPa.Aufloesung);
                sprintf(str,"%1.1f",Vorschub);
                SetDlgItemText(id_ArbeitsTempT,str);
                ParamModify = 1;
              }
            }
        */
      break;
    case IDM_LAYER: {
      int ix = ((Fl_Choice*)lParam)->value();
      int i = p_LayerListe[ix].shortcut_;
      CLayers* pLayer = gpLayers;
      while(pLayer && i >= 0) {
        if(i == 0)
          break;
        i--;
        pLayer = pLayer->m_NextLayer;
      }
      if(i >= 0) {
        WorkLayer = pLayer;
        strncpy(szWorkLayer, WorkLayer->m_Name, sizeof(szWorkLayer));
      } else {
        WorkLayer = NULL;
        szWorkLayer[0] = '\0';
      }

      if(gWrk3DSheet->visible()) {
        // gWrk3DSheet->CNCFreigeben();
        // gWrk3DSheet->invalidate();
        gWrk3DSheet->redraw();
      }
    }
    break;
    case IDM_Calculator:
      ShowTab(IDM_Calculator);
      break;
    case IDM_Tiefe1:
    case IDM_Tiefe2:
    case IDM_Tiefe3:
    case IDM_Tiefe4:
    case IDM_Tiefe5:
    case IDM_Tiefe6:
      AktTiefe = wParam - IDM_Tiefe1 + 1;
      Show();
      if(gWrk3DSheet->visible()) {
        // gWrk3DSheet->CNCFreigeben();
        // gWrk3DSheet->invalidate();
        gWrk3DSheet->redraw();
      }
      Fl::lock();
      Fl::check();
      Fl::unlock();
      CheckRadioButton(IDM_Tiefe1, IDM_TOOLBAR, wParam);
      Fl::lock();
      Fl::check();
      Fl::unlock();
      break;
    }
  }
  if(ShowResult && gWrk3DSheet->visible()) {
    // gWrk3DSheet->CNCFreigeben();
    // gWrk3DSheet->invalidate();
    gWrk3DSheet->redraw();
  }
  return 0;
}
//-------------------------------------------------------------

CCalculatorTab::CCalculatorTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_CalculatorTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_CalculatorTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

enum { Guss_Alu, Aluminium, Weichkunststoff, Hartkunststoff, PrintPlatte, Holz_hart, Holz_weich, MDF, Messing_Kupfer_Bronze, Stahl };

// Faustwerte für die Schnittgeschwindigkeit vc
// Aluminium:   100 - 500 m/min
// Kunststoffe:  50 - 150 m/min
// Holz: Bis zu      3000 m/min
// Kupfer, Messing
//  und Bronze: 100 - 200 m/min
// Stahl:        40 - 120 m/min

// n [U/min] = (vc [m/min] *1000) / (3.14 * Ãd1 [mm])

float Zahnvorschub[10][13] = {
  // Schnittwert M/min, 1,2,3.... mm
  //     1     2     3     4     5     6     7     8     9    10    11    12
  /*Guss-Aluminium>_6_prz_Si*/
  { 500, 0.010, 0.010, 0.010, 0.015, 0.015, 0.025, 0.027, 0.030, 0.034, 0.038, 0.044, 0.050 },
  /*Aluminium_Knetlegierung*/
  { 500, 0.010, 0.020, 0.025, 0.050, 0.050, 0.050, 0.056, 0.064, 0.072, 0.080, 0.090, 0.100 },
  /*Weichkunststoff*/
  { 600, 0.025, 0.030, 0.035, 0.045, 0.065, 0.090, 0.095, 0.100, 0.150, 0.200, 0.250, 0.300 },
  /*Hartkunststoff*/
  { 550, 0.015, 0.020, 0.025, 0.050, 0.060, 0.080, 0.084, 0.089, 0.094, 0.100, 0.125, 0.150 },
  /*PrintPlatte*/
  { 550, 0.015, 0.020, 0.025, 0.050, 0.060, 0.080, 0.084, 0.089, 0.094, 0.100, 0.125, 0.150 },
  /*Holz_hart*/
  { 450, 0.020, 0.025, 0.030, 0.035, 0.045, 0.055, 0.059, 0.065, 0.072, 0.080, 0.085, 0.090 },
  /*Holz_weich*/
  { 500, 0.025, 0.030, 0.035, 0.040, 0.050, 0.060, 0.065, 0.070, 0.077, 0.085, 0.090, 0.100 },
  /*MDF*/
  { 450, 0.030, 0.040, 0.045, 0.050, 0.060, 0.070, 0.075, 0.080, 0.085, 0.090, 0.100, 0.110 },
  /*Messing_Kupfer_Bronze*/
  { 365, 0.015, 0.020, 0.025, 0.025, 0.030, 0.050, 0.053, 0.056, 0.060, 0.065, 0.072, 0.080 },
  /*Stahl*/
  { 90, 0.010, 0.010, 0.012, 0.025, 0.030, 0.038, 0.041, 0.045, 0.047, 0.050, 0.070, 0.080 },

};
/*
// https://webseite.sorotec.de/download/fraesparameter/cutting_param_facemillcutter.pdf
//Cast aluminum > 6% Si
250 0,057 0,067 0,084 0,095 0,105 0,114 0,147
//Aluminum Wrought alloy
350 0,054 0,063 0,079 0,090 0,100 0,108 0,134
//Soft plastic
300 0,120 0,144 0,176 0,192 0,215 0,240 0,312
//Hard plastic
250 0,110 0,132 0,154 0,176 0,198 0,220 0,246
//Hard wood
250 0,080 0,096 0,113 0,128 0,145 0,160 0,185
//Soft wood
200 0,100 0,115 0,135 0,151 0,165 0,182 0,218
//Brass,Copper, Bronze 250 ... 350
250 0,033 0,039 0,042 0,055 0,060 0,066 0,084
//Stahl
90  0,06
*/
int EintauchTiefe[9] = { 50, 50, 200, 200, 100, 150, 200, 50, 30 };
int Schnittwert[10] = { 250, 350, 300, 250, 250, 250, 200, 450, 250, 90 };

int iTempo = 100;
int iStufenTiefe = 100;
int iDurchmesser = 300;

int iMaterial = Aluminium;
int iFraeserType = 2;

Fl_Menu_Item FraeserChoices[] = {

  { "V Stichel", 0, CFltkDialogBox::cbUniversal, (void*)0 },
  { "1 Schneide", 0, CFltkDialogBox::cbUniversal, (void*)IDC_FraeserType },
  { "2 Schneidig", 0, CFltkDialogBox::cbUniversal, (void*)IDC_FraeserType },
  { "3 Schneidig", 0, CFltkDialogBox::cbUniversal, (void*)IDC_FraeserType },
  { "4 Schneidig", 0, CFltkDialogBox::cbUniversal, (void*)IDC_FraeserType }, { 0 }
};

Fl_Menu_Item MaterialChoices[] = {

  { "Alu Guss", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "Alu Knet", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "WeichKunststoff", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "HartKunststoff", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "PrintPlatte", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "HartHolz", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "WeichHolz", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "MDF", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "Messing CU Brnze", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material },
  { "Stahl", 0, CFltkDialogBox::cbUniversal, (void*)IDC_Material }, { 0 }
};

//-------------------------------------------------------------
CCalculatorTab::~CCalculatorTab()
{
}

//-------------------------------------------------------------
int CCalculatorTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);

  int w = 100;
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_FraeserDurchmT);
  if(pCtlItem && pCtlItem->pWndObject) {
    w = ((Fl_Counter*)pCtlItem->pWndObject)->w();
  }

  pCtlItem = pGetDlgItem(IDC_FraeserType);
  if(pCtlItem && pCtlItem->pWndObject) {
    int r = ((Fl_Counter*)pCtlItem->pWndObject)->x();
    int t = ((Fl_Counter*)pCtlItem->pWndObject)->y();
    int h = ((Fl_Counter*)pCtlItem->pWndObject)->h();
    w = ((Fl_Counter*)pCtlItem->pWndObject)->w();
    Fl_Choice* choice = new Fl_Choice(r, t, w, h, "Fräser");
    choice->menu(FraeserChoices);
    choice->value(iFraeserType);
  }
  pCtlItem = pGetDlgItem(IDC_Material);
  if(pCtlItem && pCtlItem->pWndObject) {
    int r = ((Fl_Counter*)pCtlItem->pWndObject)->x();
    int t = ((Fl_Counter*)pCtlItem->pWndObject)->y();
    int h = ((Fl_Counter*)pCtlItem->pWndObject)->h();
    w = ((Fl_Counter*)pCtlItem->pWndObject)->w();
    Fl_Choice* choice = new Fl_Choice(r, t, w, h, "Material");
    choice->menu(MaterialChoices);
    choice->value(iMaterial);
  }
  if(0) {
    for(int m = Guss_Alu; m <= Stahl; m++) {
      float p = Zahnvorschub[m][8] / 100;
      if(p) {
        for(int d = 1; d <= 12; d++) {
          printf("%3d ", (int)(Zahnvorschub[m][d] / p));
        }
        printf("\n");
      }
    }
  }
  return maxxy;
}
//-------------------------------------------------------------
void CCalculatorTab::Show(void)
{
  // Fraeser Durchmesser
  char s[32];
  iTempo = gPa.Tempo[AktTiefe];
  sprintf(s, "%1.0f", (float)iTempo / (float)gPa.Aufloesung);
  SetDlgItemText(id_ArbeitsTempT, s);

  iDurchmesser = gPa.WerkzeugDuchmesser[AktTiefe];
  sprintf(s, "%1.2f", (float)iDurchmesser / (float)gPa.Aufloesung);
  SetDlgItemText(id_FraeserDurchmT, s);

  iStufenTiefe = gPa.Stufe[AktTiefe];
  sprintf(s, "%1.2f", (float)iStufenTiefe / (float)gPa.Aufloesung);
  SetDlgItemText(id_StufenTiefe1T, s);

  /*    tDialogCtlItem * pCtlItem = pGetDlgItem(id_StufenTiefe1T);
      if (pCtlItem && pCtlItem->pWndObject)
      {
        ((Fl_Counter*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
        ((Fl_Counter*)pCtlItem->pWndObject)->step(0.1);
        ((Fl_Counter*)pCtlItem->pWndObject)->lstep(1.0);
        ((Fl_Counter*)pCtlItem->pWndObject)->range(PaMin.Stufe[AktTiefe],PaMax.Stufe[AktTiefe]);
      }
  */
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_FraeserDurchmT);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Counter*)pCtlItem->pWndObject)->type(FL_SIMPLE_COUNTER);
    ((Fl_Counter*)pCtlItem->pWndObject)->step(0.1);
    ((Fl_Counter*)pCtlItem->pWndObject)->range(PaMin.Stufe[AktTiefe], PaMax.Stufe[AktTiefe]);
  }

  pCtlItem = pGetDlgItem(IDC_Spindle);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((CNumericInput*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
    ((Fl_Counter*)pCtlItem->pWndObject)->step(100);
    ((CNumericInput*)pCtlItem->pWndObject)->range(300, 20000);
  }
  // show();
}
//-------------------------------------------------------------
int CCalculatorTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[32];
    static bool recursion = 0;
    float f;
    switch(wParam) {
    case IDM_ApplyValues:

      gPa.Tempo[AktTiefe] = iTempo;
      gPa.WerkzeugDuchmesser[AktTiefe] = iDurchmesser;
      gPa.Stufe[AktTiefe] = iStufenTiefe;
      ParamModify = 1;
    case IDM_DXFColor:
      ShowTab(IDM_DXFColor);
      break;

    case IDC_Material:
      if(wParam == IDC_Material) {
        iMaterial = ((Fl_Choice*)lParam)->value();
      }
    case IDC_FraeserType:
      if(wParam == IDC_FraeserType) {
        iFraeserType = ((Fl_Choice*)lParam)->value();
      }
    case id_FraeserDurchmT:
      if(wParam == id_FraeserDurchmT) {
        GetDlgItemText(id_FraeserDurchmT, str, 20);
        if(sscanf(str, "%f", &f) == 1) {
          iDurchmesser = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        }
      }
      if(iFraeserType && iMaterial <= Stahl) {
        int upm = Schnittwert[iMaterial] * 1000.f / (M_PI * (float)iDurchmesser / (float)gPa.Aufloesung);
        sprintf(str, "Max %d U/min", upm);
        SetDlgItemText(IDM_MaxSpindleRPM, str);
      }

    case IDC_Spindle:
      GetDlgItemText(IDC_Spindle, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        float Durchmesser = iDurchmesser / 100.0;
        if(iFraeserType && iMaterial <= Stahl && Durchmesser < 12.3 && Durchmesser >= 1.0) {
          // Vorschub mm / Zahn / Um
          float Vorschub = Zahnvorschub[iMaterial][(int)Durchmesser] * iFraeserType * f / 60.0;
          iTempo = (CNCINT)roundf(Vorschub * (float)gPa.Aufloesung);
          sprintf(str, "%1.1f", Vorschub);
          SetDlgItemText(id_ArbeitsTempT, str);
          iStufenTiefe = EintauchTiefe[iMaterial] * iDurchmesser / 100;
          sprintf(str, "%1.2f", (float)iStufenTiefe / (float)gPa.Aufloesung);
          SetDlgItemText(id_StufenTiefe1T, str);
        }
      }
      break;
    }
  }
  return 0;
}

//-------------------------------------------------------------

CDXFtoCNCTab::CDXFtoCNCTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_DXFtoCNCTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_DXFtoCNCTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CDXFtoCNCTab::~CDXFtoCNCTab()
{
}

//-------------------------------------------------------------
int CDXFtoCNCTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);

  int w = 100;
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_FraeserDurchmT);
  if(pCtlItem && pCtlItem->pWndObject) {
    w = ((CNumericInput*)pCtlItem->pWndObject)->w();
  }

  pCtlItem = pGetDlgItem(IDC_FraeserType);
  if(pCtlItem && pCtlItem->pWndObject) {
    int r = (pCtlItem->pWndObject)->x();
    int t = (pCtlItem->pWndObject)->y();
    int h = (pCtlItem->pWndObject)->h();
    w = (pCtlItem->pWndObject)->w();
    Fl_Choice* choice = new Fl_Choice(r, t, w, h, "Fräser");
    choice->menu(FraeserChoices);
    choice->value(iFraeserType);
  }
  pCtlItem = pGetDlgItem(IDC_Material);
  if(pCtlItem && pCtlItem->pWndObject) {
    int r = (pCtlItem->pWndObject)->x();
    int t = (pCtlItem->pWndObject)->y();
    int h = (pCtlItem->pWndObject)->h();
    w = (pCtlItem->pWndObject)->w();
    Fl_Choice* choice = new Fl_Choice(r, t, w, h, "Material");
    choice->menu(MaterialChoices);
    choice->value(iMaterial);
  }
  if(0) {
    for(int m = Guss_Alu; m <= Stahl; m++) {
      float p = Zahnvorschub[m][8] / 100;
      if(p) {
        for(int d = 1; d <= 12; d++) {
          printf("%3d ", (int)(Zahnvorschub[m][d] / p));
        }
        printf("\n");
      }
    }
  }
  return maxxy;
}
//-------------------------------------------------------------
void CDXFtoCNCTab::Show(void)
{
  // Fraeser Durchmesser
  char s[32];
  iTempo = gPa.Tempo[Tiefe6];
  sprintf(s, "%1.0f", (float)iTempo / (float)gPa.Aufloesung);
  SetDlgItemText(id_ArbeitsTempT, s);

  iDurchmesser = gPa.WerkzeugDuchmesser[Tiefe6];
  sprintf(s, "%1.2f", (float)iDurchmesser / (float)gPa.Aufloesung);
  SetDlgItemText(id_FraeserDurchmT, s);

  iStufenTiefe = gPa.Stufe[Tiefe6];
  sprintf(s, "%1.2f", (float)iStufenTiefe / (float)gPa.Aufloesung);
  SetDlgItemText(id_StufenTiefe1T, s);

  sprintf(s, "%1.2f", (float)gPa.Tiefe[Tiefe6] / (float)gPa.Aufloesung);
  SetDlgItemText(id_EintauchTiefe1T, s);
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_EintauchTiefe1T);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((CNumericInput*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
    if(gPa.Tiefe[Tiefe6] < -100) {
      ((CNumericInput*)pCtlItem->pWndObject)->step(0.1);
    } else {
      ((CNumericInput*)pCtlItem->pWndObject)->step(0.01);
    }
    ((CNumericInput*)pCtlItem->pWndObject)->range(PaMax.Tiefe[Tiefe6], PaMin.Tiefe[Tiefe6]);
  }

  pCtlItem = pGetDlgItem(id_FraeserDurchmT);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((CNumericInput*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
    ((CNumericInput*)pCtlItem->pWndObject)->step(0.1, 1.0);
    ((CNumericInput*)pCtlItem->pWndObject)->range(0.50, 25.0);
  }

  pCtlItem = pGetDlgItem(IDC_Spindle);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((CNumericInput*)pCtlItem->pWndObject)->type(FL_NORMAL_COUNTER);
    ((CNumericInput*)pCtlItem->pWndObject)->step(100);
    ((CNumericInput*)pCtlItem->pWndObject)->range(300, 20000);
  }
  show();
}
//-------------------------------------------------------------
int CDXFtoCNCTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int ShowResult = 0;
  if(uMsg == WM_COMMAND) {
    char str[32];
    static bool recursion = 0;
    float f;
    switch(wParam) {
    case IDM_FRAESEN: {
      StartExecuteThread(Linie | Buchstaben | Kreisbogen);
    }
    break;
    case IDM_SPINDLE:
      if(!gSpindleOn) {
        SendToCNC('E', (_max(1, gPa.Maschine[eSpindlePWM]) * 255) / 100);
      } else {
        SendToCNC('E', 0);
      }
      SendToCNC('V', gSpindleOn ? 1 : -1);
      Show_ExecuteState();
      break;
    case IDM_STOPP:
      StopCNC();
      break;
    case IDM_BOHREN: {
      StartExecuteThread(gExecuteMode = Punkt);
    }
    break;

    /*    case IDM_ApplyValues:

      gPa.Tempo[Tiefe6] = iTempo;
      gPa.WerkzeugDuchmesser[Tiefe6]=iDurchmesser;
      gPa.Stufe[Tiefe6]=iStufenTiefe;
      ParamModify = 1;
    case IDM_DXFColor:
      ShowTab(IDM_DXFColor);
      break;
    */
    case IDC_AufdemStrich:
    case IDC_Gegenlauf:
    case IDC_Gleichlauf:
      break;
    case IDC_Material:
      if(wParam == IDC_Material) {
        iMaterial = ((Fl_Choice*)lParam)->value();
      }
    case id_EintauchTiefe1T:
      GetDlgItemText(id_EintauchTiefe1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tiefe[Tiefe6] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
        ShowResult = 1;
      }
      break;
    case id_ArbeitsTempT:
      GetDlgItemText(id_ArbeitsTempT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        if(f < 0.1) {
          f = 0.1;
          sprintf(str, "%1.2f", f);
          SetDlgItemText(id_ArbeitsTempT, str);
        }
        gPa.Tempo[Tiefe6] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Tempo[Tiefe6]);
        ParamModify = 1;
      }
      break;

    case id_StufenTiefe1T:
      GetDlgItemText(id_StufenTiefe1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        if(f < 0.0) {
          f = 0.0;
          sprintf(str, "%1.2f", f);
          SetDlgItemText(id_StufenTiefe1T, str);
        }
        ParamModify = 1;
        ShowResult = 1;
        gPa.Stufe[Tiefe6] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
      }
      break;

    case IDC_FraeserType:
      if(wParam == IDC_FraeserType) {
        iFraeserType = ((Fl_Choice*)lParam)->value();
      }
    case id_FraeserDurchmT:
      if(wParam == id_FraeserDurchmT) {
        GetDlgItemText(id_FraeserDurchmT, str, 20);
        if(sscanf(str, "%f", &f) == 1) {
          iDurchmesser = (CNCINT)roundf(f * (float)gPa.Aufloesung);
          gPa.WerkzeugDuchmesser[Tiefe6] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
          ParamModify = 1;
          ShowResult = 1;
        }
      }
      if(iFraeserType && iMaterial <= Stahl) {
        int upm = Schnittwert[iMaterial] * 1000.f / (M_PI * (float)iDurchmesser / (float)gPa.Aufloesung);
        sprintf(str, "Max %d U/min", upm);
        SetDlgItemText(IDM_MaxSpindleRPM, str);
      }

    case IDC_Spindle:
      GetDlgItemText(IDC_Spindle, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        float Durchmesser = iDurchmesser / 100.0;
        if(iFraeserType && iMaterial <= Stahl && Durchmesser < 12.3 && Durchmesser >= 1.0) {
          // Vorschub mm / Zahn / Um
          float Vorschub = Zahnvorschub[iMaterial][(int)Durchmesser] * iFraeserType * f / 60.0;
          iTempo = (CNCINT)roundf(Vorschub * (float)gPa.Aufloesung);

          tDialogCtlItem* pCtlItem = pGetDlgItem(id_ArbeitsTempT);
          if(pCtlItem && pCtlItem->pWndObject) {
            ((CNumericInput*)pCtlItem->pWndObject)->SetProposeVal(Vorschub);
          }
          iStufenTiefe = EintauchTiefe[iMaterial] * iDurchmesser / 100;
          pCtlItem = pGetDlgItem(id_StufenTiefe1T);
          if(pCtlItem && pCtlItem->pWndObject) {
            ((CNumericInput*)pCtlItem->pWndObject)->SetProposeVal((float)iStufenTiefe / (float)gPa.Aufloesung);
          }

          /*
                    sprintf(str,"%1.1f",Vorschub);
                    SetDlgItemText(id_ArbeitsTempT,str);
                    iStufenTiefe= EintauchTiefe[iMaterial] * iDurchmesser / 100;
                    sprintf(str,"%1.2f",(float)iStufenTiefe/(float)gPa.Aufloesung);
                    SetDlgItemText(id_StufenTiefe1T,str);*/
        }
      }
      break;
    }
  }
  if(ShowResult && gWrk3DSheet->visible()) {
    // gWrk3DSheet->CNCFreigeben();
    // gWrk3DSheet->invalidate();
    gWrk3DSheet->redraw();
  }
  return 0;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
CNullpunktTab ::CNullpunktTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_NullpunktTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_NullpunktTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CNullpunktTab::~CNullpunktTab()
{
}

//-------------------------------------------------------------
int CNullpunktTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  // SetButtonImage(id_Drehen,8);
  // SetButtonImage(ID_AufKopf,7);
  SetButtonImage(IDC_GehZuNULL, imWerkzeug);
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_ArbeitsTemp);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  SetButtonImage(IDM_Refernezfahrt, 14);
  return maxxy;
}
//-------------------------------------------------------------
void CNullpunktTab::Show(void)
{
  char s[32];
  sprintf(s, "%1.2f", (float)gPa.Ref[0] / (float)gPa.Aufloesung);
  SetDlgItemText(id_XWerNullT, s);
  sprintf(s, "%1.2f", (float)gPa.Ref[1] / (float)gPa.Aufloesung);
  SetDlgItemText(id_YWerNullT, s);
  sprintf(s, "%1.2f", (float)gPa.Ref[2] / (float)gPa.Aufloesung);
  SetDlgItemText(id_ZWerNullT, s);

  sprintf(s, "%5d Inc / mm", (int)gPa.Aufloesung);
  // CheckDlgButton(id_Drehen,(int)(gPa.Drehen!=0));
  // CheckDlgButton(ID_AufKopf,(int)(gPa.Drehen==10));
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_ArbeitsTemp);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Tempo[1], PaMin.Tempo[1]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eArbeitsTemp]);
  }
  sprintf(s, "X%1.2f", (float)gPa.WerzeugSensor[Xp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugX, s);
  sprintf(s, "Y%1.2f", (float)gPa.WerzeugSensor[Yp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugY, s);
  sprintf(s, "Z%1.2f", (float)gPa.WerzeugSensor[Zp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugZ, s);

  show();
}
//-------------------------------------------------------------
int CNullpunktTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[32];
    static bool recursion = 0;
    float f;
    switch(wParam) {
    case IDC_GehZuNULL:
      // if (gOnline) {
      StartExecuteThread(gExecuteMode = NullPunkt);
      //}
      break;
    case id_ArbeitsTemp: {
      int v = GetDlgItemInt(id_ArbeitsTemp, NULL, 0);
      if(v > 0 && v < 100000) {
        gPa.Maschine[eArbeitsTemp] = v;
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Maschine[eArbeitsTemp]);
      }
    }
    break;
    break;
    case IDM_FAHRGRENZEN:
      StartExecuteThread(gExecuteMode = FahreGrenzenAb);
      break;
    case IDM_Refernezfahrt:
      SendToCNC('R');
      gCNCEiche = 1;
      break;
    case IDM_Adjust:
      ShowTab(IDM_Adjust);
      break;

    case IDM_STOPP:
      StopCNC();
      break;
    //    case id_Drehen:
    //    case ID_AufKopf:
    //    {
    //      if (IsDlgButtonChecked(id_Drehen))
    //      {
    //        if (IsDlgButtonChecked(ID_AufKopf)) gPa.Drehen =  10;
    //        else                                gPa.Drehen =  1;
    //      }
    //      else gPa.Drehen =  0;
    //      gWrk3DSheet->redraw();
    //    }
    //    break;
    case id_XWerNullT:
      GetDlgItemText(id_XWerNullT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Ref[0] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      gWrk3DSheet->redraw();
      break;
    case id_YWerNullT:
      GetDlgItemText(id_YWerNullT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Ref[1] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      gWrk3DSheet->redraw();
      break;
    case id_ZWerNullT:
      GetDlgItemText(id_ZWerNullT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Ref[2] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      gWrk3DSheet->redraw();
      break;
    case IDM_SetXNull: {
      gPa.Ref[0] = gCNCPos[Xp];
      ParamModify = 1;
      sprintf(str, "%1.2f", (float)gPa.Ref[0] / (float)gPa.Aufloesung);
      SetDlgItemText(id_XWerNullT, str);
      gWrk3DSheet->redraw();
      if(SettingsTabs->value() == this) {
        FlashGreen(id_XWerNullT);
      } else {
#ifdef __GNUC__
        fl_beep(FL_BEEP_NOTIFICATION);
#else
        MessageBeep(MB_OK);
#endif
      }
    }
    break;
    case IDM_SetYNull: {
      gPa.Ref[1] = gCNCPos[Yp];
      ParamModify = 1;
      sprintf(str, "%1.2f", (float)gPa.Ref[1] / (float)gPa.Aufloesung);
      SetDlgItemText(id_YWerNullT, str);
      gWrk3DSheet->redraw();
      if(SettingsTabs->value() == this) {
        FlashGreen(id_YWerNullT);
      } else {
#ifdef __GNUC__
        fl_beep(FL_BEEP_NOTIFICATION);
#else
        MessageBeep(MB_OK);
#endif
      }
    }
    break;
    case IDM_SetZNull: {
      gPa.Ref[2] = gCNCPos[Zp];
      ParamModify = 1;
      sprintf(str, "%1.2f", (float)gPa.Ref[2] / (float)gPa.Aufloesung);
      SetDlgItemText(id_ZWerNullT, str);
      gWrk3DSheet->redraw();
      if(SettingsTabs->value() == this) {
        FlashGreen(id_ZWerNullT);
      } else {
#ifdef __GNUC__
        fl_beep(FL_BEEP_NOTIFICATION);
#else
        MessageBeep(MB_OK);
#endif
      }
    }
    break;
    case id_XYNull: {
      gPa.Ref[0] = gCNCPos[Xp];
      gPa.Ref[1] = gCNCPos[Yp];
      ParamModify = 1;
      sprintf(str, "%1.2f", (float)gPa.Ref[0] / (float)gPa.Aufloesung);
      SetDlgItemText(id_XWerNullT, str);
      sprintf(str, "%1.2f", (float)gPa.Ref[1] / (float)gPa.Aufloesung);
      SetDlgItemText(id_YWerNullT, str);
      gWrk3DSheet->redraw();
    }
    break;
    case id_ZNull: {
      gPa.Ref[2] = gCNCPos[Zp];
      ParamModify = 1;
      sprintf(str, "%1.2f", (float)gPa.Ref[2] / (float)gPa.Aufloesung);
      SetDlgItemText(id_ZWerNullT, str);
      gWrk3DSheet->redraw();
    }
    break;
    }
  }
  return 0;
}
//-------------------------------------------------------------
void CNullpunktTab::FlashGreen(int id)
{
  tDialogCtlItem* pItem = pGetDlgItem(id);
  if(pItem) {
    if(pItem->pWndObject) {
      Fl::lock();
      Fl_Input* pval = (Fl_Input*)pItem->pWndObject;
      pval->color(0x00FF00FF);
      pval->redraw();
      Fl::check();
      usleep(200000);
      pval->color(FL_BACKGROUND2_COLOR);
      pval->redraw();
      Fl::check();
      Fl::unlock();
    }
  }
}
//-------------------------------------------------------------
CToolAdjustTab ::CToolAdjustTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_ToolAdjustTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_ToolAdjustTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CToolAdjustTab::~CToolAdjustTab()
{
}

//-------------------------------------------------------------
int CToolAdjustTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);

  tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_WekzeugX);
  if(pCtlItem && pCtlItem->pWndObject) {
    //    ((Fl_Box*)pCtlItem->pWndObject)-> labelfont (  fl_font());
    ((Fl_Box*)pCtlItem->pWndObject)->labelcolor(FL_GREEN);
    //    ((Fl_Box*)pCtlItem->pWndObject)-> labelsize (  fl_size()*2);
  }
  pCtlItem = pGetDlgItem(IDC_WekzeugY);
  if(pCtlItem && pCtlItem->pWndObject) {
    //((Fl_Box*)pCtlItem->pWndObject)-> labelfont (  fl_font());
    ((Fl_Box*)pCtlItem->pWndObject)->labelcolor(FL_GREEN);
    //    ((Fl_Box*)pCtlItem->pWndObject)-> labelsize (  fl_size()*2);
  }
  pCtlItem = pGetDlgItem(IDC_WekzeugZ);
  if(pCtlItem && pCtlItem->pWndObject) {
    //    ((Fl_Box*)pCtlItem->pWndObject)-> labelfont (  fl_font());
    ((Fl_Box*)pCtlItem->pWndObject)->labelcolor(FL_GREEN);
    //    ((Fl_Box*)pCtlItem->pWndObject)-> labelsize (  fl_size()*2);
  }

  return maxxy;
}
//-------------------------------------------------------------
void CToolAdjustTab::Show(void)
{

  char s[32];
  sprintf(s, "%1.2f", (float)gCNCPos[Xp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugX, s);
  sprintf(s, "%1.2f", (float)gCNCPos[Yp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugY, s);
  sprintf(s, "%1.2f", (float)gCNCPos[Zp] / (float)gPa.Aufloesung);
  SetDlgItemText(IDC_WekzeugZ, s);
  show();

  CheckRadioButton(IDC_Move0001, IDC_Move1000, IDC_Move0100);

  sprintf(s, "TOOL ADJ.\n X%1.2f Y%1.2f", // Z%1.2f
          (float)gPa.WerzeugSensor[Xp] / (float)gPa.Aufloesung, (float)gPa.WerzeugSensor[Yp] / (float)gPa.Aufloesung);
  ///(float)gPa.WerzeugSensor[Zp]/(float)gPa.Aufloesung);
  SetDlgItemText(IDC_AdjustHeight, s);
}
//-------------------------------------------------------------
int CBastelButton::handle(int event)
{
  int idc = (long int)user_data();
  if(event == FL_PUSH) {
    int left = -100;
    int right = +100;
    int bottom = -100;
    int top = +100;
    int Schritt = 100;
    if(gToolAdjustTab->IsDlgButtonChecked(IDC_Move0001))
      Schritt = 1;
    else if(gToolAdjustTab->IsDlgButtonChecked(IDC_Move0010))
      Schritt = 10;
    else if(gToolAdjustTab->IsDlgButtonChecked(IDC_Move1000))
      Schritt = 1000;
    else
      Schritt = 100;

    /*    if (gPa.Drehen==1)  {
          left  = 0;
          right = gPa.Anschlag[Xp];
          bottom= 0;
          top   = gPa.Anschlag[Yp];
        } else */
    {
      left = 0;
      right = gPa.Anschlag[Xp];
      bottom = 0;
      top = gPa.Anschlag[Yp];
    }
    switch(idc) {
    case IDC_MoveXYPP10:
      SendToCNC('W', right, top);
      break;
    case IDC_MoveXYMP10:
      SendToCNC('W', left, top);
      break;
    case IDC_MoveXYPM10:
      SendToCNC('W', right, bottom);
      break;
    case IDC_MoveXYMM10:
      SendToCNC('W', left, bottom);
      break;

    case IDC_MoveXP10:
      SendToCNC('X', right);
      break;
    case IDC_MoveXM10:
      SendToCNC('X', left);
      break;
    case IDC_MoveYP10:
      SendToCNC('Y', top);
      break;
    case IDC_MoveYM10:
      SendToCNC('Y', bottom);
      break;

    case IDC_MoveXYPP1:
      SendToCNC('w', Schritt, Schritt);
      break;
    case IDC_MoveXYMM1:
      SendToCNC('w', -Schritt, -Schritt);
      break;
    case IDC_MoveXYMP1:
      SendToCNC('w', -Schritt, Schritt);
      break;
    case IDC_MoveXYPM1:
      SendToCNC('w', Schritt, -Schritt);
      break;
    case IDC_MoveXP1:
      SendToCNC('x', Schritt);
      break;
    case IDC_MoveXM1:
      SendToCNC('x', -Schritt);
      break;
    case IDC_MoveYP1:
      SendToCNC('y', Schritt);
      break;
    case IDC_MoveYM1:
      SendToCNC('y', -Schritt);
      break;

    case IDC_MoveZP1:
      SendToCNC('z', Schritt); // gPa.Anschlag[Zp]);
      break;
    case IDC_MoveZP10:
      SendToCNC('Z', gPa.Anschlag[Zp]);
      break;
    case IDC_MoveZM10:
      SendToCNC('Z', 0);
      break;
    case IDC_MoveZM1:
      SendToCNC('z', -Schritt); // 0);
      break;
    case IDM_STOPP:
      SendToCNC('\033');
      return 1;
      // if (gOkCount) gOkCount--;
      break;
    }
    // printf ("Push Button %d\n",user_data());
  } else if(event == FL_RELEASE) {
    switch(idc) {
    case IDC_MoveXYPP10:
    case IDC_MoveXYMP10:
    case IDC_MoveXYPM10:
    case IDC_MoveXYMM10:
    case IDC_MoveXP10:
    case IDC_MoveXM10:
    case IDC_MoveYP10:
    case IDC_MoveYM10:
    case IDC_MoveZP10:
    case IDC_MoveZM10:
    case IDM_STOPP:
      SendToCNC('\033');
      // if (gOkCount) gOkCount--;
      break;

      // printf ("Release Button %d\n",user_data());

      // SendToCNC('\033');
      // if (gOkCount) gOkCount--;
    }
  }
  return Fl_Button::handle(event);
}

//-------------------------------------------------------------
int CToolAdjustTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[32];
    static bool recursion = 0;
    float f;
    switch(wParam) {
    case IDC_GehZuNULL:
      StartExecuteThread(gExecuteMode = NullPunkt);
      break;
    case id_XYNull: {
      if(FltkMessageBox("SET XY ORIGIN ?", "", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        gPa.Ref[0] = gCNCPos[Xp];
        gPa.Ref[1] = gCNCPos[Yp];
        gWrk3DSheet->redraw();
      }
    }
    break;
    case id_ZNull: {
      if(FltkMessageBox("SET Z ORIGIN ?", "", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        gPa.Ref[2] = gCNCPos[Zp];
        gWrk3DSheet->redraw();
      }
    }
    break;
    case IDC_AdjustHeight:
      StartExecuteThread(gExecuteMode = WerkzeugEinrichten);
      break;
    case IDC_Move0001:
    case IDC_Move0010:
    case IDC_Move0100:
    case IDC_Move1000:
      CheckRadioButton(IDC_Move0001, IDC_Move1000, wParam);
      break;
    case IDM_STOPP:
      StopCNC();
      break;
    }
  }
  return 0;
}
//-------------------------------------------------------------
CSettingsTab ::CSettingsTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_SettingsTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_SettingsTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CSettingsTab::~CSettingsTab()
{
}

//-------------------------------------------------------------
int CSettingsTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  tDialogCtlItem* pCtlItem = pGetDlgItem(id_VerfahrTemp);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(IDC_Spindle);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  return maxxy;
}

//-------------------------------------------------------------
void CSettingsTab::Show(void)
{
  char s[32];

  sprintf(s, "%1.0f", (float)gPa.Tempo[Tempo_Verfahr] / (float)gPa.Aufloesung);
  SetDlgItemText(id_VerfahrTempT, s);

  CheckDlgButton(id_Aufrauemen, (int)gPa.Aufraumen);

  tDialogCtlItem* pCtlItem = pGetDlgItem(id_VerfahrTemp);
  if(pCtlItem && pCtlItem->pWndObject) {
    //      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)color[AktTiefe]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Tempo[Tempo_Verfahr], PaMin.Tempo[Tempo_Verfahr]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Tempo[Tempo_Verfahr]);
  }
  pCtlItem = pGetDlgItem(IDC_Spindle);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[AktTiefe]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(255, 0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eSpindlePWM]);
  }

  CheckRadioButton(IDC_GcodeModusAuto, IDC_GcodeModus5,
                   gOkModus == 0 ? IDC_GcodeModusAuto : (gOkModus == 1 ? IDC_GcodeModus1 : IDC_GcodeModus5));

  CheckDlgButton(IDC_Isel, gIselRs232Mode);

  // Eintauch Tempo
  sprintf(s, "%1.0f", (float)gPa.Tempo[Tempo_Eintauch] / (float)gPa.Aufloesung);
  SetDlgItemText(id_EintauchTempo1T, s);
  // Bohren
  sprintf(s, "%1.0f", (float)gPa.Tempo[Tempo_Bohren] / (float)gPa.Aufloesung);
  SetDlgItemText(id_BohrTempo1T, s);
  sprintf(s, "%4.2f", (float)gPa.Tiefe[Tempo_Verfahr] / (float)gPa.Aufloesung);
  SetDlgItemText(id_HebenT, s);

  show();
}
//-------------------------------------------------------------
int CSettingsTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[132];
    static bool recursion = 0;
    int v;
    float f;
    switch(wParam) {
    case id_VerfahrTemp: {
      if(recursion)
        return 1;
      GetDlgItemText(id_VerfahrTemp, str, 20);
      int v = atoi(str);
      if(v > 0 && v < 100000) {
        gPa.Tempo[Tempo_Verfahr] = v; // * 100;
        sprintf(str, "%1.0f", (float)gPa.Tempo[Tempo_Verfahr] / (float)gPa.Aufloesung);
        SetDlgItemText(id_VerfahrTempT, str);
        ParamModify = 1;
      }
    }
    break;
    case id_VerfahrTempT:
      GetDlgItemText(id_VerfahrTempT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tempo[Tempo_Verfahr] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        recursion = 1;
        SetDlgItemInt(id_VerfahrTemp, gPa.Tempo[Tempo_Verfahr], 0);
        recursion = 0;
        ParamModify = 1;
      }
      break;
    case IDM_STOPP:
      StopCNC();
      SendToCNC('E', 0);
      Show_ExecuteState();
      break;
    case IDC_Spindle: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(IDC_Spindle, NULL, 0);
      if(v >= 0 && v < 256) {
        gPa.Maschine[eSpindlePWM] = v;
        SendToCNC('E', gPa.Maschine[eSpindlePWM]);
        Show_ExecuteState();
      }
    }
    break;
    case IDC_GcodeModusAuto:
      gOkModus = 0;
      Show();
      break;
    case IDC_GcodeModus1:
      gOkModus = 1;
      Show();
      break;
    case IDC_GcodeModus5:
      gOkModus = 5;
      Show();
      break;
    case id_EintauchTempo1T:
      GetDlgItemText(id_EintauchTempo1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tempo[Tempo_Eintauch] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      break;
    case id_BohrTempo1T:
      GetDlgItemText(id_BohrTempo1T, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tempo[Tempo_Bohren] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      break;
    case id_HebenT:
      GetDlgItemText(id_HebenT, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Tiefe[0] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      break;
    case IDC_Isel:
      gOnline = 0;
      gIselRs232Mode = IsDlgButtonChecked(IDC_Isel);
      SetIniFile(gIselRs232Mode ? "IselDxf.ini" : "GoCnc.ini");
      InitLaden();
      CheckDlgButton(IDC_Isel, gIselRs232Mode);
      break;
    case IDC_SaugerAuto:
      gSaugerAuto = IsDlgButtonChecked(IDC_SaugerAuto);
      prefs->set("SaugerAuto", gSaugerAuto ? "1" : "0");
      CheckDlgButton(IDC_SaugerAuto, gSaugerAuto);
      break;
    case id_Aufrauemen:
      gPa.Aufraumen = IsDlgButtonChecked(id_Aufrauemen);
      break;
    case IDM_Sauger: {
#if LONMSG

      int saugt = IsDlgButtonChecked(IDM_Sauger);
      if(lParam == -1)
        saugt = 0;
      else if(lParam == 1)
        saugt = 1;
      if(gLonService) {
        ManagerStruct m_SendStruct;
        memset(&m_SendStruct, 0, sizeof(m_SendStruct));
        m_SendStruct.service = UNACKD_RPT;
        m_SendStruct.MSGaddr.snode.type = SUBNET_NODE;
        m_SendStruct.MSGaddr.snode.domain = 0;
        m_SendStruct.MSGaddr.snode.node = 4;
        m_SendStruct.MSGaddr.snode.subnet = 9;
        m_SendStruct.MSGaddr.snode.rpt_timer = 0;
        m_SendStruct.MSGaddr.snode.retry = 2;
        m_SendStruct.MSGaddr.snode.tx_timer = 11;
        m_SendStruct.MSGLen = 3;
        int sel = 16374;
        if(0) { // Lampe
          m_SendStruct.MSGaddr.snode.node = 6;
          m_SendStruct.MSGaddr.snode.subnet = 9;
          sel = 16377;
        }
        m_SendStruct.code = 0x80 + (sel / 256);
        m_SendStruct.MSG[0] = (sel & 0xFF);
        m_SendStruct.MSG[2] = saugt ? 0x01 : 0x0;
        m_SendStruct.MSG[1] = saugt ? 0xC0 : 0x0;
        if(gLonService->Send_Buff((unsigned char*)&m_SendStruct, MgSHaederSize + m_SendStruct.MSGLen)) {
          CheckDlgButton(IDM_Sauger, (saugt));
        }
      }
#endif
    }
    break;
    }
  }
  return 0;
}
static int LonMessageInit = 0;
void LonMessage(unsigned char* msg)
{
#if LONMSG
  if(gSettingsTab) {
    if(LonMessageInit == 0) {
      gSettingsTab->EnableDlgItem(IDM_Sauger, 1);
      LonMessageInit = 1;
      ManagerStruct m_SendStruct;
      memset(&m_SendStruct, 0, sizeof(m_SendStruct));
      m_SendStruct.service = REQUEST;
      m_SendStruct.MSGaddr.snode.type = SUBNET_NODE;
      m_SendStruct.MSGaddr.snode.domain = 0;
      m_SendStruct.MSGaddr.snode.node = 4;
      m_SendStruct.MSGaddr.snode.subnet = 9;
      m_SendStruct.MSGaddr.snode.rpt_timer = 0;
      m_SendStruct.MSGaddr.snode.retry = 2;
      m_SendStruct.MSGaddr.snode.tx_timer = 11;
      m_SendStruct.MSGLen = 1;
      m_SendStruct.code = NM_nv_fetch | NM_opcode_base;
      m_SendStruct.MSG[0] = 9;
      if(0) { // Lampe
        m_SendStruct.MSG[0] = 6;
        m_SendStruct.MSGaddr.snode.node = 6;
        m_SendStruct.MSGaddr.snode.subnet = 9;
      }
      gLonService->Send_Buff((unsigned char*)&m_SendStruct, MgSHaederSize + m_SendStruct.MSGLen);
    }
    ApllicationStruct* pManagerStruct = (ApllicationStruct*)msg;
    if((pManagerStruct->node == 4) && (pManagerStruct->subnet == 9)) {
      gSettingsTab->CheckDlgButton(IDM_Sauger, pManagerStruct->MSG[1] != 0);
    }
  }
#endif
}

//-------------------------------------------------------------
CLaserTab ::CLaserTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_LaserTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_LaserTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CLaserTab::~CLaserTab()
{
}

//-------------------------------------------------------------
BOOL CLaserTab::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}

//-------------------------------------------------------------
int CLaserTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  SetButtonImage(IDM_Tiefe1, 19);
  SetButtonImage(IDM_Tiefe2, 20);
  SetButtonImage(IDM_Tiefe3, 21);
  SetButtonImage(IDM_Tiefe4, 22);
  SetButtonImage(IDM_Tiefe5, 23);
  SetButtonImage(IDM_Tiefe6, 24);

  tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_FocusPower);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(IDC_LaserPower);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(IDC_LaserSpeed);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  return maxxy;
}

//-------------------------------------------------------------
void CLaserTab::Show(void)
{
  char s[32];
  tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_FocusPower);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_BLUE);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(255, 0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eFocusPower]);
  }
  pCtlItem = pGetDlgItem(IDC_LaserPower);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_BLUE);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(255, 0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eLaserPower]);
  }
  pCtlItem = pGetDlgItem(IDC_LaserSpeed);
  if(pCtlItem && pCtlItem->pWndObject) {
    //((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_BLUE);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax.Tempo[1], PaMin.Tempo[1]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eLaserSpeed]);
  }

  SetDlgItemInt(IDC_FocusPowerT, gPa.Maschine[eFocusPower], 0);
  SetDlgItemInt(IDC_LaserPowerT, gPa.Maschine[eLaserPower], 0);
  SetDlgItemInt(IDC_LaserSpeedT, gPa.Maschine[eLaserSpeed], 0);
  CheckRadioButton(IDC_LaserPowerOnF, IDM_LASERENGRAVE, LaserState);
  CheckRadioButton(IDM_Tiefe1, IDM_TOOLBAR, IDM_Tiefe1+AktTiefe-1);
  show();
}
//-------------------------------------------------------------
int CLaserTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[132];
    static bool recursion = 0;
    int v;
    float f;
    switch(wParam) {
    case IDM_LASERENGRAVE:
      LaserState = wParam;
      CheckRadioButton(IDC_LaserPowerOnF, IDM_LASERENGRAVE, LaserState);
      StartExecuteThread(gExecuteMode = LaserGravur | Linie | Buchstaben | Kreisbogen);
      goto doit;
    case IDM_STOPP:
      StopCNC();
      break;
    case IDC_LaserPowerOnF:
      SendToCNC('E', gPa.Maschine[eLaserPower]);
      goto doit;
    case IDC_FocusPowerOnF:
      SendToCNC('E', gPa.Maschine[eFocusPower]);
      goto doit;
    case IDC_PowerOff:
      SendToCNC('E', 0);
      goto doit;
doit : {
        LaserState = wParam;
        CheckRadioButton(IDC_LaserPowerOnF, IDM_LASERENGRAVE, LaserState);
      }
      break;
    case IDM_Tiefe1:
    case IDM_Tiefe2:
    case IDM_Tiefe3:
    case IDM_Tiefe4:
    case IDM_Tiefe5:
    case IDM_Tiefe6:

      AktTiefe = wParam - IDM_Tiefe1 + 1;
      if(gWrk3DSheet->visible()) {
        // gWrk3DSheet->invalidate();
        gWrk3DSheet->redraw();
      }
      Fl::lock();
      Fl::check();
      Fl::unlock();
      CheckRadioButton(IDM_Tiefe1, IDM_Tiefe6, wParam);
      Fl::lock();
      Fl::check();
      Fl::unlock();

      break;

    case IDC_FocusPower: {
      if(recursion)
        return 1;
      v = GetDlgItemInt(IDC_FocusPower, NULL, 50);
      if(v >= 0 && v <= 255) {
        gPa.Maschine[eFocusPower] = v;
        if(LaserState == IDC_FocusPowerOnF)
          SendToCNC('E', gPa.Maschine[eFocusPower]);
        SetDlgItemInt(IDC_FocusPowerT, gPa.Maschine[eFocusPower], 0);
        ParamModify = 1;
      }
    }
    break;
    case IDC_FocusPowerT:
      GetDlgItemText(IDC_FocusPowerT, str, 20);
      if(sscanf(str, "%d", &v) == 1) {
        recursion = 1;
        gPa.Maschine[eFocusPower] = v;
        if(LaserState == IDC_FocusPowerOnF)
          SendToCNC('E', gPa.Maschine[eFocusPower]);
        SetDlgItemInt(IDC_FocusPower, gPa.Maschine[eFocusPower], 0);
        recursion = 0;
        ParamModify = 1;
      }
      break;
    case IDC_LaserPower: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(IDC_LaserPower, NULL, 50);
      if(v >= 0 && v <= 255) {
        gPa.Maschine[eLaserPower] = v;
        if(LaserState == IDC_LaserPowerOnF)
          SendToCNC('E', gPa.Maschine[eLaserPower]);
        SetDlgItemInt(IDC_LaserPowerT, gPa.Maschine[eLaserPower], 0);
        ParamModify = 1;
      }
    }
    break;
    case IDC_LaserPowerT:
      GetDlgItemText(IDC_LaserPowerT, str, 20);
      if(sscanf(str, "%d", &v) == 1) {
        recursion = 1;
        gPa.Maschine[eLaserPower] = v;
        if(LaserState == IDC_LaserPowerOnF)
          SendToCNC('E', gPa.Maschine[eLaserPower]);
        SetDlgItemInt(IDC_LaserPower, gPa.Maschine[eLaserPower], 0);
        recursion = 0;
        ParamModify = 1;
      }
      break;
    case IDC_LaserSpeed: {
      if(recursion)
        return 1;
      int v = GetDlgItemInt(IDC_LaserSpeed, NULL, 50);
      if(v >= 5 && v <= 3000) {
        gPa.Maschine[eLaserSpeed] = v;
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Maschine[eLaserSpeed]);
        SetDlgItemInt(IDC_LaserSpeedT, gPa.Maschine[eLaserSpeed], 0);
        ParamModify = 1;
      }
    }
    break;
    case IDC_LaserSpeedT:
      GetDlgItemText(IDC_LaserSpeedT, str, 20);
      if(sscanf(str, "%d", &v) == 1) {
        recursion = 1;
        gPa.Maschine[eLaserSpeed] = v;
        if(gOkCount <= 1)
          SendToCNC('t', gPa.Maschine[eLaserSpeed]);
        SetDlgItemInt(IDC_LaserSpeed, gPa.Maschine[eLaserSpeed], 0);
        recursion = 0;
        ParamModify = 1;
      }
      break;
      /*    case IDC_Spindle:
          {
            if (recursion) return 1;
            int v = GetDlgItemInt(IDC_Spindle,NULL,0);
            if (v>= 0  && v < 256)
            {
              SpindlePWM = v;
              SendToCNC('E',SpindlePWM);
            }
          }
          break;*/
    }
  }
  return 0;
}

//-------------------------------------------------------------
CGcodeTab ::CGcodeTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_GcodeTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_GcodeTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CGcodeTab::~CGcodeTab()
{
}

//-------------------------------------------------------------
BOOL CGcodeTab::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}

//-------------------------------------------------------------
int CGcodeTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_GcodeRapid);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(IDC_GcodeFeed);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }
  pCtlItem = pGetDlgItem(IDC_PowerPercent);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
  }

  SetButtonImage(IDM_Tiefe1, 19);
  SetButtonImage(IDM_Tiefe2, 20);
  SetButtonImage(IDM_Tiefe3, 21);
  SetButtonImage(IDM_Tiefe4, 22);
  SetButtonImage(IDM_Tiefe5, 23);
  SetButtonImage(IDM_Tiefe6, 24);

  return maxxy;
}

//-------------------------------------------------------------
void CGcodeTab::Show(void)
{
  char s[32];

  // AktTiefe=1;
  CheckDlgButton(IDC_RapidOverwrite, gPa.Maschine[eGcodeRapid] != 100);
  CheckDlgButton(IDC_FeedOverwrite, gPa.Maschine[eGcodeFeed] != 100);
  CheckDlgButton(IDC_PowerOverwrite, gPa.Maschine[ePowerPercent]);

  CheckDlgButton(IDC_AB_SWAP, gPa.Maschine[eXYSwap]);
  CheckDlgButton(IDC_Drehachse, gPa.Maschine[eDrehachse]);
  CheckDlgButton(IDC_HideRapidTrans, bHideRapidTrans);

  CheckDlgButton(IDM_LASERENGRAVE,gLaserEnable);

  tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_Durchmesser);
  if(pCtlItem && pCtlItem->pWndObject) {
    if(gPa.Maschine[eDurchmesser] < 1000) {
      ((Fl_Counter*)pCtlItem->pWndObject)->step(0.1, 1.0);
    } else {
      ((Fl_Counter*)pCtlItem->pWndObject)->step(1, 10);
    }
    //    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(1, 6000);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eDurchmesser] / 100.0);
  }

  gWrk3DSheet->redraw();

  // tDialogCtlItem *
  pCtlItem = pGetDlgItem(IDC_GcodeRapid);
  if(pCtlItem && pCtlItem->pWndObject) {
    //      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)color[AktTiefe]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(300, 10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eGcodeRapid]);
  }
  pCtlItem = pGetDlgItem(IDC_GcodeFeed);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[1]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(300, 10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[eGcodeFeed]);
  }
  pCtlItem = pGetDlgItem(IDC_PowerPercent);
  if(pCtlItem && pCtlItem->pWndObject) {
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[1]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(150, 0);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gPa.Maschine[ePowerPercent]);
  }

  SetDlgItemInt(IDC_GcodeRapidT, gPa.Maschine[eGcodeRapid], 0);
  SetDlgItemInt(IDC_GcodeFeedT, gPa.Maschine[eGcodeFeed], 0);
  SetDlgItemInt(IDC_PowerPercentT, gPa.Maschine[ePowerPercent], 0);
  CheckRadioButton(IDM_Tiefe1, IDM_TOOLBAR, IDM_Tiefe1+AktTiefe-1);

  show();
}
//-------------------------------------------------------------
int CGcodeTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[132];
    static bool recursion = 0;
    int v;
    float f;
    switch(wParam) {
    case IDM_Tiefe1:
    case IDM_Tiefe2:
    case IDM_Tiefe3:
    case IDM_Tiefe4:
    case IDM_Tiefe5:
    case IDM_Tiefe6:
      AktTiefe   = wParam - IDM_Tiefe1 + 1;
      Show();
      if(gWrk3DSheet->visible()) {
        // gWrk3DSheet->CNCFreigeben();
        // gWrk3DSheet->invalidate();
        gWrk3DSheet->redraw();
      }
      Fl::lock();
      Fl::check();
      Fl::unlock();
      CheckRadioButton(IDM_Tiefe1, IDM_TOOLBAR, wParam);
      Fl::lock();
      Fl::check();
      Fl::unlock();
      break;

    case IDM_LASERENGRAVE:
      gLaserEnable = IsDlgButtonChecked(IDM_LASERENGRAVE);
      Show_ExecuteState();
      SendToCNC('E', 0);
      break;
    case IDM_SPINDLE:
      gLaserEnable=0;
      if(!gSpindleOn) {
        SendToCNC('E', ((_max(1, gPa.Maschine[eSpindlePWM])) * 255) / 100);
      } else {
        SendToCNC('E', 0);
      }
      SendToCNC('V', gSpindleOn ? 1 : -1);
      Show_ExecuteState();
      Show();
      break;
    case IDM_RUNGCODE:
      if (gLaserEnable)
        StartExecuteThread(gExecuteMode = LaserGravur | GCodeLinine);
      else
        StartExecuteThread(gExecuteMode = GCodeLinine);
      break;
    case IDM_STOPP:
      StopCNC();
      break;

    case IDC_RapidOverwrite:
      gPa.Maschine[eGcodeRapid] = 100;
      Show();
      break;
    case IDC_FeedOverwrite:
      gPa.Maschine[eGcodeFeed] = 100;
      Show();
      break;
    case IDC_PowerOverwrite:
      gPa.Maschine[ePowerPercent] = IsDlgButtonChecked(IDC_PowerOverwrite) ? 100 : 0;
      Show();
      break;

    case IDC_GcodeRapid: {
      if(recursion)
        return 1;
      v = GetDlgItemInt(IDC_GcodeRapid, NULL, 0);
      if(v > 10 && v < 300) {
        gPa.Maschine[eGcodeRapid] = v;
        SetDlgItemInt(IDC_GcodeRapidT, gPa.Maschine[eGcodeRapid], 0);
        ParamModify = 1;
      }
      CheckDlgButton(IDC_RapidOverwrite, gPa.Maschine[eGcodeRapid] != 100);
    }
    break;
    case IDC_GcodeRapidT:
      v = GetDlgItemInt(IDC_GcodeRapidT, NULL, 20);
      if(v > 10 && v < 300) {
        gPa.Maschine[eGcodeRapid] = v;
        recursion = 1;
        SetDlgItemInt(IDC_GcodeRapid, gPa.Maschine[eGcodeRapid], 0);
        recursion = 0;
        ParamModify = 1;
      }
      CheckDlgButton(IDC_RapidOverwrite, gPa.Maschine[eGcodeRapid] != 100);
      break;

    case IDC_GcodeFeed: {
      if(recursion)
        return 1;
      v = GetDlgItemInt(IDC_GcodeFeed, NULL, 50);
      if(v > 10 && v < 300) {
        gPa.Maschine[eGcodeFeed] = v;
        SetDlgItemInt(IDC_GcodeFeedT, gPa.Maschine[eGcodeFeed], 0);
        ParamModify = 1;
      }
      CheckDlgButton(IDC_FeedOverwrite, gPa.Maschine[eGcodeFeed] != 100);
    }
    break;
    case IDC_GcodeFeedT:
      v = GetDlgItemInt(IDC_GcodeFeedT, NULL, 0);
      if(v > 10 && v < 300) {
        gPa.Maschine[eGcodeFeed] = v;
        recursion = 1;
        SetDlgItemInt(IDC_GcodeFeed, gPa.Maschine[eGcodeFeed], 0);
        recursion = 0;
        ParamModify = 1;
      }
      CheckDlgButton(IDC_FeedOverwrite, gPa.Maschine[eGcodeFeed] != 100);
      break;

    case IDC_PowerPercent: {
      if(recursion)
        return 1;
      v = GetDlgItemInt(IDC_PowerPercent, NULL, 50);
      if(v >= 0 && v < 300) {
        gPa.Maschine[ePowerPercent] = v;
        SetDlgItemInt(IDC_PowerPercentT, gPa.Maschine[ePowerPercent], 0);
        ParamModify = 1;
      }
      CheckDlgButton(IDC_PowerOverwrite, gPa.Maschine[ePowerPercent]);
      gWrk3DSheet->redraw();
      if (gLaserEnable) {
        SendToCNC('E', (gPa.Maschine[ePowerPercent] * 255) / 100);
      }
    }
    break;
    case IDC_PowerPercentT:
      v = GetDlgItemInt(IDC_PowerPercentT, NULL, 0);
      if(v >= 0 && v < 300) {
        gPa.Maschine[ePowerPercent] = v;
        recursion = 1;
        SetDlgItemInt(IDC_PowerPercent, gPa.Maschine[ePowerPercent], 0);
        recursion = 0;
        ParamModify = 1;
      }
      CheckDlgButton(IDC_PowerOverwrite, gPa.Maschine[ePowerPercent]);
      gWrk3DSheet->redraw();
      if (gLaserEnable) {
        SendToCNC('E', (gPa.Maschine[ePowerPercent] * 255) / 100);
      }
      break;
    case IDC_AB_SWAP:
      gPa.Maschine[eXYSwap] = IsDlgButtonChecked(IDC_AB_SWAP);
      ParamModify = 1;
      if(!gCNCAktiv && gObjectManager)
        gObjectManager->FileOpen(NULL, 1);

      break;
    case IDC_Drehachse:
      gPa.Maschine[eDrehachse] = IsDlgButtonChecked(IDC_Drehachse);
      ParamModify = 1;
      gWrk3DSheet->redraw();
      break;
    case IDC_HideRapidTrans:
      bHideRapidTrans = IsDlgButtonChecked(IDC_HideRapidTrans);
      gWrk3DSheet->redraw();
      break;
    case IDC_Durchmesser:
      // tDialogCtlItem* pCtlItem ;
      GetDlgItemText(IDC_Durchmesser, str, 20);
      if(sscanf(str, "%f", &f) == 1) {
        gPa.Maschine[eDurchmesser] = (CNCINT)roundf(f * (float)gPa.Aufloesung);
        ParamModify = 1;
      }
      gWrk3DSheet->redraw();
      break;


    case IDC_GCODEPosStart:
      if (Fl::focus()== GetDlgItem(IDC_GCODEPosStart)) {
        int posStart = GetDlgItemInt(IDC_GCODEPosStart, NULL, 0);
        int posEnd = GetDlgItemInt(IDC_GCODEPosEnd, NULL, 0);
        if(posStart > posEnd) {
          SetDlgItemInt(IDC_GCODEPosEnd, posStart + 10, 0);
        }
        SetDlgItemInt(IDC_GCODEStartTxt, posStart, 0);
        gNAuswahl=1;
        gWrk3DSheet->redraw();
      }
      break;
    case IDC_GCODEStartTxt:
      GetDlgItemText(IDC_GCODEStartTxt, str, 20);
      if(sscanf(str, "%d", &v) == 1) {
        recursion = 1;
        SetDlgItemInt(IDC_GCODEPosStart, v, 0);
        recursion = 0;
        gNAuswahl=1;
      }
      break;


    case IDC_GCODEPosEnd:
      if (Fl::focus()== GetDlgItem(IDC_GCODEPosEnd)) {
        int posStart = GetDlgItemInt(IDC_GCODEPosStart, NULL, 0);
        int posEnd   = GetDlgItemInt(IDC_GCODEPosEnd, NULL, 0);
        if(posStart > posEnd) {
          SetDlgItemInt(IDC_GCODEPosStart, posEnd - 10, 0);
        }
        SetDlgItemInt(IDC_GCODEEndTxt, posEnd, 0);
        gNAuswahl=1;
        gWrk3DSheet->redraw();

      }
      break;
    case IDC_GCODEEndTxt:
      GetDlgItemText(IDC_GCODEEndTxt, str, 20);
      if(sscanf(str, "%d", &v) == 1) {
        recursion = 1;
        SetDlgItemInt(IDC_GCODEPosEnd, v, 0);
        recursion = 0;
        gWrk3DSheet->redraw();
        gNAuswahl=1;
      }
      break;
    }
  }
  return 0;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
CPerspektive ::CPerspektive(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_Perspektive2;
  if(X > 100)
    pCtlItem = rg_Perspektive;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    if(X > 100)
      memcpy(m_pDialogCtlItem, &rg_Perspektive, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
    else
      memcpy(m_pDialogCtlItem, &rg_Perspektive2, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CPerspektive::~CPerspektive()
{
}

//-------------------------------------------------------------
BOOL CPerspektive::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      butt->align(FL_ALIGN_INSIDE | FL_ALIGN_WRAP | FL_ALIGN_CLIP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT | FL_ALIGN_WRAP);
    // butt ->align(FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP
    //| FL_ALIGN_RIGHT|FL_ALIGN_TEXT_NEXT_TO_IMAGE| FL_ALIGN_TEXT_OVER_IMAGE
    //);
  }
  return 0;
}
BOOL CPerspektive::SetSelectionColor(int nIDDlgItem, int iColor)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    // butt->type(102);
    butt->type(FL_RADIO_BUTTON);
    butt->selection_color((Fl_Color)iColor);
  }
  return 0;
}
//-------------------------------------------------------------

//-------------------------------------------------------------
int CPerspektive::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  /*  tDialogCtlItem * pCtlItem = pGetDlgItem(id_VerfahrTemp);
    if (pCtlItem && pCtlItem->pWndObject)
    {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->type(FL_VERT_NICE_SLIDER);
    }*/
  SetButtonImage(IDM_Zoom1, 17);
  // SetButtonImage(IDM_Zoom2,15);
  SetButtonImage(IDM_ZoomGanz, 18);
  // SetButtonImage(IDM_ZoomMax,16);
  if(left > 100)
    ShowDlgItem(IDC_ModeFullscreen, 0);
  SetButtonImage(IDC_ModeFullscreen, imFullSize);

  SetSelectionColor(IDC_MausMoveXY, FL_GREEN);
  SetSelectionColor(IDC_MausMovez, FL_BLUE);
  SetSelectionColor(IDC_RotateXY, FL_RED);
  SetSelectionColor(IDC_SelectXY, FL_YELLOW);

  return maxxy;
}

//-------------------------------------------------------------
void CPerspektive::Show(void)
{
  extern float eye[3];
  extern float center[3];
  extern float RotateVal[3];
  extern float LightPos[3];
  extern float LightColor[3];
  extern float Perspective;
  extern int ShowDebug;

  char s[32];

  for(int idc = IDC_CenterX; idc <= IDC_BackValue; idc++) {
    tDialogCtlItem* pCtlItem = pGetDlgItem(idc);
    if(pCtlItem && pCtlItem->pWndObject) {
      /*if (idc  == IDC_LightColorR)
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_RED);
      else if (idc  == IDC_LightColorG)
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_GREEN);
      else if (idc  == IDC_LightColorB)
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color(FL_BLUE);*/
      // else ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)Farbe[1+idc%3]);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(100, -100);
      switch(idc) {
      case IDC_CenterX:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(center[0] * 50.0));
        break;
      case IDC_CenterY:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(center[1] * 50.0));
        break;
      case IDC_CenterZ:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(center[1] * 50.0));
        break;
      case IDC_EyeX:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(eye[0] * 20.0));
        break;
      case IDC_EyeY:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(eye[1] * 20.0));
        break;
      case IDC_EyeZ:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(eye[2] * 20.0));
        break;
      case IDC_RotationX:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(RotateVal[0] / 2.0));
        break;
      case IDC_RotationY:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(RotateVal[1] / 2.0));
        break;
      case IDC_RotationZ:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(RotateVal[2] / 2.0));
        break;
      case IDC_LightPosX:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(LightPos[0] * 5)); // 2.0));
        break;
      case IDC_LightPosY:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(LightPos[1] * 5)); // 2.0));
        break;
      case IDC_LightPosZ:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(LightPos[2] * 1)); // 2.0));
        break;
      case IDC_LightColorR:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((LightColor[0] - 1.0001) * 100));
        break;
      case IDC_LightColorG:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((LightColor[1] - 1.0001) * 100));
        break;
      case IDC_LightColorB:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((LightColor[2] - 1.0001) * 100));
        break;
      case IDC_Transparency:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((gAlphaTransp - 1.0001) * 100));
        break;
      case IDC_GrayOut:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((gGrayOut - 1.0001) * 100));
        break;
      case IDC_BackValue:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(((gBackground - 1.0001) * 100));
        break;
      case IDC_Perspective:
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(89, 1);
        ((Fl_Value_Slider*)pCtlItem->pWndObject)->value((int)(Perspective));
        break;
      }
    }
  }

  // CheckDlgButton(IDC_ShowDebug,(int)(ShowDebug!=0));

  CheckRadioButton(IDC_LookAt1, IDC_LookAt4, gLookAtIx + IDC_LookAt1);
  CheckDlgButton(IDC_Show3DFace, gbFillPolygon);
  CheckDlgButton(IDC_Background, gBackground > 0.3);
  CheckDlgButton(IDC_LookClip, gLookClip);
  CheckDlgButton(IDC_TouchScreen, bTouchScreen);

  CheckDlgButton(IDC_MausMoveXY, gMausMode == 'm');
  CheckDlgButton(IDC_MausMovez, gMausMode == 'z');
  CheckDlgButton(IDC_RotateXY, gMausMode == 'r');
  CheckDlgButton(IDC_SelectXY, gMausMode == 's');

  /*
  sprintf(s,"%1.0f",(float)Pa.Tempo[Tempo_Verfahr]/(float)Pa[oAufloesung]);
  SetDlgItemText(id_VerfahrTempT,s);

    tDialogCtlItem * pCtlItem = pGetDlgItem(id_VerfahrTemp);
    if (pCtlItem && pCtlItem->pWndObject)
    {
    //      ((Fl_Value_Slider*)pCtlItem->pWndObject)->selection_color((Fl_Color)color[AktTiefe]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(10);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(PaMax[oTempo+Tempo_Verfahr],PaMin[oTempo+Tempo_Verfahr]);
    ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(Pa.Tempo[Tempo_Verfahr]);
    }

      // Eintauch Tempo
      sprintf(s,"%1.0f",(float)Pa.Tempo[Tempo_Eintauch]/(float)Pa[oAufloesung]);
  SetDlgItemText(id_EintauchTempo1T,s);
  // Bohren
  sprintf(s,"%1.0f",(float)Pa.Tempo[Tempo_Bohren]/(float)Pa[oAufloesung]);
  SetDlgItemText(id_BohrTempo1T,s);
  sprintf(s,"%4.2f",(float)Pa[oTiefe+Tempo_Verfahr]/(float)Pa[oAufloesung]);
  SetDlgItemText(id_HebenT,s);
  */
  show();
}
//-------------------------------------------------------------
int CPerspektive::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[32];
    static bool recursion = 0;
    float f;
    if(recursion)
      return 1;
    recursion = 1;
    extern float eye[3];
    extern float center[3];
    extern float RotateVal[3];
    extern float LightPos[3];
    extern float LightColor[3];
    extern float Perspective;
    char s[32];

    tDialogCtlItem* pCtlItem = pGetDlgItem(wParam);
    if(pCtlItem && pCtlItem->pWndObject)
      if(pCtlItem->type == eSliderCtl) {
        switch(wParam) {
        case IDC_CenterX:
          center[0] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 50.0;
          break;
        case IDC_CenterY:
          center[1] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 50.0;
          break;
        case IDC_CenterZ:
          center[2] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 50.0;
          break;
        case IDC_EyeX:
          eye[0] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 20.0;
          break;
        case IDC_EyeY:
          eye[1] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 20.0;
          break;
        case IDC_EyeZ:
          eye[2] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 20.0;
          break;
        case IDC_RotationX:
          RotateVal[0] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 2.0;
          break;
        case IDC_RotationY:
          RotateVal[1] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 2.0;
          break;
        case IDC_RotationZ:
          RotateVal[2] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 2.0;
          break;
        case IDC_Perspective:
          Perspective = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value();
          break;

        case IDC_LightPosX:
          LightPos[0] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 0.20;
          break;
        case IDC_LightPosY:
          LightPos[1] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 0.20;
          break;
        case IDC_LightPosZ:
          LightPos[2] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() * 1.0;
          break;
        case IDC_LightColorR:
          LightColor[0] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        case IDC_LightColorG:
          LightColor[1] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        case IDC_LightColorB:
          LightColor[2] = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        case IDC_Transparency:
          gAlphaTransp = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        case IDC_GrayOut:
          gGrayOut = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        case IDC_BackValue:
          gBackground = (float)((Fl_Value_Slider*)pCtlItem->pWndObject)->value() / 100.0 + 1.;
          break;
        }
        extern float Zoom;
        printf("Center  %3.1f %3.1f %3.1f Eye %3.1f %3.1f %3.1f Rotate %3.1f %3.1f %3.1f Per %2.0f Zoom %0.2f\n",
               center[0], center[1], center[2], eye[0], eye[1], eye[2], RotateVal[0], RotateVal[1], RotateVal[2],
               Perspective, Zoom);
        extern int gDreiDFace;
        if(gDreiDFace) {
          printf("3D Light Pos %3.1f %3.1f %3.1f RGB %3.1f %3.1f %3.1f Tr%3.1f Gr%3.1f Bk%3.1f\n", LightPos[0],
                 LightPos[1], LightPos[2], LightColor[0], LightColor[1], LightColor[2], gAlphaTransp, gGrayOut,
                 gBackground);
        }
        // gWrk3DSheet->invalidate();
        RotateValSoll[0]=RotateVal[0];
        RotateValSoll[1]=RotateVal[1];
        RotateValSoll[2]=RotateVal[2];
        gWrk3DSheet->redraw();
      } else {
        if(wParam == IDC_ModeFullscreen) {
          void cbFullScreen(Fl_Button*, void*);
          cbFullScreen(NULL, 0);
        } else if(wParam == IDC_TouchScreen) {
          bTouchScreen = IsDlgButtonChecked(IDC_TouchScreen);
          prefs->set("TouchScreen", bTouchScreen);
        } else {
          switch(wParam) {
          case IDC_MausMoveXY:
            gMausMode = 'm';
            recursion = 0;
            gWrk3DSheet->redraw();
            return 1;
          case IDC_MausMovez:
            gMausMode = 'z';
            recursion = 0;
            gWrk3DSheet->redraw();
            return 1;
          case IDC_RotateXY:
            gMausMode = 'r';
            recursion = 0;
            gWrk3DSheet->redraw();
            return 1;
          case IDC_SelectXY:
            gMausMode = 's';
            recursion = 0;
            gWrk3DSheet->redraw();
            return 1;

          case IDM_ZoomGanz:
            wParam = 'z';
            break;
          case IDM_Zoom1:
            wParam = 'x';
            break;
          case IDC_GetDefault:
            wParam = 'd';
            break;
          case IDC_ShowTop:
            wParam = 't';
            break;
          case IDC_ShowBottom:
            wParam = 'b';
            break;
          case IDC_ShowEyeDef:
            wParam = 'w';
            break;
          case IDC_ShowLeft:
            wParam = 'l';
            break;
          case IDC_ShowRight:
            wParam = 'r';
            break;
          case IDC_ShowFront:
            wParam = 'v';
            break;
          case IDC_ShowBack:
            wParam = 'h';
            break;
          case IDC_SetDefault:
            wParam = 'i';
            break;
          case IDC_LookAt1:
            wParam = '1';
            break;
          case IDC_LookAt2:
            wParam = '2';
            break;
          case IDC_LookAt3:
            wParam = '3';
            break;
          case IDC_LookAt4:
            wParam = '4';
            break;
          case IDC_LookClip:
            wParam = 'c';
            break;
          case IDC_Show3DFace:
            wParam = 'f';
            break;
          case IDC_Background:
            wParam = 'e';
            break;
          }

          Fl::e_keysym = wParam;
          Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
          // sleep() and/or Fl::wait() as appropriate
          // Fl::handle_(FL_KEYUP, gWrk3DSheet);
          // gWrk3DSheet->WindowProc(WM_COMMAND,wParam,lParam);
          Show();
        }
      }
    recursion = 0;
  }
  return 0;
}

void SettingsShow(int show)
{
  if(gPerspektive->visible()) {
    if(show) {
      gPerspektive->Show();
    }
  }
}

//-------------------------------------------------------------
//-------------------------------------------------------------

CDXFOptimize::CDXFOptimize(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_DXFOptimize;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_DXFOptimize, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CDXFOptimize::~CDXFOptimize()
{
}

//-------------------------------------------------------------
BOOL CDXFOptimize::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}

//-------------------------------------------------------------
int CDXFOptimize::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  SetButtonImage(IDC_SetColorRed, 19);
  SetButtonImage(IDC_SetColorYellow, 20);
  SetButtonImage(IDC_SetColorGreen, 21);
  SetButtonImage(IDC_SetColorCyan, 22);
  SetButtonImage(IDC_SetColorBlue, 23);
  SetButtonImage(IDC_SetColorMagenta, 24);
  return maxxy;
}

//-------------------------------------------------------------
void CDXFOptimize::Show(void)
{
  show();
}
//-------------------------------------------------------------
int CDXFOptimize::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    if(gObjectManager == NULL)
      return 0;
    switch(wParam) {
    case IDC_Activate:
    case IDC_DeAcitvate:
      gObjectManager->AcivateDXF(wParam == IDC_Activate);
      gWrk3DSheet->redraw();
      break;
    case IDC_OptimizeDXF:
      gObjectManager->OptimizeDXF();
      gWrk3DSheet->redraw();
      break;
    case IDC_MakeDrills:
      gObjectManager->MakeDrills();
      gWrk3DSheet->redraw();
      break;
    case IDC_FlipDXF:
      gObjectManager->FlipDXF();
      gWrk3DSheet->redraw();
      break;
    case IDC_ExpandDXF015:
    case IDC_ExpandDXF100:
      gObjectManager->ExpandDXF(wParam == IDC_ExpandDXF015 ? 0.15 : 1.0);
      gWrk3DSheet->redraw();
      break;
    case IDC_ShrinkDXF015:
    case IDC_ShrinkDXF100:
      gObjectManager->ExpandDXF(wParam == IDC_ShrinkDXF015 ? -0.15 : -1.0);
      gWrk3DSheet->redraw();
      break;
    case IDC_SetColorRed:
    case IDC_SetColorYellow:
    case IDC_SetColorGreen:
    case IDC_SetColorCyan:
    case IDC_SetColorBlue:
    case IDC_SetColorMagenta:
      gObjectManager->SetColorDXF(wParam - IDC_SetColorRed + 1);
      gObjectManager->Selection(0);
      gWrk3DSheet->redraw();
      break;
    case IDC_SelectionAll:
      gObjectManager->Selection(1);
      gWrk3DSheet->redraw();
      break;
    case IDC_SelectionNone:
      gObjectManager->Selection(0);
      gWrk3DSheet->redraw();
      break;
    case IDC_SaveDXF:
      gObjectManager->SaveDXF();
      gWrk3DSheet->redraw();
      break;
    }
  }
  return 0;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
CToolBox ::CToolBox(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_ToolBox;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_ToolBox, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
  ParamModify = 0;
}

//-------------------------------------------------------------//-------------------------------------------------------------
CToolBox ::~CToolBox()
{
}
//-------------------------------------------------------------
BOOL CToolBox::SetSelectionColor(int nIDDlgItem, int iColor)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    // butt->type(102);
    butt->type(FL_RADIO_BUTTON);
    butt->selection_color((Fl_Color)iColor);
  }
  return 0;
}
//-------------------------------------------------------------
BOOL CToolBox::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}
//-------------------------------------------------------------

int CToolBox ::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  // fl_font(FL_FREE_FONT,10);
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style);

  SetButtonImage(IDC_Exit, imExit);
  SetButtonImage(IDM_UPDATE, 3);
  SetButtonImage(IDM_FILEOPEN, 1);
  SetButtonImage(IDM_FILESAVE, 2);
  SetButtonImage(IDM_ShowDXFTree, imNewDoc);
  SetButtonImage(IDM_DXFSPEZIAL, 5);
  SetButtonImage(IDM_NULLPUNKT, 9);
  //  SetButtonImage(IDM_Refernezfahrt,14);
  //  SetButtonImage(IDC_GehZuNULL,imWerkzeug);
  SetButtonImage(IDM_Adjust, imAjust);
  //  SetButtonImage(IDM_BOHREN,6);
  //  SetSelectionColor(IDM_BOHREN,2);
  //  SetButtonImage(IDM_FRAESEN,4);
  //  SetSelectionColor(IDM_FRAESEN,2);
  SetButtonImage(IDM_LASER, 7);

  SetSelectionColor(IDM_LASER, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_FILEOPEN, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_NULLPUNKT, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_DXFColor, FL_DARK_GREEN);
  SetSelectionColor(IDM_OPTIMIZEDXF, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_DXFSPEZIAL, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_TOPDTOOL, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_LAYER, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_Adjust, FL_DARK_MAGENTA);
  SetSelectionColor(IDM_DisplayAdj, FL_DARK_MAGENTA);


  SetButtonImage(IDM_Zoom1, 17);
  SetButtonImage(IDM_ZoomGanz, 18);

  /*  SetButtonImage(IDM_Tiefe1,19);
    SetButtonImage(IDM_Tiefe2,20);
    SetButtonImage(IDM_Tiefe3,21);
    SetButtonImage(IDM_Tiefe4,22);
    SetButtonImage(IDM_Tiefe5,23);
    SetButtonImage(IDM_Tiefe6,24);*/
  SetButtonImage(IDC_OnLine, imOffline);

  if(!Style) {
    SetButtonImage(IDC_ModeFullscreen, imFullSize);
  } else {
    ShowDlgItem(IDC_ModeFullscreen, 0);
  }
  SetSelectionColor(IDC_DXFtoCNCTab,FL_DARK_GREEN);
  SetSelectionColor(IDM_GBlock,FL_DARK_MAGENTA);
  SetSelectionColor(IDM_GCode, FL_DARK_GREEN);
  SetSelectionColor(IDC_Isel, FL_DARK_MAGENTA);
  SetSelectionColor(IDC_GoCnc, FL_DARK_MAGENTA);

  SetButtonImage(IDM_GCode, imGCode);

  SetButtonImage(IDC_Isel, imIsel);
  SetButtonImage(IDC_GoCnc, imGoCnc);
  CheckRadioButton(IDC_GoCnc, IDC_Isel, gIselRs232Mode ? IDC_Isel : IDC_GoCnc);

  InitLaden();
  return maxxy;
}
//-------------------------------------------------------------
void CToolBox ::Show(void)
{
  show();
}
//-------------------------------------------------------------
// int LayerAnz=5;
int CToolBox ::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    switch(wParam) {
    case IDC_Exit:
      cbExit(NULL, NULL);
      break;
    case IDM_FILESAVE: {
      InitSpeichern();
    }
    break;
    case IDM_STOPP:
      StopCNC();
      break;
    case IDM_FILEOPEN: {
      ShowTab(IDM_FILEOPEN);
      if(ParamModify && FltkMessageBox("Parameter Speichern", "", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        InitSpeichern();
        ParamModify = 0;
      }
      char str[_MAX_PATH];
      // gBastelUtils.GetProfileString("CNCFile","LastFile","",str,sizeof(str));
      prefs->get("LastFile", str, ".", sizeof(str));

#ifdef NATIVE_FILECHOOSER
            gFileExplorer->value(str);
            PickFile_CB(NULL,NULL);
            gBastelUtils.WriteProfileString("CNCFile","LastFile",gFileExplorer->value());
            prefs->set("LastFile",gFileExplorer->value());
#else
      ShowTab(IDM_FILEOPEN);

//      if(1) {
//        if(gObjectManager && gObjectManager->m_FilePfadName[0] != '\0') {
//          SelectTreeItem(gFileExplorer, gObjectManager->m_FilePfadName);
//        } else {
//          // gBastelUtils.GetProfileString("Profiler","TableNameX","",str,sizeof(str));
//          // ShowFileExplorer(str,".dxf");
//          SelectTreeItem(gFileExplorer, str);
//        }
//        Fl::focus(gFileExplorer);
//      }
#endif
      //#endif6
    }
    break;
    case IDM_ShowDXFTree:
      ShowTab(IDM_ShowDXFTree);
      break;
    case IDM_UPDATE:
      if(!gCNCAktiv && gObjectManager)
        gObjectManager->FileOpen(NULL, 1);

      break;
    case IDM_OPTIMIZEDXF:
      ShowTab(IDM_OPTIMIZEDXF);
      break;
    case IDC_DXFtoCNCTab:
      ShowTab(IDC_DXFtoCNCTab);
      break;
    case IDC_OnLine:
      gOnline = !gOnline;
      break;
    case IDC_Isel:
    case IDC_GoCnc:
      //      gOnline = 0;
      //      gIselRs232Mode = (wParam==IDC_Isel);
      //      SetIniFile(gIselRs232Mode?"IselDxf.ini":"GoCnc.ini");
      //      InitLaden();
      //      CheckDlgButton(IDC_GoCnc,gIselRs232Mode==0);
      //      CheckDlgButton(IDC_Isel,gIselRs232Mode);
      //      //CheckRadioButton(IDC_GoCnc,IDC_Isel,wParam);
      break;
    case IDM_LAYER:
      ShowTab(IDM_LAYER);
      break;

    case IDM_DXFSPEZIAL:
      ShowTab(IDM_DXFSPEZIAL);
      break;
    case IDC_WekzeugSet: {
      ShowTab(IDM_Adjust);
      char str[30];
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Xp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugX, str);
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Yp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugY, str);
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Zp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugZ, str);
    }
    break;
    //    case id_XYNull:
    //    case id_ZNull:
    case IDM_EINRICHTEN:
      ShowTab(IDM_NULLPUNKT);
      if(wParam == id_XYNull) {
        if(FltkMessageBox("XY Nullpunkt Uebernehmen ?", "Einricheten", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
          gPa.Ref[0] = gCNCPos[Xp];
          gPa.Ref[1] = gCNCPos[Yp];
        }
      }
      if(wParam == id_ZNull) {
        if(FltkMessageBox("Z Nullpunkt Uebernehmen ?", "Einricheten", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
          gPa.Ref[2] = gCNCPos[Zp];
        }
      }

      //      CFBWindow::SetFocusWindow(gNullpunktTab);
      //      CFBWindow::SetFocusWindow(gNullpunktTab->GetDlgItem(id_ZWerNullT));

      break;
    /*    case VK_CONTROL_L|VK_KP_1:
          AktTiefe =1;
          goto showlinesetup;

        case IDM_Tiefe1:
        case IDM_Tiefe2:
        case IDM_Tiefe3:
        case IDM_Tiefe4:
        case IDM_Tiefe5:
        case IDM_Tiefe6:
          AktTiefe = wParam - IDM_Tiefe1+1;
    showlinesetup:
          ShowTab(wParam);
          CheckRadioButton(IDM_Tiefe1,IDM_TOOLBAR,IDM_Tiefe1-1 +AktTiefe );
          break;
    */
    case IDM_Adjust:
      ShowTab(wParam);
      break;
      break;
    case IDM_DXFColor:
      ShowTab(wParam);
      break;
    case IDM_TOPDTOOL: // IDM_STOPP:
      ShowTab(wParam);
      break;
    //    case IDM_FRAESENKEY:
    //    {
    //      CheckDlgButton(IDM_FRAESEN,IsDlgButtonChecked(IDM_FRAESEN)?0:1);
    //    }

    case IDM_GCode: {
      ShowTab(IDM_GCode);
    }
    break;
    case IDM_GBlock: {
      ShowTab(IDM_GBlock);
    }
    break;
    case IDM_LASER: {
      ShowTab(IDM_LASER);
    }
    break;
    //    case IDM_BOHRENKEY:
    //    {

    //      if (IsDlgButtonChecked(IDM_BOHREN))
    //      {
    //        CheckDlgButton(IDM_BOHREN,!IsDlgButtonChecked(IDM_BOHREN));
    //      }
    //    }
    case IDM_NULLPUNKT: {
      ShowTab(IDM_NULLPUNKT);
    }
    break;
    case IDC_ModeFullscreen:
      void cbFullScreen(Fl_Button*, void*);
      cbFullScreen(NULL, 0);
      break;
    /*    case IDM_Zoom1:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_Zoom1,0);
          break;
        case IDM_ZoomMax:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_ZoomMax,0);
          break;
      case IDM_Zoom2:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_Zoom2,0);
          break;
           * */
    case IDM_DisplayAdj: {
      char str[100];
      GetDlgItemText(wParam, str, sizeof(str));
      if(strcmp(str, "Fullscreen") == 0) {
        void cbFullScreen(Fl_Button*, void*);
        cbFullScreen(NULL, 0);
      } else {
        ShowTab(wParam);
      }

    }
    break;
    case IDM_Show3D:
      gWrk3DSheet->show();
      //      gWrk3DSheet->WindowProc(WM_COMMAND,0,0);
      gWrk3DSheet->invalidate();
      break;
    case IDM_Refernezfahrt:
      SendToCNC('R');
      gCNCEiche = 1;
      break;

    case IDM_ZoomGanz:
      Fl::e_keysym = 'z';
      Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
      break;
    case IDM_Zoom1:
      Fl::e_keysym = 'x';
      Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
      break;
    case IDC_ShowTop:
      Fl::e_keysym = 't';
      Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
      break;
    case IDC_ShowFront:
      Fl::e_keysym = 'v';
      Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
      break;
    case IDC_GetDefault:
      Fl::e_keysym = 'd';
      Fl::handle_(FL_KEYDOWN, gWrk3DSheet);
      break;
    }
  }
  return 0;
  //  return CDialogBox::WindowProc(uMsg,wParam,lParam);
}
//-------------------------------------------------------------
COptionsBox ::COptionsBox(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_OptionsBox;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_OptionsBox, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
  ParamModify = 0;
}

//-------------------------------------------------------------//-------------------------------------------------------------
COptionsBox ::~COptionsBox()
{
}
//-------------------------------------------------------------
BOOL COptionsBox::SetButtonImage(int nIDDlgItem, int iImage)
{
  Fl_Button* butt = (Fl_Button*)GetDlgItem(nIDDlgItem);
  if(butt) {
    butt->image(R->ButtonImage_List[iImage]);
    if(strlen(butt->label()) > 0)
      // butt ->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP);
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT); // FL_ALIGN_IMAGE_BACKDROP);
    else
      butt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
  }
  return 0;
}
//-------------------------------------------------------------

int COptionsBox ::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  // fl_font(FL_FREE_FONT,10);
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style);

  SetButtonImage(IDC_Exit, imExit);
  SetButtonImage(IDM_UPDATE, 3);
  SetButtonImage(IDM_FILEOPEN, 1);
  SetButtonImage(IDM_FILESAVE, 2);
  SetButtonImage(IDM_ShowDXFTree, imNewDoc);
  SetButtonImage(IDM_DXFSPEZIAL, 5);
  SetButtonImage(IDM_NULLPUNKT, 9);
  //  SetButtonImage(IDM_Refernezfahrt,14);
  //  SetButtonImage(IDC_GehZuNULL,imWerkzeug);
  SetButtonImage(IDM_Adjust, imAjust);
  //  SetButtonImage(IDM_BOHREN,6);
  //  SetSelectionColor(IDM_BOHREN,2);
  //  SetButtonImage(IDM_FRAESEN,4);
  //  SetSelectionColor(IDM_FRAESEN,2);
  SetButtonImage(IDM_LASER, 7);
  SetButtonImage(IDC_OnLine, imOffline);

  SetButtonImage(IDC_ModeFullscreen, imFullSize);
  SetButtonImage(IDM_GCode, imGCode);
  SetButtonImage(IDC_Isel, imIsel);
  SetButtonImage(IDC_GoCnc, imGoCnc);
  CheckRadioButton(IDC_GoCnc, IDC_Isel, gIselRs232Mode ? IDC_Isel : IDC_GoCnc);

  InitLaden();
  return maxxy;
}
//-------------------------------------------------------------
void COptionsBox ::Show(void)
{
  show();
}
//-------------------------------------------------------------
// int LayerAnz=5;
int COptionsBox ::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    switch(wParam) {
    case IDC_Exit:
      cbExit(NULL, NULL);
      break;
    case IDM_FILESAVE: {
      InitSpeichern();
    }
    break;
    case IDM_STOPP:
      StopCNC();
      break;
    case IDM_FILEOPEN: {
      //      if (ParamModify
      //          && FltkMessageBox("Parameter Speichern","",MB_YESNO | MB_ICONQUESTION)== IDYES)
      //      {
      //        InitSpeichern();
      //        ParamModify=0;
      //      }

#ifdef NATIVE_FILECHOOSER
      char str[_MAX_PATH];
      // gBastelUtils.GetProfileString("CNCFile","LastFile","",str,sizeof(str));
      prefs->get("LastFile", str, ".", sizeof(str));
           gFileExplorer->value(str);
           PickFile_CB(NULL,NULL);
           gBastelUtils.WriteProfileString("CNCFile","LastFile",gFileExplorer->value());
#else
      ShowTab(IDM_FILEOPEN);
#endif
    }
    break;
    case IDM_ShowDXFTree:
      ShowTab(IDM_ShowDXFTree);
      break;
    case IDM_UPDATE:
      if(!gCNCAktiv && gObjectManager)
        gObjectManager->FileOpen(NULL, 1);

      break;
    case IDM_OPTIMIZEDXF:
      ShowTab(IDM_OPTIMIZEDXF);
      break;
    case IDC_DXFtoCNCTab:
      ShowTab(IDC_DXFtoCNCTab);
      break;
    case IDC_OnLine:
      gOnline = !gOnline;
      break;
    case IDC_Isel:
    case IDC_GoCnc:
      //      gOnline = 0;
      //      gIselRs232Mode = (wParam==IDC_Isel);
      //      SetIniFile(gIselRs232Mode?"IselDxf.ini":"GoCnc.ini");
      //      InitLaden();
      //      CheckDlgButton(IDC_GoCnc,gIselRs232Mode==0);
      //      CheckDlgButton(IDC_Isel,gIselRs232Mode);
      //      //CheckRadioButton(IDC_GoCnc,IDC_Isel,wParam);
      break;
    case IDM_LAYER:
      ShowTab(IDM_LAYER);
      break;

    case IDM_DXFSPEZIAL:
      ShowTab(IDM_DXFSPEZIAL);
      break;
    case IDC_WekzeugSet: {
      ShowTab(IDM_Adjust);
      char str[30];
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Xp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugX, str);
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Yp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugY, str);
      sprintf(str, "%1.2f", (float)gPa.WerzeugSensor[Zp] / (float)gPa.Aufloesung);
      gToolAdjustTab->SetDlgItemText(IDC_WekzeugZ, str);
    }
    break;
    //    case id_XYNull:
    //    case id_ZNull:
    case IDM_EINRICHTEN:
      ShowTab(IDM_NULLPUNKT);
      if(wParam == id_XYNull) {
        if(FltkMessageBox("XY Nullpunkt Uebernehmen ?", "Einricheten", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
          gPa.Ref[0] = gCNCPos[Xp];
          gPa.Ref[1] = gCNCPos[Yp];
        }
      }
      if(wParam == id_ZNull) {
        if(FltkMessageBox("Z Nullpunkt Uebernehmen ?", "Einricheten", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
          gPa.Ref[2] = gCNCPos[Zp];
        }
      }

      //      CFBWindow::SetFocusWindow(gNullpunktTab);
      //      CFBWindow::SetFocusWindow(gNullpunktTab->GetDlgItem(id_ZWerNullT));

      break;
    /*    case VK_CONTROL_L|VK_KP_1:
          AktTiefe =1;
          goto showlinesetup;

        case IDM_Tiefe1:
        case IDM_Tiefe2:
        case IDM_Tiefe3:
        case IDM_Tiefe4:
        case IDM_Tiefe5:
        case IDM_Tiefe6:
          AktTiefe = wParam - IDM_Tiefe1+1;
    showlinesetup:
          ShowTab(wParam);
          CheckRadioButton(IDM_Tiefe1,IDM_TOOLBAR,IDM_Tiefe1-1 +AktTiefe );
          break;
    */
    case IDM_Adjust:
      ShowTab(wParam);
      break;
      break;
    case IDM_DXFColor:
      ShowTab(wParam);
      break;
    case IDM_TOPDTOOL: // IDM_STOPP:
      ShowTab(wParam);
      break;
    //    case IDM_FRAESENKEY:
    //    {
    //      CheckDlgButton(IDM_FRAESEN,IsDlgButtonChecked(IDM_FRAESEN)?0:1);
    //    }

    case IDM_GCode: {
      ShowTab(IDM_GCode);
    }
    break;
    case IDM_LASER: {
      ShowTab(IDM_LASER);
    }
    break;
    //    case IDM_BOHRENKEY:
    //    {

    //      if (IsDlgButtonChecked(IDM_BOHREN))
    //      {
    //        CheckDlgButton(IDM_BOHREN,!IsDlgButtonChecked(IDM_BOHREN));
    //      }
    //    }
    case IDM_NULLPUNKT: {
      ShowTab(IDM_NULLPUNKT);
    }
    break;
    case IDC_ModeFullscreen:
      void cbFullScreen(Fl_Button*, void*);
      cbFullScreen(NULL, 0);
      break;
    /*    case IDM_Zoom1:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_Zoom1,0);
          break;
        case IDM_ZoomMax:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_ZoomMax,0);
          break;
      case IDM_Zoom2:
          gWrk3DSheet->WindowProc(WM_COMMAND,IDM_Zoom2,0);
          break;
           * */
    case IDM_DisplayAdj: {
      char str[100];
      GetDlgItemText(wParam, str, sizeof(str));
      if(strcmp(str, "Fullscreen") == 0) {
        void cbFullScreen(Fl_Button*, void*);
        cbFullScreen(NULL, 0);
      } else {
        ShowTab(wParam);
      }

    }
    break;
    case IDM_Show3D:
      gWrk3DSheet->show();
      //      gWrk3DSheet->WindowProc(WM_COMMAND,0,0);
      gWrk3DSheet->invalidate();
      break;
    case IDM_Refernezfahrt:
      SendToCNC('R');
      gCNCEiche = 1;
      break;
    }
  }
  return 0;
  //  return CDialogBox::WindowProc(uMsg,wParam,lParam);
}
//-------------------------------------------------------------
//-------------------------------------------------------------
void PickFile(Fl_Widget*, void*)
{
  ShowTab(IDM_FILEOPEN);
}

//-------------------------------------------------------------
int gSanDirCount;
int gSanFileCount;
int gUpdatetimer;
void ResetScanStatus(void)
{
  gSanDirCount = 0;
  gSanFileCount = 0;
  gUpdatetimer = 0;

  //  gStatus->SetDlgItemText(IDC_NowPlaying,"");
}
void IncrementFileScanStatus(void)
{
  gSanFileCount++;
  char str[100];
  if(gSanDirCount)
    sprintf(str, "SCAN: %d Dirs %d Files", gSanDirCount, gSanFileCount);
  else
    sprintf(str, "SCAN: %d Files", gSanFileCount);
  //  gStatus->SetDlgItemText(IDC_NowPlaying,str);
  int t = GetTickCount();
  if(t > gUpdatetimer + 100) {
    gUpdatetimer = t;
    Fl::lock();
    Fl::check();
    Fl::unlock();
  }
}

void IncrementDirScanStatus(void)
{
  gSanDirCount++;
  char str[100];
  if(gSanFileCount)
    sprintf(str, "SCAN: %d Dirs %d Files", gSanDirCount, gSanFileCount);
  else
    sprintf(str, "SCAN: %d Dirs", gSanDirCount);
  //  gStatus->SetDlgItemText(IDC_NowPlaying,str);
  int t = GetTickCount();
  if(t > gUpdatetimer + 100) {
    gUpdatetimer = t;
    Fl::lock();
    Fl::check();
    Fl::unlock();
  }
}

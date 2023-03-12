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
#include <FL/fl_show_colormap.H>
#include <ctype.h>
#include <math.h>

CBlockSimulation * gBlockSimulation = NULL;
#ifdef WIN32
CRITICAL_SECTION CBlockSimulation::s_critical_section;;
#else
pthread_mutex_t  CBlockSimulation::s_critical_section;
#endif
extern float glMaximum ;
extern int   gShowSymulation;
int          gSimulationModus;

//-------------------------------------------------------------
const tDialogCtlItem rg_BlockTab[] = {
  { eDialogBox, "Tool", IDC_DialogBox - 3, 4, 2, 90, 192,WS_BORDER | WS_TABSTOP}, // Dialog
  { CONTROL "", -1, 3, 0, 164, 209, WS_DLGFRAME, NULL },

  { BUTTON "Fast Sim.",IDC_StartSimulation,112,  5, 52, 18, BS_AUTORADIOBUTTON | WS_TABSTOP,NULL},
  { AUTORADIOBUTTON "Block",     IDC_ZeigeBlock,  6,  5, 31, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Gcode",     IDC_ZeigeGcode, 37,  5, 31, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Beides",    IDC_ZeigeBeides, 68,  5, 31, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { BUTTON "Oberflaeche",IDC_BlockMaterial,  6, 28, 52, 18, ES_LEFT | ES_MULTILINE },
  { BUTTON "Vertiefung", IDC_Material, 58, 28, 52, 18, ES_LEFT | ES_MULTILINE },
  { AUTORADIOBUTTON  "Auto Upd.", IDC_AutoUpdate,112, 28, 52, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP,NULL},

  { AUTORADIOBUTTON "Default",IDC_FraeserAuto,  6, 50, 40, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Flach",IDC_FraeserFormFlach, 47, 50, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "VForm",IDC_FraeserFormSpitz, 77, 50, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },
  { AUTORADIOBUTTON "Rund",IDC_FraeserFormRund,107, 50, 29, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX| WS_TABSTOP, NULL },

  { CTEXT "Durchm. Fr:", -1, 10, 69, 56, 18, ES_RIGHT | ES_MULTILINE },
  { eInputNum, "", IDC_MeisselDurchmesser, 66, 69, 70, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Laenge S - E", -1,  1, 88, 84, 12, ES_RIGHT },
  { eInputNum, "",IDC_BlockLaengeS, 10,100, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { eInputNum, "",IDC_BlockLaengeE, 80,100, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },

  { AUTORADIOBUTTON "Rund Durchm.",IDC_RundStab, 10, 124, 65, 18, BS_AUTORADIOBUTTON | BS_AUTOCHECKBOX | WS_TABSTOP,NULL},
  { eInputNum, "", IDC_BlockDurchmesser, 80, 124, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },

  { CTEXT "Breite S - E", -1,  1, 144, 84, 12, ES_RIGHT },
  { eInputNum, "",IDC_BlockBreiteS, 10, 156, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { eInputNum, "",IDC_BlockBreiteE, 80, 156, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { CTEXT "Hoehe S - E", -1,  1, 176, 84, 12, ES_RIGHT },
  { eInputNum, "",IDC_BlockHoeheS, 10, 187, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },
  { eInputNum, "",IDC_BlockHoeheE, 80, 187, 65, 18, WS_TABSTOP | WS_CLIENTEDGE, NULL },


  { eSliderCtl, "Ende",  IDC_Fortschritt,146,  54, 16,146, BS_AUTOCHECKBOX | WS_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP },

  { 0 }
};


//-------------------------------------------------------------
CBlockTab ::CBlockTab(int X, int Y, int W, int H, const char* L)
  : CFltkDialogBox(X, Y, W, H, L)
{
  const tDialogCtlItem* pCtlItem = rg_BlockTab;
  m_NumDialogCtlItem = 0;
  while(pCtlItem->idc) {
    pCtlItem++;
    m_NumDialogCtlItem++;
  }
  m_pDialogCtlItem = (tDialogCtlItem*)malloc(m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  if(m_pDialogCtlItem) {
    memcpy(m_pDialogCtlItem, &rg_BlockTab, m_NumDialogCtlItem * sizeof(tDialogCtlItem));
  }
}

//-------------------------------------------------------------
CBlockTab::~CBlockTab()
{
}

//-------------------------------------------------------------
BOOL CBlockTab::SetButtonImage(int nIDDlgItem, int iImage)
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
int CBlockTab::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  int maxxy = CFltkDialogBox::InitWindow(pParent, ID, left, top, width, height, Style | WS_TABSTOP);
  gBlockSimulation = new CBlockSimulation();
  if (gBlockSimulation) {
    tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_BlockLaengeS);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockLaengeS);
    }
    pCtlItem = pGetDlgItem(IDC_BlockLaengeE);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockBreiteE);
    }
    pCtlItem = pGetDlgItem(IDC_BlockBreiteS);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockBreiteS);
    }
    pCtlItem = pGetDlgItem(IDC_BlockBreiteE);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockLaengeE);
    }

    pCtlItem = pGetDlgItem(IDC_BlockHoeheS);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(0,50);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockHoeheS);
    }
    pCtlItem = pGetDlgItem(IDC_BlockHoeheE);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockHoeheE);
    }
    pCtlItem = pGetDlgItem(IDC_Durchmesser);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->step(1);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->range(-100,300);
      ((Fl_Value_Slider*)pCtlItem->pWndObject)->value(gBlockSimulation->m_BlockDurchmesser);
    }

  }
  SetDlgItemFloat(IDC_BlockLaengeS,gBlockSimulation->m_BlockLaengeS);
  SetDlgItemFloat(IDC_BlockLaengeE,gBlockSimulation->m_BlockLaengeE);
  SetDlgItemFloat(IDC_BlockBreiteS,gBlockSimulation->m_BlockBreiteS);
  SetDlgItemFloat(IDC_BlockBreiteE,gBlockSimulation->m_BlockBreiteE);
  SetDlgItemFloat(IDC_BlockHoeheS, gBlockSimulation->m_BlockHoeheS );
  SetDlgItemFloat(IDC_BlockHoeheE, gBlockSimulation->m_BlockHoeheE );
  SetDlgItemFloat(IDC_BlockDurchmesser, gBlockSimulation->m_BlockDurchmesser);

  CheckDlgButton(IDC_FraeserAuto,1);

  return maxxy;
}

//-------------------------------------------------------------
void CBlockTab::Show(void)
{
  char s[32];
  CheckDlgButton(IDC_RundStab,gPa.Maschine[eDrehachse]);


  if (IsDlgButtonChecked(IDC_RundStab)) {
    EnableDlgItem(IDC_BlockBreiteS,0);
    EnableDlgItem(IDC_BlockBreiteE,0);
    EnableDlgItem(IDC_BlockHoeheS,0);
    EnableDlgItem(IDC_BlockHoeheE,0);
    EnableDlgItem(IDC_BlockDurchmesser,1);
  } else {
    EnableDlgItem(IDC_BlockBreiteS,1);
    EnableDlgItem(IDC_BlockBreiteE,1);
    EnableDlgItem(IDC_BlockHoeheS,1);
    EnableDlgItem(IDC_BlockHoeheE,1);
    EnableDlgItem(IDC_BlockDurchmesser,0);
  }
  SetDlgItemFloat(IDC_BlockLaengeS,gBlockSimulation->m_BlockLaengeS);
  SetDlgItemFloat(IDC_BlockLaengeE,gBlockSimulation->m_BlockLaengeE);
  SetDlgItemFloat(IDC_BlockBreiteS,gBlockSimulation->m_BlockBreiteS);
  SetDlgItemFloat(IDC_BlockBreiteE,gBlockSimulation->m_BlockBreiteE);
  SetDlgItemFloat(IDC_BlockHoeheS, gBlockSimulation->m_BlockHoeheS );
  SetDlgItemFloat(IDC_BlockHoeheE, gBlockSimulation->m_BlockHoeheE );
  SetDlgItemFloat(IDC_BlockDurchmesser, gBlockSimulation->m_BlockDurchmesser);
  CheckRadioButton(IDC_ZeigeBlock,IDC_ZeigeBeides,IDC_ZeigeBlock+gShowSymulation-1);

  int enable = IsDlgButtonChecked(IDC_FraeserAuto)==0;
  EnableDlgItem(IDC_FraeserFormFlach,enable);
  EnableDlgItem(IDC_FraeserFormSpitz,enable);
  EnableDlgItem(IDC_FraeserFormRund,enable);
  EnableDlgItem(IDC_MeisselDurchmesser,enable);
  if (gBlockSimulation) {
    CheckRadioButton(IDC_FraeserFormFlach,IDC_FraeserFormRund,IDC_FraeserFormFlach+gBlockSimulation->m_MeisselForm);

    sprintf(s, "%1.2f",gBlockSimulation->m_MeisselDurchmesser); //(float)gPa.WerkzeugDuchmesser[AktTiefe] / (float)gPa.Aufloesung);
    SetDlgItemText(IDC_MeisselDurchmesser, s);

    tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_BlockMaterial);
    if(pCtlItem && pCtlItem->pWndObject) {
      ((Fl_Button *)pCtlItem->pWndObject)->color(gPa.Block[eOberflaeche]);
    }
    pCtlItem = pGetDlgItem(IDC_Material);
    if(pCtlItem && pCtlItem->pWndObject) {

      ((Fl_Button *)pCtlItem->pWndObject)->color(gPa.Block[eKoerper]);
    }
  }
  show();
}
//-------------------------------------------------------------
int CBlockTab::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {
    char str[132];
    static bool recursion = 0;
    int v;
    float f;
    switch(wParam) {
    case IDC_ZeigeBlock:
    case IDC_ZeigeGcode:
    case IDC_ZeigeBeides:
      CheckRadioButton(IDC_ZeigeBlock,IDC_ZeigeBeides,wParam);
      gShowSymulation= 1 + wParam - IDC_ZeigeBlock  ;

      if (wParam !=IDC_ZeigeGcode) {
        gBlockSimulation->UpdateVertexArray(0,0,gBlockSimulation->m_width,gBlockSimulation->m_height,1);
      }
      gObject_redraw = 1;;
      break;

    case IDC_BlockMaterial: {
      gPa.Block[eOberflaeche] = fl_show_colormap(gPa.Block[eOberflaeche]);
      ParamModify = 1;
      uchar r,g,b;
      tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_BlockMaterial);
      if(pCtlItem && pCtlItem->pWndObject) {
        ((Fl_Button *)pCtlItem->pWndObject)->color(gPa.Block[eOberflaeche]);
      }
      gBlockSimulation->UpdateVertexArray(0,0,gBlockSimulation->m_width,gBlockSimulation->m_height,0);
      gObject_redraw = 1;;
      break;
    }
    case IDC_Material: {
      gPa.Block[eKoerper] = fl_show_colormap(gPa.Block[eKoerper]);
      ParamModify = 1;
      uchar r,g,b;
      tDialogCtlItem* pCtlItem = pGetDlgItem(IDC_Material);
      if(pCtlItem && pCtlItem->pWndObject) {
        ((Fl_Button *)pCtlItem->pWndObject)->color(gPa.Block[eKoerper]);
      }
      gBlockSimulation->UpdateVertexArray(0,0,gBlockSimulation->m_width,gBlockSimulation->m_height,0);
      gObject_redraw = 1;;
      break;
    }
    case IDC_FraeserAuto:
      Show();
      break;
    case IDC_FraeserFormFlach:
    case IDC_FraeserFormSpitz:
    case IDC_FraeserFormRund:
      if (gBlockSimulation) {
        gBlockSimulation->m_MeisselForm = wParam - IDC_FraeserFormFlach;
        CheckRadioButton(IDC_FraeserFormFlach,IDC_FraeserFormRund,IDC_FraeserFormFlach+gBlockSimulation->m_MeisselForm);
        gBlockSimulation->CreateMeissel();
        gObject_redraw = 1;;
      }
      break;
    case IDC_MeisselDurchmesser: {
      gBlockSimulation->m_MeisselDurchmesser = GetDlgItemFloat(IDC_MeisselDurchmesser);
      gBlockSimulation->CreateMeissel();
      gObject_redraw = 1;;
    }
    break;

    case IDC_AutoUpdate :
      if (gBlockSimulation) {
//        gOkModus = 1;
//        gBlockSimulation->UpdateVertexArray(0,0,gBlockSimulation->m_width,gBlockSimulation->m_height,0);
//        gObject_redraw = 1;;
//        if (gBlockSimulation->m_RunSimulation) {
//          gBlockSimulation->m_RunSimulation=0;
//        } else {
//          gBlockSimulation->StartSimulation();
//        }
      }
      break;
    case IDC_BlockLaengeS: {
      gBlockSimulation->m_BlockLaengeS = GetDlgItemFloat(IDC_BlockLaengeS);
      gObject_redraw = 1;;
      gPa.Block[eBlockLaengeS] = gBlockSimulation->m_BlockLaengeS * fxAufloesung;
      ParamModify = 1;
    }
    break;
    case IDC_BlockLaengeE: {
      gBlockSimulation->m_BlockLaengeE = GetDlgItemFloat(IDC_BlockLaengeE);
      gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
      gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
      gPa.Block[eBlockLaengeE] = gBlockSimulation->m_BlockLaengeE * fxAufloesung;
      ParamModify = 1;
      gObject_redraw = 1;;
    }
    break;
    case IDC_BlockBreiteS: {
      gBlockSimulation->m_BlockBreiteS = GetDlgItemFloat(IDC_BlockBreiteS);
      gPa.Block[eBlockBreiteS] = gBlockSimulation->m_BlockBreiteS * fxAufloesung;
      ParamModify = 1;
      gObject_redraw = 1;;
    }
    break;
    case IDC_BlockBreiteE: {
      gBlockSimulation->m_BlockBreiteE = GetDlgItemFloat(IDC_BlockBreiteE);
      gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
      gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
      gPa.Block[eBlockBreiteE] = gBlockSimulation->m_BlockBreiteE * fxAufloesung;
      ParamModify = 1;
      gObject_redraw = 1;;
    }
    break;
    case IDC_BlockHoeheS: {
      gBlockSimulation->m_BlockHoeheS = GetDlgItemFloat(IDC_BlockHoeheS);
      gObject_redraw = 1;;
      ParamModify = 1;
      gPa.Block[eBlockHoeheS]  = gBlockSimulation->m_BlockHoeheS  * fxAufloesung;
    }
    break;
    case IDC_BlockHoeheE: {
      gBlockSimulation->m_BlockHoeheE = GetDlgItemFloat(IDC_BlockHoeheE);
      gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
      gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
      gPa.Block[eBlockHoeheE]  = gBlockSimulation->m_BlockHoeheE  * fxAufloesung;
      ParamModify = 1;
      gObject_redraw = 1;;
    }
    break;
    case IDC_BlockDurchmesser: {
      gBlockSimulation->m_BlockDurchmesser = GetDlgItemFloat(IDC_BlockDurchmesser);
      gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockDurchmesser);
      gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
      gPa.Block[eBlockDurchmesser]=gBlockSimulation->m_BlockDurchmesser * fxAufloesung;
      ParamModify = 1;
      gObject_redraw = 1;;
    }
    break;

    case IDC_RundStab:
      gPa.Maschine[eDrehachse] = IsDlgButtonChecked(IDC_RundStab);
      ParamModify = 1;
      gObject_redraw = 1;;
      Show();
      break;
    case IDC_StartSimulation:
      if (gBlockTab && gBlockTab->visible()) {
//        if (gBlockSimulation->m_rgTiefe) {
        gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
        gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
        gBlockSimulation->SimulationStart();

//        gOkModus = 1;
//        gBlockSimulation->UpdateVertexArray(0,0,gBlockSimulation->m_width,gBlockSimulation->m_height,0);
//        gObject_redraw = 1;;
//        if (gBlockSimulation->m_RunSimulation) {
//          gBlockSimulation->m_RunSimulation=0;
//        } else {
//          gBlockSimulation->StartSimulation();
//        }

//       }
      }
      break;
    case IDC_Fortschritt:
      if (Fl::focus()== GetDlgItem(IDC_Fortschritt)) {
        int posStart = 0;//GetDlgItemInt(IDC_GCODEPosStart, NULL, 0);
        int posEnd   = GetDlgItemInt(IDC_Fortschritt, NULL, 0);
        if(posStart > posEnd) {
//          SetDlgItemInt(IDC_GCODEPosStart, posEnd - 10, 0);
        }

        gObject_redraw = 1;;

      }
      break;
    }
  }
  return 0;
}

//-------------------------------------------------------------
CBlockSimulation::CBlockSimulation()
{
  if (0) {
    m_BlockLaengeS   =  -30;
    m_BlockLaengeE   =  100;
    m_BlockBreiteS   =  -10;
    m_BlockBreiteE   =   60;
    m_BlockHoeheS    =    6;
    m_BlockHoeheE    =   24;
    m_BlockDurchmesser=  25;
  } else {
    m_BlockLaengeS   =  0;
    m_BlockLaengeE   =  80;//160;
    m_BlockBreiteS   =  0;
    m_BlockBreiteE   =  100;
    m_BlockHoeheS    =    0;
    m_BlockHoeheE    =   1.5;
    m_BlockDurchmesser=  40;//25;
  }

  m_Raster         = 0.10;
  m_MeisselDurchmesser =2; //3.15; //
  m_MeisselForm    = 0;
  m_rgTiefe        = NULL;
  m_rgMeisselTiefe = NULL;
  m_MeisselPxlWidth=1;
  m_width          = 0;
  m_height         = 0;
#ifndef VECTOR
  m_vertices=0;
  m_normals=0;
  m_colors=0;
#endif
#ifdef USE_SHADER
  m_NumPoints      =  0;
  m_VertexBuffer=0;
  m_ColorBuffer =0;
  m_NormalBuffer=0;
  m_UVBuffer    =0;
#endif
  m_RunSimulation =0;
#ifdef WIN32
  m_hThread  =0;
  m_ThreadID =0;
  InitializeCriticalSection(&s_critical_section);
#else
  m_hThread   =0;
  InitializeCriticalSection(&s_critical_section);
#endif


}
//-------------------------------------------------------------
CBlockSimulation::~CBlockSimulation()
{
  FreeGlBuffer();
}
//-------------------------------------------------------------
void CBlockSimulation::FreeGlBuffer()
{
  if (m_rgTiefe) free (m_rgTiefe);
  m_rgTiefe = NULL;
#ifdef VECTOR
  m_vertices.clear();
  m_normals.clear();;
  m_colors.clear();;
#else
#ifdef WIN32
  if (m_vertices) GlobalFree(m_vertices);
  if (m_normals)  GlobalFree(m_normals);
  if (m_colors)   GlobalFree(m_colors);

#else
  if (m_vertices) free (m_vertices);
  if (m_normals)  free (m_normals);
  if (m_colors)   free (m_colors);
#endif
  m_vertices=0;
  m_normals=0;
  m_colors=0;
#endif
#ifdef USE_SHADER
  m_UV.clear();
  if (m_VertexBuffer)
    glDeleteBuffers(1, &m_VertexBuffer);
  m_VertexBuffer=0;
  if (m_NormalBuffer)
    glDeleteBuffers(1, &m_NormalBuffer);
  m_NormalBuffer=0;
  if (m_ColorBuffer)
    glDeleteBuffers(1, &m_ColorBuffer);
  m_ColorBuffer=0;
  if (m_UVBuffer)
    glDeleteBuffers(1, &m_UVBuffer);
  m_UVBuffer=0;
  m_NumPoints   =0;
#endif
  m_SimulationLayerIndex = 100000;
  m_SimulationAktTiefe   = 0;
}
//-------------------------------------------------------------
void CBlockSimulation::SimulationStart()
{
  m_SimulationLayerIndex =0;
  m_SimulationAktTiefe = 1;
  gSimulationModus = eModusFast;

  CObject * pObj;
  for (int Index = 0; Index < gObjectManager->CNCInfoCnt; Index++) {
    pObj = gObjectManager->CNCInfo[Index];
    if (pObj) {
      pObj->SetDone(0);
    }
  }

}
//-------------------------------------------------------------
void CBlockSimulation::SimulationService()
{

  if (gOnline || (m_RunSimulation && gCNCAktiv))  return;
  m_RunSimulation =0;
  gCNCAktiv=0;

  CLayers* pLayer = GetLayer(m_SimulationLayerIndex);
  if (pLayer) {
    if (pLayer->m_NumObjects) {
      WorkLayer = pLayer;
      strncpy(szWorkLayer, WorkLayer->m_Name, sizeof(szWorkLayer));
      if (m_SimulationAktTiefe < 1) m_SimulationAktTiefe = 1;
      if (m_SimulationAktTiefe < 7) {
        if (strcasecmp(pLayer->m_Name,"LEITER")==0) {
          AktTiefe  = m_SimulationAktTiefe;
          gObjectManager->Sort_Reset();
          ClipperLib::cInt X = gCNCPos[Xp];
          ClipperLib::cInt Y = gCNCPos[Yp];
          int num = gObjectManager->NumObjectofType(Linie | Buchstaben | Kreisbogen,WorkLayer,AktTiefe);
          fprintf(stderr,"GetNum %d Lines  %s %d\n",num,szWorkLayer,AktTiefe);
          if (num) {
            gSpindleOn =1;
            StartExecuteThread(gExecuteMode= (Linie | Buchstaben | Kreisbogen));
            Sleep(250);
            Fl::lock();
            Fl::check();
            Fl::unlock();
          }
        } else if (strcasecmp(pLayer->m_Name,"BOHREN")==0) {
          AktTiefe  = m_SimulationAktTiefe;
          gObjectManager->Sort_Reset();
          ClipperLib::cInt X = gCNCPos[Xp];
          ClipperLib::cInt Y = gCNCPos[Yp];
          int num = gObjectManager->NumObjectofType(Punkt,WorkLayer,AktTiefe);
          fprintf(stderr,"GetNum %d Points  %s %d\n",num,szWorkLayer,AktTiefe);
          if (num) {
            gSpindleOn =1;
            StartExecuteThread(gExecuteMode = Punkt);
            Sleep(250);
            Fl::lock();
            Fl::check();
            Fl::unlock();
          }
        } else if (strcasecmp(pLayer->m_Name,"GCODE")==0) {
          AktTiefe  = m_SimulationAktTiefe;
          gObjectManager->Sort_Reset();
          ClipperLib::cInt X = gCNCPos[Xp];
          ClipperLib::cInt Y = gCNCPos[Yp];
          int num = gObjectManager->NumObjectofType(GCodeLinine,WorkLayer,AktTiefe);
          fprintf(stderr,"GetNum %d Points  %s %d\n",num,szWorkLayer,AktTiefe);
          if (num) {
            gSpindleOn =1;
            StartExecuteThread(gExecuteMode = GCodeLinine);
            Sleep(250);
            Fl::lock();
            Fl::check();
            Fl::unlock();
          }
        }
        m_SimulationAktTiefe++;
        glDisplay_redraw=1;
      } else {
        m_SimulationLayerIndex++;
        m_SimulationAktTiefe = 0;
        glDisplay_redraw=1;
      }
    } else {
      m_SimulationLayerIndex++;
      m_SimulationAktTiefe = 0;
    }
  } else {
    gSimulationModus = eModusRealTime;
  }

}
//-------------------------------------------------------------
void CBlockSimulation::StartSimulation(float durchmesser,int Form)
{
  gCNCPos[Zp] = 4900;


  gOkModus = 1;
  if (gSimulationModus != eModusFast)   gSimulationModus = eModusRealTime;
  if (gBlockTab->IsDlgButtonChecked(IDC_FraeserAuto)) {
    m_MeisselForm = Form;
    m_MeisselDurchmesser = durchmesser;
  }
  CreateMeissel();
  int t=10;
  while (t && m_hThread!=0) {
    m_RunSimulation =0;
    Sleep(50);
    t--;
  }

  StartSimulation();
  if (m_hThread==0) {
    Sleep(50);
    StartSimulation();
  }
  UpdateVertexArray(0,0,m_width,m_height,0);
  gObject_redraw = 1;;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
void CBlockSimulation::StartSimulation()
{
#ifdef WIN32
  if (m_hThread) return;
  m_hThread= CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) SimulationThreadProc,this,0,&m_ThreadID);
  return;
#else
  if (m_hThread) return;
  pthread_create(&m_hThread,NULL,SimulationThreadProc, (void*)this);
#endif
}
//-------------------------------------------------------------
// trick to use static function for threadloop
void * CBlockSimulation::SimulationThreadProc(void * lpThreadParameter)
{
  CBlockSimulation * pCBlockSimulation = (CBlockSimulation*)lpThreadParameter;
  pCBlockSimulation ->SimulationLoop();
  return 0;
}
void CBlockSimulation::SimulationLoop(void)
{
  int runcnt =10;
  int last=GetTickCount();
  m_RunSimulation = 1;
  while (m_RunSimulation && gSpindleOn) {
    if (gOkCount) {
      extern double VIRTUALV1;
      extern double VIRTUALV2;
      extern double VIRTUALV3;
      extern double VIRTUALV4;
      extern double VIRTUALTempo;
      extern char gCNC_HALT;
      EnterCriticalSection(&CBlockSimulation::s_critical_section);
      double xd =     gCNCPos[Xp]-VIRTUALV1;
      double yd =     gCNCPos[Yp]-VIRTUALV2;
      double zd =     gCNCPos[Zp]-VIRTUALV3;
      double ad =     (gCNCPos[Ap]-VIRTUALV4)/3.0;
      LeaveCriticalSection(&CBlockSimulation::s_critical_section);
      double maxstrecke = 10;
      //if (gSimulationModus==eModusFast) maxstrecke = 100000;
      if (gSimulationModus==eModusFast) {
        maxstrecke = 115000; //m_MeisselDurchmesser*400;
      }
      if (gPa.Maschine[eDrehachse]==0) {
        if (xd==0 && yd==0 && ad==0 && zd != 0) {
          maxstrecke = fabs(zd);
        }
        if (zd == 0) {
          if (gCNCPos[Zp] >= gPa.Tiefe[0]) {
            maxstrecke = 10000;
          }
        }
      }
      //if (xd!=0 || yd!=0 || zd!=0)  ad /= 3.0;

      if (fabs(ad) > (1999*360*100/3)) {
        gCNCPos[Ap] = VIRTUALV4;
        ad = 0;
      }
      double w;
      if(gPa.Maschine[eDrehachse]) {
        w = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2) + pow(ad, 2));
      } else {
        w = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2));
      }
//   double tempo = 1+ _min(100,w/2);
//   if (w > VIRTUALTempo/12) tempo = 20+VIRTUALTempo/12;
//   else tempo =  w;

      if (maxstrecke>w) maxstrecke=w;

      if (w >= m_Raster*2) {
        gCNCPos[Xp] -= maxstrecke * xd /w;
        gCNCPos[Yp] -= maxstrecke * yd /w;
        gCNCPos[Zp] -= maxstrecke * zd /w;
        double dw = maxstrecke * ad /w;
        gCNCPos[Ap] -= dw * 3.0;
        gCNCCode = 'R';

        Meissel((float)gCNCPos[Xp],(float)gCNCPos[Yp],(float)gCNCPos[Zp],(float)gCNCPos[Ap]);
        if (gSimulationModus!=eModusFast) {
          if (m_updateSX<m_updateEX || m_updateSY < m_updateEY) {
            UpdateVertexArray(m_updateSX,m_updateSY,m_updateEX,m_updateEY,0);
            m_updateSX = m_width;
            m_updateSY = m_height;
            m_updateEX = 0;
            m_updateEY = 0;
          }
          glDisplay_redraw=1;
        }
        if (gSimulationModus == eModusRealTime) {
          int now = GetTickCount();
          if (now <  last) Sleep(1);
          last += (3000 / VIRTUALTempo);
        } else {
          runcnt =20;
        }
      } else {
        if (runcnt) {
          runcnt--;
        } else {
          if (gOkCount) {
            gOkCount--;
//            gNewPos=1;
            last=GetTickCount();
          }
          runcnt =20;
        }
      }
      //    LeaveCriticalSection(&SerialComm::s_critical_section);
    } else {
      if (gSimulationModus!=eModusFast) usleep(100);
    }
  }
  gBlockSimulation->UpdateVertexArray(0,0,m_width,m_height,0);
  m_RunSimulation = 0;

  m_hThread  = 0;
}

//-------------------------------------------------------------
void CBlockSimulation::GenerateGlBuffer()
{
#ifdef USE_SHADER
  if (m_NumPoints) {
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints * sizeof(float) * 3, &m_vertices[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints * sizeof(float) * 3, &m_colors[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_NormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints* sizeof(float) * 3, &m_normals[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints* sizeof(float) * 2, &m_UV[0].Vertex[0], GL_STATIC_DRAW);
  }
#endif
}

//-------------------------------------------------------------
int CBlockSimulation::SetBufferSize(int NumPoints)
{
#ifdef VECTOR
  m_vertices.reserve(NumPoints);
  m_vertices.resize(NumPoints);
  m_normals.reserve(NumPoints);
  m_normals.resize(NumPoints);
  m_colors.reserve(NumPoints);
  m_colors.resize(NumPoints);
#else
#ifdef WIN32
  m_vertices = (vector3*)GlobalAlloc(GMEM_FIXED,NumPoints*sizeof(vector3));
  m_normals  = (vector3*)GlobalAlloc(GMEM_FIXED,NumPoints*sizeof(vector3));
#ifdef COLOR32
  m_colors   = (DWORD*)GlobalAlloc(GMEM_FIXED,NumPoints*sizeof(DWORD));
#else
  m_colors   = (vector3*)GlobalAlloc(GMEM_FIXED,NumPoints*sizeof(vector3));
#endif
#else
  m_vertices = (vector3*)malloc(NumPoints*sizeof(vector3));
  m_normals  = (vector3*)malloc(NumPoints*sizeof(vector3));
  m_colors   = (DWORD*)malloc(NumPoints*sizeof(DWORD));
#endif
#endif
#ifdef USE_SHADER
  m_UV.reserve(NumPoints);
  m_UV.resize(NumPoints);
  m_Texture=-1;
#endif
  return 1;
}

void CBlockSimulation::loadInitValues()
{
  m_BlockLaengeS = gPa.Block[eBlockLaengeS] / fxAufloesung;
  m_BlockLaengeE = gPa.Block[eBlockLaengeE] / fxAufloesung;
  m_BlockBreiteS = gPa.Block[eBlockBreiteS] / fxAufloesung;
  m_BlockBreiteE = gPa.Block[eBlockBreiteE] / fxAufloesung;
  m_BlockHoeheS  = gPa.Block[eBlockHoeheS] / fxAufloesung;
  m_Raster       = gPa.Block[eRaster] / fxAufloesung;
  m_BlockDurchmesser = gPa.Block[eBlockDurchmesser] / fxAufloesung;


}

//-------------------------------------------------------------
int CBlockSimulation::CreateVertexArray(int ow,int oh)
{
  FreeGlBuffer();
  m_Xlast = -1E10;
  m_Ylast = -1E10;
  m_Zlast = -1E10;
  m_Alast = -1E10;
  if (ow < 5 || ow > 350)  ow = 5;
  if (oh < 5 || oh > 350)  oh = 5;
  m_BlockLaengeE = ow;
  m_BlockBreiteE = oh;
  float tiefe;
  if (gPa.Maschine[eDrehachse]) {
    m_height= m_BlockDurchmesser * 3.14 / m_Raster;
    tiefe   = m_BlockDurchmesser / m_Raster / 2;// Radius  (100.0/2.0);
  } else {
    m_height         = ((float)oh/m_Raster);
//#ifdef USE_SHADER
//    tiefe  =  m_BlockHoeheE*1;
//#else
    tiefe  = m_BlockHoeheE / m_Raster;
//#endif
  }
  m_width          = ((float)ow/m_Raster);
  m_rgTiefe = (float * )malloc (m_width*(m_height+1) * sizeof(float));
  float part    = 2.0 / m_width;
  if (gPa.Maschine[eDrehachse]) {
    for (float yy=0; yy< m_height; yy++) {
      float v = (yy*part)-1.0;
      float v2 = 1.0;
      for (float xx=0; xx< m_width; xx++) {
        m_rgTiefe[(int)(yy*m_width) + (int)xx] = tiefe;
      }
    }
  } else {
    for (float yy=0; yy< m_height; yy++) {
      float v = (yy*part)-1.0;
      float v2 = (yy==0 || yy == m_height-1)?0:1.0;
      for (float xx=0; xx< m_width; xx++) {
        // Rand auf Null herunterziehen ?
        float a2 = (xx==0 || xx == m_width-1)?0:tiefe;
        m_rgTiefe[(int)(yy*m_width) + (int)xx] =   a2 * v2;
      }
    }
  }
  m_NumPoints = (m_width+2) * m_height*2;
  SetBufferSize(m_NumPoints );


  uchar r,g,b;
#ifdef COLOR32
  Fl::get_color(gPa.Block[eKoerper],r,g,b);
  m_Koerper = (DWORD)(r) << 0 | (DWORD)(g) << 8 | (DWORD)(b) << 16 | (DWORD)(gAlphaTransp*255.0) << 24;
  Fl::get_color(gPa.Block[eOberflaeche],r,g,b);
  m_Oberflaeche = (DWORD)(r) << 0 | (DWORD)(g) << 8 | (DWORD)(b) << 16 | (DWORD)(gAlphaTransp*255.0) << 24;
  m_Boden = 0;
#else
  Fl::get_color(gPa.Block[eKoerper],r,g,b);
  m_Koerper.Vertex[0] = r / 255.0;
  m_Koerper.Vertex[1] = g / 255.0;
  m_Koerper.Vertex[2] = b / 255.0;
  //m_Koerper.Vertex[3] = gAlphaTransp;

  Fl::get_color(gPa.Block[eOberflaeche],r,g,b);
  m_Oberflaeche.Vertex[0] = r / 255.0;
  m_Oberflaeche.Vertex[1] = g / 255.0;
  m_Oberflaeche.Vertex[2] = b / 255.0;
  //m_Oberflaeche.Vertex[3] = gAlphaTransp;
  m_Boden.Vertex[0] = 0.0;
  m_Boden.Vertex[1] = 0.0;
  m_Boden.Vertex[2] = 0.0;
#endif

#ifdef USE_SHADER
  float raster = m_Raster;
  float faktor = 0.5;
#else
  float raster = m_Raster*100.0;
  float faktor = 50.0;
#endif
  if (gPa.Maschine[eDrehachse]) {
    m_BlockBreiteS=0;
    m_BlockHoeheS =0;
    for (int y=0; y < m_height; y++) {
      int index = (y * m_width);
      float dw = (2.0 * M_PI) * y / (float)m_height;
      float yc = sin(dw);
      float xc = cos(dw);
      m_Color = m_Oberflaeche;
      for (int x=0; x < m_width; x++) {
        float radius  = m_rgTiefe[index] * raster;
        m_3DFace[0] = m_3DFace[1];
        m_3DFace[1] = m_3DFace[2];
        m_3DFace[2].Vertex[0] = (x)*raster;
        m_3DFace[2].Vertex[1] = (radius * yc);// - (Verdickung * xc);
        m_3DFace[2].Vertex[2] = (radius * xc);// + (Verdickung * yc);
        m_vertices[index] = m_3DFace[2];
        index++;
      }
    }
    for (int x=0; x < m_width; x++) {
      for (int y=0; y < m_height; y++) {
        int index = (y * m_width)+x;
        m_Color = m_Oberflaeche;
        float radius  = m_rgTiefe[index] * raster;
        m_3DFace[0] = m_3DFace[1];
        m_3DFace[1] = m_3DFace[2];
        m_3DFace[2] = m_vertices[index];
        if (y) {
          CalculateNormale(1);
          m_normals [index] = m_Normal;
          m_colors  [index] = m_Color;
        }
      }
    }

  } else {
    int x;
    int y;
    for (y=0; y <= m_height; y++) {
      for (x=0; x < m_width; x++) {
        m_3DFace[2].Vertex[0] = x*raster;
        m_3DFace[2].Vertex[1] = y*raster;
        m_3DFace[2].Vertex[2] = m_rgTiefe[(y*m_width)+x] * raster;
        m_vertices[(y*m_width)+x]   = m_3DFace[2];
      }
    }
    for (y=0; y < m_height; y++) {
      for (x=0; x < m_width; x++) {
        int index = (y * m_width) + x;
        m_3DFace[0] = m_3DFace[2];
        m_3DFace[1] = m_3DFace[3];
        m_3DFace[2] = m_vertices[index];
        m_3DFace[3] = m_vertices[index + m_width];
        CalculateNormale(1);
        m_normals [index] = m_Normal;
        m_colors  [index] = m_Color;
      }
    }
  }

  GenerateGlBuffer();


  m_updateSX = m_width;
  m_updateEX = 0;
  m_updateSY = m_height;
  m_updateEY = 0;
  return 1;
}

//-------------------------------------------------------------
int CBlockSimulation::UpdateVertexArray(int xs,int ys,int xe,int ye,int cratenew )
{
  if (m_BlockLaengeE==0|| m_BlockBreiteE==0) return 0;
  uchar r,g,b;
#ifdef COLOR32
  Fl::get_color(gPa.Block[eKoerper],r,g,b);
  m_Koerper = (DWORD)(r) << 0 | (DWORD)(g) << 8 | (DWORD)(b) << 16 | (DWORD)(gAlphaTransp*255.0) << 24;
  Fl::get_color(gPa.Block[eOberflaeche],r,g,b);
  m_Oberflaeche = (DWORD)(r) << 0 | (DWORD)(g) << 8 | (DWORD)(b) << 16 | (DWORD)(gAlphaTransp*255.0) << 24;
  m_Boden = 0;
#else
  Fl::get_color(gPa.Block[eKoerper],r,g,b);
  m_Koerper.Vertex[0] = r / 255.0;
  m_Koerper.Vertex[1] = g / 255.0;
  m_Koerper.Vertex[2] = b / 255.0;
  //m_Koerper.Vertex[3] = gAlphaTransp;

  Fl::get_color(gPa.Block[eOberflaeche],r,g,b);
  m_Oberflaeche.Vertex[0] = r / 255.0;
  m_Oberflaeche.Vertex[1] = g / 255.0;
  m_Oberflaeche.Vertex[2] = b / 255.0;
  //m_Oberflaeche.Vertex[3] = gAlphaTransp;
  m_Boden.Vertex[0] = 0.0;
  m_Boden.Vertex[1] = 0.0;
  m_Boden.Vertex[2] = 0.0;
#endif
  EnterCriticalSection(&CBlockSimulation::s_critical_section);
  if (m_rgTiefe==NULL) {
    CreateVertexArray(m_BlockLaengeE,m_BlockBreiteE);
    xe=0;
    ye=0;
    cratenew=1;
  }
  if (cratenew) {
    xs=0;
    xe=m_width;
    ys=0;
    ye=m_height;
    cratenew = 1;
  }
  if (xs<0) xs =0;
  if (ys<0) ys =0;
#ifdef USE_SHADER
  float raster = m_Raster;
  float OrignalHoehe   = (m_BlockHoeheE*0.99);
  float OriginalRadius = m_BlockDurchmesser*0.5;
#else
  float raster = m_Raster*100.0;
  float OrignalHoehe   = (m_BlockHoeheE*99);
  float OriginalRadius = m_BlockDurchmesser*50;
#endif
  if (gPa.Maschine[eDrehachse]) {
    int x;
    int y;
    float radius[4];
    for (y=ys; y < m_height-1 && y <= ye; y++) {
      for (x=xs; x < m_width && x <= xe; x++) {
        int index = (y * m_width)+x;
        radius[0] = radius[2];
        radius[1] = radius[3];
        radius[2]  = m_rgTiefe[index] * raster;
        radius[3]  = m_rgTiefe[index+m_width+1] * raster;
        m_Color = m_Oberflaeche;
        if (radius[0]<1 &&
            radius[1]<1 &&
            radius[2]<1 &&
            radius[3]<1 ) m_Color = m_Boden;
        else if (radius[0] < OriginalRadius ||
                 radius[1] < OriginalRadius ||
                 radius[2] < OriginalRadius ||
                 radius[3] < OriginalRadius)   m_Color = m_Koerper;
        m_3DFace[0] = m_3DFace[2];
        m_3DFace[1] = m_3DFace[3];
        m_3DFace[2] = m_vertices[index];
        m_3DFace[3] = m_vertices[index+m_width];
        if (y) {
          CalculateNormale(1);
          m_normals [index] = m_Normal;
          m_colors  [index] = m_Color;
        }
      }
    }
  } else {
    int x;
    int y;
    for (y=ys; y <= m_height && y <= ye; y++) {
      for (x=xs; x <= m_width && x <= xe; x++) {
        int index = (y * m_width) + x;
        m_3DFace[0] = m_3DFace[2];
        m_3DFace[1] = m_3DFace[3];
        m_3DFace[2] = m_vertices[index];
        m_3DFace[3] = m_vertices[index + m_width];
        CalculateNormale(1);
        if (m_3DFace[0].Vertex[2]<1 &&
            m_3DFace[1].Vertex[2]<1 &&
            m_3DFace[2].Vertex[2]<1 &&
            m_3DFace[3].Vertex[2]<1 ) m_Color = m_Boden;
        else if (m_3DFace[0].Vertex[2]<OrignalHoehe ||
                 m_3DFace[1].Vertex[2]<OrignalHoehe ||
                 m_3DFace[2].Vertex[2]<OrignalHoehe ||
                 m_3DFace[3].Vertex[2]<OrignalHoehe ) m_Color = m_Koerper;
        else m_Color = m_Oberflaeche;
        m_normals [index] = m_Normal;
        m_colors  [index] = m_Color;
      }
    }
  }
  LeaveCriticalSection(&CBlockSimulation::s_critical_section);
  return 1;
}

//-------------------------------------------------------------
void CBlockSimulation::CalculateNormale(int dir)
{
  float normalX =  0;
  float normalY =  0;
  float normalZ =  0;
  int eins = dir?2:0;
  int drei = dir?0:1;
  float   w1;
  float   h1;
  float   t1;
  float   w2;
  float   h2;
  float   t2;
  if (dir) {
    w1 = m_3DFace[2].Vertex[0] - m_3DFace[1].Vertex[0];
    h1 = m_3DFace[2].Vertex[1] - m_3DFace[1].Vertex[1];
    t1 = m_3DFace[2].Vertex[2] - m_3DFace[1].Vertex[2];
    w2 = m_3DFace[1].Vertex[0] - m_3DFace[0].Vertex[0];
    h2 = m_3DFace[1].Vertex[1] - m_3DFace[0].Vertex[1];
    t2 = m_3DFace[1].Vertex[2] - m_3DFace[0].Vertex[2];
  } else {
    w1 = m_3DFace[0].Vertex[0] - m_3DFace[1].Vertex[0];
    h1 = m_3DFace[0].Vertex[1] - m_3DFace[1].Vertex[1];
    t1 = m_3DFace[0].Vertex[2] - m_3DFace[1].Vertex[2];
    w2 = m_3DFace[1].Vertex[0] - m_3DFace[2].Vertex[0];
    h2 = m_3DFace[1].Vertex[1] - m_3DFace[2].Vertex[1];
    t2 = m_3DFace[1].Vertex[2] - m_3DFace[2].Vertex[2];
  }
  normalX +=  (h1) * (t2) - (h2) * (t1);
  normalY +=  (t1) * (w2) - (t2) * (w1);
  normalZ +=  (w1) * (h2) - (w2) * (h1);

  if (normalX || normalY || normalZ) {
    float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
    m_Normal.Vertex[0] = normalX * isqr;
    m_Normal.Vertex[1] = normalY * isqr;
    m_Normal.Vertex[2] = normalZ * isqr;
  }
}
//-------------------------------------------------------------
void CBlockSimulation::drawBlock()
{
#ifndef USE_SHADER
  EnterCriticalSection(&CBlockSimulation::s_critical_section);
  //https://docs.gl/gl3/glDrawArrays
  if (1) {
    glEnable (GL_LIGHTING);
    glEnable (GL_COLOR_MATERIAL ) ;
    //glColorMaterial ( GL_FRONT_AND_BACK, GL_EMISSION );
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    GLfloat light[] = {  .75, .75, .75, gAlphaTransp};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light);
    GLfloat light1[] = {  .35, .35, .75, gAlphaTransp};
    glLightfv(GL_LIGHT1, GL_SPECULAR,light1);
  } else if (0) {
    glEnable (GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_EMISSION );
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    GLfloat light1[] = {  .35, .35, .75, gAlphaTransp};
    glLightfv(GL_LIGHT1, GL_DIFFUSE,light1);
    glLightfv(GL_LIGHT1, GL_SPECULAR,light1);

  } else if (0) {
    glEnable(GL_LIGHTING);
    GLfloat light[] = { 1.0, 1.0, 0.5, 1.0 };
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
    glEnable ( GL_COLOR_MATERIAL ) ;

    {
      // von oben
      glEnable(GL_LIGHT0);
      GLfloat light[] = { 1.0, 1.0, 1.0, gAlphaTransp};
      GLfloat light_position[] = {0.0,0.0,1.0,0};
      glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }
    {
      // von rechts
      GLfloat light_position[] = {1.0,0.0,0,0};
      GLfloat lightFace[] = { 0.0, 0.0, 1.0, gAlphaTransp};
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, lightFace);
      glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    }
    {
      // von hinten
      GLfloat light_position[] = {0.0,1.0,0,0};
      GLfloat lightFace[] = { 0.0, 1.0, 0.0, gAlphaTransp};
      glEnable(GL_LIGHT2);
      glLightfv(GL_LIGHT2, GL_DIFFUSE, lightFace);
      glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    }
    {
      // von vorne
      GLfloat light_position[] = {0.0,-1.0,0,0};
      GLfloat lightFace[] = { 1.0, 1.0, 0.0, gAlphaTransp };
      glEnable(GL_LIGHT3);
      glLightfv(GL_LIGHT3, GL_DIFFUSE, lightFace);
      glLightfv(GL_LIGHT3, GL_POSITION, light_position);
    }
    {
      // von links
      GLfloat light_position[] = {-1.0,0.0,0,0};
      GLfloat lightFace[] = { 0.0, 1.0, 1.0, gAlphaTransp };
      glEnable(GL_LIGHT4);
      glLightfv(GL_LIGHT4, GL_DIFFUSE, lightFace);
      glLightfv(GL_LIGHT4, GL_POSITION, light_position);
    }
    {
      // von unten
      GLfloat light_position[] = {0.0,0.0,-1.0,0};
      GLfloat lightFace[] = { 0.5, 0.5, 1.0, gAlphaTransp };
      glEnable(GL_LIGHT5);
      glLightfv(GL_LIGHT5, GL_DIFFUSE, lightFace);
      glLightfv(GL_LIGHT5, GL_POSITION, light_position);
    }
  } else if (1) {
    glEnable(GL_LIGHTING);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
    glEnable ( GL_COLOR_MATERIAL ) ;

    // Variabel
    GLfloat light_position[] = {1.0,1.0,.0,0};
    GLfloat light[] = { 0.5, 0.5, 1.0, 1.0 };

    light_position[0] += LightPos[0]/10.0;
    light_position[1] += LightPos[1]/10.0;
    light_position[2] += LightPos[2]/10.0;
    light[0] = LightColor[0] / 2.0;
    light[1] = LightColor[1] / 2.0;
    light[2] = LightColor[2] / 2.0;
    light[3] = gAlphaTransp;

    glEnable(GL_LIGHT6);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, light);
    glLightf (GL_LIGHT6, GL_SPOT_CUTOFF, 70.0);
    glLightf (GL_LIGHT6, GL_SPOT_EXPONENT, 12.0);
    glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION,light_position);

  }


  int  x;
  int rechts=m_width-1;
  int unten =m_height;

  if (m_vertices!=NULL) {
    glPushMatrix();
    glTranslatef(m_BlockLaengeS*100.0,m_BlockBreiteS*100.0,m_BlockHoeheS*100.0);
    int yy = unten *2;
    for (int y=0; y < m_height-1; y++) {
      //glBegin(GL_TRIANGLE_STRIP);
      glBegin(GL_QUAD_STRIP);
      int visible=1;
      if (gAlphaTransp > 0.9) {

        for (x=0; x < m_width; x++) {
          int index = (y*m_width)+x;
          if (x&&m_colors[index]==0) {
            if (visible) glEnd();
            visible=0;
          } else {
            //if (visible==0) glBegin(GL_TRIANGLE_STRIP);
            if (visible==0) glBegin(GL_QUAD_STRIP);
            visible=1;
          }
          if (visible) {
            glColor3ubv((GLubyte*)&m_colors[index            ]);
            glNormal3fv((float*)&m_normals [index            ].Vertex[0]);
            glVertex3fv((float*)&m_vertices[index            ].Vertex[0]);
            glVertex3fv((float*)&m_vertices[index + m_width  ].Vertex[0]);
          }
        }
      } else {
        DWORD color = 0;
        for (x=0; x < m_width; x++) {
          int index = (y*m_width)+x;
          if (color != m_colors[index]) {
            color = m_colors[index];
            float r = (color>>0 & 0xFF)/255.0;
            float g = (color>>8 & 0xFF)/255.0;
            float b = (color>>8 & 0xFF)/255.0;
            glColor4f(r,g,b,gAlphaTransp);
          }
          glNormal3fv((float*)&m_normals [index            ].Vertex[0]);
          glVertex3fv((float*)&m_vertices[index            ].Vertex[0]);
          glVertex3fv((float*)&m_vertices[index + m_width  ].Vertex[0]);
        }
      }
      glEnd();
    }
    glPopMatrix();
  }

  if (0) {
    gAlphaTransp = 0.25;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glPushMatrix();
    glTranslatef(m_BlockLaengeS*100.0,m_BlockBreiteS*100.0,m_BlockHoeheS*100.0);
    int rechts=m_width;
    int unten =m_height-1;

    unsigned int clr = 0xFF00FFFF;//gFarbe256[y&0xFF];

    glColor4f(
      (clr>>0 & 0xFF) / 255.0,
      (clr>>8 & 0xFF) / 255.0,
      (clr>>16 & 0xFF)/ 255.0,1);
    for (int y=0; y < m_height-1; y++) {
      glBegin(GL_LINE_STRIP);
      //glBegin(GL_QUAD_STRIP);
      int visible=1;
      for (x=0; x < m_width; x++) {
        int index = (y*m_width)+x;
        glVertex3fv((float*)&m_vertices[index            ].Vertex[0]);
        glVertex3fv((float*)&m_vertices[index + m_width  ].Vertex[0]);
      }
      glEnd();
    }
    glPopMatrix();

  }

  if (0) {
    // show normale
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glPushMatrix();
    glTranslatef(m_BlockLaengeS*100.0,m_BlockBreiteS*100.0,m_BlockHoeheS*100.0);
    vector3 DFace[3];
    int rechts=m_width;
    int unten =m_height-1;
    for (int y=0; y < unten; y++) {

      unsigned int clr = gFarbe256[3];

      glColor4f(
        (clr>>0 & 0xFF) / 255.0,
        (clr>>8 & 0xFF) / 255.0,
        (clr>>16 & 0xFF)/ 255.0,1);

      glBegin(GL_LINES);
      int index = y * rechts *2;
      for (int x=0; x < rechts*2; x++) {
        DFace[0] = DFace[1];
        DFace[1] = DFace[2];
        DFace[2] = m_vertices[index];
        if (x>1 && m_colors[index] !=m_Oberflaeche ) {
          float nsx = (DFace[0].Vertex[0] + DFace[1].Vertex[0] + DFace[2].Vertex[0] )/3.0 ;
          float nsy = (DFace[0].Vertex[1] + DFace[1].Vertex[1] + DFace[2].Vertex[1] )/3.0 ;;
          float nsz = (DFace[0].Vertex[2] + DFace[1].Vertex[2] + DFace[2].Vertex[2] )/3.0 ;;
          glVertex3f(nsx,nsy,nsz);
          float nx = nsx + m_normals[index].Vertex[0]*50.0;
          float ny = nsy + m_normals[index].Vertex[1]*50.0;
          float nz = nsz + m_normals[index].Vertex[2]*50.0;
          glVertex3f(nx,ny,nz);
        }
        index++;
      }
      glEnd();
    }


    glPopMatrix();

  }
  LeaveCriticalSection(&CBlockSimulation::s_critical_section);
  glDisable ( GL_COLOR_MATERIAL ) ;
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHT6);
  glDisable(GL_LIGHTING);


#else
  if (m_NumPoints >2 ) {
    //glGenBuffers  : already done

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints  * sizeof(float) * 3, &m_vertices[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints * sizeof(float) * 3, &m_colors[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumPoints * sizeof(float) * 3, &m_normals[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      1,//NormalAttribute,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    if (m_Texture>=0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,rgTextureData[m_Texture].glTexture); //Texture);
      glUniform1i(TextureParameter,1);
      GLint v = 0;
      glGetIntegerv(GL_ACTIVE_TEXTURE,&v);

      glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
      glBufferData(GL_ARRAY_BUFFER, m_NumPoints * sizeof(float) * 2, &m_UV[0].Vertex[0], GL_STATIC_DRAW);
      glVertexAttribPointer(
        3,//UVAttribute
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
      );
    } else {
      glUniform1i(TextureParameter,0);
    }

    glEnableVertexAttribArray(positionAttribute);
    glEnableVertexAttribArray(ColorAttribute);
    glEnableVertexAttribArray(NormalAttribute);
    if (m_Texture>=0) {
      glEnableVertexAttribArray(UVAttribute);
    }



    glUniform4f(uFixColor,0,0,0,0);
    glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,gAlphaTransp);  //m_Alpha);//

    /*glPushMatrix();
        int zeile = (m_width*2);
        for (int y=0;y < (m_height-3);y++) {
          if ((y*zeile+zeile) < m_NumPoints) {
            glDrawArrays(GL_TRIANGLE_STRIP, y*zeile,zeile);
            glRotatef(1,1,0,0);
          } else {
            printf("zu viel \n");
          }

        }
    glPopMatrix();
    */
    int zeile = (m_width*2);
    for (int y=0; y < (m_height-3); y++) {
      if ((y*zeile+zeile) < m_NumPoints) {
        glDrawArrays(GL_TRIANGLE_STRIP, y*zeile,zeile);
      } else {
        printf("zu viel \n");
      }

    }
//    }else {
//   glDrawArrays(GL_TRIANGLE_STRIP, 0,m_NumPoints);
//    }
    if (gAlphaTransp<0.2) {
      glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,1);
      glUniform4f(uFixColor,0,1,0,1);
      glLineWidth(2);
      glDrawArrays(GL_LINES, 0, m_NumPoints*3);
    }
    glDisableVertexAttribArray(positionAttribute);
    glDisableVertexAttribArray(NormalAttribute);
    glDisableVertexAttribArray(ColorAttribute);
    if (m_Texture>=0) {
      glDisableVertexAttribArray(UVAttribute);
    }
  }

#endif
}

//-------------------------------------------------------------
void CBlockSimulation::CreateMeissel()
{
  if (m_rgMeisselTiefe) free (m_rgMeisselTiefe);
  if (m_Raster>0) {
    int   MeisselDurchmesser = (m_MeisselDurchmesser/m_Raster);
    if (MeisselDurchmesser <1) MeisselDurchmesser = 1;
    float Tiefe  = 30.0/m_Raster; // 30 mm
    float radius  = MeisselDurchmesser/2;
    m_MeisselPxlWidth = radius*2 +1;
    m_rgMeisselTiefe = (float*)malloc( m_MeisselPxlWidth  * m_MeisselPxlWidth  *sizeof(float));
    float radius2 = radius * radius;
    int   ix=0;
    float Mtiefe = 0;
    if (m_MeisselForm==2) {
      Mtiefe = m_MeisselDurchmesser/m_Raster / 2.0;// *200/4;
      printf("radius %0.1f = %d Pxl\n",radius,m_MeisselPxlWidth);
      for (int y=-radius; y <= radius; y++) {
        int xs = sqrt(radius2-(y*y));
        for (int x=-radius; x <= radius; x++) {
          float m = (Mtiefe * ((abs(x)/radius)  + (abs (y) / radius))) - (Mtiefe/2);
          if  (abs(x) > xs) m = Tiefe;
          if (m<0) m = 0;
          m_rgMeisselTiefe[ix] = m;
          if (m>=Tiefe-1) printf(" . ");
          else            printf("%3d",(int)m);
          ix++;
        }
        printf("\n");
      }
    } else if (m_MeisselForm==1) {
      Mtiefe = m_MeisselDurchmesser/m_Raster / 1.5;//200/3;
      printf("radius %0.1f = %d Pxl\n",radius,m_MeisselPxlWidth);
      for (int y=-radius; y <= radius; y++) {
        int xs = sqrt(radius2-(y*y));
        for (int x=-radius; x <= radius; x++) {
          float m = Mtiefe * ((abs(x)/radius)  + (abs (y) / radius));
          m = (abs(x) > xs) ?Tiefe:m;
          m_rgMeisselTiefe[ix] = m;
          printf("%3d",(int)m);
          ix++;
        }
        printf("\n");
      }

    } else {

      printf("radius %0.1f = %d Pxl\n",radius,m_MeisselPxlWidth);
      for (int y=-radius; y <= radius; y++) {
        int xs = sqrt(radius2-(y*y));
        for (int x=-radius; x <= radius; x++) {
          float m = (abs(x) > xs) ?Tiefe:0;
          m_rgMeisselTiefe[ix] = m;
          ix++;
          printf("%5d",(int)m);
          //if (x==0 && y==0) printf("%c ",(abs(x) > xs) ?'?':'X');
          //else              printf("%c ",(abs(x) > xs) ?'.':'@');
        }
        printf("\n");
      }
    }
  }
}

//-------------------------------------------------------------
void CBlockSimulation::DrawMeissel(float xpos,float ypos,float zpos)
{
  if (m_rgMeisselTiefe==0) CreateMeissel();

  int   MeisselDurchmesser = m_MeisselDurchmesser/m_Raster;
  if (MeisselDurchmesser <1) MeisselDurchmesser = 1;
  float radius  = m_MeisselDurchmesser*50.0;
  int   ix=0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT7);
  glEnable( GL_COLOR_MATERIAL ) ;
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  GLfloat light7[] = { .80, .80, .90, 1.0 };
  glLightfv(GL_LIGHT7, GL_DIFFUSE,light7);// MaterialColor[8]);

  //  glBegin(GL_QUADS);
  //glColor4f(.6,1.8,1.9,1.0);
  GLfloat light1[] = { .70, .70, 1.0, 1.0 };
  GLfloat light2[] = {  .30, .3, 0.3, 1.0 };


  glColor4fv(light1);

  float w1 = 0;
  float X1 = xpos + (cos(w1) * radius);
  float Y1 = ypos - (sin(w1) * radius);
  zpos -= m_BlockHoeheE*100;
  float Z = zpos;

  const float cosn = 1;//( Tiefe0  / sqrt ( Tiefe0 * Tiefe0 + radius * radius));
//const float sinn = -1;//( radius / sqrt ( Tiefe0 * Tiefe0 + radius * radius));

#define SECTORSM  0X7
#define SECTORSN  8
  static float rot = 0;
  rot += (2.0 * M_PI) /25.0;
  if (rot > (2.0 * M_PI) ) rot -= (2.0 * M_PI) ;

  float rgXPos[17] ;
  float rgYPos[17] ;
  float rgXPos2[17] ;
  float rgYPos2[17] ;
  float Mtiefe = 0;
  if (m_MeisselForm==2) {
    Mtiefe = radius*2/3;
  } else if (m_MeisselForm==1) {
    Mtiefe = radius*1.5;
  }

  float Tiefe2  = 3000;
  float Tiefe1  = 1600;
  float Tiefe0  = (Tiefe1 - Mtiefe)/2 + Mtiefe;


  {
    glBegin(GL_TRIANGLE_STRIP);
    for (int ii = 0; ii <= SECTORSN; ii++) {

      float w = rot+(2.0 * M_PI) * ii / SECTORSN;
      float X = xpos + (cos(w) * radius);
      float Y = ypos - (sin(w) * radius);
      glNormal3f(cos(w), sin(w)*-1,cosn);
      glVertex3f(X, Y, Z+Tiefe2);
      rgXPos[ii] = X;
      rgYPos[ii] = Y;
      glVertex3f(X,Y, Z+Tiefe1);
    }
    glEnd();
  }

  if (1) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j1 = 0; j1 <= SECTORSN; j1++) {
      int j2 = (j1-1) & SECTORSM;
      float w = rot+(2.0 * M_PI) * ((float)j1+0.5) / SECTORSN;
      glNormal3f(cos(w), sin(w)*-1,cosn);
      if ((j1&3)==1) glColor4fv(light2);
      else           glColor4fv(light1);
      glVertex3f(rgXPos[j1],rgYPos[j1],   Z+Tiefe1);
      glVertex3f(rgXPos[j2],rgYPos[j2],   Z+Tiefe0);
    }
    glEnd();
  }
  if (1) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j1 = 0; j1 <= SECTORSN; j1++) {
      int j2 = (j1-1) & SECTORSM;
      float w = rot+(2.0 * M_PI) * ((float)j1+1.5) / SECTORSN;
      glNormal3f(cos(w), sin(w)*-1,cosn);
      if ((j1&3)==0) glColor4fv(light2);
      else           glColor4fv(light1);
      glVertex3f(rgXPos[j1],rgYPos[j1],   Z+Tiefe0);
      glVertex3f(rgXPos[j2],rgYPos[j2],   Z+Mtiefe);
    }
    glEnd();
  }

  glColor4fv(light1);
  if (m_MeisselForm<2) {
    glBegin(GL_TRIANGLES);
    float w = rot+(2.0 * M_PI) * 0 / SECTORSN;
    glNormal3d(cos(w), sin(w)*-1, cosn);

    for (int j=0; j<SECTORSN; j++) {
      glVertex3f(rgXPos[j+0],rgYPos[j+0],   Z+Mtiefe);
      glVertex3f(xpos,              ypos,              zpos);
      glNormal3f(cos(w), sin(w)-1, cosn);
      glVertex3f(rgXPos[j+1],rgYPos[j+1],   Z+Mtiefe);
    }
    glEnd();
  } else {
    float radiusiinen = radius / 2;
    for (int ii = 0; ii <= SECTORSN; ii++) {
      float w = rot+(2.0 * M_PI) * ii / SECTORSN;
      rgXPos2[ii] =  xpos + (cos(w) * radiusiinen);
      rgYPos2[ii] =  ypos - (sin(w) * radiusiinen);
    }

    glBegin(GL_TRIANGLES);
    for (int j=0; j<SECTORSN; j++) {
      if ((j&3)==2) {
        glColor4fv(light2);
      } else {
        glColor4fv(light1);
      }
//      if ((j&3)>=2) {
//
//      m_3DFace[0].Vertex[0] = rgXPos2[j+0];
//      m_3DFace[0].Vertex[1] = rgYPos2[j+0];
//      m_3DFace[0].Vertex[2] = Z+Mtiefe;
//      m_3DFace[1].Vertex[0] = rgXPos2[j+0];
//      m_3DFace[1].Vertex[1] = rgYPos2[j+0];
//      m_3DFace[1].Vertex[2] = Z;
//      m_3DFace[2].Vertex[0] = rgXPos2[j+1];
//      m_3DFace[2].Vertex[1] = rgYPos2[j+1];
//      m_3DFace[2].Vertex[2] = Z+Mtiefe;
//      CalculateNormale(1);
//      glNormal3fv((float*)&m_Normal.Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[0].Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[1].Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[2].Vertex[0]);
//
//      m_3DFace[0].Vertex[0] = rgXPos2[j+1];
//      m_3DFace[0].Vertex[1] = rgYPos2[j+1];
//      m_3DFace[0].Vertex[2] = Z+Mtiefe;
//      m_3DFace[1].Vertex[0] = rgXPos2[j+0];
//      m_3DFace[1].Vertex[1] = rgYPos2[j+0];
//      m_3DFace[1].Vertex[2] = Z;
//      m_3DFace[2].Vertex[0] = rgXPos2[j+1];
//      m_3DFace[2].Vertex[1] = rgYPos2[j+1];
//      m_3DFace[2].Vertex[2] = Z;
//      CalculateNormale(1);
//      glNormal3fv((float*)&m_Normal.Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[0].Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[1].Vertex[0]);
//      glVertex3fv((float*)&m_3DFace[2].Vertex[0]);
//      }
      m_3DFace[0].Vertex[0] = rgXPos[j+0];
      m_3DFace[0].Vertex[1] = rgYPos[j+0];
      m_3DFace[0].Vertex[2] = Z+Mtiefe;
      m_3DFace[1].Vertex[0] = rgXPos2[j+0];
      m_3DFace[1].Vertex[1] = rgYPos2[j+0];
      m_3DFace[1].Vertex[2] = Z;
      m_3DFace[2].Vertex[0] = rgXPos[j+1];
      m_3DFace[2].Vertex[1] = rgYPos[j+1];
      m_3DFace[2].Vertex[2] = Z+Mtiefe;
      CalculateNormale(1);
      glNormal3fv((float*)&m_Normal.Vertex[0]);
      glVertex3fv((float*)&m_3DFace[0].Vertex[0]);
      glVertex3fv((float*)&m_3DFace[1].Vertex[0]);
      glVertex3fv((float*)&m_3DFace[2].Vertex[0]);

      m_3DFace[0].Vertex[0] = rgXPos[j+1];
      m_3DFace[0].Vertex[1] = rgYPos[j+1];
      m_3DFace[0].Vertex[2] = Z+Mtiefe;
      m_3DFace[1].Vertex[0] = rgXPos2[j+0];
      m_3DFace[1].Vertex[1] = rgYPos2[j+0];
      m_3DFace[1].Vertex[2] = Z;
      m_3DFace[2].Vertex[0] = rgXPos2[j+1];
      m_3DFace[2].Vertex[1] = rgYPos2[j+1];
      m_3DFace[2].Vertex[2] = Z;
      CalculateNormale(1);
      glNormal3fv((float*)&m_Normal.Vertex[0]);
      glVertex3fv((float*)&m_3DFace[0].Vertex[0]);
      glVertex3fv((float*)&m_3DFace[1].Vertex[0]);
      glVertex3fv((float*)&m_3DFace[2].Vertex[0]);
    }


    glEnd();
    {
      glColor4f(1,1,1,1);
      glBegin(GL_TRIANGLES);

      m_3DFace[1].Vertex[0] = xpos;
      m_3DFace[1].Vertex[1] = ypos;
      m_3DFace[0].Vertex[2] = Z;
      m_3DFace[1].Vertex[2] = zpos;
      m_3DFace[2].Vertex[2] = Z;
      for (int j=0; j<SECTORSN; j++) {
        m_3DFace[0].Vertex[0] = rgXPos2[j+0];
        m_3DFace[0].Vertex[1] = rgYPos2[j+0];
        m_3DFace[2].Vertex[0] = rgXPos2[j+1];
        m_3DFace[2].Vertex[1] = rgYPos2[j+1];
        CalculateNormale(1);
        glNormal3fv(m_Normal.Vertex);
        glVertex3fv(m_3DFace[0].Vertex);
        glVertex3fv(m_3DFace[1].Vertex);
        glVertex3fv(m_3DFace[2].Vertex);
      }
      glEnd();
    }
  }
  glDisable(GL_COLOR_MATERIAL ) ;
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT7);

}

//-------------------------------------------------------------
extern int GcodeNmr;

void CBlockSimulation::Meissel(float xSoll,float ySoll,float zSoll,float aWinkel)
{
  //fprintf(stderr,"Fast X%5.2f  Y%5.2f  Z%5.2f  A%5.2f ",xSoll/100.0,ySoll/100.0,zSoll/100.0,aWinkel/100.0);

  EnterCriticalSection(&CBlockSimulation::s_critical_section);

  xSoll -= m_BlockLaengeS*100.0;
  float aSoll=0;
  // Umrechnen auf TiefenArray Raster bezogen
  float gFaktor = 100.0 * m_Raster;
  if (gPa.Maschine[eDrehachse]) {
    aSoll  = aWinkel /-100.0;
    zSoll /= gFaktor;
    xSoll /= gFaktor;
    ySoll /= gFaktor;
  } else {
    ySoll -= (m_BlockBreiteS* 100.0);
    zSoll += (m_BlockHoeheE * 100.0);//Z-Nullpunkt auf der Oberflaeche ?
    zSoll /= gFaktor;
    ySoll /= gFaktor;
    xSoll /= gFaktor;
  }
  //fprintf(stderr," -> X%4.0f  Y%4.0f  Z%4.0f  A%4.0f  \n",xSoll,ySoll,zSoll,aSoll);
  if (m_Xlast != -1E10) {
    // Bestimme Massgebliche Achse
    float xdiff = xSoll-m_Xlast;
    float ydiff = ySoll-m_Ylast;
    float zdiff = zSoll-m_Zlast;
    float adiff = aSoll-m_Alast;

    float adiffx = fabs(xdiff);
    float adiffy = fabs(ydiff);
    float adiffz = fabs(zdiff);
    float adiffa = fabs(adiff);
    enum {XAchse=1,YAchse,ZAchse,AAchse};
    int Achse = 0;

    if (adiffx > adiffy) {
      if (adiffx > adiffz) {
        if (adiffx > adiffa) Achse = XAchse;
        else                 Achse = AAchse;
      } else {
        if (adiffz > adiffa) Achse = ZAchse;
        else                 Achse = AAchse;
      }
    } else {
      if (adiffy > adiffz) {
        if (adiffy > adiffa) Achse = YAchse;
        else                 Achse = AAchse;
      } else {
        if (adiffz > adiffa) Achse = ZAchse;
        else                 Achse = AAchse;
      }
    }

    // Bestimme Verhaeltnis zur Hauptachse
    int incs;
    float XAddition;
    float YAddition;
    float ZAddition;
    float AAddition;
    if (Achse == XAchse) {
      incs = adiffx;
      XAddition = xdiff / adiffx;
      YAddition = ydiff / adiffx;
      ZAddition = zdiff / adiffx;
      AAddition = adiff / adiffx;
    } else if (Achse == YAchse) {
      incs = adiffy;
      XAddition = xdiff / adiffy;
      YAddition = ydiff / adiffy;
      ZAddition = zdiff / adiffy;
      AAddition = adiff / adiffy;
    } else if (Achse == ZAchse) {
//      adiffz = 1.0;
      incs = adiffz;
      XAddition = xdiff / adiffz;
      YAddition = ydiff / adiffz;
      ZAddition = zdiff / adiffz;
      AAddition = adiff / adiffz;
    } else {
      incs = adiffa;
      XAddition = xdiff / adiffa;
      YAddition = ydiff / adiffa;;
      ZAddition = zdiff / adiffa;
      AAddition = adiff / adiffa;
    }


    while (incs) {
      if (gPa.Maschine[eDrehachse]) {
        float resZ = m_Zlast;
        float resA = m_Alast;
        /*    if (m_Ylast) {
              // Y-Verschiebung bestimmt Winkel und Tiefe
              resZ = sqrt((m_Ylast * m_Ylast)+(m_Zlast*m_Zlast));
              resA = m_Alast - ((atan2f(m_Zlast,m_Ylast )- M_PI_2)* (360.0/M_2PI));
            }
            float resW = (fmod(resA,360.0f) * (float)m_height / 360.0f);
            if (resW<0)         resW+= m_height;
            if (resW>=m_height) resW-= m_height;
        */
        float dw = m_Alast * 2*M_PI / 360.0;
        m_Sin = sin(dw);
        m_Cos = cos(dw);

        //MeisselLinien(m_Xlast,resW + m_Ylast ,resZ,0,0);
        MeisselLinien(m_Xlast,m_Ylast,m_Zlast,m_Alast,0,0);
      } else {
        MeisselLinien(m_Xlast,m_Ylast,m_Zlast,0,0,0);
      }
      m_Xlast += XAddition;
      m_Ylast += YAddition;
      m_Zlast += ZAddition;
      m_Alast += AAddition;
      incs--;
    }
  } else {
    m_Xlast = xSoll;
    m_Ylast = ySoll;
    m_Zlast = zSoll;
    m_Alast = aSoll;
  }
  LeaveCriticalSection(&CBlockSimulation::s_critical_section);
}
//-------------------------------------------------------------

void CBlockSimulation::MeisselLinien(int xPos,int yPos,float zPos,float aPos,int w,int h)
{
  int radius = m_MeisselPxlWidth / 2;// MeisselDurchmesser / 2;
  if (radius==0) radius=1;
  int tsize  = m_width * m_height;
  int MSize  = m_MeisselPxlWidth  * m_MeisselPxlWidth;

#ifdef USE_SHADER
  float raster? = m_Raster;
  float fradius? = m_MeisselDurchmesser *0.50;
#else
  float gfaktor = m_Raster*100.0;
  //float fradius = m_MeisselDurchmesser *50.0 * m_Raster;
  //float oradius = m_BlockDurchmesser*50.0 * m_Raster;
  //float fradius = m_MeisselDurchmesser / m_Raster;
  float oradius  = m_BlockDurchmesser / m_Raster/2.0;
  float oradius2 = m_BlockDurchmesser *50;
#endif

  float wD    = 0;
  int    WtoY =  yPos;
  if (gPa.Maschine[eDrehachse]) {
    if (yPos) {
      float wD0   = 0;
      // Kreuzung Y und Aussenkannte = maximum innerhalb
      wD0 = acos(zPos/oradius) *  (360.0/M_2PI);
      wD  = asin(yPos/oradius) *  (360.0/M_2PI);
      //wD1 = ((atan2f(zPos,yPos)- M_PI_2)* (360.0/M_2PI));
      //wD = ((atan2f(oradius -zPos,yPos)- M_PI_2)* (360.0/M_2PI));

      if      (wD >  wD0) return ;
      else if (wD < -wD0) return ;

    }
    WtoY = (aPos+wD) * (float)m_height / 360.0f;
    //float resW = (fmod(resA,360.0f) * (float)m_height / 360.0f);
    while (WtoY<0)         WtoY+= m_height;
    while (WtoY>=m_height) WtoY-= m_height;

  }
  int quadrant = ((WtoY) / (m_height/4))&0x3;
  if (GcodeNmr==405){
    //      fprintf(stderr,"X%3d  Y%3d  Z%4.0f  A%4.0f wD=%4.2f wtoy=%d  Quad %d\n",xPos,yPos,zPos,aPos,wD,WtoY,quadrant);
  }




  int it =0;
  for (int y=-radius; y < radius; y++) {
//    int
//    quadrant = ((WtoY+y) / (m_height/4))&0x3;

    for (int rx=-radius; rx<=radius; rx++) {
      if (it>=0 && it < MSize) {
        float t = zPos+m_rgMeisselTiefe[it];
        int tp = ((WtoY+y)*m_width)+xPos+rx;
        if (tp >= 0 && tp < tsize) {
          if (1||m_rgTiefe[tp] > t) {
            m_rgTiefe[tp] = t;
            if (gPa.Maschine[eDrehachse]) {
              int index = tp;
              m_3DFace[0] = m_vertices[index];
              m_3DFace[2] = m_3DFace[0];
              float t3d = sqrt((m_3DFace[2].Vertex[1]*m_3DFace[2].Vertex[1])+(m_3DFace[2].Vertex[2]*m_3DFace[2].Vertex[2]));
              float radius2  = t;
              if (radius2 < -oradius )  radius2 = -(oradius);

              radius2  *= gfaktor;
              float Verdickung =gfaktor * (y + yPos);
              m_3DFace[2].Vertex[1] = (radius2 * m_Sin) + (Verdickung * m_Cos);
              m_3DFace[2].Vertex[2] = (radius2 * m_Cos) - (Verdickung * m_Sin);
              float t3d2 = sqrt((m_3DFace[2].Vertex[1]*m_3DFace[2].Vertex[1])+(m_3DFace[2].Vertex[2]*m_3DFace[2].Vertex[2]));
              if (t3d2 < t3d && t3d2 < oradius2) {
                m_vertices[index] = m_3DFace[2];
                if ((xPos+rx) < m_updateSX) m_updateSX = xPos+rx;
                if ((xPos+rx) > m_updateEX) m_updateEX = xPos+rx;
                if ((WtoY+y ) < m_updateSY) m_updateSY = WtoY+y;
                if ((WtoY+y ) > m_updateEY) m_updateEY = WtoY+y;
              }
            } else {
              t*=gfaktor;
              if (m_vertices[tp].Vertex[2] > t)  {
                m_vertices[tp].Vertex[2] = t;
                if ((xPos+rx) < m_updateSX) m_updateSX = xPos+rx;
                if ((xPos+rx) > m_updateEX) m_updateEX = xPos+rx;
                if ((WtoY+y ) < m_updateSY) m_updateSY = WtoY+y;
                if ((WtoY+y ) > m_updateEY) m_updateEY = WtoY+y;
              }
            }
          }
        }
      }
      it++;
    }
  }
}

//-------------------------------------------------------------

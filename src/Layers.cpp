//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include <FL/fl_draw.H>
#include "TreeView.h"
#include "Layers.h"
#include "Resource.h"
#include "Object.h"

//-------------------------------------------------------------
int AktLayer=0;
int LayerSwitchModify=0;
CLayers * gpLayers=NULL;
//-------------------------------------------------------------
CLayers::CLayers(const void * ObjSource,const char * Layer)
{
  m_NextLayer=NULL;
  m_Name = strdup(Layer);
  m_Color =0;
  m_LayerSwitch=1;
  m_NumObjects=0;
  m_ObjSource = ObjSource;
}

//-------------------------------------------------------------
CLayers::~CLayers()
{
  if (m_Name) free(m_Name);
  if (m_NextLayer) delete m_NextLayer;
}

//-------------------------------------------------------------
CLayers * IsLayerIndex(const void * ObjSource,const char * Layer)
{
  int ix=0;
  if (gpLayers) {
    CLayers * pLayer=gpLayers;
    while (pLayer) {
#ifndef FINGERSCROLL
      if (pLayer->m_ObjSource == ObjSource)
#endif
      {
        if (pLayer->m_Name) {
          if(strcmp(Layer,pLayer->m_Name)==0) {
            return pLayer;
          }
        }
      }
      ix++;
      pLayer = pLayer->m_NextLayer;
    }
  }
  return NULL;
}

//-------------------------------------------------------------
CLayers * GetLayer(int Layer)
{
  if (gpLayers) {
    CLayers * pLayer=gpLayers;
    int ix = -1;
    while (pLayer) {
      ix++;
      if (Layer == ix) return pLayer;
      pLayer = pLayer->m_NextLayer;
    }
  }
  return NULL;
}

//-------------------------------------------------------------
CLayers * GetLayerIndex(const void * ObjSource,const char * Layer)
{
#if 0 //def DREITABLE
  int ix=0;
  if (gpLayers) {
    CLayers * pLayer=gpLayers;
    pLayer=gpLayers;
    CLayers * prevLayer=gpLayers;
    while (pLayer) {
#ifndef FINGERSCROLL
      if (pLayer->m_ObjSource == ObjSource)
#endif
      {
        if (pLayer->m_Name) {
          if(strcmp(Layer,pLayer->m_Name)==0) {
            pLayer->m_NumObjects++;
            return pLayer;
          } else if(strcmp(Layer,pLayer->m_Name)<0) {
            CLayers * NewLayer = new CLayers(ObjSource,Layer);
            NewLayer->m_NextLayer = pLayer;
            prevLayer->m_NextLayer = NewLayer;
            NewLayer->m_NumObjects++;
            return NewLayer;
          }
        }
      }
      prevLayer = pLayer;
      if (pLayer->m_NextLayer) pLayer = pLayer->m_NextLayer;
      else break;
    }
    if (pLayer) {
      pLayer->m_NextLayer = new CLayers(ObjSource,Layer);
      pLayer->m_NextLayer->m_NumObjects++;
      return pLayer->m_NextLayer;
    }
  } else {
    gpLayers = new CLayers(ObjSource,Layer);
    return gpLayers;
  }
  return NULL;
#else
  int ix=0;
  if (gpLayers) {
    CLayers * pLay = IsLayerIndex(ObjSource,Layer);
    if (pLay) {
      pLay->m_NumObjects++;
      return pLay;
    }

    CLayers * pLayer=gpLayers;
    pLayer=gpLayers;
    CLayers * prevLayer=gpLayers;
    while (pLayer) {
#ifndef FINGERSCROLL
      if (pLayer->m_ObjSource == ObjSource)
#endif
      {
        if (0 && pLayer->m_Name) {
          if(strcmp(Layer,pLayer->m_Name)==0) {
            pLayer->m_NumObjects++;
            return pLayer;
          } else if(strcmp(Layer,pLayer->m_Name)<0) {
            CLayers * NewLayer = new CLayers(ObjSource,Layer);
            NewLayer->m_NextLayer = pLayer;
            prevLayer->m_NextLayer = NewLayer;
            NewLayer->m_NumObjects++;
            return NewLayer;
          }
        }
      }
      prevLayer = pLayer;
      if (pLayer->m_NextLayer) pLayer = pLayer->m_NextLayer;
      else break;
    }
    if (pLayer) {
      pLayer->m_NextLayer = new CLayers(ObjSource,Layer);
      pLayer->m_NextLayer->m_NumObjects++;
      return pLayer->m_NextLayer;
    }
  } else {
    gpLayers = new CLayers(ObjSource,Layer);
    gpLayers->m_NumObjects++;
    return gpLayers;
  }
#endif
  return NULL;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
#ifdef FINGERSCROLL
CFingerScroll     * gLayerFingerScroll;

//-------------------------------------------------------------
void CLayer_Button::draw()
{
  if (box()) draw_box(this==Fl::pushed() ? fl_down(box()) : box(), color());
  Fl_Color col = value() ? (active_r() ? selection_color() :
                            fl_inactive(selection_color())) : color();

  int W  = labelsize();
  int bx = Fl::box_dx(box());
  int dx = bx + 4;
  int dy = (h() - W) / 2;
  int lx = 0;

  int hh = h()-2*dy - 2;
  int xx = dx;
  if (w()<W+2*xx) xx = (w()-W)/2;
  if (0) {  //Fl::is_scheme("plastic")) {
    col = active_r() ? selection_color() : fl_inactive(selection_color());
    fl_color(value() ? col : fl_color_average(col, FL_BLACK, 0.5f));
    fl_pie(x()+w()-3 - W, y()+dy+1, W, hh, 0, 360);
  }
  if (m_ChildItem) {
    int cnt = 0;
    int con = 0;
    CLayer_Button * it = m_ChildItem;
    while (it) {
      if (it->value()) con++;
      cnt ++;
      it = it->m_Next;
    }
    col = active_r() ? selection_color() : fl_inactive(selection_color());
    fl_color(value() ? col : fl_color_average(col, FL_BLACK, 0.5f));
//    fl_pie(x()+w()-3 - W , y()+dy+1, W, hh, 0, 360);
    draw_box(FL_THIN_DOWN_BOX,x()+w()-3 - W, y()+dy+1, W, hh, fl_color_average(col, FL_BLACK,(float)con / (float)cnt));

    fl_draw_symbol(m_open?"@+52>":"@+5>>",x()+xx, y()+dy+1,W,W,FL_YELLOW);
  } else {
    draw_box(FL_THIN_DOWN_BOX,x()+w()-3 - W, y()+dy+1, W, hh, col);
  }
  lx = dx + W + 2;
  draw_label(x()+lx, y(), w()-lx-bx-W-3, h());
  if (Fl::focus() == this) draw_focus();
}

//-------------------------------------------------------------
int CLayer_Button::handle(int event)
{
  int newval;
  // get Messages for Layer Tree only
  switch (event) {
  case FL_PUSH :
  case FL_DRAG:
    if (Fl::event_inside(this)) {
      if (Fl::event_x()< w()/4) {
        return 1;
      }
    }
    break;
  case FL_RELEASE:
    if (box()) redraw();
    if (Fl::event_inside(this)) {
      if (Fl::event_x()< w()/4) {
        m_open = !m_open;
        if (m_open) Expand();
        else        Collapse();
        return 1;
      }
    }
    break;
  case FL_KEYBOARD :
    if (Fl::focus() == this) {
      int  ch = Fl::event_key();
      if (ch == FL_Left) {
        if (m_ChildItem && m_open) {
          Collapse();
          return 1;
        }
      } else if (ch == FL_Right) {
        if (m_ChildItem && !m_open) {
          Expand();
          return 1;
        }
      } else if (ch == ' ') {
        if (!(Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META))) {
          value(!value());
          do_callback();
          return 1;
        }
      }
    }
  }



  return Fl_Button::handle(event);

}

//-------------------------------------------------------------
void CLayer_Button::Expand()
{
  m_open = 1;
  CLayer_Button * it = m_ChildItem;
  while (it) {
    it->resize(x(),y(),w(),h());
    it->show();
    it = it->m_Next;
  }
  gLayerFingerScroll->m_pack->redraw();
  gLayerFingerScroll->redraw();
}
//-------------------------------------------------------------
int recurs =0;
void CLayer_Button::Collapse()
{
  m_open = 0;
  CLayer_Button * it = m_ChildItem;
  recurs++;
  while (it) {
    it->resize(x(),y(),w(),0);
    it->hide();
    if (it->m_ChildItem) {
      it->m_ChildItem->Collapse();
      it->m_ChildItem->resize(x(),y(),w(),0);
      it->m_ChildItem->hide();
    }
    it = it->m_Next;
  }
  recurs--;
  if (recurs==0) {
    gLayerFingerScroll->m_pack->redraw();
    gLayerFingerScroll->redraw();
  }
}
//-------------------------------------------------------------
void CLayer_Button::AddChild(CLayer_Button * child)
{
  align(Fl_Align(FL_ALIGN_CLIP)|FL_ALIGN_INSIDE|FL_ALIGN_LEFT| FL_ALIGN_WRAP);
  child->align(Fl_Align(FL_ALIGN_CLIP)|FL_ALIGN_INSIDE|FL_ALIGN_RIGHT| FL_ALIGN_WRAP);
  CLayer_Button * it = m_ChildItem;
  if (it  == child) return;
  if (it) {
    while (it) {
      if (it->m_Next) {
        it = it->m_Next;
      } else {
        it->m_Next = child;
        it=NULL;
        break;
      }
    }
  } else {
    m_ChildItem = child;
  }
}

//-------------------------------------------------------------
CLayer_Button::CLayer_Button(int X, int Y, int W, int H, const char* l)
  : Fl_Button(X, Y, W, H, l)
{
  m_ChildItem=NULL;
  m_Next=NULL;
  m_open = 1;
  type(FL_TOGGLE_BUTTON);
  selection_color(FL_YELLOW);
  align(Fl_Align(FL_ALIGN_CLIP)|FL_ALIGN_INSIDE|FL_ALIGN_CENTER| FL_ALIGN_WRAP);
}


//-------------------------------------------------------------
void  cbDxfLayerParent(Fl_Widget* item, void* idc)
{
  CLayer_Button * b = (CLayer_Button *)item;
  if (b==NULL) return;
  CLayer_Button * it = b->m_ChildItem;
  while (it) {
    CLayers * pLayer = (CLayers *)it->user_data() ;// GetLayer(i);
    if (pLayer) {
      pLayer->m_LayerSwitch = b->value();
      it->value(b->value());
    }
    it = it->m_Next;
  }
  UpdateMainWindow();
}
//-------------------------------------------------------------
void  cbDxfLayerButton(Fl_Widget* item, void* idc)
{
  long i = (long)idc;
  CLayer_Button * b = (CLayer_Button *)item;
  if (b) {
    CLayers * pLayer = (CLayers *)idc;// GetLayer(i);
    if (pLayer) {
      pLayer->m_LayerSwitch = b->value();
    }
  }
  LayerSwitchModify=1;

  // Search Parent Button
  int numItems = gLayerFingerScroll->m_pack->children();
  CLayer_Button * parent = (CLayer_Button *)item;
  int found = -1;
  for (i=numItems-1; i>= 0; i--) {
    CLayer_Button * test = (CLayer_Button *)gLayerFingerScroll->m_pack->child(i);
    if (test) {
      if (test==b) found = i;
      if (test->m_ChildItem && found>=0) {
        found = -1;
        int cnt = 0;
        int con = 0;
        CLayer_Button * it = test->m_ChildItem;
        while (it) {
          if (it->value()) con++;
          cnt ++;
          it = it->m_Next;
        }
        // Update Led
        test->value(con!=0);
        test->redraw();
        break;
      }
    }
  }
  UpdateMainWindow();
}
//-------------------------------------------------------------
void AdjustLayerSwitches()
{
  /*  CLayers * pLayer= gpLayers;
    CLayers * pParentLayer= 0;
    char * szparent=NULL;
    while (pLayer) {
      char szLayerName[256];
      strncpy(szLayerName,pLayer->m_Name,255);
      char * subitem = strchr(szLayerName,'.');
      if (subitem==NULL ) {
        subitem = strchr(szLayerName,'|');
      }
      if (subitem) {
        *subitem = '\0';
        subitem++;
        if (szparent && strcmp(szLayerName,szparent)!=0) {
          pParentLayer=NULL;
        }
      } else {
        subitem = szLayerName;
        pParentLayer=pLayer;
      }
      if (pLayer->m_NumObjects) {
        if (pParentLayer) {
          if (pParentLayer->m_LayerSwitch==0) pLayer->m_LayerSwitch = 0;
        }
      }
      pLayer = pLayer->m_NextLayer;

    }
    */
}

//-------------------------------------------------------------

void gShowLayerButtons(int neu)
{
  int i=0;
  if (neu==-1) {
    if (gpLayers) {
      CLayers * pLayer=gpLayers;
      while (pLayer) {
        if (pLayer->m_Name) {
          printf("%s\n",pLayer->m_Name);
        }
      pLayer = pLayer->m_NextLayer;
      }
    }
  }

  if (gLayerFingerScroll) {
    char szWorklayer[256];
    if (1) {
      int w = gLayerFingerScroll->w() - 25;
      gLayerFingerScroll->m_pack->resize(gLayerFingerScroll->x(),gLayerFingerScroll->y(),w,gLayerFingerScroll->h());
      gLayerFingerScroll->m_pack->clear();
      gLayerFingerScroll->m_pack->begin();
      char * szparent=NULL;
      CLayer_Button * dxfFile=NULL;
      CLayer_Button * parent=NULL;
      CLayers * pLayer= gpLayers;
      //CLayers * pParentLayer= 0;
      int index=0;
      int i=0;
      while (pLayer) {
        char szLayerName[256];
        strncpy(szLayerName,pLayer->m_Name,255);
        char * subitem = strchr(szLayerName,'.');
        if (subitem==NULL ) {
          subitem = strchr(szLayerName,'|');
        }
        if (subitem)  {
          *subitem = '\0';
          subitem++;
          if (szparent && strcmp(szLayerName,szparent)!=0) {
            if (parent) parent->Collapse();
            parent = NULL;
          }
          if (parent==0) {
            szparent = szLayerName;
            parent = new CLayer_Button(0,1+index*35,12,35);
            parent->copy_label(szparent);
            parent->callback(cbDxfLayerParent,(void*)pLayer);
            index++;
          }
        } else {
          if (parent) parent->Collapse();
          subitem = szLayerName;
          parent = NULL;
        }
        if (pLayer->m_NumObjects) {
          CLayer_Button * b = new CLayer_Button(0,1+index*35,12,35);
          index++;
          if (parent) {
            parent->AddChild(b);
          }
          b->copy_label(subitem);
          b->value(pLayer->m_LayerSwitch);
          b->callback(cbDxfLayerButton,(void*)pLayer);
          i++;
        }
        pLayer = pLayer->m_NextLayer;

      }
      if (parent) parent->Collapse();
      gLayerFingerScroll->m_pack->end();
      gLayerFingerScroll->scroll_to(0,0);
      gLayerFingerScroll->redraw();
    } else {
      LayerSwitchModify=0;
      int numItems = gLayerFingerScroll->m_pack->children();
      for (i=0; i< numItems; i++) {
        CLayer_Button * b = (CLayer_Button *)gLayerFingerScroll->m_pack->child(i);
        if (b) {
          CLayers * pLayer = (CLayers *)b->user_data();
          if (pLayer) {
            if (neu<0)pLayer->m_LayerSwitch = pLayer->m_DefaultSwitch;
            else pLayer->m_LayerSwitch=neu<2?0:1;
            b->value(pLayer->m_LayerSwitch);
          }
        }
      }

    }
  }
}
#else
CTreeView         * gLayerTree;
//-------------------------------------------------------------
void AdjustLayerSwitches()
{
  int numItems = gLayerTree->m_pack->children();
  CTVLayerItem * pTVParent=NULL;
  CTVLayerItem * pTVDxfFile=NULL;
  int            TotalNumActive=0;
  int            TotalNum=0;
  int            NumActive=0;
  for (int i=0; i< numItems; i++) {
    CTVLayerItem * pTVItem1 = (CTVLayerItem *)gLayerTree->m_pack->child(i);
    if (pTVItem1) {
      if (pTVItem1->m_lParam) {
        if (pTVItem1->m_NumChild) {  // layerParent
          if (pTVParent) {
            if (NumActive==pTVParent->m_NumChild) {
              pTVParent->SetImage(imLayersActive);
            } else if (NumActive) {
              pTVParent->SetImage(imLayersPart);
            } else {
              pTVParent->SetImage(imLayers);
            }
          }
          NumActive =0;
          pTVParent=pTVItem1;
        } else {
          CLayers * pLayer = (CLayers *)pTVItem1->m_lParam;
          if (pLayer && pLayer->m_LayerSwitch) {
            pTVItem1->SetImage(imLayerActive);
            if (pTVItem1->m_Level==2) NumActive++;
            TotalNumActive++;
          } else {
            pTVItem1->SetImage(imLayer);
          }
          TotalNum++;
        }
      } else { // DxfFileItem
        if (pTVParent) {
          if (NumActive==pTVParent->m_NumChild) {
            pTVParent->SetImage(imLayersActive);
          } else if (NumActive) {
            pTVParent->SetImage(imLayersPart);
          } else {
            pTVParent->SetImage(imLayers);
          }
        }
        NumActive =0;
        pTVParent=NULL;
        if (pTVDxfFile) {
          if (TotalNumActive==TotalNum) {
            pTVDxfFile->SetImage(imDxfFileActive);
          } else if (TotalNumActive) {
            pTVDxfFile->SetImage(imDxfFilePart);
          } else {
            pTVDxfFile->SetImage(imDxfFile);
          }
        }
        pTVDxfFile=pTVItem1;
        TotalNumActive=0;
        TotalNum=0;
      }
    }
  }
  if (pTVParent) {
    if (NumActive==pTVParent->m_NumChild) {
      pTVParent->SetImage(imLayersActive);
    } else if (NumActive) {
      pTVParent->SetImage(imLayersPart);
    } else {
      pTVParent->SetImage(imLayers);
    }
  }
  NumActive =0;
  pTVParent=NULL;
  if (pTVDxfFile) {
    if (TotalNumActive==TotalNum) {
      pTVDxfFile->SetImage(imDxfFileActive);
    } else if (TotalNumActive) {
      pTVDxfFile->SetImage(imDxfFilePart);
    } else {
      pTVDxfFile->SetImage(imDxfFile);
    }
  }
  gLayerTree->redraw();
  UpdateMainWindow();
}

//-------------------------------------------------------------
int CTreeView::CreateTreeView(void)
{
  if (m_pack) {
    m_pack ->begin();
    char szWebRadioTree[80];
////.....
    int xx= gLayerTree->x();
    int xy= gLayerTree->y();
    int xw= gLayerTree->w();
    int xh= gLayerTree->h();
    m_pack ->end();
    m_pack->resize(x(),y(),w(),h());
    return m_pack->children();
  }
  return 0;
}
//-------------------------------------------------------------


int CTVLayerItem::SetState(int iState)
{
  CLayers * pLayer= (CLayers *)m_lParam;
  if (pLayer) {
    if (iState==0)  {
      pLayer->m_LayerSwitch=0;
    } else if (iState==1)  {
      pLayer->m_LayerSwitch=1;
    } else if (iState==2)  {
      pLayer->m_LayerSwitch=pLayer->m_DefaultSwitch;
    } else if (iState==-1)  {
      pLayer->m_LayerSwitch= ! (pLayer->m_LayerSwitch &1);
    }
    LayerSwitchModify++;
    AdjustLayerSwitches();
  }
  return 1;
}

//-------------------------------------------------------------
CTVLayerItem::CTVLayerItem (CTreeView * pWndControl,CTVItem * pParent, DWORD record):CTVItem(pWndControl,pParent,record)
{
  ;

}

//-------------------------------------------------------------
int CTVLayerItem::OnClick(int DoubleClick,int pos)
{
  SetState(-1);
  return SelChanged(m_pWndControl->m_AktIndex);
}

//-------------------------------------------------------------
int CTVParentLayerItem::SetState(int iState)
{
  CLayers * pLayer= (CLayers *)m_lParam;
  if (pLayer) {
    if (iState==0)  {
      pLayer->m_LayerSwitch=0;
    } else if (iState==2)  {
      pLayer->m_LayerSwitch=2;
    } else if (iState==1)  {
      pLayer->m_LayerSwitch=1;
    } else if (iState==-1)  {
      pLayer->m_LayerSwitch= ! (pLayer->m_LayerSwitch &1);
      iState = pLayer->m_LayerSwitch;
    }

    if (m_NumChild  && m_pChild) {
      CTVItem* pTVItem1 = m_pChild;
      while(pTVItem1) {
        pTVItem1->SetState(iState);
        pTVItem1 = pTVItem1->m_pNext;
      }
    }
    LayerSwitchModify++;
    AdjustLayerSwitches();
  }
  return 1;
}

//-------------------------------------------------------------
CTVParentLayerItem::CTVParentLayerItem (CTreeView * pWndControl,CTVItem * pParent, DWORD record):CTVItem(pWndControl,pParent,record)
{
  m_State=1;

}
//-------------------------------------------------------------
int CTVParentLayerItem::OnClick(int DoubleClick,int pos)
{
  int ident = x()+ 5+ ICONSIZE+ m_Level*ICONSIZE;
  if (pos > ident) SetState(-1);
  return SelChanged(m_pWndControl->m_AktIndex);
}

//-------------------------------------------------------------
int CTVDxfLayerItem::SetState(int iState)
{
  if (iState==0)  {
    m_State=0;
  } else if (iState==1)  {
    m_State=1;
  } else if (iState==-1)  {
    m_State= GetImage()==imDxfFile ?2:0 ;//  (m_State &1);
  }

  if (m_NumChild  && m_pChild) {
    CTVItem* pTVItem1 = m_pChild;
    while(pTVItem1) {
      pTVItem1->SetState(m_State);
      pTVItem1 = pTVItem1->m_pNext;
    }
  }
  LayerSwitchModify++;
  AdjustLayerSwitches();

  return 1;
}

//-------------------------------------------------------------
CTVDxfLayerItem::CTVDxfLayerItem (CTreeView * pWndControl,CTVItem * pParent, DWORD record):CTVItem(pWndControl,pParent,record)
{
  m_State=1;
  m_lParam=NULL;
}

//-------------------------------------------------------------
int CTVDxfLayerItem::OnClick(int DoubleClick,int pos)
{
  SetState(-1);
  return SelChanged(m_pWndControl->m_AktIndex);
}

//-------------------------------------------------------------
void gShowLayerButtons(int neu)
{
  int i=0;

  if (gLayerTree) {

    if (neu==1) {
      CLayers * pLayer= gpLayers;
      int index=0;
      int i=0;
      if (gLayerTree->m_pack) {
        gLayerTree->Reset();
        gLayerTree->m_pack->begin();

        char   szParent[256]= {0};
        char   szFileName[256];
        const CObjectManager * curMng=NULL;
        CTVItem * DxfSourceTVItem=NULL;
        CTVItem * parentTVItem=NULL;
        CTVItem * pTVItem1=NULL;
        while (pLayer) {

          if (pLayer->m_ObjSource && curMng!= pLayer->m_ObjSource) {
            curMng = (CObjectManager *)pLayer->m_ObjSource;
            strcpy (szFileName,curMng->m_FilePfadName);
            char *pdxfFileName = strrchr(szFileName, '\\');
            if (pdxfFileName) {
              pdxfFileName++;
            } else {
              pdxfFileName = strrchr(szFileName, '/');
              if (pdxfFileName) {
                pdxfFileName++;
              }
            }
            if (pdxfFileName) {
              char *ext2 = strrchr(pdxfFileName, '.');
              if (ext2) {
                * ext2 = '\0';
                strcpy(szFileName,pdxfFileName);
              }
              DxfSourceTVItem=  (CTVItem*) new CTVDxfLayerItem(gLayerTree,NULL,0);
              if (DxfSourceTVItem) {
                DxfSourceTVItem->SetImage(imDxfFileActive);
                DxfSourceTVItem->m_lParam  = NULL;
                DxfSourceTVItem->SetText(pdxfFileName);
                DxfSourceTVItem->m_SortKey = 0xFFFFFF00;
                DxfSourceTVItem->m_NumChild = INSERTCHILDREN;
                gLayerTree->InsertRootItem((CTVItem*)DxfSourceTVItem);
                index++;

              }
            }
            parentTVItem = NULL;
            szParent[0] = 0;
          }
          char szLayerName[256];
          strncpy(szLayerName,pLayer->m_Name,255);

          char * subitem = strchr(szLayerName,'.');
          if (subitem==NULL ) {
            subitem = strchr(szLayerName,'|');
          }
          if (subitem) {
            *subitem = '\0';
            subitem++;
            if (szParent[0] && strcmp(szLayerName,szParent)!=0) {
              if (parentTVItem) parentTVItem->Collapse();
              parentTVItem = NULL;
            }
            if (parentTVItem==NULL) {
              strcpy(szParent,szLayerName);
              parentTVItem=  (CTVItem*) new CTVParentLayerItem(gLayerTree,DxfSourceTVItem,0);
              if (parentTVItem) {
                parentTVItem->SetImage(imLayersPart);
                parentTVItem->m_lParam  = pLayer;
                parentTVItem->SetText(szLayerName);
                parentTVItem->m_SortKey = 0xFFFFFF00;;
                DxfSourceTVItem->m_NumChild = INSERTCHILDREN;

                if (DxfSourceTVItem==0) gLayerTree->InsertChildItem((CTVItem*)parentTVItem,INSERT_END);
                else DxfSourceTVItem ->InsertChildItem(parentTVItem,INSERT_END);
                index++;
              }
            }
          } else {
            subitem = szLayerName;
            parentTVItem = NULL;
          }
          if (pLayer->m_NumObjects) {
            if (parentTVItem ) {
              pTVItem1=  (CTVItem*) new CTVLayerItem(gLayerTree,parentTVItem,0);
            } else {
              pTVItem1=  (CTVItem*) new CTVLayerItem(gLayerTree,DxfSourceTVItem,0);
            }
            if (pTVItem1) {
              pTVItem1->m_NumChild = INSERTCHILDREN;
              pTVItem1->SetImage((pLayer->m_LayerSwitch)?imLayerActive:imLayer);
              pTVItem1->m_lParam  = pLayer;
              pTVItem1->SetState(pLayer->m_LayerSwitch);
              pTVItem1->SetText(subitem);
              pTVItem1->m_SortKey = 0xFFFFFF00;;
              pTVItem1->m_NumChild = 0;
              index++;

              if (parentTVItem)  {
                parentTVItem->InsertChildItem(pTVItem1,INSERT_END);
              } else {
                DxfSourceTVItem ->InsertChildItem(pTVItem1,INSERT_END);
              }
            }
          }
          pLayer = pLayer->m_NextLayer;
        }
        int xx= gLayerTree->x();
        int xy= gLayerTree->y();
        extern int sMaxWidth;
        int xw= sMaxWidth;//gLayerTree->w()+100;
        int xh= gLayerTree->h();
        gLayerTree->m_pack ->end();
        gLayerTree->m_pack->resize(xx,xy,xw,xh);
        //m_pack->children();
      }
      gLayerTree->redraw();
      LayerSwitchModify=0;
    } else {
      int numItems = gLayerTree->m_pack->children();
      for (i=0; i< numItems; i++) {
        CTVLayerItem * pTVItem1 = (CTVLayerItem *)gLayerTree->m_pack->child(i);
        if (pTVItem1) {
          CLayers * pLayer = (CLayers *)pTVItem1->m_lParam;
          if (pLayer) {
            if (neu<0)pLayer->m_LayerSwitch = pLayer->m_DefaultSwitch;
            else pLayer->m_LayerSwitch=neu<2?0:1;
            if (pTVItem1->GetImage()==imLayer||pTVItem1->GetImage()==imLayerActive) {
              pTVItem1->SetImage((pLayer->m_LayerSwitch)?imLayerActive:imLayer);
            }
          }
        }
      }
      AdjustLayerSwitches();
      //gLayerTree->redraw();
    }
  }
}

#endif

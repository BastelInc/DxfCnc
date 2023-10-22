//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "FltkDialogBox.h"
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Roller.H>
#include <FL/math.h>


//-------------------------------------------------------------
CBastelButton::CBastelButton (int X, int Y, int W, int H, const char *L=0):Fl_Button(X,Y,W,H,L)
{
  ;
}
//-------------------------------------------------------------
CBastelButton::~CBastelButton()
{
  ;
}
//-------------------------------------------------------------
CFltkDialogBox::CFltkDialogBox(int X,int Y,int W,int H,const char *L):Fl_Window(X,Y,W,H,L)
{
  m_NumDialogCtlItem=0;
  m_pDialogCtlItem  = NULL;
  callback(MainProc,0);
}
//-------------------------------------------------------------
CFltkDialogBox::~CFltkDialogBox()
{

}
//-------------------------------------------------------------
void  CFltkDialogBox::MainProc(Fl_Widget* item, void* Value)
{
  long x = (long)Value;
  ((CFltkDialogBox*) item)->WindowProc(WM_COMMAND,x,0);
}
//-------------------------------------------------------------

//#define FONTFACE    FL_HELVETICA//  FL_COURIER
//#define FONTSIZE    16

int CFltkDialogBox::InitWindow(Fl_Widget* pParent, int ID, int left, int top, int width, int height, int Style)
{
  //DWORD dunit = GetDialogBaseUnits();
//   extern int glabelsize;
//  fl_font(FONTFACE,FONTSIZE);
#ifdef MINISCREEN
  int ySpace  = 15;
  int xSpace  = 8;
#else
  int ySpace  = fl_height();
  int xSpace  = fl_width("A");
#endif
  int maxx=0;
  int maxy=0;
  if (ySpace  < 3 || ySpace  >40 || xSpace  < 3 || xSpace  > 30) {
    ySpace  = 16;//16;
    xSpace  = 6;//8;
  }
#if FLTK_ABI_VERSION < 10304
  xSpace  -= 1;
#endif

// rolfs workaround
#ifndef WIN32
// xSpace = xSpace * 4 / 3;
// ySpace = ySpace * 8 / 6;
#else
//  xSpace = xSpace * 3 / 4;
//  ySpace = ySpace * 6 / 8;
#endif
  if (ID==m_pDialogCtlItem->idc) {
    //left  =   left  * xSpace  / 4;
    //top   =   top   * ySpace  / 8;
    width =   width * xSpace  / 4;
    height=   height* ySpace  / 8;
  }
  Fl_Group * pGroup = NULL;
  int num = 0;
  tDialogCtlItem * pCtlItem = m_pDialogCtlItem ;
  pCtlItem = m_pDialogCtlItem ;
  num      = m_NumDialogCtlItem -1;

  pCtlItem ++;    // skip the dialog himselve

  while (num > 0 ) {
    num --;

    int X = pCtlItem->left  = (pCtlItem->left  * xSpace  / 4);// + x();
    int Y = pCtlItem->top   = (pCtlItem->top   * ySpace  / 8);// + y();
    int W = pCtlItem->width = pCtlItem->width * xSpace  / 4;
    int H = pCtlItem->height= pCtlItem->height* ySpace  / 8;

    if ((X+W) > maxx) maxx=(X+W);
    if ((Y+H) > maxy) maxy=(Y+H);
    if ((pCtlItem->style & WS_GROUP)==0)  {
      if (pGroup) pGroup->end();
      pGroup=NULL;
    }
    switch(pCtlItem->type) {
    case eButton:
      pCtlItem->pWndObject = new Fl_Button(X,Y,W,H,pCtlItem->text);
      break;
    case eOptions:
      if (pCtlItem->style & BS_AUTOCHECKBOX)
        pCtlItem->pWndObject = new Fl_Light_Button(X,Y,W,H,pCtlItem->text);
      else
        pCtlItem->pWndObject = new Fl_Toggle_Button(X,Y,W,H,pCtlItem->text);
      break;
//        pCtlItem->pWndObject = new Fl_Radio_Button(X,Y,W,H,pCtlItem->text);
    case eEditor:
      pCtlItem->pWndObject = new Fl_Input (X,Y,W,H,pCtlItem->text);
      if (pCtlItem->pWndObject) {
//          if (pCtlItem->text[0]!= '\0')
//            pCtlItem->pWndObject->SetText(pCtlItem->text);
//          //pCtlItem->style |= WS_BORDER|WS_TABSTOP;
//          pCtlItem->style |= WS_TABSTOP;
      }

      break;
    case eInputNum: {
      pCtlItem->pWndObject = new CNumericInput(X,Y,W,H,pCtlItem->text);
      pCtlItem->type = eInputNum;
      if (pCtlItem->pWndObject) {
        //pCtlItem->pWndObject->tooltip("Standard Counter");
        //o->labelsize(8);
        //o->callback((Fl_Callback*)callback);
      }
    }
    break;
    case eCounter: {
      pCtlItem->pWndObject = new  Fl_Counter(X,Y,W,H,pCtlItem->text);
      pCtlItem->type = eCounter;
      if (pCtlItem->pWndObject) {
        //pCtlItem->pWndObject->tooltip("Standard Counter");
        //o->labelsize(8);
        //o->callback((Fl_Callback*)callback);
      }
    }
    break;
    case eSliderCtl:

      if (pCtlItem->style & BS_AUTOCHECKBOX) {

        pCtlItem->pWndObject = new Fl_Value_Slider(X,Y,W,H,pCtlItem->text);
      } else {
        //pCtlItem->pWndObject = new Fl_Roller(X,Y,W,H,pCtlItem->text);
        //pCtlItem->type = eRoller;
        //((Fl_Roller*)pCtlItem)->step(0.15);
        //pCtlItem->(Fl_Roller*)->bounds();
        pCtlItem->pWndObject = new Fl_Slider(X,Y,W,H,pCtlItem->text);
      }
      if(pCtlItem->style & WS_HSCROLL) pCtlItem->pWndObject->type(FL_HOR_SLIDER);
      pCtlItem->pWndObject->align(Fl_Align(FL_ALIGN_BOTTOM));
      break;
//      case eCustomItem:
//        pCtlItem->pWndObject = NewCustomItem(pCtlItem->idc);
//        break;
    case eList:
      pCtlItem->pWndObject = new Fl_Browser(X,Y,W,H,pCtlItem->text);
      break;
    case eControls:
      if (pCtlItem->style & WS_MAXIMIZE) {
        pCtlItem->pWndObject = new CBastelButton(X,Y,W,H,pCtlItem->text);
        break;
      }
      if (pCtlItem->style & WS_GROUP) {
        if (pGroup==NULL) {
          pGroup = new Fl_Group(X,Y,W,H);
          pGroup ->box(FL_THIN_UP_FRAME);
          pCtlItem->pWndObject = pGroup;
        }
      } else {
        pCtlItem->pWndObject = new Fl_Box(X,Y,W,H,pCtlItem->text);
      }

      if (pCtlItem->style & WS_BORDER) {
        pCtlItem->pWndObject->box(FL_DOWN_FRAME);
      }
      if (pCtlItem->style & WS_DLGFRAME) {
        pCtlItem->pWndObject->box(FL_UP_BOX);
      }
      if (pCtlItem->style & SS_ETCHEDFRAME) {
        pCtlItem->pWndObject->box(FL_DOWN_FRAME);
      }
      break;
    case eText:
      pCtlItem->pWndObject = new Fl_Box(X,Y,W,H,pCtlItem->text);
      break;
    default:
      pCtlItem->pWndObject = NULL;
      break;
    }
    if (pCtlItem->pWndObject) {
      if(pCtlItem->style & WS_DISABLED) pCtlItem->pWndObject->deactivate();
      //if (pCtlItem->style & WS_BORDER) pCtlItem->pWndObject->box(FL_FLAT_BOX);// BORDER_BOX);
      //if (pCtlItem->style & WS_BORDER) pCtlItem->pWndObject->box(FL_DOWN_BOX);

      pCtlItem->pWndObject->align(Fl_Align(FL_ALIGN_CLIP)|FL_ALIGN_INSIDE|FL_ALIGN_CENTER| FL_ALIGN_WRAP);//FL_ALIGN_LEFT);
      //pCtlItem->pWndObject->labelfont (  fl_font());
      //pCtlItem->pWndObject->labelsize (fl_size());
      if (pGroup) {
        pCtlItem->pWndObject->callback((Fl_Callback*)cbGroupProc,(void*)(long)pCtlItem->idc);
      } else {
        pCtlItem->pWndObject->callback((Fl_Callback*)cbUniversal,(void*)(long)pCtlItem->idc);
      }
    }
    //else pCtlItem->style |= WS_VISIBLE;
    pCtlItem ++;
  }
  return maxx | (maxy<<16);
}
//-------------------------------------------------------------
void  CFltkDialogBox::cbUniversal(Fl_Widget* item, void* idc)
{
  long x = (long)idc;
  ((CFltkDialogBox*)item->parent())->WindowProc(WM_COMMAND,x,(LPARAM)item);
}
//-------------------------------------------------------------
void  CFltkDialogBox::cbGroupProc(Fl_Widget* item, void* idc)
{
  long x = (long)idc;
  item = item->parent();
  if (item!=0)  ((CFltkDialogBox*)item->parent())->WindowProc(WM_COMMAND,x,(LPARAM)item);
}
//-------------------------------------------------------------
void CFltkDialogBox::draw()
{
  Fl_Window::draw();
}
//-------------------------------------------------------------
int CFltkDialogBox::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return 0;
}
//-------------------------------------------------------------
Fl_Widget* CFltkDialogBox::NewCustomItem(int IDC)
{
  return 0;
}
//-------------------------------------------------------------
Fl_Widget* CFltkDialogBox::GetDlgItem(int nIDDlgItem)
{

  int i;
  for (i=0; i < children(); i++) {
    Fl_Widget*  res = child(i);
    if (res) {
      void  * p = res->user_data();
      long x = (long)p;
      if (x == nIDDlgItem) return res;
      // if (*(int*)res->user_data()== nIDDlgItem) return res;
    }
  }
  for (i=1; i < m_NumDialogCtlItem ; i++) {
    if (m_pDialogCtlItem[i].idc == nIDDlgItem) return m_pDialogCtlItem[i].pWndObject;
  }
  return NULL;
}
//-------------------------------------------------------------
tDialogCtlItem* CFltkDialogBox::pGetDlgItem(int nIDDlgItem)
{
  for (int i=1; i < m_NumDialogCtlItem ; i++) {
    if (m_pDialogCtlItem[i].idc == nIDDlgItem) return &m_pDialogCtlItem[i];
  }
  return NULL;
}
//-------------------------------------------------------------
BOOL CFltkDialogBox::CheckDlgButton(int nIDButton, UINT uCheck)
{
  //CControls *  pItem =  (CControls * ) GetDlgItem(nIDButton);
  tDialogCtlItem * pItem = pGetDlgItem(nIDButton);
  if (pItem ) {
    switch(pItem->type) {
    case eButton:
    case eOptions:
      ((Fl_Toggle_Button*)pItem->pWndObject)->value(uCheck!=0);
      pItem->pWndObject->redraw();
      return 1;
    case eEditor:
    case eSliderCtl:
    case eList:
    case eControls:
    case eText:
      break;
    }
  }
  return 0;

}
//-------------------------------------------------------------
BOOL CFltkDialogBox::IsDlgButtonChecked(int nIDButton)
{
  //CControls *  pItem =  (CControls * ) GetDlgItem(nIDButton);
  tDialogCtlItem * pItem = pGetDlgItem(nIDButton);
  if (pItem ) {
    switch(pItem->type) {
    case eButton:
    case eOptions:
      return ((Fl_Toggle_Button*)pItem->pWndObject)->value();
    case eEditor:
    case eSliderCtl:
    case eList:
    case eControls:
    case eText:
      break;
    }
  }
  return 0;

}
//-------------------------------------------------------------
BOOL CFltkDialogBox::CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
{
  for (int idc=nIDFirstButton; idc <= nIDLastButton; idc++) {
    CheckDlgButton(idc,(idc ==  nIDCheckButton)?1:0);
  }
  return 1;

}
//-------------------------------------------------------------
int CFltkDialogBox::SendDlgItemMessage(int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return 0;
}
//-------------------------------------------------------------
int CFltkDialogBox::GetDlgItemInt(int nIDDlgItem, BOOL* lpTranslated, BOOL bSigned)
{
  char str[30];
  if (lpTranslated ) * lpTranslated = 0;
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem && pItem->pWndObject) {
    switch(pItem->type) {
    case eEditor: {
      const char * pnt = ((Fl_Input *)pItem->pWndObject)->value();
      if (pnt) {
        strncpy(str,pnt,sizeof(str));
        if (lpTranslated ) * lpTranslated = 1;
        return atoi(str);
      }
      return 0;
    }
    case eText:
    case eList:
    case eButton:
    case eOptions:
    case eControls: {
      const char * pnt  = pItem->pWndObject->label();
      if (pnt) {
        strncpy(str,pnt,sizeof(str));
        if (lpTranslated ) * lpTranslated = 1;
        return atoi(str);
      }
      return 0;
    }
    break;
    case eInputNum:
//      return  ((CNumericInput*)pItem->pWndObject)->value();
    case eSliderCtl:
    case eCounter:
    case eRoller:
      return  ((Fl_Valuator*)pItem->pWndObject)->value();
      break;
    }
  }
  return 0;
}
//-------------------------------------------------------------
float CFltkDialogBox::GetDlgItemFloat(int nIDDlgItem)
{
  char str[30];
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem && pItem->pWndObject) {
    switch(pItem->type) {
    case eInputNum:
    case eSliderCtl:
    case eCounter:
    case eRoller:
      return  ((Fl_Valuator*)pItem->pWndObject)->value();
      break;
    }
  }
  return 0;
}

//-------------------------------------------------------------
BOOL CFltkDialogBox::SetDlgItemFloat(int nIDDlgItem, float Value)
{
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem && pItem->pWndObject) {
    switch(pItem->type) {
    case eRoller:
    case eSliderCtl:
    case eInputNum:
      ((Fl_Valuator*)pItem->pWndObject)->value((double)Value);
      pItem->pWndObject->redraw();
      return 1;
      break;
    }
  }
  return 0;
}
//-------------------------------------------------------------
BOOL CFltkDialogBox::EnableDlgItem(int nIDDlgItem, BOOL bEnable)
{
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem ) {
    switch(pItem->type) {
    case eButton:
    case eOptions:
    case eEditor:
    case eSliderCtl:
    case eList:
    case eControls:
    case eText:
    case eInputNum:
      if (bEnable) pItem->pWndObject->activate();
      else pItem->pWndObject->deactivate();
      pItem->pWndObject->redraw();
      return 1;
      break;
    }

  }
  return 0;
}
//-------------------------------------------------------------
BOOL CFltkDialogBox::SetDlgItemInt(int nIDDlgItem, int uValue, BOOL bSigned)
{
  char str[30];
  sprintf(str,"%d",uValue);
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem && pItem->pWndObject) {
    switch(pItem->type) {
    case eEditor: {
      ((Fl_Input *)pItem->pWndObject)->value(str);
      pItem->pWndObject->redraw();
      return 1;
    }
    case eText:
    case eList:
    case eButton:
    case eOptions:
    case eControls: {
      pItem->pWndObject->copy_label(str);
      pItem->pWndObject->redraw();
      return 1;
    }
    break;
    case eRoller:
    case eSliderCtl:
    case eInputNum:
      ((Fl_Valuator*)pItem->pWndObject)->value((double)uValue);
      pItem->pWndObject->redraw();
      return 1;
      break;
    }
  }
  return 0;
}
//-------------------------------------------------------------
BOOL CFltkDialogBox::SetDlgItemText(int nIDDlgItem, LPCSTR lpString)
{
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem )    switch(pItem->type) {
    case eEditor: {
      if (pItem->pWndObject) {
        ((Fl_Input *)pItem->pWndObject)->value(lpString);
        pItem->pWndObject->redraw();
        return 1;
      }
    }
    case eText:
    case eList:
    case eButton:
    case eOptions:
    case eControls: {
      if (pItem->pWndObject) {
        pItem->pWndObject->copy_label(lpString);
        pItem->pWndObject->redraw();
        return 1;
      }
    }
    break;
    case eInputNum: {
      if (pItem->pWndObject) {
        double val = atof(lpString);
        ((CNumericInput*)pItem->pWndObject)->value(val);
      }
    }
    break;
    case eSliderCtl:
    case eCounter:
    case eRoller: {
      if (pItem->pWndObject) {
        double val = atof(lpString);
        ((Fl_Valuator*)pItem->pWndObject)->value(val);
      }
    }
    break;
    }
  return 0;


}
//-------------------------------------------------------------
int CFltkDialogBox::GetDlgItemText(int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem )    switch(pItem->type) {
    case eEditor: {
      if (pItem->pWndObject) {
        const char * pnt = ((Fl_Input *)pItem->pWndObject)->value();
        if (pnt) {
          strncpy(lpString,pnt,nMaxCount);
          return strlen(lpString);
        }
        return 0;
      }
    }
    case eInputNum:
    /*    {
          if (pItem->pWndObject)
          {
            double val = ((CNumericInput*)pItem->pWndObject)->value();
            sprintf(lpString,"%0.2f",val);
            return strlen(lpString);
          }
        }
        break;*/
    case eCounter: {
      if (pItem->pWndObject) {
        double val = ((Fl_Valuator*)pItem->pWndObject)->value();
        //const char * pnt = ((Fl_Valuator*)pItem->pWndObject)->value();
        //if (pnt)
        {
          sprintf(lpString,"%0.2f",val);
          //strncpy(lpString,pnt,nMaxCount);
          return strlen(lpString);
        }
        return 0;
      }
    }
    break;
    case eText:
    case eSliderCtl:
    case eRoller:
    case eList:
    case eButton:
    case eOptions:
    case eControls: {
      if (pItem->pWndObject) {
        const char * pnt  = pItem->pWndObject->label();
        if (pnt) {
          strncpy(lpString,pnt,nMaxCount);
          return strlen(lpString);
        }
        return 0;
      }
    }
    break;
    }
  return 0;
}
//-------------------------------------------------------------
BOOL CFltkDialogBox::ShowDlgItem(int nIDDlgItem, int bShow)
{
  tDialogCtlItem * pItem = pGetDlgItem(nIDDlgItem);
  if (pItem ) {
    switch(pItem->type) {
    case eButton:
    case eOptions:
    case eEditor:
    case eSliderCtl:
    case eRoller:
    case eList:
    case eControls:
    case eText:
      if (bShow) pItem->pWndObject->show();
      else       pItem->pWndObject->hide();
      pItem->pWndObject->redraw();
      return 1;
      break;
    }

  }
  return 0;

}
//-------------------------------------------------------------
BOOL CFltkDialogBox::SetButtonImage(int nIDDlgItem, int iImage)
{
  return 0;
}
//-------------------------------------------------------------


#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include <FL/fl_draw.H>
// https://www.seriss.com/people/erco/fltk/
// Demonstrate how to use Fl_Input in a touchscreen application -erco 08/25/06
void Button_Close(Fl_Widget *w, void *data);
class MyNumPad : public Fl_Window
{
  Fl_Input *in;               // input preview
  Fl_Callback *enter_cb;      // callback when user hits 'enter'
  void *enter_data;

  // Handle numeric keypad buttons pressed
  void Button_CB2(Fl_Widget *w)
  {
    Fl_Button *b = (Fl_Button*)w;
    if ( strcmp(b->label(),"Del") == 0 ) {                  // handle Delete
      // Delete
#if 1 //def WIN32
      if (in->mark() != in->position()) in->cut();
      else in->cut(-1);
#else
      if (in->mark() != in->insert_position()) in->cut();
      else in->cut(-1);
#endif
    } else if ( strcmp(b->label(), "Enter") == 0 ) {          // handle enter key
      // Do 'Enter' callback
      if ( enter_cb ) (*enter_cb)(in, enter_data);
    } else {                                                // handle all other keys
      // Appends label of button
#if 1  //def WIN32
      in->replace(in->position(), in->mark(), b->label(), 1);
#else
      in->replace(in->insert_position(), in->mark(), b->label(), 1);
#endif
    }
  }
  static void Button_CB(Fl_Widget *w, void *data)
  {
    MyNumPad *numpad = (MyNumPad*)data;
    numpad->Button_CB2(w);
  }
public:
  MyNumPad(int X,int Y,int W=190,int H=230,const char *L=0):Fl_Window(X,Y,W,H,L)
  {
    X = 0;Y=0;
    const int bsize = 40;
    // Preview input
    in = new Fl_Input(X+10,Y+10,W-20,30,L);
    in->textsize(20);
    // Numeric keypad
    Fl_Button *b;
    int colstart = 10,
        col = colstart,
        row = in->y()+in->h()+10;
    b = new Fl_Button(col,row,bsize,bsize,  "7");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "8");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "9");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "Del");
    b->callback(Button_CB, (void*)this);
    b->labelsize(10);
    col=colstart;
    row+=b->h();
    b = new Fl_Button(col,row,bsize,bsize,  "4");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "5");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "6");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "-");
    b->callback(Button_CB, (void*)this);
    col=colstart;
    row+=b->h();
    b = new Fl_Button(col,row,bsize,bsize,  "1");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "2");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "3");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "+");
    b->callback(Button_CB, (void*)this);
    col=colstart;
    row+=b->h();
    b = new Fl_Button(col,row,bsize,bsize,  ".");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize,bsize,  "0");
    b->callback(Button_CB, (void*)this);
    col+=b->w();
    b = new Fl_Button(col,row,bsize*2,bsize,"Enter");
    b->callback(Button_CB, (void*)this);
    //b->callback(Button_Close, (void*)this);
    col+=b->w();
    b->color(10);
    end();
    enter_cb = 0;
    enter_data = 0;
  }

  void SetValue(double v)
  {
    char str[80];
    sprintf(str,"%f",v);
    int i;
    for (i=(int) strlen(str)-1; i>0; i--) {
      if (str[i]!='0') break;
    }
    if (str[i]!='.') i++;
    str[i] = '\0';

    in->value(str);

  };

  // Return current value
  const char *value()
  {
    return(in->value());
  }
  // Clear current value
  void clear()
  {
    in->value("");
  }
  // Set callback for when Enter is pressed
  void SetEnterCallback(Fl_Callback *cb, void *data)
  {
    enter_cb = cb;
    enter_data = data;
  }
};

static MyNumPad * MyNumPadDialog=NULL;
void Button_Close(Fl_Widget *w, void *data)
{
  {
    if (MyNumPadDialog)delete MyNumPadDialog;
    MyNumPadDialog=NULL;
  }


}





//  if ( ! BinderInfoDialog ) BinderInfoDialog = new MyNumPad(0,0);
//  //BinderInfoDialog->SetEnterCallback(SetNumPadValue_CB, (void*)this);
//  BinderInfoDialog->position(m_hWnd->x()-100,m_hWnd->y()+20);
//  BinderInfoDialog->set_non_modal();
//  BinderInfoDialog->clear();
//  BinderInfoDialog->show();

//    MyNumPad *numpad;                   // local instance of numeric keypad widget

// Called when user finishes entering data with numeric keypad
void CNumericInput::SetNumPadValue_CB2()
{
  double val = atof(((MyNumPad *)numpad)->value());
  value(val);         // pass value from numpad to our input
  ((MyNumPad *)numpad)->hide();                 // hide numpad
}

void CNumericInput::SetNumPadValue_CB(Fl_Widget*th,void *data)
{
  CNumericInput *in = (CNumericInput*)data;
  in->SetNumPadValue_CB2();
  in->do_callback();
}




void CNumericInput::draw()
{
  int i;
  Fl_Boxtype boxtype[5];
  Fl_Color selcolor;

  boxtype[0] = box();
  if (boxtype[0] == FL_UP_BOX) boxtype[0] = FL_DOWN_BOX;
  if (boxtype[0] == FL_THIN_UP_BOX) boxtype[0] = FL_THIN_DOWN_BOX;
  for (i=1; i<5; i++)
    if (mouseobj == i)
      boxtype[i] = fl_down(box());
    else
      boxtype[i] = box();

  int xx[5], ww[5];
  int W = w()*15/100;
  xx[1] = x();
  ww[1] = W;
  xx[2] = x()+1*W;
  ww[2] = W;
  xx[0] = x()+2*W;
  ww[0] = w()-4*W;
  xx[3] = x()+w()-2*W;
  ww[3] = W;
  xx[4] = x()+w()-1*W;
  ww[4] = W;

  draw_box(boxtype[0], xx[0], y(), ww[0], h(), FL_BACKGROUND2_COLOR);
  fl_font(textfont(), textsize());
  fl_color(active_r() ? textcolor() : fl_inactive(textcolor()));
  char str[128];
  format(str);

  double v = Fl_Valuator::value();

  double lstep_ = step()*10.0;
  int c =2;
  if (lstep_>=10.0) c=0;
  else if (lstep_>=1.0) c=1;
#ifdef WIN32
  _snprintf(str, 128, "%.*f", c, v);
#else
  snprintf(str, 128, "%.*f", c, v);
#endif

  if (rint(ProposeVal*lstep_)  != rint(v*lstep_)) {
    fl_draw(str, xx[0], y(), ww[0], h()/2, FL_ALIGN_CENTER);
    //fl_color(FL_YELLOW);
    fl_color(FL_GREEN);
#ifdef WIN32
    _snprintf(str, 128, "%.*f", c, ProposeVal );
#else
    snprintf(str, 128, "%.*f", c, ProposeVal );
#endif
    fl_draw(str, xx[0], y()+h()/2, ww[0], h()/2, FL_ALIGN_CENTER);
  } else {
    fl_draw(str, xx[0], y(), ww[0], h(), FL_ALIGN_CENTER);
  }
  if (Fl::focus() == this) draw_focus(boxtype[0], xx[0], y(), ww[0], h());
  if (!(damage()&FL_DAMAGE_ALL)) return; // only need to redraw text

  if (active_r())
    selcolor = labelcolor();
  else
    selcolor = fl_inactive(labelcolor());

  draw_box(boxtype[1], xx[1], y(), ww[1], h(), color());
  fl_draw_symbol("@-4<<", xx[1], y(), ww[1], h(), selcolor);
  draw_box(boxtype[2], xx[2], y(), ww[2], h(), color());
  fl_draw_symbol("@-4<",  xx[2], y(), ww[2], h(), selcolor);
  draw_box(boxtype[3], xx[3], y(), ww[3], h(), color());
  fl_draw_symbol("@-4>",  xx[3], y(), ww[3], h(), selcolor);
  draw_box(boxtype[4], xx[4], y(), ww[4], h(), color());
  fl_draw_symbol("@-4>>", xx[4], y(), ww[4], h(), selcolor);
}


void CNumericInput::increment_cb()
{
  if (!mouseobj) return;
  double v = Fl_Valuator::value();
  double lstep_ = step()*10.0;
  switch (mouseobj) {
  case 1:
    v -= lstep_;
    break;
//  case 2: v = increment(v, -1); break;
//  case 3: v = increment(v, 1); break;
  case 2:
    v -= lstep_/10.0;
    break;
  case 3:
    v += lstep_/10.0;
    break;
  case 4:
    v += lstep_;
    break;
  }
  handle_drag(clamp(round(v)));
  ProposeVal=v;
}

#define INITIALREPEAT .5
#define REPEAT .1

void CNumericInput::repeat_callback(void* v)
{
  CNumericInput* b = (CNumericInput*)v;
  if (b->mouseobj) {
    Fl::add_timeout(REPEAT, repeat_callback, b);
    b->increment_cb();
  }
}

int CNumericInput::calc_mouseobj()
{
  int W = w()*15/100;
  if (Fl::event_inside(x(), y(), W, h())) return 1;
  if (Fl::event_inside(x()+W, y(), W, h())) return 2;
  if (Fl::event_inside(x()+w()-2*W, y(), W, h())) return 3;
  if (Fl::event_inside(x()+w()-W, y(), W, h())) return 4;
  else {
    if (rint(ProposeVal) != rint(Fl_Valuator::value())) {
      // Vorgabe übernehmen
      value(ProposeVal);
    } else {
      if ( ! MyNumPadDialog ) MyNumPadDialog = new MyNumPad(0,0);
      numpad = MyNumPadDialog;
      MyNumPadDialog->SetValue(Fl_Valuator::value());
      MyNumPadDialog->SetEnterCallback(SetNumPadValue_CB, (void*)this);
      int xpos = x();
      int ypos = y();
      if (ypos + MyNumPadDialog->h() > Fl::h()){
        ypos = Fl::h()-MyNumPadDialog->h()-50;
      }
      MyNumPadDialog->position(xpos,ypos);
      //MyNumPadDialog->position(x()-10,y()+20);
      MyNumPadDialog->set_modal();
      //MyNumPadDialog->clear();
      MyNumPadDialog->show();

    }
  }
  return -1;
}

int CNumericInput::handle(int event)
{
  int i;
  switch (event) {
  case FL_RELEASE:
    if (mouseobj) {
      Fl::remove_timeout(repeat_callback, this);
      mouseobj = 0;
      redraw();
    }
    handle_release();
    return 1;
  case FL_PUSH:
    if (Fl::visible_focus()) Fl::focus(this);
    {
      Fl_Widget_Tracker wp(this);
      handle_push();
      if (wp.deleted()) return 1;
    }
  case FL_DRAG:
    i = calc_mouseobj();
    if (i != mouseobj) {
      Fl::remove_timeout(repeat_callback, this);
      mouseobj = (uchar)i;
      if (i) Fl::add_timeout(INITIALREPEAT, repeat_callback, this);
      Fl_Widget_Tracker wp(this);
      increment_cb();
      if (wp.deleted()) return 1;
      redraw();
    }
    return 1;
  case FL_KEYBOARD :
    switch (Fl::event_key()) {
    case FL_Left:
      handle_drag(clamp(increment(Fl_Valuator::value(),-1)));
      return 1;
    case FL_Right:
      handle_drag(clamp(increment(Fl_Valuator::value(),1)));
      return 1;
    default:
      return 0;
    }
  // break not required because of switch...
  case FL_FOCUS : /* FALLTHROUGH */
  case FL_UNFOCUS :
    if (Fl::visible_focus()) {
      redraw();
      return 1;
    } else return 0;
  case FL_ENTER : /* FALLTHROUGH */
  case FL_LEAVE :
    return 1;
  default:
    return 0;
  }
}

int CNumericInput::value(double v)
{
  ProposeVal = clamp(round(v));
  return Fl_Valuator::value(v);
}

void CNumericInput::SetProposeVal(double d)
{
  ProposeVal = d;
  redraw();
}


CNumericInput::~CNumericInput()
{
  Fl::remove_timeout(repeat_callback, this);
}

CNumericInput::CNumericInput(int X, int Y, int W, int H, const char* L)
  : Fl_Valuator(X, Y, W, H, L)
{
  box(FL_UP_BOX);
  selection_color(FL_INACTIVE_COLOR); // was FL_BLUE
  align(FL_ALIGN_BOTTOM);
  bounds(-1000000.0, 1000000.0);
  Fl_Valuator::step(1, 10);
  mouseobj = 0;
  textfont_ = FL_HELVETICA;
  textsize_ = FL_NORMAL_SIZE;
  textcolor_ = FL_FOREGROUND_COLOR;
  ProposeVal=0;
}





//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

// Eintrag in .profile   :
// export MESA_GL_VERSION_OVERRIDE=3.3
// etc/groups :  tty freigeben
// Starten:
// MESA_GL_VERSION_OVERRIDE=3.3 ./dxfcnc

#if defined(WIN32)
# define GLEW_STATIC 1
#endif
#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <FL/math.h>
#include <math.h>
#include "Settings.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>

#undef MOUSESERVICE
//# d efine MOUSESERVICE

#include "GlDisplay.h"
#include "DXF3DDisplayUtils.h"
#include "Object.h"

#include "clipper.h"

#undef   ROLFDEBUG
#ifndef DREITABLE
//#include "Resource.h"
#include "Serialcom.h"
#include "DxfCnc.h"
#include "Tree.h"
#endif
//-------------------------------------------------------------
class CObject;
FL_EXPORT void glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
FL_EXPORT void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
extern void SaveStlPolygons(CObjectManager     * pObjectManager,int binary);


int     Pmodify=0;
//int     gLookAtIx=0;
static GLint viewport[4];
int    glDisplay_redraw;
int   bShowNormale=0;
int   gShowSymulation=3;
int   bTouchScreen=0;
int   bYUpp=0;
int   bArrangeZoomCenter=0;
int   gMausMode='m';
int   gMausModeStart;
float XOffset2    = -0.5;
float YOffset2    = -0.5;
float ZOffset2    = 0;
float glFaktor2   = 10;
float glMaximum   = 1000;
float glMaximumH  = 1000;
float glMaximumV  = 1000;
#ifndef DREITABLE
int   gbFillPolygon= 0;
#else
int   gbFillPolygon= 1;
#endif
#if defined(SERIALCOM) || !defined(USE_SCHADER)
GLint  ShowSeparate=1;
#else
GLint  ShowSeparate=0;
#endif
#ifdef MOUSESERVICE
int selectPiece(int mousex, int mousey);
int closest =0;
#define MAXSELECT 1024
GLuint selectBuf[MAXSELECT];
int NumHits =0;
void processHits2 (GLint hits, GLuint buffer[]) ;
void processHits(GLint hits, GLuint buffer[]);
#endif
int gObject_redraw=0;
//-------------------------------------------------------------

static RECT rcAuswahlScreen = {0,0,0,0};
static float rcAuswahl_gl[4] = {0,0,0,0};
RECT   rcAuswahlCNC;
static bool AuswahlMove = false;
static int  start_ex_, start_ey_;
static int  last_x_, last_y_;
static int  bHandle=1;
int   SBWIDTH2 = 15;
float Zoom = 1.0f;
float ZoomStart = 1.0f;
float centerStart[3]= {0,0,0};
float RotateValStart [3] = {-45,-13,-12};



float LightPos[3] = {5,3,10} ;
//float LightPos[3] = {-10,22,16} ;
float LightColor[3]= {.9,.7,1.};
float eye[3] = {0.1,0.1,2};
float center[3]= {0,0,0};
float RotateValSoll[3] = {10,33,2};
float RotateVal [3] = {10,33,2};
float Perspective=30.0;
float Deep=0.0;
float gBackground= 40;
int   gLookClip=0;



//-------------------------------------------------------------
DXF3DDisplay::~DXF3DDisplay()
{
#ifdef USE_SHADER
  if (shaderProgram)
    CleanupOpenGls();
  DeleteGaphicData();
#endif
}
//-------------------------------------------------------------
DXF3DDisplay::DXF3DDisplay(int X, int Y, int W, int H, const char *L):Fl_Gl_Window(X,Y,W,H,L)

#ifdef SCROLLBAR3D
  ,xscrollbar_(X, Y + H - SBWIDTH2, W - SBWIDTH2, SBWIDTH2),
  yscrollbar_(X + W - SBWIDTH2, Y, SBWIDTH2, H - SBWIDTH2)
#endif
{
  GlInit=0;
  //if (Fl_Gl_Window::can_do(FL_DOUBLE )) mode(FL_OPENGL3 | FL_ALPHA|FL_DEPTH	| FL_RGB | FL_DOUBLE|FL_MULTISAMPLE);
//  else                                  mode(FL_OPENGL3 | FL_ALPHA|FL_DEPTH	| FL_RGB|FL_MULTISAMPLE);
  mode(FL_OPENGL3 | FL_ALPHA|FL_DEPTH	| FL_RGB | FL_FAKE_SINGLE  |FL_MULTISAMPLE);

#ifndef DREITABLE
  Fl::e_keysym = 't';//IDC_ShowTop;
  Fl::handle_(FL_KEYDOWN, this);
#endif
#ifdef SCROLLBAR3D
  xscrollbar_.type(FL_HORIZONTAL);
  xscrollbar_.callback(scrollbar_cb, this);

  yscrollbar_.type(FL_VERTICAL);
  yscrollbar_.callback(scrollbar_cb, this);

  xscrollbar_.value(0,10,-200,400);
  yscrollbar_.value(0,10,-200,400);

  xscrollbar_.value(RotateVal[0]);
  yscrollbar_.value(RotateVal[1]);


  bShowScrollbar = 1;
  resize(X, Y, W-SBWIDTH2, H-SBWIDTH2);
#endif
#ifdef VIRTUALCNC
  InitializeCriticalSection(&SerialComm::s_critical_section);
#endif
#ifdef USE_SHADER
  shaderProgram = 0;
#endif
  Fl::add_timeout(1.0, GlTimer_CB, this);
}
//-------------------------------------------------------------
void UpdateMainWindow(void)
{
  gObject_redraw=1;
//  if (gWrk3DSheet) gWrk3DSheet->redraw();
}
//-------------------------------------------------------------
int OffsetRect_(LPRECT lprc,  int dx,   int dy  )
{
  lprc->left  += dx;
  lprc->right += dx;
  lprc->top   += dy;
  lprc->bottom+= dy;
  return 1;
}
//-------------------------------------------------------------
float DXF3DDisplay::getZoom(int Start)
{
  bHandle=!Start;
  return Zoom;
}
void  DXF3DDisplay::setZoom(float z)
{
  if (bTouchScreen) {
    Zoom = z;
    damage(FL_DAMAGE_ALL );
  }
}

//-------------------------------------------------------------
void  DXF3DDisplay::setRotation(int w)
{
  if (bTouchScreen) {
    RotateVal[2]=w;
//#ifndef DREITABLE
//    if (gPerspektive && gPerspektive ->visible()) gPerspektive->Show();
//#endif
    damage(FL_DAMAGE_ALL );
  }
}
float DXF3DDisplay::getRotation(int Start)
{
  return RotateVal[2];
}

void  DXF3DDisplay::setRotation2(int w)
{
  if (bTouchScreen) {
    RotateVal[1]=w;
//#ifndef DREITABLE
//    if (gPerspektive && gPerspektive ->visible()) gPerspektive->Show();
//#endif
    damage(FL_DAMAGE_ALL );
  }
}
float DXF3DDisplay::getRotation2(int Start)
{
  return RotateVal[1];
}

//-------------------------------------------------------------
int DXF3DDisplay::InitOpenGL()
{
#ifdef USE_SHADER
  if (shaderProgram) {
    CleanupOpenGls();
  }
#endif
  glViewport(0,0,pixel_w(),pixel_h());
  glEnable(GL_DEPTH_TEST);
// glDepthFunc(GL_LEQUAL);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );
  glShadeModel (GL_FLAT);
  glClearColor (0.0, 0.0, 0.0, 0.0);
  /*

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    */

#ifdef USE_SHADER
  InitShader();
#endif
  return GlInit=1;
}
//-------------------------------------------------------------
void DXF3DDisplay::hide()
{
  Fl_Gl_Window::hide();
}

//-------------------------------------------------------------
void DXF3DDisplay::scrollbar_cb(Fl_Widget *w, void *d)
{
#ifdef SCROLLBAR3D
  DXF3DDisplay	*object = (DXF3DDisplay *)d;
  RotateVal[0] = object->xscrollbar_.value()/1.0f;
  RotateVal[1] = object->yscrollbar_.value()/1.0f;
  object->damage(FL_DAMAGE_SCROLL);
#endif
}
//-------------------------------------------------------------
int DXF3DDisplay::handle(int event)
{
  static int first = 1;
  int key;
  switch (event) {
  case FL_SHOW:
    if (first && shown()) {
      first = 0;
      make_current();
#ifdef USE_SHADER
      GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
      if (err)
        Fl::warning("glewInit() failed returning %u", err);
      else
        fprintf(stderr,"Using GLEW %s\n", glewGetString(GLEW_VERSION));

      const uchar *glv = glGetString(GL_VERSION);
      fprintf(stderr,"GL_VERSION=%s\n", glv);
      sscanf((const char *)glv, "%d", &gl_version_major);
      if (gl_version_major < 3)
        fprintf(stderr,"\nThis platform does not support OpenGL V3\n\n");
#endif
    }
    break;
  case FL_SHORTCUT :
  case FL_KEYDOWN:
    key  = Fl::event_key();
    if (!(Fl::event_state() & (FL_CTRL | FL_ALT | FL_META))) {
      switch (key&FL_KEY_MASK) {
      case FL_BackSpace : // Previous (slideshow)
        if (!(Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META))) {
          //do_callback();
          Zoom = 1.0;
          center[0]   =  0;
          center[1]   =  0;
          center[2]   =  0;
          eye[0]      =  0;
          eye[1]      =  60;
          eye[2]      =  85;
          RotateValSoll[0]=  0;
          RotateValSoll[1]=  0;
          RotateValSoll[2]=  0;
          Perspective =  10;
          damage(FL_DAMAGE_SCROLL);
          return (1);
        }
        break;
      case FL_F+5:
        //case 'u':
        //gObjectManager->time=1;
#ifdef USE_SHADER
        gObjectManager->Update3DreiTafelProjektion();
#endif
        break;
      case 252:
      case 0x3b:  // ü
      case 'p':
#ifdef USE_SHADER
        gbUsePolygonObject=!gbUsePolygonObject;
        gObjectManager->time=1;
#endif
        break;
      case 36:
      case 0xdf: //'$':
        extern int     gbShowTransformation;
        gbShowTransformation=!gbShowTransformation;
        gObjectManager->time=1;
        break;

      case 's':
        SaveStlObjects(0);
        return 1;
      case 'i' : {
        if (gLookAtIx >=0 && gLookAtIx  <4) {
          LookAtSpeichern(gLookAtIx);
          //gPerspektive->EnableDlgItem(IDC_SetDefault,0);
        }
      }
      break;

      case FL_Insert:
      case FL_Home:
      case FL_Left:
      case FL_Up:
      case FL_Right:
      case FL_Down:
      case FL_Page_Up:
      case FL_Page_Down:
      case FL_End:
      case 'a':
      case 'z':
      case 'x':
      case '<':
      case 226:
      case 'c':
      case 'y':
      case 'e':
      case 'n':
      case 'f':
      case 'g':
      case '/'|FL_KP:
      //    case 0x6F:  //  /
      case '*'|FL_KP:
      case 0x6A:  //  *
      case '1'|FL_KP:
      case '2'|FL_KP:
      case '3'|FL_KP:
      case '4'|FL_KP:
      case '5'|FL_KP:
      case '6'|FL_KP:
      case '7'|FL_KP:
      case '8'|FL_KP:
      case '9'|FL_KP:
      case '0'|FL_KP:
      case '.'|FL_KP:

        switch (key) {
        case '/'|FL_KP:
        case 0x6F:  //  /
          LightColor[0] -=  0.02;
          LightColor[1] -=  0.02;
          LightColor[2] -=  0.02;
          break;
        case '*'|FL_KP:
        case 0x6A:  //  *
          LightColor[0] +=  0.02;
          LightColor[1] +=  0.02;
          LightColor[2] +=  0.02;
          break;
        case 'y':
          bYUpp = (bYUpp +1) %3;

          gObjectManager->time=1;
          break;
        case 'z':
          //case IDM_ZoomGanz:
          bArrangeZoomCenter= !bArrangeZoomCenter;
          Zoom = 1.0;
          center[0]   =  0;
          center[1]   =  0;
          center[2]   =  0;
          break;
        case 'x': {
          Zoom = 0.9;
          float XMax = (gObjectManager->MinMaxright -  gObjectManager->MinMaxleft);
          float Max  = (gObjectManager->MinMaxbottom - gObjectManager->MinMaxtop);
          float H = gWrk3DSheet->h();
          float W = gWrk3DSheet->w() ;
          if ((XMax / Max) > (W/ H)) {
            Max = XMax * (H/W);
          }
          float dpi;
          float vdpi;
          Fl::screen_dpi(dpi,vdpi,0);
          dpi = gBastelUtils.GetProfileInt("Display","DPIScreenRes",(int)dpi);
          Zoom = Max * (dpi / 2540*1.15) / (float)(H);;
        }
        break;
        case '<':
        case 226:
          ShowSeparate = !ShowSeparate;
          break;
        case 'c':
          gLookClip = !gLookClip;
          break;
        case 'n':
          bShowNormale = !bShowNormale;
          break;
        case 'f':
          gbFillPolygon = !gbFillPolygon;
          break;
        case FL_Insert:
          center[0]   =  0;
          center[1]   =  0;
          center[2]   =  0;
          Zoom = 1.0;
          Perspective =  10;
          break;
        case FL_Home:
          center[2]   +=  0.2;
          break;
          break;
        case FL_Left:
          if (Fl::event_state() & FL_SHIFT) {
            int wert = (float)((10.0 / (float)h()) / Zoom) / glFaktor;
            OffsetRect_(&rcAuswahlCNC,-wert,0);
          } else center[0]   -=  0.02;
          break;
        case FL_Up:
          if (Fl::event_state() & FL_SHIFT) {
            int wert = (float)((10.0 / (float)h()) / Zoom) / glFaktor;
            OffsetRect_(&rcAuswahlCNC,0,wert);
          } else center[1]   +=  0.02;
          break;
        case FL_Right:
          if (Fl::event_state() & FL_SHIFT) {
            int wert = (float)((10.0/ (float)h()) / Zoom) / glFaktor;
            OffsetRect_(&rcAuswahlCNC,wert,0);
          } else center[0]   +=  0.02;
          break;
        case FL_Down:
          if (Fl::event_state() & FL_SHIFT) {
            int wert = (float)((10.0 / (float)h()) / Zoom) / glFaktor;
            OffsetRect_(&rcAuswahlCNC,0,-wert);
          } else center[1]   -=  0.02;
          break;
        case FL_Page_Up:
          RotateVal[2] -=  1;
          break;
        case FL_Page_Down:
          RotateVal[2] +=  1;
          break;
        case FL_End:
          center[2]   -=  0.2;
          break;
//        case 'p':
//          if (Fl::event_state() & FL_SHIFT)Perspective--;
//          else                             Perspective++;
//          if (Perspective>50) Perspective=0;
//          else if (Perspective<0) Perspective=50;
//          break;
        case 'a':
          if (Fl::event_state() & FL_SHIFT)gAlphaTransp -= 0.02;
          else                             gAlphaTransp += 0.02;
          if (gAlphaTransp >1.0)       gAlphaTransp =1.0;
          else if (gAlphaTransp <0.10) gAlphaTransp =0.10;
          break;
        case 'e':
          if (Fl::event_state() & FL_SHIFT)gBackground-= 0.02;
          else                             gBackground += 0.02;
          if (gBackground >1.0)       gBackground =1.0;
          else if (gBackground <0.10) gBackground =0.10;
          break;
        case 'g':
          if (Fl::event_state() & FL_SHIFT)gGrayOut-= 0.02;
          else                             gGrayOut += 0.02;
          if (gGrayOut >1.0)       gGrayOut =1.0;
          else if (gGrayOut <0.10) gGrayOut =0.10;
          break;
        case '1'|FL_KP:
          RotateVal[0] +=  1;
          RotateVal[1] -=  1;
          break;
        case '2'|FL_KP:
          RotateVal[0] +=  1;
          break;
        case '3'|FL_KP:
          RotateVal[0] +=  1;
          RotateVal[1] +=  1;
          break;
        case '4'|FL_KP:
          RotateVal[1] -=  1;
          break;
        case '5'|FL_KP:
          RotateVal[0] =  0;
          RotateVal[1] =  0;
          RotateVal[2] =  0;
          break;
        case '6'|FL_KP:
          RotateVal[1] +=  1;
          break;
        case '7'|FL_KP:
          RotateVal[0] -=  1;
          RotateVal[1] -=  1;
          break;
        case '8'|FL_KP:
          RotateVal[0] -=  1;
          break;
        case '9'|FL_KP:
          RotateVal[0] -=  1;
          RotateVal[1] +=  1;
          break;
        case '0'|FL_KP:
          break;
        case '.'|FL_KP:
          break;

        }
        RotateValSoll[0]=RotateVal[0];
        RotateValSoll[1]=RotateVal[1];
        RotateValSoll[2]=RotateVal[2];
        SettingsShow(1|2);
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
        return (1);

      case FL_KP +'-' : // Zoom out
      case FL_KP +'+' : // Zoom in
      case 0x6d:
      case 0x6B:
      case '-' :
      case '+' : {
        float z = Zoom;
        if (Fl::event_key()=='+' || Fl::event_key()==0x6b || Fl::event_key()==(FL_KP+'+'))
          z  = z * 1.1;
        else
          z  = z * 0.9;
        Zoom = z;
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
      }

      return (1);
      case FL_Shift_L:
      case FL_Control_L:
        //redraw();
        gObject_redraw=1;
        break;
      case 'd':
      case 'l':
      case 'r':
      case 'o':
      case 't':
      case 'u':
      case 'b':
      case 'h':
      case 'v':
      case 'w':

        center[0]   =  0;
        center[1]   =  0;
        center[2]   =  0;
        eye[0]      =  0;
        eye[1]      =  0;
        eye[2]      =  3;
        RotateValSoll[0]=  0;
        RotateValSoll[1]=  0;
        RotateValSoll[2]=  0;
        Perspective =  1;
        switch (key) {
        case 'h':
          RotateValSoll[0]=  -90;
          RotateValSoll[1]=  0;
          RotateValSoll[2]=  180;
          break;
        case 'v':
          RotateValSoll[0]=  -90;
          RotateValSoll[2]=  0;
          break;
        case 'r':
//          RotateValSoll[1]=  -90;
          RotateValSoll[0]=   -90;
          RotateValSoll[1]=   0;
          RotateValSoll[2]=   -90;
          break;
        case 'l':
//          RotateValSoll[1]=   90;
//          RotateValSoll[2]=   0;
          RotateValSoll[0]=   -90;
          RotateValSoll[1]=   0;
          RotateValSoll[2]=   90;
          break;
        case 'o':
        case 't':
          break;
        case 'u':
        case 'b':
          RotateValSoll[0]=  180;
          RotateValSoll[2]=  180;
          break;
        case 'd':
          //case IDC_GetDefault :
          LookAtLaden(-1);
          if (gPa.Maschine[eDrehachse]) center[1]=-0.3;
          break;
        //case IDC_ShowEyeDef :
        case 'w':
          center[0]   = 0.2;
          center[1]   = 1;
          center[2]   = -0.20;
          eye[0]      = 0.6;//1.8;
          eye[1]      = 0.4;//-4.7;
          eye[2]      = 4;//5.0;
          RotateValSoll[0]= -90.0;
          RotateValSoll[1]= 28.0;
          RotateValSoll[2]= -76;
          Perspective =  60;
          break;

        }
        SettingsShow(2);
        damage(FL_DAMAGE_SCROLL);
        break;




      case '1':
      case '2':
      case '3':
      case '4': {
        gLookAtIx = key - '1';
        {
          if (SpeichernTimeOut) {
            if (gLookAtIx >=0 && gLookAtIx  <=4) {
              LookAtSpeichern(gLookAtIx);
            }
          } else {
            LookAtLaden(gLookAtIx);
          }
        }
      }
      SettingsShow(1|2);
      break;

      case 'q':
        cbExit(NULL,NULL);
        return 1;
      case '.':
        PickFile(NULL,NULL);
        return 1;
      case FL_Menu:
      case ' ' :
        SettingsShow(0x40);
        return 1;
        break;
      }
      //invalidate();
      //redraw();
      gObject_redraw=1;
      Pmodify++;
    } else {

      if ((Fl::event_state() & (FL_ALT | FL_META))) {
        switch (key) {
        case 'f':
        case 'n':
        case 'x':
        case 'c':
        case 'l':
        case 'd':
        case 'o':
          ShowTab(key);
          return 1;
        case 'z':
        case 'm':
        case 'r':
        case 's':
          gMausMode = key;
          SettingsShow(1);
          return 1;
          break;

        }
      } else if ((Fl::event_state() & (FL_CTRL))) {
        switch (key) {
      case FL_KP+'-' :
      case FL_KP+'+' :
      case '-' :
      case '+' : {
        float z = Zoom;
        if (Fl::event_key()=='+' || Fl::event_key()==0x6b || Fl::event_key()==(FL_KP+'+'))
          z  = z * 1.1;
        else
          z  = z * 0.9;
        Zoom = z;
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
      }
      break;
      case 'z':
#ifdef DREITABLE
          LookAtLaden(-1);
#else
          //case IDM_ZoomGanz:
          bArrangeZoomCenter= !bArrangeZoomCenter;
          Zoom = 1.0;
          center[0]   =  0;
          center[1]   =  0;
          center[2]   =  0;
#endif
          break;

        }

      }

      return 0;
    }

    //invalidate();
//    redraw();
//    Pmodify++;
    return 1;



    break;
  case FL_KEYUP :
    switch (Fl::event_key()) {
    case FL_Shift_L:
    case FL_Control_L:
      gObject_redraw=1;//redraw();
      break;
    }
    break;
  case FL_PUSH :
    if (!bHandle) return 1;
    Fl::focus(this);

    if (Fl::event_x() < (x() + w() - SBWIDTH2) &&
        Fl::event_y() < (y() + h() - SBWIDTH2)) {
      //mode_m = Fl::event_button();
      //window()->make_current();
      //fl_overlay_clear();
      gMausModeStart = gMausMode;

      if (Fl::event_button() == FL_LEFT_MOUSE) {
#ifdef DREITABLE
        gMausMode ='r';
#else
#endif        //redraw();
      } else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
#ifdef DREITABLE
        gMausMode ='m';
#else
        if (gMausModeStart=='s') {
          if (gMausMode!='m') {
            gMausMode ='m';
            SettingsShow(1);
          }

        } else {
          if (gMausMode!='r') {
            gMausMode ='r';
            SettingsShow(1);
          }
        }
#endif
      } else if (Fl::event_button() == FL_RIGHT_MOUSE) {
#ifdef DREITABLE
        gMausMode ='r';
#else
        if (gMausMode!='z') {
          gMausMode ='z';
          SettingsShow(1);
        }
#endif
      }
      last_x_   = Fl::event_x_root();
      last_y_   = Fl::event_y_root();
      start_ex_ = Fl::event_x();
      start_ey_ = Fl::event_y();
      ZoomStart = Zoom;
      memcpy(&centerStart,&center,sizeof(centerStart));
      memcpy(&RotateValStart,&RotateVal,sizeof(RotateValStart));

      if (gMausMode=='s') {
        int left,bottom,right,top;

        if (rcAuswahlScreen.right > rcAuswahlScreen.left) {
          right = rcAuswahlScreen.right;
          left  = rcAuswahlScreen.left;
        } else {
          left  = rcAuswahlScreen.right;
          right = rcAuswahlScreen.left;
        }
        // Top-Down
        if (rcAuswahlScreen.top < rcAuswahlScreen.bottom) {
          top    = rcAuswahlScreen.top;
          bottom = rcAuswahlScreen.bottom;
        } else {
          bottom = rcAuswahlScreen.top;
          top    = rcAuswahlScreen.bottom;
        }

        if (!(start_ey_ <  top
              || start_ey_ >= bottom
              || start_ex_ <  left
              || start_ex_ >= right)) {
          AuswahlMove = true;
        } else {
          rcAuswahlScreen.left = start_ex_;
          rcAuswahlScreen.top = start_ey_;
          rcAuswahlScreen.right = Fl::event_x();
          rcAuswahlScreen.bottom = Fl::event_y();

        }
        if ((Fl::event_state() & (FL_CTRL| FL_SHIFT))==0) {
          gObjectManager->Selection(0);
        }


      }
      return (1);
    }
    break;

  case FL_DRAG :
    if (!bHandle) return 1;
    if (Fl::event_x() < (x() + w() - SBWIDTH2) &&
        Fl::event_y() < (y() + h() - SBWIDTH2)) {
      float ww   = (float)(last_x_ - Fl::event_x_root())/(float)h();
      float hh   = (float)(last_y_ - Fl::event_y_root())/(float)h();
      if (gMausMode=='m') {
        if (bHandle) {
          if (ShowSeparate==0) {
            center[0] = centerStart[0] + (ww * Perspective / 29);
            center[1] = centerStart[1] - (hh * Perspective / 29);
          } else {
            center[0] = centerStart[0] + (ww)*0.7;
            center[1] = centerStart[1] - (hh)*0.7;
          }
          Pmodify++;
          damage(FL_DAMAGE_SCROLL);
          return (1);
        }
      } else if (gMausMode=='z') {
        Zoom = ZoomStart + hh;
        float val = hh-ww;
        Zoom = ZoomStart * (1.0 + val);
        if (Zoom  <0.1) Zoom =0.1;
        else if (Zoom  > 15) Zoom  = 15;
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
        return (1);
      } else if (gMausMode=='r') {
        RotateVal[2] = RotateValStart[2] - ww*180.0f;
        RotateVal[0] = RotateValStart[0] - hh*180.0f;
        RotateValSoll[0]=RotateVal[0];
        RotateValSoll[1]=RotateVal[1];
        RotateValSoll[2]=RotateVal[2];
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
        return (1);
      } else if (gMausMode=='s') {
        if (AuswahlMove) {
          OffsetRect_(&rcAuswahlScreen,
                      Fl::event_x() - start_ex_,
                      Fl::event_y() - start_ey_);
          start_ex_ = Fl::event_x();
          start_ey_ = Fl::event_y();
        } else {
          rcAuswahlScreen.right = Fl::event_x();
          rcAuswahlScreen.bottom = Fl::event_y();
        }
        // Umrechnen auf GL Fenster -1 .. +1
        float mod =  1.0;
        if (glMaximum == glMaximumH) mod =  glMaximumH / (float)w();
        else                         mod =  glMaximumV / (float)h();
        // Vom Zentrum aus rechnen
        rcAuswahl_gl[0] = (float)(rcAuswahlScreen.left-w()/2)    * mod;
        rcAuswahl_gl[1] = (float)(rcAuswahlScreen.right-w()/2)   * mod;
        rcAuswahl_gl[2] = (float)(h()/2 - rcAuswahlScreen.top)   * mod;
        rcAuswahl_gl[3] = (float)(h()/2 - rcAuswahlScreen.bottom)* mod;

        // Anzeige Zentrum ungenau zurechen
        float mx = center[0]*1.3 * glMaximum;
        float my = center[1]*1.3 * glMaximum;

        rcAuswahl_gl[0] += mx;
        rcAuswahl_gl[1] += mx;
        rcAuswahl_gl[2] += my;
        rcAuswahl_gl[3] += my;

        rcAuswahl_gl[0] /= Zoom;
        rcAuswahl_gl[1] /= Zoom;
        rcAuswahl_gl[2] /= Zoom;
        rcAuswahl_gl[3] /= Zoom;


        printf(" X%0.2f  Y%0.2f  mx%0.2f my%0.2f CX%0.2f CY%0.2f\n",rcAuswahl_gl[1],rcAuswahl_gl[3],mx,my,center[0],center[1]);
        float ZO = (gObjectManager->MinMaxback- gObjectManager->MinMaxfront)/2.0;
        float XO = (gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
        float YO = (gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;

        rcAuswahlCNC.left   = rcAuswahl_gl[0]+XO;
        rcAuswahlCNC.right  = rcAuswahl_gl[1]+XO;
        rcAuswahlCNC.top    = rcAuswahl_gl[2]+YO;
        rcAuswahlCNC.bottom = rcAuswahl_gl[3]+YO;
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
        return (1);
      }
      last_x_ = Fl::event_x_root();
      last_y_ = Fl::event_y_root();
      return (1);
    }
    RotateValSoll[0]=RotateVal[0];
    RotateValSoll[1]=RotateVal[1];
    RotateValSoll[2]=RotateVal[2];
    break;
  /*  case FL_MOVE:
      {
        int hits = drawObjects(GL_SELECT,Fl::event_x(),Fl::event_y());
        if (hits) {
              int closest = 0;
              int dist = 0xFFFFFFFFU; //2147483647;
              //hits = NumHits;
              while (hits) {
                if (selectBuf[(hits - 1) * 4 + 3]!=0){
                  if (selectBuf[(hits - 1) * 4 + 1] < dist) {
                    dist = selectBuf[(hits - 1) * 4 + 1];
                    closest = selectBuf[(hits - 1) * 4 + 3];
                  }
                }
                hits--;
              }
              printf("Hits %d Closest Hit %d\n",NumHits,closest);
          redraw();
        }

      }
      break;*/
  case FL_RELEASE :
    if (!bHandle) return 1;
    {
#ifdef DREITABLE
      if (gMausMode!=gMausModeStart) {
        gMausMode = gMausModeStart;
        SettingsShow(1);
      }
#endif
      //window()->make_current();
      //fl_overlay_clear();
      if (gMausMode=='s') {
        if (AuswahlMove) {
          AuswahlMove=false;
        }
        float ZO = (gObjectManager->MinMaxback- gObjectManager->MinMaxfront)/2.0;
        float XO = (gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
        float YO = (gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;
        rcAuswahlCNC.left   = rcAuswahl_gl[0]+XO;
        rcAuswahlCNC.right  = rcAuswahl_gl[1]+XO;
        rcAuswahlCNC.top    = rcAuswahl_gl[2]+YO;
        rcAuswahlCNC.bottom = rcAuswahl_gl[3]+YO;

        gObjectManager->SetSelection(&rcAuswahlCNC);

      }
      if (Fl::event_button() == FL_LEFT_MOUSE) {

        if (abs (rcAuswahlScreen.left-rcAuswahlScreen.right)< 3 &&  abs (rcAuswahlScreen.top-rcAuswahlScreen.bottom)< 3 ) {
          //SettingsShow(0xF0);
        } else {
          SettingsShow(0x80);
        }
        //      SetSelection(&rcAuswahlScreen);
#ifdef MOUSESERVICE
        if (gMausMode!='s' && gDreiDFace==0
            && abs (rcAuswahlScreen.left-rcAuswahlScreen.right)< 3 &&  abs (rcAuswahlScreen.top-rcAuswahlScreen.bottom)< 3 ) {
          float valx = ((float)last_x_ - Fl::event_x_root())/100.0f;
          float valy = ((float)last_y_ - Fl::event_y_root())/100.0f;
          closest = -1;
          if ((Fl::event_state() & (FL_CTRL| FL_SHIFT))==0) {
            gObjectManager->Selection(0);
          }
          int hits = drawObjects(GL_SELECT,Fl::event_x(),Fl::event_y());
          if (hits>0) {
            int dist = 0xFFFFFFFFU; //2147483647;
            //hits = NumHits;
            while (hits) {
              // find Z Axis
              if (selectBuf[(hits - 1) * 4 + 3]!=0) {
                if (selectBuf[(hits - 1) * 4 + 1] < dist) {
                  dist = selectBuf[(hits - 1) * 4 + 1];
                  closest = selectBuf[(hits - 1) * 4 + 3];
                }
              }
              hits--;
            }
            // handle Selection
            drawObjects(0,Fl::event_x(),Fl::event_y());
          } else redraw();
          //processHits(NumHits, selectBuf);
#ifdef FLTREE
          if (pDXFTree) {
            pDXFTree->redraw();
            Fl::check();
            pTreeitem_found =  pDXFTree->first_selected_item();
            Fl::check();
            pDXFTree->show_item_top(pTreeitem_found );
          }
#endif
          return 1;
        }
        redraw();
#endif
        return 1;
      } else if (Fl::event_button() == FL_RIGHT_MOUSE) {


        if (abs (rcAuswahlScreen.left-rcAuswahlScreen.right)< 3 &&  abs (rcAuswahlScreen.top-rcAuswahlScreen.bottom)< 3 ) {
          SettingsShow(0x40);

        }
      }
      RotateValSoll[0]=RotateVal[0];
      RotateValSoll[1]=RotateVal[1];
      RotateValSoll[2]=RotateVal[2];
    }
    break;
  // Receiving Drag and Drop
  case FL_DND_ENTER:
    //if (wSettings->visible()) return 0;
    return 1;

  case FL_DND_DRAG:
    return 1;
  case FL_DND_RELEASE:
    return 1;
  case FL_DND_LEAVE:
    return 1;

  case FL_PASTE: {
    // make a copy of the DND payload
    int evt_len = Fl::event_length()+1;
    if (evt_len > sizeof(evt_txt)) evt_len = sizeof(evt_txt);
    // handling timer callback
    strncpy(evt_txt, Fl::event_text(),evt_len);
    return 1;
  }
  break;

  case FL_MOUSEWHEEL: {
    if (!bHandle) return 1;
    if (1||gMausMode=='m') {
      if (Fl::event_dy()) {
        if ((Fl::event_state() & FL_CTRL) && (Fl::event_state() & FL_SHIFT)) {
          RotateVal[2] = RotateVal[2] - (float)Fl::event_dy()*3.0;
        } else if (Fl::event_state() & FL_CTRL) {
          RotateVal[0] = RotateVal[0] - (float)Fl::event_dy()*3.0;
        } else if (Fl::event_state() & FL_SHIFT) {
#ifndef DREITABLE
          if (gPa.Maschine[eDrehachse]) {
            gCNCPos[3] += Fl::event_dy()*100;
          } else {
            RotateVal[1] = RotateVal[1] - (float)Fl::event_dy()*3.0;
          }
#else
          RotateVal[1] = RotateVal[1] - (float)Fl::event_dy()*3.0;
#endif
        } else  {
          float val = (float)Fl::event_dy()/10.0;
          if (ShowSeparate==0) {
            val /= 10.0;
          }

          Zoom /= 1.0-val;
          if (Zoom  <0.1) Zoom =0.1;
          else if (Zoom  > 15) Zoom  = 15;
          float value = 0.071 * Zoom;
          if (ShowSeparate==0) {
            value = 0.02 * Zoom;
          }
#ifdef DREITABLE
          extern Fl_Window         * wMain;
          int xe = Fl::event_x_root() - wMain->x();
          int ye = Fl::event_y_root() - wMain->y();
#else
          extern  Fl_Double_Window* MainWindow;
          int xe = Fl::event_x_root() - MainWindow->x();
          int ye = Fl::event_y_root() - MainWindow->y();
#endif
          float ww   = (float)(((w()-x()))/2.0 - xe)/(float)h();
          float hh   = (float)(((h()-y()))/2.0 - ye)/(float)h();
          if (Fl::event_dy()<0) {
            center[0] +=   ww * value;
            center[1] -=   hh * value;
          } else {
            center[0] -=   ww * value;
            center[1] +=   hh * value;
          }
          //printf("ww=%0.3f hh=%0.3f val =%0.10f\n",ww,hh,value);

        }
        RotateValSoll[0]=RotateVal[0];
        RotateValSoll[1]=RotateVal[1];
        RotateValSoll[2]=RotateVal[2];
        Pmodify++;
        damage(FL_DAMAGE_SCROLL);
        return (1);
      }
    }
  }

  }
  return Fl_Gl_Window::handle(event);
}

//-------------------------------------------------------------
//-------------------------------------------------------------
void CObjectManager::Execute(int Mode)
{
#ifndef DREITABLE
  gObjectManager->Sort_Reset();
  ClipperLib::cInt X = gCNCPos[Xp];
  ClipperLib::cInt Y = gCNCPos[Yp];
  CObject * pOb =  gObjectManager->GetNearest(Mode,WorkLayer,AktTiefe,X,Y,1);
  //fprintf(stderr,"CObjectManager::Execute %s %d\n",szWorkLayer,AktTiefe);
  while (pOb && !gStopp) {
    pOb->ExecuteDrawObject(Mode);
    X = pOb->m_S.X;
    Y = pOb->m_S.Y;
    pOb =  gObjectManager->GetNearest(Mode,WorkLayer,AktTiefe,X,Y,1);
    //fprintf(stderr,".");
  }
  //fprintf(stderr,"\nEnde Execute\n");
  if (!gStopp)  CNCAbheben();
#endif
}
//-------------------------------------------------------------
int i3DreiTafel;
void DXF3DDisplay::draw(void)
{
  //CLayers * i3DreiTafelLayer = IsLayerIndex(this,"DREITAFEL");
#ifdef USE_SHADER
  i3DreiTafel= 1;
#else
  i3DreiTafel= 0;//i3DreiTafelLayer!= NULL;
#endif
  NumVertextoSave=0;
  if (gObjectManager->CNCInfoCnt>0) {
//    EnterCriticalSection(&SerialComm::s_critical_section);
    if (i3DreiTafel == 0) {
      gDreiDFace=gbFillPolygon;
      drawObjects(GL_RENDER,0,0);
    } else {
#ifdef USE_SHADER
      drawSoftMultiple();
      //else draw3TafelObjects(GL_RENDER,0,0);
#else
      draw3TafelObjects(GL_RENDER,0,0);
#endif
    }
//    LeaveCriticalSection(&SerialComm::s_critical_section);

//  } else {
//    int bGriff = (Fl::event_state() & FL_SHIFT)==0;
//#ifdef USE_SHADER
//    //if (bGriff) drawStl();else
//    drawSoftMultiple();
//#else
//    drawSoft();
//#endif
  }
}

//-------------------------------------------------------------
int DXF3DDisplay::drawObjects(int Mode,int XMouse, int YMouse)
{
  static int norecursion=0;
  if (norecursion)
    return 0;
  norecursion++;
  if ((!valid())) {
#ifdef USE_SHADER
    if (GlInit)
      CleanupOpenGls();
#endif
  }

  if (!GlInit)
    InitOpenGL();

  glViewport(0, 0, pixel_w(), pixel_h());
#ifdef USE_SHADER
  glUseProgram(0);
#endif
#ifdef MOUSESERVICE
  if (gDreiDFace==0 && Mode == GL_SELECT) {
//  glLoadName(0);
    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    glInitNames();
    glPushName((GLuint)~0);
    glPushMatrix();
  }
#endif

#ifdef USE_SHADER
  SetGeometrics(10000);
#endif
  float	X, Y, W, H;			// Interior of widget
  X = x() + Fl::box_dx(box());
  Y = y() + Fl::box_dy(box());
  W = w() - Fl::box_dw(box());
  H = h() - Fl::box_dh(box());

  // Calculate the faktor to fit gl-Objects in the Range -1.0 to 1.0
  glMaximumH = gObjectManager->MinMaxright -  gObjectManager->MinMaxleft;
  glMaximumV = gObjectManager->MinMaxbottom - gObjectManager->MinMaxtop;
  ZOffset    = 0;
  if (glMaximumV<0) {
    glMaximumV = gObjectManager->MinMaxback- gObjectManager->MinMaxfront;
    //ZOffset    = (gObjectManager->MinMaxback- gObjectManager->MinMaxfront)/2.0;

  }
  XOffset    = 0;//(gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
  YOffset    = 0;//(gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;
  glFaktor   = 10;
  if ((glMaximumH / glMaximumV) > (W/H)) {
    glMaximumV = glMaximumH * (H/W);
    glMaximum  = glMaximumV;
  } else  glMaximum = glMaximumH;
  glFaktor = 1.0;///0.5 / glMaximum;

  int wrkspleft;
  int wrksptop;    // Invers !
  int wrkspground;
  int wrkspright;
  int wrkspbottom;
  int FocusIndex =-1;
  int ArrayIndex =-1;
#ifdef FLTREE
  if (pDXFTree) {

    CDXFPolyItem * Pitem = (CDXFPolyItem *)pDXFTree->GetItem(pDXFTree->m_AktIndex);//(CDXFPolyItem *)pDXFTree->get_item_focus();
    if (Pitem && Pitem->GetImage() == imPicture) {
      FocusIndex = Pitem->m_lParam;
    } else  if (Pitem && Pitem->GetImage()== imRadio) {
      FocusIndex = ((CDXFObjectItem*)Pitem->parent())->m_lParam;
      ArrayIndex = Pitem->m_lParam;
    }
  }
#endif
  if (gPa.Drehen==1)  {
    wrkspleft  = -gPa.Ref[Yp];
    wrkspright = gPa.Anschlag[Yp] -gPa.Ref[Yp];
    wrkspbottom= gPa.Ref[Xp]-gPa.Anschlag[Xp];
    wrksptop   = gPa.Ref[Xp]-gPa.Anschlag[Xp]+gPa.Anschlag[Xp];
  } else {
    wrkspleft  = -gPa.Ref[Xp];
    wrkspright =  gPa.Anschlag[Xp] - gPa.Ref[Xp];
    wrkspbottom= -gPa.Ref[Yp];
    wrksptop   =  gPa.Anschlag[Yp] - gPa.Ref[Yp];
  }
  //wrksptop
  wrkspground= (gPa.Anschlag[Zp]/-2);
  if (gPa.Maschine[eDrehachse]==0) {
    wrkspground	 +=  gPa.Ref[Zp];
  }
#ifndef DREITABLE
  int XPos  = 0;
  int YPos  = 0;

  int ZPos = gCNCPos[Zp]-gPa.Ref[Zp];
  XPos  = gCNCPos[Xp];
  YPos  = gCNCPos[Yp];

  /*  if (gPa.Drehen==1) {
    XPos  = gCNCPos[Yp]-gPa.Ref[Yp];
    YPos  = gPa.Ref[Xp]-gCNCPos[Xp];
  } else if (gPa.Drehen==10) {
    XPos  = gPa.Ref[Yp]-gCNCPos[Yp];
    YPos  = gCNCPos[Xp]-gPa.Ref[Xp];
  } else {
    XPos  = gCNCPos[Xp]-gPa.Ref[Xp];
    YPos  = gCNCPos[Yp]-gPa.Ref[Yp];
  }*/
#endif
  glMatrixMode(GL_PROJECTION);


  if (1||Perspective==1)  glLoadIdentity();

  glGetIntegerv (GL_VIEWPORT, viewport );
#ifdef MOUSESERVICE
  if (Mode == GL_SELECT) {
    gluPickMatrix(XMouse, (viewport[3] - YMouse), 25.0, 25.0, viewport);
  }
#endif
  if (gLookClip) {
    gluPerspective(Perspective,(float)viewport[2]/(float)viewport[3], 1, 13.0);
  } else {
    gluPerspective(Perspective,(float)viewport[2]/(float)viewport[3], 0.1,5.0);
  }

  glMatrixMode(GL_MODELVIEW);
  if (gBackground) {
    float backg[4];
    backg[0] = backg[1] = backg[2] = gBackground;//LightColor[2]/2.0;
    glClearColor(backg[0],backg[1],backg[2],1);
  }
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_NORMALIZE);

  glLoadIdentity();
  float Zoom2 = Zoom * Perspective / 40;
  Zoom2 /= glMaximum;

  glTranslatef(center[0]*-1.0/30.0*Perspective,center[1]*-1.0/30.0*Perspective,center[2]/10.0-1.5);// );
  glScalef(Zoom2, Zoom2, Zoom2);

  glRotatef(RotateVal[1],0.0,1.0,0.0);
  glRotatef(RotateVal[0],1.0,0.0,0.0);
  glRotatef(RotateVal[2],0.0,0.0,1.0);


  float ZO = (gObjectManager->MinMaxback- gObjectManager->MinMaxfront)/2.0;
  float XO = (gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
  float YO = (gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;
  glTranslatef(-XO,-YO,-ZO);



  float MaterialDicke = -gPa.Ref[Zp]*glFaktor;
  int clr;
  int SelCnt=0;
  int ActicCnt=0;
  int ActArt=-1;
  int bGriff = (Fl::event_state() & (FL_CTRL|FL_SHIFT) && gDreiDFace==0);
  if (1) {
#ifndef DREITABLE
    float SXOffset =XOffset;
    float SYOffset =YOffset;
    float SZOffset =ZOffset;
    //float  rx1 = glXCNCToScreen(gPa.Ref[Xp]);
    //float  ry1 = glYCNCToScreen(gPa.Ref[Yp]);
    //float  rz1 = glZCNCToScreen(gPa.Ref[Zp]);
    float  rx1 = glXCNCToScreen(0);
    float  ry1 = glYCNCToScreen(0);
    float  rz1 = glZCNCToScreen(0);
    // float  rx1 = 0;//gPa.Ref[Xp]*glFaktor;
    // float  ry1 = 0;//gPa.Ref[Yp]*glFaktor;
    // float  rz1 = 0;//gPa.Ref[Zp]*glFaktor;
    if (1||gPa.Maschine[eDrehachse]) {
      glPushMatrix();
      int   Rotation    = ((int)fabs(gCNCPos[Ap])%36000);
      float RotationNeu = (float)(Rotation%36000) /-100.0;// / (36000.0 / (2.0 * M_PI));
      glTranslatef(rx1,ry1,rz1);
//    glTranslatef(gPa.Ref[Xp] * glFaktor,
//    gPa.Ref[Yp] * glFaktor,
//    gPa.Ref[Zp] * glFaktor);

      glRotatef(RotationNeu,1.0,0.0,0.0);

//    glTranslatef(gPa.Ref[Xp] * glFaktor,
//    gPa.Ref[Yp] * glFaktor,
//    gPa.Ref[Zp] * glFaktor);

      XOffset =0;
      YOffset =0;
      ZOffset =0;
    }
#endif
    int iName=0;
    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);
      while (pOb) {
#ifndef  DREITABLE
        if (!gCNCAktiv && inc.m_ObjectManager->GetAktObjectIndex() == FocusIndex) {
          glColor3f(0,1,1);
          float  y1 = glYCNCToScreen(pOb->m_S.Y);
          float  x1 = glXCNCToScreen(pOb->m_S.X);
          float  z1 = glZCNCToScreen(pOb->m_S.Z)+0.02;
          if (pOb->Art == Linie) {
            CPolyLineObject * pObL    =(CPolyLineObject*) pOb;
            if (ArrayIndex >= 0 && ArrayIndex < pObL->m_Path.size()) {
              y1 = glYCNCToScreen(pObL->m_Path[ArrayIndex].Y);
              x1 = glXCNCToScreen(pObL->m_Path[ArrayIndex].X);
              z1 = glZCNCToScreen(pObL->m_Path[ArrayIndex].Z)+0.02;
            }
          }
          glTranslatef(x1,y1,z1);
          glutWireCone(0.007,-0.02,9,3);
          glTranslatef(-x1,-y1,-(z1));
          glBegin(GL_LINES);
          glVertex3f(x1,y1,z1+0.1);
          glVertex3f(x1,y1,z1-0.10);
          glEnd();
        }
#endif
        clr = pOb->GetRGBColor();
#ifdef MOUSESERVICE
        if (gDreiDFace==0) {
          iName++;
          glLoadName((int)iName);
          if ( bGriff) {
            CPolyLineObject * pObL    =(CPolyLineObject*) pOb;
            glLineWidth(2);
            float xs,ys,zs;
            ys = glYCNCToScreen(pObL->m_S.Y);// m_Path[0].Y);
            xs = glXCNCToScreen(pObL->m_S.X);//m_Path[0].X);
            zs = glZCNCToScreen(pObL->m_S.Z);//m_Path[0].Z);

            float rs = 0.005;
            if (closest==iName) glColor4f (1.,0, 1.,0.5);
            else glColor4f (0.7,0, 0.7,0.5);
            if (bGriff == FL_CTRL) {
              glBegin (GL_LINES);
              glVertex3f(xs,ys-rs/2,zs);
              glVertex3f(xs,ys+rs/2,zs);
              glVertex3f(xs+rs/2,ys,zs);
              glVertex3f(xs-rs/2,ys,zs);
              glEnd ();
            }
            if (bGriff == FL_SHIFT) {
              glBegin (GL_LINES);
              glVertex3f(xs+rs/2,ys,zs);
              glVertex3f(xs-rs/2,ys,zs);
              glEnd ();
            }
            {
              glBegin (GL_QUADS);
              glColor4f (1,1, 1,0.2);
              zs-=0.001;
              //glBegin (GL_LINE_LOOP );
              glVertex3f(xs-rs,ys+rs,zs);
              glVertex3f(xs+rs,ys+rs,zs);
              glVertex3f(xs+rs,ys-rs,zs);
              glVertex3f(xs-rs,ys-rs,zs);
              glEnd ();
            }
            glLoadName(0);
          }
          if (iName) {
            if (closest==iName) {  // Mouse Cklic found
              if ((Fl::event_state() & FL_CTRL)) {
                pOb->SetSelect(Sel);
              } else if ((Fl::event_state() & FL_SHIFT)) {
                pOb->SetSelect(0);
              } else if (!pOb->IsSelected()) {
                pOb->SetSelect(Sel);
              } else {
                pOb->SetSelect(0);
              }
              clr = pOb->GetRGBColor();
              closest=-1;
              damage(FL_DAMAGE_OVERLAY);
            } else if (0) {
              for (int hi=0; hi <NumHits; hi++) {
                if (selectBuf[(hi - 1) * 4 + 3]==iName) {
                  clr = 0xF7F7F7;
                }
              }
            }
          }
        }
#endif
        glColor4f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);
        glLineWidth(pOb->GetWitdh());
        if (pOb->Is3DFace() != ActArt) {
          if (pOb->Is3DFace()) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            ///      glEnable(GL_LIGHT2);
            //   glEnable(GL_LIGHT3);


            float black[] = { 0.0, 0.0, 0.0, 1.0 };
            float yellow[] = { 1.0, 1.0, 0.95, 1.0 };
            float cyan[] = { 0.0, 1.0, 1.0, 1.0 };
            float white[] = { 1.0, 1.0, .8, 1.0 };
            float direction[] = { 1.0, 1.0, 1.0, 0.0 };
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
            glMaterialf(GL_FRONT, GL_SHININESS, 30);
            glLightfv(GL_LIGHT0, GL_AMBIENT, black);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  yellow);
            glLightfv(GL_LIGHT0, GL_SPECULAR, white);
            glLightfv(GL_LIGHT0, GL_POSITION, direction);

            glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
            white[0] = white[1] = white[2] = LightColor[0];
            //white[0] += (LightColor[1]-1.0)/3.0;
            //white[2] -= (LightColor[1]-1.0)/3.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, white);// LightColor);
            //white[0] = white[1] = white[2] = LightColor[1];
            //glLightfv(GL_LIGHT0, GL_SPECULAR,  white);
            white[0] = white[1] = white[2] = LightColor[1];
            //white[0] += (LightColor[1]-1.0)/3.0;
            //white[2] -= (LightColor[1]-1.0)/3.0;
            glLightfv(GL_LIGHT0, GL_AMBIENT, white);

          } else {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
          }
          ActArt = pOb->Is3DFace();

        }
        if (gDreiDFace) {
          clr = pOb->GetRGBColor();

          GLfloat mat_diffuse[4];
          mat_diffuse[0] = (GLfloat)(clr>>0 & 0xFF) / 255.0;
          mat_diffuse[1] = (GLfloat)(clr>>8 & 0xFF) / 255.0;
          mat_diffuse[2] = (GLfloat)(clr>>16 & 0xFF)/ 255.0;
          mat_diffuse[3] = gAlphaTransp;

          glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
          glColor4f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);



        }
        if (gShowSymulation&2) {
          pOb->ExecuteDrawObject(Draw);
        }
        pOb =  inc.m_ObjectManager->GetNext();
      }

#ifdef BLOCKSIMULATION
      //if (gBlockTab && gBlockTab->visible()|| gBlockTab && gPerspektive->visible()){
      if (gShowSymulation&1) {
        if (gBlockSimulation) gBlockSimulation->drawBlock();

      }
#endif
      //}
    }
#ifndef DREITABLE
    if (1||gPa.Maschine[eDrehachse]) {

      glPopMatrix();
      if (gNullpunktTab->visible()) {
        // Nullpunkt vom Object
        float  y1 = glYCNCToScreen(0);
        float  x1 = glXCNCToScreen(0);
        float  z1 = glZCNCToScreen(0);
        glBegin(GL_LINES);
        glColor4f(.7,0.7,0,1.0);
        int r = 0.1 *glMaximum;
        glVertex3f(x1+r,y1+r,z1);
        glVertex3f(x1-r,y1-r,z1);
        glVertex3f(x1-r,y1+r,z1);
        glVertex3f(x1+r,y1-r,z1);

        glVertex3f(x1,y1,z1+r);
        glVertex3f(x1,y1,z1-r);
        glEnd();
      }

//	  glTranslatef(0,0,-(6500+wrkspground));

      XOffset =SXOffset;
      YOffset =SYOffset;
      ZOffset =SZOffset;
    }
#endif
  }
  if (gMausMode=='s') {
    if (rcAuswahlCNC.left != rcAuswahlCNC.right && rcAuswahlCNC.top != rcAuswahlCNC.bottom) {
      glColor4f(.0,0.8,0.2,1);
      glLineWidth(1);
      float rcAuswahl[4] = {0,0,0,0};

      rcAuswahl[0] = glXCNCToScreen(rcAuswahlCNC.left);
      rcAuswahl[1] = glXCNCToScreen(rcAuswahlCNC.right);
      rcAuswahl[2] = glYCNCToScreen(rcAuswahlCNC.top);
      rcAuswahl[3] = glYCNCToScreen(rcAuswahlCNC.bottom);
      glBegin(GL_LINE_LOOP);
      glVertex3f(rcAuswahl[0],rcAuswahl[2],0.01);
      glVertex3f(rcAuswahl[1],rcAuswahl[2],0.01);
      glVertex3f(rcAuswahl[1],rcAuswahl[3],0.01);
      glVertex3f(rcAuswahl[0],rcAuswahl[3],0.01);
      glEnd();
    }
  }

#ifndef DREITABLE
  if (1) {
    if (gSpindleOn) {
      glColor3f(0.9,0.9,1);
    } else if (ZPos <  0)
      glColor3f(0.3,0.8,1);
    else           glColor3f(0.5,1,0.5);
    float  y1 = glYCNCToScreen(YPos);
    float  x1 = glXCNCToScreen(XPos);
    float  z1 = glZCNCToScreen(ZPos);
#ifdef BLOCKSIMULATION
    if (gBlockSimulation) {
      gBlockSimulation->DrawMeissel(x1,y1,z1);
    } else
#endif
    {
      z1 += 0.02*glMaximum;
      glTranslatef(x1,y1,z1);
      //glutSolidCone(0.4,0.5,20,10);
      glutSolidCone(0.010*glMaximum,-0.02*glMaximum,9,3);
      glTranslatef(-x1,-y1,-z1);
      glBegin(GL_LINES);
      glVertex3f(x1,y1,0.1*glMaximum);
      glVertex3f(x1,y1,0);
      glEnd();
    }
  }


  if (gPa.Maschine[eDrehachse]==0 && gNullpunktTab->visible()) {
    // Nullpunkt vom Object
    float  y1 = glYCNCToScreen(0);
    float  x1 = glXCNCToScreen(0);
    float  z1 = glZCNCToScreen(0);
    glBegin(GL_LINES);
    glColor4f(.7,0.7,0,1.0);
    int r = 0.1 *glMaximum;
    glVertex3f(x1+r,y1+r,z1);
    glVertex3f(x1-r,y1-r,z1);
    glVertex3f(x1-r,y1+r,z1);
    glVertex3f(x1+r,y1-r,z1);

    glVertex3f(x1,y1,z1+r);
    glVertex3f(x1,y1,z1-r);
    glEnd();
  }

  if (gDreiDFace) {
    // Arbeitsflaeche Transparent am Schluss
    float MaterialDickeU = MaterialDicke -0.1*glMaximum;
    float urcleft   = (wrkspleft) * glFaktor;
    float urcright  = (wrkspright) * glFaktor;
    float urcbottom = (wrkspbottom) * glFaktor;
    float urctop    = (wrksptop) * glFaktor;
    float wrkspz    = (wrkspground) * glFaktor;
    float Unterlage[8][3]= {
      {urcleft,urctop,wrkspz+MaterialDicke},
      {urcright,urctop,wrkspz+MaterialDicke},
      {urcright,urcbottom,wrkspz+MaterialDicke},
      {urcleft,urcbottom,wrkspz+MaterialDicke},
      {urcleft,urctop,wrkspz+MaterialDickeU},
      {urcright,urctop,wrkspz+MaterialDickeU},
      {urcright,urcbottom,wrkspz+MaterialDickeU},
      {urcleft,urcbottom,wrkspz+MaterialDickeU}
    };

    glColor4f(.3,0.3,0.3,1);

    glBegin(GL_LINE_LOOP);
    //glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
    glVertex3fv(Unterlage[0]);
    glVertex3fv(Unterlage[1]);
    glVertex3fv(Unterlage[2]);
    glVertex3fv(Unterlage[3]);
    glEnd();

    glColor4f(.1,0.3,0.3,gAlphaTransp);

    //glBegin(GL_QUADS);
    glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
    glVertex3fv(Unterlage[0]);
    glVertex3fv(Unterlage[1]);
    glVertex3fv(Unterlage[2]);
    glVertex3fv(Unterlage[3]);
    glEnd();

    glColor4f(.1,0.3,0.3,gAlphaTransp);

    glBegin(GL_POLYGON);
    glNormal3f(0,0,-1);
    glVertex3fv(Unterlage[4]);
    glVertex3fv(Unterlage[5]);
    glVertex3fv(Unterlage[6]);
    glVertex3fv(Unterlage[7]);
    glEnd();


    glColor4f(.1,0.5,0.1,gAlphaTransp);
    glBegin(GL_POLYGON);
    glNormal3f(-1,0,0);
    glVertex3fv(Unterlage[0]);
    glVertex3fv(Unterlage[3]);
    glVertex3fv(Unterlage[7]);
    glVertex3fv(Unterlage[4]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(1,0,0);
    glVertex3fv(Unterlage[1]);
    glVertex3fv(Unterlage[2]);
    glVertex3fv(Unterlage[6]);
    glVertex3fv(Unterlage[5]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0,1,0);
    glVertex3fv(Unterlage[0]);
    glVertex3fv(Unterlage[1]);
    glVertex3fv(Unterlage[5]);
    glVertex3fv(Unterlage[4]);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0,-1,0);
    glVertex3fv(Unterlage[2]);
    glVertex3fv(Unterlage[3]);
    glVertex3fv(Unterlage[7]);
    glVertex3fv(Unterlage[6]);
    glEnd();


  }
#ifndef SCROLLBAR3D
  if (Pmodify && gPerspektive && gPerspektive ->visible()) gPerspektive->Show();
  Pmodify =0;
#endif
#endif
#ifdef MOUSESERVICE
  if (gDreiDFace==0 && Mode == GL_SELECT) {
    glPopMatrix();
    NumHits = glRenderMode(GL_RENDER);
  }
#endif
  norecursion--;
#ifdef MOUSESERVICE
  return NumHits;
#else
  return 0;
#endif
}


int DXF3DDisplay::SaveStlObjects(int Mode)
{
#ifdef DREITABLE

  //i3DreiTafelLayer = IsLayerIndex("DREITAFEL");
  char PfadName[256];
  strcpy (PfadName,gObjectManager->GetIniName());
  char *ext2 = strrchr(PfadName, '.');
  if (ext2) {
    strcpy(ext2,".stl");
  }
  if (savebinary) {
    m_file = fopen(PfadName, "wb");
    if(m_file) {
      char Header[80]= {0};
      sprintf(Header,"COLOR=\177\177\177 MATERIAL=\177\177\177 \177\177\177 \177\177\177");
      fwrite(Header,80,1,m_file);
      fwrite(&NumVertextoSave,4,1,m_file);
      NumVertextoSave=0;
      SaveStlPolygons(gObjectManager,1);
      //draw3TafelObjects(SavePolygon,0,0);
#ifdef WIN32
      fpos_t  pos = 80;
#else
      fpos_t  pos = {80,0};
#endif
      fsetpos(m_file,&pos);
      fwrite(&NumVertextoSave,4,1,m_file);
      fclose(m_file);
    }
  } else {
    m_file = fopen(PfadName, "wt");
    if(m_file) {
      fprintf(m_file,"solid Name\n");
      //draw3TafelObjects(SavePolygon,0,0);
      SaveStlPolygons(gObjectManager,0);

      fprintf(m_file,"endsolid Name\n");
      fclose(m_file);
    }
  }
  m_file=NULL;
  //solid name
#endif
  return 0;
}

//-------------------------------------------------------------
int DXF3DDisplay::draw3TafelObjects(int Mode,int XMouse, int YMouse)
{
#ifndef USE_SHADER
  static int norecursion=0;
  if (norecursion)
    return 0;
  norecursion++;
  if ((!valid())) {
#ifdef USE_SHADER
    if (GlInit)
      CleanupOpenGls();
#endif
  }
  if (!GlInit)  InitOpenGL();
#ifdef USE_SHADER
  glUseProgram(0);
#endif
  glViewport(0, 0, pixel_w(), pixel_h());

  float	X, Y, W, H;			// Interior of widget
  X = x() + Fl::box_dx(box());
  Y = y() + Fl::box_dy(box());
  W = w() - Fl::box_dw(box());
  H = h() - Fl::box_dh(box());

  // Calculate the faktor to fit gl-Objects
  float XMax = gObjectManager->MinMaxright -  gObjectManager->MinMaxleft;
  float Max  = gObjectManager->MinMaxbottom - gObjectManager->MinMaxtop;
  XOffset    = (gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
  YOffset    = (gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;
  float MMToScreen   = 10;
  if ((XMax / Max) > (W/H)) {
    Max = XMax * (H/W);
  }
  //MMToScreen = 0.5 / Max;
  MMToScreen = Max * 2.0;
  ZOffset    = 0.5 * MMToScreen;

  float glXOffset =   (float)XOffset/-100.0;
  float glYOffset =   (float)YOffset/-100.0;
  float glZOffset =   -1.5;//(float)ZOffset/-100.0;

  MMToScreen /= 100.0;    // Milimeter

  float urcfront  = gObjectManager->MinMaxfront  / 100.0;;
  float urcback   = gObjectManager->MinMaxback   / 100.0;;
  float urcleft   = gObjectManager->MinMaxleft   / 100.0;
  float urcright  = gObjectManager->MinMaxright  / 100.0;
  float urcbottom = gObjectManager->MinMaxbottom / 100.0;
  float urctop    = gObjectManager->MinMaxtop    / 100.0;
  float Unterlage[8][3]= {
    {urcleft,urctop,urcfront},
    {urcright,urctop,urcfront},
    {urcright,urcbottom,urcfront},
    {urcleft,urcbottom,urcfront},
    {urcleft,urctop,urcback},
    {urcright,urctop,urcback},
    {urcright,urcbottom,urcback},
    {urcleft,urcbottom,urcback}
  };






  int FocusIndex =-1;
  int ArrayIndex =-1;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glGetIntegerv (GL_VIEWPORT, viewport );

  if (gLookClip) {
    gluPerspective(Perspective,(float)viewport[2]/(float)viewport[3], 1, 13.0);
  } else {
    gluPerspective(Perspective,(float)viewport[2]/(float)viewport[3], 0.0001,5000.0);
  }

  glMatrixMode(GL_MODELVIEW);
  float backg[4];
  backg[0] = backg[1] = backg[2] = gBackground; //LightColor[2]/2.0;
  glClearColor(backg[0],backg[1],backg[2],1);

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_NORMALIZE);
  glLoadIdentity();

  glTranslatef(0.0,0.0,-1.5);

  float Zoom2 = (Perspective / 18) / MMToScreen;

  glFaktor = 1.0 / 100.0;
  glScalef(Zoom2, Zoom2, Zoom2);

  gDreiDFace=0;

  int clr;
  int SelCnt=0;
  int ActicCnt=0;
  int ActArt=-1;
  int bGriff = (Fl::event_state() & (FL_CTRL|FL_SHIFT) && gDreiDFace==0);

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);

  float direction[] = { 1.0, 1.0, 1.0, 0.0 };
  direction[0] = LightPos[0]*MMToScreen;
  direction[1] = LightPos[1]*MMToScreen;
  direction[2] = LightPos[2]*MMToScreen;
  glLightfv(GL_LIGHT0, GL_POSITION, direction);


  if (1) {
    int iName=0;
    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);
      while (pOb) {
        if ((inc.m_ObjectManager->m_AktIndex < inc.m_ObjectManager->m_3DreiTafelProjektionStartcnt
             && (pOb->Art & StlObject)==0 && pOb->m_Konstruktion==VERARBEITEDT)) {
          if (i3DreiTafel==100 || gGrayOut > 0.20) {
            clr = pOb->GetRGBColor();
#ifdef MOUSESERVICE
            if (gDreiDFace==0) {
              iName++;
              glLoadName((int)iName);
              if (0 && bGriff) {
                CPolyLineObject * pObL    =(CPolyLineObject*) pOb;
                glLineWidth(2);
                /*
                float xs,ys,zs;
                ys = glYCNCToScreen(pObL->m_S.Y);// m_Path[0].Y);
                xs = glXCNCToScreen(pObL->m_S.X);//m_Path[0].X);
                zs = glZCNCToScreen(pObL->m_S.Z);//m_Path[0].Z);

                float rs = 0.005;
                if (closest==iName) glColor4f (1.,0, 1.,0.5);
                else glColor4f (0.7,0, 0.7,0.5);
                if (bGriff == FL_CTRL) {
                  glBegin (GL_LINES);
                  glVertex3f(xs,ys-rs/2,zs);
                  glVertex3f(xs,ys+rs/2,zs);
                  glVertex3f(xs+rs/2,ys,zs);
                  glVertex3f(xs-rs/2,ys,zs);
                  glEnd ();
                }
                if (bGriff == FL_SHIFT) {
                  glBegin (GL_LINES);
                  glVertex3f(xs+rs/2,ys,zs);
                  glVertex3f(xs-rs/2,ys,zs);
                  glEnd ();
                }
                {
                  glBegin (GL_QUADS);
                  glColor4f (1,1, 1,0.2);
                  zs-=0.001;
                  //glBegin (GL_LINE_LOOP );
                  glVertex3f(xs-rs,ys+rs,zs);
                  glVertex3f(xs+rs,ys+rs,zs);
                  glVertex3f(xs+rs,ys-rs,zs);
                  glVertex3f(xs-rs,ys-rs,zs);
                  glEnd ();
                }*/
                glLoadName(0);
              }
              if (iName) {
                if (closest==iName) {  // Mouse Cklic found
                  if ((Fl::event_state() & FL_CTRL)) {
                    pOb->SetSelect(Sel);
                  } else if ((Fl::event_state() & FL_SHIFT)) {
                    pOb->SetSelect(0);
                  } else if (!pOb->IsSelected()) {
                    pOb->SetSelect(Sel);
                  } else {
                    pOb->SetSelect(0);
                  }
                  clr = pOb->GetRGBColor();
                  closest=-1;
                  damage(FL_DAMAGE_OVERLAY);
                } else if (0) {
                  for (int hi=0; hi <NumHits; hi++) {
                    if (selectBuf[(hi - 1) * 4 + 3]==iName) {
                      clr = 0xF7F7F7;
                    }
                  }
                }
              }
            }
#endif
            if (gDreiDFace ==0 && i3DreiTafel < 100) {
              glColor4f(gGrayOut,gGrayOut,gGrayOut,0.50);
            } else {
              glColor4f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);
            }
            glLineWidth(pOb->GetWitdh());
            if (1||pOb->Is3DFace() != ActArt) {
              if (1||pOb->Is3DFace()) {
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);

                float black[] = { 0.0, 0.0, 0.0, 1.0 };
                float yellow[] = { 1.0, 1.0, 0.95, 1.0 };
                float cyan[] = { 0.0, 1.0, 1.0, 1.0 };
                float white[] = { 1.0, 1.0, .8, 1.0 };
                //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                //glMaterialf(GL_FRONT, GL_SHININESS, 30);
                /*            glLightfv(GL_LIGHT0, GL_AMBIENT, black);
                            glLightfv(GL_LIGHT0, GL_DIFFUSE,  yellow);
                            glLightfv(GL_LIGHT0, GL_SPECULAR, white);
                */
                float direction[] = { 1.0, 1.0, 1.0, 0.0 };
                direction[0] = LightPos[0];
                direction[1] = LightPos[1];
                direction[2] = LightPos[2];

//            glLightfv(GL_LIGHT0, GL_POSITION, direction);

//            glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
                white[0] = white[1] = white[2] = LightColor[2];
                //white[0] += (LightColor[1]-1.0)/3.0;
                //white[2] -= (LightColor[1]-1.0)/3.0;
                glLightfv(GL_LIGHT0, GL_DIFFUSE, white);// LightColor);
                //white[0] = white[1] = white[2] = LightColor[1];
                //glLightfv(GL_LIGHT0, GL_SPECULAR,  white);
                white[0] = white[1] = white[2] = LightColor[0];
                //white[0] += (LightColor[1]-1.0)/3.0;
                //white[2] -= (LightColor[1]-1.0)/3.0;
                glLightfv(GL_LIGHT0, GL_AMBIENT, white);

              } else {
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
              }
              ActArt = pOb->Is3DFace();

            }
            pOb->ExecuteDrawObject((Mode == Linie||Mode==SavePolygon)?Mode:Draw);
          }
        }
        pOb =  inc.m_ObjectManager->GetNext();
      }
    }
  }
  if (1) {
    if (bArrangeZoomCenter) {
      int MinMaxleft   = 0x7FFFFFFF;
      int MinMaxright  = 0x80000000;
      int MinMaxtop    = 0x7FFFFFFF;
      int MinMaxbottom = 0x80000000;
      int MinMaxfront  = 0x7FFFFFFF;
      int MinMaxback   = 0x80000000;

      for (int i =0; i < IncludeList.size(); i++) {
        IncludeType inc;
        inc = IncludeList[i];
        CObject * pTestOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);
//   int saveIndex = inc.m_ObjectManager->m_AktIndex ;
        while (pTestOb) {

          if (inc.m_ObjectManager->m_AktIndex > inc.m_ObjectManager->m_3DreiTafelProjektionStartcnt || ((pTestOb->Art & StlObject)==StlObject)) {
            if      (pTestOb->m_Xmin < MinMaxleft  )   MinMaxleft   = pTestOb->m_Xmin;
            else if (pTestOb->m_Xmax > MinMaxright )   MinMaxright  = pTestOb->m_Xmax;
            if      (pTestOb->m_Ymin < MinMaxtop   )   MinMaxtop    = pTestOb->m_Ymin;
            else if (pTestOb->m_Ymax > MinMaxbottom)   MinMaxbottom = pTestOb->m_Ymax;
            if      (pTestOb->m_Zmin < MinMaxfront )   MinMaxfront  = pTestOb->m_Zmin;
            else if (pTestOb->m_Zmax > MinMaxback  )   MinMaxback   = pTestOb->m_Zmax;
          }
          pTestOb =  inc.m_ObjectManager->GetNext();
        }
      }

      //inc.m_ObjectManager->m_AktIndex = saveIndex;
      //inc.m_ObjectManager->m_Index = inc.m_ObjectManager->m_AktIndex;

      // Calculate the faktor to fit gl-Objects in the Range -1.0 to 1.0
      XMax = MinMaxright -  MinMaxleft;
      Max  = MinMaxbottom - MinMaxtop;
      float ZMax = MinMaxback-MinMaxfront;
//      XOffset  =
      XOffset2  = (MinMaxright+MinMaxleft)/2.0;
//      YOffset  =
      YOffset2  = (MinMaxbottom+MinMaxtop)/2.0;
//      ZOffset  =
      ZOffset2  = (MinMaxback+MinMaxfront)/2.0;
      /*      glFaktor = glFaktor2 = 10;
            if ((XMax / Max) > (W/H)) {
              Max = XMax * (H/W);
            }
            if (ZMax > Max) Max = ZMax;
            glFaktor2 = glFaktor = 0.45 / Max;
            Zoom = 1.0;
      */


      bArrangeZoomCenter=0;
    } //else
    {
      XOffset    = 0;//XOffset2;
      YOffset    = 0;//YOffset2;
      ZOffset    = 0;//ZOffset2;
      //glFaktor   = glFaktor2;
    }
    glXOffset =   (float)XOffset2/-100.0;
    glYOffset =   (float)YOffset2/-100.0;
    glZOffset =   (float)ZOffset2/-100.0;

    glScalef(Zoom, Zoom, Zoom);
    gDreiDFace = gbFillPolygon;//gPerspektive->IsDlgButtonChecked(IDC_Show3DFace);
    glTranslatef(center[0]*-1.0*MMToScreen,center[1]*-1.0*MMToScreen,center[2]*MMToScreen);// );
    glRotatef(RotateVal[1],0.0,1.0,0.0);
    glRotatef(RotateVal[0],1.0,0.0,0.0);
    glRotatef(RotateVal[2],0.0,0.0,1.0);
    i3DreiTafel=100;
    glTranslatef(glXOffset,glYOffset,glZOffset );   // Center Rotate

    if (Unterlage[0][0] < Unterlage[1][0]) {


      /*    glColor4f(.3,0.3,0.3,1);

      glBegin(GL_LINE_LOOP);
      //glBegin(GL_POLYGON);
      glNormal3f(0,0,1);
      glVertex3fv(Unterlage[0]);
      glVertex3fv(Unterlage[1]);
      glVertex3fv(Unterlage[2]);
      glVertex3fv(Unterlage[3]);
      glEnd();
      */

      /*
      float r   = 0.10;
      float g   = 0.20;
      float b   = 0.0;

      GLfloat mat_diffuse[4];
      mat_diffuse[0] = r;
      mat_diffuse[1] = g;
      mat_diffuse[2] = b;



      mat_diffuse[3] = 0.2;

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
      glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
      glMaterialf(GL_FRONT, GL_SHININESS,LightColor[2]*20.0);
      */
      glColor4f(.3,0.3,0.3,0.5);

      glBegin(GL_LINE_LOOP);
      //glBegin(GL_POLYGON);
      glNormal3f(0,0,1);
      glVertex3fv(Unterlage[0]);
      glVertex3fv(Unterlage[1]);
      glVertex3fv(Unterlage[2]);
      glVertex3fv(Unterlage[3]);
      glEnd();

      //glColor4f(.1,0.3,0.3,0.1);

      glBegin(GL_LINE_LOOP);
      glNormal3f(0,0,-1);
      glVertex3fv(Unterlage[4]);
      glVertex3fv(Unterlage[5]);
      glVertex3fv(Unterlage[6]);
      glVertex3fv(Unterlage[7]);
      glEnd();


      //glColor4f(.1,0.5,0.1,0.1);
      glBegin(GL_LINE_LOOP);
      glNormal3f(-1,0,0);
      glVertex3fv(Unterlage[0]);
      glVertex3fv(Unterlage[3]);
      glVertex3fv(Unterlage[7]);
      glVertex3fv(Unterlage[4]);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glNormal3f(1,0,0);
      glVertex3fv(Unterlage[1]);
      glVertex3fv(Unterlage[2]);
      glVertex3fv(Unterlage[6]);
      glVertex3fv(Unterlage[5]);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glNormal3f(0,1,0);
      glVertex3fv(Unterlage[0]);
      glVertex3fv(Unterlage[1]);
      glVertex3fv(Unterlage[5]);
      glVertex3fv(Unterlage[4]);
      glEnd();
      glBegin(GL_LINE_LOOP);
      glNormal3f(0,-1,0);
      glVertex3fv(Unterlage[2]);
      glVertex3fv(Unterlage[3]);
      glVertex3fv(Unterlage[7]);
      glVertex3fv(Unterlage[6]);
      glEnd();




    }

    int iName=0;
    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pOb =  inc.m_ObjectManager->GetFirst(NULL,-1);
      while (pOb) {
        if (inc.m_ObjectManager->m_AktIndex >= inc.m_ObjectManager->m_3DreiTafelProjektionStartcnt
            || pOb->m_Konstruktion!=VERARBEITEDT
            || ((pOb->Art & StlObject)==StlObject)) {

          if (i3DreiTafel==100 || gGrayOut > 0.20) {
            clr = pOb->GetRGBColor();
#ifdef MOUSESERVICE
            if (gDreiDFace==0) {
              iName++;
              glLoadName((int)iName);
              if (0 && bGriff) {
                CPolyLineObject * pObL    =(CPolyLineObject*) pOb;
                glLineWidth(2);
                /*
                float xs,ys,zs;
                ys = glYCNCToScreen(pObL->m_S.Y);// m_Path[0].Y);
                xs = glXCNCToScreen(pObL->m_S.X);//m_Path[0].X);
                zs = glZCNCToScreen(pObL->m_S.Z);//m_Path[0].Z);

                float rs = 0.005;
                if (closest==iName) glColor4f (1.,0, 1.,0.5);
                else glColor4f (0.7,0, 0.7,0.5);
                if (bGriff == FL_CTRL) {
                  glBegin (GL_LINES);
                  glVertex3f(xs,ys-rs/2,zs);
                  glVertex3f(xs,ys+rs/2,zs);
                  glVertex3f(xs+rs/2,ys,zs);
                  glVertex3f(xs-rs/2,ys,zs);
                  glEnd ();
                }
                if (bGriff == FL_SHIFT) {
                  glBegin (GL_LINES);
                  glVertex3f(xs+rs/2,ys,zs);
                  glVertex3f(xs-rs/2,ys,zs);
                  glEnd ();
                }
                {
                  glBegin (GL_QUADS);
                  glColor4f (1,1, 1,0.2);
                  zs-=0.001;
                  //glBegin (GL_LINE_LOOP );
                  glVertex3f(xs-rs,ys+rs,zs);
                  glVertex3f(xs+rs,ys+rs,zs);
                  glVertex3f(xs+rs,ys-rs,zs);
                  glVertex3f(xs-rs,ys-rs,zs);
                  glEnd ();
                }*/
                glLoadName(0);
              }
              if (iName) {
                if (closest==iName) {  // Mouse Cklic found
                  if ((Fl::event_state() & FL_CTRL)) {
                    pOb->SetSelect(Sel);
                  } else if ((Fl::event_state() & FL_SHIFT)) {
                    pOb->SetSelect(0);
                  } else if (!pOb->IsSelected()) {
                    pOb->SetSelect(Sel);
                  } else {
                    pOb->SetSelect(0);
                  }
                  clr = pOb->GetRGBColor();
                  closest=-1;
                  damage(FL_DAMAGE_OVERLAY);
                } else if (0) {
                  for (int hi=0; hi <NumHits; hi++) {
                    if (selectBuf[(hi - 1) * 4 + 3]==iName) {
                      clr = 0xF7F7F7;
                    }
                  }
                }
              }
            }
#endif
            //if (gDreiDFace ==0 && i3DreiTafelLayer < 100) {
            //  glColor4f(gGrayOut,gGrayOut,gGrayOut,0.50);
            //} else {
            //  glColor4f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);
            //}
            glLineWidth(pOb->GetWitdh());
            if (1||pOb->Is3DFace() != ActArt) {
              if (pOb->Is3DFace()) {
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
//            glEnable(GL_LIGHT1);
                ///      glEnable(GL_LIGHT2);
                //   glEnable(GL_LIGHT3);


                float black[] = { 0.0, 0.0, 0.0, 1.0 };
                float yellow[] = { 1.0, 1.0, 0.95, 1.0 };
                float cyan[] = { 0.0, 1.0, 1.0, 1.0 };
                float white[] = { 1.0, 1.0, .8, 1.0 };
                float direction[] = { 1.0, 1.0, 1.0, 0.0 };
                //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                //glMaterialf(GL_FRONT, GL_SHININESS, 30);
                /*            glLightfv(GL_LIGHT0, GL_AMBIENT, black);
                            glLightfv(GL_LIGHT0, GL_DIFFUSE,  yellow);
                            glLightfv(GL_LIGHT0, GL_SPECULAR, white);
                */
                direction[0] = LightPos[0];
                direction[1] = LightPos[1];
                direction[2] = LightPos[2];

                glLightfv(GL_LIGHT0, GL_POSITION, direction);

//            glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
                white[0] = white[1] = white[2] = LightColor[2];
                //white[0] += (LightColor[1]-1.0)/3.0;
                //white[2] -= (LightColor[1]-1.0)/3.0;
                glLightfv(GL_LIGHT0, GL_DIFFUSE, white);// LightColor);
                //white[0] = white[1] = white[2] = LightColor[1];
                //glLightfv(GL_LIGHT0, GL_SPECULAR,  white);
                white[0] = white[1] = white[2] = LightColor[0];
                //white[0] += (LightColor[1]-1.0)/3.0;
                //white[2] -= (LightColor[1]-1.0)/3.0;
                glLightfv(GL_LIGHT0, GL_AMBIENT, white);

              } else {
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
              }
              ActArt = pOb->Is3DFace();

            }
            pOb->ExecuteDrawObject((Mode == Linie||Mode==SavePolygon)?Mode:Draw);
          }
        }
        pOb =  inc.m_ObjectManager->GetNext();
      }
    }
  }
  norecursion--;
#endif
  return 0;
}
//-------------------------------------------------------------

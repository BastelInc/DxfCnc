//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "FingerScroll.h"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "BastelUtils.h"
#include "GlDisplay.h"
//#include "Resource.h"
#ifdef WIN32
#include "TouchLib.h"
#endif

#include <math.h>

//#define TOUCHHOOK
#define GRAD360 57.2957795131f

CFingerScroll::CFingerScroll(int X, int Y, int W, int H,const char *L):Fl_Scroll(X,Y,W,H,L)
{
  type (2);
  //begin();
  m_pack  = new Fl_Pack(x(),y(),w(),h(),NULL);
  m_pack->type(0);
  m_pack ->box(FL_NO_BOX);//FL_DOWN_FRAME);
  m_pack->spacing(0);
//  end();

}

CFingerScroll::~CFingerScroll()
{

}



//--------------------------------------------------------------
static int scroll_start  = 0;
static int scroll_min  = 0;
static int scroll_max  = 0;
static int start_fingerx = 0;
static int start_fingery = 0;
static unsigned int startTickCount;
static int kpix_per_ms;
static int fingermovecnt = 0;
//-------------------------------------------------------------
void FingerScrollCb(void *data)
{
  CFingerScroll * pFingerScroll = (CFingerScroll*)data;
  if (fingermovecnt) {
    if (fingermovecnt>0) {
      fingermovecnt--;
      scroll_start -= (kpix_per_ms / 100);
      if (scroll_start < scroll_min) {
        scroll_start=scroll_min;
        fingermovecnt=0;
      }
    } else if (fingermovecnt< 0) {
      fingermovecnt++;
      scroll_start -= (kpix_per_ms / 100);
      if (scroll_start > scroll_max) {
        scroll_start=scroll_max;
        fingermovecnt=0;
      }
    }
    pFingerScroll->scroll_to(0,scroll_start);
    Fl::repeat_timeout(0.05, FingerScrollCb, data);
  }
}

//-------------------------------------------------------------

int CFingerScroll::handle(int event)
{
  static int validMove;
  if (event == FL_NO_EVENT) return(0);
//  if (event == FL_FOCUS) return(1);
//  if (event == FL_UNFOCUS) return(1);
  if (Fl::event_x()+scrollbar.w() > w()) {
    return Fl_Scroll::handle(event);
  }
  switch (event) {
//    case FL_FOCUS:
//    case FL_UNFOCUS:
//      return 1;
  case FL_PUSH : {
    //Fl::focus(this);
    fingermovecnt = 0;
    scroll_start  = scrollbar.value();
    scroll_min    = 0;
    scroll_max    = m_pack->h()-h();
    start_fingerx = Fl::event_x();
    start_fingery = Fl::event_y();
    int t = GetTickCount();
    validMove = (t > startTickCount+600);
    startTickCount= t;
    //return
    //Fl_Scroll::handle(event);
    //return Fl_Scroll::handle(event);
    return (1);
  }
  case FL_DRAG :
    if (validMove && Fl::event_button()== FL_LEFT_MOUSE) {
      int valy = Fl::event_y()-start_fingery;
      scroll_to(0,scroll_start-valy);
    }
    return (1);
    break;
  /*  case FL_MOVE:
  break;*/
  case FL_RELEASE : {
    int valx = Fl::event_x()-start_fingerx;
    int valy = Fl::event_y()-start_fingery;
    if (fingermovecnt) {
      fingermovecnt=0;      // Stop
    }
    if (validMove ) {
      if (abs (valx) > abs(valy)) {
        if (valx > 50) {
          //tMessage * pmsg = (tMessage *)malloc(sizeof(tMessage ));
          //pmsg->Code = IDM_PlayListTab;
          //pmsg->Value= 0;
          //pmsg->receiver = gRadioBerry;
          //Fl::awake(pmsg);
        }
      } else {
        if (valy < 10 && valy > -10) {
          //CTVItem * p = GetItem(Fl::event_y()-y());
          //if (p) {
          //  int ix = FindItemIndex(p);
          //  OnClick(p->m_Item_ID,0,Fl::event_x()-x());
          //  SelChanged(ix);
          //  p->redraw();
          //}

          Fl_Scroll::handle(FL_PUSH);
          Fl_Scroll::handle(event);
          return (0);
        } else {
          int t = (GetTickCount() - startTickCount);
          if (t < 1000) {
            int val = Fl::event_y()-start_fingery;
            scroll_start  = scrollbar.value();
            if (t) {
              kpix_per_ms = val*1000 / t;
              Fl::repeat_timeout(0.01, FingerScrollCb, this);
              fingermovecnt = (val * abs(val)) / (t);
            }
          }
        }
      }
    } else {
      startTickCount=0;
      //CTVItem * p = GetItem(Fl::event_y()-y());
      //if (p) {
      //  int ix = FindItemIndex(p);
      //  OnClick(p->m_Item_ID,1,Fl::event_x()-x());
      //  p->redraw();
      //}
      return 0;
    }
    if (Fl::event_button() == FL_RIGHT_MOUSE) {
    }

    return 1;
  }
  break;
  case FL_MOUSEWHEEL: {
    scroll_start  = scrollbar.value();
    scroll_to(0,scroll_start-Fl::event_dy()*10);
  }
  break;
    /*  case FL_KEYBOARD:
    {
    int ekey = Fl::event_key();
    if ( ekey == FL_Right) // && item_focus->is_close() )
    {
    return  1;
    }
    else if ( ekey == FL_Left)
    {
    return  1;
    }
    }*/
  }
  return Fl_Scroll::handle(event);
}

//-------------------------------------------------------------

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include <pthread.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <linux/input.h>
#endif


/*
Type
#define EV_SYN			0x00
#define EV_KEY			0x01
#define EV_REL			0x02
#define EV_ABS			0x03
#define EV_MSC			0x04
#define EV_SW			0x05
#define EV_LED			0x11
#define EV_SND			0x12
#define EV_REP			0x14
#define EV_FF			0x15
#define EV_PWR			0x16
#define EV_FF_STATUS		0x17
#define EV_MAX			0x1f
#define EV_CNT			(EV_MAX+1)

code

// Misc events

#define MSC_SERIAL		0x00
#define MSC_PULSELED		0x01
#define MSC_GESTURE		0x02
#define MSC_RAW			0x03
#define MSC_SCAN		0x04
#define MSC_TIMESTAMP		0x05
#define MSC_MAX			0x07
#define MSC_CNT			(MSC_MAX+1)

#define BTN_TOUCH		0x14a

#define ABS_MT_SLOT		0x2f	// MT slot being modified
#define ABS_MT_TOUCH_MAJOR	0x30	// Major axis of touching ellipse
#define ABS_MT_TOUCH_MINOR	0x31	// Minor axis (omit if circular)
#define ABS_MT_WIDTH_MAJOR	0x32	// Major axis of approaching ellipse
#define ABS_MT_WIDTH_MINOR	0x33	// Minor axis (omit if circular)
#define ABS_MT_ORIENTATION	0x34	// Ellipse orientation
#define ABS_MT_POSITION_X	0x35	// Center X touch position
#define ABS_MT_POSITION_Y	0x36	// Center Y touch position
#define ABS_MT_TOOL_TYPE	0x37	// Type of touching device
#define ABS_MT_BLOB_ID		0x38	// Group a set of packets as a blob
#define ABS_MT_TRACKING_ID	0x39	// Unique ID of initiated contact
#define ABS_MT_PRESSURE		0x3a	// Pressure on contact area
#define ABS_MT_DISTANCE		0x3b	// Contact hover distance
#define ABS_MT_TOOL_X		0x3c	// Center X tool position
#define ABS_MT_TOOL_Y		0x3d	// Center Y tool position

  */
#define SHOW_ABS_MT_INFO 0
#undef  GRAFIKTABLETT

#if SHOW_ABS_MT_INFO
const char * rgszCode[15] = {
  "ABS_MT_SLOT",
  "ABS_MT_TOUCH_MAJOR",
  "ABS_MT_TOUCH_MINOR",
  "ABS_MT_WIDTH_MAJOR",
  "ABS_MT_WIDTH_MINOR",
  "ABS_MT_ORIENTATION",
  "ABS_MT_POSITION_X",
  "ABS_MT_POSITION_Y",
  "ABS_MT_TOOL_TYPE",
  "ABS_MT_BLOB_ID",
  "ABS_MT_TRACKING_ID",
  "ABS_MT_PRESSURE",
  "ABS_MT_DISTANCE",
  "ABS_MT_TOOL_X",
  "ABS_MT_TOOL_Y"
};

const char * rgszTouchState[6]= { "StartTouch","StartZomm","ProcessZoom","StartRot","ProcessRot","ProcessRot2"};
#endif

#ifndef WIN32
//int m_thread;
pthread_t m_thread;
static void * gMouse_thread(void *dummy)
{

  char str[200];
  input_event input_event[64];
  int                readlen;
  //BOOL tuchpad;
  int fd = -1;
  int XFactor = 0x7FFF;
  int YFactor = 0x7FFF;

  struct pollfd pollfds[20];
  int numMouse=0;
  int i;

  pthread_detach(pthread_self());
  for (i=0; i <26 && numMouse < 20; i++) {
    sprintf(str,"/dev/input/event%d",i);
    fd = open( str, O_RDONLY|O_NONBLOCK);
    if (fd >= 0) {
      // find the Key and Mouse Events
      unsigned long evbit;
      ioctl( fd, EVIOCGBIT(0, sizeof(evbit)), &evbit );//>=0)

#ifdef USE_LINUX_INPUT_KEY
      if ((((evbit >> (EV_KEY)) & 1)&&m_Keyboard)
          ||((evbit >> (EV_REL)) & 1)
          ||((evbit >> (EV_ABS)) & 1))
#else
      if (((evbit >> (EV_REL)) & 1)
          ||1
          ||((evbit >> (EV_ABS)) & 1))
#endif
      {
        char szdevice[30];
        ioctl( fd, EVIOCGNAME(sizeof(szdevice) - 1), szdevice);
        int ret = 0;
        if (ret==0 || errno == EINVAL) {
          fprintf(stderr,"Input %d %s\n",numMouse,szdevice);
          if ((strcmp(szdevice,"Apple Inc. Magic Trackpad 2")==0)
              ||(strcmp(szdevice,"raspberrypi-ts")==0)) {
            pollfds[numMouse].fd     = fd;
            pollfds[numMouse].events = POLLIN|POLLPRI|POLLERR;
            numMouse++;
          } else if (strcmp(szdevice,"Weida Hi-Tech CoolTouch Syste")==0) {
            XFactor = Fl::w();
            pollfds[numMouse].fd     = fd;
            YFactor = Fl::h();
            pollfds[numMouse].events = POLLIN|POLLPRI|POLLERR;
            numMouse++;
          } else if (strcmp(szdevice,"HANVON UGEE Artist 10 (2nd Ge")==0) {
            XFactor = Fl::w();
            YFactor = Fl::h();
            pollfds[numMouse].fd     = fd;
            pollfds[numMouse].events = POLLIN|POLLPRI|POLLERR;
            numMouse++;
          } else if (strcmp(szdevice,"Melfas LGDisplay Incell Touch")==0) {
            XFactor = Fl::w();
            YFactor = Fl::h();
            pollfds[numMouse].fd     = fd;
            pollfds[numMouse].events = POLLIN|POLLPRI|POLLERR;
            numMouse++;
#ifdef GRAFIKTABLETT
          } else if (strcmp(szdevice,"XP-PEN STYLUS")==0
                     || strcmp(szdevice,"XP-PEN MOUSE")==0
                     || strcmp(szdevice,"XP-PEN ERASER")==0) {
            XFactor = Fl::w();
            YFactor = Fl::h();
            pollfds[numMouse].fd     = fd;
            pollfds[numMouse].events = POLLIN|POLLPRI|POLLERR;
            numMouse++;
#endif
          } else close (fd);
        } else close (fd);
      } else close (fd);
    }
  }

  int MousState=0;
  int loopcounter;
  unsigned int Event=0;
  enum {StartTouch,StartZomm,ProcessZoom,StartRot,ProcessRot,ProcessRot2};
  int TouchState=StartTouch;
  //POINTS pos;    // 32 Bit = one instruction
  POINTS Startpos;
  int    startDist;
  int    startRot;
  int    FingerValue=0;
  float  StartZoom;
  POINTS rgpos[14];
  int    slot=0;
  int KeySymulation=0;
  while (1) { //gAppRun >= AppStateRun)
    poll(pollfds, numMouse, 1);
    loopcounter++;
    Event=0;
    for (int ix=0; ix < numMouse; ix++) {
      if (pollfds[ix].revents & (POLLIN|POLLPRI)) {
        KeySymulation=0;
        int Xaxisrel;
        int Yaxisrel;
        int Zaxisrel;
        Xaxisrel=0;
        Yaxisrel=0;
        Zaxisrel=0;
        extern Fl_Window* fl_xmousewin;
        extern Fl_Double_Window* MainWindow;
#ifndef DREITABLE
        if (fl_xmousewin==MainWindow)
#endif
        {
          readlen = read(pollfds[ix].fd, &input_event, sizeof(input_event) );
          for (int i=0; i<readlen / (int)sizeof(input_event[0]); i++) {
#if SHOW_ABS_MT_INFO
//                fprintf(stderr,"%d input_event %2d type %d val %4d\n",i,input_event[i].type,
//                        rgszCode[ input_event[i].code-ABS_MT_SLOT],input_event[i].value);
#endif

            switch (input_event[i].type) {
            case EV_KEY:
              if ((input_event[i].code >= BTN_MOUSE && input_event[i].code< BTN_JOYSTICK)) {
                // ignore repeat events for buttons
                if (input_event[i].value < 2) {
                  if      (input_event[i].code == BTN_LEFT) {
                    if (input_event[i].value==1) {
                      //if (loopcounter<-3) Event |= EV_LBUTTONDBLCLK;
                      //else Event |= EV_LBUTTONDOWN;
                      //MousState  |= 0x1;
                    } else if (input_event[i].value==2) {
                      //Event |= EV_MOUSEREPEAT;
                    } else  {
                      //MousState  &= ~0x1;
                      //Event |= EV_LBUTTONUP;
                    }
                    loopcounter = - 27;   // 27 x 11 mS
                    //loopcounter = - 18;   // rolf 18 x 11 mS
                  } else if (input_event[i].code == BTN_RIGHT) {
                    //if (input_event[i].value) Event |= EV_RBUTTONDOWN,MousState  |= 0x2;
                    //else                       Event |= EV_RBUTTONUP,  MousState  &= ~0x2;
                  } else if (input_event[i].code == BTN_MIDDLE) {
                    //if (input_event[i].value) Event |= EV_MBUTTONDOWN,MousState  |= 0x4;
                    //else                      Event |= EV_MBUTTONUP  ,MousState  &= ~0x4;;
                  }
                }

              } else if (input_event[i].code == BTN_TOUCH) {
                if (input_event[i].value) {
                  /*
                  if (loopcounter<-3) Event |= EV_LBUTTONDBLCLK;
                  Event |= EV_LBUTTONDOWN;
                  MousState  |= 0x1;
                  } else {
                  Event |= EV_LBUTTONUP;
                  MousState  &= ~0x1;
                    */
                }
                loopcounter = - 27;   // 27 x 11 mS
              }//
              break;
            case EV_ABS: {
              if (input_event[i].code == ABS_X) {
              } else if (input_event[i].code == ABS_Y) {
              } else if (input_event[i].code == ABS_MT_SLOT) {
                slot=input_event[i].value;
                if (slot <14) rgpos[slot].x= 1;
                int slotcnt =0;
                int n=0;
                while (n < 14) {
                  if (rgpos[n].x)  slotcnt++;
                  n++;
                }
#if SHOW_ABS_MT_INFO
                fprintf(stderr,"%s input_event %2d typpe %d code %s val %4d cnt= %d\n",rgszTouchState[TouchState],i,input_event[i].type,
                        rgszCode[ input_event[i].code-ABS_MT_SLOT],input_event[i].value,slotcnt );
#endif

                if (slotcnt==0) {
                } else if (slotcnt==2) {
                  if (TouchState==StartTouch) {
                    TouchState=StartZomm;
                  }
                } else if (slotcnt>=3) {
                  if (TouchState < StartRot) {
                    startRot = gWrk3DSheet->getRotation(1);
                    TouchState=StartRot;
                  }
                }
              } else if (input_event[i].code == ABS_MT_TRACKING_ID) {
                if (input_event[i].value<0) {   // End
                  if (FingerValue>0) FingerValue--;
                  if (FingerValue==0) {
                    gWrk3DSheet->getZoom(0);
                    TouchState =StartTouch;
                    memset(&rgpos,0,sizeof(rgpos));
                    //fprintf(stderr,"ABS_MT_TRACKING_ID val %4d s%d\n",input_event[i].value,TouchState);
                  }
                } else {
                  FingerValue++;
                }
                //fprintf(stderr,"ABS_MT_TRACKING_ID val %4d s%d\n",input_event[i].value,TouchState);
              } else if (input_event[i].code == ABS_MT_POSITION_X) {
                if (slot>=0 && slot < 14) {
                  rgpos[slot].x = input_event[i].value * XFactor / 0x7FFF;
                }
              } else if (input_event[i].code == ABS_MT_POSITION_Y) {
                if (slot>=0 && slot < 14) {
                  rgpos[slot].y = input_event[i].value * YFactor / 0x7FFF;
                }
              } else {
#ifdef GRAFIKTABLETT
                  fprintf(stderr,"Dev %d ",ix);
                if (input_event[i].code == ABS_PRESSURE) {
                  fprintf(stderr,"ABS_PRESSURE val %4d \n",input_event[i].value);
                } else if (input_event[i].code == ABS_DISTANCE) {
                  fprintf(stderr,"DISTANCE val %4d ",input_event[i].value);
                } else if (input_event[i].code == ABS_TILT_X) {
                  //fprintf(stderr,"TILT_X val %4d ",input_event[i].value);
                } else if (input_event[i].code == ABS_TILT_Y) {
                  //fprintf(stderr,"TILT_Y val %4d\n",input_event[i].value);
                } else if (input_event[i].code == ABS_TOOL_WIDTH) {
                  fprintf(stderr,"ABS_TOOL_WIDTH val %4d\n",input_event[i].value);
                } else if (input_event[i].code == ABS_VOLUME) {
                  fprintf(stderr,"ABS_VOLUME val %4d\n",input_event[i].value);
                } else if (input_event[i].code == ABS_MISC) {
                  fprintf(stderr,"ABS_MISC val %4d\n",input_event[i].value);
                } else {
                fprintf(stderr,"%d input_event type %2d code %d val %4d\n",i,input_event[i].type,
                        input_event[i].code,input_event[i].value);
                }
#endif
              }
              //else
#if (SHOW_ABS_MT_INFO == 2)
              if (input_event[i].code >= ABS_MT_SLOT && input_event[i].code<= ABS_MT_TOOL_Y) {
                fprintf(stderr,"%s input_event %2d typpe %d code %s val %4d\n",rgszTouchState[TouchState],i,input_event[i].type,
                        rgszCode[ input_event[i].code-ABS_MT_SLOT],input_event[i].value );
              } else {
                fprintf(stderr,"%s input_event %2d typpe %d code %3x val %4d\n",rgszTouchState[TouchState],i,input_event[i].type,input_event[i].code,input_event[i].value );
              }
#endif
              break;
              case EV_SYN:
                if (FingerValue>=2) {
//                  fprintf(stderr,"SYN_REPORT Start %d\n",TouchState );
                  int first=0;
                  for (; first<14; first++) {
                    if (rgpos[first].x) break;
                  };
                  int second=first+1;
                  for (; second<14; second++) {
                    if (rgpos[second].x) break;
                  };
                  if (rgpos[first].x!=1 && rgpos[second].x!=1) {
                    float H = rgpos[first].x-rgpos[second].x;
                    float V = rgpos[first].y-rgpos[second].y;
                    //if (abs(H) > 10  && abs(V)> 10) {
                    //fprintf(stderr,"ABS_MT_StartCode %d H %d %d V %d %d\n",TouchState,rgpos[first].x,rgpos[second].x,rgpos[first].y,rgpos[second].y);
                    int d = sqrt((H*H)+ (V*V));
                    if (TouchState==StartZomm) {
                      startDist=d;
                      //Start = 102;  // Zoom Mode
                      TouchState=ProcessZoom;
                      StartZoom = gWrk3DSheet->getZoom(1);
                      //fprintf(stderr,"ABS_MT_SLOT val %4d\n",slot);
                    } else  if (TouchState==ProcessZoom) {
                      if (startDist) {
                        int v = d*100/startDist;
                        //fprintf(stderr,"Fc=%d Dist = %d  %d  %0.3f\n",FingerValue,d,startDist,StartZoom);
                        gWrk3DSheet->setZoom(StartZoom* (float)v/100.0);
                      }
                    } else if (TouchState == StartRot) {

                      double winkel = atan2((double)H,(double)V);
                      int w = winkel *GRAD360;
                      if (w <45 || w > 315) {
                        startRot = w-gWrk3DSheet->getRotation2(1);
                        //fprintf(stderr,"Start Rot w %d = %d\n",w,startRot);
                        TouchState=ProcessRot2;
                      } else {
                        startRot = w-gWrk3DSheet->getRotation(1);
                        //fprintf(stderr,"Start Rot w %d = %d\n",w,startRot);
                        TouchState=ProcessRot;
                      }
                    } else if (TouchState==ProcessRot) {
                      double winkel = atan2((double)H,(double)V);
                      int w = winkel *GRAD360;
                      //fprintf(stderr,"Cont Rot w %d = %d\n",w,w-startRot);
                      gWrk3DSheet->setRotation(w-startRot);
                    } else if (TouchState==ProcessRot2) {
                      double winkel = atan2((double)H,(double)V);
                      int w = winkel *GRAD360;
                      //fprintf(stderr,"Cont Rot w %d = %d\n",w,w-startRot);
                      gWrk3DSheet->setRotation2(w-startRot);
                    }
                  }
                  //}
                }
                break;

              }
              break;
            case EV_REL: {
              switch (input_event[i].code) {
              case REL_X:
                Xaxisrel = input_event[i].value;
                break;

              case REL_Y:
                Yaxisrel = input_event[i].value;
                break;

              case REL_Z:
              case REL_WHEEL:
                Zaxisrel = input_event[i].value;
                break;
              default:
                break;
              }
            }
            break;
            default:
              ;
            }
          }
        }
        usleep(10000);
      }
    }
  }

  for (int ix=0; ix < numMouse; ix++) {
    errno=0;
    ioctl( pollfds[ix].fd, EVIOCGRAB, 0 );
    close( pollfds[ix].fd );
  }

  m_thread = 0;
  pthread_exit(0);
  return 0;


}
#else

//typedef BOOL (*tGetTouchInputInfo) (HANDLE hTouchInput, UINT cInputs, rPTOUCHINPUT pInputs, int cbSize);
tGetTouchInputInfo pGetTouchInputInfo=NULL;
//    if (rGetTouchInputInfo((HANDLE)lParam, cInputs, pInputs, sizeof(rTOUCHINPUT))) {
//typedef BOOL (*tCloseTouchInputHandle)(HANDLE hTouchInput);
tCloseTouchInputHandle pCloseTouchInputHandle=NULL;

int  TouchWindow(HWND hwnd,WPARAM wParam, LPARAM lParam)
{
  UINT cInputs;
  PTOUCHINPUT pInputs;
  POINT ptInput;
  enum {StartTouch,StartZomm,ProcessZoom,StartRot,ProcessRot,ProcessRot2};
  static int TouchState=StartTouch;
  static POINTS Startpos;
  static int    startDist;
  static int    startRot;
  static int    FingerValue=0;
  static float  StartZoom;

  cInputs = LOWORD(wParam);

  //fprintf(stderr,"TouchWindow= %d ",wParam);
  pInputs = new TOUCHINPUT[cInputs];
  if (pInputs && pGetTouchInputInfo) {
    if (pGetTouchInputInfo((HANDLE)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
      for (int i=0; i < static_cast<int>(cInputs); i++) {
        TOUCHINPUT ti = pInputs[i];

        if (ti.dwID != 0) {
          ptInput.x = ti.x / 100;
          ptInput.y = ti.y / 100;
          ScreenToClient(hwnd, &ptInput);
          //fprintf(stderr," X %d  y%d dwFlags %02x"/ ",ptInput.x,ptInput.y,ti.dwFlags );
          if (ti.dwFlags & TOUCHEVENTF_MOVE) { // Neuer Finger dazugekommen
            if (cInputs==2) {
              //fprintf(stderr,"Start 2 Finger  X %d  y%d / ",ptInput.x,ptInput.y);
              TouchState=StartZomm;
            } else if (cInputs==3) {
              //fprintf(stderr,"Start 3 Finger  X %d  y%d / ",ptInput.x,ptInput.y);
              if (TouchState < StartRot) {
                startRot = gWrk3DSheet->getRotation(1);
                TouchState=StartRot;
              }
            }
          } else if (ti.dwFlags & TOUCHEVENTF_DOWN) {
            if (ti.dwFlags & TOUCHEVENTF_INRANGE) {
              if (i==1) {   // 2 Punkte vorhanden
                int H = (pInputs[0].x - pInputs[1].x)/100;
                int V = (pInputs[0].y - pInputs[1].y)/100;
                int d = sqrt((double)(H*H)+ (double)(V*V));
                if (TouchState==StartZomm) {
                  startDist=d;
                  TouchState=ProcessZoom;
                  StartZoom = gWrk3DSheet->getZoom(1);
                } else  if (TouchState==ProcessZoom) {
                  if (startDist) {
                    int v = d*100/startDist;
                    gWrk3DSheet->setZoom(StartZoom* (float)v/100.0);
                  }
                } else if (TouchState == StartRot) {
                  double winkel = atan2((double)H,(double)V);
                  int w = winkel *GRAD360;
                  if (w <45 || w > 315) {
                    startRot = w-gWrk3DSheet->getRotation2(1);
                    //fprintf(stderr,"Start Rot w %d = %d\n",w,startRot);
                    TouchState=ProcessRot2;
                  } else {
                    startRot = w-gWrk3DSheet->getRotation(1);
                    //fprintf(stderr,"Start Rot w %d = %d\n",w,startRot);
                    TouchState=ProcessRot;
                  }
                } else if (TouchState==ProcessRot) {
                  double winkel = atan2((double)H,(double)V);
                  int w = winkel *GRAD360;
                  //fprintf(stderr,"Cont Rot w %d = %d\n",w,w-startRot);
                  gWrk3DSheet->setRotation(w-startRot);
                } else if (TouchState==ProcessRot2) {
                  double winkel = atan2((double)H,(double)V);
                  int w = winkel *GRAD360;
                  //fprintf(stderr,"Cont Rot w %d = %d\n",w,w-startRot);
                  gWrk3DSheet->setRotation2(w-startRot);
                }
              }
            }
          } else if (ti.dwFlags & TOUCHEVENTF_UP) {
            if (i< 2) {
              gWrk3DSheet->getZoom(0);
              TouchState =StartTouch;
            }
          } else {
          }
        }
      }
      pCloseTouchInputHandle((HANDLE)lParam);
    }
    delete [] pInputs;
  }
  return 0;
}


HHOOK hhook;
#ifndef WM_TABLET_FIRST
#define WM_TABLET_FIRST 0x02C0
#endif
#define TABLET_DISABLE_PRESSANDHOLD        0x00000001
#define TABLET_DISABLE_PENTAPFEEDBACK      0x00000008
#define TABLET_DISABLE_PENBARRELFEEDBACK   0x00000010
#define TABLET_DISABLE_TOUCHUIFORCEON      0x00000100
#define TABLET_DISABLE_TOUCHUIFORCEOFF     0x00000200
#define TABLET_DISABLE_TOUCHSWITCH         0x00008000
#define TABLET_DISABLE_FLICKS              0x00010000
#define TABLET_ENABLE_FLICKSONCONTEXT      0x00020000
#define TABLET_ENABLE_FLICKLEARNINGMODE    0x00040000
#define TABLET_DISABLE_SMOOTHSCROLLING     0x00080000
#define TABLET_DISABLE_FLICKFALLBACKKEYS   0x00100000
#define TABLET_ENABLE_MULTITOUCHDATA       0x01000000


#define WM_TABLET_QUERYSYSTEMGESTURESTATUS WM_TABLET_FIRST+12

#ifdef TOUCHHOOK

#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A
/*typedef struct tagGESTURENOTIFYSTRUCT {
  UINT   cbSize;
  DWORD  dwFlags;
  HWND   hwndTarget;
  POINTS ptsLocation;
  DWORD  dwInstanceID;
} GESTURENOTIFYSTRUCT, *PGESTURENOTIFYSTRUCT;


typedef struct tagGESTUREINFO {
  UINT      cbSize;
  DWORD     dwFlags;
  DWORD     dwID;
  HWND      hwndTarget;
  POINTS    ptsLocation;
  DWORD     dwInstanceID;
  DWORD     dwSequenceID;
  ULONGLONG ullArguments;
  UINT      cbExtraArgs;
} GESTUREINFO, *PGESTUREINFO;
*/
#define HGESTUREINFO  HANDLE

typedef BOOL (*tGetGestureInfo)(HGESTUREINFO hGestureInfo,PGESTUREINFO pGestureInfo);
tGetGestureInfo pGetGestureInfo=NULL;

typedef (*tCloseGestureInfoHandle)(HGESTUREINFO hGestureInfo);
tCloseGestureInfoHandle pCloseGestureInfoHandle=NULL;
int MouseProc(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{
  static int geste = 0;
  if (uMsg== WM_GESTURE) {
    geste = wParam;
    GESTUREINFO * ginfo  = (GESTUREINFO*) lParam;
    //fprintf(stderr,"WM_GESTURE wp %d lp ox%08X\n",wParam,lParam);
    GESTUREINFO gi;

    ZeroMemory(&gi, sizeof(GESTUREINFO));

    gi.cbSize = sizeof(GESTUREINFO);
    if (pGetGestureInfo && pCloseGestureInfoHandle) {
      BOOL bResult  = pGetGestureInfo((HGESTUREINFO)lParam, &gi);
      //fprintf(stderr,"WM_GESTURE wp %d x%d y%d %08x %08x\n",gi.dwID,gi.ptsLocation.x,gi.ptsLocation.y,(int)(gi.ullArguments>>32),(int)gi.ullArguments);
      fprintf(stderr,"WM_GESTURE wp %d x%d y%d %d\n",gi.dwID,gi.ptsLocation.x,gi.ptsLocation.y,1000+(int)gi.ullArguments);
      pCloseGestureInfoHandle((HGESTUREINFO)lParam);
    }
  } else if (uMsg== WM_GESTURENOTIFY) {
    geste = 0;
    GESTURENOTIFYSTRUCT * gn = (GESTURENOTIFYSTRUCT *) lParam;
    if (gn->ptsLocation.x == gn->ptsLocation.x ) {
      gn->ptsLocation.x ++;
    }
    fprintf(stderr,"WM_GESTURENOTIFY x%d y%d\n",gn->ptsLocation.x,gn->ptsLocation.y);
  } else if (uMsg== WM_TABLET_QUERYSYSTEMGESTURESTATUS) {

    return TABLET_ENABLE_FLICKSONCONTEXT  | TABLET_ENABLE_FLICKLEARNINGMODE| TABLET_ENABLE_MULTITOUCHDATA;//TABLET_DISABLE_FLICKS;
  } else fprintf(stderr,"MouseProc 0x%04X %d  %d \n",uMsg,wParam,lParam);
  return 0;


}
#endif
#endif

void InitFingerTouchService(void)
{
#ifdef WIN32
#ifdef TOUCHHOOK

  pGetGestureInfo=(tGetGestureInfo)GetProcAddress(GetModuleHandle("user32.dll"), "GetGestureInfo");
  pCloseGestureInfoHandle=(tCloseGestureInfoHandle)GetProcAddress(GetModuleHandle("user32.dll"), "CloseGestureInfoHandle");


  typedef int (*WM_TouchHookCB)(HWND,UINT,WPARAM, LPARAM);
  extern WM_TouchHookCB  gTouchHookCB;
  gTouchHookCB = MouseProc;

#else
  typedef BOOL (*tRegisterTouchWindow)(HWND hWnd, ULONG ulFlags);

  tRegisterTouchWindow  resulrt = (tRegisterTouchWindow) GetProcAddress(GetModuleHandle("user32.dll"), "RegisterTouchWindow");
  if (resulrt) {
    resulrt(fl_xid(gWrk3DSheet), 0);
  }
  pGetTouchInputInfo = (tGetTouchInputInfo) GetProcAddress(GetModuleHandle("user32.dll"), "GetTouchInputInfo");
  pCloseTouchInputHandle = (tCloseTouchInputHandle)GetProcAddress(GetModuleHandle("user32.dll"), "CloseTouchInputHandle");

  typedef int (*WM_TouchWindowCB )(HWND,WPARAM, LPARAM);
  extern WM_TouchWindowCB  gTouchWindowCB;
#ifdef  TOUCHWINDOW
  gTouchWindowCB = TouchWindow;
#endif

#endif
#else
  pthread_attr_t attr;
  struct sched_param sched_param;
  pthread_attr_init(&attr);
  sched_param.sched_priority=SCHED_RR;
  pthread_attr_setschedparam(&attr,&sched_param);
  if (m_thread==0) pthread_create(&m_thread,&attr, gMouse_thread, (void*)NULL);
#endif
}

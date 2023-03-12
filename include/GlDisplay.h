#if !defined(AFX_GLDISPLAY_H__7F6A8903_29F3_4CC4_A2A5_8D90C136D17D__INCLUDED_)
#define AFX_GLDISPLAY_H__7F6A8903_29F3_4CC4_A2A5_8D90C136D17D__INCLUDED_

#pragma once


#include <stdarg.h>
#include "BastelUtils.h"
#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include "Layers.h"

#if defined(WIN32)
# define GLEW_STATIC 1
#endif
//#include <GL/glew.h>
//#include "CncSetup.h"

#ifndef DREITABLE
#define  VIRTUALCNC
#endif

#undef SCROLLBAR3D
class DXF3DDisplay : public Fl_Gl_Window
{
public:
#ifdef SCROLLBAR3D
  Fl_Scrollbar		xscrollbar_;
  Fl_Scrollbar		yscrollbar_;
  int    			bShowScrollbar;
#endif
  //GLuint shaderProgram;
  //GLuint vertexArrayObject;
  //GLuint vertexBuffer;
  //GLuint uvBuffer;
  //GLint positionUniform;
  //GLint colourAttribute;
  //GLint positionAttribute;
  int GlInit;
  int gl_version_major;
//  Fl_Scrollbar		xscrollbar_;
//  Fl_Scrollbar		yscrollbar_;
//  int    			bShowScrollbar;
  FILE * m_file;

  DXF3DDisplay(int X, int Y, int W, int H, const char *L);
  virtual ~DXF3DDisplay();
  void updatedrwaw();

  int  drawGcodeObjects(int Mode,int X, int Y);
  int  drawObjects(int Mode,int X, int Y);
  int  draw3TafelObjects(int Mode,int X, int Y);
  void drawSoftMultiple();
  void drawSoft();
  //void drawStl();
  int  SaveStlObjects(int Mode);
  void draw();
  void hide();
//  void virtual CNCFreigeben(void);


  static void	scrollbar_cb(Fl_Widget *w, void *d);
  virtual int handle(int event);
  //int  LoadGaphicData(const char * file_path);
  //int  LoadStlFile(const char * file_path);
  //int  LoadDXF();
  //int  CreateVLine(float x1,float y1,float x2,float y2,float winkel,float breite,float tiefe);
  //int  CreateGridLine(float x1,float y1,float x2,float y2,float winkel,float breite,float tiefe);
  //int  CreateFLine(float x1,float y1,float x2,float y2,float winkel,float breite,float tiefe);
  //int  CreateHole (float x1,float y1,float durchmesser,float tiefe);
  void DeleteGaphicData();
  int  InitOpenGL();
  int  InitShader();
  void SetGeometrics(int mode);
  void SetFlatGeometrics(int mmsize);
  void SetPerspective(int mode);
  void CleanupOpenGls();
  float getZoom(int Start);
  void  setZoom(float z);
  void  setRotation(int w);
  float getRotation(int Start);
  void  setRotation2(int w);
  float getRotation2(int Start);

};
extern int gObject_redraw;

extern float Zoom;
extern float Perspective;
extern float Deep;
extern float LightPos[3];
extern float LightColor[3];
extern float eye[3];
extern float center[3];
extern float RotateVal [3];
extern float gBackground;
extern int   gLookClip;


extern int   Pmodify;
extern int   gbModify;
extern int   gbFillPolygon;
extern int   gbUsePolygonObject;

extern int   bShowNormale;
extern int   bYUpp;
extern int   bArrangeZoomCenter;
extern int   gMausMode;
;

extern float XOffset;
extern float YOffset;
extern float ZOffset;
extern float glFaktor;
extern float MMToScreen;
extern int   closest;
extern int   AktLayer;
extern DXF3DDisplay * gWrk3DSheet;
void cbExit(Fl_Widget*, void*);
void PickFile(Fl_Widget*, void*);

extern void GlTimer_CB(void *data);
extern Fl_Window  * wSettings;
extern char  evt_txt [256];

#endif

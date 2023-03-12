#ifndef CGLDIALOG_H
#define CGLDIALOG_H
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Value_Slider.H>
#include "FltkWindow.h"
#include "include/BastelUtils.h"

#ifdef WIN32
#    define snprintf _snprintf
#endif

#ifndef M_PI
#define M_PI   3.1415926535897932384
#define M_PI_2 1.57079632680
#define M_PI_4 0.78539816340f
#endif

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


class Gl_Value_Slider : public Fl_Value_Slider{
private:
  void draw_bg(int X, int Y, int W, int H);
  void draw(int X, int Y, int W, int H);
protected:
  void draw_label(int X, int Y, int W, int H);
  void draw_box();
  void draw() ;
  void draw_box(Fl_Boxtype t, int x,int y,int w,int h, Fl_Color c);
public:
  Gl_Value_Slider(int x,int y,int w,int h, const char *l = 0);
};



#endif // CGLDIALOG_H

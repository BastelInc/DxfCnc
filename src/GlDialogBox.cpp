#include "FltkDialogBox.h"
#include "Resource.h"
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

#include <math.h>
#if defined(WIN32)
# define GLEW_STATIC 1
//#include <GL/glew.h>
#else
//#include "glew.h"
#endif



//-------------------------------------------------------------

Gl_Value_Slider::Gl_Value_Slider(int x,int y,int w,int h, const char *l):Fl_Value_Slider(x,y,w,h,l){
;
}

void Gl_Value_Slider::draw_box(){

  //Fl_Value_Slider::draw_box();
  draw_box((Fl_Boxtype)0, x(), y(), w(), h(), FL_RED);

}

void Gl_Value_Slider::draw_box(Fl_Boxtype t, int x,int y,int w,int h, Fl_Color c)
{
  //Fl_Value_Slider::draw_box(t,x,y,w,h,c);
  if (active_r()) {
    //Fl_Widget::fl_box_table[t].f(x, y, w, h, c);
    fl_color(FL_RED); //selection_color());
	fl_line(x,y  , x+w, y  );
	fl_line(x,y+h, x+w, y+h);
	fl_line(x,  y, x  , y+h);
	fl_line(x+w,y, x+w, y+h);


  }
}

float glFaktor= 1.0;
float glCNCToScreen(float c)
{
  return (c * glFaktor);
}
#define CNCINT float


//-------------------------------------------------------------


void Gl_Value_Slider::draw_label(int x, int y, int w, int h){
//  Fl_Value_Slider::draw_label(x,y,w,h);
  float m_Winkel = 0;
  float Drehung  =  (float)(m_Winkel * M_PI / 180.0);
  CNCINT BX = x;
  CNCINT BY = y+h;
  int Down=0;
  const char * m_Ps = label();
  int r = h / 7;
  int m_Height = h - 2*r;//20;
  BX += r;
  BY -= r;

  // 100 = 1 mm Char 10mm = 100
  int l = strlen(m_Ps);
  for (int i=0; i < l; i++)
  {
    unsigned char ch = m_Ps[i];
    switch (ch)
    {
    case 228:
    case 142:
    case 132: // ?
      ch = 127;
      break;	//* Umlate nicht sortiert
    case 148:
    case 246:
    case 153: // ?
      ch = 128;
      break;
    case 129:
    case 252:
    case 154: // ?
      ch = 129;
      break;
    case 227:
      ch = 130;
      break;
    case 231:
    case 135: // ?
      ch = 131;
      break;
    case 243:
    case 162: // ?
      ch = 132;
      break;
    }
    if (ch >= ErsterBuchstabe && ch <= LetzterBuchstabe)
    {
      CNCINT XX = 0;
      CNCINT YY = 0;
      CNCINT LX = 0;
      CNCINT LY = 0;
      CNCINT LZ = 0;

      ch -= (char)ErsterBuchstabe;
      int l = Korrektur[ch].Si / 2;
      bvector * Bustr = Zeichen[ch];
      if (Bustr!= NULL) {
        for (int i=0; i<l; i++)
        {
          if  (Bustr[i].x == 0 && Bustr[i].y == 0) {
            // (StartPosition)
            if (Down) glEnd();
            Down = 0;
          } else {
            XX = (int)((float)(Bustr[i].x-Korrektur[ch].Ofs) * m_Height /100.0);
            YY = (int)((float)(Bustr[i].y-100.0) * m_Height /100.0);

            CNCINT X2 = XX;
            XX = (int)(XX *cos(Drehung) -YY *sin(Drehung));
            YY = (int)(YY *cos(Drehung) +X2 *sin(Drehung));
            float x,y,z;
            if (Down==0) glBegin(GL_LINE_STRIP);
            x = glCNCToScreen(BX+XX);
            y = glCNCToScreen(BY-YY);
            z = 0.0;
            glVertex3f(x,y,z);
            if (Down) fl_line(LX,LY,x,y);
            Down = 1;
            LX = BX+XX;
            LY = BY-YY;
            LZ = 0.0;
          }
        }
        if (Down) glEnd();
        Down = 0;
        float space = ((float)(Korrektur[ch].Br + 32 ) * m_Height /100.0f);
        BX = BX+ (int)(space *cos(Drehung));
        BY = BY+ (int)(space *sin(Drehung));
      } else {
        float space = ((float)(Korrektur[ch].Br + 32 ) * m_Height /100.0f);
        BX = BX+ (int)(space *cos(Drehung));
        BY = BY+ (int)(space *sin(Drehung));
      }

      if (Down) glEnd();
      Down = 0;
    }
Exit:
    ;
  }
}




void Gl_Value_Slider::draw_bg(int X, int Y, int W, int H) {
  fl_push_clip(X, Y, W, H);
  draw_box();
  {
    fl_color(FL_BLACK); 
    fl_rectf(X,Y,W,H);
  }
  fl_pop_clip();

  Fl_Color black = active_r() ? FL_FOREGROUND_COLOR : FL_INACTIVE_COLOR;
  if (type() == FL_VERT_NICE_SLIDER) {
    draw_box(FL_THIN_DOWN_BOX, X+W/2-2, Y, 4, H, black);
  } else if (type() == FL_HOR_NICE_SLIDER) {
    draw_box(FL_THIN_DOWN_BOX, X, Y+H/2-2, W, 4, black);
  }
}

void Gl_Value_Slider::draw(int X, int Y, int W, int H) {

  double val;
  if (minimum() == maximum())
    val = 0.5;
  else {
    val = (value()-minimum())/(maximum()-minimum());
    if (val > 1.0) val = 1.0;
    else if (val < 0.0) val = 0.0;
  }

  int ww = (horizontal() ? W : H);
  int xx, S;
  if (type()==FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) {
    S = int(val*ww+.5);
    if (minimum()>maximum()) {S = ww-S; xx = ww-S;}
    else xx = 0;
  } else {
    S = int(slider_size()*ww+.5);//slider_size_
    int T = (horizontal() ? H : W)/2+1;
    if (type()==FL_VERT_NICE_SLIDER || type()==FL_HOR_NICE_SLIDER) T += 4;
    if (S < T) S = T;
    xx = int(val*(ww-S)+.5);
  }
  int xsl, ysl, wsl, hsl;
  if (horizontal()) {
    xsl = X+xx;
    wsl = S;
    ysl = Y;
    hsl = H;
  } else {
    ysl = Y+xx;
    hsl = S;
    xsl = X;
    wsl = W;
  }

  draw_bg(X, Y, W, H);

  Fl_Boxtype box1 = slider();
  if (!box1) {box1 = (Fl_Boxtype)(box()&-2); if (!box1) box1 = FL_UP_BOX;}
  if (type() == FL_VERT_NICE_SLIDER) {
    draw_box(box1, xsl, ysl, wsl, hsl, FL_GRAY);
    int d = (hsl-4)/2;
    draw_box(FL_THIN_DOWN_BOX, xsl+2, ysl+d, wsl-4, hsl-2*d,selection_color());
  } else if (type() == FL_HOR_NICE_SLIDER) {
    draw_box(box1, xsl, ysl, wsl, hsl, FL_GRAY);
    int d = (wsl-4)/2;
    draw_box(FL_THIN_DOWN_BOX, xsl+d, ysl+2, wsl-2*d, hsl-4,selection_color());
  } else {
    if (wsl>0 && hsl>0) draw_box(box1, xsl, ysl, wsl, hsl, selection_color());

    if (type() != FL_HOR_FILL_SLIDER && type() != FL_VERT_FILL_SLIDER &&
        Fl::is_scheme("gtk+")) {
      if (W>H && wsl>(hsl+8)) {
        // Draw horizontal grippers
	int yy, hh;
	hh = hsl-8;
	xx = xsl+(wsl-hsl-4)/2;
	yy = ysl+3;

	fl_color(fl_darker(selection_color()));
	fl_line(xx, yy+hh, xx+hh, yy);
	fl_line(xx+6, yy+hh, xx+hh+6, yy);
	fl_line(xx+12, yy+hh, xx+hh+12, yy);

        xx++;
	fl_color(fl_lighter(selection_color()));
	fl_line(xx, yy+hh, xx+hh, yy);
	fl_line(xx+6, yy+hh, xx+hh+6, yy);
	fl_line(xx+12, yy+hh, xx+hh+12, yy);
      } else if (H>W && hsl>(wsl+8)) {
        // Draw vertical grippers
	int yy;
	xx = xsl+4;
	ww = wsl-8;
	yy = ysl+(hsl-wsl-4)/2;

	fl_color(fl_darker(selection_color()));
	fl_line(xx, yy+ww, xx+ww, yy);
	fl_line(xx, yy+ww+6, xx+ww, yy+6);
	fl_line(xx, yy+ww+12, xx+ww, yy+12);

        yy++;
	fl_color(fl_lighter(selection_color()));
	fl_line(xx, yy+ww, xx+ww, yy);
	fl_line(xx, yy+ww+6, xx+ww, yy+6);
	fl_line(xx, yy+ww+12, xx+ww, yy+12);
      }
    }
  }

  draw_label(xsl, ysl, wsl, hsl);
  if (Fl::focus() == this) {
    if (type() == FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) draw_focus();
    else draw_focus(box1, xsl, ysl, wsl, hsl);
  }
}

void Gl_Value_Slider::draw() {
  if (damage()&FL_DAMAGE_ALL) draw_box();
  draw(x()+Fl::box_dx(box()),
       y()+Fl::box_dy(box()),
       w()-Fl::box_dw(box()),
       h()-Fl::box_dh(box()));
}


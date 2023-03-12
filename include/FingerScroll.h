#if !defined(AFX_FINGERSCROLL_H__6B96DA78_5B8C_4006_BE44_B78E45FB7C42__INCLUDED_)
#define AFX_FINGERSCROLL_H__6B96DA78_5B8C_4006_BE44_B78E45FB7C42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>


class CFingerScroll : public Fl_Scroll  
{
public:
  Fl_Pack * m_pack;
  CFingerScroll(int X, int Y, int W, int H,const char *L);
  virtual ~CFingerScroll();
  int handle(int event);

};

extern void InitFingerTouchService(void);

#endif 

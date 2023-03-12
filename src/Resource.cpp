//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "Resource.h"


#ifdef DREITABLE

#include "icons/AppIcon.xpm"
#include "icons/AppIconSettings.xpm"
#include "icons/Dxf3TableTree.xpm"
#undef static

Fl_Pixmap    tmp (AppIcon_xpm);
Fl_RGB_Image ReseorceInit::ic_AppIcon_xpm (&tmp);
Fl_Pixmap    tmp2 (AppIconSettings_xpm);
Fl_RGB_Image ReseorceInit::ic_AppIconSettings_xpm (&tmp2);

Fl_Pixmap  ReseorceInit::TreeViewPics(Dxf3TableTree_xpm);

#else

#define static const
#include "icons/AppIcon.xpm"
#include "icons/Dxfcnc.xpm"
#include "icons/Exit.xpm"
#include "icons/Comm0.xpm"
#include "icons/Comm1.xpm"
#include "icons/FullSize.xpm"
#include "icons/TreeViewPics44.xpm"
#include "icons/Database.xpm"
#include "icons/OpenData.xpm"
#include "icons/Appssystem.xpm"
#include "icons/Adjust.xpm"
#include "icons/GcodeVelo.xpm"
#include "icons/Isel.xpm"
#include "icons/GoCnc.xpm"
#undef static



Fl_Pixmap    tmp (AppIcon_xpm);
Fl_RGB_Image ReseorceInit::ic_AppIcon_xpm (&tmp);
Fl_Pixmap  ReseorceInit::TreeViewPics(TreeViewPics44_xpm);

#endif


ReseorceInit::ReseorceInit()
{
#ifndef DREITABLE
  Fl_Pixmap  *  tmp = new Fl_Pixmap (Dxfcnc_xpm);
  Fl_RGB_ImageList * RGB = new Fl_RGB_ImageList(tmp);

  int image_w = RGB ->w()/26;
  int image_h = RGB ->h();

  for (int i=0; i < 26; i++)
  {
    ButtonImage_List[i] = RGB->copy(image_w,image_h,i);
  }
  delete RGB;
  delete ButtonImage_List[imNewDoc];
  ButtonImage_List[imNewDoc] = new Fl_Pixmap (Database_xpm);
  delete ButtonImage_List[imFILEOPEN];
  ButtonImage_List[imFILEOPEN] = new Fl_Pixmap (OpenData_xpm);

  ButtonImage_List[26] = new Fl_Pixmap (Comm1_xpm);
  ButtonImage_List[27] = new Fl_Pixmap (Comm0_xpm);
  ButtonImage_List[28] = new Fl_Pixmap (FullSize_xpm);
  ButtonImage_List[29] = new Fl_Pixmap (Exit_xpm);
  ButtonImage_List[30] = new Fl_Pixmap (Appssystem_xpm);
  ButtonImage_List[31] = new Fl_Pixmap (Adjust_xpm);
  ButtonImage_List[32] = new Fl_Pixmap (Isel_xpm);
  ButtonImage_List[33] = new Fl_Pixmap (GcodeVelo_xpm);
  ButtonImage_List[34] = new Fl_Pixmap (GoCnc_xpm);
#endif
}

ReseorceInit::~ReseorceInit()
{
  //dtor
}



Fl_RGB_ImageList::Fl_RGB_ImageList(const Fl_Pixmap *pxm, Fl_Color bg):Fl_RGB_Image(pxm,bg)
{


}

Fl_RGB_ImageList::~Fl_RGB_ImageList()
{

}



Fl_Image* Fl_RGB_ImageList::copy(int W, int H, int index)
{
  Fl_RGB_Image	*new_image;
  uchar		*new_array;

  if (((W*(index+1)) <= w() && H <= h()) &&  array)
  {
    // Make a copy of the image data and return a new Fl_RGB_Image...
    new_array = new uchar[W * H * d()];
    const uchar *src = array + (W*index*d());
    uchar *dst = new_array;
    int dy, dh = h();
    int wd  = W*d();
    int wld = ld() ? ld() : w() * d();
    for (dy=0; dy<dh; dy++)
    {
      memcpy(dst, src, wd);
      src += wld;
      dst += wd;
    }
    new_image = new Fl_RGB_Image(new_array, W, H, d());
    new_image->alloc_array = 1;
    return new_image;
  }
  return NULL;
}


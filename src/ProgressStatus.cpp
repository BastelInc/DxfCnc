//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "BastelUtils.h"
#include "ProgressStatus.h"
#include <FL/Fl.H>
#include <FL/Fl_Progress.H>
extern Fl_Progress*BusyStatus;
static float maxVal;
static float courVal;
//-------------------------------------------------------------
void SetBusyDisplay(int wert,LPCSTR lptext)
{
 if (BusyStatus==0) return ;
 Fl::lock();
 BusyStatus->copy_label(lptext);
 BusyStatus->minimum(0);
 BusyStatus->maximum(maxVal=wert);
 BusyStatus->value(0);
 //BusyStatus->redraw();
 BusyStatus->color(FL_BACKGROUND2_COLOR);
 BusyStatus->selection_color(FL_YELLOW);
// BusyStatus->color(0x88888800);
// BusyStatus->selection_color(0x4444ff00);
 BusyStatus->labelcolor(FL_FOREGROUND_COLOR );
 Fl::check();
 Fl::unlock();
}
//-------------------------------------------------------------
void SetBusyStatus(LPCSTR lptext)
{
 if (BusyStatus==0) return ;
 Fl::lock();
 BusyStatus->copy_label(lptext);
 BusyStatus->redraw();
 Fl::check();
 Fl::unlock();
}
//-------------------------------------------------------------
void AdjustBusyDisplay(int wert)
{
  if (BusyStatus==0) return ;
  courVal=wert;
  if (wert - BusyStatus->value() >100) UpdateDisplay();
  return;
}
//-------------------------------------------------------------
void StepBusyDisplay(void)
{
 if (BusyStatus==0) return ;
 Fl::lock();
 BusyStatus->value(BusyStatus->value()+1);
 BusyStatus->redraw();
 Fl::check();
 Fl::unlock();
}
//-------------------------------------------------------------
void UpdateDisplay(void)
{
  if (BusyStatus==0) return ;
  if(courVal != BusyStatus->value()) {
    Fl::lock();
    BusyStatus->value(courVal);
    BusyStatus->redraw();
    Fl::check();
    Fl::unlock();
  }
}
//-------------------------------------------------------------
void ResetBusyDisplay(void)
{
 if (BusyStatus==0) return ;
 BusyStatus->copy_label("");
 BusyStatus->value(0);
 BusyStatus->redraw();
}

//-------------------------------------------------------------

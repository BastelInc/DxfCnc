#ifndef CVISUAL_PAD_H
#define CVISUAL_PAD_H

#include <FL/Fl_Overlay_Window.H>


class CVisual_Pad : public Fl_Overlay_Window
{
    public:
        CVisual_Pad(int X, int Y, int W, int H, const char *L=0);
//        virtual ~CVisual_Pad();

    void draw_overlay();
    protected:

    private:
};

#endif // CVISUAL_PAD_H

#ifndef CLAYERS_H
#define CLAYERS_H
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include "FingerScroll.h"
#ifdef FINGERSCROLL
extern CFingerScroll     * gLayerFingerScroll;
#else 
//#include <FL/Fl_Tree.H>
#include "TreeView.h"
#endif
extern void UpdateMainWindow(void);
extern void gShowLayerButtons(int newi);
extern int  LayerSwitchModify;
extern void AdjustLayerSwitches();


class CLayers
{
  public:
  CLayers(const void * ObjSource,const char * Layer);
  virtual ~CLayers();
  CLayers * m_NextLayer;
  char    * m_Name;
  unsigned int m_Color;
  int      m_LayerSwitch;
  int      m_DefaultSwitch;
  int      m_NumObjects;
  const void * m_ObjSource;
  protected:

  private:
};
extern CLayers * gpLayers;
CLayers * GetLayer(int Layer);
CLayers * GetLayerIndex(const void * ObjSourcec,const char * Layer);
CLayers * IsLayerIndex(const void * ObjSource,const char * Layer);

typedef CLayers* LAYERTYPE  ;

#ifdef FINGERSCROLL
class  CLayer_Button : public Fl_Button {
protected:
    int m_open;
    virtual void draw();
public:
    CLayer_Button * m_ChildItem;
    CLayer_Button * m_Next;
    virtual int handle(int);
    CLayer_Button(int x,int y,int w,int h,const char *l = 0);
    void AddChild(CLayer_Button * child);
    void Expand(void);
    void Collapse(void);
};
#else
//extern Fl_Tree * gLayerTree;
extern CTreeView * gLayerTree; 
#endif
//-------------------------------------------------------------

#endif // CLAYERS_H

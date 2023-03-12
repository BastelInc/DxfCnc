#if !defined(_TREEVIEW_H__368FF20F_89FC_4CAD_89CE_537745BB2F8A__INCLUDED_)
#define _TREEVIEW_H__368FF20F_89FC_4CAD_89CE_537745BB2F8A__INCLUDED_

#include <basteltype.h>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DREITABLE
#define ICONSIZE    22
#define TREEITEMHEIGTH 32
#else
#define ICONSIZE    32
#define TREEITEMHEIGTH 44
#endif

//#define FONTFACE    FL_HELVETICA//  FL_COURIER
//#define FONTSIZE    16
//#define ICONSIZE    22
//#define TREEITEMHEIGTH 28

extern char musicpath[80];

#define HTMLTREE
#undef  HTMLTREE
#define INSERTCHILDREN          0x80000000
#define INSERT_END              0x7fffffff
#define INSERT_SORT             0x7ffffffE

#define TVHT_VK_LEFT            0x8000
#define TVHT_VK_RIGHT           0x4000

class CTreeView;
class CTreeViewWindowItem;
//#define PWNDCONTROLW CTreeView *
typedef  CTreeView * PWNDCONTROLW ;
class CTVItem  : public Fl_Widget
{
public:
  CTVItem *     m_pParent;
  CTVItem *     m_pChild;
  CTVItem *     m_pNext;
  CTVItem *     m_pPrevoius;
  int           m_NumChild;
  int           m_bExpand;
  int           m_Level;
  unsigned char * m_url;
  unsigned char * m_defApp;
  unsigned char * m_defIcon;

  CTVItem(CTreeView  * pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CTVItem();
  virtual int   DrawHTMLTVItem();
  virtual int   ExpandItem(int Hit);
  virtual int   InsertChildItem(CTVItem **ppFirstChildItem, CTVItem * pItem,int iInsertAfter);
  virtual int   InsertChildItem(CTVItem * pItem,int iInsertAfter);

  void          EnumOpenItems(int * counter);
  void          EnumTVitemChilds(int * pindex,CTVItem * * pResTVItem);
  void          EnumTVitemIndex(CTVItem ** pItem,int * counter);
  void          EnumFindItem(CTVItem ** pItem,LPCSTR lpStr);
  void          EnumFindItemPart(CTVItem ** pItem,LPCSTR lpStr);
  void          ResetState(CTVItem ** pItem);
  virtual LPSTR CatParentName(LPSTR );
  virtual int   BuildTreePath(LPSTR lpszDir);

  CTreeView  *    m_pWndControl;
  DWORD           m_Item_ID;
  DWORD           m_SortKey;
  void *          m_lParam;
  int             m_Selected;
  virtual void    Invalidate(){m_State |= 0x8000;redraw();};
  virtual int     IsDirty(){return (m_State & 0x8000)==0x8000;};
  virtual void    ClrDirty(){m_State &= ~0x8000;};
  virtual int     SetImage(int iImage);
  virtual int     GetImage(void){return  m_Image;};
  virtual int     SetState(int iState);
  virtual int     GetState(void){return m_State & 0x7FFF;};
  virtual int     SelChanging(int nIndex,unsigned int wParam);
  virtual int     SelChanged(int nIndex);
  virtual int     SetText(const char * lpStr);
  virtual char *  GetText(void){return m_pText;};
  virtual int     OnClick(int DoubleClick,int pos);
  virtual int     MoveHorizontal(int movepixel){return 0;};
// HTML Stuff
  char *getTVClassString();
  void  EnumHtmlItems(int files);
  int   NumUpdates();
  void  EnumFindItem(CTVItem ** pItem,int Id);
  void  GetStateUpdates(char ** pnt);
  void  GetTVItemUpdates(char ** pnt);
// Fltk Stuff
  virtual void draw();
  void  Expand();
  void  Collapse();
  void  ShrinkChildItems();
  void  EnumCloseAll();

protected:
  int     m_State;
  int     m_Image;
  char *  m_pText;
};


class CTVDxfLayerItem: public CTVItem
{
public:
  CTVDxfLayerItem (CTreeView  * pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CTVDxfLayerItem (){;};
  virtual int SetState(int iState);
  virtual int OnClick(int DoubleClick,int pos);
};

class CTVLayerItem: public CTVItem
{
public:
  CTVLayerItem (CTreeView  * pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CTVLayerItem (){;};
  virtual int SetState(int iState);
  virtual int OnClick(int DoubleClick,int pos);
};

class CTVParentLayerItem: public CTVItem
{
public:
  CTVParentLayerItem (CTreeView  * pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CTVParentLayerItem (){;};
  virtual int SetState(int iState);
  virtual int OnClick(int DoubleClick,int pos);
};



class CTreeView  : public Fl_Scroll
{
public:
  int             m_AktIndex;
  int             m_Breite;
  int             m_ContextInvalidate;
  CRITICAL_SECTION m_TV_criticalsection;
  int             m_NumOpenItems;
  int             m_Journalix;
  int             m_rgJournal[200];
  int             m_SetVisibleIndex;
  int             m_Busy;
  CTVItem *       m_TVItem;
  CTVItem *       m_pCurItem;

  CTreeView(int X, int Y, int W, int H,const char *L=0);
  virtual        ~CTreeView();
  virtual int     DrawHTML();
  int             GetNumOpenItems(void);
  CTVItem *       GetTVitem(int index);
  CTVItem *       FindItem(LPCSTR lpStr,CTVItem* pStarTItem );
  CTVItem *       FindItemPart(LPCSTR lpStr,CTVItem* pStarTItem );
  CTVItem *       FindItem(int ID,CTVItem* pStarTItem );
  int             FindItemIndex(CTVItem* pItem);
  int             InvalidateItem(CTVItem* pItem);
  int             EnsureVisible(CTVItem* pItem);
  int             EnsureVisible(int nIndex);
  void            InvalidateItem(int nIndex);

  virtual int     SetState(int index);
  virtual int     SelChanging(int nIndex,unsigned int wParam);
  virtual int     SelChanged(int nIndex);
  int             InsertRootItem(CTVItem *pItem);
  int             InsertChildItem(CTVItem *pItem, int iInsertAfter);
  void            CloseAll();
  CTVItem *       ResetState();
  void            Reset(void);

  int HTMLTreeView(int *pfd);
  int NumUpdates();
  char * GetStateUpdates(char * pnt,int maxsize);
  char * GetTVItemUpdates(char * pnt,int maxsize);
  virtual int     OnClick(int Id,int Image,int pos);

  //CTreeViewWindowItem * AddItem(int pos,char * path);

  //int     m_Damage;
  int CreateTreeView(void);
  CTVItem * GetItem(int pos);
  void Resize();
  virtual int handle(int e);
  Fl_Pack *m_pack;

};

extern CTreeView * gTreeView;


#endif

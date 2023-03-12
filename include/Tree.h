#pragma once
#include "TreeView.h"

class CDXLayerItem : public CTVItem
{
public:
  CDXLayerItem(PWNDCONTROLW pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CDXLayerItem();
  virtual BOOL ExpandItem(int Hit);
  virtual int  SetState(int iState);
  virtual BOOL SelChanging(unsigned int wParam);
  virtual int  GetLayerObjects(LPCSTR pszLayer);
  virtual int  GetObjectVertex(LPCSTR psz);
};

 
class CDXFObjectItem : public CTVItem 
{
public:
  CDXFObjectItem(PWNDCONTROLW pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CDXFObjectItem();
  virtual int  SetState(int iState);
  virtual BOOL SelChanging(unsigned int wParam);
  virtual int  SelChanged(void);
  char    is_selected() const;
};

class CDXFPolyItem : public CTVItem 
{
public:
  CDXFPolyItem(PWNDCONTROLW pWndControl,CTVItem * pParent, DWORD record);
  virtual ~CDXFPolyItem();
  virtual BOOL ExpandItem(int Hit);
  virtual int  SetState(int iState);
  virtual BOOL SelChanging(unsigned int wParam);
  virtual int  SelChanged(void);
  void select(int val=1);
  char is_selected() const;
};

extern CTreeView * pDXFTree;

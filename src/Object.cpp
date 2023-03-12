//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "Object.h"

#include "stdlib.h"
#include "Resource.h"
//#include "CncSetup.h"
//#include "Tree.h"
//#include "ProgressStatus.h"
#include "DxfCnc.h"
#include "GlDisplay.h"
#include "Settings.h"
#include <math.h>
#include <sys/stat.h>
//#include "clipper.h"
int Abbruch=0;
int AktTiefe = 6;
LAYERTYPE WorkLayer = NULL;
char    szWorkLayer[256]= {0};
int gStopp=0;
typedef  struct {
  char SymName[64];
  fpos_t  Filepos;
  long int len;
} SymType;


tIncludeList IncludeList;
int  ReadFirst;

static char   dxfstr[256];
static int    DXFCode;
int    gnumSelect=0;
#define NUMUNGET 5
static char   ungetstr    [NUMUNGET][256];
static int    ungetDXFCode[NUMUNGET];
static FILE * ungetstream [NUMUNGET];
static int    ungetCount = 0;
float  fxAufloesung;
int gNewSelection=0;
bool   Zoll=0;
char LeiterLayer[30] = "LEITER";
char BohrenLayer[30] = "BOHREN";
//-------------------------------------------------------------
static SymType Symbole[NUMSYM];
static int SymAnz;
//-------------------------------------------------------------
CObject::CObject (CObjectManager * ObjSource,ClipperLib::cInt  x,ClipperLib::cInt  y,ClipperLib::cInt  z,const char * lpLayer,unsigned int color)
{
  ETiefe = color;
  Art = -1;
  m_Select = Active;
  m_S.X = x;
  m_S.Y = y;
  m_S.Z = z;
  m_Konstruktion=UNDEF;
  m_Alpha = 1;
  m_dir = 0;
  m_Texture=-1;
  m_TextureZoom=1.0;
  m_ObjSource = ObjSource;
  if (x < gObjectManager->MinMaxleft)       gObjectManager->MinMaxleft  = x;
  if (x > gObjectManager->MinMaxright)      gObjectManager->MinMaxright = x;
  if (y < gObjectManager->MinMaxtop)        gObjectManager->MinMaxtop   = y;
  if (y > gObjectManager->MinMaxbottom)     gObjectManager->MinMaxbottom= y;
  if (z < gObjectManager->MinMaxfront)      gObjectManager->MinMaxfront = z;
  if (z > gObjectManager->MinMaxback)       gObjectManager->MinMaxback  = z;

  m_Xmin = 0x7FFFFFFF;
  m_Xmax = 0x80000000;
  m_Ymin = 0x7FFFFFFF;
  m_Ymax = 0x80000000;
  m_Zmin = 0x7FFFFFFF;
  m_Zmax = 0x80000000;

  m_pLayer = GetLayerIndex(m_ObjSource,lpLayer);
}
//-------------------------------------------------------------
CObject::~CObject ()
{


}
//-------------------------------------------------------------
unsigned int  gFarbe256[256] = {
  0x007f7f7f,
  0x000000FF,
  0x0000FFFF,
  0x0000FF00,
  0x00FFFF00,
  0x00FF0000,
  0x00FF00FF,
  0x00FFFFFF,
  0x00414141,
  0x00808080,
  0x000000FF,
  0x00AAAAFF,
  0x000000BD,
  0x007E7EBD,
  0x00000081,
  0x00565681,
  0x00000068,
  0x00454568,
  0x0000004F,
  0x0035354F,
  0x00003FFF,
  0x00AABFFF,
  0x00002EBD,
  0x007E8DBD,
  0x00001F81,
  0x00566081,
  0x00001968,
  0x00454E68,
  0x0000134F,
  0x00353B4F,
  0x00007FFF,
  0x00AAD4FF,
  0x00005EBD,
  0x007E9DBD,
  0x00004081,
  0x00566B81,
  0x00003468,
  0x00455668,
  0x0000274F,
  0x0035424F,
  0x0000BFFF,
  0x00AAEAFF,
  0x00008DBD,
  0x007EADBD,
  0x00006081,
  0x00567681,
  0x00004E68,
  0x00455F68,
  0x00003B4F,
  0x0035494F,
  0x0000FFFF,
  0x00AAFFFF,
  0x0000BDBD,
  0x007EBDBD,
  0x00008181,
  0x00568181,
  0x00006868,
  0x00456868,
  0x00004F4F,
  0x00354F4F,
  0x0000FFBF,
  0x00AAFFEA,
  0x0000BD8D,
  0x007EBDAD,
  0x00008160,
  0x00568176,
  0x0000684E,
  0x0045685F,
  0x00004F3B,
  0x00354F49,
  0x0000FF7F,
  0x00AAFFD4,
  0x0000BD5E,
  0x007EBD9D,
  0x00008140,
  0x0056816B,
  0x00006834,
  0x00456856,
  0x00004F27,
  0x00354F42,
  0x0000FF3F,
  0x00AAFFBF,
  0x0000BD2E,
  0x007EBD8D,
  0x0000811F,
  0x00568160,
  0x00006819,
  0x0045684E,
  0x00004F13,
  0x00354F3B,
  0x0000FF00,
  0x00AAFFAA,
  0x0000BD00,
  0x007EBD7E,
  0x00008100,
  0x00568156,
  0x00006800,
  0x00456845,
  0x00004F00,
  0x00354F35,
  0x003FFF00,
  0x00BFFFAA,
  0x002EBD00,
  0x008DBD7E,
  0x001F8100,
  0x00608156,
  0x00196800,
  0x004E6845,
  0x00134F00,
  0x003B4F35,
  0x007FFF00,
  0x00D4FFAA,
  0x005EBD00,
  0x009DBD7E,
  0x00408100,
  0x006B8156,
  0x00346800,
  0x00566845,
  0x00274F00,
  0x00424F35,
  0x00BFFF00,
  0x00EAFFAA,
  0x008DBD00,
  0x00ADBD7E,
  0x00608100,
  0x00768156,
  0x004E6800,
  0x005F6845,
  0x003B4F00,
  0x00494F35,
  0x00FFFF00,
  0x00FFFFAA,
  0x00BDBD00,
  0x00BDBD7E,
  0x00818100,
  0x00818156,
  0x00686800,
  0x00686845,
  0x004F4F00,
  0x004F4F35,
  0x00FFBF00,
  0x00FFEAAA,
  0x00BD8D00,
  0x00BDAD7E,
  0x00816000,
  0x00817656,
  0x00684E00,
  0x00685F45,
  0x004F3B00,
  0x004F4935,
  0x00FF7F00,
  0x00FFD4AA,
  0x00BD5E00,
  0x00BD9D7E,
  0x00814000,
  0x00816B56,
  0x00683400,
  0x00685645,
  0x004F2700,
  0x004F4235,
  0x00FF3F00,
  0x00FFBFAA,
  0x00BD2E00,
  0x00BD8D7E,
  0x00811F00,
  0x00816056,
  0x00681900,
  0x00684E45,
  0x004F1300,
  0x004F3B35,
  0x00FF0000,
  0x00FFAAAA,
  0x00BD0000,
  0x00BD7E7E,
  0x00810000,
  0x00815656,
  0x00680000,
  0x00684545,
  0x004F0000,
  0x004F3535,
  0x00FF003F,
  0x00FFAABF,
  0x00BD002E,
  0x00BD7E8D,
  0x0081001F,
  0x00815660,
  0x00680019,
  0x0068454E,
  0x004F0013,
  0x004F353B,
  0x00FF007F,
  0x00FFAAD4,
  0x00BD005E,
  0x00BD7E9D,
  0x00810040,
  0x0081566B,
  0x00680034,
  0x00684556,
  0x004F0027,
  0x004F3542,
  0x00FF00BF,
  0x00FFAAEA,
  0x00BD008D,
  0x00BD7EAD,
  0x00810060,
  0x00815676,
  0x0068004E,
  0x0068455F,
  0x004F003B,
  0x004F3549,
  0x00FF00FF,
  0x00FFAAFF,
  0x00BD00BD,
  0x00BD7EBD,
  0x00810081,
  0x00815681,
  0x00680068,
  0x00684568,
  0x004F004F,
  0x004F354F,
  0x00BF00FF,
  0x00EAAAFF,
  0x008D00BD,
  0x00AD7EBD,
  0x00600081,
  0x00765681,
  0x004E0068,
  0x005F4568,
  0x003B004F,
  0x0049354F,
  0x007F00FF,
  0x00D4AAFF,
  0x005E00BD,
  0x009D7EBD,
  0x00400081,
  0x006B5681,
  0x00340068,
  0x00564568,
  0x0027004F,
  0x0042354F,
  0x003F00FF,
  0x00BFAAFF,
  0x002E00BD,
  0x008D7EBD,
  0x001F0081,
  0x00605681,
  0x00190068,
  0x004E4568,
  0x0013004F,
  0x003B354F,
  0x00333333,
  0x00505050,
  0x00696969,
  0x00828282,
  0x00BEBEBE,
  0x00FFFFFF
};

//-------------------------------------------------------------
unsigned int CObject::GetRGBColor()
{
  if (ETiefe>255) {
    unsigned int clr = ETiefe;
    if (m_pLayer && ETiefe==256) {   // Color from Layer
      clr =  m_pLayer->m_Color;
    }
    return ((unsigned int)clr&0xFF00)<< 8 | ((unsigned int)clr&0xFF0000)>> 8  | ((unsigned int)clr&0xFF000000)>> 24;
  } else if ((m_Select & Done) == Done) {
    return gFarbe256[0];
  } else if ((m_Select&Active)==Active) {
    return gFarbe256[ETiefe&0xFF];
  } else {
    return gFarbe256[19];
  }
}

//-------------------------------------------------------------
int CObject::GetWitdh()
{
  if (m_pLayer != WorkLayer) return 1+1;

  if ((gnumSelect>0 && IsSelected())) {
    return 3+1;
  } else   if (AktTiefe== (ETiefe&0xFF) && m_Select/* && m_pLayer == WorkLayer*/) {
    return 3+1;
  } else {
    return 1+1;
  }
}
//-------------------------------------------------------------
int CObject::GetNumItems()
{
  return 1;
}

//-------------------------------------------------------------
CObject * CObjectManager::GetFirst(LAYERTYPE Layer,int Tiefe)
{
  m_AktLayer = Layer;
  m_AktTiefe = Tiefe;
  m_AktXPos = 0;
  m_AktYPos = 0;
  m_Index = -1;
  return GetNext();

}
//-------------------------------------------------------------
CObject * CObjectManager::GetNext(void)
{
  while (m_Index>= -1 && m_Index < CNCInfoCnt-1) {
    m_Index++;
    if (CNCInfo[m_Index]!= NULL) {
      if (m_AktLayer==NULL) {
        if (CNCInfo[m_Index]->m_pLayer->m_LayerSwitch) {
          if (m_AktTiefe==-1 || m_AktTiefe == (CNCInfo[m_Index]->ETiefe&0xFF)) {
            return  CNCInfo[m_AktIndex=m_Index];
          }
        }
      } else {
        if (CNCInfo[m_Index]->m_pLayer==m_AktLayer) {
          if (m_AktTiefe==-1 || m_AktTiefe == (CNCInfo[m_Index]->ETiefe&0xFF)) {
            return  CNCInfo[m_AktIndex=m_Index];
          }
        }
      }
    }
  }
  return NULL;
}

//-------------------------------------------------------------
void CObject::SetDone(int d)
{
  if (d) m_Select |= Done;
  else   m_Select &= ~Done;
}

//-------------------------------------------------------------
void CObject::SetSelect(int s)
{
  if (s) {
    if ((m_Select& Sel)==0) {
      gnumSelect++;
      m_Select |= Sel;
      m_Select &= ~Done;
    }
  } else {
    if ((m_Select& Sel)&&gnumSelect) gnumSelect--;
    m_Select &= ~Sel;
  }
  char str[64];
  //sprintf(str,"Selection %d Items",gnumSelect);
  //SetStatusText(str,2,0);
}

//-------------------------------------------------------------
void CObject::SetActive(int a)
{
  if (a) m_Select |= Active;
  else   m_Select &= ~Active;
}

//-------------------------------------------------------------
int  CObject::IsExecutable()
{
  if (m_pLayer != WorkLayer) return 0;
  // Alle muessen Ungetan Aktiv und mit der richtigen Tiefe sein
  if (((m_Select & (Done|Active))!= Active) || (ETiefe&0xFF) != AktTiefe) return 0;
  // Explizit Selectiert
  //if (m_Select & (Sel==Sel))  return 1;
  if ((m_Select & Sel) && gnumSelect>0)  return 1;
  // Gar nicht selectiert und das richtige Layer
  return (m_pLayer == WorkLayer) || (gnumSelect==0);
};

//-------------------------------------------------------------
int  CObject::IsSelectedOrNothing()
{
  return m_Select & Sel || gnumSelect==0;
};

//-------------------------------------------------------------
int  CObject::IsSelected()
{
  return m_Select & Sel;
};

//-------------------------------------------------------------
int  CObject::IsActive()
{
  return m_Select & Active;
}

//-------------------------------------------------------------
int  CObject::IsDone()
{
  return m_Select & Done;
}

//-------------------------------------------------------------
int CObjectManager::NumObjectofType(int Art, LAYERTYPE Layer,int Tiefe)
{
  CObject * pObj;
  int count=0;
  for (int Index = 0; Index < CNCInfoCnt; Index++) {
    pObj = CNCInfo[Index];
    if (pObj) {
      if (pObj->IsExecutable() && ((pObj->Art&Art)!=0)) {
        count++;
      }
    }
  }
  return  count;
}
//-------------------------------------------------------------

CObject * CObjectManager::GetNearest(int Art, LAYERTYPE Layer,int Tiefe,ClipperLib::cInt x,ClipperLib::cInt y,int Selected)
{
  int Nearest= -1;
  float dist;
  float NearestDist = 10000000;
  CObject * pObj;
  for (int Index = 0; Index < CNCInfoCnt; Index++) {
    pObj = CNCInfo[Index];
    if (pObj) {
      if (pObj->IsExecutable()
          && (pObj->m_Gefunden==0)
          && ((pObj->Art&Art)!=0)) {
        /*      int OLayer = 1 << pObj->Layer;
              //if (((Layer&OLayer)==OLayer)
              if ((gnumSelect>0 && pObj->IsSelected() && pObj->IsDone()==0)
                ||((pObj->Layer ==Layer)
                && (Tiefe==pObj->ETiefe)
                && (pObj->m_Gefunden==0)
                && ((pObj->Art&Art)!=0)
                && (!Selected || ((pObj->m_Select & (Active|Done))==Active)))){*/
        if (pObj->Art == GCodeLinine) {
          Nearest =Index;
          break;
        } else {
          float AxS  = x - pObj->m_S.X;
          float AyS  = y - pObj->m_S.Y;
          float AbstandS = sqrt((AxS*AxS)+(AyS*AyS));
          if (AbstandS<NearestDist) {
            NearestDist=AbstandS;
            Nearest =Index;
          }
        }
      }
    }
  }
  if (Nearest>=0 && Nearest<CNCInfoCnt) {
    CNCInfo[m_AktIndex=Nearest]->m_Gefunden++;
    return CNCInfo[m_AktIndex];
  }
  return NULL;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
int CObjectManager::GetNearestIx(int Art, ClipperLib::cInt x,ClipperLib::cInt y)
{
  int Nearest= -1;
  float dist;
  float NearestDist = 10000000.0f;
  CObject * pObj;
  for (int Index = 0; Index < CNCInfoCnt; Index++) {
    pObj = CNCInfo[Index];
    if (pObj) {
      if ((pObj->m_Gefunden==0)
          && ((pObj->Art&Art)!=0)) {
        if (pObj->Art == GCodeLinine) {
          Nearest =Index;
          break;
        } else {
          float AxS  = x - pObj->m_S.X;
          float AyS  = y - pObj->m_S.Y;
          float AbstandS = sqrt((AxS*AxS)+(AyS*AyS));
          if (AbstandS<NearestDist) {
            NearestDist=AbstandS;
            Nearest =Index;
          }
        }
      }
    }
  }
  if (Nearest>=0 && Nearest<CNCInfoCnt) {
    CNCInfo[m_AktIndex=Nearest]->m_Gefunden++;
    return m_AktIndex;
  }
  return -1;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
static void  unget_dxf(FILE * stream,int Code,char* pnt)
{
  if (ungetCount<NUMUNGET) {
    // stream : liest moeglicherweise Block von anderer Fileposition
    ungetstream[ungetCount] = stream;
    strcpy(ungetstr[ungetCount],pnt);
    ungetDXFCode[ungetCount] = Code;
    ungetCount++;
  }
}
//-------------------------------------------------------------
static bool is_unget(FILE * stream)
{
  if (ungetCount && ungetstream[ungetCount-1] == stream) {
    ungetCount--;
    strcpy(dxfstr,ungetstr[ungetCount]);
    DXFCode = ungetDXFCode[ungetCount];
    return true;
  }
  return false;
}

//-------------------------------------------------------------
//extern int round05(double f);
int round05(double f)
{
  return (int)(f + 0.5);
}

CObjectManager::CObjectManager()
{
  time=0;
  gnumSelect=0;
  CNCSaveCnt=0;
#ifdef  USEVECTOR
  CNCInfo.reserve(AnzahlCNC2);
  CNCInfo.resize(AnzahlCNC2);
#endif
}

CObjectManager::~CObjectManager()
{
  CNCFreigeben();
  SaveFreigeben();
  LayersFreigeben(true);
}

#if 0
//-------------------------------------------------------------
static CObjectArray * prgCNCInfo2;
static int  gStartPunktIx;
static int  gEndPunktIx;

static void HVVerbinden(int Line1ix,int Line2ix,int zumAnfang)
{

  CPolyLineObject * pCNCOb1;
  CPolyLineObject * pCNCOb2;
  if (Line1ix <  Line2ix) {
    int x = Line1ix;
    Line1ix = Line2ix;
    Line2ix = x;
  }
  pCNCOb1= (CPolyLineObject*) (*prgCNCInfo2)[Line1ix];
  pCNCOb2= (CPolyLineObject*) (*prgCNCInfo2)[Line2ix];
  if (pCNCOb1==NULL||pCNCOb2==NULL
      ||pCNCOb1->m_Path.size() != 1 ||pCNCOb2->m_Path.size() != 1
      ||pCNCOb1->Art!= Linie||pCNCOb2->Art!=Linie) return;

  int W1;
  int H1;
  int W2;
  int H2;
  W1  = pCNCOb1->m_S.X-pCNCOb1->m_Path[1].X;
  H1  = pCNCOb1->m_S.Y-pCNCOb1->m_Path[1].Y;
//  T1  = sqrt((W1*W1)+(H1*H1));
  W2  = pCNCOb2->m_S.X-pCNCOb2->m_Path[1].X;
  H2  = pCNCOb2->m_S.Y-pCNCOb2->m_Path[1].Y;
//  T2  = sqrt((W2*W2)+(H2*H2));
  pCNCOb1 ->ETiefe = 3;
  pCNCOb2 ->ETiefe = 3;
  int Farbe = 5 ;//zumAnfang? 1:2;
  int ok=0;
  if (abs(W1) < 2 && abs(H2) < 2) {
    if (zumAnfang) {
      if (1) {
        pCNCOb1->m_Path[1].Y = pCNCOb2->m_S.Y;
        pCNCOb2->m_S.X = pCNCOb1->m_Path[1].X;
        ok = 1;
      }
    } else {
      if (1) {
        pCNCOb1->m_S.Y = pCNCOb2->m_Path[1].Y;
        pCNCOb2->m_Path[1].X = pCNCOb1->m_S.X;
        ok = 1;
      }
    }
  } else if (abs(W2) < 2 && abs(H1) < 2) {
    if (zumAnfang) {
      if (1) {
        pCNCOb1->m_Path[1].X = pCNCOb2->m_S.X;
        pCNCOb2->m_S.Y = pCNCOb1->m_Path[1].Y;
        ok = 1;
      }
    } else {
      if (1) {
        pCNCOb1->m_S.X = pCNCOb2->m_Path[1].X;
        pCNCOb2->m_Path[1].Y = pCNCOb1->m_S.Y;
        ok = 1;
      }
    }
  }
  if (ok) {
    pCNCOb1 ->ETiefe = Farbe;
    pCNCOb2 ->ETiefe = Farbe;
    int i;
    if (zumAnfang==0) {
      for (i= Line2ix+1; i < Line1ix; i++) {
        pCNCOb1= (CPolyLineObject*) (*prgCNCInfo2)[i];
        pCNCOb1 ->ETiefe = 6;
        pCNCOb1 ->Art |=  Weggemacht;
      }
    } else {
      for (i= gStartPunktIx ; i < Line2ix; i++) {
        pCNCOb1= (CPolyLineObject*) (*prgCNCInfo2)[i];
        pCNCOb1 ->ETiefe = 6;
        pCNCOb1 ->Art |=  Weggemacht;
      }
      for (i= Line1ix+1; i <= gEndPunktIx; i++) {
        pCNCOb1= (CPolyLineObject*) (*prgCNCInfo2)[i];
        pCNCOb1 ->ETiefe = 6;
        pCNCOb1 ->Art |=  Weggemacht;
      }
    }
  }

}
#endif
//-------------------------------------------------------------
void CObjectManager::AcivateDXF(int a)
{
  CObject * pCNCObj;
  Sort_Reset();
  int bis = CNCInfoCnt;
  for(int Index=0; Index <= bis; Index++) {
    pCNCObj =  CNCInfo[Index];

    if (pCNCObj && pCNCObj->IsSelectedOrNothing()) {
      pCNCObj->SetActive(a);
      switch (pCNCObj->Art) {
      case Linie: {
        CPolyLineObject * pCNCPLinieObj = (CPolyLineObject*) CNCInfo[Index];
      }
      break;
      case Buchstaben:
        break;
      case Punkt: {
        CBohren * pCNCBorenObj = (CBohren*) CNCInfo[Index];
      }
      break;
      case Kreisbogen : {
        CBogenObject * pCNCKreisObj = (CBogenObject*) CNCInfo[Index];
      }
      break;
      }
    }
  }

}

//-------------------------------------------------------------
void CObjectManager::FlipDXF(void)
{
  float Wendepunkt = (MinMaxright+MinMaxleft);
  CObject * pCNCObj;
  Sort_Reset();
  int bis = CNCInfoCnt;
  for(int Index=0; Index <= bis; Index++) {
    pCNCObj =  CNCInfo[Index];

    if (pCNCObj && pCNCObj->IsSelectedOrNothing()) {
      switch (pCNCObj->Art) {
      case Linie: {
        CPolyLineObject * pCNCPLinieObj = (CPolyLineObject*) CNCInfo[Index];
        pCNCPLinieObj->m_S.X =  Wendepunkt - pCNCPLinieObj->m_S.X;
        for (int i=0; i < pCNCPLinieObj->m_Path.size(); i++) {
          ((ClipperLib::IntPoint*) &pCNCPLinieObj->m_Path[i])->X =
            Wendepunkt - ((ClipperLib::IntPoint*) &pCNCPLinieObj->m_Path[i])->X;
        }
      }
      break;
      case Buchstaben:
        break;
      case Punkt: {
        CBohren * pCNCBorenObj = (CBohren*) CNCInfo[Index];
        pCNCBorenObj->m_S.X = Wendepunkt - pCNCBorenObj->m_S.X;
      }
      break;
      case Kreisbogen : {
        CBogenObject * pCNCKreisObj = (CBogenObject*) CNCInfo[Index];
        pCNCKreisObj ->m_S.X = Wendepunkt - pCNCKreisObj ->m_S.X;
      }
      break;
      }
    }
  }
}

//-------------------------------------------------------------
void CObjectManager::ExpandDXF(float m)
{
  float Wendepunkt = (MinMaxright+MinMaxleft);
  CObject * pCNCObj;
  Sort_Reset();

  for(int Index=0; Index < CNCInfoCnt; Index++) {
    pCNCObj =  CNCInfo[Index];

    if (pCNCObj && pCNCObj->IsSelected()) {
      switch (pCNCObj->Art) {


      case Linie: {
        CPolyLineObject * pPolyLineObject = (CPolyLineObject*) CNCInfo[Index];
        ClipperLib::ClipperOffset co;

        co.MiterLimit = 10.0;

        co.AddPath(pPolyLineObject->m_Path, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
        ClipperLib::Paths ps;
        co.Execute(ps, m * fxAufloesung); //g_scalefactor * gRadius );
        pPolyLineObject->m_Path.clear();
        if (!ps.empty()) pPolyLineObject->m_Path = ps.at(0);
        pPolyLineObject->m_S = pPolyLineObject->m_Path[0];
      }
      break;
      }
    }
  }

}

//-------------------------------------------------------------
void CObjectManager::SetColorDXF(unsigned int color)
{
  CObject * pCNCObj;
  Sort_Reset();
  for(int Index=0; Index < CNCInfoCnt; Index++) {
    pCNCObj =  CNCInfo[Index];

    if (pCNCObj && pCNCObj->IsSelected()) {
      pCNCObj->ETiefe = color;
      switch (pCNCObj->Art) {
      case Linie: {
        CPolyLineObject * pCNCPLinieObj = (CPolyLineObject*) CNCInfo[Index];
      }
      break;
      case Buchstaben:
        break;
      case Punkt: {
        CBohren * pCNCBorenObj = (CBohren*) CNCInfo[Index];
      }
      break;
      case Kreisbogen : {
        CBogenObject * pCNCKreisObj = (CBogenObject*) CNCInfo[Index];
      }
      break;
      }
    }
  }

}

//-------------------------------------------------------------
void CObjectManager::MakeDrills(void)
{
  float Bohrer[7];
  Bohrer[0] = 1.f;
  Bohrer[1] = 0.4f; // Rot 0.65 mm
  Bohrer[2] = 0.5f; // Gelb 0.8 mm
  Bohrer[3] = 0.625f; // Gruen 1.0 mm
  Bohrer[4] = 0.75; // Tuerkis 1.25 mm
  Bohrer[5] = 1.0f; // Blau 2 mm
  Bohrer[6] = 2.f;
  Bohrer[0] *= fxAufloesung;
  Bohrer[1] *= fxAufloesung; // Rot
  Bohrer[2] *= fxAufloesung; // Gelb
  Bohrer[3] *= fxAufloesung; // Gruen
  Bohrer[4] *= fxAufloesung; // Tuerkis
  Bohrer[5] *= fxAufloesung; // Blau
  Bohrer[6] *= fxAufloesung;

  CObject * pCNCObj;

  //prgCNCInfo2  = &CNCInfo;

  Sort_Reset();

  for(int Index=0; Index < CNCInfoCnt; Index++) {
    pCNCObj =  CNCInfo[Index];

    if (pCNCObj && pCNCObj->IsSelectedOrNothing()) {
      switch (pCNCObj->Art) {
      case Kreisbogen : {
        CBogenObject * pCNCKreisObj = (CBogenObject*) CNCInfo[Index];
        if (pCNCKreisObj!=NULL && pCNCKreisObj->m_Radius < Bohrer[0]) {
          int Color=0;
          if (pCNCKreisObj->m_Radius < Bohrer[1]) {
            Color = 1;   // Rot 0.65 mm
          } else if (pCNCKreisObj->m_Radius < Bohrer[2]) {
            Color = 2;   // Gelb 0.8 mm
          } else if (pCNCKreisObj->m_Radius < Bohrer[3]) {
            Color = 3;   // Gruen 1.0 mm
          } else if (pCNCKreisObj->m_Radius < Bohrer[4]) {
            Color = 4;   // Tuerkis 1.25 mm
          } else  {
            Color = 5;   // Blau 2 mm
          }
          pCNCKreisObj->m_Gefunden=1;
          CBohren * Ob = new CBohren(this,(float)pCNCKreisObj->m_S.X,(float)pCNCKreisObj->m_S.Y,(float)pCNCKreisObj->m_S.Z,BohrenLayer,Color);
          delete CNCInfo[Index];
          Ob->m_Gefunden=1;
          CNCInfo[Index]=Ob;
          int ix = GetNearestIx(Kreisbogen,Ob->m_S.X,Ob->m_S.Y);
          if (ix!=Index&&ix>=0&&ix<CNCInfoCnt) {
            CBohren * Obound = (CBohren * )CNCInfo[ix];
            float w = Ob->m_S.X-Obound->m_S.X;
            float h = Ob->m_S.Y-Obound->m_S.Y;
            if (w < 0.1 && w > -0.1 && h < 0.1 && h > -0.1 ) {
              delete CNCInfo[ix];
              CNCInfo[ix]=NULL;
            }

          }
        }
      }
      break;
      }
    }
  }
  gShowLayerButtons(1);

}

//-------------------------------------------------------------
#if 1
void CObjectManager::OptimizeDXF(void)
{
}
#else
void CObjectManager::OptimizeDXF(void)
{
  int T;
  int W;
  int H;
  int M = 127;
  int Skip;
  int  StartPunktX;
  int  StartPunktY;
  int  Horizontal1;
  int  Horizontal2;
  int  Vertikal1;
  int  Vertikal2;
  int  StartHorizontal;
  int  StartVertikal;


  CPolyLineObject * pCNCObj;
  bool PolyAbgeschlossen=true;

//  prgCNCInfo2  = &CNCInfo;

  CPolyLineObject * Ob = NULL;
#define START -3000000
  ClipperLib::cInt X=START;
  ClipperLib::cInt Y=START;
  StartPunktX = START;
  StartPunktY = START;

  Horizontal2 = Horizontal1 = Vertikal2   = Vertikal1 = -1;
  StartHorizontal = StartVertikal= -1;

  AktTiefe=5;
  {
    Sort_Reset();
    int bis = CNCInfoCnt;
    for(int Index=0; Index < bis; Index++) {
      pCNCObj = (CPolyLineObject*) CNCInfo[Index];

      if (pCNCObj && pCNCObj->IsSelectedOrNothing()) {
        if (pCNCObj->Art!= Linie && !PolyAbgeschlossen) {
          if (Ob) Ob->AddVertex(X,Y,0,0.0);
          PolyAbgeschlossen=true;
          X = START;
          Y = START;
        }

        switch (pCNCObj->Art) {
        case Linie: {
          W  = pCNCObj->m_S.X-pCNCObj->m_Path[1].X;
          H  = pCNCObj->m_S.Y-pCNCObj->m_Path[1].Y;
          if (StartPunktX == START && StartPunktY == START) {
            StartPunktX  = pCNCObj->m_S.X;
            StartPunktY  = pCNCObj->m_S.Y;
            gStartPunktIx = Index;
          }
          {
            T  = (int)(float)sqrt((float)(W*W)+(H*H));

            if (T > M || abs(W) < 2 || abs(H) < 2) {
              if (T > 13) {
                Skip = 1;
                Horizontal2 = Horizontal1;
                Vertikal2   = Vertikal1;
                if (T > M && abs(W) >= 2 && abs(H)  >= 2) {
                  Vertikal1                             = -1;
                  Horizontal1                           = -1;
                  //Schraeg !
                } else {
                  if (abs(H) < 2 ) {
                    if (StartHorizontal == -1) {
                      StartHorizontal = Index;
                      StartVertikal   = -2;
                    }
                    Horizontal1     = Index;
                    Vertikal1       = -1;
                    Skip = 0;
                  } else if (abs(W) < 2 ) {
                    if (StartVertikal == -1) {
                      StartVertikal= Index;
                      StartHorizontal = -2;
                    }
                    Vertikal1    = Index;
                    Horizontal1  = -1;
                    Skip = 0;
                  } else {
                    Vertikal1                             = -1;
                    Horizontal1                           = -1;
                    Skip = 1;
                  }
                  if (Horizontal1 >= 0 &&  Vertikal2 >= 0) {
                    HVVerbinden(Horizontal1,Vertikal2,0);
                  } else if (Horizontal2 >= 0 &&  Vertikal1 >= 0) {
                    HVVerbinden(Vertikal1,Horizontal2,0);
                  }
                }
              }
            }
            if (StartPunktX == pCNCObj->m_Path[1].X && StartPunktY == pCNCObj->m_Path[1].Y) {
              gEndPunktIx = Index;
              if  (Skip == 0) {
                if (Horizontal1 >= 0 &&  StartVertikal>= 0) {
                  HVVerbinden(Horizontal1,StartVertikal,1);
                }
                if (StartHorizontal >= 0 &&  Vertikal1 >= 0) {
                  HVVerbinden(Vertikal1,StartHorizontal,1);
                }
              }
              StartPunktX = START;
              StartPunktY = START;
              StartHorizontal = StartVertikal= -1;
              Horizontal2     = Vertikal1    = -1;
              Horizontal1     = Vertikal2    = -1;
            }
          }
Neu:
          if (X==START && Y == START) {
            if (CNCInfoCnt >= AnzahlCNC2-1) return ;
            {
              X = pCNCObj->m_S.X;
              Y = pCNCObj->m_S.Y;
              Ob = new CPolyLineObject(this,X,Y,0,LeiterLayer,5);//pCNCObj->ETiefe);
              if (Ob==NULL) return;
              CNCInfo[CNCInfoCnt] = (CObject*) Ob;
              CNCInfoCnt++;
            }
            Ob->AddVertex(X,Y,0,0.0);

            X = pCNCObj->m_S.X;
            Y = pCNCObj->m_S.Y;
            PolyAbgeschlossen=false;
          }
          if (X == pCNCObj->m_S.X && Y == pCNCObj->m_S.Y) {
            if (Ob) Ob->AddVertex(X,Y,0,0.0);
            X = pCNCObj->m_Path[1].X;
            Y = pCNCObj->m_Path[1].Y;

          } else {
            if (Ob) Ob->AddVertex(X,Y,0,0.0);
            if (Ob->m_S.X == X && Ob->m_S.Y == Y) {
              Ob->m_ClosedLoop=true;
            }
            PolyAbgeschlossen=true;
            X = START;
            Y = START;
            goto Neu;
          }
        }
        break;

        default:
          break;
        }
      }
      if (ifAbbruch()) goto weiter;
      if (pCNCObj != NULL && pCNCObj->Art== Linie
          && pCNCObj->IsSelectedOrNothing()) {
        delete CNCInfo[Index];
        CNCInfo[Index]=NULL;
      }
    }
  }
weiter:
  if (pCNCObj!=NULL && pCNCObj->Art== Linie && !PolyAbgeschlossen) {
    if (Ob) Ob->AddVertex(X,Y,0,0.0);
    PolyAbgeschlossen=true;
    X = START;
    Y = START;
  }
  ShowLayerButtons();
}
#endif

//-------------------------------------------------------------
void CObjectManager::SaveDXF(void)
{

  CPolyLineObject * pCNCObj;

  char Snamen[256];
  bool PolyAbgeschlossen=true;

  char *ext;
  ext = strrchr(m_FilePfadName, '.');
  if (ext==NULL) return;

  //prgCNCInfo2  = &CNCInfo;

  strcpy (Snamen,m_FilePfadName);
  char *ext2 = strrchr(Snamen, '.');
  if (ext2) {
    strcpy(ext2,"_O.dxf");
  }

  CPolygonObject * Ob = NULL;

  {
    FILE * stream;
    stream = fopen(Snamen,"w+");
    {
      float fAufloesung = (float)gPa.Aufloesung;
      if (fAufloesung < 1 || fAufloesung  > 5000) fAufloesung  = 100.0;
      fprintf(stream,"  0\nSECTION\n  2\nENTITIES\n");
      {
        Sort_Reset();
        for(int Index=0; Index <= CNCInfoCnt; Index++) {
          pCNCObj = (CPolyLineObject*) CNCInfo[Index];

          if (pCNCObj && pCNCObj->IsSelectedOrNothing()) {
            switch (pCNCObj->Art) {
            case Linie: {
              if (pCNCObj->m_Path.size()==2) {
                fprintf(stream,"0\nLINE\n8\n%s\n62\n%d\n",pCNCObj->m_pLayer->m_Name,(pCNCObj->ETiefe&0xFF));
                fprintf(stream,"10\n%3.2f\n20\n%3.2f\n11\n%3.2f\n21\n%3.2f\n",
                        (float)pCNCObj->m_Path[0].X / fAufloesung,(float)pCNCObj->m_Path[0].Y / fAufloesung,
                        (float)pCNCObj->m_Path[1].X / fAufloesung,(float)pCNCObj->m_Path[1].Y / fAufloesung);
              } else {
                fprintf(stream,"0\nPOLYLINE\n8\n%s\n6\n"
                        "CONTINUOUS\n62\n%d\n66\n1\n70\n1\n",pCNCObj->m_pLayer->m_Name,(pCNCObj->ETiefe&0xFF));
                for (int i=0; i < pCNCObj->m_Path.size(); i++) {
                  float X = ((ClipperLib::IntPoint*) &pCNCObj->m_Path[i])->X;
                  float Y = ((ClipperLib::IntPoint*) &pCNCObj->m_Path[i])->Y;
                  fprintf(stream,"0\nVERTEX\n8\n%s\n10\n%3.2f\n20\n%3.2f\n",LeiterLayer,
                          (float)X / fAufloesung,(float)Y / fAufloesung);
                }
                fprintf(stream,"0\nSEQEND\n8\n1\n");
              }
            }
            break;

            case Punkt: {
              CBohren * pCNCKreisObj = (CBohren*) CNCInfo[Index];
              fprintf(stream,"0\nPOINT\n8\n%s\n"
                      "62\n%d\n"
                      "10\n%3.2f\n20\n%3.2f\n",pCNCObj->m_pLayer->m_Name,(pCNCKreisObj->ETiefe&0xFF),
                      (float)pCNCKreisObj->m_S.X / fAufloesung,(float)pCNCKreisObj->m_S.Y / fAufloesung);
            }
            break;
            case Kreisbogen : {
              CBogenObject * pCNCKreisObj = (CBogenObject*) CNCInfo[Index];
              fprintf(stream,"0\nCIRCLE\n8\n%s\n"
                      "62\n%d\n"
                      "10\n%3.2f\n20\n%3.2f\n40\n%3.2f\n",pCNCObj->m_pLayer->m_Name,
                      pCNCKreisObj->ETiefe,
                      (float)pCNCKreisObj->m_S.X / fAufloesung,
                      (float)pCNCKreisObj->m_S.Y / fAufloesung,
                      (float)pCNCKreisObj->m_Radius / fAufloesung);
            }
            break;

            case Buchstaben :
              //              TextZeichnen(pCNCObj->m_S.X,pCNCObj->m_S.Y,pCNCObj->Ps,pCNCObj->m_Path[1].Y,pCNCObj->Radius);
              break;
            default:
              //MessageBeep(0);
              break;
            }
          }
          if (ifAbbruch()) goto weiter;
        }
      }
weiter:
      fprintf(stream,"0\nENDSEC\n0\nEOF");
      fclose(stream);
    }
  }
}

//-------------------------------------------------------------
bool CObjectManager::ifAbbruch(void)
{
  bool res = Abbruch;
  Abbruch  = false;
  return res;//ifEscape();
}
//-------------------------------------------------------------
bool CObjectManager::GetSavename(LPSTR Snamen)
{
//  OPENFILENAME ofnTemp;//={0};
//  memset(&ofnTemp,0,sizeof(ofnTemp));
//  char szTemp[] = "DXF Files (*.DXF)\0*.DXF\0";
//  char szFNamen[80];
//  strcpy(szFNamen,Snamen);
//  memset(&ofnTemp,0,sizeof(ofnTemp));
//  ofnTemp.lStructSize	= sizeof( OPENFILENAME );
//  ofnTemp.hwndOwner	= hDXFObjekt;
//  ofnTemp.lpstrFilter	= (LPSTR)szTemp;
//  ofnTemp.lpstrFile	= (LPSTR)szFNamen;
////  ofnTemp.lpstrInitialDir=(LPSTR)szDirPath;
//  ofnTemp.nMaxFile	= sizeof( szFNamen );
//  ofnTemp.lpstrTitle	= "DXF Datei Speichern";
//  ofnTemp.lpstrDefExt	= "*.DXF";
//  ofnTemp.nFilterIndex	= 1;
//  ofnTemp.Flags		= OFN_HIDEREADONLY
//    | OFN_PATHMUSTEXIST
//    | OFN_OVERWRITEPROMPT
//    | OFN_EXPLORER;
//  ofnTemp.hInstance     = hInst;
//  if(GetSaveFileName( &ofnTemp ) == true)
//  {
//    strncpy(Snamen,szFNamen,100);
//    return true;
//  }
  return false;
}

//-------------------------------------------------------------
const char * gGetIniName()
{
  if (gObjectManager) return gObjectManager->GetIniName();
  else return "";

}

//-------------------------------------------------------------
#ifdef USE_SHADER
void CObjectManager::Update3DreiTafelProjektion()
{
  unsigned cnt = (unsigned)IncludeList.size();
  // Sollt nur noch nicht Verarbeitete Polylinien bearbeiten
  for (int i =0; i < IncludeList.size(); i++) {
    IncludeType inc;
    inc = IncludeList[i];
    CLayers *i3DreiTafelLayer = IsLayerIndex(inc.m_ObjectManager,"DREITAFEL");
    if (i3DreiTafelLayer!=NULL) {
      inc.m_ObjectManager->Make3DreiTafelProjektion(inc.X,inc.Y,inc.Z,inc.W);
    }
  }
}
#endif

//-------------------------------------------------------------
bool CObjectManager::FileEinlesen(bool SizeAdjust)
{
  //gStopp = false;
  bool res  = false;
  char *ext;
  ext = strrchr(m_FilePfadName, '.');
  if (ext==NULL) return 0;
  if (SizeAdjust) LayerSwitchModify=0;
  strcpy (m_PfadName,m_FilePfadName);
  char *ext3 = strrchr(m_PfadName, '\\');
  if (ext3) {
    ext3++;
    *ext3='\0';
  } else {
    ext3 = strrchr(m_PfadName, '/');
    if (ext3) {
      ext3++;
      *ext3='\0';
    }
  }
  strcpy (m_WorkPfadName,m_FilePfadName);
  char *ext2 = strrchr(m_WorkPfadName, '.');
  if (ext2) {
    strcpy(ext2,".ini");
    if (SizeAdjust) LoadWorkValues(m_WorkPfadName);
  }
  fxAufloesung =   (float)gPa.Aufloesung;
  if (fxAufloesung < 1 || fxAufloesung  > 5000) fxAufloesung  = 100.0;
  char str[255];
  ClipperLib::cInt  X,Y;
  if (strncasecmp(ext,".GER",5)==0) {
    res = GerberEinlesen();
    fxAufloesung *= 100;
  } else if (strncasecmp(ext,".NCD",5)==0) {
    res = ExellonEinlesen();
  } else if (strncasecmp(ext,".DXF",5)==0) {
    //if ((DXFMode & 4) ==4) res = DXFSpezialEinlesen();
    //else
    LayersFreigeben(1);//LayerSwitchModify==0);
#ifdef USE_SHADER
    TextureFreigeben();
    res = DXFEinlesen();
    TextureEinlesen();
    m_3DreiTafelProjektionStartcnt = CNCInfoCnt;

    CLayers *i3DreiTafelLayer = IsLayerIndex(this,"DREITAFEL");
    if (i3DreiTafelLayer!=NULL) {
      Make3DreiTafelProjektion(0,0,0,0);
    }
    MakePolylineProjektion();
#else
    res = DXFEinlesen();
#ifdef BLOCKSIMULATION
    if (SizeAdjust && gBlockSimulation) {
      gBlockSimulation->loadInitValues();
    }
    if (gBlockTab) { // gBlockTab->visible()){
      if (gBlockSimulation->m_BlockLaengeE>0 && gBlockSimulation->m_BlockBreiteE>0) {
        gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
        gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
        if (gBlockTab->IsDlgButtonChecked(IDC_AutoUpdate)) {
          //gBlockSimulation->CreateVertexArray((MinMaxright-MinMaxleft)/fxAufloesung,(MinMaxbottom-MinMaxtop)/fxAufloesung);
          gBlockSimulation->SimulationStart();
        }
      }
    }
#endif
#endif
  } else if ((strncasecmp(ext,".GCODE",7)==0)
             || (strncasecmp(ext,".NC",4)==0)
             || (strncasecmp(ext,".CNC",5)==0)) {
    LayersFreigeben(true);
    IncludeList.clear();
    IncludeType manager;
    memset(&manager,0,sizeof(manager));
    manager.m_ObjectManager = this;
    IncludeList.push_back(manager);
    extern int gNAuswahl;

    if (SizeAdjust) gNAuswahl=0;

    res = GCodeEinlesen();
#ifdef BLOCKSIMULATION
    if (SizeAdjust && gBlockSimulation) {
      gBlockSimulation->loadInitValues();
    }
    if (gBlockTab) { // gBlockTab->visible()){
      if (gBlockSimulation->m_BlockLaengeE>0 && gBlockSimulation->m_BlockBreiteE>0) {
        gBlockSimulation->CreateVertexArray(gBlockSimulation->m_BlockLaengeE,gBlockSimulation->m_BlockBreiteE);
        gBlockSimulation->UpdateVertexArray(0,0,0,0,1);
        if (gBlockTab->IsDlgButtonChecked(IDC_AutoUpdate)) {
          //gBlockSimulation->CreateVertexArray((MinMaxright-MinMaxleft)/100.0,(MinMaxbottom-MinMaxtop)/100.0);
          gBlockSimulation->SimulationStart();
        }
      }
    }
#endif
  } else if (strncasecmp(ext,".STL",5)==0) {
#ifdef USE_SHADER
    CNCFreigeben();
    SaveFreigeben();
    LayersFreigeben(true);
    gStopp = false;
    struct stat statbuf;
    if (stat(m_FilePfadName,&statbuf)==0) {

      CStlObject * Ob = new CStlObject(this,0,0,0,"Layer",0x70707000);
      if (Ob) {
        //Ob->m_Winkel = 0;
        //Ob->m_Zoom = 1;
        Ob->m_Path = (char*) malloc(strlen(m_FilePfadName)+1);
        strcpy(Ob->m_Path,m_FilePfadName);
        Ob->LoadStlFile(NULL);
        Ob->statbuf_mtime = statbuf.st_mtime;
        CNCInfo[CNCInfoCnt] = (CObject*) Ob;
        CNCInfoCnt++;
        IncludeList.clear();
        IncludeType manager;
        memset(&manager,0,sizeof(manager));
        manager.m_ObjectManager = this;
        IncludeList.push_back(manager);

        if (SizeAdjust) {
          LookAtLaden(-1);
        }
      }
    }
#endif
  }
  struct stat st;
  stat(m_FilePfadName,&st);
  // Zeitpunkt zum bestimmen vom Neu Einlesen
#ifdef WIN32
  time = st.st_mtime;
#else
  time = st.st_mtim.tv_sec;
#endif

  if (SizeAdjust) {
    if (Zoll) fxAufloesung /= 2.54f;
    bArrangeZoomCenter=1;
  }
  X = MinMaxright -  MinMaxleft;
  Y = MinMaxbottom - MinMaxtop;
  if (X <= 0) MinMaxright = MinMaxleft+1;
  if (Y <= 0) MinMaxtop   = MinMaxbottom +1;
  float t = fxAufloesung;
  char *filen;
  filen = strrchr(m_FilePfadName, '/');
  sprintf(str," x%4.1f | y%4.1f | b%5.1f | h%5.1f %s",
          ((float)MinMaxleft/t),
          ((float)MinMaxtop/t),
          ((float)X/t),
          ((float)(int)Y/t),filen);
  SetStatusText(str, 1, 0);
  if (CNCInfoCnt >= AnzahlCNC2-1)
    fprintf(stderr,"Fehler : Speicher erschoepft\rDaten unvollstaendig !");

  Sort_Reset();
  return res || CNCInfoCnt >= 1;
}
//-------------------------------------------------------------
bool CObjectManager::FileOpen(const char * pszFile,int bObjectTree)
{
  Abbruch  = false;
  static int recursive=0;
  // Wird gleichzeitig auch im GlTimer_CB Timer als frisch erkannt
  if (recursive) return true;
  recursive++;
  bool res = false;
  m_InUse++;
  m_3DreiTafelProjektionStartcnt=0;
  if (pszFile) {
    struct stat st;
    stat(pszFile,&st);
#ifdef WIN32
    time = st.st_mtime;
#else
    time = st.st_mtim.tv_sec;
#endif
    int AnderesFile =0;
    if (strcmp(m_FilePfadName,pszFile)!=0) {
      AnderesFile=1;
    }
    strcpy(m_FilePfadName,pszFile);
    if (FileEinlesen(AnderesFile)) {
      res = true;
      gShowLayerButtons(1);
    }
  } else {
    res = FileEinlesen(0);
    gShowLayerButtons(0);   // immer Neu weil die alten Layers nicht mehr existieren
  }
#ifndef DREITABLE
  if (gDxfColorTab->visible()) gDxfColorTab->Show();
#endif
  if (bObjectTree && CNCInfoCnt< 10000) {
    DXFTreeEinlesen();
  } else {
#ifdef FLTREE
    if (pDXFTree)  pDXFTree->Reset();
#endif
  }
  gWrk3DSheet->redraw();
//  extern void MakeLayerMenu(void);
//  MakeLayerMenu();
  m_InUse=0;
  recursive--;
  return res;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
#ifdef FLTREE
extern CTreeView * pDXFTree;
bool CObjectManager::DXFTreeEinlesen(void)
{
  if (pDXFTree && pDXFTree->m_pack) {

    pDXFTree->m_pack->clear();



    pDXFTree->m_pack ->begin();
    CTVItem* pTVItem1=0;

    int i=0;
    pDXFTree->m_TVItem=NULL;

    for (i=0; i< gLayerAnz; i++) {
      CTVItem * parent = NULL;
      CTVItem * pTVItem1=  (CTVItem*) new CDXLayerItem(pDXFTree,parent,0);
      pTVItem1->SetImage(imFolderClose);

      if (pTVItem1) {
        pTVItem1->m_lParam  = 1;
        pTVItem1->SetText(gszLayer[i]);
        pTVItem1->m_NumChild = INSERTCHILDREN;
        pTVItem1->m_SortKey = 0xFFFFFF00;
        pDXFTree->InsertChildItem((CTVItem*)pTVItem1,INSERT_END);
      }
    }

    pDXFTree->m_pack ->end();
    pDXFTree->show();
    pDXFTree->redraw();
    return pDXFTree->m_pack->children();
  }
  pDXFTree->show();
  return 1;
}
#else
bool CObjectManager::DXFTreeEinlesen(void)
{
  return false;
}
#endif
//-------------------------------------------------------------
//-------------------------------------------------------------
bool CObjectManager::DXFIncludeEinlesen(char * FilePfadName,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale)
{
  FILE * stream;
  bool OK = true;
  if ((stream = fopen(FilePfadName,"r"))== NULL) return false;

  while (feof(stream )==0 && !gStopp) {
    if (ReadCode(stream)) {
      if (DXFCode ==0) {
        if      (strcmp(dxfstr,"LINE")==0)		  LinieEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"POLYLINE")==0)	  PolyLinieEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"LWPOLYLINE")==0)  LWPolyLinieEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"CIRCLE")==0)	  CircleEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"ARC")==0)		  ArcEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"POINT")==0)		  PointEinlesen(stream,0,0,0,0,Scale);
        else if (strcmp(dxfstr,"MTEXT")==0)		  TextEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"TEXT")==0)		  TextEinlesen(stream,X,Y,Z,W,Scale);

        else if (strcmp(dxfstr,"INSERT")==0)	 InsertEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"BLOCK")==0)	  	 BlockEinlesen(stream);
        else if (strcmp(dxfstr,"LAYER")==0)	     LayerEinlesen(stream);
        else if (strcmp(dxfstr,"3DFACE")==0)	 DreiDFACEEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"3DSOLID")==0)	 DreiDSOLIDEinlesen(stream,X,Y,Z,W,Scale);
      }
    }
    //else OK = false;
  }
  fclose(stream);

#ifdef USE_SHADER
  TextureEinlesen();
  m_3DreiTafelProjektionStartcnt = CNCInfoCnt;
  CLayers *i3DreiTafelLayer = IsLayerIndex(this,"DREITAFEL");
  if (i3DreiTafelLayer!=NULL) {
    Make3DreiTafelProjektion(X,Y,Z,W);
  }
#endif
  return OK;

}

bool CObjectManager::DXFEinlesen(void)
{
  memset(ungetstream,0,sizeof(ungetstream));

  fxAufloesung = (float)gPa.Aufloesung;
  if (fxAufloesung < 1 || fxAufloesung  > 5000) fxAufloesung  = 100.0;
  if (Zoll)fxAufloesung *= 25.4f;
  FILE * stream;
  memset(&Symbole,0,sizeof(Symbole));
  SymAnz     = 0;
  ungetCount = 0;
  CNCFreigeben();
  gStopp = false;

  struct stat st;
  stat(m_FilePfadName,&st);
#ifdef WIN32
  time = st.st_mtime;
#else
  time = st.st_mtim.tv_sec;
#endif
  //SetBusyDisplay(st.st_size,"DXF einlesen");
  int i;
  for (i =1; i < IncludeList.size(); i++) {
    IncludeType inc;
    inc = IncludeList[i];
    inc.m_ObjectManager->CNCFreigeben();
    delete(inc.m_ObjectManager);
  }
  ReadFirst=1;
  IncludeList.clear();
  IncludeType manager;
  memset(&manager,0,sizeof(manager));
  manager.m_ObjectManager = gObjectManager;
  IncludeList.push_back(manager);
  //recursion++;
//              DXFIncludeEinlesen(PfadundName,OX+AX,OY+AY,OZ+AZ,OW+S,1);
  //recursion--;

  if ((stream = fopen(m_FilePfadName,"r"))== NULL) return false;



  while (feof(stream )==0 && !gStopp) {
    if (ReadCode(stream)) {
      if (DXFCode ==0) {
        if      (strcmp(dxfstr,"LINE")==0)		 LinieEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"POLYLINE")==0)	 PolyLinieEinlesen(stream,0,00,0,0,1);
        else if (strcmp(dxfstr,"LWPOLYLINE")==0) LWPolyLinieEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"CIRCLE")==0)	 CircleEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"ARC")==0)		 ArcEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"POINT")==0)	  	 PointEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"MTEXT")==0)	  	 TextEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"TEXT")==0)	  	 TextEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"INSERT")==0)	 InsertEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"BLOCK")==0)	  	 BlockEinlesen(stream);
        else if (strcmp(dxfstr,"LAYER")==0)	     LayerEinlesen(stream);
        else if (strcmp(dxfstr,"3DFACE")==0)	 DreiDFACEEinlesen(stream,0,0,0,0,1);
        else if (strcmp(dxfstr,"3DSOLID")==0)	 DreiDSOLIDEinlesen(stream,0,0,0,0,1);
//		else if (strcmp(dxfstr,"HATCH")==0);
        //    AdjustBusyDisplay(ftell(stream));
      }
    }
  }
//  if   (!gStopp) SetBusyStatus("Ok");
//  else          SetBusyStatus("Abbruch");

  fclose(stream);
  unsigned cnt = (unsigned)IncludeList.size();
  for (i=1; i < IncludeList.size(); i++) {
    IncludeType inc;
    IncludeList[i].m_ObjectManager = new CObjectManager();
    inc = IncludeList[i];
    strcpy(inc.m_ObjectManager->m_FilePfadName,inc.SymName);
    inc.m_ObjectManager->CNCInfoCnt=0;
    // Pfad fuer Include files
    strcpy (inc.m_ObjectManager->m_PfadName,m_PfadName);
    inc.m_ObjectManager->DXFIncludeEinlesen(inc.SymName,inc.X,inc.Y,inc.Z,inc.W,inc.Scale);
  }




  if (CNCInfoCnt>1) {
    fprintf(stderr,"CNCInfoCnt= %d",CNCInfoCnt);
    struct stat st;
    stat(m_FilePfadName,&st);
#ifdef WIN32
    time = st.st_mtime;
#else
    time = st.st_mtim.tv_sec;
#endif
//  SetBusyDisplay(st.st_size,"DXF einlesen");
  }
  SaveFreigeben();
  return true;
}

//-------------------------------------------------------------
bool CObjectManager::LinieEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  float  RX =0;
  float  RY =0;
  float  RZ =0;
  float  RX2=0;
  float  RY2=0;
  float  RZ2 =0;
  int    Color=256;
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);
        else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);
        else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);
        else if (DXFCode==11) sscanf(dxfstr,"%f\n",&RX2);
        else if (DXFCode==21) sscanf(dxfstr,"%f\n",&RY2);
        else if (DXFCode==31) sscanf(dxfstr,"%f\n",&RZ2);
        else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          ClipperLib::cInt AX = (int)((RX * fxAufloesung));
          ClipperLib::cInt AY = (int)((RY * fxAufloesung));
          ClipperLib::cInt AZ = (int)((RZ * fxAufloesung));
          ClipperLib::cInt X  = (int)((RX2 * fxAufloesung));
          ClipperLib::cInt Y  = (int)((RY2 * fxAufloesung));
          ClipperLib::cInt Z  = (int)((RZ2 * fxAufloesung));
          CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
          CNCInfo[CNCInfoCnt] = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
          CObject * Ob = CNCInfo[CNCInfoCnt];
          CNCInfoCnt++;
          if (Ob) {
            Ob->AddVertex(AX,AY,AZ,0);
            CNCComputePoint(&X,&Y,&Z,OX,OY,OZ,OW,Scale);
            Ob->AddVertex(X,Y,Z,0);
            return true;
          }
          return false;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::PointEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char Layer[80];
  char SubLayer[80];
  float RX=0;
  float RY=0;
  float RZ =0;
  int  Color=256;
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);
        else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);
        else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);
        else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          ClipperLib::cInt  AX = (int)(RX * fxAufloesung);
          ClipperLib::cInt  AY = (int)(RY * fxAufloesung);
          ClipperLib::cInt  AZ = (int)(RZ * fxAufloesung);
          CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
          CBohren * Ob = new CBohren(this,AX,AY,AZ,Layer,Color);
          CNCInfo[CNCInfoCnt] = (CObject*) Ob;
          CNCInfoCnt++;
          if (Ob) {
            return true;
          }
          return false;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::ArcEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  char   Keyword[80];
  float  RX=0;
  float  RY=0;
  float  RZ =0;
  float  RX2=0;
  float  RY2=0;
  int    Color=256;
  float  Radius=0;
  memset(&Keyword,0,sizeof(Keyword));
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);
        else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);
        else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);
        else if (DXFCode==40) sscanf(dxfstr,"%f\n",&Radius);
        else if (DXFCode==50) sscanf(dxfstr,"%f\n",&RX2);
        else if (DXFCode==51) sscanf(dxfstr,"%f\n",&RY2);
        else if (DXFCode==1000) {
          sscanf(dxfstr,"%s\n",Keyword);
        } else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          ClipperLib::cInt AX = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ = (int)(RZ * fxAufloesung);

          if (RX2==RY2) RY2+=360.0;

          if (0) {
            CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
            CNCInfo[CNCInfoCnt] = new CBogenObject(this,AX+100,AY+100,AZ,Layer,Color);
            CBogenObject * Ob = (CBogenObject*)CNCInfo[CNCInfoCnt];
            CNCInfoCnt++;
            if (Ob) {
              if (strcmp(Keyword,"reversed:1")==0) {
                if (RY2 < RX2) RY2 += 360.0;
                Ob->m_StartWinkel = RY2;
                Ob->m_EndWinkel   = RX2;
              } else {
                if (RY2 < RX2) RY2 += 360.0;
                Ob->m_StartWinkel = RX2;
                Ob->m_EndWinkel   = RY2;
              }
              Ob->m_Radius = Radius* fxAufloesung;
            }
          }
          if (1) {
            // Bogen zu Polyline umrechnen
            CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
            CPolyLineObject * Ob2 = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
            if (Ob2) {
              float StartWinkel2;
              float EndWinkel2;

              if (strcmp(Keyword,"reversed:1")==0) {
                if (RY2 < RX2) RY2 += 360.0;
                StartWinkel2 = RY2;
                EndWinkel2   = RX2;
              } else {
                if (RY2 < RX2) RY2 += 360.0;
                StartWinkel2 = RX2;
                EndWinkel2   = RY2;
              }
              float Radius2 = Radius* fxAufloesung;



              CNCInfo[CNCInfoCnt] = (CObject*) Ob2;
              CNCInfoCnt++;

              float I = (cos(StartWinkel2 / GRAD360)*Radius2);
              float J = (sin(StartWinkel2 / GRAD360)*Radius2);
              float Xe =(cos(EndWinkel2 / GRAD360)*Radius2);
              float Ye =(sin(EndWinkel2 / GRAD360)*Radius2);
              float winkel = EndWinkel2 - StartWinkel2;

              float Bulge = 1;
              if (winkel > 180 || winkel < -180) {
                Bulge = tan(winkel/GRAD360/8.0);
                float Xm =(cos((EndWinkel2-winkel/2.0) / GRAD360)*Radius2);
                float Ym =(sin((EndWinkel2-winkel/2.0) / GRAD360)*Radius2);
                ClipperLib::cInt  Xs2 = AX+I;
                ClipperLib::cInt  Ys2 = AY+J;
                ClipperLib::cInt  Z = AZ;
                ClipperLib::cInt  Xm2 = AX+Xm;
                ClipperLib::cInt  Ym2 = AY+Ym;
                ClipperLib::cInt  Xe2 = AX+Xe;
                ClipperLib::cInt  Ye2 = AY+Ye;


                CNCComputePoint(&Xs2,&Ys2,&Z,OX,OY,OZ,OW,Scale);
                Ob2->AddVertex(Xs2,Ys2,Z,Bulge*100000.0);
                Z = AZ;
                CNCComputePoint(&Xm2,&Ym2,&Z,OX,OY,OZ,OW,Scale);
                Ob2->AddVertex(Xm2,Ym2,Z,Bulge*100000.0);
                Z = AZ;
                CNCComputePoint(&Xe2,&Ye2,&Z,OX,OY,OZ,OW,Scale);
                Ob2->AddVertex(Xe2,Ye2,Z,0.0);
              } else {
                Bulge = tan(winkel/GRAD360/4.0);
                ClipperLib::cInt  Xs2 = AX+I;
                ClipperLib::cInt  Ys2 = AY+J;
                ClipperLib::cInt  Z = AZ;
                ClipperLib::cInt  Xe2 = AX+Xe;
                ClipperLib::cInt  Ye2 = AY+Ye;


                CNCComputePoint(&Xs2,&Ys2,&Z,OX,OY,OZ,OW,Scale);
                Ob2->AddVertex(Xs2,Ys2,AZ,Bulge*100000.0);
                CNCComputePoint(&Xe2,&Ye2,&Z,OX,OY,OZ,OW,Scale);
                Ob2->AddVertex(Xe2,Ye2,AZ,0.0);

              }

            }
            //}


            return true;
          }
          return false;
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::CircleEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  char   User[80]= {0};
  float  RX=0;
  float  RY=0;
  float  RZ =0;
  int    Color=256;
  float  Radius=0;
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);
        else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);
        else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);
        else if (DXFCode==40) sscanf(dxfstr,"%f\n",&Radius);
        else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          ClipperLib::cInt AX = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ = (int)(RZ * fxAufloesung);
          if (Radius < 1.01f) {
            CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
            CNCInfo[CNCInfoCnt] = new CBogenObject(this,AX,AY,AZ,Layer,Color);
            CBogenObject * Ob = (CBogenObject*)CNCInfo[CNCInfoCnt];
            CNCInfoCnt++;
            if (Ob) {
              Ob->m_StartWinkel = 0;
              Ob->m_EndWinkel   = 360;
              Ob->m_Radius = Radius* fxAufloesung;
              Ob->m_Xmin = Ob->m_S.X - Ob->m_Radius;
              Ob->m_Xmax = Ob->m_S.X + Ob->m_Radius;
              Ob->m_Ymin = Ob->m_S.Y + Ob->m_Radius;
              Ob->m_Ymax = Ob->m_S.Y - Ob->m_Radius;

              //return true;
            }
          }
          /*          if (Radius < 1.01f) {
                      if (Radius < 0.4f) {
                        Color = 1;   // Rot 0.65 mm
                      } else if (Radius < 0.50f) {
                        Color = 2;   // Gelb 0.8 mm
                      } else if (Radius < 0.6f) {
                        Color = 3;   // Gruen 1.0 mm
                      } else if (Radius < 0.75f) {
                        Color = 4;   // Tuerkis 1.25 mm
                      } else  {
                        Color = 5;   // Blau 2 mm
                      }

                      CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                      CBohren 77* Ob = new CBohren(AX,AY,AZ,BohrenLayer,Color);
                      CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                      CNCInfoCnt++;
                      if (Ob) {
                        return true;
                      }
                    }
          */
          else if (1) {
            // Bogen zu Polyline umrechnen
            ClipperLib::cInt AX2 = AX;
            ClipperLib::cInt AY2 = AY;
            ClipperLib::cInt AZ2 = AZ;

            //Nur Objekt (m_S.X Y Z Min Max etc)  berechnen
            CNCComputePoint(&AX2,&AY2,&AZ2,OX,OY,OZ,OW,Scale);

            CPolyLineObject * Ob2 = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
            if (Ob2) {
              float StartWinkel2=0;
              float EndWinkel2=360;
              float Radius2 = Radius* fxAufloesung;
              Ob2->m_ClosedLoop = true;



              CNCInfo[CNCInfoCnt] = (CObject*) Ob2;
              CNCInfoCnt++;
              if (strncasecmp(User,"alpha:",6)==0) {
                if (Ob2) Ob2->m_Alpha = atof(&User[6]);
              }

              float I = (cos(StartWinkel2 / GRAD360)*Radius2);
              float J = (sin(StartWinkel2 / GRAD360)*Radius2);
              float winkel = EndWinkel2 - StartWinkel2;

              float Bulge = 1;
              float Xm =(cos(180.0 / GRAD360)*Radius2);
              float Ym =(sin(180.0 / GRAD360)*Radius2);
              ClipperLib::cInt  Xs2 = AX+I;
              ClipperLib::cInt  Ys2 = AY+J;
              ClipperLib::cInt  Z = AZ;
              ClipperLib::cInt  Xm2 = AX+Xm;
              ClipperLib::cInt  Ym2 = AY+Ym;


              CNCComputePoint(&Xs2,&Ys2,&Z,OX,OY,OZ,OW,Scale);
              Z = AZ;
              Ob2->AddVertex(Xs2,Ys2,AZ,Bulge*100000.0);
              CNCComputePoint(&Xm2,&Ym2,&Z,OX,OY,OZ,OW,Scale);
              Ob2->AddVertex(Xm2,Ym2,AZ,Bulge*100000.0);
              Z = AZ;
              Ob2->AddVertex(Xs2,Ys2,AZ,0.0);


            }
          }

          return false;
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==1000) {
          sscanf(dxfstr,"%s\n",User);
        }
      }
    }
  }
  return false;
}

//-------------------------------------------------------------
bool CObjectManager::DreiDSOLIDEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  float  RX=0;
  float  RY=0;
  float  RZ=0;
  float  Format=0;
  int    Color=256;
  char   str2[1024];
  char   str[256];
  CDreiDFaceObject * Ob = NULL;
  memset(str2,0,sizeof(str2));

  //printf("-----------------------------\n");
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==70) {
          sscanf(dxfstr,"%f\n",&Format);
        } else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==1) {
          strcpy(str,dxfstr);
          if (strlen(str)  + strlen(str2) < sizeof(str2)) {
            strcat(str2,str);
          }
          int l = strlen(str2);
          for (int i =0; i < l; i++) {
            if (str2[i]==' ') {
              //printf("\n");
            } else {
              char ch = 159-str2[i];
              //printf("%c",ch);
            }

          }


          memset(str2,0,sizeof(str2));
        } else if (DXFCode==3) {
          strcpy(str,dxfstr);
          if (strlen(str)  + strlen(str2) < sizeof(str2)) {
            strcat(str2,str);
          }
        } else if (0) {
          sscanf(dxfstr,"%f\n",&RZ);
          ClipperLib::cInt AX  = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY  = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ  = (int)(RZ * fxAufloesung);
          CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
          if (DXFCode==30) {
            CNCInfo[CNCInfoCnt] = new CDreiDFaceObject(this,AX,AY,AZ,Layer,Color);
            Ob = (CDreiDFaceObject*)CNCInfo[CNCInfoCnt];
            CNCInfoCnt++;
          }
          if (Ob) {
            Ob->m_3DFace[DXFCode-30].Y = AX;
            Ob->m_3DFace[DXFCode-30].Y = AY;
            Ob->m_3DFace[DXFCode-30].Z = AZ;
            if (DXFCode==33) {
              Ob->ETiefe = Color;
              // ZumAnfang
              return true;
            }
          }
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          return true;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::DreiDFACEEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  float  RX=0;
  float  RY=0;
  float  RZ=0;
  int    Color=256;
  CDreiDFaceObject * Ob = NULL;

  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10 || DXFCode==11 || DXFCode==12 || DXFCode==13) {
          sscanf(dxfstr,"%f\n",&RX);
        } else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==20 || DXFCode==21 || DXFCode==22|| DXFCode==23) {
          sscanf(dxfstr,"%f\n",&RY);
        } else if (DXFCode==30 || DXFCode==31|| DXFCode==32|| DXFCode==33) {
          sscanf(dxfstr,"%f\n",&RZ);
          ClipperLib::cInt AX  = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY  = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ  = (int)(RZ * fxAufloesung);
          CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
          if (DXFCode==30) {
            CNCInfo[CNCInfoCnt] = new CDreiDFaceObject(this,AX,AY,AZ,Layer,Color);
            Ob = (CDreiDFaceObject*)CNCInfo[CNCInfoCnt];
            CNCInfoCnt++;
          }
          if (Ob) {
            Ob->m_3DFace[DXFCode-30].X = AX;
            Ob->m_3DFace[DXFCode-30].Y = AY;
            Ob->m_3DFace[DXFCode-30].Z = AZ;
            if (DXFCode==33) {
              Ob->ETiefe = Color;
              // ZumAnfang
              Ob->CalculateNormale(0,0,0);
              return true;
            }
          }
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==0) {
          unget_dxf(stream,DXFCode,dxfstr);
          return true;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::PolyLinieEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  char   UserParam[80]= {0};
  ClipperLib::cInt SX=0;
  ClipperLib::cInt SY=0;
  ClipperLib::cInt SZ=0;
  ClipperLib::cInt AX=0;
  ClipperLib::cInt AY=0;
  ClipperLib::cInt AZ=0;
  float  RX=0;
  float  RY=0;
  float  RZ =0;
  float  GlobalZ=0;
  float  Thickness=0;
  float  Bulge=0;
  int    Color=256;
  int    NumVertices=0;
  bool   PolyStart=false;
  bool   ZumAnfang=false;
  bool   StartPunkt=false;
  int    LastDXFCode=0;
  int    XYCnt=0;
  int    PolyLineFalg=0;
  int    VertexCount =0;
  int    FacesCount =0;
  char   User[80]= {0};
  CPolyLineObject * Ob = NULL;
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if (DXFCode ==0 ) {
          if (PolyLineFalg == 64) {
            if (Ob==0) goto PolyMesh;
          } else if (PolyLineFalg == 16) {
            if (Ob==0) goto PolygonMesh;
          } else if (XYCnt>=2 && NumVertices) {
            AX  = (int)(RX * fxAufloesung);
            AY  = (int)(RY * fxAufloesung);
            AZ  = (int)(RZ * fxAufloesung);
            if (PolyLineFalg != 128) {
              if (!PolyStart) {
                SX = AX;		// StartPunkt merken
                SY = AY;
                SZ = AZ;
                PolyStart= true;
                StartPunkt= true;
                CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                Ob = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
                if (Ob==NULL) return false;
                CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                CNCInfoCnt++;
                AX = SX;
                AY = SY;
                AZ = SZ;
              }
              CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
              Ob->AddVertex(AX,AY,AZ,Bulge*100000.0);
              Bulge=0;
              StartPunkt=false;
            }
          }
        }
        LastDXFCode = DXFCode;
        if      (DXFCode==10) {
          sscanf(dxfstr,"%f\n",&RX);
          XYCnt++;
        } else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode==30) {
          sscanf(dxfstr,"%f\n",&RZ);
          XYCnt++;
        } else if (DXFCode==38) {
          sscanf(dxfstr,"%f\n",&GlobalZ);
          OZ+= GlobalZ* fxAufloesung;
        } else if (DXFCode==39) {
          sscanf(dxfstr,"%f\n",&Thickness);
        } else if (DXFCode==42) {
          sscanf(dxfstr,"%f\n",&Bulge);
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
          RGBColor |= Color;
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==70) {
          sscanf(dxfstr,"%d\n",&PolyLineFalg);
          if (PolyLineFalg&1) ZumAnfang = 1;
          if (Ob) Ob->m_ClosedLoop=ZumAnfang;
        } else if (DXFCode==71) {
          sscanf(dxfstr,"%d\n",&VertexCount);
        } else if (DXFCode==72) {
          sscanf(dxfstr,"%d\n",&FacesCount );
        } else if (DXFCode==20) {
          sscanf(dxfstr,"%f\n",&RY);
          XYCnt++;
        } else if (DXFCode==1000) {
          sscanf(dxfstr,"%s %s\n",User,UserParam);
        } else if (DXFCode==0) {
          if (strcmp(dxfstr,"VERTEX")== 0) {
            if (NumVertices==1) {
              SX = AX;		// StartPunkt merken
              SY = AY;
              SZ = AZ;
              if (Ob) {
                Ob->m_S.X = AX;
                Ob->m_S.Y = AY;
                Ob->m_S.Z = AZ;
              }
            }
            NumVertices++;
          } else if (strcmp(dxfstr,"SEQEND")== 0) {
            if (ZumAnfang) {
              ZumAnfang=0;
              Ob->m_ClosedLoop=1;
              if (Ob && (AX != SX || AY != SY)) Ob->AddVertex(SX,SY,SZ,0);
              Ob->m_Konstruktion = KONTUR;
            }
            if (Ob) {
              if ((AX == SX && AY == SY)) Ob->m_Konstruktion = KONTUR;
              Ob->CalculateNormale(0);
            }
            if (strcasecmp(User,"form:")==0) {
              if (Ob) Ob->m_Konstruktion = FORM;
            }
            if (strncasecmp(User,"textur:",7)==0) {
#ifdef USE_SHADER
              if (Ob) {
                if (strncasecmp(User,"textur:Holz",7+4)==0) {
                  Ob->m_Texture = 0;
                  if (rgTextureData[0].szFilename==NULL)
                    rgTextureData[0].szFilename = strdup(rgDefaultTexture[0]);
                }
                if (strncasecmp(User,"textur:Putz",7+4)==0) {
                  Ob->m_Texture = 1;
                  if (rgTextureData[1].szFilename==NULL)
                    rgTextureData[1].szFilename = strdup(rgDefaultTexture[1]);
                }
                if (strncasecmp(User,"textur:Wand",7+4)==0) {
                  Ob->m_Texture = 2;
                  if (rgTextureData[2].szFilename==NULL)
                    rgTextureData[2].szFilename = strdup(rgDefaultTexture[2]);
                }
                if (strncasecmp(User,"textur:Antik",7+5)==0) {
                  Ob->m_Texture = 3;
                  if (rgTextureData[3].szFilename==NULL)
                    rgTextureData[3].szFilename = strdup(rgDefaultTexture[3]);
                }
                if (strncasecmp(User,"textur:Wolke",7+5)==0) {
                  Ob->m_Texture = 4;
                  if (rgTextureData[4].szFilename==NULL)
                    rgTextureData[4].szFilename = strdup(rgDefaultTexture[4]);
                }
                if (strncasecmp(User,"textur:Kies",7+4)==0) {
                  Ob->m_Texture = 5;
                  if (rgTextureData[5].szFilename==NULL)
                    rgTextureData[5].szFilename = strdup(rgDefaultTexture[5]);
                }
                if (strncasecmp(User,"textur:Schalung",7+8)==0) {
                  Ob->m_Texture = 6;
                  if (rgTextureData[6].szFilename==NULL)
                    rgTextureData[6].szFilename = strdup(rgDefaultTexture[6]);
                }
                if (strncasecmp(User,"textur:Wiese",7+5)==0) {
                  Ob->m_Texture = 7;
                  if (rgTextureData[7].szFilename==NULL)
                    rgTextureData[7].szFilename = strdup(rgDefaultTexture[7]);
                }
                if (strncasecmp(User,"textur:Gebuesch",7+8)==0) {
                  Ob->m_Texture = 8;
                  if (rgTextureData[8].szFilename==NULL)
                    rgTextureData[8].szFilename = strdup(rgDefaultTexture[8]);
                }
                if (strncasecmp(User,"textur:Steine",7+6)==0) {
                  Ob->m_Texture = 9;
                  if (rgTextureData[9].szFilename==NULL)
                    rgTextureData[9].szFilename = strdup(rgDefaultTexture[9]);
                }
                int len = strlen(UserParam);
                int a=0;
                for (a=0; a < len; a++) {
                  if (isdigit(UserParam[a])) break;
                }
                if (a < len) Ob->m_TextureZoom = atof(&UserParam[a]);
                else if (len==1) {
                  if      (*UserParam == 'X' || *UserParam == 'x') Ob->m_dir =  1;
                  else if (*UserParam == 'Y' || *UserParam == 'y') Ob->m_dir = -1;
                  else if (*UserParam == 'Z' || *UserParam == 'z') Ob->m_dir = -2;
                }
              }
#endif
            }
            if (strncasecmp(User,"alpha:",6)==0) {
              if (Ob) Ob->m_Alpha = atof(&User[6]);
            }
            return true;
          } else {

            if (Ob) Ob->CalculateNormale(0);
            unget_dxf(stream,DXFCode,dxfstr);
            return true;
          }
        }
      }
    }
  }
PolyMesh:
  if (VertexCount && FacesCount) {
    ClipperLib::IntPoint * pParray = (ClipperLib::IntPoint *) malloc(VertexCount *sizeof(ClipperLib::IntPoint));
    int NumPoints=0;
    int P1=0,P2=0,P3=0,P4=0;

    float mx = 0;
    float my = 0;
    float mz = 0;
    bool  mittelpunkt = 0;

    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if (DXFCode ==0 ) {
          if (1) {
            AX  = (int)(RX * fxAufloesung);
            AY  = (int)(RY * fxAufloesung);
            AZ  = (int)(RZ * fxAufloesung);
            if ((PolyLineFalg &64) == 64) {
              CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
              if (NumPoints < VertexCount) {
                pParray[NumPoints].X    = AX;
                pParray[NumPoints].Y    = AY;
                pParray[NumPoints].Z    = AZ;
                pParray[NumPoints].Attr = 0;
                NumPoints++;
                mx += AX;
                my += AY;
                mz += AZ;
              }


            } else if (PolyLineFalg == 128) {
              if (mittelpunkt==0) {
                mx /= NumPoints;
                my /= NumPoints;
                mz /= NumPoints;
                mittelpunkt=1;
              }
              if (0) {
                if (P1>0 && P1 <= NumPoints) {
                  P1--;
                  AX = pParray[P1].X;
                  AY = pParray[P1].Y;
                  AZ = pParray[P1].Z;
                }
                if (!PolyStart) {
                  SX = AX;		// StartPunkt merken
                  SX = AX;		// StartPunkt merken
                  SY = AY;
                  SZ = AZ;
                  PolyStart= true;
                  StartPunkt= true;
                  CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                  Ob = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
                  if (Ob==NULL) return false;
                  CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                  CNCInfoCnt++;
                  AX = SX;
                  AY = SY;
                  AZ = SZ;
                }
                Ob->AddVertex(AX,AY,AZ,0);
                StartPunkt=false;
                if (P2>0 && P2 <= NumPoints) {
                  P2--;
                  AX = pParray[P2].X;
                  AY = pParray[P2].Y;
                  AZ = pParray[P2].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
                if (P3>0 && P3 <= NumPoints) {
                  P3--;
                  AX = pParray[P3].X;
                  AY = pParray[P3].Y;
                  AZ = pParray[P3].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
                if (P4>0 && P4 <= NumPoints) {
                  P4--;
                  AX = pParray[P4].X;
                  AY = pParray[P4].Y;
                  AZ = pParray[P4].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
              } else {
                if (P1>0 && P1 <= NumPoints) {
                  P1--;
                  AX = pParray[P1].X;
                  AY = pParray[P1].Y;
                  AZ = pParray[P1].Z;
                }

                if (CNCInfoCnt < AnzahlCNC2-1) {
                  CNCInfo[CNCInfoCnt] = new CDreiDFaceObject(this,AX,AY,AZ,Layer,Color);
                  CDreiDFaceObject* Ob3 = (CDreiDFaceObject*)CNCInfo[CNCInfoCnt];
                  CNCInfoCnt++;
                  if (Ob3) {
                    Ob3->m_3DFace[0].X = AX;
                    Ob3->m_3DFace[0].Y = AY;
                    Ob3->m_3DFace[0].Z = AZ;
                    if (P2>0 && P2 <= NumPoints) {
                      P2--;
                      AX = pParray[P2].X;
                      AY = pParray[P2].Y;
                      AZ = pParray[P2].Z;
                      Ob3->m_3DFace[1].X = AX;
                      Ob3->m_3DFace[1].Y = AY;
                      Ob3->m_3DFace[1].Z = AZ;
                    }
                    if (P3>0 && P3 <= NumPoints) {
                      P3--;
                      AX = pParray[P3].X;
                      AY = pParray[P3].Y;
                      AZ = pParray[P3].Z;
                      Ob3->m_3DFace[2].X = AX;
                      Ob3->m_3DFace[2].Y = AY;
                      Ob3->m_3DFace[2].Z = AZ;
                    }
                    if (P4>0 && P4 <= NumPoints) {
                      P4--;
                      AX = pParray[P4].X;
                      AY = pParray[P4].Y;
                      AZ = pParray[P4].Z;
                      Ob3->m_3DFace[3].X = AX;
                      Ob3->m_3DFace[3].Y = AY;
                      Ob3->m_3DFace[3].Z = AZ;
                    }
                    Ob3->CalculateNormale(mx,my,mz);
                  }
                }
              }
            }
          }
        }
        LastDXFCode = DXFCode;
        if (DXFCode== 8) {
          //sscanf(dxfstr,"%s\n",Layer);
        } else if      (DXFCode==10) {
          sscanf(dxfstr,"%f\n",&RX);
        } else if (DXFCode==20) {
          sscanf(dxfstr,"%f\n",&RY);
        } else if (DXFCode==30) {
          sscanf(dxfstr,"%f\n",&RZ);
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==70) {
          sscanf(dxfstr,"%d\n",&PolyLineFalg);
        } else if (DXFCode==71) {
          sscanf(dxfstr,"%d\n",&P1);
        } else if (DXFCode==72) {
          sscanf(dxfstr,"%d\n",&P2);
        } else if (DXFCode==73) {
          sscanf(dxfstr,"%d\n",&P3);
        } else if (DXFCode==74) {
          sscanf(dxfstr,"%d\n",&P4);
        } else if (DXFCode==0) {
          if (strcmp(dxfstr,"VERTEX")== 0) {
          } else if (strcmp(dxfstr,"SEQEND")== 0) {
            free (pParray);
            return true;
          } else {
            unget_dxf(stream,DXFCode,dxfstr);
            return true;
          }
        }
      }
    }
    free (pParray);
  }
  return false;
PolygonMesh:

  if (VertexCount && FacesCount) {
    int MeshPoints = VertexCount * FacesCount;
    ClipperLib::IntPoint  * pParray = (ClipperLib::IntPoint *) malloc(MeshPoints *sizeof(ClipperLib::IntPoint));
    int NumPoints=0;
    int P1=0,P2=0,P3=0,P4=0;

    float mx = 0;
    float my = 0;
    float mz = 0;
    bool  mittelpunkt = 0;

    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if (DXFCode ==0 ) {
          if (1) {
            AX  = (int)(RX * fxAufloesung);
            AY  = (int)(RY * fxAufloesung);
            AZ  = (int)(RZ * fxAufloesung);
            if (PolyLineFalg == 192  || PolyLineFalg == 64) {
              CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
              if (NumPoints < MeshPoints) {
                pParray[NumPoints].X    = AX;
                pParray[NumPoints].Y    = AY;
                pParray[NumPoints].Z    = AZ;
                pParray[NumPoints].Attr = 0;
                NumPoints++;
                mx += AX;
                my += AY;
                mz += AZ;
              }


            } else if (PolyLineFalg == 128  || PolyLineFalg == 16) {
              if (mittelpunkt==0) {
                mx /= NumPoints;
                my /= NumPoints;
                mz /= NumPoints;
                mittelpunkt=1;
              }
              if (0) {
                if (P1>0 && P1 <= NumPoints) {
                  P1--;
                  AX = pParray[P1].X;
                  AY = pParray[P1].Y;
                  AZ = pParray[P1].Z;
                }
                if (!PolyStart) {
                  SX = AX;		// StartPunkt merken
                  SY = AY;
                  SZ = AZ;
                  PolyStart= true;
                  StartPunkt= true;
                  CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                  Ob = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
                  if (Ob==NULL) return false;
                  CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                  CNCInfoCnt++;
                  AX = SX;
                  AY = SY;
                  AZ = SZ;
                }
                Ob->AddVertex(AX,AY,AZ,0);
                StartPunkt=false;
                if (P2>0 && P2 <= NumPoints) {
                  P2--;
                  AX = pParray[P2].X;
                  AY = pParray[P2].Y;
                  AZ = pParray[P2].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
                if (P3>0 && P3 <= NumPoints) {
                  P3--;
                  AX = pParray[P3].X;
                  AY = pParray[P3].Y;
                  AZ = pParray[P3].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
                if (P4>0 && P4 <= NumPoints) {
                  P4--;
                  AX = pParray[P4].X;
                  AY = pParray[P4].Y;
                  AZ = pParray[P4].Z;
                  Ob->AddVertex(AX,AY,AZ,0);
                }
              } else {
                if (P1>0 && P1 <= NumPoints) {
                  P1--;
                  AX = pParray[P1].X;
                  AY = pParray[P1].Y;
                  AZ = pParray[P1].Z;
                }

                CNCInfo[CNCInfoCnt] = new CDreiDFaceObject(this,AX,AY,AZ,Layer,Color);
                CDreiDFaceObject* Ob3 = (CDreiDFaceObject*)CNCInfo[CNCInfoCnt];
                CNCInfoCnt++;
                if (Ob3) {
                  Ob3->m_3DFace[0].X = AX;
                  Ob3->m_3DFace[0].Y = AY;
                  Ob3->m_3DFace[0].Z = AZ;
                  if (P2>0 && P2 <= NumPoints) {
                    P2--;
                    AX = pParray[P2].X;
                    AY = pParray[P2].Y;
                    AZ = pParray[P2].Z;
                    Ob3->m_3DFace[1].X = AX;
                    Ob3->m_3DFace[1].Y = AY;
                    Ob3->m_3DFace[1].Z = AZ;
                  }
                  if (P3>0 && P3 <= NumPoints) {
                    P3--;
                    AX = pParray[P3].X;
                    AY = pParray[P3].Y;
                    AZ = pParray[P3].Z;
                    Ob3->m_3DFace[2].X = AX;
                    Ob3->m_3DFace[2].Y = AY;
                    Ob3->m_3DFace[2].Z = AZ;
                  }
                  if (P4>0 && P4 <= NumPoints) {
                    P4--;
                    AX = pParray[P4].X;
                    AY = pParray[P4].Y;
                    AZ = pParray[P4].Z;
                    Ob3->m_3DFace[3].X = AX;
                    Ob3->m_3DFace[3].Y = AY;
                    Ob3->m_3DFace[3].Z = AZ;
                  }
                  Ob3->CalculateNormale(mx,my,mz);
                }
              }
            }
          }
        }
        LastDXFCode = DXFCode;
        if (DXFCode== 8) {
          //sscanf(dxfstr,"%s\n",Layer);
        } else if      (DXFCode==10) {
          sscanf(dxfstr,"%f\n",&RX);
        } else if (DXFCode==20) {
          sscanf(dxfstr,"%f\n",&RY);
        } else if (DXFCode==30) {
          sscanf(dxfstr,"%f\n",&RZ);
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==70) {
          sscanf(dxfstr,"%d\n",&PolyLineFalg);
        } else if (DXFCode==71) {
          sscanf(dxfstr,"%d\n",&P1);
        } else if (DXFCode==72) {
          sscanf(dxfstr,"%d\n",&P2);
        } else if (DXFCode==73) {
          sscanf(dxfstr,"%d\n",&P3);
        } else if (DXFCode==74) {
          sscanf(dxfstr,"%d\n",&P4);
        } else if (DXFCode==0) {
          if (strcmp(dxfstr,"VERTEX")== 0) {
          } else if (strcmp(dxfstr,"SEQEND")== 0) {
            for (int M=0; M < VertexCount-1; M++) {
              for (int N=0; N < FacesCount-1; N++) {
                P1 = N+M*FacesCount;
                P2 = N+M*FacesCount+1;
                P3 = P2+FacesCount;
                P4 = P1+FacesCount;
                if (CNCInfoCnt < AnzahlCNC2-1) {
                  if (0) {
                    if (P1 < NumPoints) {
                      AX = pParray[P1].X;
                      AY = pParray[P1].Y;
                      AZ = pParray[P1].Z;
                    }
                    if (!PolyStart) {
                      SX = AX;		// StartPunkt merken
                      SY = AY;
                      SZ = AZ;
                      PolyStart= true;
                      StartPunkt= true;
                      CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                      Ob = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
                      if (Ob==NULL) return false;
                      CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                      CNCInfoCnt++;
                      AX = SX;
                      AY = SY;
                      AZ = SZ;
                    }
                    Ob->AddVertex(AX,AY,AZ,0);
                    StartPunkt=false;
                    if (P2 < NumPoints) {
                      AX = pParray[P2].X;
                      AY = pParray[P2].Y;
                      AZ = pParray[P2].Z;
                      Ob->AddVertex(AX,AY,AZ,0);
                    }
                    if (P3 < NumPoints) {
                      AX = pParray[P3].X;
                      AY = pParray[P3].Y;
                      AZ = pParray[P3].Z;
                      Ob->AddVertex(AX,AY,AZ,0);
                    }
                    if (P4 < NumPoints) {
                      AX = pParray[P4].X;
                      AY = pParray[P4].Y;
                      AZ = pParray[P4].Z;
                      Ob->AddVertex(AX,AY,AZ,0);
                    }
                    PolyStart=false;
                  } else {
                    if (P1 < NumPoints) {
                      AX = pParray[P1].X;
                      AY = pParray[P1].Y;
                      AZ = pParray[P1].Z;
                    }

                    CNCInfo[CNCInfoCnt] = new CDreiDFaceObject(this,AX,AY,AZ,Layer,Color);
                    CDreiDFaceObject* Ob3 = (CDreiDFaceObject*)CNCInfo[CNCInfoCnt];
                    CNCInfoCnt++;
                    if (Ob3) {
                      Ob3->m_3DFace[0].X = AX;
                      Ob3->m_3DFace[0].Y = AY;
                      Ob3->m_3DFace[0].Z = AZ;
                      if (P2 < NumPoints) {
                        AX = pParray[P2].X;
                        AY = pParray[P2].Y;
                        AZ = pParray[P2].Z;
                        Ob3->m_3DFace[1].X = AX;
                        Ob3->m_3DFace[1].Y = AY;
                        Ob3->m_3DFace[1].Z = AZ;
                      }
                      if (P3 < NumPoints) {
                        AX = pParray[P3].X;
                        AY = pParray[P3].Y;
                        AZ = pParray[P3].Z;
                        Ob3->m_3DFace[2].X = AX;
                        Ob3->m_3DFace[2].Y = AY;
                        Ob3->m_3DFace[2].Z = AZ;
                      }
                      if (P4 < NumPoints) {
                        AX = pParray[P4].X;
                        AY = pParray[P4].Y;
                        AZ = pParray[P4].Z;
                        Ob3->m_3DFace[3].X = AX;
                        Ob3->m_3DFace[3].Y = AY;
                        Ob3->m_3DFace[3].Z = AZ;
                      }
                      Ob3->CalculateNormale(mx,my,mz);
                    }

                  }
                }
              }
            }
            free (pParray);
            return true;
          } else {
            unget_dxf(stream,DXFCode,dxfstr);
            return true;
          }
        }
      }
    }
    free (pParray);
  }
  return false;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
bool CObjectManager::LWPolyLinieEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   SubLayer[80];
  ClipperLib::cInt SX=0;
  ClipperLib::cInt SY=0;
  ClipperLib::cInt SZ=0;
  ClipperLib::cInt AX=0;
  ClipperLib::cInt AY=0;
  ClipperLib::cInt AZ=0;
  float  RX=0;
  float  RY=0;
  float  RZ =0;
  float  GlobalZ=0;
  float  Thickness=0;
  float  Bulge=0;
  int    Color=256;
  int    NumVertices=0;
  // Lightweight-Polylinie
  int    LWVertices=0;
  bool   PolyStart=false;
  bool   ZumAnfang=false;
  bool   StartPunkt=false;
  int    LastDXFCode=0;
  int    XYCnt=0;
  char   User[80]= {0};
  char   UserParam[80]= {0};
  CPolyLineObject * Ob = NULL;
  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if (LastDXFCode > DXFCode) {
          if (XYCnt>=2  && (LWVertices || NumVertices)) {
            LWVertices--;
            AX  = (int)(RX * fxAufloesung);
            AY  = (int)(RY * fxAufloesung);
            AZ  = (int)(RZ * fxAufloesung);
            if (!PolyStart) {
              SX = AX;		// StartPunkt merken
              SY = AY;
              SZ = AZ;
              PolyStart= true;
              StartPunkt= true;
              CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
              Ob = new CPolyLineObject(this,AX,AY,AZ,Layer,Color);
              if (Ob==NULL) return false;
              if (Ob) Ob->m_ClosedLoop=ZumAnfang;
              CNCInfo[CNCInfoCnt] = (CObject*) Ob;
              CNCInfoCnt++;
              AX = SX;
              AY = SY;
              AZ = SZ;
            }
            CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
            Ob->AddVertex(AX,AY,AZ,Bulge*100000.0);
            Bulge=0;
            StartPunkt=false;
          }
          if (LWVertices==0) {
            if (ZumAnfang) {
              if (Ob) {
                ZumAnfang=0;
                Ob->m_ClosedLoop++;
                // Zum berechnen von Boegen notwendig
                CNCComputePoint(&SX,&SY,&SZ,OX,OY,OZ,OW,Scale);
                if (AX != SX || AY != SY) Ob->AddVertex(SX,SY,SZ,Ob->m_Path[0].Attr);
              }
            }
          }
        }
        LastDXFCode = DXFCode;
        if      (DXFCode==10) {
          sscanf(dxfstr,"%f\n",&RX);
          XYCnt++;
        } else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          };
        } else if (DXFCode==30) {
          sscanf(dxfstr,"%f\n",&RZ);
          XYCnt++;
        } else if (DXFCode==38) {
          sscanf(dxfstr,"%f\n",&GlobalZ);
          OZ+= GlobalZ* fxAufloesung;
        } else if (DXFCode==39) {
          sscanf(dxfstr,"%f\n",&Thickness);
        } else if (DXFCode==42) {
          sscanf(dxfstr,"%f\n",&Bulge);
        } else if (DXFCode==62) {
          sscanf(dxfstr,"%d\n",&Color);
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
          if (Ob) Ob->ETiefe = Color;
        } else if (DXFCode==90) {
          sscanf(dxfstr,"%d\n",&LWVertices);
        } else if (DXFCode==70) {
          int a;
          sscanf(dxfstr,"%d\n",&a);
          ZumAnfang = (a==1 || a==129);
          if (Ob) Ob->m_ClosedLoop=ZumAnfang;
        } else if (DXFCode==20) {
          sscanf(dxfstr,"%f\n",&RY);
          XYCnt++;
          /*
                    if (LWVertices || NumVertices) {
                      LWVertices--;
                      AX  = (int)(RX * fxAufloesung);
                      AY  = (int)(RY * fxAufloesung);
                      AZ  = (int)(RZ * fxAufloesung);
                      if (!PolyStart) {
                        SX = AX;		// StartPunkt merken
                        SY = AY;
                        SZ = AZ;
                        PolyStart= true;
                        StartPunkt= true;
                        CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                        Ob = new CPolyLineObject(AX,AY,AZ,Layer,Color);
                        if (Ob==NULL) return false;
                        CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                        CNCInfoCnt++;
                        AX = SX;
                        AY = SY;
                        AZ = SZ;
                      }
                      LX = AX;        // LetztenPunk merken
                      LY = AY;
                      CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                      Ob->AddVertex(AX,AY,AZ,Bulge*1000.0);
                      Bulge=0;
                       StartPunkt=false;
                    }
                    if (LWVertices==0) {
                      if (ZumAnfang) {
                        if (Ob) Ob->m_ClosedLoop=1;
                        CNCComputePoint(&SX,&SY,&SZ,OX,OY,OZ,OW,Scale);
                        if (AX != SX || AY != SY) Ob->AddVertex(SX,SY,SZ,Ob->m_Path[0].Attr);
                      }
                    }
                    */
        } else if (DXFCode==1000) {
          sscanf(dxfstr,"%s %s\n",User,UserParam);
        } else if (DXFCode==0) {
          if (strcmp(dxfstr,"VERTEX")== 0) {
            if (NumVertices==1) {
              SX = AX;		// StartPunkt merken
              SY = AY;
              SZ = AZ;
              if (Ob) {
                Ob->m_S.X = AX;
                Ob->m_S.Y = AY;
                Ob->m_S.Z = AZ;
              }
            }
            NumVertices++;
          } else if (strcmp(dxfstr,"SEQEND")== 0) {
            if (ZumAnfang) {
              Ob->m_ClosedLoop=1;
              //if (Ob && (AX != SX || AY != SY)) Ob->AddVertex(SX,SY,SZ,0);
              Ob->m_Konstruktion = KONTUR;
            }
            if (Ob && (AX == SX && AY == SY)) Ob->m_Konstruktion = KONTUR;
            Ob->CalculateNormale(0);
            if (strcasecmp(User,"form:")==0) {
              if (Ob) Ob->m_Konstruktion = FORM;
            }
            if (strncasecmp(User,"alpha:",6)==0) {
              if (Ob) Ob->m_Alpha = atof(&User[6]);
            }
            return true;
          } else {
            unget_dxf(stream,DXFCode,dxfstr);
            if (Ob) Ob->CalculateNormale(0);
            if (Ob && (AX == SX && AY == SY)) Ob->m_Konstruktion = KONTUR;
            if (strcasecmp(User,"form:")==0) {
              if (Ob) Ob->m_Konstruktion = FORM;
            }
            if (strncasecmp(User,"textur:",7)==0) {
#ifdef USE_SHADER
              if (Ob) {
                if (strncasecmp(User,"textur:Holz",7+4)==0) {
                  Ob->m_Texture = 0;
                  if (rgTextureData[0].szFilename==NULL)
                    rgTextureData[0].szFilename = strdup(rgDefaultTexture[0]);
                }
                if (strncasecmp(User,"textur:Putz",7+4)==0) {
                  Ob->m_Texture = 1;
                  if (rgTextureData[1].szFilename==NULL)
                    rgTextureData[1].szFilename = strdup(rgDefaultTexture[1]);
                }
                if (strncasecmp(User,"textur:Wand",7+4)==0) {
                  Ob->m_Texture = 2;
                  if (rgTextureData[2].szFilename==NULL)
                    rgTextureData[2].szFilename = strdup(rgDefaultTexture[2]);
                }
                if (strncasecmp(User,"textur:Antik",7+5)==0) {
                  Ob->m_Texture = 3;
                  if (rgTextureData[3].szFilename==NULL)
                    rgTextureData[3].szFilename = strdup(rgDefaultTexture[3]);
                }
                if (strncasecmp(User,"textur:Wolke",7+5)==0) {
                  Ob->m_Texture = 4;
                  if (rgTextureData[4].szFilename==NULL)
                    rgTextureData[4].szFilename = strdup(rgDefaultTexture[4]);
                }
                if (strncasecmp(User,"textur:Kies",7+4)==0) {
                  Ob->m_Texture = 5;
                  if (rgTextureData[5].szFilename==NULL)
                    rgTextureData[5].szFilename = strdup(rgDefaultTexture[5]);
                }
                if (strncasecmp(User,"textur:Schalung",7+8)==0) {
                  Ob->m_Texture = 6;
                  if (rgTextureData[6].szFilename==NULL)
                    rgTextureData[6].szFilename = strdup(rgDefaultTexture[6]);
                }
                if (strncasecmp(User,"textur:Wiese",7+5)==0) {
                  Ob->m_Texture = 7;
                  if (rgTextureData[7].szFilename==NULL)
                    rgTextureData[7].szFilename = strdup(rgDefaultTexture[7]);
                }
                if (strncasecmp(User,"textur:Gebuesch",7+8)==0) {
                  Ob->m_Texture = 8;
                  if (rgTextureData[8].szFilename==NULL)
                    rgTextureData[8].szFilename = strdup(rgDefaultTexture[8]);
                }
                if (strncasecmp(User,"textur:Steine",7+6)==0) {
                  Ob->m_Texture = 9;
                  if (rgTextureData[9].szFilename==NULL)
                    rgTextureData[9].szFilename = strdup(rgDefaultTexture[9]);
                }
                int len = strlen(UserParam);
                int a=0;
                for (a=0; a < len; a++) {
                  if (isdigit(UserParam[a])) break;
                }
                if (a < len) Ob->m_TextureZoom = atof(&UserParam[a]);
                else if (len==1) {
                  if      (*UserParam == 'X' || *UserParam == 'x') Ob->m_dir =  1;
                  else if (*UserParam == 'Y' || *UserParam == 'y') Ob->m_dir = -1;
                  else if (*UserParam == 'Z' || *UserParam == 'z') Ob->m_dir = -2;
                }
              }
#endif
            }
            if (strncasecmp(User,"alpha:",6)==0) {
              if (Ob) Ob->m_Alpha = atof(&User[6]);
            }
            return true;
          }
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::TextEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  char   Layer[80];
  char   User[80]= {0};
  char   SubLayer[80];
  char   Schrift[80];
  char   PfadundName[256];
  float  RX=0;
  float  RY=0;
  float  RZ=0;
  float  RX2=0;
  float  RY2=0;
  int    Color=256;
  int    Ausrichtung=0;
  int    AusrichtungT=0;
  float  wcos=1.0;
  float  wsin=0.0;
  char   * Pnt =  NULL;

  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    while (feof(stream )==0) {
      if (ReadCode(stream)) {
        if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);
        else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);
        else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);
        else if (DXFCode== 8) {
          if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
            strcat(Layer,".");
            strcat(Layer,SubLayer);
          }
        } else if (DXFCode== 8) sscanf(dxfstr,"%s\n",Layer);
        else if (DXFCode== 5) sscanf(dxfstr,"%x\n",&Ausrichtung);
        else if (DXFCode== 7) sscanf(dxfstr,"%s\n",Schrift);
        else if (DXFCode==50) sscanf(dxfstr,"%f\n",&RX2);		//Text rotation (optional; default = 0)
        else if (DXFCode==40) sscanf(dxfstr,"%f\n",&RY2);
        else if (DXFCode==62) sscanf(dxfstr,"%d\n",&Color);
        else if (DXFCode==420) {
          unsigned int RGBColor;
          sscanf(dxfstr,"%d\n",&RGBColor);
          Color =  Color | RGBColor << 8;
        } else if (DXFCode==71) sscanf(dxfstr,"%x\n",&AusrichtungT);
        else if (DXFCode==11) sscanf(dxfstr,"%f\n",&wcos);
        else if (DXFCode==21) sscanf(dxfstr,"%f\n",&wsin);
        else if (DXFCode==1000)sscanf(dxfstr,"%s\n",User);
        else if (DXFCode== 1) {
          if (strlen(dxfstr) < sizeof(dxfstr)) {
            Pnt =  new char[strlen(dxfstr)+1];
            strcpy(Pnt,dxfstr);
          }
        } else if (DXFCode== 0) {
          ClipperLib::cInt AX = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ = (int)(RZ * fxAufloesung);
          ClipperLib::cInt D  = (int)(RY2* fxAufloesung);
          ClipperLib::cInt W  = (int)(RX2);
          if (Ausrichtung==0x5D) {

            // TODO Winkel
            AY -= D;
          } else if (Ausrichtung==0x38) {
            W = (asin(wsin)*GRAD360);
            if (wcos < 0) {
              W = 180-W;
            }
          } else if (Ausrichtung==0x6E0) {
            W = (asin(wsin)*GRAD360);
            if (wcos < 0) {
              W = 180-W;
            }
          } else if (AusrichtungT) {
            W = (asin(wsin)*GRAD360);
            if (wcos < 0) {
              W = 180-W;
            }
          }
          if (AusrichtungT==1) {
            //Ausrichtun oben =  minus Hoehe 8.6.19
            AY -= D;
          }
          CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
          CTextObject * Ob = new CTextObject(this,AX,AY,AZ,Layer,Color);
          CNCInfo[CNCInfoCnt] = (CObject*) Ob;
          CNCInfoCnt++;
          if (Ob) {
            Ob->m_Winkel = W+OW;
            Ob->m_Height = D;
            Ob->m_Ps     = Pnt;
            Pnt = NULL;
            unget_dxf(stream,0,dxfstr);
            return true;
          } else   return false;
        }
      }
    }
  }
  if (Pnt) delete Pnt;
  return false;
}
//-------------------------------------------------------------

bool CObjectManager::InsertEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale)
{
  int AktuellesSymol=0;
  char   PfadundName[256];
  char   SymName[80];
  char   User[80]= {0};
  char   Layer[80];
  char   SubLayer[80];
  //bool   Block = false;
  float  RX=0;
  float  RY=0;
  float  RZ =0;
  float  RX2=0;
  float  SX=1;
  float  SY=1;
  int    Color=256;
  while (feof(stream )==0) {
    if (ReadCode(stream)) {
      if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);     //DXF: X value of base point
      else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);	  //DXF: Y value of base point
      else if (DXFCode==30) sscanf(dxfstr,"%f\n",&RZ);	  //DXF: Z value of base point
      else if (DXFCode== 8) {
        if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
          strcat(Layer,".");
          strcat(Layer,SubLayer);
        }
      } else if (DXFCode== 41)sscanf(dxfstr,"%f\n",&SX); //X scale factor (optional; default = 1)
      else if (DXFCode== 42)sscanf(dxfstr,"%f\n",&SY);  //Y scale factor (optional; default = 1)
      else if (DXFCode==62) sscanf(dxfstr,"%d\n",&Color);
      else if (DXFCode==420) {
        unsigned int RGBColor;
        sscanf(dxfstr,"%d\n",&RGBColor);
        Color =  Color | RGBColor << 8;
      } else if (DXFCode==1000) {
        sscanf(dxfstr,"%s\n",User);
      } else if (DXFCode== 2) {
        strcpy(SymName,dxfstr);
        for (AktuellesSymol=0; AktuellesSymol<SymAnz; AktuellesSymol++) {
          if (strcasecmp(dxfstr,Symbole[AktuellesSymol].SymName)==0) break;
        }
        if (AktuellesSymol >= SymAnz) {
          char str[300];
          sprintf(str,"Symbol %s Error!",dxfstr);
//          FltkMessageBox(str,"SBError",MB_OK);
          //return false;
        }
      } else if (DXFCode== 50) {						// Rotation angle
        sscanf(dxfstr,"%f\n",&RX2);
      } else if (DXFCode== 0) {
        {
          ClipperLib::cInt AX = (int)(RX * fxAufloesung);
          ClipperLib::cInt AY = (int)(RY * fxAufloesung);
          ClipperLib::cInt AZ = (int)(RZ * fxAufloesung);
          ClipperLib::cInt S  = (int)RX2;
          unget_dxf(stream,0,dxfstr);
          if (strstr(SymName,".dxf")) {
            strcpy(PfadundName,m_PfadName);
            strcat(PfadundName,SymName);
            struct stat statbuf;
            if (stat(PfadundName,&statbuf)!=0) {
              strcpy(PfadundName,m_PfadName);
              strcat(PfadundName,"Teile/");
              strcat(PfadundName,SymName);

            }
            if (stat(PfadundName,&statbuf)==0) {
              static int recursion=0;
              if (recursion==0 && statbuf.st_size) {
                if (ReadFirst) {
                  // Das erste mal include daten speichern
                  // beim zweiten mal nichts mehr weiter ,
                  // da das file schon zum verarbeiten vorgemerkt war
                  IncludeType inc;
                  inc.Scale = 1;
                  strcpy(inc.SymName,PfadundName);
                  CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
                  inc.X = AX;
                  inc.Y = AY;
                  inc.Z = AZ;
                  inc.W = OW+S;
                  strcpy(inc.SymName,PfadundName);
                  inc.mtime = statbuf.st_mtime;
                  IncludeList.push_back(inc);
                }
              }
              return true;
            }
          } else if (strstr(SymName,".stl")) {
#ifdef USE_SHADER
            CNCComputePoint(&AX,&AY,&AZ,OX,OY,OZ,OW,Scale);
            strcpy(PfadundName,m_PfadName);
            strcat(PfadundName,SymName);
            struct stat statbuf;
            if (stat(PfadundName,&statbuf)!=0) {
              strcpy(PfadundName,m_PfadName);
              strcat(PfadundName,"Teile/");
              strcat(PfadundName,SymName);

            }
            if (stat(PfadundName,&statbuf)==0) {
              for (int i=0; i< CNCSaveCnt; i++) {
                if (CNCSave[i] != NULL) {
                  if (CNCSave[i]->Art==StlObject) {
                    CStlObject * Obsave = (CStlObject * )CNCSave[i];
                    if (Obsave->m_Path!=NULL) {
                      if (strcmp(Obsave->m_Path,PfadundName)==0) {
                        if (Obsave->statbuf_mtime == statbuf.st_mtime) {
                          if (Obsave->ETiefe == Color
                              && Obsave->m_S.X == AX
                              && Obsave->m_S.Y == AY
                              && Obsave->m_S.Z == AZ
                              && Obsave->m_Zoom== SX
                              && Obsave->m_Winkel== (S+OW)) {
                            CStlObject * Ob = (CStlObject *) CNCSave[i];
                            char str[120];
                            strcpy(str,Layer);
                            strcat(str,".");
                            strcat(str,SymName);
                            Ob->m_pLayer = GetLayerIndex(this,str);
                            CNCSave[i]=NULL;
                            printf("%s\n",PfadundName);
                            if (Ob!=NULL) {
                              CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                              CNCInfoCnt++;
                            }
                            return 1;
                          }
                        }
                      }
                    }
                  }
                }
              }

              static int recursion=0;
              if (recursion==0 && statbuf.st_size) {
                char str[120];
                strcpy(str,Layer);
                strcat(str,".");
                strcat(str,SymName);
                CStlObject * Ob = new CStlObject(this,AX,AY,AZ,str,Color);
                if (Ob) {
                  if (strncasecmp(User,"alpha:",6)==0) {
                    if (Ob) Ob->m_Alpha = atof(&User[6]);
                  }
                  Ob->m_Winkel = S+OW;
                  Ob->m_Zoom = SX;
                  Ob->m_Path = (char*) malloc(strlen(PfadundName)+1);
                  strcpy(Ob->m_Path,PfadundName);
                  if (User[0]!= '\0')Ob->LoadStlFile(User);
                  else Ob->LoadStlFile(NULL);
                  Ob->statbuf_mtime = statbuf.st_mtime;
                } else   return false;

                if (Ob!=NULL) {
                  CNCInfo[CNCInfoCnt] = (CObject*) Ob;
                  CNCInfoCnt++;
                }
              }
              return true;
            }
#endif
          }

          //if (OX==0 && OY==0 && OZ ==0)
          {
            int exCNCInfoCnt = CNCInfoCnt;
            BlockAufloesen(AktuellesSymol,OX+AX,OY+AY,OZ+AZ,OW+S,(SX+SY)/2.0f);
            for (int i = exCNCInfoCnt; i <CNCInfoCnt; i++) {
              // Das Layer vom Block uerberschreiben
              CObject*  Ob = CNCInfo[i];
              CLayers * pLayer = GetLayerIndex(this,Layer);
              if (Ob && pLayer) {
                Ob->m_pLayer = pLayer;
              }
            }
          }
        }
        return true;
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::BlockEinlesen(FILE * stream)
{
  char Layer[80];
  char SubLayer[80];
//  ClipperLib::cInt  X=0;
//  ClipperLib::cInt  Y=0;
//  ClipperLib::cInt  AX=0;
//  ClipperLib::cInt  AY=0;
  float RX=0;
  float RY=0;
//  float RZ =0;
  float RX2=0;
  //float RY2=0;
  bool NameBekann=false;
  if (SymAnz<NUMSYM-1) {
    // Anfangsposition des Blocks merken
    fgetpos(stream, &Symbole[SymAnz].Filepos);
  } else return false;
  while (feof(stream )==0) {
    if (ReadCode(stream)) {
      if      (DXFCode==10) sscanf(dxfstr,"%f\n",&RX);       //DXF: X value of base point
      else if (DXFCode==20) sscanf(dxfstr,"%f\n",&RY);	    //DXF: Y value of base point
      else if (DXFCode== 8) {
        if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
          strcat(Layer,".");
          strcat(Layer,SubLayer);
        }
      } else if (DXFCode==70) sscanf(dxfstr,"%f\n",&RX2);	  //70 Block-type flags (bit coded values, may be combined):
      else if (DXFCode== 2) {
        if (NameBekann==false) {
          sscanf(dxfstr,"%s\n",Symbole[SymAnz].SymName); //2	Block name
        }
        NameBekann=true;
      } else if (DXFCode== 5) { //3	Handle
      } else if (DXFCode== 3) { //3	Block name
      } else if (DXFCode ==0) {
        if (strcmp(dxfstr,"ENDBLK")==0) {
          if (SymAnz<NUMSYM) {
            // Endposition des Blocks merken
            fpos_t fpos;
            fgetpos(stream, &fpos);
#ifdef WIN32
            Symbole[SymAnz].len =  (int)(fpos - Symbole[SymAnz].Filepos);
#else
            Symbole[SymAnz].len =  (int)(fpos.__pos - Symbole[SymAnz].Filepos.__pos);
#endif
            SymAnz++;
          } else {
//            FltkMessageBox("Symbol count Error!","SBError",MB_OK);
            return false;
          }
          return true;
        }
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
bool CObjectManager::BlockAufloesen(int Symbol,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale)
{
  FILE * stream;
  bool OK = true;
  if ((stream = fopen(m_FilePfadName,"r"))== NULL) return false;

  fsetpos(stream,&Symbole[Symbol].Filepos);

  while (feof(stream )==0 && !gStopp) {
    if (ReadCode(stream)) {
      if (DXFCode ==0) {
        if      (strcmp(dxfstr,"LINE")==0)		  LinieEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"POLYLINE")==0)	  PolyLinieEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"LWPOLYLINE")==0)  LWPolyLinieEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"CIRCLE")==0)	  CircleEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"ARC")==0)		  ArcEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"POINT")==0)		  PointEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"MTEXT")==0)		  TextEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"TEXT")==0)		  TextEinlesen(stream,X,Y,Z,W,Scale);
        else if (strcmp(dxfstr,"BLOCK")==0)	  	  BlockEinlesen(stream);
        else if (strcmp(dxfstr,"INSERT")==0) {
          InsertEinlesen(stream,X,Y,Z,W,Scale);
        } else if (strcmp(dxfstr,"ENDBLK")==0)	  break;
//		else if (strcmp(dxfstr,"HATCH")==0);
      }
    }
    //else OK = false;
  }
  fclose(stream);
  return OK;
}
//-------------------------------------------------------------
bool CObjectManager::LayerEinlesen(FILE * stream)
{
  char Layer[80];
  char SubLayer[80];
  int  Color;
  int  ColorRGB=0;
  //bool NameBekann=false;
  SubLayer[0] = '\0';

  while (feof(stream )==0) {
    if (ReadCode(stream)) {
      if (DXFCode== 2) {
        if (sscanf(dxfstr,"%s ... %s\n",Layer,SubLayer)==2) {
        }
      } else if (DXFCode== 5) {
      } else if (DXFCode== 3) {
      } else if (DXFCode==62) {
        sscanf(dxfstr,"%d\n",&Color);
      } else if (DXFCode==420) {
        sscanf(dxfstr,"%d\n",&ColorRGB);
      } else if (DXFCode ==0) {
        if (SubLayer[0]) {
          strcat(Layer,".");
          strcat(Layer,SubLayer);
        }
        CLayers * pLayer = GetLayerIndex(this,Layer);
        if (pLayer) {
          pLayer->m_NumObjects=0;
          if (ColorRGB) pLayer->m_Color=ColorRGB<<8;
          else pLayer->m_Color=(Color<0)?-Color:Color;
          int DxfSwitch = (Color>=0);
          pLayer->m_DefaultSwitch = DxfSwitch;
          if (LayerSwitchModify==0) {
            pLayer->m_LayerSwitch = DxfSwitch;
          }
        }
        unget_dxf(stream,DXFCode,dxfstr);
        return true;
      }
    }
  }
  return false;
}
//-------------------------------------------------------------
void CObjectManager::LayersFreigeben(bool Alle)
{
  if (Alle) {
    if (gpLayers) delete gpLayers;
    gpLayers=NULL;
  } else {
    CLayers * pLayer = gpLayers;
    while  (pLayer) {
      pLayer->m_NumObjects = 0;
      pLayer = pLayer->m_NextLayer;
    }
  }
}

//-------------------------------------------------------------
/*
static char GcodeStr[255];

bool CObjectManager::ReadCommand(FILE * stream)
{
  if (feof(stream )!=0) return false;
  if (fgets(GcodeStr,sizeof(GcodeStr)-1,stream)==0) return false;
  strtok(GcodeStr,"\n\r");
  return true;
}
 */
//-------------------------------------------------------------

bool CObjectManager::ReadCode(FILE * stream)
{
  char str[20];
  if (is_unget(stream)==false) {
    if (feof(stream )!=0) return false;
    if (fgets(str,sizeof(str)-1,stream)==0) return false;
    DXFCode = atoi(str);
    if (fgets(dxfstr,sizeof(dxfstr)-1,stream)==0) return false;
    strtok(dxfstr,"\n\r");
  }
  return true;
}

//-------------------------------------------------------------
bool CObjectManager::CNCComputePoint(ClipperLib::cInt * pX,ClipperLib::cInt * pY,ClipperLib::cInt * pZ,ClipperLib::cInt DXOffset,ClipperLib::cInt DYOffset,ClipperLib::cInt DZOffset,int Winkel,float Scale)
{
  *pX = round05((float)*pX*Scale);
  *pY = round05((float)*pY*Scale);
  *pZ = round05((float)*pZ*Scale);
  if (Winkel==0) {
    *pX += DXOffset;
    *pY += DYOffset;
    *pZ += DZOffset;
  } else {
    float  Drehung;
    Drehung  =  (float)(Winkel * M_PI / 180.0);
    ClipperLib::cInt X;
    ClipperLib::cInt Y;
    X = DXOffset+(ClipperLib::cInt )(*pX *cos(Drehung) - *pY *sin(Drehung));
    Y = DYOffset+(ClipperLib::cInt )(*pY *cos(Drehung) + *pX *sin(Drehung));
    *pX = X;
    *pY = Y;
    *pZ += DZOffset;
  }
  return 1;
}

//-------------------------------------------------------------
void CObjectManager::SaveFreigeben(void)
{
  for (int i=0; i< CNCSaveCnt; i++) {
    if (CNCSave[i] != NULL) {
      delete CNCSave[i];
      CNCSave[i] = NULL;
    }
  }
  CNCSaveCnt=0;
}

//-------------------------------------------------------------
void CObjectManager::CNCFreigeben(void)
{
  int i;
  for (i=0; i< CNCInfoCnt; i++) {
    if (CNCInfo[i] != NULL) {
      if (CNCInfo[i]->Art==StlObject) {
        if (((CStlObject*)CNCInfo[i])->m_Path!=NULL) {
          if (CNCSaveCnt < ANZAHLSAVE) {
            CNCSave[CNCSaveCnt] = CNCInfo[i];
            CNCSaveCnt++;
            CNCInfo[i]=NULL;
            continue;
          }
        }
      }
      delete CNCInfo[i];
      CNCInfo[i] = NULL;
    }
  }
#ifndef  USEVECTOR
  memset(&CNCInfo,0,sizeof(CNCInfo));
#endif
  CNCInfoCnt =0;
  MinMaxleft   = 0x7FFFFFFF;
  MinMaxtop    = 0x7FFFFFFF;
  MinMaxright  = 0x80000000;
  MinMaxbottom = 0x80000000;
  MinMaxfront  = 0x7FFFFFFF;
  MinMaxback   = 0x80000000;
  m_InUse      = 1;

}

//-------------------------------------------------------------
int CObjectManager::TestForUpdate()
{
#ifdef USE_SHADER
  int res=0;

  unsigned cnt = (unsigned)IncludeList.size();
  int i;
  for (i =1; i < IncludeList.size(); i++) {
    IncludeType inc;
    inc = IncludeList[i];
    struct stat statbuf;
    stat(inc.SymName,&statbuf);
    if (statbuf.st_mtime != inc.mtime) {
      IncludeList[i].cmprcnt++;
      if (IncludeList[i].cmprcnt>2) {
        ReadFirst=0;
        inc.m_ObjectManager->CNCFreigeben();
        inc.m_ObjectManager->CNCInfoCnt=0;
        inc.m_ObjectManager->DXFIncludeEinlesen(inc.SymName,inc.X,inc.Y,inc.Z,inc.W,inc.Scale);
        IncludeList[i].mtime = statbuf.st_mtime;
        res =1;
      }
    } else IncludeList[i].cmprcnt=0;
  }
  return res;

// TODO  Stl service

  for (i=1; i< CNCInfoCnt; i++) {       // nur ein stl Oobject nicht testen
    if (CNCInfo[i] != NULL) {
      if (CNCInfo[i]->Art==StlObject) {
        CStlObject* ob =(CStlObject*)CNCInfo[i];
        if (ob->m_Path!=NULL && ob->m_pLayer->m_LayerSwitch) {
          char TestFileName[MAX_PATH];
          strcpy(TestFileName,ob->m_Path);
          char *ext;
          ext = strrchr(TestFileName, '.');
          if (ext) {
            strcpy(ext,".dxf");
            struct stat statbuf;
            if (stat(TestFileName, &statbuf)==0) {
              if (statbuf.st_size > 10000 && statbuf.st_mtime > (ob->statbuf_mtime+3)) {
                printf("neuer %s\n",TestFileName);
                CLayers * LayeSave = gpLayers;
                int LayerSwitchModifySave = LayerSwitchModify;
                LayerSwitchModify = 0;
                gpLayers=NULL;
                CObjectManager    * TempObjectManager =  new CObjectManager ();
                strcpy(TempObjectManager->m_FilePfadName,TestFileName);
                TempObjectManager->CNCInfoCnt=0;
                // Pfad fuer Include files
                strcpy (TempObjectManager->m_PfadName,m_PfadName);
                TempObjectManager->DXFEinlesen();
                AdjustLayerSwitches();
                m_3DreiTafelProjektionStartcnt = CNCInfoCnt;
                CLayers *i3DreiTafelLayer = IsLayerIndex(this,"DREITAFEL");
                if (i3DreiTafelLayer!=NULL) {
                  TempObjectManager->Make3DreiTafelProjektion(0,0,0,0);
                }
                if (0) {
                  for (int j=0; j< TempObjectManager->CNCInfoCnt; j++) {
                    if (TempObjectManager->CNCInfo[j] != NULL) {
                      if (TempObjectManager->CNCInfo[j]->Art==StlObject) {
                        CStlObject* ob2 =(CStlObject*)TempObjectManager->CNCInfo[j];
                        ob2->m_pLayer = ob->m_pLayer;
                        delete CNCInfo[i];
                        CNCInfo[i] = ob2;
                        ob2->statbuf_mtime = statbuf.st_mtime;
                        ob2->m_Path = (char*) malloc(strlen(TestFileName)+1);
                        strcpy(ob2->m_Path,TestFileName);
                        TempObjectManager->CNCInfo[j]=NULL;
                        res++;
                      }
                    }
                  }

                } else {
                  extern void SaveStlPolygons(CObjectManager     * pObjectManager,int binary);

                  char PfadName[256];
                  strcpy (PfadName,TestFileName);
                  char *ext2 = strrchr(PfadName, '.');
                  if (ext2) {
                    strcpy(ext2,".stl");
                    if (savebinary) {
                      gWrk3DSheet->m_file = fopen(PfadName, "wb");
                      if(gWrk3DSheet->m_file) {
                        char Header[80]= {0};
                        fwrite(Header,80,1,gWrk3DSheet->m_file);
                        fwrite(&NumVertextoSave,4,1,gWrk3DSheet->m_file);
                        NumVertextoSave=0;
                        SaveStlPolygons(TempObjectManager,1);
#ifdef WIN32
                        fpos_t  pos = 80;
#else
                        fpos_t  pos = {80,0};
#endif
                        fsetpos(gWrk3DSheet->m_file,&pos);
                        fwrite(&NumVertextoSave,4,1,gWrk3DSheet->m_file);
                        fclose(gWrk3DSheet->m_file);
                        res++;
                      }
                    }
                    ob->LoadStlFile(NULL);
                    ob->statbuf_mtime = statbuf.st_mtime;
                  }
                }
                TempObjectManager->CNCFreigeben();
                delete TempObjectManager;
                delete gpLayers;
                gpLayers = LayeSave;
                LayerSwitchModify = LayerSwitchModifySave;
              }
            }
          }
        }
      }
    }
  }
  return res;
#else
  return 0;
#endif
}
//-------------------------------------------------------------
void CObjectManager::Sort_Reset(void)
{
  for (int Index = 0 ; Index < CNCInfoCnt; Index++) {
    if (CNCInfo[Index]) CNCInfo[Index]->m_Gefunden=0;
  }


  /*  for (int I=0; I<=CNCInfoCnt; I++)
    {
      CNCSort[I].SIndex = 0;
      //CNCSort[I].Abheben= true;
      CNCSort[I].Vertauschen= false;
      CNCSort[I].Gefunden = false;
    }
    Sort_Index = 0;
    */
  KurzX = 0;
  KurzY = 0;
//  extern HWND hWndToolbar;
//  SendMessage(hWndToolbar,TB_SETSTATE, IDM_Tiefe1,((AktTiefe==1)?TBSTATE_CHECKED:0) | TBSTATE_ENABLED);
//  SendMessage(hWndToolbar,TB_SETSTATE, IDM_Tiefe2,((AktTiefe==2)?TBSTATE_CHECKED:0) | TBSTATE_ENABLED);
//  SendMessage(hWndToolbar,TB_SETSTATE, IDM_Tiefe3,((AktTiefe==3)?TBSTATE_CHECKED:0) | TBSTATE_ENABLED);
//  SendMessage(hWndToolbar,TB_SETSTATE, IDM_Tiefe4,((AktTiefe==4)?TBSTATE_CHECKED:0) | TBSTATE_ENABLED);
//  SendMessage(hWndToolbar,TB_SETSTATE, IDM_Tiefe5,((AktTiefe==5)?TBSTATE_CHECKED:0) | TBSTATE_ENABLED);
}
//-------------------------------------------------------------
void CObjectManager::Sort_exec(void)
{
  // die Funktion muss CNCInfoCnt mal aufgerufen werden
//  CNCSort[Sort_Index].SIndex = Sort_Index++;
//return ;
  /*  ClipperLib::cInt AbstandS,AbstandE,Vergleich;
    int TestIndex;
    int FoundIndex;
    CObject  Obj;
    bool Keine = true;
    if (Sort_Index < CNCInfoCnt)
    {
      Sort_Index++;
      Vergleich  = 32000000l;
      FoundIndex = 0;
      for (TestIndex=1; TestIndex <= CNCInfoCnt; TestIndex++)
      {
        if (!CNCSort[TestIndex].Gefunden)
        {
          Obj = *((CNCInfo)[TestIndex]);
          if (Obj.ETiefe ==  AktTiefe)
          {
            Keine = false;
            //Abstand1 = labs(labs(KurzX)-labs(Obj.S.x))+labs(labs(KurzY)-labs(Obj.S.y));
            //Abstand2 = labs(labs(KurzX)-labs(Obj.E.x))+labs(labs(KurzY)-labs(Obj.E.y));
            if ((m_Mode & Linie) && (Obj.Art & 0x3F) == Linie )
            {
              float AxS  = KurzX - Obj.S.x;
              float AyS  = KurzY - Obj.S.y;
              AbstandS = sqrt((AxS*AxS)+(AyS*AyS));
              float AxE  = KurzX - Obj.E.x;
              float AyE  = KurzY - Obj.E.y;
              AbstandE = sqrt((AxE*AxE)+(AyE*AyE));
              if (AbstandS < Vergleich || AbstandE < Vergleich)
              {
                if (AbstandS <= AbstandE)
                {
                  Vergleich = AbstandS;
                  CNCSort[Sort_Index].Vertauschen = false;
                  //CNCSort[Sort_Index].Abheben  = (AbstandS != 0);
                }
                else
                {
                  Vergleich = AbstandE;
                  CNCSort[Sort_Index].Vertauschen = true;
                  //CNCSort[Sort_Index].Abheben =  (AbstandE != 0);
                }
                FoundIndex = TestIndex;
                //CNCSort[Sort_Index].SIndex = TestIndex;
              }
              if (AbstandS == 0 || AbstandE == 0) break;      // Stimmt exact
            }
            else if (m_Mode != Linie && (Obj.Art & 0x3F) != Linie )
            {
              if (KurzX < 0 || KurzY< 0 || Obj.S.x < 0|| Obj.S.y < 0)
              {
                KurzX=0;
              }
              float AxS  = KurzX - Obj.S.x;
              float AyS  = KurzY - Obj.S.y;
              AbstandS = sqrt((AxS*AxS)+(AyS*AyS));
              if (AbstandS <= Vergleich)
              {
                Vergleich = AbstandS;
                CNCSort[Sort_Index].Vertauschen = false;
                //CNCSort[Sort_Index].Abheben     = 1;//(AbstandS != 0);
                FoundIndex = TestIndex;
                //CNCSort[Sort_Index].SIndex      = TestIndex;
              }
            }
          }
        }
      }
      if (FoundIndex) //CNCSort[Sort_Index].SIndex != 0)
      {
        CNCSort[Sort_Index].SIndex  = FoundIndex;
        Obj = *((CNCInfo)[FoundIndex]);
        CNCSort[FoundIndex].Gefunden=true;
        if (CNCSort[Sort_Index].Vertauschen || (Obj.Art & 0x3F) != Linie)
  //      Obj = *((CNCInfo)[FoundIndex]);
  //      CNCSort[Sort_Index].Gefunden= true;
  //      if (CNCSort[Sort_Index].Vertauschen || (Obj.Art & 0x3F) != Linie)
        {
          KurzX = Obj.S.x;	// End Position einer Linie
          KurzY = Obj.S.y;
        }
        else
        {
          KurzX = Obj.E.x;
          KurzY = Obj.E.y;
        }
      }
    }
    char str[40];
    if (Keine)Sort_Index = 32000;
    */
}
//-------------------------------------------------------------
//-------------------------------------------------------------
// TODO
void CObjectManager::Leiterbahnenerzeugen(void) {;}
bool CObjectManager::GerberEinlesen(void)
{
  return 0;
}
bool CObjectManager::ExellonEinlesen(void)
{
  return 0;
}
bool CObjectManager::DXFSpezialEinlesen(void)
{
  return 0;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
/*
typedef unsigned char  uint8_t ;
typedef unsigned short uint16_t;
typedef struct FloatPoint {
  float x;
  float y;
  float z;
} FloatPoint;

/ *static FloatPoint current_units;
static FloatPoint target_units;
static float feedrate = 0.0;
* /
static long ActualFeddrate = 0;
static long courfeedrate = 0;

static char  abs_mode = true;   //0 = incremental; 1 = absolute

double search_string(char key, char instruction[], int string_size);

char has_command(char key, char instruction[], int string_size);
/ *
void set_target2(float x, float y, float z)
{
  target_units.x = x;
  target_units.y = y;
  target_units.z = z;
  current_units = target_units;
}

void set_position2(float x, float y, float z)
{
//  current_units.x = x;
//  current_units.y = y;
//  current_units.z = z;
}
* /


//look for the number that appears after the char key and return it
double search_string(char key, char instruction[], int string_size)
{
  char temp[10] = "";

  for (uint8_t i=0; i<string_size; i++) {
    if (toupper(instruction[i]) == key) {
      i++;
      int k = 0;
      while (i < string_size && k < 10) {
        if (instruction[i] == 0 || (k>3 && instruction[i] == ' '))
          break;

        temp[k] = toupper(instruction[i]);
        i++;
        k++;
      }
      return strtod(temp, NULL);
    }
  }

  return 10000;
}

//look for the command if it exists.
char has_command(char key, char instruction[], int string_size)
{
  for (uint8_t i=0; i<string_size; i++) {
    if (toupper(instruction[i]) == key)
      return true;
  }

  return false;
}



bool CObjectManager::GCodeEinlesen(void)
{
  fxAufloesung = (float)gPa.Aufloesung;
  if (fxAufloesung < 1 || fxAufloesung  > 5000) fxAufloesung  = 100.0;
  if (Zoll)fxAufloesung *= 25.4f;
  FILE * stream;
  memset(&Symbole,0,sizeof(Symbole));
  SymAnz     = 0;
  ungetCount = 0;
  CNCFreigeben();
  SaveFreigeben();
  LayersFreigeben(true);
  gStopp = false;
  int Index;
  FloatPoint Current_Pos;
  Current_Pos.x = 0.0;
  Current_Pos.y = 0.0;
  Current_Pos.z = 0.0;
  int   PStart = 1;
  int   Color  = 1;
  int G0Feddrate = 6000;    // 100 mm /Sec
  int G1Feddrate = 180;
  int M3_4PWM    = 255;
  int MPWM       = 255;
  int bLaserMode = 0;
  struct stat st;
  stat(m_FilePfadName,&st);
#ifdef WIN32
  time = st.st_mtime;
#else
  time = st.st_mtim.tv_sec;
#endif
//  SetBusyDisplay(st.st_size,"Gcode einlesen");
  ActualFeddrate = courfeedrate = gPa.Tempo[Tempo_Verfahr];
  uint16_t Lastcode = 0;;

  if ((stream = fopen(m_FilePfadName,"r"))== NULL) return false;

  if (CNCInfoCnt >= AnzahlCNC2-1) gStopp = true;
  else {
    //CNCInfo[CNCInfoCnt] = new CGCodeLineObject(Current_Pos.x * 100.0,Current_Pos.y * 100.0,Current_Pos.z * 100.0,"GCode",ActualFeddrate == Pa.Tempo[Tempo_Verfahr]?7:Color);
    CObject * Ob = NULL;// CNCInfo[CNCInfoCnt];
    //CNCInfoCnt++;
    //if (Ob)
    {
      while (feof(stream )==0 && !gStopp && CNCInfoCnt < AnzahlCNC2) {
        if (ReadCommand(stream)) {

          char * instruction = GcodeStr;
          int size = strlen(GcodeStr);
          if (instruction[0] == '/' || instruction[0] == ';') {
            //printf("ok\n");
            continue;
          }

          //init baby!
          FloatPoint fp;
          fp.x = 0.0;
          fp.y = 0.0;
          fp.z = 0.0;

          uint16_t code = 0;;
          //did we get a gcode?
          if (has_command('G', instruction, size)|| has_command('X', instruction, size)) {
            code = (int)search_string('G', instruction, size);
            if (code == 10000) code = Lastcode;

            // Get co-ordinates if required by the code type given
            switch (code) {
            case 0:
            case 1:
            case 2:
            case 3:
              Lastcode=code;
              if(abs_mode) {
                //we do it like this to save time. makes curves better.
                //eg. if only x and y are specified, we dont have to waste time looking up z.
                if (has_command('X', instruction, size))
                  fp.x = search_string('X', instruction, size);
                else
                  fp.x = Current_Pos.x;

                if (has_command('Y', instruction, size))
                  fp.y = search_string('Y', instruction, size);
                else
                  fp.y = Current_Pos.y;

                if (has_command('Z', instruction, size))
                  fp.z = search_string('Z', instruction, size);
                else
                  fp.z = Current_Pos.z;
              } else {
                if (has_command('X', instruction, size))
                  fp.x = search_string('X', instruction, size) + Current_Pos.x;
                if (has_command('Y', instruction, size))
                  fp.y = search_string('Y', instruction, size) + Current_Pos.y;
                if (has_command('Z', instruction, size))
                  fp.z = search_string('Z', instruction, size) + Current_Pos.z;
              }
              if (has_command('S', instruction, size)) {
                MPWM = M3_4PWM = (search_string('S', instruction, size));
              }
              break;
            }

            //do something!
            switch (code) {
            //Rapid Positioning
            //Linear Interpolation
            //these are basically the same thing.
            case 0:
            case 1:
              //set our target.
              if (1)  {
                //Current_Pos  = fp; //set_target(fp.x, fp.y, fp.z);
                if (code == 1) {
                  //adjust if we have a specific G1Feddrate.
                  if (has_command('F', instruction, size))
                    G1Feddrate  = search_string('F', instruction, size);
                  if (ActualFeddrate != G1Feddrate) PStart = 1;
                  ActualFeddrate = G1Feddrate;
                } else {
                  if (has_command('F', instruction, size))
                    G0Feddrate  = search_string('F', instruction, size);
                  ActualFeddrate = G0Feddrate;
                }
                //finally move.
                //return StartMove(ActualFeddrate);
                if (code) {
                  if (PStart) {
                    if (CNCInfoCnt <= AnzahlCNC2-1) {
                      //CNCInfo[CNCInfoCnt] = new CGCodeLineObject(Current_Pos.x * 100.0,Current_Pos.y * 100.0,Current_Pos.z * 100.0,(char*)"GCode",ActualFeddrate == gPa.Tempo[Tempo_Verfahr]?7:Color);
                      CNCInfo[CNCInfoCnt] = new CGCodeLineObject(this,Current_Pos.x * 100.0,Current_Pos.y * 100.0,Current_Pos.z * 100.0,(char*)"GCode",ActualFeddrate >= G0Feddrate?7:Color);

                      Ob = CNCInfo[CNCInfoCnt];
                      CNCInfoCnt++;
                      if (Ob) {
                        Ob->AddVertex((long)(Current_Pos.x * 100.0),(long)(Current_Pos.y * 100.0),(long)(Current_Pos.z * 100.0),ActualFeddrate >= gPa.Tempo[Tempo_Verfahr]?1:MPWM);
                        ((CGCodeLineObject*)Ob)->m_G0Feddrate = G0Feddrate;
                        ((CGCodeLineObject*)Ob)->m_G1Feddrate = G1Feddrate;
                      }
                    }
                  }
                  if (Ob) {
                    Ob->AddVertex((long)(fp.x * 100.0),(long)(fp.y * 100.0),(long)(fp.z * 100.0),0);
                    int ixp = ((CGCodeLineObject*)Ob)->m_Path.size()-1;
((CGCodeLineObject*)Ob)->m_Path[ixp].Attr = ActualFeddrate >= gPa.Tempo[Tempo_Verfahr]?1:MPWM;
                  }
                  PStart=0;
                } else {
                  PStart = 1;
                }

//                Ob->AddVertex((long)(fp.x * 100.0),(long)(fp.y * 100.0),(long)(fp.z * 100.0),0);
//                if (courfeedrate != ActualFeddrate && Ob->GetNumItems()>1) {
//                  if (CNCInfoCnt <= AnzahlCNC2-1) {
//                    //if (ActualFeddrate < G0Feddrate ) {
//                    CNCInfo[CNCInfoCnt] = new CGCodeLineObject((long)(fp.x * 100.0),(long)(fp.y * 100.0),(long)(fp.z * 100.0),"GCode",
//                      ActualFeddrate == Pa.Tempo[Tempo_Verfahr]?7:Color);
//                    courfeedrate = ActualFeddrate;
//                    Ob = CNCInfo[CNCInfoCnt];
//                    CNCInfoCnt++;
//                    Ob->AddVertex((long)(fp.x * 100.0),(long)(fp.y * 100.0),(long)(fp.z * 100.0),0);
//                    //}
//                  }
//                }
//                PStart=0;
                Current_Pos.x = fp.x;
                Current_Pos.y = fp.y;
                Current_Pos.z = fp.z;
              }
              break;

            //Clockwise arc
            case 2:
            //Counterclockwise arc
            case 3: {
              FloatPoint cent;
              // Centre coordinates are always relative
              cent.x = search_string('I', instruction, size) + Current_Pos.x;
              cent.y = search_string('J', instruction, size) + Current_Pos.y;
              if (has_command('F', instruction, size))
                G1Feddrate  = search_string('F', instruction, size);
              ActualFeddrate = G1Feddrate;

              float radius, aX, aY, bX, bY;

              aX = (Current_Pos.x - cent.x);
              aY = (Current_Pos.y - cent.y);
              bX = (fp.x - cent.x);
              bY = (fp.y - cent.y);

              float angleA, angleB;
              if (bX==0 && bY==0) {
                angleA = atan2(aY, aX);
                if (code == 2) { // Clockwise
                  angleB = angleA + 2 * M_PI;
                } else { // Counterclockwise
                  angleB  = angleA;
                  angleA += 2 * M_PI;
                }
              } else {
                angleA = atan2(bY, bX);
                angleB = atan2(aY, aX);
                if (code == 2) { // Clockwise
                  if (angleB <= angleA) angleB += 2 * M_PI;
                } else { // Counterclockwise
                  if (angleA <= angleB) angleA += 2 * M_PI;
                }
              }
              radius = sqrt(aX * aX + aY * aY);
              Current_Pos  = fp; //set_target(fp.x, fp.y, fp.z);
              //return StartCircularMove(ActualFeddrate,angleA,angleB,radius);
            }
            break;

            //Dwell
            case 4:
              //delay((int)search_string('P', instruction, size));
              break;

            //Inches for Units
            case 20:
              break;
            //mm for Units
            case 21:
              break;
            //go home.
            case 28:
//            Nullen = 0;
//            if (has_command('X', instruction, size)) Nullen |= 0x03;
//            if (has_command('Y', instruction, size)) Nullen |= 0x0C;
//            if (has_command('Z', instruction, size)) Nullen |= 0x30;
//            RunMode= REFERENZLAUF;
//            SetSpeed(6000);
//            HandshakeCode='0';
//            if (Nullen==0) Nullen =0x3F;
//            MovingPendent=1;
//            return;
              break;

            case 61: // G61   Exact stop check, modal   M   T   Can be canceled with G64. The non-modal version is G09.
            case 62: // G62   Automatic corner override   M   T
            case 64: // G64   Default cutting mode (cancel exact stop check mode)   M   T   Cancels G61.
              break;


            //Absolute Positioning
            case 90:
              abs_mode = true;
              break;

            //Incremental Positioning
            case 91:
              abs_mode = false;
              break;

            //Set Relative Home
            case 92:
              if (has_command('X', instruction, size)
                  ||  has_command('Y', instruction, size)
                  ||  has_command('Z', instruction, size)) {
//                / * SetOffsetActualPos(
//                search_string('X', instruction, size),
//                search_string('Y', instruction, size),
//                search_string('Z', instruction, size));* /
              }
              break;

            default:
              printf("huh? G %d\n",code);
            }
          } else if (has_command('M', instruction, size)) {   //find us an m code.
            code = search_string('M', instruction, size);
            switch (code) {

            case 0:
              //todo: stop program
              break;

            case 1:
              //todo: optional stop
              break;

            case 2:
              //todo: program end
              break;

            case 3:
            // M3: Spindle On, Clockwise (CNC specific)
            //        break;
            case 4:
              // M4: Spindle On, Counter-Clockwise (CNC specific)
            {
              if (has_command('S', instruction, size)) {
                M3_4PWM = (search_string('S', instruction, size));
              }
              //Spindle(M3_4PWM );
              MPWM = M3_4PWM;
            }
            break;
            case 5:
              // M5: Spindle Off (CNC specific)
              //Spindle(0);
              MPWM = 0;
              break;


            case 6: // M6: Tool change
              PStart=1;
              Color++;
              break;


            case 7: // M7: Mist Coolant On (CNC specific)
            case 8: // M8: Flood Coolant On (CNC specific)
            case 9: // M9: Coolant Off (CNC specific)
              break;
            //set max extruder speed, 0-255 PWM
            case 100:
              //extruder_speed = (int)(search_string('P', instruction, size));
              break;


            //turn extruder on, forward
            case 101:
              //extruder_set_direction(1);
              //extruder_set_speed(extruder_speed);
              break;

            //turn extruder on, reverse
            case 102:
              //extruder_set_direction(0);
              //extruder_set_speed(extruder_speed);
              break;

            //turn extruder off
            case 103:
              //extruder_set_speed(0);
              break;

            //custom code for temperature control
            case 104:
              / *extruder_set_temperature((int)search_string('P', instruction, size));

                //warmup if we're too cold.
                while (extruder_get_temperature() < extruder_target_celsius)
                {
                extruder_manage_temperature();
                printf("T: %d\n",extruder_get_temperature());
                delay(1000);
                }
                * /
              break;
            //turn fan on
            case 106:
              //extruder_set_cooler(255);
              break;

            //turn fan off
            case 107:
              //extruder_set_cooler(0);
              break;
            case 82:
              // M82: Set extruder to absolute mode
              break;
            case 109:
              // M109: Set Extruder Temperature and Wait
              break;

            //custom code for temperature reading
            case 105:
              printf("ok T:500.0 /0.0 B:0.0 /0.0\n");   // CNC Model
              //        return;
              //printf("ok T:%d\n",201);//extruder_get_temperature());
              break;
            case 114:
              printf("ok\n");
              //        SendCurrentPos();
              break;
            case 115:
              break;
            case 551:  // m_Select FFF Printer Mode
              bLaserMode = 2;
              break;
            case 552:  // m_Select Laser Printer Mode
              bLaserMode = 1;
              break;
            case 553:  // m_Select CNC Printer Mode
              bLaserMode = 0;
              break;
            case 999:
              //         GCodeMode =0;
              break;
            case 1005:
              //printf("Firmware Version: Bastel.Inc 1.1.0\nRelease Date Jul 23 1019\n");
              printf("Firmware Version: Snapmaker-Base-2.2\nRelease Date: Mar 27 2018\n");
              break;
            case 1006:
              break;
            case 10000:
              break;
            default:
              printf("Huh? M %d\n",code);
              break;
            }
          } else if (has_command('F', instruction, size)) {
            G1Feddrate  = search_string('F', instruction, size);
            ActualFeddrate = G1Feddrate;
          } else if (has_command('S', instruction, size)) {
            MPWM = M3_4PWM = (search_string('S', instruction, size));
          }
//        if ((CNCInfoCnt & 0x3F)==0) AdjustBusyDisplay(ftell(stream));
        }
      }
    }

  }
//  if   (!gStopp) SetBusyStatus("Ok");
//  else          SetBusyStatus("Abbruch");

  fclose(stream);
  return true;
}
*/
//-------------------------------------------------------------
int  CPolyLineObject::IsIxCrossedRect(int ix,int left,int top,int right,int bottom)
{
  if (ix < 1) return 0;
  if (ix >= m_Path.size()) return 0;
  int x1 = m_Path[ix-1].X;
  int x2 = m_Path[ix].X;
  int y1 = m_Path[ix-1].Y;
  int y2 = m_Path[ix].Y;

  int a  = x2 - x1;
  int b  = y2 - y1;
  if (x2 == x1) {
    // Vertikal
    //ETiefe = 1;
    if (x2 < left || x2 > right) return 0;
    if (y2 > y1) {
      if (y2 < bottom || y1 > top) return 0;
    } else {
      if (y1 < bottom || y2 > top) return 0;
    }
    return b>0?4:2;//CNCInfo[Index]->SetSelect(iSel);
  } else if (y2 == y1) {
    // Horizontal
    //ETiefe = 2;
    if (y2 < bottom || y2 > top) return 0;
    if (x2 > x1) {
      if (x2 < left || x1 > right) return 0;
    } else {
      if (x1 < left || x2 > right) return 0;
    }
    return a>0?3:1;//CNCInfo[Index]->SetSelect(iSel);
  } else  if  (a >0) {
    //continue;
    if (b>0) {
      // 0 - 90
      //ETiefe = 3;
      int yy = bottom-y1;
      int yyy= top -y1;
      int xx = left-x1;
      int xxx= right -x1;

      if (xxx < 0 && xx < 0 ) return 0;
      if (yyy < 0 && yy < 0 ) return 0;

      int h  = xx * b / a;
      int w  = yy * a / b;
      if ( (w > xx) && (w < xxx)) {
        if (a > w ) {
          return 13;
        }
      } else if ((h > yy) && (h < yyy)) {
        if (b> h) {
          return 14;
        }
      }
    } else {
      // 270 - 360
      //ETiefe = 6;
      int yy = top-y1;
      int yyy= bottom -y1;
      int xx = left-x1;
      int xxx= right -x1;

      if (xxx < 0 && xx < 0 ) return 0;
      if (yyy > 0 && yy > 0 ) return 0;

      int h  = xx * b / a;
      int w  = yy * a / b;
      if ( (w > xx) && (w < xxx)) {
        if (a > w ) {
          return 15;
        }
      } else if ((h < yy) && (h > yyy)) {
        if (b< h) {
          return 16;
        }
      }
    }
  } else {
    if (b>0) {
      // 90 - 180
      //ETiefe = 4;
      int yy = bottom-y1;
      int yyy= top -y1;
      int xx = right -x1;
      int xxx= left-x1;

      if (xxx > 0 && xx > 0 ) return 0;
      if (yyy < 0 && yy < 0 ) return 0;

      int h  = xx * b / a;
      int w  = yy * a / b;
      if ( (w < xx) && (w > xxx)) {
        if (a < w ) {
          return 17;
        }
      } else if ((h > yy) && (h < yyy)) {
        if (b> h) {
          return 18;
        }
      }
    } else {
      // 180 - 270
      //ETiefe = 5;
      int yy = top-y1;
      int yyy= bottom -y1;
      int xx = right -x1;
      int xxx= left-x1;

      if (xxx > 0 && xx > 0 ) return 0;
      if (yyy > 0 && yy > 0 ) return 0;

      int h  = xx * b / a;
      int w  = yy * a / b;
      if ( (w < xx) && (w > xxx)) {
        if (a < w ) {
          return 19;
        }
      } else if ((h < yy) && (h > yyy)) {
        if (b< h) {
          return 110;
        }
      }
    }
  }
  return 0;
}

//----------------------------------
int CPolyLineObject::IsCrossedRect(LPRECT rc)
{
  ClipperLib::cInt left,bottom,right,top;

  if (rc->right > rc->left) {
    right = rc->right;
    left  = rc->left;
  } else {
    left  = rc->right;
    right = rc->left;
  }
  if (rc->top > rc->bottom) {
    top    = rc->top;
    bottom = rc->bottom;
  } else {
    bottom = rc->top;
    top    = rc->bottom;
  }
  for (int i=1; i< m_Path.size(); i++) {
    m_SelectIx = i-1;
    if (IsIxCrossedRect(i,left,top,right,bottom)) return 1;
  }
  m_SelectIx = -1;
  return 0;
}

void CObjectManager::SetSelection(LPRECT rc)
{
  gNewSelection++;
  gnumSelect=0;
  int iSel=Sel;
  if (Fl::event_state() & FL_SHIFT) iSel=UnSel;

  if (rc->left==rc->right || rc->top==rc->bottom) {
    // All Objects
    for(int Index=0; Index < CNCInfoCnt; Index++) {
      if (CNCInfo[Index]) {
        CNCInfo[Index]->SetSelect(1);
        gnumSelect++;
      }
    }
  } else if (rc->left > rc->right && rc->bottom > rc->top) {
    //  Selection from right bottom  to left top
    for(int Index=0; Index < CNCInfoCnt; Index++) {
      if (CNCInfo[Index]!=NULL) {
        CObject * Ob = CNCInfo[Index];
        if ((Ob->Art & 0x3F) == Linie) {
          if (((CPolyLineObject*)Ob)->IsCrossedRect(rc)) {
            Ob->SetSelect(iSel);
          }
        } else if (((rc->right <= Ob->m_S.X) && (rc->left >=Ob->m_S.X))
                   &&((rc->top <= Ob->m_S.Y) && (rc->bottom >=Ob->m_S.Y))) {
          Ob->SetSelect(iSel);
        }


      }
    }
  } else {
    // Inside Rect
    for(int Index=0; Index < CNCInfoCnt; Index++) {
      if (CNCInfo[Index]) {
        CObject * Ob = CNCInfo[Index];
        if ( rc->left   <= Ob->m_Xmin && rc->right >= Ob->m_Xmin
             && rc->bottom <= Ob->m_Ymax && rc->top   >= Ob->m_Ymax
             && rc->left   <= Ob->m_Xmax && rc->right >= Ob->m_Xmax
             && rc->bottom <= Ob->m_Ymin && rc->top   >= Ob->m_Ymin) {
          Ob->SetSelect(iSel);
        }
      }
    }
  }
}

//-------------------------------------------------------------
void CObjectManager::Selection(int mode)
{
  int FirstSelection = mode;
  if (mode==-1 && CNCInfo[0]) FirstSelection = ! CNCInfo[0]->IsSelected();
  if (FirstSelection==0) gnumSelect=0;
  for(int Index=0; Index < CNCInfoCnt; Index++) {
    if (CNCInfo[Index]) {
      CNCInfo[Index]->SetSelect(FirstSelection);
    }
  }
  char str[80];
  sprintf(str,"Selection %d Items",gnumSelect);
  //SetStatusText(str,2,0);

}
//-------------------------------------------------------------

#if !defined(AFX_OBJECT_H__4ABD9417_9A09_41B4_B3AA_B350C7D625D9__INCLUDED_)
#define AFX_OBJECT_H__4ABD9417_9A09_41B4_B3AA_B350C7D625D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//typedef  int ClipperLib::cInt;
#include "basteltype.h"
#include "Layers.h"
//#include <FL/gl.h>

//#include "CncSetup.h"
#include "clipper.h"
#define NUMSYM  250
#define  USE_GLEW
#if defined(WIN32)
# define GLEW_STATIC 1
#endif
#include <GL/glew.h>


extern int  gnumSelect;
extern int  gDreiDFace;
extern int  bShowNormale;
extern int  bArrangeZoomCenter;
extern int  bYUpp;
extern int savebinary;
extern DWORD NumVertextoSave;

//typedef char * LPSTR;
//typedef uint32_t DWORD;
extern int AktTiefe;
extern unsigned int gFarbe256[256];
extern LAYERTYPE WorkLayer;
extern char  szWorkLayer[256];
extern float gGrayOut;
extern float gAlphaTransp;
extern int   gDreiDFace;
extern float fxAufloesung;
class CObjectManager;
#define  Draw        0x0
#define  Linie	     0x1
#define  Kreisbogen  0x4
#define  Buchstaben  0x8
#define  Punkt       0x10
#define  DreiDFace   0x20
#define  GCodeLinine 0x40
#define  LaserGravur 0x80
#define  Weggemacht 0x100
#define  NullPunkt  0x200
#define  WerkzeugEinrichten  0x400
#define  FahreGrenzenAb  0x800
#define  SavePolygon     0x1000
#define  StlObject  0x2000
#define  Tiefe6 6



#define  Sel                 1
#define  Active              2
#define  UnSel	             0
#define  Done                8
#define  DisplayUpdate    0x10

#define  DisplayTest      0x20
#define M_2PI  6.28318530718
#ifndef M_PI
#define M_PI   3.14159265359
#define M_PI_2 1.57079632680
#define M_PI_4 0.78539816340f
#endif
#define M_PI_8 0.39269908170f
#define M_PI_6 0.523598776f
#define GRAD360 57.2957795131f
#define CBWNDEXTRA 4
#undef  VECTORTEST

#define KONTUR        3
#define FORM         10
#define KONSTRUKTION  2
#define VERARBEITEDT  1
#define UNDEF         0

typedef struct {
 char * szFilename;
 float  xmmFaktor;
 float  ymmFaktor;
 GLuint    glTexture;
 time_t mtime;
} tTextureData;
extern tTextureData rgTextureData[10];
extern const char * rgDefaultTexture[10];

class CObject {
public:
  CObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CObject ();
  CObjectManager * m_ObjSource;
  ClipperLib::IntPoint  m_S;
  int  m_Xmin;
  int  m_Xmax;
  int  m_Ymin;
  int  m_Ymax;
  int  m_Zmin;
  int  m_Zmax;
  unsigned int ETiefe;
  float m_Alpha;
  int  Art;
  int  m_dir;
  int  m_Select;
  int  m_Texture;
  float m_TextureZoom;
  CLayers * m_pLayer;
  int  m_Gefunden;
  int  m_Konstruktion;
  void SetDone(int d);
  void SetSelect(int s);
  void SetActive(int a);
  int  IsSelectedOrNothing();
  int  IsExecutable();
  int  IsSelected();
  int  IsActive();
  int  IsDone();
  virtual unsigned int GetRGBColor();
  virtual int GetWitdh();
  virtual int GetNumItems();
  virtual void AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr){;};
  virtual void ExecuteDrawObject(int mode){;};
  virtual void SaveStlObject(void){;};
  virtual int Is3DFace(){return 0;};
};

class CBohren : public  CObject {
public:
  CBohren(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,char * lpLayer,unsigned int color);
  virtual ~CBohren();
  virtual void ExecuteDrawObject(int mode);
};

class CPolyLineObject : public  CObject {
public:
  CPolyLineObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CPolyLineObject ();
  ClipperLib::Path m_Path;
  CPolyLineObject* m_Insider;
  int        m_SelectIx;
  int        m_ClosedLoop;
  int        m_Projektion;
  virtual void AddBulgeVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr);
  virtual void AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr);
  float     m_nx;
  float     m_ny;
  float     m_nz;
  virtual int GetNumItems() {return m_Path.size();};// m_NumPoints;};
  virtual void ExecuteDrawObject(int mode);
#ifdef USE_SHADER
  virtual void MakeStlProjection();
#endif
  virtual void SaveConvex(void);
  //virtual void DrawConvex(void);
  virtual int  Is3DFace(){return m_Projektion != 0 && gDreiDFace;};
  virtual void CalculateNormale(int rechts=0);
  virtual int  IsIxCrossedRect(int ix,int left,int top,int right,int bottom);
  virtual int  IsCrossedRect(LPRECT rc);
  virtual void ExecuteDrawObjectInRect(int mode,int left,int top,int right,int bottom);
  virtual void SaveStlObject(void);
};


class CPolygonObject : public  CObject {
public:
  CPolygonObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CPolygonObject();
  ClipperLib::Paths m_Paths;
  int        m_SelectIx;
  int        m_ClosedLoop;
  int        m_Projektion;
  int        m_PathIndex;
  virtual void AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr=0);
  virtual int  AddPath(int was);
  float     m_nx;
  float     m_ny;
  float     m_nz;

#ifdef VECTORTEST
  float     m_nx2;
  float     m_ny2;
  float     m_nz2;
  virtual int  GetProjektion();
  virtual int  CalculateDirofRot(int ix);
#endif
  virtual int GetNumItems() ;//{return m_Paths.size();};// m_NumPoints;};
  virtual void ExecuteDrawObject(int mode);
  virtual void SaveConvex(void);
  virtual void DrawConvex(void);
//  virtual int  Is3DFace(){return m_Projektion != 0 && gDreiDFace;};
  virtual void CalculateNormale(int innen=0);

};

class CDreiTafelProjektion : public  CPolygonObject {
public:

  CDreiTafelProjektion(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CDreiTafelProjektion();
  ClipperLib::Paths m_Paths;
  int        m_SelectIx;
  int        m_Projektion;
  int        m_PathIndex;
  virtual void AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr);
  virtual int  AddPath(int was);
  float     m_nx;
  float     m_ny;
  float     m_nz;

#ifdef VECTORTEST
  float     m_nx2;
  float     m_ny2;
  float     m_nz2;
  virtual int  GetProjektion();
  virtual int  CalculateDirofRot(int ix);
#endif
  virtual int GetNumItems() ;//{return m_Paths.size();};// m_NumPoints;};
  virtual void CalculateNormale(int innen=0);
};
/*
class CGCodeLineObject : public  CPolyLineObject {
public:
  int m_G0Feddrate;
  int m_G1Feddrate;
  CGCodeLineObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CGCodeLineObject();
  virtual void ExecuteDrawObject(int mode);
};
*/
class CTextObject : public  CObject {
public:
  char *    m_Ps;
  int       m_Height;
  int       m_Winkel;
  CTextObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CTextObject ();
  virtual void ExecuteDrawObject(int mode);
};

#pragma pack ( push,1)

typedef struct StlTriangle{
  float Normale[3];
  float Vertex[9];
  WORD  Attribute;
}StlTriangle;

typedef struct vector2{
  GLfloat Vertex[2];
}vector2;
typedef struct vector3{
  GLfloat Vertex[3];
}vector3;

typedef struct vector4{
  GLfloat Vertex[4];
}vector4;

#pragma pack( pop)

extern GLuint uLightPos;
extern GLuint uColor;
extern GLuint uFixColor;
extern GLuint uLightColor;
extern GLuint uNormal;
extern GLuint VertexBuffer;
extern GLuint shaderProgram;
extern GLuint uMatrixID;
extern GLuint uViewMatrixID;
extern GLuint uModelMatrixID;
extern GLint  positionAttribute;
extern GLint  ColorAttribute;
extern GLint  TextureParameter;
extern GLint  NormalAttribute;
extern GLint  UVAttribute;
extern GLint  TextureID;
extern GLuint Texture;
extern GLint  uOffset_modelspace;
extern GLint  ShowSeparate;

class CStlObject : public  CObject {
public:
  std::vector<vector3> m_vertices;
  std::vector<vector3> m_colors;
  std::vector<vector3> m_normals;
  std::vector<vector2> m_UV;
  GLuint m_VertexBuffer;
  GLuint m_ColorBuffer;
  GLuint m_NormalBuffer;
  GLuint m_UVBuffer;

  StlTriangle * m_StlObject;
  int       m_NumStlObjects;
  int       m_StlHasColor;
  char *    m_Path;
  time_t    statbuf_mtime;

  float     m_Zoom;
  int       m_Winkel;
  CStlObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CStlObject ();
  int      LoadStlFile(char * pUser);
  int      SetBufferSize(int NumPints);
  void     GenerateGlBuffer();
  int      AddPolygon(const ClipperLib::Paths &pgs,vector3 * pNormal,vector3 * pColor);
  int      CalcTexture(int start,int dir,int ebene);
  void     PushOneVertex();
  void     CalcualteNormalePlus(float nz);

  virtual unsigned int GetRGBColor();
  virtual void ExecuteDrawObject(int mode);
  virtual void SaveStlObject(void);
};



class CBogenObject : public  CObject {
public:
  int       m_Radius;
  float     m_StartWinkel;
  float     m_EndWinkel;
  CBogenObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CBogenObject ();
  virtual void ExecuteDrawObject(int mode);
};

class CDreiDFaceObject : public  CObject {
public:
  ClipperLib::IntPoint  m_3DFace[4];
  float     m_nx;
  float     m_ny;
  float     m_nz;
  int       m_points;
  CDreiDFaceObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color);
  virtual ~CDreiDFaceObject ();
  virtual void ExecuteDrawObject(int mode);
  virtual void CalculateNormale(float mpx,float mpy,float mpz);
  virtual int Is3DFace();
  virtual void SaveStlObject(void);

};
#define USEVECTOR
#undef  USEVECTOR

#ifdef  USEVECTOR
#define  AnzahlCNC2   5000
typedef CObject * CObjectArray[AnzahlCNC2];
#else
#define  AnzahlCNC2   50000
#endif

#define  ANZAHLSAVE   1000



typedef  struct {
  char SymName[256];
  ClipperLib::cInt X;
  ClipperLib::cInt Y;
  ClipperLib::cInt Z;
  int W;
  CObjectManager * m_ObjectManager;
  float Scale;
  int  cmprcnt;
  time_t mtime;
} IncludeType;

typedef std::vector< IncludeType > tIncludeList;
extern tIncludeList IncludeList;

typedef std::vector< CObject * > tCNCInfo;


class CObjectManager
{
public:
	CObjectManager();
	virtual ~CObjectManager();

public:
#ifdef  USEVECTOR
  tCNCInfo   CNCInfo;
#else
  CObject *  CNCInfo[AnzahlCNC2];
#endif
  CObject *  CNCSave[ANZAHLSAVE];
  LAYERTYPE m_AktLayer;
  int  m_AktTiefe;
  ClipperLib::cInt m_AktXPos;
  ClipperLib::cInt m_AktYPos;
  int  m_InUse;
  int  m_Index;
  int  m_AktIndex;

  //int  m_ExecAktLayer;
  //int  m_ExecAktTiefe;
  //int  m_ExecIndex;
  int  CNCInfoCnt;
  int  CNCSaveCnt;
  int  m_3DreiTafelProjektionStartcnt;
  ClipperLib::cInt KurzX,KurzY;
  ClipperLib::cInt Zi;
  int m_Mode;
  char  m_PfadName[256];
  char  m_FilePfadName[256];
  char  m_WorkPfadName[256];
  unsigned long int time;
  int   MinMaxleft;
  int   MinMaxtop;
  int   MinMaxright;
  int   MinMaxbottom;
  int   MinMaxfront;
  int   MinMaxback;

public:
  void virtual AcivateDXF(int a);
  void virtual OptimizeDXF(void);
  void virtual MakeDrills(void);
  void virtual FlipDXF(void);
  void virtual ExpandDXF(float m);
  void virtual SetColorDXF(unsigned int color);
#ifdef USE_SHADER
  void virtual Update3DreiTafelProjektion();
  void virtual Make3DreiTafelProjektion(ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W);
  void virtual MakePolylineProjektion(void);
#endif
  void virtual SaveDXF(void);
  bool virtual FileOpen(const char *pszFile,int bObjectTree);
  bool virtual DXFTreeEinlesen(void);
  void virtual Leiterbahnenerzeugen(void);
  const char * GetIniName(void) {return m_WorkPfadName;};
  bool virtual GetSavename(LPSTR Snamen);
  bool virtual DXFIncludeEinlesen(char * DXFText,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool virtual DXFEinlesen(void);
#ifdef USE_SHADER
  bool virtual TextureFreigeben(void);
  bool virtual TextureEinlesen(void);
#endif
  bool virtual GCodeEinlesen(void);
  bool LinieEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool ArcEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool CircleEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool DreiDFACEEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool DreiDSOLIDEinlesen(FILE * stream,ClipperLib::cInt OX,ClipperLib::cInt OY,ClipperLib::cInt OZ,int OW,float Scale);
  bool PolyLinieEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool LWPolyLinieEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool TextEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool InsertEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool PointEinlesen(FILE * stream,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool BlockEinlesen(FILE * stream);
  bool LayerEinlesen(FILE * stream);
  bool BlockAufloesen(int Symbol,ClipperLib::cInt X,ClipperLib::cInt Y,ClipperLib::cInt Z,int W,float Scale);
  bool ReadCode(FILE * stream);
//  bool ReadCommand(FILE * stream);
  bool CNCComputePoint(ClipperLib::cInt *pX,ClipperLib::cInt *pY,ClipperLib::cInt *pZ,ClipperLib::cInt DXOffset,ClipperLib::cInt DYOffset,ClipperLib::cInt DZOffset,int Winkel,float Scale);
  bool virtual FileEinlesen(bool SizeAdjust);
  bool virtual GerberEinlesen(void);
  bool virtual ExellonEinlesen(void);
  bool virtual DXFSpezialEinlesen(void);
  void virtual CNCFreigeben(void);
  void virtual LayersFreigeben(bool Alle);
  void virtual SaveFreigeben(void);
  int virtual  TestForUpdate();

  bool virtual ifAbbruch(void);
  CObject * GetFirst(LAYERTYPE Layer,int Tiefe);
  CObject * GetNext(void);
  //void          ExecSortReset(int Layer,int Tiefe);
  //CObject * ExecGetNext(void);
  int NumObjectofType(int Art, LAYERTYPE Layer,int Tiefe);
  CObject * GetNearest(int Art, LAYERTYPE Layer,int Tiefe,ClipperLib::cInt x,ClipperLib::cInt y,int Selected);
  int           GetNearestIx(int Art, ClipperLib::cInt x,ClipperLib::cInt y);
  int GetAktObjectIndex(void) {return m_AktIndex;};
  void Execute(int Mode);
  void Sort_Reset(void);
  void Sort_exec();
  void virtual SetSelection(LPRECT rc);
  void virtual Selection(int mode);
};

extern CObjectManager     * gObjectManager;
float glXCNCToScreen(float x);
float glYCNCToScreen(float y);
float glZCNCToScreen(float z);
float glCNCToScreen(float c);
float glXScreenToCNC(float x);
float glYScreenToCNC(float y);
extern RECT   rcAuswahlCNC;


#endif // !defined(AFX_OBJECT_H__4ABD9417_9A09_41B4_B3AA_B350C7D625D9__INCLUDED_)

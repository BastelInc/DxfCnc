#if !defined(_BLOCKMODEL_INCLUDED_)
#define _BLOCKMODEL_INCLUDED_

#include "FltkWindow.h"
#include "FltkDialogBox.h"
#include "FingerScroll.h"
#include "Layers.h"
#include <FL/Fl_Choice.H>

#include "Object.h"

#ifdef USE_SHADER
#define  USE_GLEW
#if defined(WIN32)
# define GLEW_STATIC 1
#endif
#include <GL/glew.h>
#endif
#ifdef USE_SHADER
  #define VECTOR
  #undef COLOR32
#else
  #undef VECTOR
  #define COLOR32
#endif
class CBlockTab: public CFltkDialogBox
{
public:
  CBlockTab(int X, int Y, int W, int H, const char *L);
  ~CBlockTab();
public:
  BOOL SetButtonImage(int nIDDlgItem, int iImage);
  virtual int WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int InitWindow(Fl_Widget  * pParent,int ID,int left,int top,int width, int height,int Style);
  virtual void      Show(void);
};

typedef struct MeshPoint{
 float  X;
 float  Y;
 float  Z;
}MeshPoint;

class CBlockSimulation
{
public:
  CBlockSimulation();
  ~CBlockSimulation();
public:
  float m_Raster;
  float m_BlockLaengeS;
  float m_BlockLaengeE;
  float m_BlockBreiteS;
  float m_BlockBreiteE;
  float m_BlockHoeheS;
  float m_BlockHoeheE;
  float m_BlockDurchmesser;
  float m_MeisselDurchmesser;
  int   m_MeisselPxlWidth;
  float * m_rgMeisselTiefe;
  void  loadInitValues();
  float m_MeisselForm;
  void  drawBlock();
  float * m_rgTiefe;
  float m_Xlast;
  float m_Ylast;
  float m_Zlast;
  float m_Alast;
  int   m_updateSX;
  int   m_updateEX;
  int   m_updateSY;
  int   m_updateEY;

  void Meissel(float x,float y,float z,float a);
  void MeisselLinien(int xPos,int yPos,float zPos ,int w,int h);
#ifdef VECTOR
  std::vector<vector3> m_vertices;
  std::vector<vector3> m_normals;
#else
  vector3 * m_vertices;
  vector3 * m_normals;
#endif

#ifdef COLOR32
  DWORD   * m_colors;
  DWORD    m_Oberflaeche;
  DWORD    m_Koerper;
#else
#ifdef VECTOR
  std::vector<vector3> m_colors;
#else
  vector4 * m_colors;
#endif
  vector3  m_Oberflaeche;
  vector3  m_Koerper;
#endif
#ifdef USE_SHADER
  std::vector<vector2> m_UV;
  GLuint m_VertexBuffer;
  GLuint m_ColorBuffer;
  GLuint m_NormalBuffer;
  GLuint m_UVBuffer;
  int    m_Texture;
#endif
  int  m_NumPoints;
  int  m_width;
  int  m_height;
  vector3 m_3DFace[3];
#ifdef COLOR32
  DWORD m_Color;
#else
  vector3 m_Color;
#endif
  vector3 m_Normal;
  int  SetBufferSize(int NumPoints);
  int  CreateVertexArray(int w,int h);
  int  UpdateVertexArray(int xs,int ys,int xe,int ye,int cratenew );
  void FreeGlBuffer();
  void GenerateGlBuffer();

  void CalculateNormale(int dir);
  void CalculateNormaleCompens(int dir,float abstand, float tiefe);

  int  m_SimulationLayerIndex;
  int  m_SimulationAktTiefe;
  void SimulationStart();
  void SimulationService();

  void StartSimulation();
  void StartSimulation(float durchmesser,int Form);
  virtual void SimulationLoop(void);
  static void * SimulationThreadProc(void * lpThreadParameter);

  int   m_RunSimulation;
  static CRITICAL_SECTION	s_critical_section;
#ifdef WIN32
  HANDLE m_hThread;
  DWORD  m_ThreadID;
 #else
  pthread_t m_hThread;
#endif
  void CreateMeissel();
  void DrawMeissel(float x,float y,float z);
};
extern CBlockSimulation * gBlockSimulation;
enum {eModusRealTime=1,eModusFast};
extern int gSimulationModus;
#endif

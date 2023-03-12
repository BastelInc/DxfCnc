// Eintrag in .profile  Raspberry :
// export MESA_GL_VERSION_OVERRIDE=3.3
// etc/groups :  tty freigeben
// Starten:
// MESA_GL_VERSION_OVERRIDE=3.3 ./dxfcnc

#define  USE_GLEW
#if defined(WIN32)
# define GLEW_STATIC 1
#endif
#include <GL/glew.h>
#include <FL/math.h>
#include <math.h>
#include "MatrixTransform.h"
#include "Settings.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>

#include "GlDisplay.h"
#include "DXF3DDisplayUtils.h"
#include "Object.h"
#include "DxfCnc.h"

#include "clipper.h"
float MMToScreen=100;

float up[3]  = {0,1,0};

tMatrix4x4 ModelMatrix = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};

tMatrix4x4 MVP = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};
tMatrix4x4  ViewMatrix = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};

tMatrix4x4  ProjectionMatrix= {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};



GLuint uLightPos;
GLuint uColor;
GLuint uFixColor;
GLuint uLightColor;
GLuint uNormal;
GLuint VertexBuffer=0;
GLuint shaderProgram=0;
GLuint uMatrixID;
GLuint uViewMatrixID;
GLuint uModelMatrixID;
GLint  positionAttribute;
GLint  ColorAttribute;
GLint  TextureParameter;
GLint  NormalAttribute;
GLint  UVAttribute;
GLint  uOffset_modelspace;

GLuint Texture;
GLint  TextureID;

static  GLfloat * p_Color_buffer_data=0;     // R G B
static  GLfloat * p_uv_buffer_data=0;




//const char * szvertexshaderFile= {"../DXF3DDisplay.vertexshader"};
//const char * szfragmentshaderFile= {"../DXF3DDisplay.fragmentshader"};
const char * szvertexshaderFile= {"StlDisplay.vertexshader"};
const char * szfragmentshaderFile= {"StlDisplay.fragmentshader"};
tTextureData rgTextureData[10] = {
  {NULL,1920,1280,0},
  {NULL,940,625,0},
  {NULL,512,512,0},
  {NULL,1100,920,0},
  {NULL,512,512,0},
  0
};
const char * rgDefaultTexture[10] = {
  "TexturHolz.bmp",
  "TexturPutz.bmp",
  "TexturWand.bmp",
  "TexturAntik.bmp",
  "TexturWolke.bmp",
  "TexturKies.bmp",
  "TexturSchalung.bmp",
  "TexturWiese.bmp",
  "TexturGebuesch.bmp",
  "TexturSteine.bmp"
};

GLuint vertexbuffer;
GLuint colorbuffer;
GLuint normalbuffer;
GLfloat VUnterlage[8][3];
GLfloat NUnterlage[8][3];
GLfloat CUnterlage[8][3];

int CreateUnterlageVertexBuffer()
{
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(NUnterlage), VUnterlage, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CUnterlage), CUnterlage, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(NUnterlage), NUnterlage, GL_DYNAMIC_DRAW);
  return 1;
}

int DeleteUnterlageVertexBuffer()
{
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteBuffers(1, &normalbuffer);
  return 1;
}

bool CObjectManager::TextureFreigeben(void)
{
#ifdef USE_SHADER
  for (int i=0; i < sizeof(rgTextureData)/ sizeof(rgTextureData[0]); i++) {
    if (rgTextureData[i].glTexture)
      glDeleteTextures(1, &rgTextureData[i].glTexture);
    rgTextureData[i].glTexture=0;
  }
#endif
  return 1;
}
bool CObjectManager::TextureEinlesen(void)
{
#ifdef USE_SHADER
  for (int i=0; i < sizeof(rgTextureData)/ sizeof(rgTextureData[0]); i++) {
    if (rgTextureData[i].glTexture==0) {
      rgTextureData[i].glTexture = loadBMP_custom(i);
    }
  }
#endif
  return 1;
}
#ifdef WIN32

int FindPixelformat(void)
{
  // Funktioniert erst ween bereits ein opengl context erzeugt wurde
  HMODULE  instance = LoadLibraryW(L"opengl32.dll");
  if (instance) {
#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_NO_ACCELERATION_ARB           0x2025
#define WGL_ARB_framebuffer_sRGB 1
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB  0x20A9

#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

    typedef PROC (WINAPI * WGLGETPROCADDRESS_T)(LPCSTR);
    WGLGETPROCADDRESS_T _glfw_wglGetProcAddress= (WGLGETPROCADDRESS_T )GetProcAddress(instance, "wglGetProcAddress");
    typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;

    GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)_glfw_wglGetProcAddress("wglGetPixelFormatAttribivARB");
    if (GetPixelFormatAttribivARB ) {
      int value=0;
      int Flags=0;
      int Flags2=0;
      int Pixeltype=0;
      int Acceleration=0;
      int sRGB=0;
      int attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

      int nativeCount=0;
      GetPixelFormatAttribivARB(fl_gc,0,0, 1, &attrib, &nativeCount );

      for (int i=1; i < nativeCount ; i++) {

        PIXELFORMATDESCRIPTOR pfd;

        // Get pixel format attributes through legacy PFDs

        if (!DescribePixelFormat(fl_gc,
                                 i,
                                 sizeof(PIXELFORMATDESCRIPTOR),
                                 &pfd)) {
          continue;
        }
        if (pfd.dwFlags  != (PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SWAP_EXCHANGE | PFD_SUPPORT_COMPOSITION)) continue;

        if (pfd.cDepthBits != 0x18 || pfd.cStencilBits   != 0x08) continue;

        attrib = WGL_SAMPLES_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &value);
        if (value!=4) continue;

        attrib = WGL_SUPPORT_OPENGL_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &Flags);
        if (Flags==0) continue;

        attrib = WGL_DRAW_TO_WINDOW_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &Flags2);
        if (Flags2==0) continue;

        attrib = WGL_PIXEL_TYPE_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &Pixeltype);
        if (Pixeltype != WGL_TYPE_RGBA_ARB) continue;

        attrib = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &sRGB);
        if (sRGB == WGL_ARB_framebuffer_sRGB) continue;

        attrib = WGL_ACCELERATION_ARB;
        GetPixelFormatAttribivARB(fl_gc,i,0, 1, &attrib, &Acceleration);
        if (Acceleration == WGL_NO_ACCELERATION_ARB) continue;

        return i;
      }
    }
  }
  return 0;
}

#endif
//-------------------------------------------------------------
int DXF3DDisplay::  InitShader()
{
  if (shaderProgram) {
    CleanupOpenGls();
  }
  shaderProgram = LoadShaders(szvertexshaderFile, szfragmentshaderFile);
  uLightPos         = glGetUniformLocation(shaderProgram, "LightPosition_worldspace");
  uOffset_modelspace= glGetUniformLocation(shaderProgram, "Offset_modelspace");
  uColor            = glGetUniformLocation(shaderProgram, "uColor");
  uFixColor         = glGetUniformLocation(shaderProgram, "uFixColor");
  uMatrixID         = glGetUniformLocation(shaderProgram, "MVP");
  uViewMatrixID     = glGetUniformLocation(shaderProgram, "V");
  uModelMatrixID    = glGetUniformLocation(shaderProgram, "M");
  NormalAttribute   = glGetAttribLocation( shaderProgram, "vertexNormal_modelspace");
  positionAttribute = glGetAttribLocation( shaderProgram, "vertexPosition_modelspace");
  ColorAttribute    = glGetAttribLocation( shaderProgram, "VertexColor");
  UVAttribute       = glGetAttribLocation( shaderProgram, "VertexUV");
  TextureParameter  = glGetUniformLocation(shaderProgram, "parameterValue");
  TextureID         = glGetUniformLocation(shaderProgram, "TextureSampler");

  CreateUnterlageVertexBuffer();

  return shaderProgram;

}
//-------------------------------------------------------------
void  DXF3DDisplay::CleanupOpenGls()
{
#ifdef USE_GLEW
  if (shaderProgram)
    glDeleteProgram(shaderProgram);
#endif
  shaderProgram=0;
  GlInit=0;
}

//-------------------------------------------------------------
void DXF3DDisplay::SetGeometrics(int MaxSize)
{
  memset(&ModelMatrix,0,sizeof(ModelMatrix));
  ModelMatrix[0][0] = 1.0f;
  ModelMatrix[1][1] = 1.0f;
  ModelMatrix[2][2] = 1.0f;
  ModelMatrix[3][3] = 1.0f;

  lookAt(&ViewMatrix,eye,center,up);
  // Projection matrix : n? Field of View, ratio, display range : 0.1 unit <-> 100 units
  perspective(&ProjectionMatrix,Perspective/57.29577f,
              (float)w() / (float)h(),
              (Deep>0.01) ? 1.0f+Deep:0.01, 100.0f);

  if (Perspective==0) Perspective =1;
  float faktor = MaxSize * 25.0 / Perspective;// / 2.0;
  Scale(&ModelMatrix,&ModelMatrix,Zoom/faktor,Zoom/faktor,Zoom/faktor);

  tMatrix4x4  RotationMatrix;
  eulerAngleXYZ(&RotationMatrix,RotateVal[0]/57.29577f,RotateVal[1]/57.29577f,RotateVal[2]/57.29577f);
  Mul4x4(&ModelMatrix,&RotationMatrix,&ModelMatrix);


  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[0]/57.29577f,1,0,0);
  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[1]/57.29577f,0,1,0);
  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[2]/57.29577f,0,0,1);

  //glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  Mul4x4(&MVP,&ViewMatrix,&ModelMatrix);
  Mul4x4(&MVP,&ProjectionMatrix,&MVP);
}

//-------------------------------------------------------------
void DXF3DDisplay::SetFlatGeometrics(int MaxSize)
{
  memset(&ModelMatrix,0,sizeof(ModelMatrix));
  ModelMatrix[0][0] = 1.0f;
  ModelMatrix[1][1] = 1.0f;
  ModelMatrix[2][2] = 1.0f;
  ModelMatrix[3][3] = 1.0f;
  //float ct[3] = {0,0,0};
  //float ey[3] = {0,0,1};
  //lookAt(&ViewMatrix,ey,ct,up);
  memset(&ViewMatrix,0,sizeof(ViewMatrix));
  ViewMatrix[0][0] = 1.0f;
  ViewMatrix[1][1] = 1.0f;
  ViewMatrix[2][2] = 1.0f;
  ViewMatrix[3][2] =-1.0f;
  ViewMatrix[3][3] = 1.0f;
  // Projection matrix : n? Field of View, ratio, display range : 0.1 unit <-> 100 units
  //perspective(&ProjectionMatrix,Perspective/57.29577f,
  //  (float)w() / (float)h(),
  //  .1f, 100.0f);
  memset(&ProjectionMatrix,0,sizeof(ProjectionMatrix));
  ProjectionMatrix[0][0] = 1.0f;
  ProjectionMatrix[1][1] = 1.0f*(float)w() / (float)h();
  ProjectionMatrix[2][2] =-1.0f;
  ProjectionMatrix[2][3] =-1.0f;
  ProjectionMatrix[3][2] =-0.2f;
  //  if (Perspective==0) Perspective =1;
  float faktor = 3.0 / MaxSize;
  Scale(&ModelMatrix,&ModelMatrix,faktor,faktor,faktor);
//  tMatrix4x4  RotationMatrix;
//  eulerAngleXYZ(&RotationMatrix,0,0,0);
//  Mul4x4(&ModelMatrix,&RotationMatrix,&ModelMatrix);


  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[0]/57.29577f,1,0,0);
  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[1]/57.29577f,0,1,0);
  //Rotate(&ModelMatrix,&ModelMatrix,RotateVal[2]/57.29577f,0,0,1);

  //glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  Mul4x4(&MVP,&ViewMatrix,&ModelMatrix);
  Mul4x4(&MVP,&ProjectionMatrix,&MVP);
}

//-------------------------------------------------------------
void DXF3DDisplay::DeleteGaphicData()
{
  DeleteUnterlageVertexBuffer();
}
//-------------------------------------------------------------
void DrawContainer(float urcleft,float urcright,float urctop,float urcbottom,float urcfront,float urcback)
{
  GLfloat lVUnterlage[8][3]= {
    {urcleft,urctop,urcfront},
    {urcright,urctop,urcfront},
    {urcright,urcbottom,urcfront},
    {urcleft,urcbottom,urcfront},
    {urcleft,urctop,urcback},
    {urcright,urctop,urcback},
    {urcright,urcbottom,urcback},
    {urcleft,urcbottom,urcback}
  };
  GLfloat lNUnterlage[8][3]= {
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,-1},
    {0,0,-1},
    {0,0,-1},
    {0,0,-1}
  };
  GLfloat lCUnterlage[8][3]= {
    {1,1,0},
    {0,1,0},
    {0,1,0},
    {1,1,0},
    {1,0,0},
    {1,1,0},
    {1,1,0},
    {1,0,0}
  };
  memcpy(VUnterlage,lVUnterlage,sizeof(VUnterlage));
  memcpy(NUnterlage,lNUnterlage,sizeof(NUnterlage));
  memcpy(CUnterlage,lCUnterlage,sizeof(CUnterlage));

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(NUnterlage), VUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

  glEnableVertexAttribArray(NormalAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(NUnterlage), NUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

  glEnableVertexAttribArray(ColorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CUnterlage), CUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  GLbyte rgCube[24]= {
    0,1,2,3,
    4,5,6,7,
    0,3,7,4,
    1,2,6,5,
    0,1,5,4,
    2,3,7,6
  };

  glUniform4f(uColor,0.4, 0.4, 0.4,0.2);

  glDrawElements(GL_QUADS,24,GL_UNSIGNED_BYTE,rgCube);


  glDisableVertexAttribArray(positionAttribute);
  glDisableVertexAttribArray(NormalAttribute);
  glDisableVertexAttribArray(ColorAttribute);
}

void enableMultisample(int msaa)
{
  if (msaa) {
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    // detect current settings
    GLint iMultiSample = 0;
    GLint iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);
    printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);
  } else {
    glDisable(GL_MULTISAMPLE);
    printf("MSAA off\n");
  }
}


//-------------------------------------------------------------
void DXF3DDisplay::drawSoftMultiple(void)
{
  static int norecursion=0;
  if (norecursion)
    return;
  norecursion++;
  if ((!valid())) {
    if (shaderProgram)
      CleanupOpenGls();
  }

  int showMargins=0;
  if (!shaderProgram || !GlInit)
    InitOpenGL();
  else if ((!valid())) {
    glViewport(0, 0, pixel_w(), pixel_h());
  }
  
  //enableMultisample(gbFillPolygon);
  glEnable(GL_MULTISAMPLE);
  glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
  if (gBackground) {
    glClearColor(gBackground,gBackground,gBackground,1);
  }
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);
  GLint p=0;
  glGetIntegerv(GL_CURRENT_PROGRAM,&p);

  float	X, Y, W, H;			// Interior of widget
  X = x() + Fl::box_dx(box());
  Y = y() + Fl::box_dy(box());
  W = w() - Fl::box_dw(box());
  H = h() - Fl::box_dh(box());

  static int MinMaxleft   = -20000;
  static int MinMaxright  =  20000;
  static int MinMaxtop    = -15000;
  static int MinMaxbottom =  15000;
  static int MinMaxfront  = -15000;
  static int MinMaxback   =  15000;
#ifdef BLOCKSIMULATION
  int XPos = gCNCPos[Xp];
  int YPos = gCNCPos[Yp];
  int ZPos = gCNCPos[Zp]-gPa.Ref[Zp];
#endif

  gDreiDFace = gbFillPolygon;//gPerspektive->IsDlgButtonChecked(IDC_Show3DFace);
  if (bArrangeZoomCenter) {
    MinMaxleft   = 0x7FFFFFFF;
    MinMaxright  = 0x80000000;
    MinMaxtop    = 0x7FFFFFFF;
    MinMaxbottom = 0x80000000;
    MinMaxfront  = 0x7FFFFFFF;
    MinMaxback   = 0x80000000;
    // Calculate the faktor to fit gl-Objects in the Range -n to +n
    unsigned cnt = (unsigned)IncludeList.size();
    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pTestOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);
      while (pTestOb) {
        if (ShowSeparate==0 || pTestOb->m_Konstruktion >VERARBEITEDT) {
          //((pTestOb->Art & StlObject)==StlObject)) {
          if (pTestOb->m_Xmin < MinMaxleft  )   MinMaxleft   = pTestOb->m_Xmin;
          if (pTestOb->m_Xmax > MinMaxright )   MinMaxright  = pTestOb->m_Xmax;
          if (pTestOb->m_Ymin < MinMaxtop   )   MinMaxtop    = pTestOb->m_Ymin;
          if (pTestOb->m_Ymax > MinMaxbottom)   MinMaxbottom = pTestOb->m_Ymax;
          if (pTestOb->m_Zmin < MinMaxfront )   MinMaxfront  = pTestOb->m_Zmin;
          if (pTestOb->m_Zmax > MinMaxback  )   MinMaxback   = pTestOb->m_Zmax;
        }
        pTestOb =  inc.m_ObjectManager->GetNext();
      }
      bArrangeZoomCenter=0;
    }
  }
  {
    float XMax = MinMaxright -  MinMaxleft;
    float ZMax = MinMaxback -  MinMaxfront;
    float Max  = MinMaxbottom - MinMaxtop;
    XOffset    = (MinMaxright+MinMaxleft)/2.0;
    YOffset    = (MinMaxbottom+MinMaxtop)/2.0;
    ZOffset    = (MinMaxback+MinMaxfront)/2.0;

    MMToScreen   = 10;
    if ((XMax / Max) > (W/H)) {
      Max = XMax * (H/W);
    }
    if (ZMax > Max) Max = ZMax;
extern float glMaximum ;
    glMaximum = Max / 100.0;
    
    //MMToScreen = 0.5 / Max;
    MMToScreen = Max * 2.0;
    //ZOffset    = 0.5 * MMToScreen;
    float glXOffset = (float)XOffset/-100.0;
    float glYOffset = (float)YOffset/-100.0;
    float glZOffset = (float)ZOffset/-100.0;
    glFaktor = 1.0 / 100.0;
    MMToScreen /= 100.0;    // Milimeter


    SetGeometrics(MMToScreen);

    glUniform3f(uLightColor,LightColor[0],LightColor[1],LightColor[2]);
    glUniform3f(uLightPos, LightPos[0], LightPos[1], LightPos[2]);
    glUniform3f(uOffset_modelspace, glXOffset, glYOffset, glZOffset);
    // Send our transformation to the currently bound shader, in the ModelPerspective uniform
    glUniformMatrix4fv(uMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(uModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(uViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    //glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,gAlphaTransp);
    glUniform3f(uColor,LightColor[0],LightColor[1],LightColor[2]);

    XOffset    = 0;
    YOffset    = 0;
    ZOffset    = 0;


    glUniform1i(TextureParameter,0);

    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pOb2  =  inc.m_ObjectManager->GetFirst(NULL,-1);
      while (pOb2) {
        if (ShowSeparate==0 || pOb2->m_Konstruktion > VERARBEITEDT) {
          DWORD clr = pOb2->GetRGBColor();
          // Schwarze Kanten
          if (gbUsePolygonObject) glUniform4f(uFixColor,0.0,0.0,0.0,1.0);
          else   glUniform4f(uFixColor,(clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);

          pOb2->ExecuteDrawObject(Draw);
        }
        pOb2 =  inc.m_ObjectManager->GetNext();
      }
    }
#ifdef BLOCKSIMULATION
    if (!gOnline ) { //gBlockTab && gBlockTab->visible()|| gBlockTab && gPerspektive->visible()){
      if (gBlockSimulation) gBlockSimulation->drawBlock();
    }
#endif
  }
  if (showMargins) {
    DrawContainer(MinMaxleft   / 100.0,MinMaxright  / 100.0,
      MinMaxbottom / 100.0,MinMaxtop    / 100.0,MinMaxfront  / 100.0,MinMaxback   / 100.0);
  }

#ifdef BLOCKSIMULATION
  if (1) {
    float glMaximum = 100.0;
      DWORD clr;// = pOb2->GetRGBColor();
          // Schwarze Kanten

    if (gSpindleOn)     clr = RGB(220,220,255);
    else if (ZPos <  0) clr = RGB(80,200,255);
    else                clr = RGB(127,255,127);
    glUniform4f(uFixColor,(clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);

    float  y1 = glYCNCToScreen(YPos);
    float  x1 = glXCNCToScreen(XPos);
    float  z1 = glZCNCToScreen(ZPos);//+0.02*glMaximum;
//    glTranslatef(x1,y1,z1);
    //glutSolidCone(0.4,0.5,20,10);
//FL_EXPORT void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
//    glutSolidCone(0.010*glMaximum,-0.02*glMaximum,9,3);
//    glTranslatef(-x1,-y1,-z1);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex3f(x1,y1,z1+0.02*glMaximum);
    glVertex3f(x1,y1,z1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex3f(x1,y1,0.1*glMaximum);
    glVertex3f(x1,y1,0);
    glEnd();
  }
#endif

  if (ShowSeparate && gGrayOut>0.101) {
    float XMax = gObjectManager->MinMaxright -  gObjectManager->MinMaxleft;
    float Max  = gObjectManager->MinMaxbottom - gObjectManager->MinMaxtop;
    XOffset    = (gObjectManager->MinMaxright+gObjectManager->MinMaxleft)/2.0;
    YOffset    = (gObjectManager->MinMaxbottom+gObjectManager->MinMaxtop)/2.0;
    float MMToScreen   = 10;
    if ((XMax / Max) > (W/H)) {
      Max = XMax * (H/W);
    }
    MMToScreen = Max * 2.0;

    float glXOffset =   (float)XOffset/-100.0;
    float glYOffset =   (float)YOffset/-100.0;
    float glZOffset =   -1.5;
    glFaktor = 1.0 / 100.0; // CncUnits to mm

    MMToScreen /= 100.0;    // Milimeter

    SetFlatGeometrics(MMToScreen);
    glUniform3f(uLightColor,LightColor[0],LightColor[1],LightColor[2]);
    glUniform3f(uLightPos, LightPos[0], LightPos[1], LightPos[2]);
    glUniform3f(uOffset_modelspace, glXOffset, glYOffset, glZOffset);

    // Send our transformation to the currently bound shader
    glUniformMatrix4fv(uMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(uModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(uViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    for (int i =0; i < IncludeList.size(); i++) {
      IncludeType inc;
      inc = IncludeList[i];
      CObject * pOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);

      glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,gAlphaTransp);

      XOffset    = 0;
      YOffset    = 0;
      ZOffset    = 0;

      glUniform4f(uFixColor,gGrayOut, gGrayOut, gGrayOut,1.0);
      while (pOb) {
        if (pOb->m_Konstruktion <= VERARBEITEDT) {
          pOb->ExecuteDrawObject(Draw);
        }
        pOb =  inc.m_ObjectManager->GetNext();
      }
    }
  }
  norecursion--;
}
//-------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <math.h>
#if defined(WIN32)
# define GLEW_STATIC 1
#include <GL/glew.h>
#else   
//#include "glew.h"
#include <GL/glew.h>
#endif
#include "MatrixTransform.h"
#include "GlDisplay.h"
#include "Object.h"
#include "clipper.h"

int     gbUsePolygonObject=0;
extern int    gbShowTransformation;

#define ANSICHTvVORNE 'V'
#define ANSICHTvLINKS 'L'
#define ANSICHTvOBEN  'O'
int  s_dir;


CStlObject * gpStlObject=NULL;
static int modus=0;
static int count=0;

//------------------------------------------------------------------------------
vector3 Stlcolor;
vector3 Stlnormal;
vector3 v1;
vector3 v2;
vector3 v3;

#ifndef WIN32
#define CALLBACK
#endif

int XMitte;
int YMitte;
int ZMitte;
//-------------------------------------------------------------
CDreiTafelProjektion::CDreiTafelProjektion (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color):CPolygonObject(ObjSource,x,y,z,lpLayer,color)
{
  Art = SavePolygon;
  m_Paths.clear();// = (CNCPOINT *) malloc(m_ArraySize *sizeof(CNCPOINT));
  m_nx=0;
  m_ny=0;
  m_nz=0;
  //ClipperLib::Path  p;
  //m_Paths.push_back(p);// reserve(1);
  //m_Paths.resize(1);
  m_PathIndex=-1;
  m_SelectIx=-1;
  m_Projektion=0;
}
//-------------------------------------------------------------
CDreiTafelProjektion::~CDreiTafelProjektion ()
{
  m_Paths.clear();
}
//-------------------------------------------------------------
int CDreiTafelProjektion::GetNumItems()
{
  int count =0;
  for (int i = 0; i < m_Paths.size(); ++i) {
    count += m_Paths[i].size();
  }
  return count;
}
//-------------------------------------------------------------
#ifdef VECTORTEST
int  CDreiTafelProjektion::CalculateDirofRot(int ix)
{
  int s = m_Paths.size();
  int res=0;
  int dir1;
  int dir2;
  char Projektion = GetProjektion();
  if (ix>=0 && ix < s) {
    int ps = m_Paths[ix].size();
    double w;
    double h;
    double t;
    int i;
    if (Projektion==ANSICHTvVORNE) {
      //fprintf(stderr,"%c ",Projektion);
      for (i=0; i < ps; i++) {
        int ii = i+1;
        if (ii >= ps) ii = 0;
        w = m_Paths[ix][ii].X-m_Paths[ix][i].X;
        h = m_Paths[ix][ii].Y-m_Paths[ix][i].Y;
        if (w==0 && h==0) {
        } else {
          dir1 = (atan2(h,w) *GRAD360);
          if (i) {
            int wi =(dir2-dir1);
            if (wi >= 180)       wi -= 360;
            else if (wi <= -180) wi += 360;
            res +=wi;
          }
          dir2 = dir1;
          //fprintf(stderr,"%d=%4d (h %5.1f t %5.1f) ",i,dir1,w/fxAufloesung,h/fxAufloesung);
        }
      }
      //fprintf(stderr,"end %d\n",res);
    } else if (Projektion==ANSICHTvLINKS) {
      //fprintf(stderr,"%c ",Projektion);
      for (i=0; i < ps; i++) {
        int ii = i+1;
        if (ii >= ps) {
          ii = 0;
        }
        float h1 = m_Paths[ix][i].Y;
        float h2 = m_Paths[ix][ii].Y;
        float z1 = m_Paths[ix][i].Z;
        float z2 = m_Paths[ix][ii].Z;
        h = m_Paths[ix][ii].Y-m_Paths[ix][i].Y;
        t = m_Paths[ix][ii].Z-m_Paths[ix][i].Z;
        if (t==0 && h==0) {
        } else {
          dir1 = (atan2(t,h) *GRAD360);
          if (i) {
            int wi =(dir2-dir1);
            if (wi >= 180)       wi -= 360;
            else if (wi <= -180) wi += 360;
            res +=wi;
          }
          //fprintf(stderr,"%d=%4d (h %5.1f t %5.1f) ",i,w,h/fxAufloesung,t/fxAufloesung);
          dir2 = dir1;
        }

      }
      //fprintf(stderr,"end %d\n",res);
    } else if (Projektion==ANSICHTvOBEN) {
      fprintf(stderr,"%c ",Projektion);
      for (i=0; i < ps; i++) {
        int ii = i+1;
        if (ii >= ps) ii = 0;
        w = m_Paths[ix][ii].X-m_Paths[ix][i].X;
        t = m_Paths[ix][ii].Z-m_Paths[ix][i].Z;
        if (w==0 && t==0) {
        } else {
          dir1 = (atan2(t,w) *GRAD360);
          if (i) {
            int wi =(dir2-dir1);
            if (wi >= 180)       wi -= 360;
            else if (wi <= -180) wi += 360;
            res +=wi;
            fprintf(stderr,"%d=%d (w %0.0f t %0.0f) ",i,wi,w/fxAufloesung,t/fxAufloesung);
          }
          dir2 = dir1;
        }
      }
      fprintf(stderr,"end %d\n",res);
    }

  }
  return res;
}
//-------------------------------------------------------------
int  CDreiTafelProjektion::GetProjektion()
{
  if (m_Projektion=='F') {
    int w = m_Xmax - m_Xmin;
    int h = m_Ymax- m_Ymin;
    int t = m_Zmax  - m_Zmin;
    if (t < h) {
      if (t < w) {
        return ANSICHTvVORNE;   // fast kein Z
      } else if (w < h) {
        return ANSICHTvLINKS;   // fast kein X
      } else {
        return ANSICHTvOBEN;   // fast kein Y
      }
    } else {
      if (w < h) {
        return ANSICHTvLINKS;   // fast kein X
      } else {
        return ANSICHTvOBEN;   // fast kein Y
      }
    }
  } else {
    return m_Projektion;
  }
}
#endif

void CDreiTafelProjektion::CalculateNormale(int rechts)
{
#ifdef VECTORTEST
  m_nx2 = 0;
  m_ny2 = 0;
  m_nz2 = 0;
  if  (bShowNormale) {
    char Projektion = GetProjektion();
    int dir = CalculateDirofRot(0);
    if (Projektion ==ANSICHTvVORNE) {
      m_nx2 = 0;
      m_ny2 = 0;
      m_nz2 = (dir > 0)?1:-1;
      //return ;
    } else if (Projektion ==ANSICHTvLINKS) {
      m_nx2 = (dir > 0)?1:-1;
      m_ny2 = 0;
      m_nz2 = 0;
      //return ;
    } else if (Projektion ==ANSICHTvOBEN) {
      m_nx2 = 0;
      m_ny2 = (dir > 0)?1:-1;
      m_nz2 = 0;
      //return ;
    } else {
    }
  }
#endif
  if (m_PathIndex >= 0 && m_Paths.size() > m_PathIndex) {
    int nP = m_Paths[0].size()-3;
    float normalX =  0;
    float normalY =  0;
    float normalZ =  0;
    while (nP>=0) {
      float   w1 = m_Paths[0][nP+2].X - m_Paths[0][nP+1].X;
      float   h1 = m_Paths[0][nP+2].Y - m_Paths[0][nP+1].Y;
      float   t1 = m_Paths[0][nP+2].Z - m_Paths[0][nP+1].Z;
      float   w2 = m_Paths[0][nP+1].X - m_Paths[0][nP+0].X;
      float   h2 = m_Paths[0][nP+1].Y - m_Paths[0][nP+0].Y;
      float   t2 = m_Paths[0][nP+1].Z - m_Paths[0][nP+0].Z;
      normalX +=  (h1) * (t2) - (h2) * (t1);
      normalY +=  (t1) * (w2) - (t2) * (w1);
      normalZ +=  (w1) * (h2) - (w2) * (h1);
      nP--;
    }
    if (normalX || normalY || normalZ) {
      float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      if (rechts) isqr *= -1.0;
      m_nx = normalX * isqr;
      m_ny = normalY * isqr;
      m_nz = normalZ * isqr;
    }
  }
  if (m_Xmin < gpStlObject->m_Xmin)   gpStlObject->m_Xmin  = m_Xmin;
  if (m_Xmax > gpStlObject->m_Xmax)   gpStlObject->m_Xmax = m_Xmax;
  if (m_Ymin < gpStlObject->m_Ymin)   gpStlObject->m_Ymin   = m_Ymin ;
  if (m_Ymax > gpStlObject->m_Ymax)   gpStlObject->m_Ymax= m_Ymax;
  if (m_Zmin< gpStlObject->m_Zmin)    gpStlObject->m_Zmin = m_Zmin;
  if (m_Zmax > gpStlObject->m_Zmax)   gpStlObject->m_Zmax  = m_Zmax;


  DWORD clr = GetRGBColor();
  vector3 c = {(clr>>0 & 0xFF) / 255.0f,(clr>>8 & 0xFF) / 255.0f,(clr>>16 & 0xFF)/ 255.0f};
  vector3 n = {m_nx,m_ny,m_nz};
  int bsize = gpStlObject->m_NumStlObjects;
  gpStlObject->AddPolygon(m_Paths,&n,&c);


  int ebene =0;
  int nx = abs(m_nx*10.0);
  int ny = abs(m_ny*10.0);
  int nz = abs(m_nz*10.0);
  if (nx > ny) {
    ebene = (nz > nx)?-1:0;
  } else {
    ebene = (nz > ny)?-1:1;
  }
  
  if (gpStlObject->m_Texture>=0) {
    gpStlObject->CalcTexture(bsize,gpStlObject->m_dir!=0?gpStlObject->m_dir:s_dir,ebene);
  }

  if (bShowNormale) { // Hilfe : zeige Normale
    float xx = glXCNCToScreen((m_Xmin + m_Xmax)/2.0);
    float yy = glYCNCToScreen((m_Ymin + m_Ymax)/2.0);
    float zz = glZCNCToScreen((m_Zmin + m_Zmax)/2.0);
    GLfloat mat_diffuse[4];
    mat_diffuse[0] = 0.4;
    mat_diffuse[1] = 1.0;
    mat_diffuse[2] = 0.4;

    Stlcolor.Vertex[0]=0;
    Stlcolor.Vertex[1]=1;
    Stlcolor.Vertex[2]=0;
    Stlnormal.Vertex[0]=0.5;
    Stlnormal.Vertex[1]=0.5;
    Stlnormal.Vertex[2]=0.5;
    float einheit = MMToScreen/1000;
    v1.Vertex[0] = xx;
    v1.Vertex[1] = yy;
    v1.Vertex[2] = zz;
    v3 = v1;
    v3.Vertex[0] += einheit/2.0;
    v3.Vertex[1] += einheit/2.0;
    v3.Vertex[2] += einheit/2.0;
    yy+= n.Vertex[1]*einheit *10;
    xx+= n.Vertex[0]*einheit *10;
    zz+= n.Vertex[2]*einheit *10;
    v2.Vertex[0] = xx;
    v2.Vertex[1] = yy;
    v2.Vertex[2] = zz;
    gpStlObject->PushOneVertex();
    v3.Vertex[0] -= einheit;
    v3.Vertex[1] -= einheit;
    v3.Vertex[2] -= einheit;
    gpStlObject->PushOneVertex();
  }
}
//------------------------------------------------------------------------------
//-------------------------------------------------------------
typedef std::vector< GLdouble* > tPolygonVectors3;
tPolygonVectors3 PolygonVectors3;

//------------------------------------------------------------------------------
// heap memory management for GLUtesselator ...
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
GLdouble* NewPolygonVector3(GLdouble x, GLdouble y,GLdouble z)
{
  GLdouble *vert = new GLdouble[3];
  vert[0] = x;
  vert[1] = y;
  vert[2] = z;
  PolygonVectors3.push_back(vert);
  return vert;
}
//------------------------------------------------------------------------------

void ClearPolygonVectors3()
{
  for (tPolygonVectors3::size_type i = 0; i < PolygonVectors3.size(); ++i)
    delete[] PolygonVectors3[i];
  PolygonVectors3.clear();
}


//------------------------------------------------------------------------------
void CALLBACK CombinePolygonCB3(GLdouble coords[3], GLdouble*[4], GLfloat[4], GLdouble **dataOut )
{
  GLdouble *vert = NewPolygonVector3(coords[0], coords[1],coords[2]);
  *dataOut = vert;
}
//------------------------------------------------------------------------------

void CStlObject::PushOneVertex()
{
  int i = m_NumStlObjects*3;
#ifdef USE_SHADER
  m_vertices[i] = v1;
  m_normals[i]  = Stlnormal;
  m_colors[i]   = Stlcolor;
  i++;
  m_vertices[i] = v2;
  m_normals[i]  = Stlnormal;
  m_colors[i]   = Stlcolor;
  i++;
  m_vertices[i] = v3;
  m_normals[i]  = Stlnormal;
  m_colors[i]   = Stlcolor;
  i++;
#endif
  m_NumStlObjects++;
}

void CALLBACK BeginStlPolyVertexCB3(GLenum type)
{
  modus =type;
  count = 0;
}
//------------------------------------------------------------------------------
void CALLBACK EndStlPolyVertexCB3()
{
  modus =0;
  count = 0;
}
//------------------------------------------------------------------------------

void CALLBACK VertexStlPolyVertexCB3(GLvoid *vertex)
{
  const GLdouble  *vals = (GLdouble  * ) vertex;
  if (count==0) {
    v1.Vertex[0] = vals[0];
    v1.Vertex[1] = vals[1];
    v1.Vertex[2] = vals[2];
    count=1;
  } else if (count==1) {
    v2.Vertex[0] = vals[0];
    v2.Vertex[1] = vals[1];
    v2.Vertex[2] = vals[2];
    count=2;
  } else if (count==2) {
    if (modus == GL_TRIANGLES) {
      v3.Vertex[0] = vals[0];
      v3.Vertex[1] = vals[1];
      v3.Vertex[2] = vals[2];
      count=0;
      gpStlObject->PushOneVertex();
    } else if (modus == GL_TRIANGLE_STRIP) {
      v3.Vertex[0] = vals[0];
      v3.Vertex[1] = vals[1];
      v3.Vertex[2] = vals[2];
      count=2;
      gpStlObject->PushOneVertex();
      v1 = v2;
      v2 = v3;
    } else if (modus == GL_TRIANGLE_FAN ) {
      v3.Vertex[0] = vals[0];
      v3.Vertex[1] = vals[1];
      v3.Vertex[2] = vals[2];
      count=2;
      gpStlObject->PushOneVertex();
      v2 = v3;
    } else { //if (modus == GL_QUADS) {
      v3.Vertex[0] = vals[0];
      v3.Vertex[1] = vals[1];
      v3.Vertex[2] = vals[2];
      count=0;
    }
  } else { //if (count==3) {
    count++;
  }

}
//------------------------------------------------------------------------------
int CStlObject::AddPolygon(const ClipperLib::Paths &pgs,vector3 * pNormal,vector3 * pColor)
{

  GLUtesselator* tess = gluNewTess();
  gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())&BeginStlPolyVertexCB3);
  gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCB3);

  Stlcolor  = *pColor;
  Stlnormal = *pNormal;

  gluTessNormal(tess, pNormal->Vertex[0],pNormal->Vertex[1],pNormal->Vertex[2]);
  //unsigned int clr = GetRGBColor();

  gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())&EndStlPolyVertexCB3);
  gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)())&CombinePolygonCB3);

  // gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD); // zweites Poly ausschneiden

  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessBeginPolygon(tess, NULL);

  for (int p = 0; p < pgs.size(); ++p) {
    gluTessBeginContour(tess);
    for (int i=0; i < pgs[p].size(); i++) {
      float x,y,z;
      y = pgs[p][i].Y;
      x = pgs[p][i].X;
      z = pgs[p][i].Z;
      GLdouble *vert;
      vert = NewPolygonVector3(x/fxAufloesung,y/fxAufloesung,z/fxAufloesung);
      gluTessVertex(tess, vert, vert);
    }
    gluTessEndContour(tess);
  }
  gluTessEndPolygon(tess);
  //final cleanup ...
  gluDeleteTess(tess);
  ClearPolygonVectors3();
  return 1;
}

int CStlObject::CalcTexture(int start,int dir,int ebene)
{
#ifdef USE_SHADER
  float xf = rgTextureData[m_Texture].xmmFaktor * m_TextureZoom;//1920;
  float hf = rgTextureData[m_Texture].ymmFaktor * m_TextureZoom;// 1280;
//  float xf = 512;
//  float hf = 512;
//  float xf = 624.0;// / ((float)(m_Xmax - m_Xmin)*1.0);
//  float hf = 416.0;// / ((float)(m_Ymax - m_Ymin)*1.);
    Stlcolor.Vertex[0]=0;
    Stlcolor.Vertex[1]=1;
    Stlcolor.Vertex[2]=0;

  if (dir==1) {
    if (ebene ==0) {        //x > zy
    Stlcolor.Vertex[0]=0.8;  // rot
    Stlcolor.Vertex[1]=0;
    Stlcolor.Vertex[2]=0;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[1]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[2]/hf;
//        m_colors[i] = Stlcolor;
      }
    } else if (ebene ==1) { //y > xz;
    Stlcolor.Vertex[0]=0.7; // gelb
    Stlcolor.Vertex[1]=0.7;
    Stlcolor.Vertex[2]=0;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[0]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[2]/hf;
//        m_colors[i] = Stlcolor;
      }
    } else {                //z > xy
    Stlcolor.Vertex[0]=0; // gruen
    Stlcolor.Vertex[1]=0.9;
    Stlcolor.Vertex[2]=0;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[0]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[1]/hf;
//        m_colors[i] = Stlcolor;
      }
    }
  } else {
    if (ebene ==0) {        //x > zy
    Stlcolor.Vertex[0]=0.6;  // magenta
    Stlcolor.Vertex[1]=0;
    Stlcolor.Vertex[2]=0.6;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[2]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[1]/hf;
//        m_colors[i] = Stlcolor;
      }
    } else if (ebene ==1) { //y > xz;
    Stlcolor.Vertex[0]=0.5; // weiss
    Stlcolor.Vertex[1]=0.5;
    Stlcolor.Vertex[2]=0.5;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[2]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[0]/hf;
//        m_colors[i] = Stlcolor;
      }
    } else {                //z > xy
    Stlcolor.Vertex[0]=0; // blau
    Stlcolor.Vertex[1]=0;
    Stlcolor.Vertex[2]=0.7;
      for (int i =start*3;i <  m_NumStlObjects*3;i++){
        m_UV[i].Vertex[0] = m_vertices[i].Vertex[1]/xf;
        m_UV[i].Vertex[1] = m_vertices[i].Vertex[0]/hf;
//        m_colors[i] = Stlcolor;
      }
    }
  }
  #endif
  return 1;
}
//-------------------------------------------------------------
int CDreiTafelProjektion::AddPath(int was)
{
  m_PathIndex++;
  m_Paths.resize(m_PathIndex+1);
  int s = m_Paths.size();
  return s;
}

//-------------------------------------------------------------
void CDreiTafelProjektion::AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr=0)
{
  ClipperLib::IntPoint p;
  if (bYUpp==1) {
    p.X = x;
    p.Y = z + YMitte;
    p.Z = -y  + YMitte;;
  } else  if (bYUpp==2) {
    p.X = (YMitte-y) + XMitte;
    p.Y = z + YMitte;
    p.Z = XMitte - x;
  } else {
    p.X = x;
    p.Y = y;
    p.Z = z;
  }
  p.Attr=Attr;
  if (m_PathIndex >= 0 && m_Paths.size() > m_PathIndex) {
    m_Paths[m_PathIndex].push_back(p);

    if (p.X < m_Xmin)       m_Xmin  = p.X;
    if (p.X > m_Xmax)      m_Xmax = p.X;
    if (p.Y < m_Ymin)     m_Ymin   = p.Y;
    if (p.Y > m_Ymax)        m_Ymax= p.Y;
    if (p.Z < m_Zmin)      m_Zmin = p.Z;
    if (p.Z > m_Zmax)       m_Zmax  = p.Z;
  }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CStlObject ::SaveStlObject(void)
{
  if (savebinary) {
    vector3 color  = {0};
    vector3 normal = {0};
#pragma pack ( push,1)
    struct tbuff {
      float Normale[3];
      float Vertex1[3];
      float Vertex2[3];
      float Vertex3[3];
      WORD  Attribute;
    } buff;
#pragma pack( pop)
#ifdef USE_SHADER
     for (int ix=0; ix<m_NumStlObjects; ix++) {
      buff.Normale[0] = m_normals[ix*3].Vertex[0];
      buff.Normale[1] = m_normals[ix*3].Vertex[1];
      buff.Normale[2] = m_normals[ix*3].Vertex[2];
      buff.Vertex1[0] = m_vertices[ix*3+0].Vertex[0];
      buff.Vertex1[1] = m_vertices[ix*3+0].Vertex[1];
      buff.Vertex1[2] = m_vertices[ix*3+0].Vertex[2];
      buff.Vertex2[0] = m_vertices[ix*3+1].Vertex[0];
      buff.Vertex2[1] = m_vertices[ix*3+1].Vertex[1];
      buff.Vertex2[2] = m_vertices[ix*3+1].Vertex[2];
      buff.Vertex3[0] = m_vertices[ix*3+2].Vertex[0];
      buff.Vertex3[1] = m_vertices[ix*3+2].Vertex[1];
      buff.Vertex3[2] = m_vertices[ix*3+2].Vertex[2];
      buff.Attribute  = ((WORD)(m_colors[ix*3].Vertex[2]*31.0) << 10) |((WORD)(m_colors[ix*3].Vertex[1]*31.0) << 5) | ((WORD)(m_colors[ix*3].Vertex[0]*31.0)) | 0x8000;
      int s = sizeof(buff);
      if (s==50) {
        fwrite((const void *)&buff,1,50,gWrk3DSheet->m_file);
        NumVertextoSave++;
      }
    }
  #else
  if (m_StlObject) {
    fwrite((const void *)m_StlObject,m_NumStlObjects,50,gWrk3DSheet->m_file);
  }
  
  #endif
  } else {
    for (int ix=0; ix<m_NumStlObjects; ix++) {
      fprintf(gWrk3DSheet->m_file,
              " facet normal %d %d %d\n" \
              "  outer loop\n" \
              "   vertex %0.3f %0.3f %0.3f\n" \
              "   vertex %0.3f %0.3f %0.3f\n" \
              "   vertex %0.3f %0.3f %0.3f\n" \
              "  endloop\n" \
              " endfacet\n",//(int)m1,(int)m2,-(int)m3,
              (int)m_normals[ix*3].Vertex[0],
              (int)m_normals[ix*3].Vertex[1],
              (int)m_normals[ix*3].Vertex[2],
              m_vertices[ix*3+0].Vertex[0],
              m_vertices[ix*3+0].Vertex[1],
              m_vertices[ix*3+0].Vertex[2],
              m_vertices[ix*3+1].Vertex[0],
              m_vertices[ix*3+1].Vertex[1],
              m_vertices[ix*3+1].Vertex[2],
              m_vertices[ix*3+2].Vertex[0],
              m_vertices[ix*3+2].Vertex[1],
              m_vertices[ix*3+2].Vertex[2]);
    }
  }
}

//-------------------------------------------------------------
void SaveStlPolygons(CObjectManager     * pObjectManager,int binary)
{

  for (int i =0;i < IncludeList.size();i++) {
    IncludeType inc;
    inc = IncludeList[i];
    CObject * pOb  =  inc.m_ObjectManager->GetFirst(NULL,-1);
    while (pOb) {
      pOb->SaveStlObject();
      pOb =  inc.m_ObjectManager->GetNext();
    }
  }
}

//-------------------------------------------------------------
typedef std::vector< CPolyLineObject* > PolyFace;
class CDreiTafelObj
{
public:
//  int m_layer;
  int m_color;
  PolyFace pPlObjVorne;
  PolyFace pPlObjLinks;
  PolyFace pPlObjOben;
  int NumPointsVorne;
  int NumPointsLinks;
  int NumPointsOben;
  char Kontur;
  char DritteDim;
  char HilfsDim;


  CDreiTafelObj (/*int l,*/int c):/*m_layer(l),*/m_color(c)
  {
    NumPointsVorne=0;
    NumPointsLinks=0;
    NumPointsOben=0;
    Kontur = ' ';
    DritteDim = ' ';
    HilfsDim = ' ';
  };

  ~CDreiTafelObj ()
  {
    //for (int i=0;i < pPlObj.size();i++)delete pPlObj[i];
    pPlObjVorne.clear();
    pPlObjLinks.clear();
    pPlObjOben.clear();
  };
  int Add(CPolyLineObject * o)
  {
    // mittelpunkt entscheidet
    int xm = (o->m_Xmax  + o->m_Xmin)/2;
    int ym = (o->m_Ymax + o->m_Ymin)/2;

    if (xm < XMitte && ym > YMitte) {
      o->m_Projektion = ANSICHTvVORNE;
      NumPointsVorne += o->m_Path.size();
      if (o->m_ClosedLoop) NumPointsVorne += 50;
      if (o->m_Konstruktion==FORM) NumPointsVorne = 10;
      pPlObjVorne.push_back(o);
    } else if (xm < XMitte && ym < YMitte) {
      o->m_Projektion = ANSICHTvOBEN;
      NumPointsOben += o->m_Path.size();
      pPlObjOben.push_back(o);
      if (o->m_ClosedLoop) NumPointsOben += 50;
      if (o->m_Konstruktion==FORM) NumPointsOben = 10;
    } else {
      o->m_Projektion = ANSICHTvLINKS;
      NumPointsLinks += o->m_Path.size();
      pPlObjLinks.push_back(o);
      if (o->m_ClosedLoop) NumPointsLinks += 50;
      if (o->m_Konstruktion==FORM) NumPointsLinks = 10;
    }
    return 1;
  }
  int Analize(CPolyLineObject * o);

};


//------------------------------------------------------74-------
int CDreiTafelObj ::Analize(CPolyLineObject * o)
{
  float ValVorne = (NumPointsVorne)?(float)NumPointsVorne / (float)pPlObjVorne.size():0;
  float ValLinks = (NumPointsLinks)?(float)NumPointsLinks / (float)pPlObjLinks.size():0;
  float ValOben  = (NumPointsOben)?(float)NumPointsOben   / (float)pPlObjOben.size():0;
  Kontur = ' ';
  DritteDim = ' ';
  HilfsDim = ' ';

  if (ValVorne > ValLinks) {
    if (ValVorne > ValOben) {
      Kontur = ANSICHTvVORNE;
      if (pPlObjLinks.size() == pPlObjOben.size())DritteDim = ValLinks > ValOben ?ANSICHTvLINKS:ANSICHTvOBEN;
      else    DritteDim = pPlObjLinks.size() > pPlObjOben.size()?ANSICHTvLINKS:ANSICHTvOBEN;
      HilfsDim  = (DritteDim==ANSICHTvOBEN)?ANSICHTvLINKS:ANSICHTvOBEN;
    } else  {
      Kontur = ANSICHTvOBEN;
      if (pPlObjLinks.size() == pPlObjVorne.size())DritteDim = ValLinks > ValVorne ?ANSICHTvLINKS:ANSICHTvVORNE;
      else   DritteDim = pPlObjLinks.size() > pPlObjVorne.size()?ANSICHTvLINKS:ANSICHTvVORNE;
      HilfsDim  = (DritteDim==ANSICHTvVORNE)?ANSICHTvLINKS:ANSICHTvVORNE;
    }
  } else if (ValVorne < ValLinks) {
    if (ValLinks > ValOben) {
      Kontur = ANSICHTvLINKS;
      if (pPlObjVorne.size() == pPlObjOben.size()) DritteDim = ValVorne > ValOben?ANSICHTvVORNE:ANSICHTvOBEN;
      else DritteDim = pPlObjVorne.size() > pPlObjOben.size()?ANSICHTvVORNE:ANSICHTvOBEN;
      HilfsDim  = (DritteDim==ANSICHTvOBEN)?ANSICHTvVORNE:ANSICHTvOBEN;

    } else {
      Kontur = ANSICHTvOBEN;
      if (pPlObjLinks.size() == pPlObjVorne.size()) DritteDim = ValLinks > ValVorne?ANSICHTvLINKS:ANSICHTvVORNE;
      else  DritteDim = pPlObjLinks.size() > pPlObjVorne.size()?ANSICHTvLINKS:ANSICHTvVORNE;
      HilfsDim  = (DritteDim==ANSICHTvVORNE)?ANSICHTvLINKS:ANSICHTvVORNE;
    }
  } else {
    if (ValLinks >= ValOben) {
      if (pPlObjVorne.size()  < pPlObjLinks.size() ) {
        Kontur = ANSICHTvLINKS;
        DritteDim = ANSICHTvVORNE;
        HilfsDim  = ANSICHTvOBEN;
      } else {
        Kontur = ANSICHTvVORNE;
        DritteDim = ANSICHTvOBEN;
        HilfsDim  = ANSICHTvLINKS;
      }
    } else {
      Kontur = ANSICHTvOBEN;
      if (pPlObjVorne.size() == pPlObjLinks.size()) DritteDim = ValVorne > ValLinks?ANSICHTvVORNE:ANSICHTvLINKS;
      else  DritteDim = pPlObjVorne.size() > pPlObjLinks.size()?ANSICHTvVORNE:ANSICHTvLINKS;
      HilfsDim  = (DritteDim==ANSICHTvVORNE)?ANSICHTvLINKS:ANSICHTvVORNE;
    }
  }
  // Sind Polygone zum ausschneiden innwendig
  if (Kontur==ANSICHTvVORNE) {
    int cnt = pPlObjVorne.size();
    if (cnt == 1) return 1;
    for(int i = 0; i < cnt; ++i) {
      if (pPlObjVorne[i]) {
        for(int i2 = 0; i2 < cnt; ++i2) {
          if (i != i2 && pPlObjVorne[i2] && pPlObjVorne[i]) {
            int n = pPlObjVorne[i]->m_Path.size();
            int res= n > 3;
            int nn = 100;
            while (res && n) {
              n--;
              res = ClipperLib::PointInPolygon(pPlObjVorne[i]->m_Path[n],pPlObjVorne[i2]->m_Path);
            }
            if (res == 1) {
              CPolyLineObject *  Iterator=pPlObjVorne[i2];
              while (Iterator->m_Insider && nn--)  Iterator = Iterator->m_Insider;
              Iterator->m_Insider=pPlObjVorne[i];
              if (pPlObjVorne[i]) pPlObjVorne[i]->m_Konstruktion=VERARBEITEDT;
              pPlObjVorne[i]=NULL;
              res++;
            }
          }
        }
      }
    }
  } else if (Kontur==ANSICHTvLINKS) {
    int cnt = pPlObjLinks.size();
    if (cnt == 1) return 1;
    for(int i = 0; i < cnt; ++i) {
      if (pPlObjLinks[i]) {
        for(int i2 = 0; i2 < cnt; ++i2) {
          if (i != i2 && pPlObjLinks[i2] && pPlObjLinks[i]) {
            int n = pPlObjLinks[i]->m_Path.size();
            int res= n > 3;
            int nn = 100;
            while (res && n) {
              n--;
              res = ClipperLib::PointInPolygon(pPlObjLinks[i]->m_Path[n],pPlObjLinks[i2]->m_Path);
            }
            if (res == 1) {
              CPolyLineObject *  Iterator=pPlObjLinks[i2];
              while (Iterator->m_Insider && nn--)  Iterator = Iterator->m_Insider;
              Iterator->m_Insider=pPlObjLinks[i];
              if (pPlObjLinks[i]) pPlObjLinks[i]->m_Konstruktion=VERARBEITEDT;
              pPlObjLinks[i]=NULL;
              res++;
            }
          }
        }
      }
    }
  } else if (Kontur==ANSICHTvOBEN) {
    int cnt = pPlObjOben.size();
    if (cnt == 1) return 1;
    for(int i = 0; i < cnt; ++i) {
      if (pPlObjOben[i]) {
        for(int i2 = 0; i2 < cnt; ++i2) {
          if (i != i2 && pPlObjOben[i2] && pPlObjOben[i]) {
            int n = pPlObjOben[i]->m_Path.size();
            int res= n > 3;
            int nn = 100;
            while (res && n) {
              n--;
              res = ClipperLib::PointInPolygon(pPlObjOben[i]->m_Path[n],pPlObjOben[i2]->m_Path);
            }
            if (res == 1) {
              res++;
              CPolyLineObject *  Iterator=pPlObjOben[i2];
              while (Iterator->m_Insider && nn--)  Iterator = Iterator->m_Insider;
              Iterator->m_Insider=pPlObjOben[i];
              if (pPlObjOben[i]) pPlObjOben[i]->m_Konstruktion=VERARBEITEDT;
              pPlObjOben[i]=NULL;
            }
          }
        }
      }
    }
  }
  return 1;
 }

#define AnzahlDreiTafelObj  AnzahlCNC2/4
CDreiTafelObj  *  rgDreiTafelObj[AnzahlDreiTafelObj];
int DreiTafelObjCnt =0;


CDreiTafelProjektion * pDreiTafelProjektion = NULL;
CPolygonObject       * NeuesPolylineneElement(CObjectManager     * pObjectManager,const CPolyLineObject * Kontur,char Projektion)
{
    if (pObjectManager->CNCInfoCnt >= AnzahlCNC2-1) return NULL;
    char layerstr[120];
    sprintf(layerstr,"3D%c",Projektion);
    sprintf(layerstr,"%s",Kontur->m_pLayer->m_Name);
    CPolygonObject * Ob = new CPolygonObject(pObjectManager,Kontur->m_S.X,Kontur->m_S.Y,Kontur->m_S.Z,layerstr,Kontur->ETiefe);
    if (Ob==NULL) return NULL;
    Ob->m_ClosedLoop=1;
    Ob->m_Projektion=Projektion;
    Ob->m_Konstruktion=KONTUR;
    pObjectManager->CNCInfo[pObjectManager->CNCInfoCnt] = (CObject*) Ob;
    pObjectManager->CNCInfoCnt++;
    return (CDreiTafelProjektion * )Ob;
}

CDreiTafelProjektion * NeuesPolyStlElement(CObjectManager     * pObjectManager,const CPolyLineObject * Kontur,char Projektion)
{
//  if (gbUsePolygonObject || gpStlObject==NULL) {
//    if (pObjectManager->CNCInfoCnt >= AnzahlCNC2-1) return NULL;
//    char layerstr[120];
//    sprintf(layerstr,"3D%c",Projektion);
//    sprintf(layerstr,"%s",Kontur->m_pLayer->m_Name);
//    CPolygonObject * Ob = new CPolygonObject(Kontur->m_S.X,Kontur->m_S.Y,Kontur->m_S.Z,layerstr,Kontur->ETiefe);
//    if (Ob==NULL) return NULL;
//    Ob->m_ClosedLoop=1;
//    Ob->m_Projektion=Projektion;
//    Ob->m_Konstruktion=KONTUR;
//    pObjectManager->CNCInfo[pObjectManager->CNCInfoCnt] = (CObject*) Ob;
//    pObjectManager->CNCInfoCnt++;
//    return (CDreiTafelProjektion * )Ob;
//  } else {
    if (pObjectManager->CNCInfoCnt >= AnzahlCNC2-1) return NULL;
    char layerstr[120];
    sprintf(layerstr,"3D%c",Projektion);
    sprintf(layerstr,"%s",Kontur->m_pLayer->m_Name);
    if (pDreiTafelProjektion ==NULL) {
      pDreiTafelProjektion  = new CDreiTafelProjektion(pObjectManager,Kontur->m_S.X,Kontur->m_S.Y,Kontur->m_S.Z,layerstr,Kontur->ETiefe);
    }
    pDreiTafelProjektion  ->m_pLayer = Kontur->m_pLayer;
    pDreiTafelProjektion  ->m_S.X = Kontur->m_S.X;
    pDreiTafelProjektion  ->m_S.Y = Kontur->m_S.Y;
    pDreiTafelProjektion  ->m_S.Z = Kontur->m_S.Z;
    pDreiTafelProjektion  ->ETiefe= Kontur->ETiefe;
    pDreiTafelProjektion  ->m_Paths.clear();
    pDreiTafelProjektion  ->m_nx=0;
    pDreiTafelProjektion  ->m_ny=0;
    pDreiTafelProjektion  ->m_nz=0;
    pDreiTafelProjektion  ->m_PathIndex=-1;
    pDreiTafelProjektion  ->m_Projektion=Projektion;
    pDreiTafelProjektion  ->m_Xmin   = 0x7FFFFFFF;
    pDreiTafelProjektion  ->m_Xmax   = 0x80000000;
    pDreiTafelProjektion  ->m_Ymin   = 0x7FFFFFFF;
    pDreiTafelProjektion  ->m_Ymax   = 0x80000000;
    pDreiTafelProjektion  ->m_Zmin   = 0x7FFFFFFF;
    pDreiTafelProjektion  ->m_Zmax   = 0x80000000;

    int x= Kontur->m_S.X;;
    int y= Kontur->m_S.Y;;
    int z= Kontur->m_S.Z;;

    if      (x < pObjectManager->MinMaxleft)       pObjectManager->MinMaxleft  = x;
    else if (x > pObjectManager->MinMaxright)      pObjectManager->MinMaxright = x;
    if      (y < pObjectManager->MinMaxtop)        pObjectManager->MinMaxtop   = y;
    else if (y > pObjectManager->MinMaxbottom)     pObjectManager->MinMaxbottom= y;
    if      (z < pObjectManager->MinMaxfront)      pObjectManager->MinMaxfront = z;
    else if (z > pObjectManager->MinMaxback)       pObjectManager->MinMaxback  = z;

    return pDreiTafelProjektion  ;
//  }
}
//-------------------------------------------------------------
void CObjectManager::Make3DreiTafelProjektion(ClipperLib::cInt oX,ClipperLib::cInt oY,ClipperLib::cInt oZ,int Winkel)
{
  int n,NodeIx;
  for (n=0; n< DreiTafelObjCnt; n++) {
    delete rgDreiTafelObj[n];
  }
  XMitte=0;
  YMitte=0;
  ZMitte=0;
  float Drehung  =  (float)(Winkel * M_PI / 180.0);
  float FixCos = cos(Drehung);
  float FixSin = sin(Drehung);

  DreiTafelObjCnt=0;
  CObject * pObj;
  CLayers * p3DreiTafelLayer = IsLayerIndex(this,"DREITAFEL");
  CPolyLineObject * pPlObj1;
  gpStlObject=NULL;
  int stlobcnt=0;
  int DreiTafelfound=0;
  // DreiTafel Trennlinien Refernz finden
  m_3DreiTafelProjektionStartcnt = CNCInfoCnt;
  {
    for (int Index  = 0; Index < m_3DreiTafelProjektionStartcnt; Index++) {
      pPlObj1 = (CPolyLineObject*)CNCInfo[Index];
      if (pPlObj1) {
        if (pPlObj1->m_pLayer==p3DreiTafelLayer && pPlObj1->Art == Linie) {
          if (pPlObj1->m_Path.size()==2) {
            if (pPlObj1->m_Path[0].X == pPlObj1->m_Path[1].X) {
              XMitte = pPlObj1->m_Path[0].X;
            } else if (pPlObj1->m_Path[0].Y == pPlObj1->m_Path[1].Y) {
              YMitte = pPlObj1->m_Path[0].Y;
            }
            DreiTafelfound++;
          }
        }
      }
    }
  }
if (DreiTafelfound< 2) return ;
  int Index;
  // Verwandte Polylinie suchen
  CLayers * pLayer = gpLayers;
  while (pLayer) {
    if (pLayer->m_LayerSwitch) {
      //for (int color =0; color < 256 ; color ++) 
      {
        NodeIx=0;
        CDreiTafelObj  * AktuellesObj=NULL;
        for (Index  = 0; Index < m_3DreiTafelProjektionStartcnt; Index++) {
          CPolyLineObject * pPlObj = (CPolyLineObject*)CNCInfo[Index];
          if (pPlObj) {
            if (pPlObj->m_pLayer==pLayer && pPlObj->m_pLayer!=p3DreiTafelLayer  
            && pPlObj->m_Konstruktion != VERARBEITEDT
            && /*(pPlObj->ETiefe&0xFF) == color  &&*/  pPlObj->Art == Linie) {
              if (DreiTafelObjCnt < AnzahlDreiTafelObj) {
                //pPlObj->m_Konstruktion=true;
                if (AktuellesObj==NULL) {
                  AktuellesObj =  new CDreiTafelObj(0);//color);
                  rgDreiTafelObj[DreiTafelObjCnt] = AktuellesObj;
                  DreiTafelObjCnt++;
                }
                AktuellesObj->Add(pPlObj);
                NodeIx++;
              }
            }
          }
        }
      }
    }
    pLayer = pLayer->m_NextLayer;
  }


  const PolyFace    * KonturList = NULL;
  const PolyFace    * AxisList   = NULL;
  const PolyFace    * AxisHilfsList   = NULL;
  CPolyLineObject   * Kontur = NULL;
  CPolyLineObject   * Axis   = NULL;
  CPolyLineObject   * AxisHilfs = NULL;
  int               * rgFormPaarIx = NULL;

  CDreiTafelProjektion        * Ob = NULL;
  CPolygonObject              * ObPl=NULL;
  int Ox = 0;
  int Oy = 0;
  int Oz = 0;

  int NumPoints2 = 0;

  //fprintf(stderr,"\n");
  {
    int AnzahlKontPk=60;
    int KonturenTiefe=0;
    int AnzahlAxPk=60;
    int AHilfsAxPk=60;
    int FormPaarIxSize=0;
    //int AHilfsAxen=60;
    int ArraySize=sizeof(ClipperLib::IntPoint )*AnzahlKontPk*AnzahlAxPk;
    ClipperLib::IntPoint * rgKonturen = (ClipperLib::IntPoint *) malloc (ArraySize);
    ClipperLib::IntPoint * rgAxen     = (ClipperLib::IntPoint *) malloc (ArraySize);
    ClipperLib::IntPoint * rgHilfsAxen= (ClipperLib::IntPoint *) malloc (ArraySize);
    ClipperLib::IntPoint * rgResult   = (ClipperLib::IntPoint *) malloc (ArraySize);
    rgFormPaarIx = (int*) malloc (((FormPaarIxSize=AnzahlAxPk)+2)*sizeof(int));

    for (NodeIx=0; NodeIx< DreiTafelObjCnt; NodeIx++) {
      CDreiTafelObj  * node = rgDreiTafelObj[NodeIx] ;
      node ->Analize(NULL);
      // Die 3 Partner bestimmen
      if      (node ->Kontur==ANSICHTvOBEN) {
        KonturList = &node->pPlObjOben;
        if (node ->DritteDim==ANSICHTvLINKS) {
          AxisList = &node->pPlObjLinks;
          AxisHilfsList = &node->pPlObjVorne;
        } else {
          AxisList = &node->pPlObjVorne;
          AxisHilfsList = &node->pPlObjLinks;
        }
      } else if (node ->Kontur==ANSICHTvLINKS) {
        KonturList = &node->pPlObjLinks;
        if (node ->DritteDim==ANSICHTvOBEN) {
          AxisList = &node->pPlObjOben;
          AxisHilfsList = &node->pPlObjVorne;
        } else {
          AxisList = &node->pPlObjVorne;
          AxisHilfsList = &node->pPlObjOben;
        }
      } else {
        KonturList = &node->pPlObjVorne;
        if (node ->DritteDim==ANSICHTvLINKS) {
          AxisList = &node->pPlObjLinks;
          AxisHilfsList = &node->pPlObjOben;
        } else {
          AxisList = &node->pPlObjOben;
          AxisHilfsList = &node->pPlObjLinks;
        }
      }
      int LetzteKontur=0;
      if (gbShowTransformation) {
        // Hilfe um die Kovertierte Polyline anzuzeigen
        for (int l1=0; l1 < KonturList->size(); l1++) {
          Kontur = KonturList->at(l1);
          if (Kontur != NULL)  LetzteKontur=l1;
        }
      }
      // ArbeitsSpeicherDimension bestimmen
      for (int l1=0; l1 < KonturList->size(); l1++) {
        Kontur = KonturList->at(l1);
        if (Kontur != NULL) {
          int Konstruktion = Kontur ->m_Konstruktion;
          if (AxisList ) {
            int AnzahlAxen= AxisList->size();
            for (int la=0; la < AnzahlAxen; la++) {

          float W;
          float H;
          float T;
          AnzahlKontPk=0;
          KonturenTiefe=0;
          AnzahlAxPk=0;
          CPolyLineObject * Iterator = Kontur;
          while (Iterator!= NULL) {
            AnzahlKontPk += Iterator->m_Path.size();
            Iterator->m_Konstruktion=VERARBEITEDT;
            KonturenTiefe++;
            if (Iterator == Iterator->m_Insider) {
              Iterator->m_Insider=NULL;
            };
            Iterator = Iterator->m_Insider;
          }
//          if (AxisList ) {
  //          for (int la=0; la < AxisList->size(); la++) {
              Axis = AxisList->at(la);
              if (Axis ) {
                //if (AnzahlAxPk < Axis->m_Path.size()) 
                AnzahlAxPk=Axis->m_Path.size();
              }
    //        }
      //    }
          if (AxisHilfsList) {
          //  for (int la=0; la < AxisHilfsList->size(); la++) {
            if (la < AxisHilfsList->size()) {
              AxisHilfs = AxisHilfsList->at(la);
              if (AxisHilfs) {
                if (AHilfsAxPk < AxisHilfs->m_Path.size()) AHilfsAxPk=AxisHilfs->m_Path.size();
                //AHilfsAxen++;
              }
            }
          }

#define MINIMUMPUNKTE  6
          // Speichergroesse kontrollieren
          int sollsize = sizeof(ClipperLib::IntPoint )*AnzahlKontPk*AnzahlAxPk;
          if (ArraySize < sollsize) {
            free(rgKonturen);
            free(rgAxen);
            free(rgHilfsAxen);
            free(rgResult);
            rgKonturen = (ClipperLib::IntPoint *) malloc (sollsize);
            rgAxen     = (ClipperLib::IntPoint *) malloc (sollsize);
            rgHilfsAxen= (ClipperLib::IntPoint *) malloc (sollsize);
            rgResult   = (ClipperLib::IntPoint *) malloc (sollsize);
            //int s = malloc_usable_size(rgKonturen);
            ArraySize = sollsize;
          }
          if (FormPaarIxSize<AnzahlAxPk) {
            free (rgFormPaarIx);
            rgFormPaarIx = (int*) malloc (((FormPaarIxSize=AnzahlAxPk)+2)*sizeof(int));
           
          }

          int Ebenen = AnzahlAxPk/2;
          int Punkte = AnzahlKontPk;
          unsigned int nAxen=Ebenen*2;
          memset(rgHilfsAxen,0,ArraySize);

          if (AnzahlAxPk > 3) {
            for (int ie=0; ie < Ebenen; ie++) {
              rgFormPaarIx[ie*2]=ie;
              rgFormPaarIx[(ie*2)+1]= nAxen - ie -1;
            }
          } else {
            *rgFormPaarIx =1;
            *(rgFormPaarIx+1)=2;
            *(rgFormPaarIx+2)=1;
            *(rgFormPaarIx+3)=2;
          }

//          if (AxisList ) {
//            int AnzahlAxen= AxisList->size();
//            for (int la=0; la < AnzahlAxen; la++) {
              int   m_left   = 0x7FFFFFFF;
              int   m_right  = 0x80000000;
              int   m_bottom = 0x7FFFFFFF;
              int   m_top    = 0x80000000;
              int   m_front  = 0x7FFFFFFF;
              int   m_back   = 0x80000000;

              //--------------------------
              if      (node ->Kontur==ANSICHTvOBEN)  {
                //--------------------------
                Iterator = Kontur;
                int Offset=0;
                int Deep=0;
                while (Iterator!= NULL) {
                  int i=0;
                  int umkehr= (Deep)?Iterator->m_Path.size()-1:0;
                  for (i=0; i < Iterator->m_Path.size(); i++) {
                    int index = (umkehr)?umkehr-i:i;
                    ClipperLib::IntPoint XYZ = Iterator->m_Path[index];
                    XYZ.Attr=Deep;
                    rgKonturen[Offset+i] = XYZ;
                    if (XYZ.X< m_left  )  m_left  = XYZ.X;
                    if (XYZ.X> m_right )  m_right = XYZ.X;
                    if (XYZ.Y< m_bottom)  m_bottom= XYZ.Y;
                    if (XYZ.Y> m_top   )  m_top   = XYZ.Y;
                  }
                  Offset+=i;
                  Deep++;
                  Iterator = Iterator->m_Insider;
                }

                Axis = AxisList->at(la);
                if (Axis ) {
                  if      (node ->DritteDim==ANSICHTvVORNE)  {
                    int mitte  = (Axis->m_Xmin + Axis->m_Xmax)/2 ;
                    if (AnzahlAxen > 1 && (mitte < Kontur->m_Xmin || mitte > Kontur->m_Xmax)){ 
                      continue;
                    }


                    int umkehr= (1)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      XYZ.Z = XYZ.Y - YMitte;
                      XYZ.Y = 0;
                      if (gbShowTransformation && l1 == LetzteKontur) Axis->m_Path[index] = XYZ;
                      if (i < Ebenen) XYZ.X -= m_left;
                      else            XYZ.X -= m_right;
                      rgAxen[i] = XYZ;
                      if (XYZ.Z< m_front )  m_front = XYZ.Z;
                      if (XYZ.Z> m_back  )  m_back  = XYZ.Z;
                    }
                  } else if (node ->DritteDim==ANSICHTvLINKS) {
                    int Ymitte  = XMitte + YMitte - (Axis->m_Xmin + Axis->m_Xmax)/2 ;
                    if (AnzahlAxen > 1 && (Ymitte < Kontur->m_Ymin || Ymitte > Kontur->m_Ymax)){ 
                      continue;
                    }

                    int umkehr= (1)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      XYZ.Z = XYZ.Y - YMitte;
                      XYZ.Y = YMitte + XMitte - XYZ.X;
                      XYZ.X = 0;
                      if (gbShowTransformation && l1 == LetzteKontur) Axis->m_Path[index] = XYZ;
                      if (i < Ebenen) XYZ.Y -= m_top;
                      else            XYZ.Y -= m_bottom;
                      rgAxen[i] = XYZ;
                      if (XYZ.Z< m_front )  m_front = XYZ.Z;
                      if (XYZ.Z> m_back  )  m_back  = XYZ.Z;
                    }
                  }
                }
                if (AxisHilfsList && AxisHilfsList->size()) {
                  AxisHilfs= AxisHilfsList->at(0);
                  if (AxisHilfs) {
                    AxisHilfs->m_Konstruktion=VERARBEITEDT;
                    if      (node ->HilfsDim==ANSICHTvVORNE)  {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = XYZ.Y - YMitte;
                        XYZ.Y = 0;
                        if (gbShowTransformation && l1 == LetzteKontur) AxisHilfs->m_Path[index] = XYZ;
                        if (i < Ebenen) XYZ.X -= m_left;
                        else            XYZ.X -= m_right;
                        rgHilfsAxen[i] = XYZ;
                      }
                    } else if (node ->HilfsDim==ANSICHTvLINKS) {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = XYZ.Y - YMitte;
                        XYZ.Y = YMitte + XMitte - XYZ.X;
                        XYZ.X = 0;
                        if (gbShowTransformation && l1 == LetzteKontur) AxisHilfs->m_Path[index] = XYZ;
                        if (i < Ebenen) XYZ.Y -= m_top;
                        else            XYZ.Y -= m_bottom;
                        rgHilfsAxen[i] = XYZ;
                      }
                    }
                  }
                }
                W = m_right-m_left;
                H = m_top-m_bottom;
                T = m_back-m_front;


                if (W && H && T) {
                  if (AnzahlAxPk < MINIMUMPUNKTE && Axis->m_Konstruktion!=FORM) {  // Kein Profil, dritte dim. genmaess Min Max
                    Ebenen=2;
                    for (int a1=0; a1 < Ebenen; a1++) {
                      for (int k1=0; k1 < AnzahlKontPk; k1++) {
                        ClipperLib::IntPoint P = rgKonturen[k1];
                        P.Z += (a1) ?m_back:m_front;
                        rgResult [(Punkte*a1)+ k1] =P;
                      }
                    }
                  } else  {
                    for (int a1=0; a1 < Ebenen; a1++) {
                      ClipperLib::IntPoint P1 = rgAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2 = rgAxen[rgFormPaarIx[a1*2+1]];
                      ClipperLib::IntPoint P1h = rgHilfsAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2h = rgHilfsAxen[rgFormPaarIx[a1*2+1]];
                      if (node->DritteDim==ANSICHTvLINKS)  {
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float xp = m_right  - P.X;
                          float yp = m_top - P.Y;
                          // liear interpolieren
                          if (P.Attr==0) { // Innenseiten nicht modifizieren
                            P.X += (((float)P1h.X * xp / W) + ((float)P2h.X * (W-xp) / W));
                            P.Y += (((float)P2.Y * yp / H) + ((float)P1.Y * (H-yp) / H));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }
                          // Hauptaxe Y
                          if (P1h.Z) {
                            // Hat Hilsrichtung X
                            float z = (((float)P1h.Z * xp / W) + ((float)P2h.Z * (W-xp) / W));
                            z      += (((float)P2.Z * yp / H) + ((float)P1.Z * (H-yp) / H));
                            P.Z    += z-((P1h.Z+P2h.Z)/2.0);
                          } else {
                            P.Z += (((float)P2.Z * yp / H) + ((float)P1.Z * (H-yp) / H));
                          }
                          rgResult [(Punkte*a1)+ k1] =P;
                        }
                      } else {
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float xp = m_right  - P.X;
                          float yp = m_top - P.Y;
                          // liear interpolieren
                          if (P.Attr==0) { // Innenseiten nicht modifizieren
                            P.X += (((float)P1.X * xp / W) + ((float)P2.X * (W-xp) / W));
                            P.Y += (((float)P2h.Y * yp / H) + ((float)P1h.Y * (H-yp) / H));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }
                          
// Hauptaxe X
                          if (P1h.Z) {
                            // Hat Hilsrichtung Y
                            float z = (((float)P1.Z * xp / W) + ((float)P2.Z * (W-xp) / W));
                            z      += (((float)P2h.Z * yp / H) + ((float)P1h.Z * (H-yp) / H));
                            P.Z    += z-((P1h.Z+P2h.Z)/2.0);
                          } else {
                            P.Z += (((float)P1.Z * xp / W) + ((float)P2.Z * (W-xp) / W));
                          }
                          rgResult [(Punkte*a1)+ k1] =P;
                        }
                      }
                    }
                  }
                }
                if (abs(W) > abs(T)) s_dir = (abs (W) > abs (H))?1:-1;
                else                 s_dir = (abs (T) < abs (H))?1:-1;
                //--------------------------
              } else if      (node ->Kontur==ANSICHTvVORNE)  {
                //--------------------------
                 Iterator = Kontur;
                int Offset=0;
                int Deep=0;
                while (Iterator!= NULL) {
                  int i=0;
                  int umkehr= (!Deep)?Iterator->m_Path.size()-1:0;
                  for (i=0; i <  Iterator->m_Path.size(); i++) {
                    int index = (umkehr)?umkehr-i:i;
                    ClipperLib::IntPoint XYZ = Iterator->m_Path[index];
                    XYZ.Z += XYZ.Y - YMitte;
                    XYZ.Y = 0;
                    if (gbShowTransformation && la == AnzahlAxen-1) Iterator->m_Path[index] = XYZ;
                    XYZ.Attr=Deep;
                    rgKonturen[Offset+i] = XYZ;
                    if (XYZ.X< m_left  )  m_left  = XYZ.X;
                    if (XYZ.X> m_right )  m_right = XYZ.X;
                    if (XYZ.Z< m_front )  m_front = XYZ.Z;
                    if (XYZ.Z> m_back  )  m_back  = XYZ.Z;
                  }
                  Offset+=i;
                  Deep++;
                  Iterator = Iterator->m_Insider;
                }

                Axis = AxisList->at(la);
                if (Axis ) {
                  if (node ->DritteDim==ANSICHTvLINKS) {
                    int mitte  = (Axis->m_Ymin + Axis->m_Ymax)/2;
                    if (AnzahlAxen > 1 && ( mitte < Kontur->m_Ymin || mitte > Kontur->m_Ymax)){ 
                      continue;
                    }
                    int umkehr= (1)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      XYZ.Z += XYZ.Y - YMitte;
                      XYZ.Y = YMitte + XMitte - XYZ.X;
                      XYZ.X = 0;
                      if (gbShowTransformation && l1 == LetzteKontur) Axis->m_Path[index] = XYZ;
                      if (i < Ebenen) XYZ.Z -= m_front;
                      else            XYZ.Z -= m_back;
                      rgAxen[i] = XYZ;
                      if (XYZ.Y< m_bottom   )  m_bottom   = XYZ.Y;
                      if (XYZ.Y> m_top)  m_top= XYZ.Y;
                    }
                  } else { // if (node ->DritteDim==ANSICHTvOBEN) {
                    int mitte  = (Axis->m_Xmin + Axis->m_Xmax)/2;
                    if (AnzahlAxen > 1 && (mitte < Kontur->m_Xmin || mitte > Kontur->m_Xmax)) {
                      continue;
                    }
                    int umkehr= (1)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      //XYZ.Z = 0;
                      if (i < Ebenen) XYZ.X -= m_left;
                      else            XYZ.X -= m_right;
                      rgAxen[i] = XYZ;
                      if (XYZ.Y< m_bottom   )  m_bottom   = XYZ.Y;
                      if (XYZ.Y> m_top)  m_top= XYZ.Y;
                    }
                  }
                }

                if (AxisHilfsList && AxisHilfsList->size()) {
                  AxisHilfs= AxisHilfsList->at(0);
                  if (AxisHilfs) {
                    AxisHilfs->m_Konstruktion=VERARBEITEDT;
                    if (node ->HilfsDim==ANSICHTvLINKS) {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = XYZ.Y - YMitte;
                        XYZ.Y = YMitte + XMitte - XYZ.X;
                        XYZ.X = 0;
                        if (gbShowTransformation && l1 == LetzteKontur) AxisHilfs->m_Path[index] = XYZ;
                        if (i < Ebenen)  XYZ.Z -= m_front;
                        else            XYZ.Z -= m_back;
                        rgHilfsAxen[i] = XYZ;
                      }
                    } else { // if (node ->HilfsDim==ANSICHTvOBEN) {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = 0;
                        if (i < Ebenen) XYZ.X -= m_left;
                        else            XYZ.X -= m_right;
                        rgHilfsAxen[i] = XYZ;
                      }
                    }
                  }
                }
                W = m_right-m_left;
                H = m_top-m_bottom;
                T = m_back-m_front;
                if (W && H && T) {
                  if (AnzahlAxPk < MINIMUMPUNKTE && Axis->m_Konstruktion!=FORM) {  // Kein Profil, dritte dim. genmaess Min Max
                    Ebenen=2;
                    for (int a1=0; a1 < Ebenen; a1++) {
                      for (int k1=0; k1 < AnzahlKontPk; k1++) {
                        ClipperLib::IntPoint P = rgKonturen[k1];
                        P.Y += (a1) ?m_top:m_bottom;
                        rgResult [(Punkte*a1)+ k1] =P;
                      }
                    }
                  } else {
                    for (int a1=0; a1 < Ebenen; a1++) {
                      ClipperLib::IntPoint P1 = rgAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2 = rgAxen[rgFormPaarIx[a1*2+1]];
                      ClipperLib::IntPoint P1h = rgHilfsAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2h = rgHilfsAxen[rgFormPaarIx[a1*2+1]];
                      if (node->DritteDim==ANSICHTvLINKS)  {
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float xp = m_right  - P.X;
                          float zp = m_back  - P.Z;
                          // liear interpolieren
                          if (P.Attr==0) {
                            P.X += (((float)P1h.X * xp / W) + ((float)P2h.X * (W-xp) / W));
                            P.Z += (((float)P1.Z * zp / T) + ((float)P2.Z * (T-zp) / T));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }
                          P.Y += (((float)P1.Y * zp / T) + ((float)P2.Y * (T-zp) / T));
                          P.Y += (((float)P1h.Y * xp / W) + ((float)P2h.Y * (W-xp) / W));
                          P.Y -= (P1h.Y + P2h.Y)/2.0;
                          rgResult [(Punkte*a1)+ k1] =P;

                        }
                      } else { // ANSICHTvVORNE
                        P1.Z = P1h.Z;
                        P2.Z = P2h.Z;
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float xp = m_right  - P.X;
                          float zp = m_back  - P.Z;
                          // liear interpolieren
                          if (P.Attr==0) {
                            P.X += (((float)P1.X * xp / W) + ((float)P2.X * (W-xp) / W));
                            P.Z += (((float)P1.Z * zp / T) + ((float)P2.Z * (T-zp) / T));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }
                          P.Y += (((float)P1h.Y * zp / T) + ((float)P2h.Y * (T-zp) / T));
                          P.Y -= (P1h.Y + P2h.Y)/2.0;
                          P.Y += (((float)P1.Y * xp / W) + ((float)P2.Y * (W-xp) / W));
                          rgResult [(Punkte*a1)+ k1] =P;
                        }
                      }
                    }
                  }
                }
                if (abs(W) > abs(T)) s_dir = (abs (W) > abs (H))?1:-1;
                else                 s_dir = (abs (T) < abs (H))?1:-1;
                //--------------------------
              } else  { // if  (node ->Kontur==ANSICHTvLINKS)
                //--------------------------
                Iterator = Kontur;
                Axis = AxisList->at(la);
                int Offset=0;
                int Deep=0;
                while (Iterator!= NULL) {
                  int i=0;
                  int umkehr=(!Deep)?Iterator->m_Path.size()-1:0;
                  for (i=0; i < Iterator->m_Path.size(); i++) {
                    int index = (umkehr)?umkehr-i:i;
                    ClipperLib::IntPoint XYZ = Iterator->m_Path[index];
                    XYZ.Z += XYZ.Y - YMitte;
                    XYZ.Y = YMitte + XMitte - XYZ.X;
                    XYZ.X = 0;
                    if (gbShowTransformation && la == AnzahlAxen-1) Iterator->m_Path[index] = XYZ;
                    XYZ.Attr=Deep;
                    rgKonturen[Offset+(i)] = XYZ;
                    if (XYZ.Y< m_bottom   )  m_bottom   = XYZ.Y;
                    if (XYZ.Y> m_top)  m_top= XYZ.Y;
                    if (XYZ.Z< m_front )  m_front = XYZ.Z;
                    if (XYZ.Z> m_back  )  m_back  = XYZ.Z;
                  }
                  Offset+=i;
                  Deep++;
                  Iterator = Iterator->m_Insider;
                }
                if (Axis ) {
                  if      (node ->DritteDim==ANSICHTvVORNE)  {
                    int Ymitte  = (Axis->m_Ymin + Axis->m_Ymax)/2;
                    if (AnzahlAxen > 1 && (Ymitte < Kontur->m_Ymin || Ymitte > Kontur->m_Ymax)){ 
                      continue;
                    }

                      int umkehr= (0)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      XYZ.Z += XYZ.Y - YMitte;
                      XYZ.Y = 0;
                      if (gbShowTransformation && l1 == LetzteKontur) Axis->m_Path[index] = XYZ;
                      if (i < Ebenen) XYZ.Z -= m_front;
                      else            XYZ.Z -= m_back;
                      rgAxen[i] = XYZ;
                      if (XYZ.X< m_left  )  m_left  = XYZ.X;
                      if (XYZ.X> m_right )  m_right = XYZ.X;
                    }
                  } else { // if (node ->DritteDim==ANSICHTvOBEN) {

//                    int Xmitte  = (Axis->m_Xmin + Axis->m_Xmax)/2;
//                    if (Xmitte < Kontur->m_Xmin || Xmitte > Kontur->m_Xmax ) {
//                      continue;
//                    }
                    int mitte  = XMitte + YMitte - (Axis->m_Ymin + Axis->m_Ymax)/2 ;
                    if (AnzahlAxen > 1 && (mitte < Kontur->m_Xmin || mitte > Kontur->m_Xmax)){ 
                      continue;
                    }

                    int umkehr= (0)?Axis->m_Path.size()-1:0;
                    for (int i=0; i < Axis->m_Path.size(); i++) {
                      int index = (umkehr)?umkehr-i:i;
                      ClipperLib::IntPoint XYZ = Axis->m_Path[index];
                      //XYZ.Z = 0;
                      if (i < Ebenen) XYZ.Y -= m_bottom;
                      else            XYZ.Y -= m_top;
                      rgAxen[i] = XYZ;
                      if (XYZ.X< m_left  )  m_left  = XYZ.X;
                      if (XYZ.X> m_right )  m_right = XYZ.X;
                    }
                  }
                }
                if (AxisHilfsList && AxisHilfsList->size()) {
                  AxisHilfs= AxisHilfsList->at(0);
                  if (AxisHilfs) {
                    AxisHilfs->m_Konstruktion=VERARBEITEDT;
                    if      (node ->HilfsDim ==ANSICHTvVORNE)  {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = XYZ.Y - YMitte;
                        XYZ.Y = 0;
                        if (gbShowTransformation && l1 == LetzteKontur) AxisHilfs->m_Path[index] = XYZ;
                        if (i < Ebenen)  XYZ.Z -= m_back;
                        else             XYZ.Z -= m_front;
                        rgHilfsAxen[i] = XYZ;
                      }
                    } else { // if (node ->HilfsDimDim==ANSICHTvOBEN) {
                      int umkehr= (1)?AxisHilfs->m_Path.size()-1:0;
                      for (int i=0; i < AxisHilfs->m_Path.size(); i++) {
                        int index = (umkehr)?umkehr-i:i;
                        ClipperLib::IntPoint XYZ = AxisHilfs->m_Path[index];
                        XYZ.Z = 0;
                        if (i < Ebenen)  XYZ.Y -= m_top;
                        else             XYZ.Y -= m_bottom;
                        rgHilfsAxen[i] = XYZ;
                      }
                    }
                  }
                }
                W = m_right-m_left;
                H = m_top-m_bottom;
                T = m_back-m_front;
                if (W && H && T) {
                  if (AnzahlAxPk < MINIMUMPUNKTE && Axis->m_Konstruktion!=FORM) {  // Kein Profil, dritte dim. genmaess Min Max
                    Ebenen=2;
                    for (int a1=0; a1 < Ebenen; a1++) {
                      for (int k1=0; k1 < AnzahlKontPk; k1++) {
                        ClipperLib::IntPoint P = rgKonturen[k1];
                        P.X += (a1) ?m_right:m_left;
                        rgResult [(Punkte*a1)+ k1] =P;
                      }
                    }
                  } else {
                    for (int a1=0; a1 < Ebenen; a1++) {
                      ClipperLib::IntPoint P1 = rgAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2 = rgAxen[rgFormPaarIx[a1*2+1]];
                      ClipperLib::IntPoint P1h = rgHilfsAxen[rgFormPaarIx[a1*2]];
                      ClipperLib::IntPoint P2h = rgHilfsAxen[rgFormPaarIx[a1*2+1]];
                      if (node->DritteDim==ANSICHTvVORNE)  {
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float yp = m_top - P.Y;
                          float zp = m_back  - P.Z;
                          // liear interpolieren
                          if (P.Attr==0) { // Innenseiten nicht modifizieren
                            P.Y += (((float)P2h.Y * yp / H) + ((float)P1h.Y * (H-yp) / H));
                            P.Z += (((float)P1.Z * zp / T) + ((float)P2.Z * (T-zp) / T));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }

                          if (P1h.X) {
                            float x = (((float)P1.X  * zp / T) + ((float)P2.X * (T-zp) / T));
                            x += (((float)P2h.X * yp / H) + ((float)P1h.X * (H-yp) / H));
                            P.X += x - (P2h.X +P1h.X)/2.0;
                          } else {
                            P.X += (((float)P1.X  * zp / T) + ((float)P2.X * (T-zp) / T));
                          }
                          rgResult [(Punkte*a1)+ k1] =P;
                        }
                      } else {   // ANSICHTvOBEN
                        for (int k1=0; k1 < AnzahlKontPk; k1++) {
                          ClipperLib::IntPoint P = rgKonturen[k1];
                          float yp = m_top - P.Y;
                          float zp = m_back  - P.Z;
                          // liear interpolieren
                          if (P.Attr==0) { // Innenseiten nicht modifizieren
                            P.Y += (((float)P1.Y * yp / H) + ((float)P2.Y * (H-yp) / H));
                            P.Z += (((float)P2h.Z * zp / T) + ((float)P1h.Z * (T-zp) / T));
                          } else if (a1 !=0 && a1 != (Ebenen-1)) { 
                            P.Attr=1;
                          }
                          if (P1h.X) {
                            float x = (((float)P2h.X  * zp / T) + ((float)P1h.X * (T-zp) / T));
                            x += (((float)P1.X * yp / H) + ((float)P2.X * (H-yp) / H));
                            P.X += x - (P2h.X +P1h.X)/2.0;
                          } else {
                            P.X += (((float)P1.X  * zp / T) + ((float)P2.X * (T-zp) / T));
                          }
                          rgResult [(Punkte*a1)+ k1] =P;
                        }
                      }
                    }
                  }
                }
                //--------------------------
                if (abs(W) > abs(T)) s_dir = (abs (W) > abs (H))?1:-1;
                else                 s_dir = (abs (T) < abs (H))?1:-1;
              }

              //--------------------------
              NumPoints2 = AnzahlKontPk * AnzahlAxPk * 4;
              if (!gbShowTransformation && CNCInfoCnt < AnzahlCNC2-1) {
                if (gpStlObject) gpStlObject ->GenerateGlBuffer();
                stlobcnt++;
                //printf("%3d %s\n",stlobcnt,Kontur->m_pLayer->m_Name);
                gpStlObject = new CStlObject(this,0,0,0,Kontur->m_pLayer->m_Name,1);
                gpStlObject->m_Alpha = Kontur->m_Alpha;
                gpStlObject->m_dir = Kontur->m_dir;
                gpStlObject->m_Texture=Kontur->m_Texture;// Konstruktion;
                gpStlObject->m_TextureZoom=Kontur->m_TextureZoom;// Konstruktion;
                CNCInfo[CNCInfoCnt] = (CObject*) gpStlObject;
                CNCInfoCnt++;
                gpStlObject->SetBufferSize(NumPoints2);

                ClipperLib::IntPoint po,P,P1,P2,P3,P4,PStart,PStart1,PStart2;
                if (oX||oY||oZ||Winkel) {
                  for (int y=0; y < Ebenen; y++) {
                    for (int x=0; x < Punkte; x++) {
                      P =  rgResult [(Punkte*y)+ x];
                      float pX = P.X;
                      float pY = P.Y;
                      P.X = oX + (((float)pX *FixCos) - ((float)pY *FixSin));
                      P.Y = oY + (((float)pY *FixCos) + ((float)pX *FixSin));
                      P.Z += oZ;
                      rgResult [(Punkte*y)+ x]=P;
                    }
                  }
                }



                int PolyDeep=0;
                for (int y=0; y < Ebenen; y++) {
                  if (y==0 || y == (Ebenen-1)) {
                    Ob = NeuesPolyStlElement(this,Kontur,ANSICHTvVORNE);
                    Ob->m_Konstruktion =  Konstruktion;
                    if (gbUsePolygonObject) {
                      ObPl = NeuesPolylineneElement(this,Kontur,ANSICHTvVORNE);
                      ObPl->AddPath(0);
                    }
                    Ob->AddPath(0);
                    PolyDeep=0;
                    for (int x=0; x < Punkte; x++) {
                      P =  rgResult [(Punkte*y)+ x];
                      if (x==0) PStart = P;
                      if (P.Attr != PolyDeep) {
                        if (gbUsePolygonObject) {
                          ObPl->AddVertex(PStart.X,PStart.Y,PStart.Z);
                          ObPl->AddPath(0);
                        }
                        // Neue Polilinie innside
                        PolyDeep = P.Attr;
                        Ob->AddPath(0);
                        PStart = P;
                      }
                      Ob->AddVertex(P.X,P.Y,P.Z);
                      if (gbUsePolygonObject) {
                        ObPl->AddVertex(P.X,P.Y,P.Z);
                      }
                    }
                    if (gbUsePolygonObject) {
                      ObPl->AddVertex(PStart.X,PStart.Y,PStart.Z);
                      ObPl->CalculateNormale(y);
                    }
                    Ob->CalculateNormale(y);
                  }
                  if (y< (Ebenen-1)) {
                    PolyDeep=0;
                    for (int x=0; x < Punkte; x++) {
                      P1 =  rgResult [(Punkte*(y)  )+ x  ];
                      P2 =  rgResult [(Punkte*(y+1))+ x  ];
                      P3 =  rgResult [(Punkte*(y)  )+ x+1];
                      P4 =  rgResult [(Punkte*(y+1))+ x+1];
                      if (P1.Attr==0 || Ebenen==2) {        //  Bei mehreren Ebenen keine Durchkontakierung
                        Ob = NeuesPolyStlElement(this,Kontur,ANSICHTvVORNE);
                        if (gbUsePolygonObject) {
                          ObPl = NeuesPolylineneElement(this,Kontur,ANSICHTvVORNE);
                          ObPl->AddPath(0);
                        }
                        Ob->AddPath(0);
                        if (x==0) {
                          PStart1 = P1;
                          PStart2 = P2;
                        }
                        if (x == Punkte-1 || P3.Attr != PolyDeep) { //Schliessen
                          P       = P3;
                          P3      = PStart1;
                          PStart1 = P;
                          P       = P4;
                          P4      = PStart2;
                          PStart2 = P;
                          PolyDeep=P.Attr;
                        }
                        Ob->AddVertex(P2.X,P2.Y,P2.Z);
                        Ob->AddVertex(P1.X,P1.Y,P1.Z);
                        Ob->AddVertex(P3.X,P3.Y,P3.Z);
                        Ob->AddVertex(P4.X,P4.Y,P4.Z);
                        Ob->CalculateNormale(1);
                        if (gbUsePolygonObject) {
                          ObPl->AddVertex(P2.X,P2.Y,P2.Z);
                          ObPl->AddVertex(P1.X,P1.Y,P1.Z);
                          ObPl->AddVertex(P3.X,P3.Y,P3.Z);
                          ObPl->AddVertex(P4.X,P4.Y,P4.Z);
                          ObPl->CalculateNormale(1);
                        }
                      }
                    }
                  }
                }
              } else if (0) {
                ObPl = NeuesPolylineneElement(this,Kontur,ANSICHTvVORNE);
                ObPl->AddPath(0);
                ObPl->ETiefe=2;
                for (int i=0; i < Ebenen*2; i++) {
                  ClipperLib::IntPoint P1 = rgAxen[i];
                  ObPl->AddVertex(P1.X,P1.Y,P1.Z);
                }
                ObPl->CalculateNormale(0);
                if (AxisHilfs) {
                  ObPl = NeuesPolylineneElement(this,Kontur,ANSICHTvVORNE);
                  ObPl->AddPath(0);
                  ObPl->ETiefe=3;
                  for (int i=0; i < Ebenen*2; i++) {
                    ClipperLib::IntPoint P1 = rgHilfsAxen[i];
                    ObPl->AddVertex(P1.X,P1.Y,P1.Z);
                  }
                  ObPl->CalculateNormale(0);
                }
              }
            }
          }
        }
      }
      for (int l2=0; l2 < KonturList->size(); l2++) {
        Kontur = KonturList->at(l2);
        if (Kontur != NULL) {
          for (int la=0; la < AxisList->size(); la++) {
            Axis = AxisList->at(la);
            if (gbShowTransformation) {
              if (AxisHilfsList && la < AxisHilfsList->size()) {
                  AxisHilfs = AxisHilfsList->at(la);
              } else AxisHilfs = NULL;
              if (Kontur) Kontur->m_Konstruktion=KONTUR;
              if (Axis) Axis->m_Konstruktion=KONTUR;
              if (AxisHilfs) AxisHilfs->m_Konstruktion = KONTUR;
            } else Axis->m_Konstruktion=VERARBEITEDT;
          }
        }
      }
    }
    if (rgKonturen)  free(rgKonturen);
    if (rgAxen) free(rgAxen);
    if (rgHilfsAxen) free(rgHilfsAxen);
    if (rgResult) free(rgResult);
    if (rgFormPaarIx) free(rgFormPaarIx);


    if (gpStlObject) gpStlObject ->GenerateGlBuffer();
    gpStlObject=NULL;
  }


  for (n=0; n< DreiTafelObjCnt; n++) {
    delete rgDreiTafelObj[n];
  }
  DreiTafelObjCnt=0;
  if (!gbUsePolygonObject) {
    if (pDreiTafelProjektion) delete(pDreiTafelProjektion);
  }
  pDreiTafelProjektion = NULL;


}

//--------------------------------------------------------------
void CStlObject::CalcualteNormalePlus(float nz)
{
  for (int ix=0; ix<(m_NumStlObjects); ix++) {
    v1 = m_vertices[ix*3+0];
    v2 = m_vertices[ix*3+1];
    v3 = m_vertices[ix*3+2];
    float   w1 = v3.Vertex[0] - v2.Vertex[0];
    float   h1 = v3.Vertex[1] - v2.Vertex[1];
    float   t1 = v3.Vertex[2] - v2.Vertex[2];
    float   w2 = v2.Vertex[0] - v1.Vertex[0];
    float   h2 = v2.Vertex[1] - v1.Vertex[1];
    float   t2 = v2.Vertex[2] - v1.Vertex[2];
    float   normalX =  (h1) * (t2) - (h2) * (t1);
    float   normalY =  (t1) * (w2) - (t2) * (w1);
    float   normalZ =  (w1) * (h2) - (w2) * (h1);
    if (normalX || normalY || normalZ) {
      normalX /= 2.0;
      normalY /= 2.0;
      float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      Stlnormal.Vertex[0] = normalX * isqr;
      Stlnormal.Vertex[1] = normalY * isqr;
      Stlnormal.Vertex[2] = normalZ * isqr;
      if ((Stlnormal.Vertex[2] > 0.0)!= (nz>0.0)) {
        // Drehrichtung ist wild gemischt
        m_vertices[ix*3+0] = v3;
        m_vertices[ix*3+2] = v1;
        Stlnormal.Vertex[0] = -Stlnormal.Vertex[0];
        Stlnormal.Vertex[1] = -Stlnormal.Vertex[1];
        Stlnormal.Vertex[2] = -Stlnormal.Vertex[2];
      }
      m_normals[ix*3+0] = Stlnormal;
      m_normals[ix*3+1] = Stlnormal;
      m_normals[ix*3+2] = Stlnormal;
    }
  }

  if (bShowNormale) { // Hilfe : zeige Normale
    int numVerts = m_NumStlObjects;
    float einheit = MMToScreen/1000;
    for (int ix=0; ix<(numVerts ); ix++) {
      v1 = m_vertices[ix*3+0];
      v2 = m_vertices[ix*3+1];
      v3 = m_vertices[ix*3+2];
      Stlnormal = m_normals[ix*3+0];

      float xx = (v1.Vertex[0] + v2.Vertex[0] + v3.Vertex[0])/3.0;
      float yy = (v1.Vertex[1] + v2.Vertex[1] + v3.Vertex[1])/3.0;
      float zz = (v1.Vertex[2] + v2.Vertex[2] + v3.Vertex[2])/3.0;
      GLfloat mat_diffuse[4];
      mat_diffuse[0] = 0.4;
      mat_diffuse[1] = 1.0;
      mat_diffuse[2] = 0.4;

      Stlcolor.Vertex[0]=0;
      Stlcolor.Vertex[1]=1;
      Stlcolor.Vertex[2]=0;
      v1.Vertex[0] = xx;
      v1.Vertex[1] = yy;
      v1.Vertex[2] = zz;
      v3 = v1;
      v3.Vertex[0] += einheit/2.0;
      v3.Vertex[1] += einheit/2.0;
      v3.Vertex[2] += einheit/2.0;
      yy+= Stlnormal.Vertex[1]*einheit *10;
      xx+= Stlnormal.Vertex[0]*einheit *10;
      zz+= Stlnormal.Vertex[2]*einheit *10;
      v2.Vertex[0] = xx;
      v2.Vertex[1] = yy;
      v2.Vertex[2] = zz;
      Stlnormal.Vertex[0]=0.5;
      Stlnormal.Vertex[1]=0.5;
      Stlnormal.Vertex[2]=0.5;
      gpStlObject->PushOneVertex();
      v3.Vertex[0] -= einheit;
      v3.Vertex[1] -= einheit;
      v3.Vertex[2] -= einheit;
      gpStlObject->PushOneVertex();
    }
  }
}

void CPolyLineObject::MakeStlProjection()
{
  if (gpStlObject==NULL) return;
  if (m_Path.size()) {
    int nP = m_Path.size()-3;

    float normalX =  0;
    float normalY =  0;
    float normalZ =  0;
    while (nP>=0) {
      float   w1 = m_Path[nP+2].X - m_Path[nP+1].X;
      float   h1 = m_Path[nP+2].Y - m_Path[nP+1].Y;
      float   t1 = m_Path[nP+2].Z - m_Path[nP+1].Z;
      float   w2 = m_Path[nP+1].X - m_Path[nP+0].X;
      float   h2 = m_Path[nP+1].Y - m_Path[nP+0].Y;
      float   t2 = m_Path[nP+1].Z - m_Path[nP+0].Z;
      normalX +=  (h1) * (t2) - (h2) * (t1);
      normalY +=  (t1) * (w2) - (t2) * (w1);
      normalZ +=  (w1) * (h2) - (w2) * (h1);
      nP--;



    }
    if (normalX || normalY || normalZ) {
      float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      m_nx = normalX * isqr;
      m_ny = normalY * isqr;
      m_nz = normalZ * isqr;
    }
  }
  if (m_Xmin < gpStlObject->m_Xmin)       gpStlObject->m_Xmin  = m_Xmin;
  if (m_Xmax > gpStlObject->m_Xmax)     gpStlObject->m_Xmax = m_Xmax;
  if (m_Ymin < gpStlObject->m_Ymin)         gpStlObject->m_Ymin   = m_Ymin ;
  if (m_Ymax > gpStlObject->m_Ymax)   gpStlObject->m_Ymax= m_Ymax;
  if (m_Zmin< gpStlObject->m_Zmin)      gpStlObject->m_Zmin = m_Zmin;
  if (m_Zmax > gpStlObject->m_Zmax)       gpStlObject->m_Zmax  = m_Zmax;


  DWORD clr = GetRGBColor();
  vector3 c = {(clr>>0 & 0xFF) / 255.0f,(clr>>8 & 0xFF) / 255.0f,(clr>>16 & 0xFF)/ 255.0f};
  vector3 n = {0,0,(m_nz>0.0f) ?1.0f:-1.0f};
  ClipperLib::Paths Paths;

  int ebene =0;
  int nx = abs(m_nx*10.0);
  int ny = abs(m_ny*10.0);
  int nz = abs(m_nz*10.0);
  if (nx > ny) {
    ebene = (nz > nx)?-1:0;
  } else {
    ebene = (nz > ny)?-1:1;
  }

  Paths.push_back(m_Path);
  int bstart = gpStlObject->m_NumStlObjects;
  gpStlObject->AddPolygon(Paths,&n,&c);
  gpStlObject->CalcualteNormalePlus(n.Vertex[2]);
  if (m_Texture>=0)
    gpStlObject->CalcTexture(bstart,gpStlObject->m_dir!=0?gpStlObject->m_dir:s_dir,ebene);
  Paths.clear();
}

void CObjectManager::MakePolylineProjektion(void)
{
  CPolyLineObject*pPlObj1 ;
  gpStlObject =NULL;

  pPlObj1  =  (CPolyLineObject*)GetFirst(NULL,-1);
  while (pPlObj1) {
//    if (pPlObj1->m_Konstruktion==KONSTRUKTION && pPlObj1->Art == Linie) {
    if ((pPlObj1->m_Konstruktion==UNDEF || pPlObj1->m_Konstruktion==KONTUR ) && pPlObj1->Art == Linie) {
      if (gpStlObject) gpStlObject ->GenerateGlBuffer();
      gpStlObject = new CStlObject(this,0,0,0,pPlObj1->m_pLayer->m_Name,1);
      if (gpStlObject) {
        if (CNCInfoCnt < AnzahlCNC2-1) {
          CNCInfo[CNCInfoCnt] = (CObject*) gpStlObject;
          CNCInfoCnt++;
          int W = pPlObj1->m_Xmax-pPlObj1->m_Xmin;
          int H = pPlObj1->m_Ymax-pPlObj1->m_Ymin;
          int T = pPlObj1->m_Zmax-pPlObj1->m_Zmin;

          if (abs(W) > abs(T)) s_dir = (abs (W) > abs (H))?1:-1;
          else                 s_dir = (abs (T) < abs (H))?1:-1;


          pPlObj1->m_Konstruktion=VERARBEITEDT;
          gpStlObject->m_Alpha = pPlObj1->m_Alpha;
          gpStlObject->m_dir = pPlObj1->m_dir;
          gpStlObject->m_Texture=pPlObj1->m_Texture;
          gpStlObject->m_TextureZoom=pPlObj1->m_TextureZoom;



          if (bShowNormale) gpStlObject->SetBufferSize(pPlObj1->m_Path.size()*5);
          else              gpStlObject->SetBufferSize(pPlObj1->m_Path.size()*3);
          pPlObj1 ->MakeStlProjection();
          pPlObj1->m_Konstruktion=VERARBEITEDT;
          gpStlObject ->GenerateGlBuffer();
          gpStlObject =NULL;
        }
      }
    }
    pPlObj1 = (CPolyLineObject*)GetNext();
  }
  if (gpStlObject) gpStlObject ->GenerateGlBuffer();
  gpStlObject=NULL;
}

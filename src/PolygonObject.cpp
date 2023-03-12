//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

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

extern float Zoom;
extern int gStopp;
int savebinary=1;
DWORD NumVertextoSave;

#ifndef WIN32
#define CALLBACK
#endif

//-------------------------------------------------------------
CPolygonObject::CPolygonObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color) {
  Art = SavePolygon;
  m_Paths.clear();
  m_nx=0;
  m_ny=0;
  m_nz=0;
  m_PathIndex=-1;
  m_ClosedLoop=0;
  m_SelectIx=-1;
  m_Projektion=0;
}
//-------------------------------------------------------------
CPolygonObject::~CPolygonObject () {
  m_Paths.clear();
}
//-------------------------------------------------------------
int CPolygonObject::GetNumItems() {
  int count =0;
  for (int i = 0; i < m_Paths.size(); ++i){
    count += m_Paths[i].size();
  }
  return count;
}
//-------------------------------------------------------------

void CPolygonObject::ExecuteDrawObject(int mode) {
  if (m_PathIndex >= 0 && m_Paths.size() > m_PathIndex) {
    if (m_Paths[0].size()<2) return;
    if (1||mode == Draw ) {
      int db=0;
#ifdef CTRLSHIFT
      if (Fl::event_state() & FL_SHIFT  && gDreiDFace==0) {
        db=1;
        if (m_Paths[0][0].Attr==0) {
          float xs = glXCNCToScreen(m_Paths[0][0].X);
          float ys = glYCNCToScreen(m_Paths[0][0].Y);
          float zs = glZCNCToScreen(m_S.Z);
          float w = m_Paths[0][1].x - m_Paths[0][0].X;
          float h = m_Paths[0][1].y - m_Paths[0][0].Y;
          float StartWinkel = atan2(h,w)*GRAD360;
          float rs = 0.01;

          float ydm  = cos((StartWinkel+90) / GRAD360)*rs;
          float xdm  = sin((StartWinkel+90) / GRAD360)*rs;
          float yds  = cos((StartWinkel) / GRAD360)*rs;
          float xds  = sin((StartWinkel) / GRAD360)*rs;
          //glColor4f (1,1, 1,1);
          glBegin (GL_LINE_STRIP);

          glVertex3f(xs+xds,ys-yds,zs);

          glVertex3f(xs+xdm,ys-ydm,zs);

          glVertex3f(xs-xds,ys+yds,zs);
          glEnd ();
        }
      }
#endif
      DWORD clr = GetRGBColor();
      glColor4f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);
      GLfloat mat_diffuse[4];
      mat_diffuse[0] = (clr>>0 & 0xFF) / 255.0 * LightColor[1];
      mat_diffuse[1] = (clr>>8 & 0xFF) / 255.0 * LightColor[1];
      mat_diffuse[2] = (clr>>16 & 0xFF)/ 255.0 * LightColor[1];
      mat_diffuse[3] = 1;
//      glUniform4f(uColor,mat_diffuse[0], mat_diffuse[1], mat_diffuse[2],gAlphaTransp);
//      glEnd();



     glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
    const GLfloat model_specular[4] = {0.6f, 0.6f, 0.6f, 1.0f};
    //// Set model material (used for perspective view, lighting enabled)
    glMaterialfv(GL_FRONT, GL_SPECULAR, model_specular);

        //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        //glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
//        glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);


      float x,y,z;
      int i;

      if (10) {
      if (!Is3DFace() || gAlphaTransp < 0.3) {
        // Konturen Zeigen
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        int s = m_Paths.size();
        for (int p = 0; p < s; ++p) {
          //glBegin(GL_LINE_STRIP);
          glBegin(GL_LINE_STRIP);
          int ps = m_Paths[p].size();
          for (i=0; i < ps; i++) {
            float x,y,z;
            y = glYCNCToScreen(m_Paths[p][i].Y);
            x = glXCNCToScreen(m_Paths[p][i].X);
            z = glZCNCToScreen(m_Paths[p][i].Z);
            glVertex3f(x,y,z);
          }
          glEnd();
        }
      }
      if (Is3DFace()) {
        mat_diffuse[3] = gAlphaTransp;

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
        glMaterialf(GL_FRONT, GL_SHININESS,LightColor[2]*20.0);
//        glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);

        if ((mode & Art)==Art) {
          SaveConvex();
        }
        if (m_Paths[0].size()>4) {
          DrawConvex();
        } else {
          int s = m_Paths.size();
          for (int p = 0; p < s; ++p) {
            glNormal3d(m_nx,m_ny,m_nz);
            glBegin(GL_POLYGON);
            int ps = m_Paths[p].size();
            for (i=0; i < ps; i++) {
              float x,y,z;
              y = glYCNCToScreen(m_Paths[p][i].Y);
              x = glXCNCToScreen(m_Paths[p][i].X);
              z = glZCNCToScreen(m_Paths[p][i].Z);
              glVertex3f(x,y,z);
            }
            glEnd();
          }
        }
      }
      }
      if (bShowNormale) { // Hilfe : zeige Normale
        float xx = glXCNCToScreen((m_Xmin + m_Xmax)/2.0);
        float yy = glYCNCToScreen((m_Ymin + m_Ymax)/2.0);
        float zz = glZCNCToScreen((m_Zmin + m_Zmax)/2.0);
        GLfloat mat_diffuse[4];
#ifdef VECTORTEST
        char Projektion = GetProjektion();
        mat_diffuse[0] = Projektion == 'A'?1.0:0.0;
        mat_diffuse[1] = Projektion == 'B'?1.0:0.0;
        mat_diffuse[2] = Projektion == 'C'?1.0:0.0;
        float xx2=xx;
        float yy2=yy;
        float zz2=zz;
#else
        mat_diffuse[0] = 0.4;
        mat_diffuse[1] = 1.0;
        mat_diffuse[2] = 0.4;
#endif
        mat_diffuse[3] = 0.9;//gAlphaTransp;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        glColor4f (mat_diffuse[0],mat_diffuse[1],mat_diffuse[2],mat_diffuse[3]);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex3f(xx,yy,zz);
        yy+= m_ny/10.0;
        xx+= m_nx/10.0;
        zz+= m_nz/10.0;
        // Zeiger
        glVertex3f(xx,yy,zz);
        glEnd();

#ifdef VECTORTEST
        glLineWidth(4);
        if (1) {
          glBegin(GL_LINES);
          glVertex3f(xx2,yy2,zz2);
          yy2+= m_ny2/60.0;
          xx2+= m_nx2/60.0;
          zz2+= m_nz2/60.0;
          // Zeiger
          glVertex3f(xx2,yy2,zz2);
          glEnd();
        }
#endif
      }
      if (IsSelected()) {
        for (i=0; i <= m_Paths[0].size(); i++) {
          int j = (i < m_Paths[0].size())?i:0; // Polygon schliessen
          ClipperLib::IntPoint p = m_Paths[0][j];

          float xs,ys,zs;
          glLineWidth(2);
          ys = glYCNCToScreen(p.Y);
          xs = glXCNCToScreen(p.X);
          zs = glZCNCToScreen(p.Z);
          float rs = 0.005;
          if (i==m_SelectIx) {
            glColor4f (1,1, 0.5,1);
            //Color4f (0.7,0, 0.7,0.5);
            glBegin (GL_LINES);
            glVertex3f(xs+rs,ys-rs,zs);
            glVertex3f(xs-rs,ys+rs,zs);
            glEnd ();
            glBegin (GL_LINES);
            glVertex3f(xs+rs,ys+rs,zs);
            glVertex3f(xs-rs,ys-rs,zs);
            glEnd ();
          } else glColor4f (1,1, 1,0.5);
          {
            //glBegin (GL_QUADS);

            zs-=0.001;
            glBegin (GL_LINE_LOOP );
            glVertex3f(xs-rs,ys+rs,zs);
            glVertex3f(xs+rs,ys+rs,zs);
            glVertex3f(xs+rs,ys-rs,zs);
            glVertex3f(xs-rs,ys-rs,zs);
            glEnd ();
          }
        }
      }


  } else if ((mode & Art)==Art) {
  } else  return;
  }
}

//-------------------------------------------------------------
void CPolygonObject::CalculateNormale(int rechts){
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
    if (normalX + normalY + normalZ) {
      float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      if (rechts==0) isqr *= -1.0;
      m_nx = normalX * isqr;
      m_ny = normalY * isqr;
      m_nz = normalZ * isqr;
    }
  }
  if (m_Xmin < gObjectManager->MinMaxleft)   gObjectManager->MinMaxleft  = m_Xmin;
  if (m_Xmax > gObjectManager->MinMaxright)  gObjectManager->MinMaxright = m_Xmax;
  if (m_Ymin < gObjectManager->MinMaxtop)    gObjectManager->MinMaxtop   = m_Ymin ;
  if (m_Ymax > gObjectManager->MinMaxbottom) gObjectManager->MinMaxbottom= m_Ymax;
  if (m_Zmin< gObjectManager->MinMaxfront)   gObjectManager->MinMaxfront = m_Zmin;
  if (m_Zmax > gObjectManager->MinMaxback)   gObjectManager->MinMaxback  = m_Zmax;
}


//------------------------------------------------------------------------------
//-------------------------------------------------------------
typedef std::vector< GLdouble* > tPolygonVectors;
tPolygonVectors PolygonVectors;

//------------------------------------------------------------------------------
// heap memory management for GLUtesselator ...
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static GLdouble* NewPolygonVector(GLdouble x, GLdouble y,GLdouble z)
{
  GLdouble *vert = new GLdouble[3];
  vert[0] = x;
  vert[1] = y;
  vert[2] = z;
  PolygonVectors.push_back(vert);
  return vert;
}
//------------------------------------------------------------------------------

static void ClearPolygonVectors()
{
  for (tPolygonVectors::size_type i = 0; i < PolygonVectors.size(); ++i)
    delete[] PolygonVectors[i];
  PolygonVectors.clear();
}


//------------------------------------------------------------------------------
int CPolygonObject::AddPath(int was){
   m_PathIndex++;
   m_Paths.resize(m_PathIndex+1);
   int s = m_Paths.size();
   return s;
}

//-------------------------------------------------------------
void CPolygonObject::AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr) {
  ClipperLib::IntPoint p;
  if (bYUpp) {
    p.X = x;
    p.Y = -z;
    p.Z = y;
  } else {
    p.X = x;
    p.Y = y;
    p.Z = z;
  }
  p.Attr=Attr;
  if (m_PathIndex >= 0 && m_Paths.size() > m_PathIndex) {
  m_Paths[m_PathIndex].push_back(p);

  if      (p.X < m_Xmin)  m_Xmin = p.X;
  else if (p.X > m_Xmax)  m_Xmax = p.X;
  if      (p.Y < m_Ymin)  m_Ymin = p.Y;
  else if (p.Y > m_Ymax)  m_Ymax = p.Y;
  if      (p.Z < m_Zmin)  m_Zmin = p.Z;
  else if (p.Z > m_Zmax)  m_Zmax = p.Z;
  }
}


//------------------------------------------------------------------------------
// GLUtesselator callback functions ...
//------------------------------------------------------------------------------
//void CALLBACK BeginCallback(GLenum type)
float nx,ny,nz;
void CALLBACK BeginPolygonCB(GLenum type)
{
    glBegin(type);
    glNormal3f(nx,ny,nz);
}
//------------------------------------------------------------------------------
void CALLBACK EndPolygonCB() {
    glEnd();
}
//------------------------------------------------------------------------------

void CALLBACK VertexPolygonCB(GLvoid *vertex) {
  const GLdouble  *vals = (GLdouble  * ) vertex;
  glVertex3d(vals[0],vals[1],vals[2]);
}
//------------------------------------------------------------------------------
void CALLBACK CombinePolygonCB(GLdouble coords[3], GLdouble*[4], GLfloat[4], GLdouble **dataOut )
{
  GLdouble *vert = NewPolygonVector(coords[0], coords[1],coords[2]);
  *dataOut = vert;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Jede STL-Datei in ASCII-Code ist folgendermaßen aufgebaut:

//solid name
// facet normal n1 n2 n3
//  outer loop
//   vertex p1x p1y p1z
//   vertex p2x p2y p2z
//   vertex p3x p3y p3z
//  endloop
// endfacet
//endsolid name
//name steht für den Dateinamen,
//der Block von facet bis endfacet steht für ein Dreieck und wird entsprechend der Anzahl an Dreiecken wiederholt.
//ni gibt den Normalenvektor des Dreiecks an,
//p1j bis p3j die x-, y- und z-Koordinate der Eckpunkte des Dreiecks.

static float m1;
static float m2;
static float m3;

GLdouble stlx1,stly1,stlz1;
GLdouble stlx2,stly2,stlz2;
GLdouble stlx3,stly3,stlz3;
GLdouble stlx4,stly4,stlz4;
WORD StlRgb;
static int modus=0;
static int count=0;

//-------------------------------------------------------------
void SaveOneVertex(){
  if (savebinary) {
    //if (stly2==0) {
    //  stly2=0;
    //} else
    {
      struct tbuff {
        float Normale[3];
        float Vertex1[3];
        float Vertex2[3];
        float Vertex3[3];
        WORD  Attribute;
      }buff;

      buff.Normale[0] = m1;
      buff.Normale[1] = m2;
      buff.Normale[2] = m3;
      buff.Vertex1[0]  = stlx1/fxAufloesung;
      buff.Vertex1[1]  = stly1/fxAufloesung;
      buff.Vertex1[2]  = stlz1/fxAufloesung;
      buff.Vertex2[0]  = stlx2/fxAufloesung;
      buff.Vertex2[1]  = stly2/fxAufloesung;
      buff.Vertex2[2]  = stlz2/fxAufloesung;
      buff.Vertex3[0]  = stlx3/fxAufloesung;
      buff.Vertex3[1]  = stly3/fxAufloesung;
      buff.Vertex3[2]  = stlz3/fxAufloesung;
      buff.Attribute   = StlRgb;
      int s = sizeof(buff);
      if (s==52) {
        fwrite((const void *)&buff,1,50,gWrk3DSheet->m_file);
        NumVertextoSave++;
      }
      }
  } else {
    if (stly2==0) {
      stly2=0;
    } else {
      fprintf(gWrk3DSheet->m_file,
      " facet normal %d %d %d\n" \
      "  outer loop\n" \
      "   vertex %0.3f %0.3f %0.3f\n" \
      "   vertex %0.3f %0.3f %0.3f\n" \
      "   vertex %0.3f %0.3f %0.3f\n" \
      "  endloop\n" \
      " endfacet\n",(int)m1,(int)m2,-(int)m3,
        stlx1/fxAufloesung,stly1/fxAufloesung,stlz1/fxAufloesung,
        stlx2/fxAufloesung,stly2/fxAufloesung,stlz2/fxAufloesung,
        stlx3/fxAufloesung,stly3/fxAufloesung,stlz3/fxAufloesung);
      }
  }
}

void PushStlValues(GLdouble X,GLdouble Y,GLdouble Z){
//    X+= gObjectManager->MinMaxleft;
//    Y+= gObjectManager->MinMaxtop;
    if (count==0) {
      stlx1=X;stly1=Y;stlz1=Z;
      count=1;
    } else if (count==1) {
      stlx2=X;stly2=Y;stlz2=Z;
      count=2;
    } else if (count==2) {
      if (modus == GL_TRIANGLES) {
        stlx3=X;stly3=Y;stlz3=Z;
        count=3;
        SaveOneVertex();
      } else if (modus == GL_TRIANGLE_STRIP) {
        stlx3=X    ;stly3=Y    ;stlz3=Z;
        count=2;
        SaveOneVertex();
        stlx1=stlx2;stly1=stly2;stlz1=stlz2;
        stlx2=stlx3;stly2=stly3;stlz2=stlz3;
      } else if (modus == GL_TRIANGLE_FAN ) {
        stlx3=X    ;stly3=Y    ;stlz3=Z;
        count=2;
        SaveOneVertex();
        stlx2=stlx3;stly2=stly3;stlz2=stlz3;
      } else { //if (modus == GL_QUADS) {
        stlx3=X    ;stly3=Y    ;stlz3=Z;
        count=3;
      }
    } else { //if (count==3) {
      count++;
    }


}

void CALLBACK BeginStlPolyVertexCB(GLenum type)
{
  modus =type;
  count = 0;
}

//------------------------------------------------------------------------------
void CALLBACK EndStlPolyVertexCB() {
    //glEnd();
  count = 0;
}

//------------------------------------------------------------------------------

void CALLBACK VertexStlPolyVertexCBA(GLvoid *vertex) {
  const GLdouble  *vals = (GLdouble  * ) vertex;
  PushStlValues(vals[0],vals[1],vals[2]);
}
//------------------------------------------------------------------------------

void CALLBACK VertexStlPolyVertexCBB(GLvoid *vertex) {
  const GLdouble  *vals = (GLdouble  * ) vertex;
  PushStlValues(vals[2],vals[1],vals[0]);
}
//------------------------------------------------------------------------------

void CALLBACK VertexStlPolyVertexCBC(GLvoid *vertex) {
  const GLdouble  *vals = (GLdouble  * ) vertex;
  PushStlValues(vals[1],vals[2],vals[0]);
}
//------------------------------------------------------------------------------


void CPolygonObject::SaveConvex(void) {
  GLUtesselator* tess = gluNewTess();
  gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())&BeginStlPolyVertexCB);
  switch (m_Projektion) {
  case 'B':
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBB);
    gluTessNormal(tess, m1=m_nz,m2=m_ny,m3=m_nx);
    break;
  case 'C':
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBC);
    gluTessNormal(tess, m1=m_nz,m2=m_nx,m3=m_ny);
    break;

  case 'A':
  default:
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBA);
    gluTessNormal(tess, m1=m_nx,m2=m_ny,m3=m_nz);
    break;
  }
  m1=m_nx,m2=m_ny,m3=m_nz;
  unsigned int clr = GetRGBColor();
/*  unsigned int r = (clr>>3 & 0x1F);
  unsigned int g = (clr>>11 & 0x1F);
  unsigned int b = (clr>>19 & 0x1F);

  StlRgb = b << 10 |g << 5 | r << 0 | 0x8000;
*/
  StlRgb = ((clr & 0xF80000) >> 9) |((clr & 0xF800) >> 6) | ((clr & 0xF8) >> 3) | 0x8000;

  gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())&EndStlPolyVertexCB);
  gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)())&CombinePolygonCB);

  // gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD); // zweites Poly ausschneiden

  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessBeginPolygon(tess, NULL);

  for (int p = 0; p < m_Paths.size(); ++p) {
    gluTessBeginContour(tess);
    for (int i=0; i < m_Paths[p].size(); i++) {
      float x,y,z;
      y = m_Paths[p][i].Y;
      x = m_Paths[p][i].X;
      z = m_Paths[p][i].Z;
      GLdouble *vert;
      if (m_Projektion=='B') {
        vert = NewPolygonVector(z,y,x);
      } else if (m_Projektion=='C') {
        vert = NewPolygonVector(z,x,y);
      } else {
        vert = NewPolygonVector(x,y,z);
      }
      gluTessVertex(tess, vert, vert);
    }
    gluTessEndContour(tess);
  }
  gluTessEndPolygon(tess);
  //final cleanup ...
  gluDeleteTess(tess);
  ClearPolygonVectors();
}
//------------------------------------------------------------------------------
void CPolyLineObject::SaveConvex(void) {
  GLUtesselator* tess = gluNewTess();
  gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())&BeginStlPolyVertexCB);
  switch (m_Projektion) {
  case 'B':
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBB);
    gluTessNormal(tess, m1=m_nz,m2=m_ny,m3=m_nx);
    break;
  case 'C':
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBC);
    gluTessNormal(tess, m1=m_nz,m2=m_nx,m3=m_ny);
    break;

  case 'A':
  default:
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())&VertexStlPolyVertexCBA);
    gluTessNormal(tess, m1=m_nx,m2=m_ny,m3=m_nz);
    break;
  }
  m1=m_nx,m2=m_ny,m3=m_nz;
  unsigned int clr = GetRGBColor();
/*  unsigned int r = (clr>>3 & 0x1F);
  unsigned int g = (clr>>11 & 0x1F);
  unsigned int b = (clr>>19 & 0x1F);

  StlRgb = b << 10 |g << 5 | r << 0 | 0x8000;
*/
  StlRgb = ((clr & 0xF80000) >> 9) |((clr & 0xF800) >> 6) | ((clr & 0xF8) >> 3) | 0x8000;

  gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())&EndStlPolyVertexCB);
  gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)())&CombinePolygonCB);

  // gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD); // zweites Poly ausschneiden

  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessBeginPolygon(tess, NULL);

    gluTessBeginContour(tess);
    for (int i=0; i < m_Path.size(); i++) {
      float x,y,z;
      y = m_Path[i].Y;
      x = m_Path[i].X;
      z = m_Path[i].Z;
      GLdouble *vert;
      if (m_Projektion=='B') {
        vert = NewPolygonVector(z,y,x);
      } else if (m_Projektion=='C') {
        vert = NewPolygonVector(z,x,y);
      } else {
        vert = NewPolygonVector(x,y,z);
      }
      gluTessVertex(tess, vert, vert);
    }
    gluTessEndContour(tess);
  gluTessEndPolygon(tess);
  //final cleanup ...
  gluDeleteTess(tess);
  ClearPolygonVectors();
}
//------------------------------------------------------------------------------

void CPolygonObject::DrawConvex(void) {
  GLUtesselator* tess = gluNewTess();
  gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())&BeginPolygonCB);
  gluTessCallback(tess, GLU_TESS_VERTEX,  (void (CALLBACK*)())&VertexPolygonCB);
  gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())&EndPolygonCB);
  gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)())&CombinePolygonCB);

  // gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD); // zweites Poly ausschneiden

  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessBeginPolygon(tess, NULL);
  gluTessNormal(tess, m_nx,m_ny,m_nz);
  nx=m_nx;
  ny=m_ny;
  nz=m_nz;

  for (int p = 0; p < m_Paths.size(); ++p) {
    gluTessBeginContour(tess);
    for (int i=0; i < m_Paths[p].size(); i++) {
      float x,y,z;
      y = glYCNCToScreen(m_Paths[p][i].Y);
      x = glXCNCToScreen(m_Paths[p][i].X);
      z = glZCNCToScreen(m_Paths[p][i].Z);
      GLdouble *vert;
        vert = NewPolygonVector(x,y,z);
      gluTessVertex(tess, vert, vert);
    }
    gluTessEndContour(tess);
  }
  gluTessEndPolygon(tess);
  //final cleanup ...
  gluDeleteTess(tess);
  ClearPolygonVectors();

}
//-------------------------------------------------------------

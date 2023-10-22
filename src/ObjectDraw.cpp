//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

// Eintrag in .profile   :
// export MESA_GL_VERSION_OVERRIDE=3.3
// etc/groups :  tty freigeben
// Starten:
// MESA_GL_VERSION_OVERRIDE=3.3 ./dxfcnc

int     gbShowTransformation = 0;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
//#define _USE_MATH_DEFINES
#include <math.h>
//#define M_PI		3.14159265358979323846
#if defined(WIN32)
# define GLEW_STATIC 1
#include <GL/glew.h>
#else
//#include "glew.h"
#include <GL/glew.h>
#endif
#include "MatrixTransform.h"
//#include "DxfCnc.h"
#include "Object.h"
//#include "Draw.h"
//#include "Tree.h"
//#include "Resource.h"
//#include "Serialcom.h"
#include "Settings.h"
#include "GlDisplay.h"
#include "clipper.h"

class SaveConvex;
extern float Zoom;
extern int gStopp;
static int LastAttr=0;
DWORD glFarbe[9] =
{
  0x7f7f7f,
  0x0000FF,
  0x00FFFF,
  0x00FF00,
  0xFFFF00,
  0xFF0000,
  0xFF00FF,
  0x3F3F3F,
  0xEFEFEF
};

float XOffset = 0.0;
float YOffset = 0.0;
float ZOffset = 0.0;
float glFaktor  = 1.0/100.0;
float gGrayOut=0.60;
float gAlphaTransp=1;
int   gDreiDFace=0;

//-------------------------------------------------------------
float glXScreenToCNC(float x)
{
  return ((x/glFaktor) + XOffset);
}
//-------------------------------------------------------------
float glYScreenToCNC(float y)
{
  return ((y/glFaktor) + YOffset);
}
//-------------------------------------------------------------
float glXCNCToScreen(float x)
{
  return ((x - XOffset) * glFaktor);// - m_XOffset ;
}
//-------------------------------------------------------------
float glYCNCToScreen(float y)
{
  return ((y-YOffset) * glFaktor);
}
//-------------------------------------------------------------
float glZCNCToScreen(float z)
{
  return ((z - ZOffset)* glFaktor);
}
//-------------------------------------------------------------
float glCNCToScreen(float c)
{
  return (c * glFaktor);
}

//-------------------------------------------------------------
static int fghCircleTable(double start, double end,double **sint,double **cost,const int size)
{
    int i;
    // Determine the angle between samples
    const double angle = 2*M_PI/(double)( ( size == 0 ) ? 1 : size);

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    if (!(*sint) || !(*cost)) {
      if (*sint) free(*sint);
      if (*cost) free(*cost);
      *sint = NULL;
      *cost = NULL;
      return 0;
    }

    // Compute cos and sin around the circle

    double w = start;
    int dir = start < end;
    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;
    for (i=0; i<size; i++) {
        if (dir) {
          if (w >= end) {
            (*sint)[i] = sin(end);
            (*cost)[i] = cos(end);
            return i;
          }
          (*sint)[i] = sin(w);
          (*cost)[i] = cos(w);
          w += angle;
        } else {
          if (w <= end) {
            (*sint)[i] = sin(end);
            (*cost)[i] = cos(end);
            return i;
          }
          (*sint)[i] = sin(w);
          (*cost)[i] = cos(w);
          w -= angle;
        }
    }

    // Last sample is duplicate of the first
    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
    return size;
}

//-------------------------------------------------------------
CBogenObject::CBogenObject(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color) {
  Art = Kreisbogen;
  m_Radius = 0;
  m_StartWinkel= 0;
  m_EndWinkel= 0;
}

//-------------------------------------------------------------
CBogenObject::~CBogenObject() {

}

void CBogenObject::ExecuteDrawObject(int mode) {
  float XX,YY;
  if (m_Radius > 5)
  {
    int slices = m_Radius / 10;
    if (slices  < 7) slices = 7;
    double *sint=NULL,*cost=NULL;
    int n = fghCircleTable(m_StartWinkel / GRAD360,m_EndWinkel/ GRAD360,&sint,&cost,slices);
    if ((mode & Art)==Art) {
      ETiefe &= 0xFF;
      XX = m_S.X + (cost[0]*m_Radius);
      YY = m_S.Y + (sint[0]*m_Radius);
      if (AbhebenwennNoetig(XX,YY)==0) return;
      PositionierWarteBis(XX,YY,gPa.Tiefe[0],(int)gPa.Tempo[Tempo_Eintauch]);
    } else {
      glBegin(GL_LINE_STRIP);
    }
    for (int i=0;i <= n;i++){
      XX = m_S.X + (cost[i]*m_Radius);
      YY = m_S.Y + (sint[i]*m_Radius);
      if ((mode & Art)==Art) {
        //  XY Fahren
        PositionierWarteBis(XX,YY,gPa.Tiefe[ETiefe],(int)gPa.Tempo[ETiefe]);
      } else {
        float x,y,z;
        x = glXCNCToScreen(XX);
        y = glYCNCToScreen(YY);
        z = glZCNCToScreen(m_S.Z);//Pa.Tiefe[ETiefe]);
        glVertex3f(x,y,z);
      }
    }
    if ((mode & Art)==Art) {
     if (!gStopp) SetDone(1);
   } else {
      glEnd();
    }
    if (sint) free(sint);
    if (cost) free(cost);
  }
  {
    float I = (cos(m_StartWinkel / GRAD360)*m_Radius);
    float J = (sin(m_StartWinkel / GRAD360)*m_Radius);
    float X2 =(cos(m_EndWinkel / GRAD360)*m_Radius);
    float Y2 =(sin(m_EndWinkel / GRAD360)*m_Radius);
    float rs =0.01;

#ifdef CTRLSHIFT
    if (Fl::event_state() & FL_SHIFT && gDreiDFace==0){
      float xs = glXCNCToScreen(m_S.X + I);
      float ys = glYCNCToScreen(m_S.Y + J);
      float zs = glZCNCToScreen(m_S.Z);

      float richtung = m_StartWinkel< m_EndWinkel?180:0;
      float ydm  = cos((m_StartWinkel+richtung) / GRAD360)*rs;
      float xdm  = sin((m_StartWinkel+richtung) / GRAD360)*rs;
      float yds  = cos((m_StartWinkel+90) / GRAD360)*rs;
      float xds  = sin((m_StartWinkel+90) / GRAD360)*rs;

      glColor4f (1,1, 1,1);
      glBegin (GL_LINE_STRIP);
      glVertex3f(xs+xds,ys-yds,zs);
      glVertex3f(xs+xdm,ys-ydm,zs);
      glVertex3f(xs-xds,ys+yds,zs);
      glEnd ();

      xs = glXCNCToScreen(m_S.X + X2);
      ys = glYCNCToScreen(m_S.Y + Y2);

      glColor4f (0.7,1, 1,1);
      glBegin (GL_LINES);
      glVertex3f(xs,ys-rs/2,zs);
      glVertex3f(xs,ys+rs/2,zs);
      glVertex3f(xs+rs/2,ys,zs);
      glVertex3f(xs-rs/2,ys,zs);
      glEnd ();
    }
#endif
  }


};

//-------------------------------------------------------------
CDreiDFaceObject::CDreiDFaceObject(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color) {
  Art = DreiDFace;
  m_nx =0;
  m_ny =0;
  m_nz =1;
  m_points=4;
//  if (gDreiDFace==0) {
//    gDreiDFace=1;
//    gPerspektive->CheckDlgButton(IDC_Show3DFace,gDreiDFace);
//    gWrk3DSheet->WindowProc(WM_COMMAND,IDC_GetDefault,0);
//  }
}

//-------------------------------------------------------------
CDreiDFaceObject::~CDreiDFaceObject() {
}
//-------------------------------------------------------------
void CDreiDFaceObject::CalculateNormale(float mpx,float mpy,float mpz){
  /*float p1[3];
  float p2[3];
  float rgNormal[3];
  p1[0] = m_3DFace[1].X - m_3DFace[2].X;
  p2[0] = m_3DFace[2].X - m_3DFace[3].X;
  p1[1] = m_3DFace[1].Y - m_3DFace[2].Y;
  p2[1] = m_3DFace[2].Y - m_3DFace[3].Y;
  p1[2] = m_3DFace[1].Z - m_3DFace[2].Z;
  p2[2] = m_3DFace[2].Z - m_3DFace[3].Z;
  cross3(&rgNormal[0],&p1[0],&p2[0]);
  normalize(&rgNormal[0],&rgNormal[1],&rgNormal[2]);
  m_nx = rgNormal[0];
  m_ny = rgNormal[1];
  m_nz = rgNormal[2];
  */
  if (1) {
    float normalX =  0;
    float normalY =  0;
    float normalZ =  0;
    float   w1 = m_3DFace[2].X - m_3DFace[1].X;
    float   h1 = m_3DFace[2].Y - m_3DFace[1].Y;
    float   t1 = m_3DFace[2].Z - m_3DFace[1].Z;
    float   w2 = m_3DFace[1].X - m_3DFace[0].X;
    float   h2 = m_3DFace[1].Y - m_3DFace[0].Y;
    float   t2 = m_3DFace[1].Z - m_3DFace[0].Z;
    normalX +=  (h1) * (t2) - (h2) * (t1);
    normalY +=  (t1) * (w2) - (t2) * (w1);
    normalZ +=  (w1) * (h2) - (w2) * (h1);

    if (normalX || normalY || normalZ) {
      float isqr = 1.0f /  sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      m_nx = normalX * isqr;
      m_ny = normalY * isqr;
      m_nz = normalZ * isqr;
    }
  }
}
//-------------------------------------------------------------

int CDreiDFaceObject::Is3DFace(){
  return (m_Select||gnumSelect==0) & gDreiDFace;
}

void CDreiDFaceObject::ExecuteDrawObject(int mode) {
  if ((mode & Art)==Art) {
  } else {
    DWORD clr = GetRGBColor();
#ifdef USE_SHADER
    //if (Is3DFace()==0) {
    glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
    glUniform4f(uFixColor,(clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0,1.0);
    glBegin(GL_LINE_LOOP);
    //}else {
    for (int i=0;i < m_points ;i++){
      if ((mode & Art)==Art) {
      } else {
        float x,y,z;
        x = glXCNCToScreen(m_3DFace[i].X);
        y = glYCNCToScreen(m_3DFace[i].Y);
        z = glZCNCToScreen(m_3DFace[i].Z);
        glVertex3f(x,y,z);
      }
    }
    glEnd();


#else
    if (Is3DFace()) {
      GLfloat mat_diffuse[4];
      mat_diffuse[0] = (clr>>0 & 0xFF) / 255.0;
      mat_diffuse[1] = (clr>>8 & 0xFF) / 255.0;
      mat_diffuse[2] = (clr>>16 & 0xFF)/ 255.0;
      mat_diffuse[3] = gAlphaTransp;

      //glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
      glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
      glBegin(GL_POLYGON);
      glNormal3d(m_nx,m_ny,m_nz);
    }
#endif
  }
  int i;
  for (i=0;i < m_points ;i++){
    if ((mode & Art)==Art) {
    } else {
      float x,y,z;
      x = glXCNCToScreen(m_3DFace[i].X);
      y = glYCNCToScreen(m_3DFace[i].Y);
      z = glZCNCToScreen(m_3DFace[i].Z);


      glVertex3f(x,y,z);
    }
  }
  if ((mode & Art)==Art) {
  } else {
    glEnd();
  }

#ifdef USE_SHADER

  vector3 lVUnterlage[8];
  vector3 lNUnterlage[8];
  vector3 lCUnterlage[8];

    DWORD clr = GetRGBColor();

    float r = (clr>>0 & 0xFF) / 255.0;
    float g = (clr>>8 & 0xFF) / 255.0;
    float b = (clr>>16 & 0xFF) / 255.0;
  for (i=0;i < m_points ;i++){
      float x,y,z;
      x = glXCNCToScreen(m_3DFace[i].X);
      y = glYCNCToScreen(m_3DFace[i].Y);
      z = glZCNCToScreen(m_3DFace[i].Z);

      lVUnterlage[i].Vertex[0] = x;
      lVUnterlage[i].Vertex[1] = y;
      lVUnterlage[i].Vertex[2] = z;
      lNUnterlage[i].Vertex[0] = m_nx;
      lNUnterlage[i].Vertex[1] = m_ny;
      lNUnterlage[i].Vertex[2] = m_nz;
      lCUnterlage[i].Vertex[0] = r;
      lCUnterlage[i].Vertex[1] = g;
      lCUnterlage[i].Vertex[2] = b;
  }
  lVUnterlage[4]= lVUnterlage[0];
  lVUnterlage[5]= lVUnterlage[2];
  lNUnterlage[4]= lNUnterlage[0];
  lNUnterlage[5]= lNUnterlage[2];
  lCUnterlage[4]= lCUnterlage[0];
  lCUnterlage[5]= lCUnterlage[2];

extern GLuint vertexbuffer;
extern GLuint colorbuffer;
extern GLuint normalbuffer;

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(lNUnterlage), lVUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

  glEnableVertexAttribArray(NormalAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(lNUnterlage), lNUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

  glEnableVertexAttribArray(ColorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(lCUnterlage), lCUnterlage, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  //GLbyte rgCube[24]= {
  //  0,1,2,3,
  //  4,5,6,7,
  //  0,3,7,4,
  //  1,2,6,5,
  //  0,1,5,4,
  //  2,3,7,6
  //};

  glUniform4f(uFixColor,0,0,0,0);
  glUniform4f(uColor,0.4, 0.4, 0.4,0.7);
  glDrawArrays(GL_TRIANGLES,0,6);
  //glDrawElements(GL_QUADS,24,GL_UNSIGNED_BYTE,rgCube);


  glDisableVertexAttribArray(positionAttribute);
  glDisableVertexAttribArray(NormalAttribute);
  glDisableVertexAttribArray(ColorAttribute);
#endif

}

//------------------------------------------

void CDreiDFaceObject::SaveStlObject(void){
#pragma pack ( push,1)
    struct tbuff {
      float Normale[3];
      float Vertex1[3];
      float Vertex2[3];
      float Vertex3[3];
      WORD  Attribute;
    }buff;
#pragma pack( pop)
  vector3 lVUnterlage[8];
  WORD StlRgb;
  DWORD clr = GetRGBColor();
  StlRgb = ((clr & 0xF80000) >> 9) |((clr & 0xF800) >> 6) | ((clr & 0xF8) >> 3) | 0x8000;
  for (int i=0;i < m_points ;i++){
      float x,y,z;
      x = glXCNCToScreen(m_3DFace[i].X);
      y = glYCNCToScreen(m_3DFace[i].Y);
      z = glZCNCToScreen(m_3DFace[i].Z);
      lVUnterlage[i].Vertex[0] = x;
      lVUnterlage[i].Vertex[1] = y;
      lVUnterlage[i].Vertex[2] = z;
  }
  lVUnterlage[4]= lVUnterlage[0];
  lVUnterlage[5]= lVUnterlage[2];


  for (int ix=0;ix<2;ix++) {
    buff.Normale[0] = m_nx;
    buff.Normale[1] = m_ny;
    buff.Normale[2] = m_nz;
    buff.Vertex1[0] = lVUnterlage[ix*3+0].Vertex[0];
    buff.Vertex1[1] = lVUnterlage[ix*3+0].Vertex[1];
    buff.Vertex1[2] = lVUnterlage[ix*3+0].Vertex[2];
    buff.Vertex2[0] = lVUnterlage[ix*3+1].Vertex[0];
    buff.Vertex2[1] = lVUnterlage[ix*3+1].Vertex[1];
    buff.Vertex2[2] = lVUnterlage[ix*3+1].Vertex[2];
    buff.Vertex3[0] = lVUnterlage[ix*3+2].Vertex[0];
    buff.Vertex3[1] = lVUnterlage[ix*3+2].Vertex[1];
    buff.Vertex3[2] = lVUnterlage[ix*3+2].Vertex[2];
    buff.Attribute  = StlRgb;
    int s = sizeof(buff);
    if (s==50) {
      fwrite((const void *)&buff,1,50,gWrk3DSheet->m_file);
      NumVertextoSave++;
    }
  }
}


//-------------------------------------------------------------
CTextObject::CTextObject(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color) {
  m_Ps = NULL;
  Art = Buchstaben;
}

//-------------------------------------------------------------
CTextObject::~CTextObject() {
  if (m_Ps) free (m_Ps);

}
//-------------------------------------------------------------
void CTextObject::ExecuteDrawObject(int mode) {
  float Drehung  =  (float)(m_Winkel * M_PI / 180.0);
  ClipperLib::cInt BX = m_S.X;
  ClipperLib::cInt BY = m_S.Y;
  int Down=0;
  if (m_Ps == NULL) return;

  if ((mode & Art)==Art) {
    if ((mode & LaserGravur)==0) {
      if (AbhebenwennNoetig(BX,BY)==0) return;
    //    GCodeMode=gOnline;
    } else {
      SpindleWarteBis(0);
    }
  } else {
  }
  // 100 = 1 mm Char 10mm = 100
  int l = strlen(m_Ps);
  for (int i=0; i < l; i++)
  {
    unsigned char ch = m_Ps[i];
    switch (ch)
    {
    case 228:
    case 142:
    case 132: // ä
      ch = 127;
      break;	//* Umlate nicht sortiert
    case 148:
    case 246:
    case 153: // ö
      ch = 128;
      break;
    case 129:
    case 252:
    case 154: // ü
      ch = 129;
      break;
    case 227:
      ch = 130;
      break;
    case 231:
    case 135: // ç
      ch = 131;
      break;
    case 243:
    case 162: // ó
      ch = 132;
      break;
    }
    if (ch >= ErsterBuchstabe && ch <= LetzterBuchstabe)
    {
      ClipperLib::cInt XX = 0;
      ClipperLib::cInt YY = 0;
      ClipperLib::cInt LX = 0;
      ClipperLib::cInt LY = 0;
      ClipperLib::cInt LZ = 0;

      ch -= (char)ErsterBuchstabe;
      int l = Korrektur[ch].Si / 2;
      bvector * Bustr = Zeichen[ch];
      ETiefe &= 0xFF;
      if (Bustr!= NULL) {
        for (int i=0; i<l; i++)
        {
          if  (Bustr[i].x == 0 && Bustr[i].y == 0) {
            // (StartPosition)
            if ((mode & Art)==Art) {
              LX = (BX+XX);
              LY = (BY+YY);
              LZ = m_S.Z+gPa.Tiefe[0];
              if ((mode & LaserGravur)==0) {
                PositionierWarteBis(LX,LY,LZ,(int)gPa.Tempo[Tempo_Eintauch]);
              } else {
                SpindleWarteBis(0);
                PositionierWarteBis(LX,LY,m_S.Z,(int)gPa.Tempo[Tempo_Eintauch]);
              }
            } else {
              if (Down) glEnd();
            }
            Down = 0;
          } else {
            XX = (int)((float)(Bustr[i].x-Korrektur[ch].Ofs) * m_Height /100.0);
            YY = (int)((float)(Bustr[i].y-100.0) * m_Height /100.0);

            ClipperLib::cInt X2 = XX;
            XX = (int)(XX *cos(Drehung) -YY *sin(Drehung));
            YY = (int)(YY *cos(Drehung) +X2 *sin(Drehung));
            if ((mode & Art)==Art) {
              LX = (BX+XX);
              LY = (BY+YY);
              //  XY Fahren
              if ((mode & LaserGravur)==0) {
                if (Down==0) {
                  LZ = gPa.Tiefe[0];
                  PositionierWarteBis(LX,LY,LZ,(int)gPa.Tempo[ETiefe]);
                }
                LZ = m_S.Z+gPa.Tiefe[(ETiefe < 7)?ETiefe:0];
                PositionierWarteBis(LX,LY,LZ,(int)gPa.Tempo[ETiefe]);
              } else {
                if (Down==0) {
                  PositionierWarteBis(LX,LY,m_S.Z,(int)gPa.Tempo[0]);
                }
                SpindleWarteBis(gPa.Maschine[eLaserPower]);
                PositionierWarteBis(LX,LY,m_S.Z,gPa.Maschine[eLaserSpeed]);
              }
            } else {
              float x,y,z;
              if (Down==0) glBegin(GL_LINE_STRIP);
              x = glXCNCToScreen(BX+XX);
              y = glYCNCToScreen(BY+YY);
              z = glZCNCToScreen(m_S.Z/*+Pa.Tiefe[(ETiefe < 7)?ETiefe:0]*/ );
              glVertex3f(x,y,z);
            }
            Down = 1;
            LX = BX+XX;
            LY = BY+YY;
            LZ = m_S.Z+gPa.Tiefe[(ETiefe < 7)?ETiefe:0];
          }
        }
        if ((mode & Art)==Art) {
          // Z Heben
          if ((mode & LaserGravur)==0) {
            PositionierWarteBis(LX,LY,LZ = gPa.Tiefe[0],(int)gPa.Tempo[Tempo_Bohren]);
          } else {
            SpindleWarteBis(0);
          }
        } else {
          if (Down) glEnd();

        }
        Down = 0;
        float space = ((float)(Korrektur[ch].Br + 32 ) * m_Height /100.0f);
        BX = BX+ (int)(space *cos(Drehung));
        BY = BY+ (int)(space *sin(Drehung));
      } else {
        float space = ((float)(Korrektur[ch].Br + 32 ) * m_Height /100.0f);
        BX = BX+ (int)(space *cos(Drehung));
        BY = BY+ (int)(space *sin(Drehung));
      }

      if ((mode & Art)==Art) {
        if ((mode & LaserGravur)==1) {
          SpindleWarteBis(0);
        }
        if (gStopp) {
          SendToCNC('\033');// SoftSTOP);
          //CNCAktiv = false;
          goto Exit;
        }
      } else {
        if (Down) glEnd();
        Down = 0;
      }
    }
Exit:
    ;
  }
  if ((mode & Art)==Art) {
    if (! gStopp && gOkCount>0) {
      usleep(15000);
    }
    if (!gStopp) SetDone(1);
//    GCodeMode=0;
  }

}
//-------------------------------------------------------------
CBohren::CBohren(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,char * lpLayer,unsigned int color):CObject(ObjSource,x,y,z,lpLayer,color) {
  m_Xmin = x;
  m_Xmax = x;
  m_Ymin = y;
  m_Ymax = y;
  m_Zmin = z;
  m_Zmax = z;

  Art = Punkt;
}

//-------------------------------------------------------------
CBohren::~CBohren() {
}

//-------------------------------------------------------------
//void glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
void glutWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);

void CBohren::ExecuteDrawObject(int mode) {
  if (mode == Draw ) {
    float x,y,z;
    y = glYCNCToScreen(m_S.Y);
    x = glXCNCToScreen(m_S.X);
    z = glZCNCToScreen(m_S.Z);
    if ((ETiefe&0xFF)>=0 && (ETiefe&0xFF) < 7 && IsExecutable()) {
#ifdef USE_SHADER
      glLineWidth(2+gPa.WerkzeugDuchmesser[ETiefe&0xFF]/30.0);
      float radius = 100*glFaktor;  // 0.007
      glBegin(GL_LINE_STRIP);//GL_LINES);
      glVertex3f(x-radius,y-radius ,z);
      glVertex3f(x+radius,y+radius ,z);
      glEnd();
      glBegin(GL_LINE_STRIP);//GL_LINES);
      glVertex3f(x-radius,y+radius ,z);
      glVertex3f(x+radius,y-radius ,z);
      glEnd();
      glLineWidth(1);
#else
      float radius =  gPa.WerkzeugDuchmesser[ETiefe&0xFF]*glFaktor/2.0;
      float tiefe  =  gPa.Tiefe[ETiefe&0xFF]*glFaktor;
      int   Stufe  =  3;
      glTranslatef(x,y,tiefe );
      glutWireCylinder(radius,-tiefe,5+gPa.WerkzeugDuchmesser[ETiefe&0xFF]/30,Stufe );
      glTranslatef(-x,-y,-tiefe );
#endif
    } else {
      float radius = 100*glFaktor;  // 0.007
      glBegin(GL_LINE_STRIP);//GL_LINES);
      glVertex3f(x-radius,y-radius ,z);
      glVertex3f(x+radius,y+radius ,z);
      glEnd();
      glBegin(GL_LINE_STRIP);//GL_LINES);
      glVertex3f(x-radius,y+radius ,z);
      glVertex3f(x+radius,y-radius ,z);
      glEnd();


    }
    if (IsSelected()) {
      float rs = 100*glFaktor;  // 0.010;
      glLineWidth(2);
      glColor4f (1,1, 1,0.5);
      glBegin (GL_LINES);
      glVertex3f(x,y-rs/2,z);
      glVertex3f(x,y+rs/2,z);
      glVertex3f(x+rs/2,y,z);
      glVertex3f(x-rs/2,y,z);
      glEnd ();

      if (1) {
        //glBegin (GL_QUADS);
        glColor4f (1,1, 1,0.5);
        z-=0.001;
        glBegin (GL_LINE_LOOP );
        glVertex3f(x-rs,y+rs,z);
        glVertex3f(x+rs,y+rs,z);
        glVertex3f(x+rs,y-rs,z);
        glVertex3f(x-rs,y-rs,z);
        glEnd ();
      }
    }


  //} else if (mode == Art) {
  } else if (IsExecutable()) {
    if ((mode & Art)==Art) {
    ETiefe &= 0xFF;
    ClipperLib::cInt X1,Y1,Z1;
    Y1 = m_S.Y;
    X1 = m_S.X;
    Z1 = m_S.Z;
    if (AbhebenwennNoetig(X1,Y1)==0) return;

    // Abheben
    //Positionier(X1,Y1,Pa.Tiefe[0],(int)Pa.Tempo[Tempo_Verfahr]);
    WarteAufHaltAntwort();
    //while(GetAntwort()!= 'H' && ! gStopp);

    // Eintauchen (Bohren)
    PositionierWarteBis(X1,Y1,gPa.Tiefe[ETiefe&0xFF],(int)gPa.Tempo[Tempo_Bohren]);

    // Abheben
    PositionierWarteBis(X1,Y1,gPa.Tiefe[0],(int)gPa.Tempo[Tempo_Verfahr]);
    if (!gStopp) SetDone(1);
    }
  }
}
//-------------------------------------------------------------
/*
CGCodeLineObject::CGCodeLineObject(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color):CPolyLineObject(ObjSource,x,y,z,lpLayer,color) {
  Art = GCodeLinine;
}

//-------------------------------------------------------------
CGCodeLineObject::~CGCodeLineObject () {
//  free (m_Path);
}

//-------------------------------------------------------------
void CGCodeLineObject::ExecuteDrawObject(int mode) {
  if (m_Path.size() <2) return;
  if (mode == Draw ) {
    / *if (m_Path[1].Attr) {
      int w = m_Path[1].Attr/25;
      if (w==0) w = 1;
      if (w>10) w = 10;
      glLineWidth(w);
    }* /
    int PWM=1255;
    glBegin(GL_LINE_STRIP);//GL_LINES);
    int i;
    for (i=0;i < m_Path.size();i++) {
      float x,y,z;
      y = glYCNCToScreen(m_Path[i].Y);
      x = glXCNCToScreen(m_Path[i].X);
      z = glZCNCToScreen(m_Path[i].Z);
      if (PWM != m_Path[i].Attr) {
        glVertex3f(x,y,z);
        glEnd();
        PWM = m_Path[i].Attr;
        if (gPa.Maschine[ePowerPercent]) {
          PWM = _min(PWM*gPa.Maschine[ePowerPercent]/100,254);
        }
        if (IsDone() || PWM > 250 || PWM < 20) {
          DWORD clr = GetRGBColor();
          glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
        } else {
          DWORD clr = GetRGBColor();
          glColor3f(PWM/256.0, PWM/256.0,1.0-PWM/256.0);
        }
        glBegin(GL_LINE_STRIP);
        glVertex3f(x,y,z);
      } else if (m_SelectIx >= 0 && (i==m_SelectIx||i==(m_SelectIx+1))) {
        glVertex3f(x,y,z);
        glEnd();
        if (i==m_SelectIx) {
          glLineWidth(4);
          glColor3f(0.9,0.9,1);
        } else {
          glLineWidth(GetWitdh());
          DWORD clr = GetRGBColor();
          glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
        }
        glBegin(GL_LINE_STRIP);
        glVertex3f(x,y,z);
      } else {
        glVertex3f(x,y,z);
      }
    }
    glEnd();
  } else if ((mode & Art)==Art) {
    ClipperLib::cInt X1,Y1,Z1;
    Y1 = m_Path[0].Y;
    X1 = m_Path[0].X;
    Z1 = m_Path[0].Z;
    PositionierWarteBis(X1,Y1,Z1,(int)(m_G0Feddrate*gPa.Maschine[eGcodeRapid]/60));
    //if (AbhebenwennNoetig(X1,Y1)==0) return;
    //PositionierWarteBis(X1,Y1,Z1,(int)Pa.Tempo[Tempo_Eintauch]);
  } else  return;

  ClipperLib::cInt X1=0,Y1=0,Z1=0;
  if (ETiefe == AktTiefe) {
    int index=0;
    int runden=1;
    if (mode == Draw ) {
      glBegin(GL_LINE_STRIP);
    }
    while (runden){
      if (mode == Draw ) {
        float x,y,z;
        y = glYCNCToScreen(m_Path[index].Y);
        x = glXCNCToScreen(m_Path[index].X);
        z = glZCNCToScreen(m_Path[index].Z);
        glVertex3f(x,y,z);
      } else if ((mode & Art)==Art) {
        Y1 = m_Path[index].Y;
        X1 = m_Path[index].X;
        Z1 = m_Path[index].Z;
        int PWM = m_Path[index].Attr;
        if (gPa.Maschine[ePowerPercent]) SpindleWarteBis(1+_min(PWM*gPa.Maschine[ePowerPercent]/100,254));
        PositionierWarteBis(X1,Y1,Z1,(int)(m_G1Feddrate*gPa.Maschine[eGcodeFeed]/60));
      }
      if (runden==1) {
        if (index < (m_Path.size()-1)) {
          index++;
        } else {
          runden =0;
        }
      }
    }
    if (mode == Draw ) {
      glEnd();
    }
  }
  if ((mode & Art)==Art) {
    if (gPa.Maschine[ePowerPercent]) SpindleWarteBis(1);   // 1 Releais nicht schalten
    if (! gStopp && gOkCount>0) {
      usleep(15000);
    }
    if (!gStopp) SetDone(1);
    while(GetAntwort()!= 'H' && ! gStopp);
    //PositionierWarteBis(X1,Y1,Pa.Tiefe[0],(int)(m_G0Feddrate*GcodeRapid/100));
  }
}
*/
//-------------------------------------------------------------
CPolyLineObject::CPolyLineObject (CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color) {
  Art = Linie;
  m_Path.clear();// = (CNCPOINT *) malloc(m_ArraySize *sizeof(CNCPOINT));
  m_ClosedLoop=0;
  m_SelectIx=-1;
  m_Projektion=0;
  m_Insider=NULL;
  LastAttr=0;
}
//-------------------------------------------------------------
CPolyLineObject::~CPolyLineObject () {
  m_Path.clear();
}
//-------------------------------------------------------------
float GetBulgeCentre(float x1,float y1,float z1,float bulge, float x2,float y2,float z2, float * pi, float * pj) {
  float Bulge = bulge / 100000.0;
  double a = (x2-x1)/2.0;
  double b = (y2-y1)/2.0;
  float ph4= atan(Bulge);
  float u  = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
  float u2 = u / 2.0;
  float i  = u2 * Bulge;
  float c2 = sqrt(u2*u2+i*i)/2.0;
  float radius = c2 / sin(ph4);
  float winkel = atan2(a,b);
  winkel += ph4*2;
  if (pi!= NULL) *pi = radius * cos(winkel) * -1.0;
  if (pj!= NULL) *pj = radius * sin(winkel);
  return ph4*2;
}

//-------------------------------------------------------------
int DrawBulge(float x1, float y1, float z1, float bulge, float x2, float y2,
              float z2, int db) {
  float Bulge = bulge / 100000.0;
  double a = (x2 - x1) / 2.0;
  double b = (y2 - y1) / 2.0;
  float ph4 = atan(Bulge);
  // float swinkel = ph4 * 360.0 / (2.0 * M_PI)*4.0;

  float u = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
  float u2 = u / 2.0;
  float i = u2 * Bulge;
  float c2 = sqrt(u2 * u2 + i * i) / 2.0;
  float radius = c2 / sin(ph4);

  float winkel = atan2(a, b);  // + M_PI_2;    // 90 Grad zu A L
  float wk = winkel * 360.0 / (2.0 * M_PI);
  winkel += ph4 * 2;
  wk = winkel * 360.0 / (2.0 * M_PI);

  float CentreX = radius * cos(winkel);
  float CentreY = radius * sin(winkel);
  float xc = x1 - CentreX;
  float yc = y1 + CentreY;

/*  if (db) {
    glLineWidth(1);
    glColor3f(1, 0.0, 0);
    glBegin(GL_LINE_STRIP);
    float xx = glXCNCToScreen(xc);
    float yy = glYCNCToScreen(yc);
    float zz = glZCNCToScreen(z1);
    glVertex3f(xx - 0.01, yy, zz);
    glVertex3f(xx + 0.01, yy, zz);
    glVertex3f(xx, yy - 0.01, zz);
    glVertex3f(xx, yy + 0.01, zz);
    glEnd();
  }
*/
  int slices = radius / 15;
  double *sint=NULL,*cost=NULL;
  int n;
  if (slices < 0) {
    if (slices > -7) slices = -7;
    n = fghCircleTable(winkel, winkel - ph4 * 4, &sint, &cost, -slices);
  } else {
    if (slices < 7) slices = 7;
    n = fghCircleTable(winkel, winkel - ph4 * 4, &sint, &cost, slices);
  }

  /*
  if (db) {
    glLineWidth(4);
    glColor3f(0.9, 0.9, 1);
    glBegin(GL_LINE_STRIP);
  }*/

    float rs =0.007;
    if (db){
      float xs = xc + (cost[0] * radius);
      float ys = yc - (sint[0] * radius);
      float zs = glZCNCToScreen(z1);
      xs = glXCNCToScreen(xs);
      ys = glYCNCToScreen(ys);
      float ydm  = sin(winkel-M_PI_2)*rs;
      float xdm  = cos(winkel-M_PI_2)*rs;
      float yds  = sin(winkel)*rs;
      float xds  = cos(winkel)*rs;
      glVertex3f(xs+xds,ys-yds,zs);
      glVertex3f(xs+xdm,ys-ydm,zs);
      glVertex3f(xs-xds,ys+yds,zs);
    }

  float dz = z2 - z1;
  for (int ii = 0; ii <= n; ii++) {
    float XX = xc + (cost[ii] * radius);
    float YY = yc - (sint[ii] * radius);
    float x, y, zz;
    x = glXCNCToScreen(XX);
    y = glYCNCToScreen(YY);
    zz = glZCNCToScreen(z1 + (dz * ii / n));
    glVertex3f(x, y, zz);
  }

//  if (db) {
//    glEnd();
//  }


    if (sint) free(sint);
    if (cost) free(cost);
  return n;
}
//-------------------------------------------------------------
/*
int BogenFahren(int mode ,int X2,int Y2, int Z2,int D,
                int X1,int Y1, int Z1,int Tempo,int dir )
{
        if (mode == Draw ) {
          DrawBulge(X2,Y2,Z2,D,X1,Y1,Z1,0);
        } else if ((mode & Linie)==Linie) {
          float W,I=0,J=0;
          W= GetBulgeCentre(X2,Y2,Z2,D,X1,Y1,Z1,&I, &J);
          if (!gStopp) {
            if ((mode & LaserGravur)) {
              SpindleWarteBis(gPa.Maschine[eLaserPower]);
            }
            PositionierBogenWarteBis(X1,Y1,Z1,I,J,(W<0)?Tempo:-Tempo);
          }
        }

  return 0;
}
//-------------------------------------------------------------
int LinieFahren(int mode ,int X1,int Y1, int Z1,int Tempo)
{
       if (mode == Draw ) {
          float x,y,z;
          y = glYCNCToScreen(Y1);
          x = glXCNCToScreen(X1);
          z = glZCNCToScreen(Z1);
          glVertex3f(x,y,z);
        } else if ((mode & Linie)==Linie) {
          if (!gStopp) {
            if ((mode & LaserGravur)==0) {
              PositionierWarteBis(X1,Y1,Z1,(int)(int)Tempo);
            } else {
              if (gSpindleOn!=gPa.Maschine[eLaserPower]) {
                SpindleWarteBis(gPa.Maschine[eLaserPower]);
              }
              PositionierWarteBis(X1,Y1,Z1,gPa.Maschine[eLaserSpeed]);
            }
          }
        }

  return 0;
}

      // mach die Arbeit!
      bool bBulge = prevIndex>=0 && ((index && dir && m_Path[index-1].Attr!=0) || (!dir && m_Path[index].Attr!=0));
      Y1 = m_Path[index].Y;
      X1 = m_Path[index].X;
      Z1 = m_Path[index].Z+zustellen-eintauchen;
       if (!bBulge) {
        LinieFahren(mode ,X1,Y1,Z1,gPa.Tempo[ETiefe]);
      } else {
        if (dir) BogenFahren(mode ,m_Path[index-1].X,m_Path[index-1].Y,m_Path[index-1].Z+zustellen-eintauchenex,m_Path[index-1].Attr,
              X1,Y1,Z1,gPa.Tempo[ETiefe], dir);
        else BogenFahren(mode,m_Path[index+1].X,m_Path[index+1].Y,m_Path[index].Z+zustellen-eintauchenex,-m_Path[index].Attr,
              X1,Y1,Z1,gPa.Tempo[ETiefe], dir);
      }
      prevIndex = index;
*/
//-------------------------------------------------------------
void CPolyLineObject::ExecuteDrawObjectInRect(int mode,int left,int top,int right,int bottom){
  float x,y,z=0;
  int i;
  int db=0;
  int Inside=0;
  int Positoniert=0;
  int LastX=0;
  int LastY=0;
  int LastZ=0;
  int Z1=0;
  for (i=1; i < m_Path.size(); i++) {
    int x1  = m_Path[i-1].X;
    int x2  = m_Path[i].X;
    int y1  = m_Path[i-1].Y;
    int y2  = m_Path[i].Y;
    int xx1 = x1;
    int xx2 = x2;
    int yy1 = y1;
    int yy2 = y2;
    enum {PRot=100,PGelb,PGruen,PCyan,PBlau,PMagenta,PWeiss,PGrau};

    int horizontal  = x2 - x1;  // positve  == nach rechts
    int vertikal    = y2 - y1;  // positive == nach oben

    Inside=0;
    float vert_blau  = y1;
    float vert_cyan  = y2;
    float horz_gruen = x1;
    float horz_rot   = x2;
    if (horizontal) {
      vert_blau  = y1 + (float)(left -x1) * (float)vertikal   / (float)horizontal;
      vert_cyan  = y1 + (float)(right-x1) * (float)vertikal   / (float)horizontal;
    }
    if (vertikal) {
      horz_gruen = x1 + (float)(bottom-y1)* (float)horizontal / (float)vertikal;
      horz_rot   = x1 + (float)(top   -y1)* (float)horizontal / (float)vertikal;
    }

    if (horizontal==0) {
      // Vertikal
      if (x2 < left || x2 > right) goto extest;
      if (vertikal > 0) {
        if (y2 < bottom || y1 > top) goto extest;
        Inside =  PRot;
        if (y2 > top)    yy2  = top;
        if (y1 < bottom) yy1  = bottom;
      } else {
        if (y1 < bottom || y2 > top) goto extest;
        Inside =  PGelb;
        if (y1 > top)    yy1  = top;
        if (y2 < bottom) yy2  = bottom;
      }
    } else if (vertikal == 0) {
      // Horizontal
      if (y2 < bottom || y2 > top) goto extest;
      if (horizontal  > 0) {
        if (x2 < left || x1 > right) goto extest;
        Inside =  PGruen;
        if (x1 < left)  xx1  = left;
        if (x2 > right) xx2  = right;
      } else {
        if (x1 < left || x2 > right) goto extest;
        Inside =  PBlau;
        if (x2 < left)  xx2  = left;
        if (x1 > right) xx1  = right;
      }
    } else  {
      if  (horizontal >0) {
        if (vertikal<0) {
          // 270 - 360
          if (right  < x1 && left < x1) goto extest;
          if (bottom > y1 && top  > y1) goto extest;

          if (horz_rot >= left && horz_rot < right) {
            if (x2 > horz_rot) {
              Inside =  PRot;
              if (horz_rot > x1 && top < y1) {
                xx1 = horz_rot;
                yy1 = top;
              }
            }
          } else if ((vert_blau <= top) && (vert_blau > bottom)) {
            if (y2 < vert_blau) {
              Inside =  PGelb;
              if (left>x1 && vert_blau < y1) {
                xx1 = left;
                yy1 = vert_blau;
              }
            }
          }
          if (Inside) {
            if (horz_gruen < right && horz_gruen > left) {
              if (horz_gruen < x2 && bottom > y2) {
                xx2 = horz_gruen;
                yy2 = bottom;
              }
            }
            if (vert_cyan < top && vert_cyan > bottom) {
              if (right < x2 && vert_cyan > y2) {
                xx2 = right;
                yy2 = vert_cyan;
              }
            }
          }
        } else {
          // 0 - 90
          if (right < x1 && left < x1) goto extest;
          if (top < y1 && bottom < y1) goto extest;

          if ( (horz_gruen >= left) && (horz_gruen < right)) {
            if (x2 > horz_gruen ) {
              Inside =  PGruen;
              if (horz_gruen>x1 && bottom>y1) {
                xx1 = horz_gruen;
                yy1 = bottom;
              }
            }
          } else if ((vert_blau > bottom) && (vert_blau < top)) {
            if (y2 > vert_blau) {
              Inside =  PCyan;
              if (left >x1 && vert_blau>y1) {
                xx1 = left;
                yy1 = vert_blau;
              }
            }
          }
          if (Inside) {
            if (horz_rot < right && horz_rot > left) {
              if (horz_rot < x2 && top < y2) {
                xx2 = horz_rot;
                yy2 = top;
              }
            }
            if (vert_cyan < top && vert_cyan  > bottom) {
              if (right < x2 && vert_cyan < y2) {
                xx2 = right;
                yy2 = vert_cyan;
              }
            }
          }
        }
      } else {
        if (vertikal>0) {
          // 90 - 180
          if (left > x1 && right > x1 ) goto extest;
          if (top < y1 && bottom < y1 ) goto extest;

          if ( (horz_gruen < right) && (horz_gruen > left)) {
            if (x2 < horz_gruen) {
              Inside =  PBlau;
              if (horz_gruen < x1 && bottom> y1) {
                xx1 = horz_gruen;
                yy1 = bottom;
              }
            }
          } else if ((vert_cyan > bottom) && (vert_cyan < top)) {
            if (y2 > vert_cyan) {
              Inside =  PMagenta;
              if (right  < x1 && vert_cyan > y1) {
                xx1 = right;
                yy1 = vert_cyan;
              }
            }
          }
          if (Inside) {
            if (horz_rot < right && horz_rot > left) {
              if (horz_rot > x2 && top < y2) {
                xx2 = horz_rot;
                yy2 = top;
              }
            }
            if (vert_blau < top && vert_blau  > bottom) {
              if (left > x2 && vert_blau < y2) {
                xx2 = left;
                yy2 = vert_blau;
              }
            }
          }
        } else {
          // 180 - 270
          if (left   > x1 && right > x1) goto extest;
          if (bottom > y1 && top  > y1 ) goto extest;

          if ( (horz_rot < right) && horz_rot > left) {
            if (x2 < horz_rot) {
              Inside =  PWeiss;
              if (horz_rot < x1 && top < y1) {
                xx1 = horz_rot;
                yy1 = top;
              }
            }
          } else if ((vert_cyan < top) && (vert_cyan > bottom)) {
            if (y2 < vert_cyan) {
              Inside =  PGrau;
              if (right < x1 && vert_cyan < y1) {
                xx1 = right;
                yy1 = vert_cyan;
              }
            }
          }
          if (Inside) {
            if (horz_gruen < right && horz_gruen > left) {
              if (horz_gruen > x2 && bottom > y2) {
                xx2 = horz_gruen;
                yy2 = bottom;
              }
            }
            if (vert_blau < top && vert_blau  > bottom) {
              if (left > x2 && vert_blau > y2) {
                xx2 = left;
                yy2 = vert_blau;
              }
            }
          }
        }
      }
    }
extest:

#ifdef TESTSTUFF
    if (Inside) {
      glBegin(GL_LINES);
      glLineWidth(1);
      if (1) {
        glColor3f(1.0,0.0,0.0);
        glVertex3f(glXCNCToScreen(horz_rot),glYCNCToScreen(vert_cyan),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(horz_rot),glYCNCToScreen(vert_blau),glZCNCToScreen(z));

        glColor3f(0.0,1.0,0.0);
        glVertex3f(glXCNCToScreen(horz_gruen),glYCNCToScreen(vert_cyan),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(horz_gruen),glYCNCToScreen(vert_blau),glZCNCToScreen(z));

        glColor3f(0.0,0.0,1.0);
        glVertex3f(glXCNCToScreen(horz_gruen),glYCNCToScreen(vert_blau),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(horz_rot),glYCNCToScreen(vert_blau),glZCNCToScreen(z));

        glColor3f(0.0,1.0,1.0);
        glVertex3f(glXCNCToScreen(horz_gruen),glYCNCToScreen(vert_cyan),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(horz_rot),glYCNCToScreen(vert_cyan),glZCNCToScreen(z));

      }
      int tiefe= ETiefe;
      if (Inside>=PRot) {
        if (1) {
          ETiefe = Inside-PRot+1;
          DWORD clr = GetRGBColor();
          glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
          glVertex3f(glXCNCToScreen(x1+10),glYCNCToScreen(y1),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(x1-10),glYCNCToScreen(y1),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(x1),glYCNCToScreen(y1-10),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(x1),glYCNCToScreen(y1+10),glZCNCToScreen(z));
        }
        glEnd();
        glLineWidth(GetWitdh());
        ETiefe = tiefe;
        DWORD clr = GetRGBColor();
        glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);

        glBegin(GL_LINES);
        glVertex3f(glXCNCToScreen(xx1),glYCNCToScreen(yy1),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(xx2),glYCNCToScreen(yy2),glZCNCToScreen(z));
      }
      glEnd();
    }
#endif
    if (mode == Draw ) {
      glLineWidth(2);
      if (Inside ==0) {
        glColor3f(0.7,0.7,0.7);
        if (i > 0  && m_Path[i-1].Attr ) {
          glBegin(GL_LINE_STRIP);
          DrawBulge(m_Path[i-1].X,m_Path[i-1].Y,m_Path[i-1].Z,m_Path[i-1].Attr,
          m_Path[i].X,m_Path[i].Y,m_Path[i].Z,i==1&&db);
        }
        else {
          glBegin(GL_LINES);
          glVertex3f(glXCNCToScreen(x1),glYCNCToScreen(y1),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(x2),glYCNCToScreen(y2),glZCNCToScreen(z));
        }
        glEnd();

      } else {
        if (x1!=xx1 ||y1 != yy1) {
          glColor3f(0.7,0.7,0.0);
          glBegin(GL_LINES);
          glVertex3f(glXCNCToScreen(x1),glYCNCToScreen(y1),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(xx1),glYCNCToScreen(yy1),glZCNCToScreen(z));
          glEnd();
        }

        if (x2!=xx2 ||y2 != yy2) {
          glColor3f(0.7,0.7,0.0);
          glBegin(GL_LINES);
          glVertex3f(glXCNCToScreen(x2),glYCNCToScreen(y2),glZCNCToScreen(z));
          glVertex3f(glXCNCToScreen(xx2),glYCNCToScreen(yy2),glZCNCToScreen(z));
          glEnd();
        }

        glLineWidth(GetWitdh());
        DWORD clr = GetRGBColor();
        glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
        glBegin(GL_LINES);
        glVertex3f(glXCNCToScreen(xx1),glYCNCToScreen(yy1),glZCNCToScreen(z));
        glVertex3f(glXCNCToScreen(xx2),glYCNCToScreen(yy2),glZCNCToScreen(z));
        glEnd();
      }
    } else if ((mode & Art)==Art) {
      ETiefe &= 0xFF;
      if (Inside) {
        if (abs (xx1-LastX) > 5 || abs (yy1-LastY) > 5 ) {
          Positoniert=0;
        }
        if (Positoniert==0) {
          if ((mode & LaserGravur)==0) {
            int Z1 = gPa.Tiefe[0];
            //if (
            AbhebenwennNoetig(xx1,yy1);//==0);// return;
            PositionierWarteBis(xx1,yy1,Z1,(int)gPa.Tempo[Tempo_Verfahr]);
            Z1 = m_S.Z+gPa.Tiefe[(ETiefe < 7)?ETiefe:0];
            PositionierWarteBis(xx1,yy1,Z1,(int)gPa.Tempo[Tempo_Eintauch]);
          } else {
            Z1 = m_Path[0].Z;
            PositionierWarteBis(xx1,yy1,Z1,(int)gPa.Tempo[Tempo_Verfahr]);
          }
          Positoniert=1;
          LastX=xx1;
          LastY=yy1;
          LastZ=Z1;
        }
        //  XY Fahren
        if ((mode & LaserGravur)==0) {
          Z1 = m_S.Z+gPa.Tiefe[(ETiefe < 7)?ETiefe:0];
          PositionierWarteBis(xx2,yy2,Z1,(int)gPa.Tempo[ETiefe]);
        } else {
          SpindleWarteBis(gPa.Maschine[eLaserPower]);
          PositionierWarteBis(xx2,yy2,m_S.Z,gPa.Maschine[eLaserSpeed]);
        }
        LastX=xx2;
        LastY=yy2;
        LastZ=Z1;
        if (gStopp) {
          LastX=xx2;
          LastY=yy2;
          LastZ=Z1;
        }
      }
    }
  }
  if ((mode & Art)==Art) {
    if ((mode & LaserGravur)==1) {
      SpindleWarteBis(0);
    }
    if (gStopp) {
      SendToCNC('\033');
      goto Exit;
    }
  }

Exit:
  if ((mode & Art)==Art) {
    if (! gStopp && gOkCount>0) {
      usleep(15000);
    }
    while(GetAntwort()!= 'H' && ! gStopp);
    if (Positoniert) {
      if ((mode & LaserGravur)==0) {
        PositionierWarteBis(LastX,LastY,gPa.Tiefe[0],(int)gPa.Tempo[Tempo_Verfahr]);
      } else {
      }
    }
//    if (!gStopp) SetDone(1);
  }
}

//-------------------------------------------------------------
void CPolyLineObject::ExecuteDrawObject(int mode) {
  if (m_Path.size()<2) return;
  int istParitial = rcAuswahlCNC.left > rcAuswahlCNC.right && rcAuswahlCNC.top > rcAuswahlCNC.bottom;
  if (istParitial) {
	  ExecuteDrawObjectInRect(mode,rcAuswahlCNC.right,rcAuswahlCNC.top,rcAuswahlCNC.left,rcAuswahlCNC.bottom);
	  return ;
  }
  if (mode == Draw ) {
    int db=0;
#ifdef CTRLSHIFT
    if (Fl::event_state() & FL_SHIFT  && gDreiDFace==0) {
      db=1;
      if (m_Path[0].Attr==0) {
        float xs = glXCNCToScreen(m_Path[0].X);
        float ys = glYCNCToScreen(m_Path[0].Y);
        float zs = glZCNCToScreen(m_S.Z);
        float w = m_Path[1].x - m_Path[0].X;
        float h = m_Path[1].y - m_Path[0].Y;
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
    float x,y,z;
    int i;
    //if (Is3DFace()==0) {
//      glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
//     glBegin(GL_LINE_LOOP);
      glLineWidth(GetWitdh());
      glBegin(GL_LINE_STRIP);//GL_LINES);

      int Inside=1;
      int InsideLast=-1;

      for (i=0; i < m_Path.size(); i++) {
        y = glYCNCToScreen(m_Path[i].Y);
        x = glXCNCToScreen(m_Path[i].X);
        z = glZCNCToScreen(m_Path[i].Z);
        if (m_SelectIx>=0 && (i==m_SelectIx||i==(m_SelectIx+1))) {
          glVertex3f(x,y,z);
          glEnd();
          if (i==m_SelectIx) {
            glLineWidth(4);
            glColor3f(0.9,0.9,1);
          } else {
            glLineWidth(GetWitdh());
            DWORD clr = GetRGBColor();
            glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
          }
          glBegin(GL_LINE_STRIP);
          //glVertex3f(x,y,z);
        }
        //else
        {
          if (i > 0  && m_Path[i-1].Attr ) {
            DrawBulge(m_Path[i-1].X,m_Path[i-1].Y,m_Path[i-1].Z,m_Path[i-1].Attr,
                      m_Path[i].X,m_Path[i].Y,m_Path[i].Z,i==1&&db);
          }
          else glVertex3f(x,y,z);
        }
      }
      if (m_ClosedLoop) {
        y = glYCNCToScreen(m_Path[0].Y);
        x = glXCNCToScreen(m_Path[0].X);
        z = glZCNCToScreen(m_Path[0].Z);
        glVertex3f(x,y,z);

      }
      glEnd();
    //}

    extern int     gbShowTransformation;
    //else {
    if ((m_Konstruktion != VERARBEITEDT && m_Konstruktion!=KONTUR)
      && m_Projektion != 0 && gDreiDFace && !gbShowTransformation) {
/*      if (m_Path.size()>4) {
        DrawConvex();
      } else */ {
        DWORD clr = GetRGBColor();
        GLfloat mat_diffuse[4];
        mat_diffuse[0] = (clr>>0 & 0xFF) / 255.0;
        mat_diffuse[1] = (clr>>8 & 0xFF) / 255.0;
        mat_diffuse[2] = (clr>>16 & 0xFF)/ 255.0;
        mat_diffuse[3] = gAlphaTransp;

        //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        //glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
//        glColor3f((clr>>0 & 0xFF) / 255.0,(clr>>8 & 0xFF) / 255.0,(clr>>16 & 0xFF) / 255.0);
        glBegin(GL_POLYGON);
//      glNormal3d(m_nx,m_ny,m_nz);

        for (i=0; i < m_Path.size(); i++) {
          if ((mode & Art)==Art) {
          } else {
            float x,y,z;

            y = glYCNCToScreen(m_Path[i].Y);
            x = glXCNCToScreen(m_Path[i].X);
            z = glZCNCToScreen(m_Path[i].Z);

            glVertex3f(x,y,z);
          }
        }
        glEnd();
      }
    }

#ifdef USE_SHADER
    if (gbShowTransformation) {
        ClipperLib::IntPoint p = m_Path[0];
        float xs,ys,zs;
        glLineWidth(2);
        ys = glYCNCToScreen(p.Y);
        xs = glXCNCToScreen(p.X);
        zs = glZCNCToScreen(p.Z);
        float rs = 0.005*MMToScreen;
        glColor4f (1,1, 1,0.5);
        {
          glBegin (GL_LINE_LOOP );
          glVertex3f(xs-rs,ys+rs,zs);
          glVertex3f(xs+rs,ys+rs,zs);
          glVertex3f(xs+rs,ys-rs,zs);
          glVertex3f(xs-rs,ys-rs,zs);
          glEnd ();
        }
    }
#endif
    if (0&&IsSelected()) {
      for (i=0; i <= m_Path.size(); i++) {
        int j = (i < m_Path.size())?i:0; // Polygon schliessen
        ClipperLib::IntPoint p = m_Path[j];

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
  //------------------------------------------
  ClipperLib::cInt X1=0,Y1=0,Z1=0;

  if (IsExecutable()) {
    ETiefe &= 0xFF;   // Kein RGB !
    if (mode == Draw ) {
      glColor3f(0.7,0.7,1);
      glColor3f(0.2 + 0.7 * gGrayOut,0.2 + 0.7 * gGrayOut,0.2 + 0.9 * gGrayOut);
      glLineWidth(1);
    } else if ((mode & Art)==Art) {
      ClipperLib::cInt X1,Y1,Z1;
      Y1 = m_S.Y;
      X1 = m_S.X;
      if ((mode & LaserGravur)==0) {
        Z1 = gPa.Tiefe[0];
        if (AbhebenwennNoetig(X1,Y1)==0) return;
        PositionierWarteBis(X1,Y1,Z1,(int)gPa.Tempo[Tempo_Verfahr]);
        Y1 = m_S.Y;
        X1 = m_S.X;
        Z1 = m_S.Z;
        PositionierWarteBis(X1,Y1,Z1,(int)gPa.Tempo[Tempo_Eintauch]);
      } else {
        //Z1 = m_S.Z;
        Y1 = m_Path[0].Y;
        X1 = m_Path[0].X;
        Z1 = m_Path[0].Z;
        PositionierWarteBis(X1,Y1,Z1,(int)gPa.Tempo[Tempo_Verfahr]);
      }
    }
    // Fraeser - Arbeit zeigen
    float len=0;
    float tauchlen=0;
    float breite =  100;
    float tiefe  =  100;
    float stufe  =  100;
    int runden=2;
    int endIndex =0;
    if (ETiefe >=0 && ETiefe <= 6) {
      breite =  gPa.WerkzeugDuchmesser[ETiefe];
      tiefe  =  gPa.Tiefe[ETiefe];
      stufe  =  gPa.Stufe[ETiefe];
    }
    if ((mode & LaserGravur)==0) {
      if (tiefe >=0) return;
    } else {
      runden   = 1;
      tiefe    = 0;
      stufe    = 100;
      endIndex =m_Path.size()-1;
    }

    int last = m_Path.size()-1;
    ClipperLib::cInt xd = m_Path[0].X - m_Path[last].X;
    ClipperLib::cInt yd = m_Path[0].Y - m_Path[last].Y;
    ClipperLib::cInt zd = m_Path[0].Z - m_Path[last].Z;
    if (!m_ClosedLoop && abs(xd) < 3 && abs(yd) < 3 && abs(zd) < 3 ) {
      m_ClosedLoop=true;
    }

    if (!m_ClosedLoop) stufe /= 2;
    int n = ((-tiefe / stufe)+0.9);
    if (n>0) stufe = -tiefe / n;    // Stufen aufteilen

    if (stufe > -tiefe ) stufe = -tiefe;
    float zustellen = 0.0;
    float eintauchen=0;
    float eintauchenex=0;
    int index=0;
    int dir=m_ClosedLoop;
    int prevIndex=-1;
    int schlussflag = 1;
    glBegin(GL_LINE_STRIP);
    if (!m_ClosedLoop && tiefe) {
      // Startpunkt zum schief eintauchen bestimmen
      tauchlen = stufe * 5;
      for (int i=0; i < m_Path.size(); i++) {
        float x1,y1,z1;
        float x2,y2,z2;
        x2 = m_Path[i].X;
        y2 = m_Path[i].Y;
        z2 = m_Path[i].Z;
        if (i > 0) {
          float w = sqrt(pow(x2-x1,2)+pow(y2-y1,2)+pow(z2-z1,2));
          len = len +w;
          if (index ==0 && len >= tauchlen) {
            index = i;
            dir = 0;
            m_S.X = x2;       // zum Startpunkt Anfahren
            m_S.Y = y2;
            m_S.Z = z2;
            break;
          }
        }
        x1 = x2;
        y1 = y2;
        z1 = z2;
      }
    }
    if (gPa.Stufe[ETiefe]<= 10) {
      m_S.X = m_Path[0].X;
      m_S.Y = m_Path[0].Y;
      m_S.Z = m_Path[0].Z;
      zustellen = tiefe;
      runden = 2;
    }
    if ((mode & LaserGravur)) {
      dir = 1;
    }
    while (runden) {
      if (prevIndex>=0) {
        if (eintauchen < stufe) {
          // Eintachtiefe fuer Teilstück bestimmen
          float w = sqrt(pow((float)(m_Path[index].X-m_Path[prevIndex].X),2)+pow((float)(m_Path[index].Y-m_Path[prevIndex].Y),2));
          eintauchen += (w / 5)+1;  // etwa 11 Grad
          if (eintauchen >= stufe) {
            // bei diesem index erreicht
            eintauchen = stufe;
            if (endIndex==0 ) endIndex =index;
          }
          if ((zustellen-eintauchen)  < tiefe) eintauchen = (tiefe - zustellen)*-1;
        }
      }
      if (zustellen  <= tiefe) {
        zustellen = tiefe;
      }
      // mach die Arbeit!
      bool bBulge = prevIndex>=0 && ((index && dir && m_Path[index-1].Attr!=0) || (!dir && m_Path[index].Attr!=0));
      if (!bBulge) {
        if (mode == Draw ) {
          float x,y,z;
          y = glYCNCToScreen(m_Path[index].Y);
          x = glXCNCToScreen(m_Path[index].X);
          z = glZCNCToScreen(m_Path[index].Z+zustellen-eintauchen);
          glVertex3f(x,y,z);
        } else if ((mode & Art)==Art) {
      Y1 = m_Path[index].Y;
      X1 = m_Path[index].X;
      Z1 = m_Path[index].Z+zustellen-eintauchen;
          if (!gStopp) {
            if ((mode & LaserGravur)==0) {
              PositionierWarteBis(X1,Y1,Z1,(int)(int)gPa.Tempo[ETiefe]);
      } else {
              if (gSpindleOn!=gPa.Maschine[eLaserPower]) {
                SpindleWarteBis(gPa.Maschine[eLaserPower]);
              }
              PositionierWarteBis(X1,Y1,Z1,gPa.Maschine[eLaserSpeed]);
            }
          }
        }
      } else {
        if (mode == Draw ) {
          if (dir) {
            DrawBulge(
              m_Path[index-1].X,m_Path[index-1].Y,m_Path[index-1].Z+zustellen-eintauchenex,m_Path[index-1].Attr,
              m_Path[index  ].X,m_Path[index  ].Y,m_Path[index  ].Z+zustellen-eintauchen,  0);
          } else {
            DrawBulge(
              m_Path[index+1].X,m_Path[index+1].Y,m_Path[index+1].Z+zustellen-eintauchenex,-m_Path[index].Attr,
              m_Path[index].X,m_Path[index].Y,m_Path[index].Z+zustellen-eintauchen,  0);
          }
        } else if ((mode & Art)==Art) {
          Y1 = m_Path[index].Y;
          X1 = m_Path[index].X;
          Z1 = m_Path[index].Z+zustellen-eintauchen;
          float W,I=0,J=0;
          if (dir) {
            W= GetBulgeCentre(
                 m_Path[index-1].X,m_Path[index-1].Y,m_Path[index-1].Z+zustellen-eintauchenex,m_Path[index-1].Attr,
                 m_Path[index  ].X,m_Path[index  ].Y,m_Path[index  ].Z+zustellen-eintauchen, &I, &J);
          } else {
            W= GetBulgeCentre(
                 m_Path[index+1].X,m_Path[index+1].Y,m_Path[index+1].Z+zustellen-eintauchenex,-m_Path[index].Attr,
                 m_Path[index  ].X,m_Path[index  ].Y,m_Path[index  ].Z+zustellen-eintauchen, &I, &J);
          }
          if (!gStopp) {
            if ((mode & LaserGravur)) {
              SpindleWarteBis(gPa.Maschine[eLaserPower]);
            }
            if (W<0) {
              PositionierBogenWarteBis(X1,Y1,Z1,I,J,gPa.Tempo[ETiefe]);
            } else {
              PositionierBogenWarteBis(X1,Y1,Z1,I,J,-gPa.Tempo[ETiefe]);
            }
          }
        }
      }
      prevIndex = index;

      // naechsten Punkt bestimmen
      if (m_ClosedLoop) {
        if (runden==1) {
          // Strecke bei der eingetaucht wurde mit Solltiefe abfahren
          if (index < endIndex) {
            index++;
          } else {
            runden =0;
          }
        } else  if (index < m_Path.size()-1) {
          index ++;
        } else {
          index = 0;
          // Solange runden drehen bis Tiefe erreicht ist
          if (runden > 1 && zustellen > tiefe) {
            zustellen -= eintauchen;
            eintauchen=0;
          }
          if (zustellen <= tiefe) {
            if (runden) runden --;
            if (endIndex==0 ) endIndex =m_Path.size()-1;
          }
        }
      } else if (dir) {
        if (index < m_Path.size()-1) {
          index ++;
        } else {
          dir = 0;
          index--;
          if (runden > 1 && zustellen > tiefe) {
            zustellen -= eintauchen;
            eintauchen=0;
          }
          if (zustellen <= tiefe) {
            if (runden) runden --;
            if (endIndex==0 ) endIndex = m_Path.size()-1;
          }
        }
      } else {
        if (index  > 0) {
          index --;
        } else {
          dir = 1;
          index++;
          if (runden > 1 && zustellen > tiefe) {
            zustellen -= eintauchen;
            eintauchen=0;
          }
          if (zustellen <= tiefe) {
            if (runden) runden --;
            if (endIndex==0 ) endIndex =m_Path.size()-1;
          }
        }
      }
      eintauchenex = eintauchen;
    }
    if ((mode & LaserGravur)) {
      SpindleWarteBis(0);
    }
    if (mode == Draw ) {
      glEnd();
    }
  }
  if ((mode & Art)==Art) {
    if (! gStopp && gOkCount>0) {
      usleep(15000);
    }
//    GCodeMode=0;
    while(GetAntwort()!= 'H' && ! gStopp);
    if ((mode & LaserGravur)==0) {
      PositionierWarteBis(X1,Y1,gPa.Tiefe[0],(int)gPa.Tempo[Tempo_Verfahr]);
    } else {
    }
    if (!gStopp) SetDone(1);
  }
}
//------------------------------------------
void CPolyLineObject::SaveStlObject(void) {
  if (m_Path.size()>3) {
    if (m_Konstruktion != VERARBEITEDT) SaveConvex();
  }
}


//-------------------------------------------------------------
void CPolyLineObject::CalculateNormale(int rechts){
  float p1[3];
  float p2[3];
  float rgNormal[3]={0,0,0};
  if (m_Path.size()>3) {
    if (!rechts) {
      p1[0] = m_Path[3].X - m_Path[2].X;
      p2[0] = m_Path[2].X - m_Path[1].X;
      p1[1] = m_Path[3].Y - m_Path[2].Y;
      p2[1] = m_Path[2].Y - m_Path[1].Y;
      p1[2] = m_Path[3].Z - m_Path[2].Z;
      p2[2] = m_Path[2].Z - m_Path[1].Z;
    } else {
      p1[0] = m_Path[1].X - m_Path[2].X;
      p2[0] = m_Path[2].X - m_Path[3].X;
      p1[1] = m_Path[1].Y - m_Path[2].Y;
      p2[1] = m_Path[2].Y - m_Path[3].Y;
      p1[2] = m_Path[1].Z - m_Path[2].Z;
      p2[2] = m_Path[2].Z - m_Path[3].Z;
    }
    cross3(&rgNormal[0],&p1[0],&p2[0]);
    normalize(&rgNormal[0],&rgNormal[1],&rgNormal[2]);
    m_nx = rgNormal[0];
    m_ny = rgNormal[1];
    m_nz = rgNormal[2];
  }
}


void CPolyLineObject::AddBulgeVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr) {
  float x2 = x;
  float y2 = y;
  float z2 = z;
  float bulge = Attr;
  int last=m_Path.size()-1;
  if (last<0) return;
  ClipperLib::IntPoint p = m_Path[last];

  float x1 = p.X;
  float y1 = p.Y;
  float z1 = p.Z;

  float Bulge = bulge / 100000.0;
  double a = (x2 - x1) / 2.0;
  double b = (y2 - y1) / 2.0;
  float winkel4 = atan(Bulge);

  // float swinkel = winkel4 * 360.0 / (2.0 * M_PI)*4.0;

  float u = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
  float u2 = u / 2.0;
  float i = u2 * Bulge;
  float c2 = sqrt(u2 * u2 + i * i) / 2.0;
  float radius = c2 / sin(winkel4);

  float winkel = atan2(a, b);  // + M_PI_2;    // 90 Grad zu A L
//  float wk1 = winkel * 360.0 / (2.0 * M_PI);
  winkel += winkel4 * 2;
//  float wk2 = winkel * 360.0 / (2.0 * M_PI);

//  printf("%0.1f %0.1f %s \n",wk1,wk2,m_pLayer->m_Name);

  float CentreX = radius * cos(winkel);
  float CentreY = radius * sin(winkel);
  float xc = x1 - CentreX;
  float yc = y1 + CentreY;

  int slices = 1;

  if      (winkel4 > M_PI_4) slices = 16;


  //int wk = (winkel4)* -4.0 * 360.0 / (2.0 * M_PI) + 0.5;;
  int wk = (winkel4)* -720.0 / M_PI;;
  if      (wk > 280) slices = 16;
  else if (wk > 190) slices = 12;
  else if (wk > 100) slices = 8;
  else               slices = 4;

  if      (wk < -280) slices = 16;
  else if (wk < -190) slices = 12;
  else if (wk < -100) slices = 8;
  else if (wk < 0   ) slices = 4;
  if (0&&m_Konstruktion!=FORM) {        // erst am schluss gueltig
    if      (radius>  1500) slices*=2;
    else if (radius< -1500) slices*=2;
    if      (radius>  3000) slices*=2;
    else if (radius< -3000) slices*=2;
  } else {
   slices*=2;
  }
  //printf("%d %0.1f %d %s \n",wk,radius,slices,m_pLayer->m_Name);

  float dz = z2 - z1;
  float startw = winkel;
  float teil = (winkel4)*-4.0 / (float)slices;
  int j=slices;
  while (j) {
    j--;


    startw += teil;
    p.X = xc + (cos(startw) * radius);
    p.Y = yc - (sin(startw) * radius);
    p.Z = z1;//z1 - (dz * j / slices);
    p.Attr=0;

//    if (m_ClosedLoop &&  p.X==m_S.X && p.Y == m_S.Y && p.Z == m_S.Z) {
    if (m_ClosedLoop &&  p.X==m_Path[0].X && p.Y == m_Path[0].Y && p.Z == m_Path[0].Z) {
      p.Attr=0;
    } else {
      m_Path.push_back(p);
    }

    if (p.X < m_Xmin) m_Xmin = p.X;
    if (p.X > m_Xmax) m_Xmax = p.X;
    if (p.Y < m_Ymin) m_Ymin = p.Y;
    if (p.Y > m_Ymax) m_Ymax = p.Y;
  }
  if (m_Xmin < gObjectManager->MinMaxleft)       gObjectManager->MinMaxleft  = m_Xmin;
  if (m_Xmax > gObjectManager->MinMaxright)      gObjectManager->MinMaxright = m_Xmax;
  if (m_Ymin < gObjectManager->MinMaxtop)        gObjectManager->MinMaxtop   = m_Ymin;
  if (m_Ymax > gObjectManager->MinMaxbottom)     gObjectManager->MinMaxbottom= m_Ymax;

  return ;
}
//-------------------------------------------------------------
void CPolyLineObject::AddVertex(ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,int Attr) {
  ClipperLib::IntPoint p;
  //static int anzahl=0;
  if (m_ClosedLoop && m_Path.size()>1 && x==m_Path[0].X && y == m_Path[0].Y && z == m_Path[0].Z) {
    // macht ev. noralisieren falsch wenn Punkt doppelt
    if (LastAttr==0) return;
  }

  if (z < -400000) {
  z=0;
  }

  if (LastAttr>100 ||LastAttr<-100) {
    int a= LastAttr;
    LastAttr=0;
    //anzahl++;
    AddBulgeVertex(x,y,z,a);
    LastAttr=Attr;
    return;
  }
  LastAttr=Attr;

  p.X = x;
  p.Y = y;
  p.Z = z;
  p.Attr=0;//Attr;
  m_Path.push_back(p);

  if (x < m_Xmin) m_Xmin  = x;
  if (x > m_Xmax) m_Xmax = x;
  if (y < m_Ymin) m_Ymin   = y;
  if (y > m_Ymax) m_Ymax= y;
  if (z < m_Zmin) m_Zmin   = z;
  if (z > m_Zmax) m_Zmax= z;

  if (x < gObjectManager->MinMaxleft)       gObjectManager->MinMaxleft  = x;
  if (x > gObjectManager->MinMaxright)      gObjectManager->MinMaxright = x;
  if (y < gObjectManager->MinMaxtop)        gObjectManager->MinMaxtop   = y;
  if (y > gObjectManager->MinMaxbottom)     gObjectManager->MinMaxbottom= y;
  if (z < gObjectManager->MinMaxfront)      gObjectManager->MinMaxfront = z;
  if (z > gObjectManager->MinMaxback)       gObjectManager->MinMaxback  = z;
}

//-------------------------------------------------------------

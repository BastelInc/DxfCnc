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
#include "Object.h"
extern int gStopp;
#include "Settings.h"

//-------------------------------------------------------------
CStlObject::CStlObject(CObjectManager * ObjSource,ClipperLib::cInt x,ClipperLib::cInt y,ClipperLib::cInt z,const char * lpLayer,unsigned int color) :CObject(ObjSource,x,y,z,lpLayer,color)
{
  m_Path = NULL;
  m_Zoom = 1;
  m_StlObject=NULL;
  m_NumStlObjects=0;
  m_StlHasColor=0;
  m_VertexBuffer=0;
  m_ColorBuffer=0;
  m_NormalBuffer=0;
  m_Konstruktion=KONTUR;
  Art = StlObject;
}

//-------------------------------------------------------------
CStlObject::~CStlObject()
{
  if (m_Path) free (m_Path);
  if (m_StlObject) free(m_StlObject);
#ifdef USE_SHADER
  m_vertices.clear();
  m_colors.clear();;
  m_normals.clear();;
  m_UV.clear();
  if (m_VertexBuffer)
    glDeleteBuffers(1, &m_VertexBuffer);
  m_VertexBuffer=0;
  if (m_NormalBuffer)
    glDeleteBuffers(1, &m_NormalBuffer);
  m_NormalBuffer=0;
  if (m_ColorBuffer)
    glDeleteBuffers(1, &m_ColorBuffer);
  m_ColorBuffer=0;
  if (m_UVBuffer)
    glDeleteBuffers(1, &m_UVBuffer);
  m_UVBuffer=0;
#endif
}

//-------------------------------------------------------------
unsigned int CStlObject::GetRGBColor()
{
  if (ETiefe>255) {
    return ((unsigned int)ETiefe&0xFF00)<< 8 | ((unsigned int)ETiefe&0xFF0000)>> 8  | ((unsigned int)ETiefe&0xFF000000)>> 24;
  }
  if ((m_Select & Done) == Done) {
    return gFarbe256[0];
  } else if ((m_Select&Active)==Active) {
    return gFarbe256[ETiefe&0xFF];
  } else {
    return gFarbe256[19];
  }
}

//-------------------------------------------------------------
int CStlObject::LoadStlFile(char * pUser)
{

  FILE * stream;
  char str[255];
  m_NumStlObjects=0;
  float Objzoom = m_Zoom;
  int   YUpp    = 0;
  unsigned short CurColor=0;
  m_Xmin = 0x7FFFFFFF;
  m_Xmax = 0x80000000;
  m_Ymin = 0x7FFFFFFF;
  m_Ymax = 0x80000000;
  m_Zmin = 0x7FFFFFFF;
  m_Zmax = 0x80000000;
//m_Texture=1;
  if (pUser) {
    if (strncmp(pUser,"ZOOM:",5)==0) {
      sscanf(pUser,"ZOOM:%f\n",&Objzoom);
    } else if (strncmp(pUser,"YUPP",4)==0) {
      YUpp    = 1;
    } else if (strncmp(pUser,"ZOOMYUPP:",9)==0) {
      sscanf(pUser,"ZOOMYUPP:%f\n",&Objzoom);
      YUpp    = 1;
    } else if (strncmp(pUser,"ZOOMDOWN:",9)==0) {
      sscanf(pUser,"ZOOMDOWN:%f\n",&Objzoom);
      YUpp    = 2;
    } else if (strncmp(pUser,"ZOOMYDOWN:",9)==0) {
      sscanf(pUser,"ZOOMYDOWN:%f\n",&Objzoom);
      YUpp    = 3;
    }
  }

  m_vertices.clear();
  m_colors.clear();;
  m_normals.clear();;
  m_UV.clear();;

  unsigned int clr = GetRGBColor();
  WORD StlRgb;
  StlRgb = ((clr & 0xF80000) >> 9) |((clr & 0xF800) >> 6) | ((clr & 0xF8) >> 3) | 0x8000;

  if ((stream = fopen(m_Path,"r"))) {
//    NumVertex=0;
    m_StlHasColor=0;

    struct stat statbuf;
    fstat(fileno(stream), &statbuf);
    if (statbuf.st_size < 100) return 0;
    fgets(str,sizeof(str)-1,stream);

    float vmin[3] = {100000,100000,100000};
    float vmax[3] = {-100000,-100000,-100000};
    float rgVertex[9];
    float rgNormal[3];
    bool isBinary;
    isBinary = (strncmp(str, "solid",5) != 0);
    fpos_t pos = {0};
    fsetpos(stream,&pos);
    if (isBinary) {
      fclose(stream);
      FILE *f =  fopen(m_Path, "rb");   // Binaer oeffen
      if (!f)
        return false;


      char title[80];
      int numFaces;
      int r = fread(title, 80, 1, f);
      if (r<=0) {
        fclose(f);
        return 0;
      }
      r = fread((void*)&numFaces, 4, 1, f);
      if (r<=0) {
        fclose(f);
        return 0;
      }
      unsigned short uint16;
      unsigned int  normal1;
      unsigned int  normal2=-1;
//      AdjustGraphicBuffers(numFaces*3+3);
      int stlbus_size = numFaces*sizeof(StlTriangle);
      m_StlObject = (StlTriangle*)malloc(stlbus_size );
      r = fread((void*)m_StlObject,1,stlbus_size,  f);
      fclose(f);
      if (m_StlObject[10].Attribute) m_StlHasColor=1;
      if (r!=stlbus_size) {
        free (m_StlObject);
        m_StlObject=NULL;
        return 0;
      }
      m_NumStlObjects = numFaces;
    } else {
      int numFaces =  statbuf.st_size / 80;
      int stlbus_size = numFaces*sizeof(StlTriangle);
      m_StlObject = (StlTriangle*)malloc(stlbus_size );


      char title[80];
      char  * pnt;
      char   s0[80];
      char   s1[80];
      unsigned short uint16;
      unsigned int  normal1;
      unsigned int  normal2=-1;
      uint16 =  (20) + (20 << 5) + (20 << 10);
      m_NumStlObjects=0;
      fgets(title, 80,stream);

      while (fgets(str,sizeof(str)-1,stream)!= 0) {
        pnt = str;
        int indent =0;
        while (*pnt == ' ')
          indent++, pnt++;
        if (strncmp(pnt,"facet",5) ==0) {
          if (sscanf(str," %s %s %f %f %f",s0,s1,&rgNormal[0],&rgNormal[1],&rgNormal[2])==5) {
            fgets(str,sizeof(str)-1,stream);
            fgets(str,sizeof(str)-1,stream);
            if (sscanf(str," %s %f %f %f",s1,&rgVertex[0],&rgVertex[1],&rgVertex[2])==4) {
              fgets(str,sizeof(str)-1,stream);
              if (sscanf(str," %s %f %f %f",s1,&rgVertex[3+0],&rgVertex[3+1],&rgVertex[3+2])==4) {
                fgets(str,sizeof(str)-1,stream);
                if (sscanf(str," %s %f %f %f",s1,&rgVertex[6+0],&rgVertex[6+1],&rgVertex[6+2])==4) {
                  {
                    normal1 = rgNormal[0] + rgNormal[1]*10 + rgNormal[2]*100;
                    //if (normal1 != normal2)
                    {
                      normal2  = normal1;
                      if (m_NumStlObjects<numFaces) {
                        m_StlObject[m_NumStlObjects].Normale[0]  = rgNormal[0];
                        m_StlObject[m_NumStlObjects].Normale[1]  = rgNormal[1];
                        m_StlObject[m_NumStlObjects].Normale[2]  = rgNormal[2];
                        m_StlObject[m_NumStlObjects].Attribute   = StlRgb;

                        for (size_t i = 0; i<9; ++i) {
                          int i2 = i /9;
                          int i3 = i %9;
                          int j  = i % 3;
                          m_StlObject[m_NumStlObjects+i2].Vertex[i3] = rgVertex[i];
                          if (rgVertex[i] < vmin[j])
                            vmin[j] = rgVertex[i];
                          if (rgVertex[i] > vmax[j])
                            vmax[j] = rgVertex[i];
                        }

                        m_NumStlObjects++;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      fclose(stream);
    }
    float resizetoMM =  Objzoom;

    float Drehung = m_Winkel / 57.29577f;
    int numcalc=m_NumStlObjects*3;
    size_t i3;
    for (i3 = 0; i3 < numcalc; ++i3) {
      int i = i3/3;
      int i2= (i3%3)*3;
      m_StlObject[i].Vertex[i2+0]  *=  resizetoMM;
      m_StlObject[i].Vertex[i2+1]  *=  resizetoMM;
      m_StlObject[i].Vertex[i2+2]  *=  resizetoMM;


      if (m_StlObject[i].Vertex[i2+0] < m_Xmin) m_Xmin = m_StlObject[i].Vertex[i2+0];
      if (m_StlObject[i].Vertex[i2+0] > m_Xmax) m_Xmax = m_StlObject[i].Vertex[i2+0];
      if (m_StlObject[i].Vertex[i2+1] < m_Ymin) m_Ymin = m_StlObject[i].Vertex[i2+1];
      if (m_StlObject[i].Vertex[i2+1] > m_Ymax) m_Ymax = m_StlObject[i].Vertex[i2+1];
      if (m_StlObject[i].Vertex[i2+2] < m_Zmin) m_Zmin = m_StlObject[i].Vertex[i2+2];
      if (m_StlObject[i].Vertex[i2+2] > m_Zmax) m_Zmax = m_StlObject[i].Vertex[i2+2];
      if (m_StlHasColor==0 && i2==0) m_StlObject[i].Attribute   = StlRgb;
    }

    float xf = rgTextureData[m_Texture].xmmFaktor;
    float hf = rgTextureData[m_Texture].ymmFaktor;// / ((float)(m_Ymax - m_Ymin)*1.);

    SetBufferSize(m_NumStlObjects);

    for (i3 = 0; i3 < numcalc; ++i3) {
      int i = i3/3;
      int i2= (i3%3)*3;

      float x  =  m_StlObject[i].Vertex[i2+0] /*- m_Xmin */;
      float y  =  m_StlObject[i].Vertex[i2+1] /*- m_Ymin*/;
      float z  =  m_StlObject[i].Vertex[i2+2] /*- m_Zmin + m_S.Z/100.0*/;

      m_StlObject[i].Vertex[i2+0]  =  x;
      if (YUpp==1) {
        m_StlObject[i].Vertex[i2+1]  = -z;
        m_StlObject[i].Vertex[i2+2]  = y -m_Ymin;
      } else if (YUpp==3) {
        m_StlObject[i].Vertex[i2+2]  = -y;
        m_StlObject[i].Vertex[i2+1]  = z;
      } else {
        m_StlObject[i].Vertex[i2+1]  =  y;
        m_StlObject[i].Vertex[i2+2]  =  z;
      }

      vector3 v;
      if (m_Winkel) {
        float x = m_StlObject[i].Vertex[i2+0];
        float y = m_StlObject[i].Vertex[i2+1];
        v.Vertex[0] = (x *cos(Drehung) -y *sin(Drehung))+ (m_S.X/100.0);
        v.Vertex[1] = (y *cos(Drehung) +x *sin(Drehung))+ (m_S.Y/100.0);
        v.Vertex[2] = m_StlObject[i].Vertex[i2+2]+ (m_S.Z/100.0);
        if (i2==0) {
          x = m_StlObject[i].Normale[0];
          y = m_StlObject[i].Normale[1];
          m_StlObject[i].Normale[0] = (x *cos(Drehung) -y *sin(Drehung));
          m_StlObject[i].Normale[1] = (y *cos(Drehung) +x *sin(Drehung));
        }
      } else {
        v.Vertex[0] = m_StlObject[i].Vertex[i2+0]+ (m_S.X/100.0);
        v.Vertex[1] = m_StlObject[i].Vertex[i2+1]+ (m_S.Y/100.0);
        v.Vertex[2] = m_StlObject[i].Vertex[i2+2]+ (m_S.Z/100.0);
      }

#ifdef USE_SHADER
//      m_vertices.push_back(v);
      m_vertices[i3]= v;

      vector3 n = {m_StlObject[i].Normale[0],m_StlObject[i].Normale[1],m_StlObject[i].Normale[2]};
//      m_normals.push_back(n);
      m_normals[i3] = n;
      unsigned short uint16= m_StlObject[i].Attribute;
      vector3 c = {(float)((uint16>>0) & 0x1F) / 31.0f,(float)((uint16>>5) & 0x1F) / 31.0f,(float)((uint16>>10)& 0x1F) / 31.0f};
      //m_colors.push_back(c);
      m_colors[i3] = c;
      if (m_Texture>=0) {
        m_UV[i3].Vertex[0] = m_vertices[i3].Vertex[0] / xf;
        m_UV[i3].Vertex[1] = m_vertices[i3].Vertex[1] / hf;
      }

#else
      m_StlObject[i].Vertex[i2+0] = v.Vertex[0];
      m_StlObject[i].Vertex[i2+1] = v.Vertex[1];
      m_StlObject[i].Vertex[i2+2] = v.Vertex[2];
#endif

    }
    GenerateGlBuffer();

    m_Xmin = m_Xmin * 100.0 + m_S.X;
    m_Xmax = m_Xmax * 100.0 + m_S.X;

    float to = m_Ymin * 100.0;
    float bo = m_Ymax * 100.0;
    float fr = m_Zmin * 100.0;
    float ba = m_Zmax * 100.0;


    if (YUpp==1) {
      m_Zmin = to +m_S.Z-(m_Ymin*100);
      m_Zmax = bo +m_S.Z-(m_Ymin*100);
      m_Ymin = -ba+m_S.Y;
      m_Ymax = -fr+m_S.Y;
    } else if (YUpp==3) {
      m_Ymin = fr +m_S.Y;
      m_Ymax = ba +m_S.Y;
      m_Zmin = bo +m_S.Z;
      m_Zmax = to +m_S.Z;
    } else {
      m_Ymin = to +m_S.Y;
      m_Ymax = bo +m_S.Y;
      m_Zmin = fr +m_S.Z;
      m_Zmax = ba +m_S.Z;
    }

    if (m_Xmin < gObjectManager->MinMaxleft)   gObjectManager->MinMaxleft  = m_Xmin;
    if (m_Xmax > gObjectManager->MinMaxright)  gObjectManager->MinMaxright = m_Xmax;
    if (m_Ymin < gObjectManager->MinMaxtop)    gObjectManager->MinMaxtop   = m_Ymin ;
    if (m_Ymax > gObjectManager->MinMaxbottom) gObjectManager->MinMaxbottom= m_Ymax;
    if (m_Zmin< gObjectManager->MinMaxfront)   gObjectManager->MinMaxfront = m_Zmin;
    if (m_Zmax > gObjectManager->MinMaxback)   gObjectManager->MinMaxback  = m_Zmax;

#ifdef USE_SHADER
    if (m_StlObject) free(m_StlObject);
    m_StlObject=NULL;
#endif
    return 1;
  }
  return 0;
}
void CStlObject::GenerateGlBuffer()
{
  if (m_NumStlObjects) {
#ifdef USE_SHADER
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects * sizeof(float) * 9, &m_vertices[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects * sizeof(float) * 9, &m_colors[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_NormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects* sizeof(float) * 9, &m_normals[0].Vertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects* sizeof(float) * 6, &m_UV[0].Vertex[0], GL_STATIC_DRAW);
#endif
  }
}

int CStlObject::SetBufferSize(int NumPoints)
{
#ifdef USE_SHADER
  m_vertices.reserve(NumPoints *3);
  m_vertices.resize(NumPoints *3);
  m_normals.reserve(NumPoints *3);
  m_normals.resize(NumPoints *3);
  m_colors.reserve(NumPoints *3);
  m_colors.resize(NumPoints *3);
  m_UV.reserve(NumPoints *3);
  m_UV.resize(NumPoints *3);
#endif
  return 1; //m_NumStlObjects=0;
}

//-------------------------------------------------------------
void CStlObject::ExecuteDrawObject(int mode)
{
  float Drehung  =  (float)(m_Winkel * M_PI / 180.0);
  ClipperLib::cInt BX = m_S.X;
  ClipperLib::cInt BY = m_S.Y;
  unsigned short uint16 = 0x8000;
  glEnd();

#ifdef USE_SHADER
  if (0) {
#else
  if (1) {
#endif
    if (m_Path == NULL || m_NumStlObjects==0) return;
    size_t i = 0;
    for (; i<m_NumStlObjects; ++i) {

      if (uint16 != m_StlObject[i].Attribute) {
        uint16 = m_StlObject[i].Attribute;
        float r   = (float)((uint16>>0) & 0x1F) / 31.0;
        float g   = (float)((uint16>>5) & 0x1F) / 31.0;
        float b   = (float)((uint16>>10)& 0x1F) / 31.0;
        GLfloat mat_diffuse[4];
        mat_diffuse[0] = r;//*gGrayOut;
        mat_diffuse[1] = g;//*gGrayOut;
        mat_diffuse[2] = b;//*gGrayOut;
        glColor4f(r,g,b,1.0);

        mat_diffuse[3] = gAlphaTransp;

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,mat_diffuse);
        mat_diffuse[0] = 0;
        mat_diffuse[1] = 0;
        mat_diffuse[2] = 0;
        glMaterialfv(GL_FRONT, GL_SHININESS,mat_diffuse);
        glMaterialf(GL_FRONT, GL_SHININESS,LightColor[2]*20.0);
      }

      glBegin(GL_TRIANGLES);

      glNormal3fv(m_StlObject[i].Normale);
      glVertex3fv(&m_StlObject[i].Vertex[0]);
      glVertex3fv(&m_StlObject[i].Vertex[3]);
      glVertex3fv(&m_StlObject[i].Vertex[6]);
      glEnd();
    }
  } else if (m_NumStlObjects>2 ) {
    //glGenBuffers  : already done 

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects * sizeof(float) * 9, &m_vertices[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects * sizeof(float) * 9, &m_colors[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects* sizeof(float) * 9, &m_normals[0].Vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
      1,//NormalAttribute,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    if (m_Texture>=0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,rgTextureData[m_Texture].glTexture); //Texture);
      glUniform1i(TextureParameter,1);
      GLint v = 0;
      glGetIntegerv(GL_ACTIVE_TEXTURE,&v);

      glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
      glBufferData(GL_ARRAY_BUFFER, m_NumStlObjects* sizeof(float) * 6, &m_UV[0].Vertex[0], GL_STATIC_DRAW);
      glVertexAttribPointer(
        3,//UVAttribute
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
      );
    } else {   
      glUniform1i(TextureParameter,0);
    }

    glEnableVertexAttribArray(positionAttribute);
    glEnableVertexAttribArray(ColorAttribute);
    glEnableVertexAttribArray(NormalAttribute);
    if (m_Texture>=0) {
      glEnableVertexAttribArray(UVAttribute);
    }



    glUniform4f(uFixColor,0,0,0,0);
    glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,m_Alpha);// gAlphaTransp);
    glDrawArrays(GL_TRIANGLES, 0,m_NumStlObjects*3);
    if (gAlphaTransp<0.2) {
      glUniform4f(uColor,gGrayOut, gGrayOut, gGrayOut,1);
      glUniform4f(uFixColor,0,0,0,1);
      glLineWidth(2);
      glDrawArrays(GL_LINES, 0, m_NumStlObjects*3);
    }
    glDisableVertexAttribArray(positionAttribute);
    glDisableVertexAttribArray(NormalAttribute);
    glDisableVertexAttribArray(ColorAttribute);
    if (m_Texture>=0) {
      glDisableVertexAttribArray(UVAttribute);
    }
  }
}
//-------------------------------------------------------------

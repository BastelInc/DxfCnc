#if !defined(AFX_GCODE_OBJECT_H_INCLUDED_)
#define AFX_GCODE_OBJECT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000
// typedef  int ClipperLib::cInt;
#include "Layers.h"
#include "basteltype.h"
//#include <FL/gl.h>

#include "Object.h"
#include "clipper.h"
#define NUMSYM 250
#define USE_GLEW
#if defined(WIN32)
#define GLEW_STATIC 1
#endif
//#include <GL/glew.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <ostream>
#include <set>
#include <stdexcept>
#include <vector>

extern int gnumSelect;
extern int gDreiDFace;
extern int bShowNormale;
extern int bArrangeZoomCenter;
extern int bYUpp;
extern int savebinary;
extern DWORD NumVertextoSave;

// typedef char * LPSTR;
// typedef uint32_t DWORD;
extern int AktTiefe;
extern unsigned int gFarbe256[256];
extern LAYERTYPE WorkLayer;
extern char szWorkLayer[256];
extern float gGrayOut;
extern float gAlphaTransp;
extern int gDreiDFace;
extern int gNAuswahl;

extern float fxAufloesung;
class CObjectManager;

typedef  unsigned int uint32_t;
typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;

struct GcodePoint
{
  int A;
  int X;
  int Y;
  int Z;
  uint32_t N;
  uint32_t FPos;
  uint8_t  S;  // Laser power
  uint8_t  P;
  uint16_t Gcode;
  uint16_t mmSec100;
  GcodePoint(int a = 0, int x = 0, int y = 0, int z = 0, int pos = 0) : A(a), X(x), Y(y), Z(z), FPos(pos), S(0), P(0), Gcode(0), mmSec100(0){};

  friend inline bool operator==(const GcodePoint& a, const GcodePoint& b)
  {
      return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.A == b.A;
    //return a.X == b.X && a.Y == b.Y;
  }
//  friend inline bool operator!=(const GcodePoint& a, const GcodePoint& b)
//  {
//    return a.X != b.X || a.Y != b.Y;
//  }
//
//  // friend inline bool operator <(const GcodePoint& p) const {
//  friend inline bool operator<(const GcodePoint& a, const GcodePoint& b)
//  {
//    return (a.X < b.X) || ((a.X == b.X) && (a.Y < b.Y));
//  }
};
//------------------------------------------------------------------------------

typedef std::vector<GcodePoint> gcPath;

inline gcPath& operator<<(gcPath& poly, const GcodePoint& p)
{
  poly.push_back(p);
  return poly;
}

class CGCodeLineObject : public CObject
{  // CPolyLineObject {
 public:
//  int m_G0Feddrate;
//  int m_G1Feddrate;
  gcPath m_Path;
  int m_SelectIx;

  CGCodeLineObject(CObjectManager* ObjSource,
                   ClipperLib::cInt x,
                   ClipperLib::cInt y,
                   ClipperLib::cInt z,
                   ClipperLib::cInt a,
                   const char* lpLayer,
                   unsigned int color);
  virtual ~CGCodeLineObject();
  virtual void AddVertex   (ClipperLib::cInt x, ClipperLib::cInt y, ClipperLib::cInt z,ClipperLib::cInt a,int S,int Gcode,int mmSec100, int FPos,int Nmr);
  virtual void ExecuteDrawObject(int mode);
};

#endif

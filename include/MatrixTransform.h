#if !defined(MATRIXRASFORM_H__2DC9C020_3119_47AA_8F0D_14ACAEFBCDD1__INCLUDED_)
#define MATRIXRASFORM_H__2DC9C020_3119_47AA_8F0D_14ACAEFBCDD1__INCLUDED_

//#include <GL/glew.h>


typedef float tMatrix4x4[4][4];
void normalize(float * x,float * y,float * z);
void cross3(float * result,float * x,float * y);
float dot(float * x, float * y);
void Rotate(tMatrix4x4 * result, tMatrix4x4 * mx,float angle,  float axisx ,float axisy, float axisz);
void Scale (tMatrix4x4 * result, tMatrix4x4 * mx,float zoomx,float zoomy,float zoomz);
void perspective(tMatrix4x4 * result, float  rad ,float  aspect,float  zNear,float  zFar);
void Mul4x4(tMatrix4x4 * result, tMatrix4x4 * mx1,tMatrix4x4 * mx2);
void Translate(tMatrix4x4 * result, tMatrix4x4 * mx,float ox ,float oy, float oz);
void lookAt(tMatrix4x4 * result, float * eye,float * center,float * up);
void eulerAngleXYZ(tMatrix4x4 * result,float t1 ,float t2, float t3);

#endif

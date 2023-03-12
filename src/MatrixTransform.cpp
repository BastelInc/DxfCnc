 

#include <math.h>
#include <string.h>
#include "MatrixTransform.h"


//-------------------------------------------------------------
void normalize(float * x,float * y,float * z) {
   float isqr = 1.0f /  sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
   *x = (*x) * (isqr);
   *y = (*y) * (isqr);
   *z = (*z) * (isqr);
}

float inversesqrt(float   x) {
  return 1.0f / (float)sqrt(x);
}

/*
float crossX (float * xx,float * xy,float * xz,float * yx,float * yy,float * yz) {
	return (*xy) * (*yz) - (*yy) * (*xz);
}
float crossY (float * xx,float * xy,float * xz,float * yx,float * yy,float * yz) {
	return (*xz) * (*yx) - (*yz) * (*xx);
}
float crossZ (float * xx,float * xy,float * xz,float * yx,float * yy,float * yz) {
	return (*xx) * (*yy) - (*yx) * (*xy);
}
*/

//-------------------------------------------------------------
void Rotate(tMatrix4x4 * result, tMatrix4x4 * mx,float angle,  float axisx ,float axisy, float axisz) {
  float  anglex = angle;
  float  c = (float)cos(anglex);
  float  s = (float)sin(anglex);
  float  temp[3];
  float  axis[3] = {axisx,axisy,axisz};
  tMatrix4x4 Rotate;
  tMatrix4x4 Result;
  tMatrix4x4 m ;
  memset(&Result,0,sizeof(Result));
  memcpy(&m,mx,sizeof(tMatrix4x4 ));

  normalize(&axisx,&axisy,&axisz);
  axis[0] = axisx;
  axis[1] = axisy;
  axis[2] = axisz;

  temp[0] =  ((1.0f - c) * axisx);
  temp[1] =  ((1.0f - c) * axisy);
  temp[2] =  ((1.0f - c) * axisz);

  Rotate[0][0] = c + temp[0] * axis[0];
  Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
  Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

  Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
  Rotate[1][1] = c + temp[1] * axis[1];
  Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

  Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
  Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
  Rotate[2][2] = c + temp[2] * axis[2];

  Result[0][0] = m[0][0] * Rotate[0][0] + m[1][0] * Rotate[0][1] + m[2][0] * Rotate[0][2] ;
  Result[0][1] = m[0][1] * Rotate[0][0] + m[1][1] * Rotate[0][1] + m[2][1] * Rotate[0][2] ;
  Result[0][2] = m[0][2] * Rotate[0][0] + m[1][2] * Rotate[0][1] + m[2][2] * Rotate[0][2] ;
  Result[1][0] = m[0][0] * Rotate[1][0] + m[1][0] * Rotate[1][1] + m[2][0] * Rotate[1][2] ;
  Result[1][1] = m[0][1] * Rotate[1][0] + m[1][1] * Rotate[1][1] + m[2][1] * Rotate[1][2] ;
  Result[1][2] = m[0][2] * Rotate[1][0] + m[1][2] * Rotate[1][1] + m[2][2] * Rotate[1][2] ;
  Result[2][0] = m[0][0] * Rotate[2][0] + m[1][0] * Rotate[2][1] + m[2][0] * Rotate[2][2] ;
  Result[2][1] = m[0][1] * Rotate[2][0] + m[1][1] * Rotate[2][1] + m[2][1] * Rotate[2][2] ;
  Result[2][2] = m[0][2] * Rotate[2][0] + m[1][2] * Rotate[2][1] + m[2][2] * Rotate[2][2] ;
  Result[3][0] = m[3][0];
  Result[3][1] = m[3][1];
  Result[3][2] = m[3][2];
  Result[3][3] = m[3][3];

  memcpy(result,&Result,sizeof(tMatrix4x4 ));

  return ;
}



void Scale (tMatrix4x4 * result, tMatrix4x4 * mx,float zoomx,float zoomy,float zoomz)
{
  tMatrix4x4 Result;
  tMatrix4x4 m ;
  memset(&Result,0,sizeof(Result));
  memcpy(&m,mx,sizeof(tMatrix4x4 ));

  Result[0][0] = m[0][0] * zoomx;
  Result[0][1] = m[0][1] * zoomx;
  Result[0][2] = m[0][2] * zoomx;
  Result[1][0] = m[1][0] * zoomy;
  Result[1][1] = m[1][1] * zoomy;
  Result[1][2] = m[1][2] * zoomy;
  Result[2][0] = m[2][0] * zoomz;
  Result[2][1] = m[2][1] * zoomz;
  Result[2][2] = m[2][2] * zoomz;
  Result[3][0] = m[3][0];
  Result[3][1] = m[3][1];
  Result[3][2] = m[3][2];
  Result[3][3] = m[3][3];

  memcpy(result,&Result,sizeof(tMatrix4x4 ));

/*
		Result[0] = m[0] * v[0];
		Result[1] = m[1] * v[1];
		Result[2] = m[2] * v[2];
		Result[3] = m[3];
		return Result;
*/
}
/*
		GLM_FUNC_QUALIFIER static T call(detail::tvec3<T, P> const & x, detail::tvec3<T, P> const & y)
		{
			detail::tvec3<T, P> tmp(x * y);
			return tmp.x + tmp.y + tmp.z;
		}
*/

void perspective(tMatrix4x4 * result, float  rad ,float  aspect,float  zNear,float  zFar) {
  tMatrix4x4 Result;
  float tanHalfFovy = (float)tan(rad / 2.0f);
  memset(&Result,0,sizeof(Result));
  if (aspect) {
    Result[0][0] = 1.0f / (aspect * tanHalfFovy);
    Result[1][1] = 1.0f / (tanHalfFovy);
    Result[2][2] = - (zFar + zNear) / (zFar - zNear);
    Result[2][3] = - 1.0f;
    Result[3][2] = - (2.0f * zFar * zNear) / (zFar - zNear);
//    Result[2][3] = 1.0f;
  } else {
    Result[0][0] = 1.0f;
    Result[1][1] = 1.0f;
    Result[2][2] = 1.0f;
    Result[2][3] = 1.0f;
  }
  memcpy(result,&Result,sizeof(tMatrix4x4 ));

}


void cross3(float * result,float * x,float * y) {
  result[0] =  (x[1]) * (y[2]) - (y[1]) * (x[2]);
  result[1] =  (x[2]) * (y[0]) - (y[2]) * (x[0]);
  result[2] =  (x[0]) * (y[1]) - (y[0]) * (x[1]);
}

float dot(float * x, float * y) {
  float tmp[3] = {x[0] * y[0],x[1] * y[1],x[2] * y[2]};
  return tmp[0] + tmp[1] + tmp[2];
}


void lookAt(tMatrix4x4 * result, float * eye,float * center,float * up) {
  tMatrix4x4 Result;
  memset(&Result,0,sizeof(Result));
  Result[3][3]=1;
//  float f[3];  //(normalize(center - eye));
  //float  f[3] = {*center - *eye,*(center+1) - *(eye+1),*(center+2) - *(eye+2)};

  float  f[3] = {center[0] - eye[0],center[1] - eye[1],center[2] - eye[2]};
  normalize(&f[0],&f[1],&f[2]);

  float s[3];		//detail::tvec3<T, P> s(normalize(cross(f, up)));
  cross3(s,f,up);
  normalize(&s[0],&s[1],&s[2]);

  
  float u[3];		//detail::tvec3<T, P> u(cross(s, f));
  cross3(u,s,f);
		Result[0][0] = s[0];
		Result[1][0] = s[1];
		Result[2][0] = s[2];
		Result[0][1] = u[0];
		Result[1][1] = u[1];
		Result[2][1] = u[2];
		Result[0][2] =-f[0];
		Result[1][2] =-f[1];
		Result[2][2] =-f[2];
		Result[3][0] =-dot(s, eye);
		Result[3][1] =-dot(u, eye);
		Result[3][2] = dot(f, eye);

  memcpy(result,&Result,sizeof(tMatrix4x4 ));
  

/*
	GLM_FUNC_QUALIFIER detail::tmat4x4<T, P> lookAt
	(
		detail::tvec3<T, P> const & eye,
		detail::tvec3<T, P> const & center,
		detail::tvec3<T, P> const & up
	)
	{
		detail::tvec3<T, P> f(normalize(center - eye));
		detail::tvec3<T, P> s(normalize(cross(f, up)));
		detail::tvec3<T, P> u(cross(s, f));

		detail::tmat4x4<T, P> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] =-f.x;
		Result[1][2] =-f.y;
		Result[2][2] =-f.z;
		Result[3][0] =-dot(s, eye);
		Result[3][1] =-dot(u, eye);
		Result[3][2] = dot(f, eye);
		return Result;
	}

*/
}

//		Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
//		Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
//		Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
//		Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];

void Mul4x4(tMatrix4x4 * result, tMatrix4x4 * mx1,tMatrix4x4 * mx2) {
  tMatrix4x4 Result;
  tMatrix4x4 m1 ;
  tMatrix4x4 m2 ;
  memset(&Result,0,sizeof(Result));
  memcpy(&m1,mx1,sizeof(tMatrix4x4 ));
  memcpy(&m2,mx2,sizeof(tMatrix4x4 ));

  Result[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3] ;
  Result[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3] ;
  Result[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3] ;
  Result[0][3] = m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3] ;

  Result[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3] ;
  Result[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3] ;
  Result[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3] ;
  Result[1][3] = m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3] ;

  Result[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3] ;
  Result[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3] ;
  Result[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3] ;
  Result[2][3] = m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3] ;

  Result[3][0] = m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3] ;
  Result[3][1] = m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3] ;
  Result[3][2] = m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3] ;
  Result[3][3] = m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3] ;


  memcpy(result,&Result,sizeof(tMatrix4x4 ));

  return ;
}

void Translate(tMatrix4x4 * result, tMatrix4x4 * mx,float ox ,float oy, float oz) {
  tMatrix4x4 Result;
  tMatrix4x4 m ;
  memcpy(&Result,mx,sizeof(tMatrix4x4 ));
  memcpy(&m,mx,sizeof(tMatrix4x4 ));
//		Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];

  Result[3][0] = m[0][0] * ox + m[1][0] * oy + m[2][0] * oz + m[3][0];
  Result[3][1] = m[0][1] * ox + m[1][1] * oy + m[2][1] * oz + m[3][1];
  Result[3][2] = m[0][2] * ox + m[1][2] * oy + m[2][2] * oz + m[3][2];
  Result[3][3] = m[0][3] * ox + m[1][3] * oy + m[2][3] * oz + m[3][3];


  memcpy(result,&Result,sizeof(tMatrix4x4 ));
  return ;
}
	

void eulerAngleXYZ(tMatrix4x4 * result,float t1 ,float t2, float t3) {
  tMatrix4x4 Result;
  memset(&Result,0,sizeof(tMatrix4x4 ));
        float c1 = cos(-t1);
        float c2 = cos(-t2);
        float c3 = cos(-t3);
        float s1 = sin(-t1);
        float s2 = sin(-t2);
        float s3 = sin(-t3);
        
        Result[0][0] = c2 * c3;
        Result[0][1] =-c1 * s3 + s1 * s2 * c3;
        Result[0][2] = s1 * s3 + c1 * s2 * c3;
        Result[0][3] = 0.0f;
        Result[1][0] = c2 * s3;
        Result[1][1] = c1 * c3 + s1 * s2 * s3;
        Result[1][2] =-s1 * c3 + c1 * s2 * s3;
        Result[1][3] = 0.0f;
        Result[2][0] =-s2;
        Result[2][1] = s1 * c2;
        Result[2][2] = c1 * c2;
        Result[2][3] = 0.0f;
        Result[3][0] = 0.0f;
        Result[3][1] = 0.0f;
        Result[3][2] = 0.0f;
        Result[3][3] = 1.0f;
  memcpy(result,&Result,sizeof(tMatrix4x4 ));
  }
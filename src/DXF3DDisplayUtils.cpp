#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <vector>
#include <iostream>
#include <fstream>
//#include <algorithm>
#include <sstream>
//#if defined(WIN32)
#define GLEW_STATIC 1
//#endif
#include <GL/glew.h>
#include <stdarg.h>
#include "Object.h"

//-------------------------------------------------------------
GLuint loadBMP_custom(int index){//const char * imagepath){

    if (rgTextureData[index].szFilename == NULL) return 0;
    
	printf("Reading image %s\n", rgTextureData[index].szFilename);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;
    char filepath[256];
	// Open the file
	FILE * file = fopen(rgTextureData[index].szFilename,"rb");
	if (!file){
      strcpy(filepath,gObjectManager->m_PfadName);
      strcat(filepath,"/");
      strcat(filepath,rgTextureData[index].szFilename);
      file = fopen(filepath,"rb");
      if (!file){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n",filepath);
		return 0;
      }
      free(rgTextureData[index].szFilename);
      rgTextureData[index].szFilename = strdup(filepath);
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){
      printf("Not a correct BMP file\n");
      fclose(file);
	  return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
	  printf("Not a correct BMP file\n");
      fclose(file);
      return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {
      printf("Not a correct BMP file\n");    
      fclose(file); 
      return 0;
    }
	if ( *(int*)&(header[0x1C])!=24 )         {
      printf("Not a correct BMP file\n");    
      fclose(file); 
      return 0;
    }

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file can be closed.
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
//glGenerateMipmap(GL_TEXTURE_2D);
	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// ... nice trilinear filtering ...
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
    // ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);
    rgTextureData[index].xmmFaktor = width;
    rgTextureData[index].ymmFaktor = height;
    rgTextureData[index].glTexture = textureID;
	// Return the ID of the texture we just created
	return textureID;
}

#ifdef DDS
// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library,
// or do it yourself (just like loadBMP_custom and loadDDS)
//GLuint loadTGA_glfw(const char * imagepath){
//
//	// Create one OpenGL texture
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Read the file, call glTexImage2D with the right parameters
//	glfwLoadTexture2D(imagepath, 0);
//
//	// Nice trilinear filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	// Return the ID of the texture we just created
//	return textureID;
//}



#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);


	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch(fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width  /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	}

	free(buffer);

	return textureID;


}
#endif


const char * vertexShader={
"#version 330 core\n"\

// Input vertex data, different for all executions of this shader.
"layout(location = 0) in vec3 vertexPosition_modelspace;\n"\
"layout(location = 1) in vec3 vertexNormal_modelspace;\n"\
"layout(location = 2) in vec3 VertexColor;\n"\
"layout(location = 3) in vec2 VertexUV;\n"\

// Output data ; will be interpolated for each fragment.
"out vec3 ColorRGB;\n"\
"out vec2 UV;\n"\
"out vec3 Position_worldspace;\n"\
"out vec3 Normal_cameraspace;\n"\
"out vec3 EyeDirection_cameraspace;\n"\
"out vec3 LightDirection_cameraspace;\n"\

// Values that stay constant for the whole mesh.
"uniform mat4 MVP;\n"\
"uniform mat4 V;\n"\
"uniform mat4 M;\n"\
"uniform vec3 LightPosition_worldspace;\n"\
"uniform vec3 Offset_modelspace;\n"\
"void main(){\n"\

	// Output position of the vertex, in clip space : MVP * position
	"gl_Position =  MVP * vec4(vertexPosition_modelspace+Offset_modelspace,1);\n"\
	
	// Position of the vertex, in worldspace : M * position
	"Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;\n"\
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;\n"\
	"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"\

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	"vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;\n"\
	"LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;\n"\
	
	// Normal of the the vertex, in camera space
    // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
    "Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;\n"\
	
	// UV of the vertex. No special space for this one.
    "ColorRGB = VertexColor;\n"\
    "UV = VertexUV;\n"\
"}\n"

};  


const char * fragmendShader={
"#version 330 core\n"\
// Interpolated values from the vertex shaders
"in vec2 UV;\n"\
"in vec3 ColorRGB;\n"\
"in vec3 Position_worldspace;\n"\
"in vec3 Normal_cameraspace;\n"\
"in vec3 EyeDirection_cameraspace;\n"\
"in vec3 LightDirection_cameraspace;\n"\

// Ouput data
//out vec3 color;
"out vec4 color;\n"\

// Values that stay constant for the whole mesh.
"uniform sampler2D TextureSampler;\n"\
"uniform mat4 MV;\n"\
"uniform vec3 LightPosition_worldspace;\n"\
"uniform vec4 uColor;\n"\
"uniform vec4 uFixColor;\n"\
"vec3 MaterialDiffuseColorTx;\n"\
"uniform int  parameterValue;\n"\

"void main(){\n"\

	// Light emission properties
	// You probably want to put them as uniforms
	"vec3 LightColor =  vec3(1,1,1); ;//uColor.rgb*2.0;// vec3(1,1,1);\n"\
	"float LightPower = 3.0f;\n"\
	
	// Material properties
	"vec3 MaterialDiffuseColor = ColorRGB;\n"\
	"if (parameterValue>0) MaterialDiffuseColorTx = texture( TextureSampler, UV ).rgb;\n"\
	"else MaterialDiffuseColorTx = ColorRGB/3.0;//vec3(0,0,0);//\n"\
	
    "vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;\n"\
	"vec3 MaterialSpecularColor =  vec3(0.5,0.5,0.5);\n"\

	// Distance to the light
	"float distance = length( LightPosition_worldspace - Position_worldspace );\n"\

	// Normal of the computed fragment, in camera space
	"vec3 n = normalize( Normal_cameraspace );\n"\
	// Direction of the light (from the fragment to the light)
	"vec3 l = normalize( LightDirection_cameraspace );\n"\
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	"float cosTheta = clamp( dot( n,l ), 0,1 );\n"\
	
	// Eye vector (towards the camera)
	"vec3 E = normalize(EyeDirection_cameraspace);\n"\
	// Direction in which the triangle reflects the light
	"vec3 R = reflect(-l,n);\n"\
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	"float cosAlpha = clamp( dot( E,R ), 0,1 );\n"\
	"if (uFixColor.a > 0.1) {\n"\
	// Ohne Color Buffer
    "  color = uFixColor;\n"\
	"} else {\n"\

	
	"color.rgb = ColorRGB / 3.0 +\n"\
		// Ambient : simulates indirect lighting
	"	MaterialAmbientColor +\n"\
		// Diffuse : "color" of the object
    "   MaterialDiffuseColorTx/2 + "\
	"	(MaterialDiffuseColor) * LightColor * LightPower * cosTheta / (distance*distance) +\n"\
		// Specular : reflective highlight, like a mirror
	"	MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);\n"\
    "color.a    = uColor.a;  // 0.81;\n"\
    "}\n"\
"}\n"
};

int LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
static int once = 2;
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

//  int Mslv, mslv; // major and minor version numbers of the shading language
//  char str[200];
//  char * pnt = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
//  if (pnt) {
//  //strncpy(str,,200);
//  sscanf(pnt, "%d.%d", &Mslv, &mslv);
//  //add_output("Shading Language Version=%d.%d\n",Mslv, mslv);
//  } else {
//    //return 0;
//    Mslv=3;
//    mslv=30;
//  }
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << VertexShaderStream.rdbuf();
    VertexShaderCode = sstr.str();
    VertexShaderStream.close();
  } else {
    if (once) { 
      once --;
      printf("Impossible to open %s. Are you in the right directory ?\nLoad hard-coded Shader !\n", vertex_file_path);
    }
  //  getchar();
  //  return 0;
    VertexShaderCode = vertexShader;
  }


  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << FragmentShaderStream.rdbuf();
    FragmentShaderCode = sstr.str();
    FragmentShaderStream.close();
  } else {
    FragmentShaderCode = fragmendShader;
  }


  GLint Result = GL_FALSE;
  int InfoLogLength;


  // Compile Vertex Shader
  //printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
//  char fvs_string[5000];const char *fvs = fvs_string ;
//  sprintf(fvs_string, VertexSourcePointer, Mslv, mslv);
//  glShaderSource(VertexShaderID, 1, &fvs, NULL);
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 1 ) {
    char VertexShaderErrorMessage[1000];
    if (InfoLogLength > 1000) InfoLogLength = 1000;
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
    printf("%s\n",VertexShaderErrorMessage);
  }

  // Compile Fragment Shader
  //printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
//  char ffs_string[5000]; const char *ffs = ffs_string;
//  //printf("Geht %s\n",FragmentSourcePointer);
//  sprintf(ffs_string, FragmentSourcePointer, Mslv, mslv);
//  glShaderSource(FragmentShaderID, 1, &ffs, NULL);


  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 1 ) {
    char FragmentShaderErrorMessage[1000];
    if (InfoLogLength > 1000) InfoLogLength = 1000;
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
    printf("%s\n", FragmentShaderErrorMessage);
  }



  // Link the program
  //printf("Linking program\n");
  GLuint  ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 1 ) {
    char ProgramErrorMessage[1000];
    if (InfoLogLength > 1000) InfoLogLength = 1000;
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
    printf("%s\n", ProgramErrorMessage);
  }


  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}


//-------------------------------------------------------------


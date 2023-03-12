//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

// BastelUtils.cpp: Implementierung der Klasse CBastelUtils.
//-------------------------------------------------------------

#include "basteltype.h"
#include "BastelUtils.h"

#ifdef WIN32
#include <direct.h>
#endif
char CBastelUtils::s_IniPath [_MAX_PATH];
//-------------------------------------------------------------
#ifndef WIN32
DWORD GetTickCount(void)
{
  timespec rltime;
  //    clock_gettime(CLOCK_REALTIME,&rltime);
  clock_gettime(CLOCK_MONOTONIC,&rltime);
  unsigned int MsTime = (rltime.tv_sec * 1000) + (rltime.tv_nsec / 1000000);// + 600000;
  return MsTime;
}
#endif
//-------------------------------------------------------------
CBastelUtils::CBastelUtils()
{
  memset(&m_IniFile,0,sizeof(m_IniFile));
  m_modify=0;
  m_size=0;
  m_content=0;
  m_holefile=NULL;
  m_size2=0;
  m_index2=0;
  m_SaveBuff=NULL;
}
//-------------------------------------------------------------
CBastelUtils::~CBastelUtils()
{

}

void CBastelUtils::OverwriteIniFileNmae(LPCSTR  lpfile)
{
  if (strcmp(m_IniFile,lpfile)){
    m_content=0;
  };
  strncpy(m_IniFile,lpfile,sizeof(m_IniFile));
}

//-------------------------------------------------------------
void CBastelUtils::SetIniFile(LPCSTR  lpfile)
{
  FILE *  f_IniFile;
  struct stat fileinfo;
//  if (m_size) free(m_holefile);
//  if (m_size2) free(m_SaveBuff);
//  m_size=0;
  m_content=0;
//  m_size2=0;
  if( stat(s_IniPath, &fileinfo ) < 0 ) {
#ifdef WIN32
    mkdir(s_IniPath);
#else
    mkdir(s_IniPath,0755);
#endif
  }
  char path[_MAX_PATH];
  strcpy(path,s_IniPath);
  strcat(path,"/");
  strcat(path,lpfile);

  f_IniFile = fopen(path, "rt");
  if(f_IniFile) {
    fclose(f_IniFile);
    strcpy(m_IniFile,path);
    OutputDebugInfo ODebug(DEBUG_INFO,m_IniFile);
    return;
  }

  // create it!
  {
    char *has_path;
    has_path = strrchr(path, '/');
    if (has_path==NULL) {
      getcwd(m_IniFile, sizeof(m_IniFile));
      strcat(m_IniFile,"/");
      strcat(m_IniFile,lpfile);
    } else {
      strcpy(m_IniFile,path);
    }
    f_IniFile = fopen(m_IniFile, "wt");
    if(f_IniFile) {
      fclose(f_IniFile);
    }
  }
  OutputDebugInfo ODebug(DEBUG_INFO,m_IniFile);
}
//-------------------------------------------------------------
int CBastelUtils::GetProfileInt(LPCSTR  appname, LPCSTR  valuename, int uDefault)
{
  char str[40];
  unsigned int Val = uDefault;
  if (GetProfileString(appname,valuename,"",str,sizeof(str))) Val = atoi(str);
  return Val;
}
//-------------------------------------------------------------
void CBastelUtils::WriteProfileInt(LPCSTR  appname, LPCSTR  valuename, int uValue)
{
  char str[40];
  sprintf(str,"%d",uValue);
  WriteProfileString(appname,valuename,str);
}


//-------------------------------------------------------------
int  CBastelUtils::ReadIniFile()
{
  struct stat fileinfo;
  int res =0;
  if( stat(m_IniFile, &fileinfo ) >= 0 ) {
    m_content = fileinfo.st_size;
    if (m_content >= m_size) {
      m_size = 256 + m_content;
      if (m_holefile) {
        m_holefile =  (char * )realloc(m_holefile,m_size);
      } else {
        m_holefile =  (char * )malloc(m_size);
      }
    }
    FILE *  f_IniFile = fopen(m_IniFile, "r");
    if(f_IniFile) {
      res=fread(m_holefile,1,m_content,f_IniFile);
      if (res <= m_size) m_holefile[res]='\0';
      fclose(f_IniFile);
    }
    m_modify=0;
  }
  return res;
}
//-------------------------------------------------------------
int  CBastelUtils::SaveIniFile()
{
  FILE *  f_IniFile  = fopen(m_IniFile, "wt");
  //FILE *  f_IniFile  = fopen("/home/rolf/test.ini", "w");
  int res =0;
  if(f_IniFile) {
    res = fwrite(m_holefile,m_content,1,f_IniFile);
    fclose(f_IniFile);
  };
  return res;
}
//-------------------------------------------------------------
int  CBastelUtils::ExternModify() {
  m_content=0;
  return 0;
}
//-------------------------------------------------------------
int  CBastelUtils::FlipBuff() {
  int res =0;
  m_content = m_index2;
  m_index2  = 0;
  int s   = m_size;
  m_size  = m_size2;
  m_size2 = s;
  char * p = m_holefile;
  m_holefile = m_SaveBuff;
  m_SaveBuff = p;
  return res;
}

//-------------------------------------------------------------
int  CBastelUtils::ReadLine(char * buff,int len){
  int res =0;
  if (m_index<m_content) {
    char * pnt = strchr(&m_holefile[m_index],'\n');
    if (pnt==NULL){
      pnt = strchr(&m_holefile[m_index],'\0');
    }
    res = pnt - &m_holefile[m_index];
    if (res>0) {
      pnt++;
      len = _min(res,len-1);
      memcpy(buff,&m_holefile[m_index],len);
      buff[len]= '\0';
      m_index += res+1;
      if (*pnt < 20) m_index++;
      if (res<4) {
        res =0;
      }
    }
    else res=0;
  }
  //fprintf(stderr,"%s\n",buff);
  return res;
}

//-------------------------------------------------------------
int  CBastelUtils::WriteLine(char * buff,int len){
  int res =0;

  if ((m_index2 + len)< m_size2) {
    memcpy(&m_SaveBuff[m_index2],buff,len);
    m_index2+=len;
    m_SaveBuff[m_index2]='\0';
    res = len;
  }
  return res;
}

//-------------------------------------------------------------
unsigned int CBastelUtils::GetProfileString(LPCSTR  appname, LPCSTR  valuename, LPCSTR  pDefault, LPSTR   pResult, DWORD cbResult)
{
  int valuenameLen = strlen(valuename);
  int appnameLen   = strlen(appname);
  int found = false;
  strncpy(pResult,pDefault,cbResult);
  m_index=0;
  if (m_content || ReadIniFile()) {
//  FILE *  f_IniFile = fopen(m_IniFile, "rt");
//  if(f_IniFile) {
    BOOL bAppName=false;
    char buf[_MAX_PATH *2];
    //while (fgets (buf, sizeof (buf), f_IniFile)) {
    while (ReadLine(buf, sizeof (buf))) {
      if (buf[0] == '[') {
        bAppName=false;
        if (buf[1+appnameLen]==']') {
          if (strncmp (&buf[1], appname, appnameLen)==0) bAppName=true;
        }
      } else if (bAppName) {
        if (buf[valuenameLen]=='=') {
          if (strncmp (buf, valuename, valuenameLen)==0) {
            // Modify the string
            char * pend;
            pend = strrchr(&buf[valuenameLen], '\n');
            if (pend )*pend = '\0';
            strncpy(pResult,&buf[valuenameLen+1],cbResult);
            found = true;
            break;
          }
        }
      }
    }
//    fclose (f_IniFile );
  }
 // if (found == false)WriteProfileString(appname,valuename,pDefault);
  return strlen (pResult);
}
//-------------------------------------------------------------
void CBastelUtils::WriteProfileString(LPCSTR  appname, LPCSTR  valuename, LPCSTR  pData)
{
  int valuenameLen = strlen(valuename);
  int appnameLen   = strlen(appname);

  m_index2=0;
  m_index=0;
  int newsize = m_content+256;
  if (newsize >= m_size2) {
    m_size2 = newsize;
    if (m_SaveBuff) {
      m_SaveBuff =  (char * )realloc(m_SaveBuff,m_size2);
    } else {
      m_SaveBuff =  (char * )malloc(m_size2);
    }
  }





//  char tpath[_MAX_PATH+20];
//  strcpy(tpath,m_IniFile);
//  strcat(tpath,".tmp");
//
//  FILE *  f_IniFile = fopen(m_IniFile, "rt");
//  FILE *  ftemp     = fopen(tpath, "wt");
//  if(ftemp )     {
    BOOL bAppName=false;
    BOOL bValuetoSaved=false;
    BOOL NoAppfound=true;      // create a new applabel and add the value if it not exists
    int  res =0;
//    if (f_IniFile) {
      char buf[_MAX_PATH *2];
      char buf2[_MAX_PATH *2 +2];
      // copy records from the old file
      // while (res >= 0 && fgets (buf, sizeof (buf), f_IniFile)) {
      while (ReadLine(buf, sizeof (buf))) {

        if (buf[0] == '[') {
          if (bValuetoSaved) {
            // append to file
            bValuetoSaved = false;
            //res = fprintf (ftemp, "%s=%s\n",valuename,pData);
            res = sprintf (buf2, "%s=%s\n",valuename,pData);
            if (res>0) WriteLine(buf2,res);
          }
          bAppName=false;
          if (buf[1+appnameLen]==']') {
            if (strncmp (&buf[1], appname, appnameLen)==0) {
              bAppName      = true;
              bValuetoSaved = true;
              NoAppfound    = false;
            }
          }
        } else if (bAppName) {
          if (buf[valuenameLen]=='=') {
            if (strncmp (buf, valuename, valuenameLen)==0) {
              // Modify the string
              sprintf (&buf[valuenameLen], "=%s\n",pData);
              bValuetoSaved = false;
            }
          }
        }
        if (res >= 0) {
          res = sprintf (buf2, "%s\n", buf);
          if (res>0) WriteLine(buf2,strlen(buf2));
        }
      }
      if (bValuetoSaved||NoAppfound) {
        if (NoAppfound) {
          int res1 = sprintf (buf2,"[%s]\n", appname);
          if (res1>0) WriteLine(buf2,res1);
        }
        if (res >= 0)  {
          res = sprintf (buf2, "%s=%s\n",valuename,pData);
          if (res>0) WriteLine(buf2,res);
        }
      }
      //fclose(f_IniFile );
      //fclose(ftemp);
      if (m_index2 >= 3) {
        FlipBuff();
        //remove(m_IniFile);
        //rename(tpath, m_IniFile);
      }
//    } else fclose (ftemp);
//  } else if (f_IniFile ) fclose (f_IniFile );
}
//-------------------------------------------------------------
//unsigned char _ctype[] = {
//_C,_C,_C,_C,_C,_C,_C,_C,                    // 0-7
//_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,     // 8-15
//_C,_C,_C,_C,_C,_C,_C,_C,                    // 16-23
//_C,_C,_C,_C,_C,_C,_C,_C,                    // 24-31
//_S|_SP,_P,_P,_P,_P,_P,_P,_P,                // 32-39    !"#$%&'
//_P,_P,_P,_P,_P,_P,_P,_P,                    // 40-47   ()*+,-./
//_D,_D,_D,_D,_D,_D,_D,_D,                    // 48-55   01234567
//_D,_D,_P,_P,_P,_P,_P,_P,                    // 56-63   89:;<=>?
//_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,  // 64-71   @ABCBEFG
//_U,_U,_U,_U,_U,_U,_U,_U,                    // 72-79   HIJKLMNO
//_U,_U,_U,_U,_U,_U,_U,_U,                    // 80-87   PQRSTUVW
//_U,_U,_U,_P,_P,_P,_P,_U,//_P,                    // 88-95   XYZ[\]^_
//_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,  // 96-103  `abcdefg
//_L,_L,_L,_L,_L,_L,_L,_L,                    // 104-111 hijklmno
//_L,_L,_L,_L,_L,_L,_L,_L,                    // 112-119 pqrstuvw
//_L,_L,_L,_P,_P,_P,_P,_C,                    // 120-127 xyz{|}~
//0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            // 128-143 
//0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            // 144-159
//_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
//_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
//_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
//_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
//_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
//_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */


//-------------------------------------------------------------

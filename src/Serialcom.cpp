//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include <stdio.h>
#include "FltkWindow.h"
#include "Serialcom.h"
#ifdef WIN32

char buff[10]= {12};

char szMGMCommStatusText[64];
char      gMsgMonitor[60];
CRITICAL_SECTION SerialComm::s_critical_section;;

extern char      gCNCCode;
extern char      gCNC_HALT;
extern bool      gCNCEiche;
extern double  gCNCPos[4];
extern int       gOkCount;
int gNewPos=0;

void Sleep(int ms)
{
  usleep(ms*1000);
}
//--------------------------------------------------------------
//-------------------------------------------------------------
SerialComm::SerialComm(char * lpszrs)
{
  m_Devicerun=1;
  m_hCommx= INVALID_HANDLE_VALUE;
  m_hThread= NULL;
  m_ThreadID=0;
  memset(&osReadComm,0,sizeof(osReadComm));
  memset(&osWriteComm,0,sizeof(osWriteComm));
  name = "SerialComm";

  char str[20];
  int no = atoi(lpszrs+3);
  if (no>  0 && no < 10) {
    sprintf(str,"COM%d",no);
  } else {
    strcpy(str,"COM1");
  }
  DCB dDCB;

  m_hCommx =  CreateFile(str,GENERIC_READ|GENERIC_WRITE,
    0,   NULL,  OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
  if (m_hCommx== INVALID_HANDLE_VALUE) goto rs232IniFail;

  if (m_hCommx ==  INVALID_HANDLE_VALUE) goto rs232IniFail;

  dDCB.DCBlength = sizeof(DCB);
  GetCommState(m_hCommx,&dDCB);
  dDCB.fBinary  = true;
  dDCB.Parity   = NOPARITY;
  dDCB.ByteSize = 8;
  dDCB.StopBits = ONESTOPBIT;
  dDCB.BaudRate = 115200;

  if (!SetCommState((HANDLE)m_hCommx,&dDCB))  goto rs232IniFail;

  COMMTIMEOUTS  ct;
  memset(&ct,0,sizeof(ct));
  ct.ReadIntervalTimeout =       10;
  ct.ReadTotalTimeoutMultiplier= 1;
  ct.ReadTotalTimeoutConstant=   1;
  ct.WriteTotalTimeoutMultiplier=1;
  ct.WriteTotalTimeoutConstant =50;

  SetCommTimeouts((HANDLE)m_hCommx,&ct);
  if (!SetupComm((HANDLE)m_hCommx,2048,256))  goto rs232IniFail;
  osReadComm.hEvent  = CreateEvent( NULL,TRUE,  FALSE,NULL );
  osWriteComm.hEvent = CreateEvent( NULL,TRUE,  FALSE,NULL );

//xmc_device.DeviceEvent_cb(USB_USBD_EVENT_SOF);
  strcpy(szMGMCommStatusText,str);
  InitializeCriticalSection(&s_critical_section);
  m_hThread= CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) ReceiveThreadProc,this,0,&m_ThreadID);
  return;
rs232IniFail:
  m_hCommx =  INVALID_HANDLE_VALUE;
  m_Devicerun=0;
  return;

}

//---------------------------------------------------------
SerialComm::~SerialComm()
{
  EnterCriticalSection(&SerialComm::s_critical_section);
  strcpy(szMGMCommStatusText,"  COMM   : MGM Closed");
  if (m_hThread) {
    TerminateThread(m_hThread,1);
    for (int i=0; i< 10;i++) {
      DWORD dwexcode = -1;
      GetExitCodeThread(m_hThread,&dwexcode);
      if (dwexcode==1 || dwexcode!=STILL_ACTIVE) break;
      Sleep(50);
    }
    CloseHandle(m_hThread);
    m_hThread=NULL;
  }
  //  if (s_critical_section.LockCount)  DeleteCriticalSection(&s_critical_section);
  if (m_hCommx != INVALID_HANDLE_VALUE) {
    CloseHandle(m_hCommx);
    m_hCommx = INVALID_HANDLE_VALUE;
  }
  if (osReadComm.hEvent)  CloseHandle(osReadComm.hEvent ) ;
  if (osWriteComm.hEvent) CloseHandle(osWriteComm.hEvent ) ;

  m_hThread = NULL;
  LeaveCriticalSection(&SerialComm::s_critical_section);

}
//---------------------------------------------------------------
//---------------------------------------------------------------

BOOL SerialComm::SendMsg(unsigned char *bufout, int nout)
{
  if (nout) {
    gMsgMonitor[0]++;
    Send_Buff((unsigned char*)bufout,nout);
  }
  return 0;
}
//-------------------------------------------------------------
int SerialComm::Send_Buff(unsigned char * lBlock, int nLength)
{
  BOOL        fWriteStat ;
  DWORD xx;
  // create I/O event used for overlapped read
  fWriteStat = WriteFile(m_hCommx,lBlock,nLength,&xx,&osWriteComm);
  if (!fWriteStat) {
    if(GetLastError() == ERROR_IO_PENDING) {
      while(!GetOverlappedResult(m_hCommx,&osWriteComm, &xx, TRUE )) {
        DWORD dwError = GetLastError();
        if(dwError == ERROR_IO_INCOMPLETE) {
          continue;
        } else {
          m_Devicerun=0;
          break;
        }
      }
    }
  }
  return xx;
}
//-------------------------------------------------------------
int SerialComm::Get_Buff(unsigned char * lBlock, int nLength)
{
  DWORD    dwRead=0;
  BOOL       fReadStat ;
  DWORD      dwError;
  fReadStat = ReadFile( m_hCommx, lBlock, nLength, &dwRead, &osReadComm) ;
  if (!fReadStat)
  {
    dwError = GetLastError();
    if (dwError  == ERROR_IO_PENDING)
    {
      // We have to wait for read to complete.
      // This function will timeout according to the
      // CommTimeOuts.ReadTotalTimeoutConstant variable
      // Every time it times out, check for port errors
      while(!GetOverlappedResult(m_hCommx, &osReadComm, &dwRead, TRUE )) {
        dwError = GetLastError();
        //if(dwError == ERROR_IO_INCOMPLETE) continue;  // normal result if not finished
        break;                   // an error occurred, try to recover
      }
    }
  }
  return dwRead;
}

//-------------------------------------------------------------
// trick to use static function for threadloop
void * SerialComm::ReceiveThreadProc(void * lpThreadParameter)
{
  SerialComm * pCHID_MGM20 = (SerialComm *)lpThreadParameter;
  pCHID_MGM20->ReceiveLoop();
  return 0;
}
//-------------------------------------------------------------
int WaitFeedback=0;
void SerialComm::ReceiveLoop(void)
{
  DWORD Readbytes;
  unsigned char   m_NCBuff[64*2];
  char cmdword[64];
  memset(&m_NCBuff,0,sizeof(m_NCBuff));
  Readbytes=0;
  BOOL       fReadStat ;
  int serial_count=0;
  while (m_hThread && m_hCommx != INVALID_HANDLE_VALUE && m_Devicerun) {
    memset(&m_NCBuff,0,sizeof(m_NCBuff));
    Readbytes = Get_Buff((unsigned char*)&m_NCBuff[0],sizeof(m_NCBuff));
    int index=0;
    while (index < Readbytes) {
      char c = m_NCBuff[index++];
      bool eol = (c == '\n' || c == '\r');
      if (!eol && serial_count<sizeof(cmdword)) {
        cmdword[serial_count] = c;
        serial_count++;
      }
      if ((serial_count && eol) || serial_count >60 ) {
        cmdword[serial_count]='\0';
        if (serial_count!= 19) printf("%s\n",cmdword);
        if (serial_count==2) {
          if (cmdword[0]=='o' && cmdword[1]=='k') {
            gCNC_HALT=gCNCCode =  'H';
            gOkCount--;
            printf("2ok %d\n",gOkCount);
            //memmove(&CNCPos[0],&CNCPos[3],sizeof(CNCPos)-(sizeof(CNCPos[3])*3));
            gNewPos ++;
          }
        } else if (serial_count>=19) {
          int a=0;
          if (serial_count> 19) {
            a = atoi((char*)&cmdword[19]);
            cmdword[19]='\0';
          }
          int z = atoi((char*)&cmdword[13]);
          cmdword[13]='\0';
          int y = atoi((char*)&cmdword[7]);
          cmdword[7]='\0';
          int x = atoi((char*)&cmdword[1]);
          int c = cmdword[0];

          if (c=='H')   gCNC_HALT = c;
          gCNCCode    = c;
          gCNCPos[0] = x;
          gCNCPos[1] = y;
          gCNCPos[2] = z;
          gCNCPos[3] = a;
          if (c =='H')        gCNCEiche = true;
          else if (c =='I')   gCNCEiche = false;
          gNewPos ++;

          if (cmdword[0]=='O') {
            gOkCount--;
            printf("ok %d\n",gOkCount);
          }
        } else {
//          printf("-%s-",cmdword);
        }

        memset(&cmdword,0,sizeof(cmdword));
        serial_count=0;
      }
    }
  }
  strcpy(&szMGMCommStatusText[8]," : MGM Closed");
  m_hThread = 0;
}
#else

#include <termios.h>
#include <sys/poll.h>
//#include "resource.h"
termios options;
termios options_save;
char buff[10]= {12};

char szMGMCommStatusText[64];
char      gMsgMonitor[60];
pthread_mutex_t  SerialComm::s_critical_section;//PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP ;

#define WAIT_OBJECT_0   1
#define WAIT_TIMEOUT    0
#define WAIT_ABANDONED -1

extern char      gCNCCode;
extern char      gCNC_HALT;
extern bool      gCNCEiche;
extern double    gCNCPos[4];
extern int       gOkCount;
int gNewPos=0;
//  end hiddev.h
void Sleep(int ms)
{
  usleep(ms*1000);
}
//--------------------------------------------------------------

#define WAIT_OBJECT_0   1
#define WAIT_TIMEOUT    0
#define WAIT_ABANDONED -1

DWORD WaitForSingleObject(int * pEvent,int tout)
{
  tout = (tout+9) / 10;
  while (*pEvent==0 && tout--)
  {
    tout--;
    usleep(10000);
  }
  return tout ? WAIT_OBJECT_0:WAIT_TIMEOUT;
}
//--------------------------------------------------------------
void ResetDTR(int fd) // Setzt DTR auf OFF
{
  int currstat;
  ioctl(fd, TIOCMGET, &currstat);
  currstat &= ~TIOCM_DTR;
  ioctl(fd, TIOCMSET, &currstat);
}


void SetDTR(int fd) // Setzt DTR auf ON
{
  int currstat;
  ioctl(fd, TIOCMGET, &currstat);
  currstat |= TIOCM_DTR;
  ioctl(fd, TIOCMSET, &currstat);
}


void ResetRTS(int fd) // Setzt RTS auf OFF
{
  int currstat;
  ioctl(fd, TIOCMGET, &currstat);
  currstat &= ~TIOCM_RTS;
  ioctl(fd, TIOCMSET, &currstat);
}

void SetRTS(int fd) // Setzt RTS auf ON
{
  int currstat;
  ioctl(fd, TIOCMGET, &currstat);
  currstat |= TIOCM_RTS;
  ioctl(fd, TIOCMSET, &currstat);
}

//-------------------------------------------------------------
SerialComm::SerialComm(char * lpszrs)
{
  m_Devicerun=1;
  m_thread=0;
  m_hCommx= INVALID_HANDLE_VALUE;;
  name = "SerialComm";

  char str[32];
  int no = atoi(lpszrs+3);
  if (no== 9){
    sprintf(str,"/dev/ttyACM0");
  } else if (no==11){
    sprintf(str,"/dev/ttyACM1");//,no-10);
  } else if (no==10){
    sprintf(str,"/dev/ttyUSB0");//,no-10);
  } else if (no>  0 && no < 10)
  {
    sprintf(str,"/dev/ttyS%d",no-1);
  }
  else strcpy(str,"/dev/ttyS0");


  int fd=0; // Filedeskriptor
#ifdef WIN32
  fd = open (str, O_RDWR | O_NOCTTY | / *O_NONBLOCK | * /O_SYNC);
#else
  fd = open (str, O_RDWR | O_NOCTTY | /*O_NONBLOCK | */O_SYNC);
#endif // WIN32
  fcntl(fd, F_SETFL, 0);
  if (tcgetattr(fd, &options) != 0) goto HID_IniFail;
  if (1)
  {
    tcgetattr(fd, &options_save);

    bzero(&options, sizeof(options)); // Structure loeschen, ggf vorher sichern
    //  und bei Programmende wieder restaurieren
    // Alternativ:
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // setze Optionen
    options.c_cflag &= ~PARENB;         // kein Paritybit
    options.c_cflag &= ~CSTOPB;         // 1 Stoppbit
    options.c_cflag &= ~CSIZE;          // 8 Datenbits
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);// CD-Signal ignorieren
    //options.c_cflag |= CRTSCTS;         // enable Hardwareflow control
    options.c_cflag &= ~CRTSCTS;          // disable Hardwareflow control

    options.c_iflag &= ~(IXON|IXOFF);

    // Kein Echo, keine Steuerzeichen, keine Interrupts
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;          // setze "raw" Input
    options.c_cc[VMIN]  = 0;            // warten auf min. 0 Zeichen
    options.c_cc[VTIME] = 0;            // Timeout 0  ZenthelSekunde

    tcflush(fd,TCIOFLUSH);
    if (tcsetattr(fd, TCSAFLUSH, &options) != 0) goto HID_IniFail;
  }
  m_hCommx = fd;
  strcpy(szMGMCommStatusText,str);
  m_Devicerun=true;
  InitializeCriticalSection(&s_critical_section);
  pthread_create(&m_thread,NULL,ReceiveThreadProc, (void*)this);
  return;

HID_IniFail:
  if (fd != INVALID_HANDLE_VALUE) close(fd);
  m_hCommx = INVALID_HANDLE_VALUE;
  m_Devicerun=0;
  return;
}

//---------------------------------------------------------
SerialComm::~SerialComm()
{
  strcpy(szMGMCommStatusText,"  COMM   : MGM Closed");
  m_Devicerun = false;
  int tout = 100;
  while (m_thread && tout--)
  {
    Sleep(20);
  };

  if (m_hCommx != INVALID_HANDLE_VALUE)
  {
    tcsetattr(m_hCommx, TCSANOW, &options_save);
    close(m_hCommx);
  }
  //if (s_critical_section.LockCount)  DeleteCriticalSection(&s_critical_section);
  m_hCommx = INVALID_HANDLE_VALUE;

  m_thread = 0;
}
//---------------------------------------------------------------

BOOL SerialComm::SendMsg(unsigned char *bufout, int nout)
{
  if (nout) {
    gMsgMonitor[0]++;
    if (Send_Buff((unsigned char*)bufout,nout)) return 1;
  }
  return 0;
}
//-------------------------------------------------------------
int SerialComm::Send_Buff(unsigned char * lBlock, int nLength)
{
  int xx;
  xx = write(m_hCommx,lBlock,nLength);
  if (xx < 0) {
    m_Devicerun=0;
    printf("USB Unplugged \n");
  }
  return xx;
}
//-------------------------------------------------------------
int SerialComm::Get_Buff(unsigned char * lBlock, int nLength)
{
  int readbytes=0;
  int res = 0;
  int n   = nLength;
  unsigned char * pnt = lBlock;
  struct pollfd pollfds[1];
  pollfds[0].fd     = m_hCommx;
  pollfds[0].events = POLLIN|POLLPRI|POLLERR;
  do {
    res = 0;
    poll(pollfds, 1, 10);
    if (pollfds[0].revents & (POLLIN|POLLPRI)) res = read(m_hCommx,pnt,n);
    if (res > 0) {
      readbytes+= res;
      pnt      += res;
      n        -= res;
    }
  }
  while (res>0 && readbytes < nLength);
  if (res<0) {
    m_Devicerun=0;
    printf("USB Unplugged \n");
  }
  return readbytes;
}

//-------------------------------------------------------------
// trick to use static function for threadloop
void * SerialComm::ReceiveThreadProc(void * lpThreadParameter)
{
  SerialComm * pCHID_MGM20 = (SerialComm *)lpThreadParameter;
  pCHID_MGM20->ReceiveLoop();
  return 0;
}
extern int SendToCNCBuff[5];
//-------------------------------------------------------------
int WaitFeedback=0;
void SerialComm::ReceiveLoop(void)
{
  int Readbytes;
  unsigned char   m_NCBuff[64*2];
  char cmdword[64];
  memset(&m_NCBuff,0,sizeof(m_NCBuff));
  Readbytes=0;
  int serial_count=0;
  while (m_thread && m_Devicerun) {
    memset(&m_NCBuff,0,sizeof(m_NCBuff));
    Readbytes = Get_Buff((unsigned char*)&m_NCBuff,sizeof(m_NCBuff));
    if (Readbytes) {
      gMsgMonitor[0]++;
    }
    int index=0;
    while (index < Readbytes) {
      char c = m_NCBuff[index++];
      bool eol = (c == '\n' || c == '\r');
      if (!eol && serial_count<sizeof(cmdword)) {
        cmdword[serial_count] = c;
        serial_count++;
      }
      if ((serial_count && eol) || serial_count >60 ) {
        cmdword[serial_count]='\0';
        //if (serial_count!= 19) printf("%s\n",cmdword);
        if (serial_count==2) {
          if (cmdword[0]=='o' && cmdword[1]=='k') {
            gCNC_HALT=gCNCCode =  'H';
            gOkCount--;
            printf("2ok %d\n",gOkCount);
            //memmove(&CNCPos[0],&CNCPos[3],sizeof(CNCPos)-(sizeof(CNCPos[3])*3));
            gNewPos ++;
          }
        } else if (serial_count>=19) {
          int a;
          if (serial_count> 19) {
            a = atoi((char*)&cmdword[19]);
            cmdword[19]='\0';
          }
          int z = atoi((char*)&cmdword[13]);
          cmdword[13]='\0';
          int y = atoi((char*)&cmdword[7]);
          cmdword[7]='\0';
          int x = atoi((char*)&cmdword[1]);
          int c = cmdword[0];

          if (c=='H')   gCNC_HALT = c;
          gCNCCode  = c;
          gCNCPos[0] = x;
          gCNCPos[1] = y;
          gCNCPos[2] = z;
          gCNCPos[3] = a;
          if (c =='H')        gCNCEiche = true;
          else if (c =='I')   gCNCEiche = false;
          gNewPos ++;

          if (cmdword[0]=='O') {
            gOkCount--;
            printf("ok %d\n",gOkCount);
          }
        } else {
//          printf("-%s-",cmdword);
        }

        memset(&cmdword,0,sizeof(cmdword));
        serial_count=0;
      }

    }
  }
  strcpy(&szMGMCommStatusText[8]," : Thread Closed");
  m_thread = 0;
}

//---------------------------------------------------------
#endif // /WIN32

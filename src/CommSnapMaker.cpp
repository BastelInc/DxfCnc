#include "CommSnapMaker.h"

extern char buff[10];

extern char szMGMCommStatusText[64];
extern char gMsgMonitor[60];
static BOOL devicerun = false;
extern char CNCCode;
extern int  CNCPos[3];
extern bool CNCPosValid;
extern int gNewPos;
#ifdef WIN32

//-------------------------------------------------------------
CommSnapMaker::CommSnapMaker(char * lpszrs)
{
  name = "CommSnapMaker";
  m_hCommx= INVALID_HANDLE_VALUE;
  m_hThread= NULL;
  m_ThreadID=0;
  memset(&osReadComm,0,sizeof(osReadComm));
  memset(&osWriteComm,0,sizeof(osWriteComm));


  char str[20];
  int no = atoi(lpszrs+3);
  if (no>  0 && no < 10)
  {
    sprintf(str,"COM%d",no);
  }
  else strcpy(str,"COM1");
  DCB dDCB;
  memset(&dDCB,0,sizeof(dDCB));

  dDCB.DCBlength = sizeof(DCB);
  if (!BuildCommDCB(str,&dDCB))  goto rs232IniFail;


  m_hCommx =  CreateFile(str,GENERIC_READ|GENERIC_WRITE,
                          FILE_SHARE_READ|FILE_SHARE_WRITE,   NULL,  OPEN_EXISTING,
                          //FILE_FLAG_WRITE_THROUGH,
						  FILE_FLAG_OVERLAPPED,
						  NULL);
   if (m_hCommx== INVALID_HANDLE_VALUE) goto rs232IniFail;


  if (m_hCommx ==  INVALID_HANDLE_VALUE) goto rs232IniFail;

  if (m_hCommx!= INVALID_HANDLE_VALUE)
  {
	dDCB.DCBlength = sizeof(DCB);
	//GetCommState(m_hCommx,&dDCB);
	dDCB.fBinary  = true;
	dDCB.Parity   = NOPARITY;
	dDCB.ByteSize = 8;
	dDCB.StopBits = ONESTOPBIT;
	dDCB.BaudRate = 115200;
	//dDCB.fOutxDsrFlow =false;

/*	if (m_SysHandshake) dDCB.fDtrControl = DTR_CONTROL_ENABLE;
	else                dDCB.fDtrControl = DTR_CONTROL_DISABLE;
	if (m_HardWare)
	{
	  if (m_SysHandshake) dDCB.fRtsControl = RTS_CONTROL_TOGGLE;
	  else                dDCB.fRtsControl = RTS_CONTROL_ENABLE;
	  dDCB.fOutxCtsFlow=TRUE;
	}
	else
*/
	{
	  dDCB.fOutxCtsFlow=FALSE;
	  dDCB.fRtsControl = RTS_CONTROL_ENABLE;
	  dDCB.fAbortOnError=FALSE;
	}
	if (!SetCommState((HANDLE)m_hCommx,&dDCB))  goto rs232IniFail;
	COMMTIMEOUTS  ct;
    memset(&ct,0,sizeof(ct));
/*	if (m_HardWare)
	{
	  ct.ReadIntervalTimeout        = 100;
	  ct.ReadTotalTimeoutMultiplier =   0;
	  ct.ReadTotalTimeoutConstant   =   0;
	  ct.WriteTotalTimeoutMultiplier=   0;
	  ct.WriteTotalTimeoutConstant  = 500;
	}
	else
*/
	{
	  ct.ReadIntervalTimeout =       1;//MAXDWORD;
	  ct.ReadTotalTimeoutMultiplier= 1;
	  ct.ReadTotalTimeoutConstant=   1;
	  ct.WriteTotalTimeoutMultiplier=1;
	  ct.WriteTotalTimeoutConstant=50;
	}

	SetCommTimeouts((HANDLE)m_hCommx,&ct);
	if (!SetupComm((HANDLE)m_hCommx,2048,256))  goto rs232IniFail;

    osReadComm.hEvent  = CreateEvent( NULL,TRUE,	FALSE,NULL );
    osWriteComm.hEvent = CreateEvent( NULL,TRUE,	FALSE,NULL );


//
//
//  int fd=0; // Filedeskriptor
//  fd = open (str, O_RDWR | O_NOCTTY | / *O_NONBLOCK | * /O_SYNC);
//
//  fcntl(fd, F_SETFL, 0);
//  if (tcgetattr(fd, &options) != 0) goto HID_IniFail;
//  if (1)
//  {
//    bzero(&options, sizeof(options)); // Structure loeschen, ggf vorher sichern
//    //  und bei Programmende wieder restaurieren
//    // Alternativ:
//    cfsetispeed(&options, B115200);
//    cfsetospeed(&options, B115200);
//
//    // setze Optionen
//    options.c_cflag &= ~PARENB;         // kein Paritybit
//    options.c_cflag &= ~CSTOPB;         // 1 Stoppbit
//    options.c_cflag &= ~CSIZE;          // 8 Datenbits
//    options.c_cflag |= CS8;
//    options.c_cflag |= (CLOCAL | CREAD);// CD-Signal ignorieren
////    options.c_cflag |= CRTSCTS;	        // enable Hardwareflow control
//    options.c_cflag &= ~CRTSCTS;	        // disable Hardwareflow control
//
//    options.c_iflag &= ~(IXON|IXOFF);
//
//    // Kein Echo, keine Steuerzeichen, keine Interrupts
//    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
//    options.c_oflag &= ~OPOST;          // setze "raw" Input
//    options.c_cc[VMIN]  = 0;            // warten auf min. 0 Zeichen
//    options.c_cc[VTIME] = 0;            // Timeout 0  ZenthelSekunde
//
//    tcflush(fd,TCIOFLUSH);
//    if (tcsetattr(fd, TCSAFLUSH, &options) != 0) goto HID_IniFail;
  }
//  m_hCommx = fd;
  strcpy(szMGMCommStatusText,str);
  m_hDataAvilable[0] =0;
  m_hManagerEvent[0] =0;
  InitializeCriticalSection(&s_critical_section);
  //pthread_create(&m_thread,0,ReceiveThreadProc, (void*)this);
  m_hThread= CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) ReceiveThreadProc,this,0,&m_ThreadID);
  //SetThreadPriority(m_hThread,THREAD_PRIORITY_BELOW_NORMAL);//THREAD_PRIORITY_HIGHEST);
  SetThreadPriority(m_hThread,THREAD_PRIORITY_HIGHEST);


  devicerun=true;
  return;
//
//HID_IniFail:
rs232IniFail:
//  if (fd != INVALID_HANDLE_VALUE) close(fd);
  m_hCommx =  INVALID_HANDLE_VALUE;
  return;

}

//---------------------------------------------------------
CommSnapMaker::~CommSnapMaker()
{

}


#else
#include <termios.h>
#include <sys/poll.h>
extern termios options;
//-------------------------------------------------------------
CommSnapMaker::CommSnapMaker(char *lpszrs):SerialComm(lpszrs)
{
  m_thread=0;
  m_hCommx= INVALID_HANDLE_VALUE;;

  char str[20];
  int no = atoi(lpszrs+3);
  if (no>  0 && no < 10)
  {
    sprintf(str,"/dev/ttyUSB%d",no-1);
  }
  else strcpy(str,"/dev/ttyUSB0");


  int fd=0; // Filedeskriptor
  fd = open (str, O_RDWR | O_NOCTTY | /*O_NONBLOCK | */O_SYNC);

  fcntl(fd, F_SETFL, 0);
  if (tcgetattr(fd, &options) != 0) goto HID_IniFail;
  if (1)
  {
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
//    options.c_cflag |= CRTSCTS;	        // enable Hardwareflow control
    options.c_cflag &= ~CRTSCTS;	        // disable Hardwareflow control

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
  m_hDataAvilable[0] =0;
  m_hManagerEvent[0] =0;

  dataBufSize= 1024;
  dataBuf = (char *)malloc(dataBufSize);


  InitializeCriticalSection(&s_critical_section);
  devicerun=true;
  pthread_create(&m_thread,0,ReceiveThreadProc, (void*)this);
  return;

HID_IniFail:
  if (fd != INVALID_HANDLE_VALUE) close(fd);
  m_hCommx = INVALID_HANDLE_VALUE;
  return;

}
//-------------------------------------------------------------
char* CommSnapMaker::getClassName() {
    return "CommSnapMaker";
}


//-------------------------------------------------------------
CommSnapMaker::~CommSnapMaker()
{
   devicerun = false;
}
#endif
//-------------------------------------------------------------
int  CommSnapMaker::getLine(char * lpbuf,int len) {
//  dataBufSize= 1024;
//  dataBuf = (char *)malloc(dataBufSize);
  len--;
repeatRead:
  for (int i=0;i < PushBackCnt && i < len;i++) {
    if (dataBuf [i]== '\n') {
      i++;
      memcpy(lpbuf,dataBuf,i);
      lpbuf[i] = 0;
      int rest = PushBackCnt-i;
      if (rest > 0) {
        memcpy(&dataBuf[0],&dataBuf[i],rest);
      }
      PushBackCnt = rest;
      return i;
    }
  }
  int Readbytes = Get_Buff((unsigned char*)&dataBuf[PushBackCnt],dataBufSize-PushBackCnt);
  if (Readbytes > 0) {
    PushBackCnt += Readbytes ;
    goto repeatRead;
  }

  return 0;
}

//-------------------------------------------------------------

void CommSnapMaker::ReceiveLoop(void)
{
  int Readbytes;
  unsigned char		m_NCBuff[640];
//  int  PushBackCnt=0;
  memset(&m_NCBuff,0,sizeof(m_NCBuff));
  Readbytes=0;

#ifdef WIN32
  while (m_hThread && m_hCommx != INVALID_HANDLE_VALUE && devicerun)
#else
  while (m_thread && devicerun)
#endif
  {
    Readbytes=0;

//    Readbytes = Get_Buff((unsigned char*)&m_NCBuff[PushBackCnt],sizeof(m_NCBuff)-PushBackCnt);
    Readbytes = getLine((char*)&m_NCBuff,sizeof(m_NCBuff));
    if (Readbytes)//+PushBackCnt)
{
      //Readbytes += PushBackCnt;
      //PushBackCnt = 0;
      gMsgMonitor[0]++;
      int code = 0;
      int done = 0;
      float Val = 0;
      char  msg[64] = {0};
      int j = 0;
      int i = 0;
      int answerVal=0;
      int l2 = strlen((char*)&m_NCBuff);
      if (l2 < Readbytes) Readbytes = l2;
      m_NCBuff[Readbytes]= 0;
      printf((char*)&m_NCBuff);
      for (;done==0&&i<Readbytes; i++) {
        if (code == 0 && isalpha(m_NCBuff[i])) {
          code = m_NCBuff[i];
        } else if (code == 0 && m_NCBuff[i]== '0') {
          //CNCCode = answerVal? 'H':'0';
          CNCCode = '0';
          printf(" H=ok \n");
          done=1;
        } else if (m_NCBuff[i]== ':') {
            if (m_NCBuff[i+1]== ' ') i++;
            answerVal++;
        } else if (m_NCBuff[i]=='\n'||m_NCBuff[i]== ' ') {
          switch (code) {
            case 'X':
              if (answerVal==1) {
//                CNCPos[0] = atof(msg)*100;
//                printf(" X%d",CNCPos[0]);
              } else if (answerVal>=3) {
                CNCPos[0] = atof(msg)/4;
                printf(" x%d",CNCPos[0]);
              }
              code = 0;
              break;
            case 'Y':
              if (answerVal==2) {
//                CNCPos[1] = atof(msg)*100;
//                printf(" Y%d",CNCPos[1]);
              } else if (answerVal>=4) {
                CNCPos[1] = atof(msg)/4;
                printf(" y%d",CNCPos[1]);
              }
              code = 0;
              break;
            case 'Z':
              if (answerVal==3) {
//                CNCPos[2] = atof(msg)*100;
//                printf(" Z%d",CNCPos[2]);
              } else if (answerVal>=5) {
                CNCPos[2] = atof(msg)/4;
                printf(" z%d",CNCPos[2]);
              }
              CNCPosValid=true;
              code = 0;
              gNewPos ++;
              break;
            case 'E':
              if (answerVal==3) {
                float E = atof(msg);
                printf(" Z%f.2",E);
              }
              code = 0;
              break;
            case 'C':
              code = 0;
              break;
            default :
              code = 0;
              break;
          }
          j=0;
          memset(&msg,0,sizeof(msg));
          if (m_NCBuff[i]== '\n') done = 1;
        } else {
          switch (code) {
            case 'o':
              if (m_NCBuff[i] == 'k') {
                //CNCCode = '0';
                CNCCode = code;
                printf(" H=%c \n",CNCCode);
                done=1;
              }
            break;
            case 'X':
              if (j < sizeof(msg)) msg[j]=m_NCBuff[i];
              j++;
              break;
            case 'Y':
              if (j < sizeof(msg)) msg[j]=m_NCBuff[i];
              j++;
              break;
            case 'Z':
              if (j < sizeof(msg)) msg[j]=m_NCBuff[i];
              j++;
              break;
            case 'C':
              if (j < sizeof(msg)) msg[j]=m_NCBuff[i];
              j++;
              break;
            case 'E':
              if (j < sizeof(msg)) msg[j]=m_NCBuff[i];
              j++;
              break;
            default :
              //code = 0;
              break;
          }
        }
      }
      //Readbytes=i+1;
      //memcpy(&m_NCBuff[0],&m_NCBuff[i],Readbytes);
      //memset(&m_NCBuff[0],sizeof(m_NCBuff),0);
      //PushBackCnt = 0;//Readbytes;


/*      unsigned char checks=0;
      printf((char *)m_NCBuff);
      for (int i=0; i< 21; i++)

      checks -= m_NCBuff[i];
      if (checks==0)
      {
        if (m_NCBuff[0]>='A'&& m_NCBuff[0] <='z'&& m_NCBuff[19]=='\r' )
        {
          int z = atoi((char*)&m_NCBuff[13]);
          m_NCBuff[13]='\0';
          int y = atoi((char*)&m_NCBuff[7]);
          m_NCBuff[7]='\0';
          int x = atoi((char*)&m_NCBuff[1]);
          int c = m_NCBuff[0];
          Readbytes -= 21;
          memcpy(&m_NCBuff[0],&m_NCBuff[21],Readbytes);
          PushBackCnt = Readbytes;

          {
            EnterCriticalSection(&s_critical_section);
		        CNCCode = c;
				CNCPos[0] = x;
				CNCPos[1] = y;
				CNCPos[2] = z;
            gNewPos ++;
            LeaveCriticalSection(&s_critical_section);
          }
        }
      }
 */
    }
  }
  strcpy(&szMGMCommStatusText[8]," : MGM Closed");
#ifdef WIN32
  m_hThread = NULL;
#else
  m_thread = 0;
#endif
}

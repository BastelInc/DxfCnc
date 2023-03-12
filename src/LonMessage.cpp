//  This file is part of the Bastel project. It is distributed under
//  GNU General Public License: https://www.gnu.org/licenses/gpl-3.0
//  Copyright (C) Bastel.Inc - All Rights Reserved

#include "include/BastelUtils.h"
#include "LonMessage.h"
#include "CncSetup.h"

#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // WIN32
#include <stdlib.h>
#pragma pack(push, 1)

//--------------------------------------------------------------
#pragma comment(lib,"ws2_32.lib")
//-------------------------------------------------------------
CUDP_Comm::CUDP_Comm()
{
  m_hCommx= (int)-1;
  m_Callback=NULL;
#ifdef WIN32
WSADATA wsa;
//Initialise winsock
//http://www.binarytides.com/udp-socket-programming-in-winsock/
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
#endif

  if ((m_hCommx=socket(AF_INET, SOCK_DGRAM, 0))==-1)
  {
    perror("m_hCommx AF_INET");
  }
  int port      = 1320;
  int broadcast = 1;
  setsockopt(m_hCommx, SOL_SOCKET, SO_BROADCAST,(char *) &broadcast, sizeof broadcast);

  memset(&Remote_Address, 0, sizeof(Remote_Address));
  Remote_Address.sin_family      = AF_INET;
  Remote_Address.sin_port        = htons(port);
  Remote_Address.sin_addr.s_addr = INADDR_ANY;   // 0

  memset(&Server_Address, 0, sizeof(Server_Address));
  Server_Address.sin_family      = AF_INET;
  Server_Address.sin_port        = htons(port);
#ifdef WIN32
//  unsigned long  addr = inet_addr("192.168.1.107");
  //Server_Address.sin_addr.s_addr = (inet_addr("192.168.1.107"));   // 0
  //Server_Address.sin_addr.s_addr = (inet_addr("192.168.1.131"));   // 0
  Server_Address.sin_addr.s_addr = (inet_addr("192.168.1.133"));   // 0
#else
  Server_Address.sin_addr.s_addr = htonl(inet_network ("192.168.1.133"));   // 0
#endif

  if (bind(m_hCommx, (sockaddr *)&Remote_Address, sizeof(sockaddr))==-1)
  {
    perror("bind fails");
  }



#ifdef WIN32
  DWORD  m_ThreadID;
  m_thread= CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) ReceiveThreadProc,this,0,&m_ThreadID);
  //SetThreadPriority(m_thread,THREAD_PRIORITY_TIME_CRITICAL);//THREAD_PRIORITY_HIGHEST);//THREAD_PRIORITY_BELOW_NORMAL);//
  SetThreadPriority(m_thread,THREAD_PRIORITY_HIGHEST);
#else
  pthread_create(&m_thread,0,ReceiveThreadProc, (void*)this);
#endif

  return;
}

//---------------------------------------------------------
CUDP_Comm::~CUDP_Comm()
{
  m_thread =0;

  if (m_hCommx != (int)INVALID_HANDLE_VALUE)
  {
#ifdef WIN32
    closesocket(m_hCommx);
    WSACleanup();
#else
    close(m_hCommx );
#endif
  }
  m_hCommx = (int)INVALID_HANDLE_VALUE;
  printf("\nClose socket");
}

int CUDP_Comm::Send_Buff(unsigned char * lBlock, int nLength)
{
#pragma pack(push, 1)
  int xx;
  ApllicationStruct	TotalMSG;
  memset(&TotalMSG,0,sizeof(TotalMSG));
  //  TotalMSG.TransaktionID;
  //  TotalMSG.subnet;
  //  TotalMSG.node;
  //  TotalMSG.dwTTick;
  memcpy(&TotalMSG.code,lBlock,sizeof(ManagerStruct));
  nLength = nLength + 7;//offsetof(ApllicationStruct,code);
  nLength = sizeof(TotalMSG);
  printf("SENDTO : IP %s (%d bytes)\n", inet_ntoa(Server_Address.sin_addr),  nLength);

  xx = sendto(m_hCommx,(char*)&TotalMSG,nLength, 0, (struct sockaddr*)&Server_Address, sizeof Server_Address);
  return xx;
#pragma pack(pop)
}




// trick to use static function for threadloop
void * CUDP_Comm::ReceiveThreadProc(void * lpThreadParameter)
{
  CUDP_Comm * pCHID_MGM20 = (CUDP_Comm*)lpThreadParameter;
  pCHID_MGM20->ReceiveLoop();
  return 0;
}
//-------------------------------------------------------------
void CUDP_Comm::ReceiveLoop(void)
{
  msg_out_addr  CurrentMSGaddr;
  memset(&CurrentMSGaddr,0,sizeof(CurrentMSGaddr));

  int Readbytes;
  ApllicationStruct  RcvMngMsg;
  unsigned char		m_NCBuff[200];
#ifdef WIN32
#else
  pthread_detach(pthread_self());
#endif

  memset(&m_NCBuff,0,sizeof(m_NCBuff));
  Readbytes=0;
  int responses=0;
  while (m_thread) // && Readbytes!= 69)
  {
    memset(&RcvMngMsg,0,sizeof(RcvMngMsg));
    Readbytes=0;
    ApllicationStruct * MG=(ApllicationStruct * )&m_NCBuff;
#ifdef WIN32
    int slen=sizeof(sockaddr);
#else
    socklen_t slen=sizeof(sockaddr);
#endif
    Readbytes = recvfrom(m_hCommx,(char*)&m_NCBuff,sizeof(m_NCBuff), 0, (sockaddr *)&Server_Address, &slen);

    if (m_Callback!=NULL) {
      if (Readbytes ==69) m_Callback(m_NCBuff);
    }
    //if (message ==-1)  perror("Error: recvfrom call failed");
    //printf("CLIENT: read %d bytes from IP %s(0x%x)\n", Readbytes, inet_ntoa(Server_Address.sin_addr), MG->code);
  }
  m_thread = 0;

}

#pragma pack(pop)

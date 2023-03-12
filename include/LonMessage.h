#ifndef CLONMESSAGE_H
#define CLONMESSAGE_H
#include "include/BastelUtils.h"
#include "NNetmgmt.h"

#pragma pack(push, 1)

typedef struct ApllicationStruct
{
  unsigned char  TransaktionID;
  unsigned char  subnet;
  unsigned char  node;
  DWORD          dwTTick;

  unsigned char	 code;
  unsigned char	 service;
  msg_out_addr	 MSGaddr;
  unsigned char	 MSGLen;
  unsigned char	 MSG[MgSMsgSize];

}ApllicationStruct;

#pragma pack(pop)

#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

typedef void (CUDP_CommCallback ) (unsigned char * pnt);

class CUDP_Comm
{
public:
#ifdef WIN32
  HANDLE      m_thread;
#else
  pthread_t   m_thread;
#endif
  sockaddr_in Remote_Address, Server_Address;
  int m_hCommx;
  CUDP_Comm();
  virtual ~CUDP_Comm();
  int  Send_Buff(unsigned char *buf, int n);
  static void * ReceiveThreadProc(void * lpThreadParameter);
  void ReceiveLoop(void);

  CUDP_CommCallback * m_Callback;
};

extern CUDP_Comm   *     gLonService;

#endif // CLONMESSAGE_H

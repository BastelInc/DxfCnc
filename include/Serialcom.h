#if !defined(AFX_SERILCOM_H__2E08E567_100A_4a29_B1AD_065A695D45A2__INCLUDED_)
#define AFX_SERILCOM_H__2E08E567_100A_4a29_B1AD_065A695D45A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern char szMGMCommStatusText[64];

class SerialComm
{
public:
  const char* name;
  BOOL m_Devicerun;
  SerialComm();
  SerialComm(char * lpszrs);
  virtual ~SerialComm();

  virtual BOOL SendMsg(unsigned char *bufout, int nout);
  static CRITICAL_SECTION	s_critical_section;
  const char* getClassName() {
    return name;
  }
  int IsRunning() {
    return m_Devicerun;
  }
#ifdef WIN32
  HANDLE m_hCommx;
  HANDLE m_hThread;
  DWORD  m_ThreadID;
  OVERLAPPED  osWriteComm;
  OVERLAPPED  osReadComm;
 #else
  int m_hCommx;
  pthread_t m_thread;
#endif
  virtual void ReceiveLoop(void);
  static void * ReceiveThreadProc(void * lpThreadParameter);
  int  Send_Buff(unsigned char *buf, int n);
  int  Get_Buff (unsigned char *buf, int n);
};

#endif

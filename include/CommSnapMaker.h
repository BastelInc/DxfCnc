#ifndef COMMSNAPMAKER_H
#define COMMSNAPMAKER_H

#include <stdio.h>
#include <FltkWindow.h>
#include "Serialcom.h"


class CommSnapMaker : public SerialComm
{
  public:
        CommSnapMaker(char * lpszrs);
        virtual ~CommSnapMaker();

  virtual int  getLine(char * lpbuf,int len);
    protected:
  virtual void ReceiveLoop(void);

    private:
};

#endif // COMMSNAPMAKER_H

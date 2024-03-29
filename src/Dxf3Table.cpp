#include <FL/Fl.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Window.H>

#include "FingerScroll.h"
#include "GlDisplay.h"
#include "Object.h"
#include "Resource.h"
#include "Settings.h"
#include "TreeView.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_ask.H>
#include <signal.h>
#define LEFTFULLSCREEN
#undef LEFTFULLSCREEN
#define CNCINT int

int OutputDebugInfo::level = 2;
int OutputDebugInfo::debug = 1;
CBastelUtils gBastelUtils;

CObjectManager *gObjectManager = 0;
DXF3DDisplay *gWrk3DSheet = 0;
Fl_Window *wMain = NULL;
Fl_Window *wSettings = NULL;
Fl_Button *gSaveBtn = NULL;
// CTreeView         * gLayerTree;
// CFingerScroll     * gLayerFingerScroll = NULL;

char evt_txt[256] = {0};
int gIselRs232Mode = 0;
#ifdef WIN32
typedef int (*WM_LoadFileCB)(HWND, WPARAM, LPARAM);
extern WM_LoadFileCB gLoadFileCB;
#else
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#undef SEMAPHORE_MSG
int IsMainFileChange(void);
#endif
//-------------------------------------------------------------
void ShowTab(int key) { ; }
void SetStatusText(const char *pnt, int num, int style) { ; }
Fl_Preferences *prefs = NULL;
//-------------------------------------------------------------
void Background_cb(Fl_Widget *o)
{
  gBackground = (((Fl_Slider *)o)->value()) / 100.0;
  gWrk3DSheet->redraw();
}

void GrayOut_cb(Fl_Widget *o)
{
  gGrayOut = (((Fl_Slider *)o)->value()) / 100.0;
  gWrk3DSheet->redraw();
}

void Transparecy_cb(Fl_Widget *o)
{
  gAlphaTransp = (((Fl_Slider *)o)->value()) / 100.0;
  gWrk3DSheet->redraw();
}

void Deep_cb(Fl_Widget *o)
{
  Deep = (((Fl_Slider *)o)->value()) / 100.0;
  gLookClip = 1;
  gWrk3DSheet->redraw();
}

//---------------------------------------------------------------
void SetIniFile(LPCSTR FileName)
{
  char *pnt = getenv("HOME");
  if (pnt) {
    strcpy(CBastelUtils::s_IniPath, pnt);
    strcat(CBastelUtils::s_IniPath, "/.");
  } else {
#ifdef WIN32
    if (!pnt)
      pnt = getenv("HOME_PATH");
    if (!pnt)
      pnt = getenv("UserProfile");
    strcpy(CBastelUtils::s_IniPath, pnt);
    strcat(CBastelUtils::s_IniPath, "/.");
#else
    strcpy(CBastelUtils::s_IniPath, "");
#endif
  }
  strcat(CBastelUtils::s_IniPath, "bastel");
  if (FileName == NULL)
    gBastelUtils.SetIniFile("DXF3Table.ini");
  else
    gBastelUtils.SetIniFile(FileName);
}
//---------------------------------------------------------------
void cbExit(Fl_Widget *, void *)
{
  /*  if (prefs)
  {
  if (gEditemain)yTab      = gEditemain->h();
  Mainwidth = MainWindow->w();
  Mainheight=MainWindow->h();
  MainPosX  =MainWindow->x();
  MainPosY  =MainWindow->y();
  if (MainWindow->fullscreen_active ()==0)
  {
  if (gEditemain) prefs->set("Editemain",    yTab);
  prefs->set("Mainwidth",    Mainwidth);
  prefs->set("Mainheight",   Mainheight);
  prefs->set("MainPosX",     MainPosX);
  prefs->set("MainPosY",     MainPosY);
  }
  prefs->set("IselRs232Mode",gIselRs232Mode);

    prefs->flush();
    }
    */
  //  if (ParamModify) {
  //    int ret = fl_choice("Data modyify, you want to quit?","Nein","Ja",NULL);
  //    if (ret == 1)  exit(0);
  //  } else
  exit(0);
}
//---------------------------------------------------------------
void cbMainWindowExit(Fl_Widget *w, void *)
{
  if (w == wSettings ||
      Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
    if (wSettings)
      wSettings->hide();
    return; // ignore Escape
  }
  cbExit(NULL, NULL);
}

//-------------------------------------------------------------
static time_t statbuf_mtime = 0;

void GlTimer_CB(void *data)
{
  DXF3DDisplay *pWrk3DSheet = (DXF3DDisplay *)data;

  extern const char *szvertexshaderFile;
  extern const char *szfragmentshaderFile;

  static time_t statbufS_mtime = 0;
  static time_t statbufV_mtime = 0;

  struct stat statbufV;
  struct stat statbufS;
  struct stat statbufUV;
  statbufV_mtime = 0;
  statbufS_mtime = 0;
  stat(szvertexshaderFile, &statbufV);
  stat(szfragmentshaderFile, &statbufS);
  int reload = 0;
  int reloadTxt = 0;
  if (statbufV_mtime != statbufV.st_mtime ||
      statbufS_mtime != statbufS.st_mtime) {
    if (statbufV_mtime != 0) {
      reload++;
    }
    statbufV_mtime = statbufV.st_mtime;
    statbufS_mtime = statbufS.st_mtime;
  }
  for (int i = 0; i < sizeof(rgTextureData) / sizeof(rgTextureData[0]); i++) {
    statbufUV.st_mtime = 0;
    if (rgTextureData[i].szFilename) {
      stat(rgTextureData[i].szFilename, &statbufUV);
      if (rgTextureData[i].mtime != statbufUV.st_mtime)
        if (rgTextureData[i].mtime != 0) {
          reloadTxt++;
        }
      rgTextureData[i].mtime = statbufUV.st_mtime;
    }
  }
  if (reloadTxt) {
    gObjectManager->TextureFreigeben();
    gObjectManager->TextureEinlesen();
    glDisplay_redraw = 1;
  }
  if (reload) {
    pWrk3DSheet->InitOpenGL();
    glDisplay_redraw = 1;
  }
  if (SpeichernTimeOut) {
    SpeichernTimeOut--;
    if (gSaveBtn) {
      gSaveBtn->value(SpeichernTimeOut != 0);
    }
  }
  if (LookAtTimeOut)
    LookAtTimeOut--;
  struct stat statbuf;
  stat(gBastelUtils.m_IniFile, &statbuf);
  if (statbuf_mtime != statbuf.st_mtime) {
    if (LookAtTimeOut == 0 && statbuf_mtime != 0) {
      LookAtLaden(gLookAtIx);
      glDisplay_redraw = 1;
    }
    statbuf_mtime = statbuf.st_mtime;
  }

#ifndef WIN32
#if SEMAPHORE_MSG
  IsMainFileChange();
#endif
#endif
  static int fsize = 0;
  if (gObjectManager && gObjectManager->m_InUse == 0) {
    if (evt_txt[0]) { // von Drag and Drop oder vom Zweiter Instance
      char *pnt = strchr(evt_txt, '\n');
      if (pnt)
        *pnt = '\0';
      if (strncmp(evt_txt, "file://", 7) == 0) {
        if (gObjectManager->FileOpen(&evt_txt[7], 1)) {
          memset(evt_txt, 0, sizeof(evt_txt));
        }
      } else {
        if (gObjectManager->FileOpen(evt_txt, 1)) {
          memset(evt_txt, 0, sizeof(evt_txt));
        }
      }
    } else if (stat(gObjectManager->m_FilePfadName, &statbuf) == 0) {
      if (gObjectManager->time != statbuf.st_mtime) {
        if (fsize == statbuf.st_size) { // muss zweites mal gleich sein
          gObjectManager->FileOpen(NULL, 1);
        }
        fsize = statbuf.st_size;
      } else {
        if (gObjectManager->TestForUpdate()) {
          glDisplay_redraw = 1;
          // gObjectManager->time=1;
        }
      }
    }
  }
  if (glDisplay_redraw) {
    pWrk3DSheet->redraw();
    glDisplay_redraw = 0;
  }
  static float degTimer = 0;
#if 1 //def MOUSSERVICE
  if (gObject_redraw) {
    pWrk3DSheet->redraw();
    gObject_redraw = 0;
  }
#endif
  Fl::repeat_timeout(0.2, GlTimer_CB, data);
}

#include <FL/Fl_Native_File_Chooser.H>

// GLOBALS
Fl_Input *G_filename = NULL;

void PickFile(Fl_Widget *, void *)
{
  // Create native chooser
  if (gObjectManager)
    gObjectManager->m_InUse++;
  strcpy(evt_txt, gObjectManager->m_FilePfadName);

  Fl_Native_File_Chooser native;
  native.title("Pick a file");
  native.type(Fl_Native_File_Chooser::BROWSE_FILE);
  // native.filter("Files\t*.{dxf}\n");
  native.filter("Files\t*.{dxf,stl}\n");
  native.filter("DXF\t*.dxf\n"
                "Files\t*.{dxf,stl,gcode}\n"
                "Text\t*.{txt}\n");
  native.preset_file(evt_txt);
  // Show native chooser
  switch (native.show()) {
  case -1:
    fprintf(stderr, "ERROR: %s\n", native.errmsg());
    break; // ERROR
  case 1:
    fprintf(stderr, "*** CANCEL\n");
    fl_beep();
    break; // CANCEL
  default: // PICKED FILE
    if (native.filename()) {
      strcpy(evt_txt, native.filename());
    } else {
      // G_filename->value("NULL");
    }
    break;
  }
  if (gObjectManager && gObjectManager->m_InUse > 0)
    gObjectManager->m_InUse--;
}

//-------------------------------------------------------------

void cbDisplayKey(Fl_Widget *item, void *idc)
{
  long x = (long)idc;
  switch (x) {
  case 'q':
    cbExit(NULL, NULL);
    break;
  case 'd':
    PickFile(NULL, NULL);
    break;
  case 's':
    SpeichernTimeOut = 10;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
    gLookAtIx = x - '1';
    {
      if (SpeichernTimeOut) {
        if (gLookAtIx >= 0 && gLookAtIx <= 4) {
          LookAtSpeichern(gLookAtIx);
          if (gSaveBtn) {
            gSaveBtn->value(SpeichernTimeOut = 0);
          }
        }
      } else {
        LookAtLaden(gLookAtIx);
      }
    }
    break;
  case 'z':
    bArrangeZoomCenter = !bArrangeZoomCenter;
    Zoom = 1.0;
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;
     UpdateMainWindow();
    break;
  case 'a':
    gShowLayerButtons(10000);
    UpdateMainWindow();
    break;
  case 'n':
    gShowLayerButtons(0);
    UpdateMainWindow();
    break;
  case 'D':
    gShowLayerButtons(-1);
    UpdateMainWindow();
    break;
  case FL_F + 5:
    gObjectManager->Update3DreiTafelProjektion();
    UpdateMainWindow();
    break;
  }
}
//-------------------------------------------------------------
void SettingsShow(int show)
{
  if (show & 0x40) {
    if (wSettings)
      wSettings->show();
  } else if (show & 0x80) {
    if (wSettings)
      wSettings->hide();
  }
}

//-------------------------------------------------------------

#ifndef WIN32
using namespace std;

int getProcIdByName(string procName)
{
  int num = 0;
  int pid = -1;
  // Open the /proc directory
  DIR *dp = opendir("/proc");
  if (dp != NULL) {
    // Enumerate all entries in directory until process found
    struct dirent *dirp;
    while (/*pid < 0 &&*/ (dirp = readdir(dp))) {
      // Skip non-numeric entries
      int id = atoi(dirp->d_name);
      if (id > 0) {
        // Read contents of virtual /proc/{pid}/cmdline file
        string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
        ifstream cmdFile(cmdPath.c_str());
        string cmdLine;
        getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
          // Keep first cmdline item which contains the program path
          size_t pos = cmdLine.find('\0');
          if (pos != string::npos)
            cmdLine = cmdLine.substr(0, pos);
          // Keep program name only, removing the path
          pos = cmdLine.rfind('/');
          if (pos != string::npos)
            cmdLine = cmdLine.substr(pos + 1);
          // Compare against requested process name
          if (procName == cmdLine) {
            if (id != getpid())
              pid = id;
            num++;
          }
        }
      }
    }
  }
  closedir(dp);
  return pid;
}

const char *BackingFile = "DXF3Aufruf";
#ifdef SEMAPHORE_MSG
const char *SemaphoreName = "DXF3AufrufM";
#endif
int AccessPerms = 0644;
int ByteSize = 256;
sem_t *semptr = 0;
int SendMainFileChange(int pid, const char *pContents)
{
  int fd = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);
  if (fd >= 0) {
    ftruncate(fd, ByteSize);
    caddr_t memptr = (caddr_t)mmap(NULL, ByteSize, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, fd, 0);
    if ((caddr_t)-1 != memptr) {
      // fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize -
      // 1); fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );
      strcpy(memptr, pContents);
      if (strlen(pContents) < 2)
        fprintf(stderr, "App already running: Bring window to Top\n");
      else
        fprintf(stderr, "App already running: Load File into\n");

#ifdef SEMAPHORE_MSG
      semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);
      if (semptr !=
          (void *)-1) { // increment the semaphore so that memreader can read
        if (sem_post(semptr) < 0)
          report_and_exit("sem_post");
        sem_init(semptr, 0, 1);
      }
#else
      if (pid > 0)
        kill(pid, SIGUSR1);
#endif
      sleep(1);                 // give reader a chance
      munmap(memptr, ByteSize); // unmap the storage
      close(fd);
#ifdef SEMAPHORE_MSG
      if (semptr != (void *)-1)
        sem_close(semptr);
#endif
      shm_unlink(BackingFile); // unlink from the backing file
    }
  } else {
    close(fd);
  }
  return 0;
}

#ifdef SEMAPHORE_MSG
void InitMainFileChange(void)
{
  // create a semaphore for mutual exclusion
  semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);
  if (semptr != (void *)-1) {
    sem_init(semptr, 0, 0);
  }
}

int IsMainFileChange(void)
{
  int sval = 0;
  if (semptr)
    sem_getvalue(semptr, &sval);
  if (sval) {
    sval = 0;
    sem_init(semptr, 0, 0);
    int fd = shm_open(BackingFile, O_RDWR, AccessPerms); /* empty to begin */
    if (fd >= 0) {
      /* get a pointer to memory */
      caddr_t memptr =
        (caddr_t)mmap(NULL,     /* let system pick where to put segment */
                      ByteSize, /* how many bytes */
                      PROT_READ | PROT_WRITE, /* access protections */
                      MAP_SHARED, /* mapping visible to other processes */
                      fd,         /* file descriptor */
                      0);         /* offset: start at 1st byte */
      if ((caddr_t)-1 == memptr)
        report_and_exit("Can't access segment...");

      strncpy(evt_txt, (const char *)memptr, sizeof(evt_txt));
      // sem_post(semptr);

      /* cleanup */
      munmap(memptr, ByteSize);
      close(fd);
      // sem_close(semptr);
      unlink(BackingFile);
      return 1;
    }
  }

  return 0;
}
#else
static void check_signal();
void InitMainFileChange(void)
{
  signal(SIGUSR1, (void (*)(int))check_signal);
}

static void check_signal(void)
{
  int fd = shm_open(BackingFile, O_RDWR, AccessPerms); /* empty to begin */
  if (fd >= 0) {
    caddr_t memptr = (caddr_t)mmap(NULL, ByteSize, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, fd, 0);
    if ((caddr_t)-1 != memptr) {
      strncpy(evt_txt, (const char *)memptr, sizeof(evt_txt));
      if (strlen(evt_txt) < 4) {
        evt_txt[0] = '\0';
        if (wMain) {
          wMain->make_current();
          wMain->hide();
          Fl::check();
          wMain->show();
        }
      }
      fprintf(stderr, "Signal New Load File %s\n", evt_txt);
      munmap(memptr, ByteSize);
    }
    close(fd);
    unlink(BackingFile);
  }
}
#endif
//-------------------------------------------------------------
#else
//-------------------------------------------------------------
int getProcId(const char *lpCmdLine)
{
  HANDLE hM = CreateMutex(NULL, TRUE, "DXF3Table_M");
  DWORD getlaste = GetLastError();
  char szWindowClass[] = "FLTK";
  if (getlaste == ERROR_ALREADY_EXISTS) {
    HWND hWnd = FindWindow(szWindowClass, NULL);
    SetForegroundWindow((HWND)hWnd);
    if (lpCmdLine && hWnd) {
      HANDLE hMapFile;
      hMapFile = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0,
                                   _MAX_PATH, "MyFileMappingObject");

      if (hMapFile) {
        LPVOID lpMapAddress;
        lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

        if (lpMapAddress) {
          // show the File in the existing window
          strcpy((LPTSTR)lpMapAddress, lpCmdLine);
          PostMessage(hWnd, WM_USER + 10, 0, 1234);
          Sleep(100);
          UnmapViewOfFile(lpMapAddress);
        }
        CloseHandle(hMapFile);
      }
    }
    return 0;
  }
  return 1;
}

//-------------------------------------------------------------
int LoadFileCB(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
  if (lParam == 1234) {
    HANDLE hMapFile;
    LPVOID lpMapAddress;
    hMapFile =
      OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, "MyFileMappingObject");
    if (hMapFile) {
      lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_COPY, 0, 0, 0);
      if (lpMapAddress) {
        strncpy(evt_txt, (LPSTR)lpMapAddress, sizeof(evt_txt));
        UnmapViewOfFile(lpMapAddress);
      }
      CloseHandle(hMapFile);
    }
  }
  return 0;
}

#endif
//-------------------------------------------------------------
//-------------------------------------------------------------
int main(int argc, char **argv)
{

#ifdef WIN32
  gLoadFileCB = LoadFileCB;
  if (getProcId(argv[1]) == 0) {
    // Send Filename to First Instance
    return 0;
  }
#else
  int pid = 0;
  if ((pid = getProcIdByName("Dxf3Table")) > 1) {
    if (argc > 1) {
      SendMainFileChange(pid, argv[1]);
      return 0;
    } else {
      SendMainFileChange(pid, "");
      return 0;
    }
  } else {
    InitMainFileChange();
  }

#endif

  Fl::use_high_res_GL(1);
  Fl::get_system_colors();
  Fl::scheme("gtk+");
  Fl::foreground(240, 240, 240);
  Fl::background(92, 91, 86);
  Fl::background2(86, 85, 80);
  Fl::set_color(FL_SELECTION_COLOR, 241, 176, 0);
#if 1
#ifdef WIN32
  wSettings = new Fl_Window(-1200, 40, 200, 720);
#else
  wSettings = new Fl_Window(20, 40, 230, 710);
#endif
  wSettings->callback(cbMainWindowExit);
  wSettings->icon(&ReseorceInit::ic_AppIconSettings_xpm);

  int w = (wSettings->w() - 6) / 5;
  int h = wSettings->h() / 20;
  int i = 0;

  SetIniFile(NULL);
  //  LookAtLaden(gLookAtIx=0);

  Fl_Button load(4, h * i, w * 1, h, "Open");
  load.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'d');
  Fl_Light_Button save(4 + w * 1, h * i, w * 2, h, "SaveP");
  gSaveBtn = &save;
  save.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'s');
  Fl_Button quit(4 + w * 3, h * i++, w * 2, h, "Quit");
  quit.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'q');
  Fl_Button load1(4 + w * 0, h * i, w, h, "1");
  load1.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'1');
  Fl_Button load2(4 + w * 1, h * i, w, h, "2");
  load2.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'2');
  Fl_Button load3(4 + w * 2, h * i, w, h, "3");
  load3.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'3');
  Fl_Button load4(4 + w * 3, h * i, w, h, "4");
  load4.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'4');
  Fl_Button Fit(4 + w * 4, h * i++, w, h, "Fit");
  Fit.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'z');

  Fl_Slider deepslider((int)(w * 4.5), 3 + h * i, w / 2, h * 3 - 6, "Deep");
  deepslider.align(FL_ALIGN_LEFT);
  deepslider.callback(Deep_cb);
  deepslider.value(Deep * 100.0);
  deepslider.step(1);
  deepslider.bounds(0, 100);

  Fl_Hor_Slider bslider(w, 3 + h * i++, w * 3, h - 6, "Back");
  bslider.align(FL_ALIGN_LEFT);
  bslider.callback(Background_cb);
  bslider.value(gBackground * 100.0);
  bslider.step(1);
  bslider.bounds(3, 100);

  Fl_Hor_Slider slider(w, 3 + h * i++, w * 3, h - 6, "Gray");
  slider.align(FL_ALIGN_LEFT);
  slider.callback(GrayOut_cb);
  slider.value(gGrayOut * 100.0);
  slider.step(1);
  slider.bounds(3, 100);

  Fl_Hor_Slider oslider(w, 3 + h * i++, w * 3, h - 6, "Alpha");
  oslider.align(FL_ALIGN_LEFT);
  oslider.callback(Transparecy_cb);
  oslider.value(gAlphaTransp * 100.0);
  oslider.step(1);
  oslider.bounds(3, 100);

  Fl_Button layerall(4, 5 + h * i, w * 1, h, "All");
  layerall.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'a');
  Fl_Button layernone(4 + w * 1, 5 + h * i, w * 1, h, "None");
  layernone.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'n');
  Fl_Button layerdef(4 + w * 2, 5 + h * i, w * 1, h, "Def.");
  layerdef.callback((Fl_Callback *)cbDisplayKey, (void *)(long)'D');
  Fl_Button layerupdate(4 + w * 3, 5 + h * i++, w * 2, h, "Update");
  layerupdate.callback((Fl_Callback *)cbDisplayKey, (void *)(long)(FL_F + 5));

#ifdef FINGERSCROLL
  gLayerFingerScroll =
    new CFingerScroll(4, 13 + h * i, w * 4, h * (20 - i) - 20, "");
  gLayerFingerScroll->type(Fl_Scroll::VERTICAL);
  gLayerFingerScroll->scrollbar_size(22);
  gLayerFingerScroll->m_pack->end();
#else
  gLayerTree = new CTreeView(4, 13 + h * i, w * 5, h * (20 - i) - 20, "");

  //  gLayerTree = ne w   Fl_Tree (4,13+h*i,w*4,h*(20-i)-20,"");
  //  gLayerTree ->type(Fl_Scroll::VERTICAL);
  // gLayerTree ->scrollbar_size(22);
  //  gLayerTree ->CreateTreeView();
  // gLayerTree ->labelcolor(FL_YELLOW);
  // gLayerTree->show();
  gLayerTree->end();
#endif
  wSettings->end();
#endif
#ifdef LEFTFULLSCREEN
#ifdef WIN32
  Fl_Window window(-1000, 10, 300, 370);
#else
  Fl_Window window(0, 0, 1920, 1080);
  window.border(0);
#endif
  window.fullscreen();
#else
  Fl_Window window(0, 0, 1280, 1080);
#endif
  wMain = (Fl_Window *)&window;
  wMain->icon(&ReseorceInit::ic_AppIcon_xpm);
  window.callback(cbMainWindowExit);
  int wm = window.w();
  int hm = window.h();
  DXF3DDisplay sDXF3DDisplay(0, 0, wm, hm, NULL);
  window.resizable(&sDXF3DDisplay);

  gWrk3DSheet = &sDXF3DDisplay;

  gObjectManager = new CObjectManager();

  window.end();
  window.show(/*argc,argv*/ 0, NULL);

  window.fullscreen(); // Wenn vor show aktiviert, zeigt Windowsmanager kein
  // Symbole

  // int FindPixelformat(void) ;
  // i = FindPixelformat();
  if (wSettings)
    wSettings->hide();
  gbFillPolygon = 0;
  ShowSeparate = 1;
  gLookAtIx = 0;
  gbUsePolygonObject = 0;

  if (argc < 2) {
#ifdef WIN32
    // if (gObject)
    // gObject->FileOpen("s:/cad/LonRelais/LonSchalter/DreitafelNurSeite.dxf",1);//TablarMitLoch.dxf",1);//DreitafelTablar.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("s:/cad/LonRelais/LonSchalter/DreitafelTablar.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("d:/cad/HAUS_SCAD/GartenhausVersuch.dxf",1); if
    // (gObjectManager)
    // gObjectManager->FileOpen("d:/cad/tmp/GartenhausRot.dxf",1); if
    // (gObjectManager) gObjectManager->FileOpen("d:/cad/tmp/POLYLINIE.dxf",1);
    if (gObjectManager)
      gObjectManager->FileOpen("d:/cad/Haus_3Tafel/Grundstueck.dxf", 1);
    gbFillPolygon = 0;
    ShowSeparate = 1;
    gLookAtIx = 0;
    gbUsePolygonObject = 0;

//    if (gObjectManager)
//    gObjectManager->FileOpen("d:/cad/tmp/VersuchDreiTafel.dxf",1);
// if (gObjectManager)
// gObjectManager->FileOpen("d:/cad/Haus_3Tafel/Teile/HolzTest.dxf",1);
#else
    // if (gObject)
    // gObject->FileOpen("/home/server/cad/LonRelais/LonSchalter/DreitafelNurTablarMitLoch.dxf",1);//DreitafelTablar.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/server/cad/LonRelais/LonSchalter/DreitafelTablar.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/server/cad/LonRelais/LonSchalter/DreitafelTablar.stl",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/rolf/cad/HAUS_SCAD/GartenhausRot.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/rolf/cad/Haus_3Tafel/VersuchDreiTafel.dxf",1);
    //    if (gObjectManager)
    //    gObjectManager->FileOpen("/home/rolf/cad/Haus_3Tafel/Haus_Keller.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/rolf/cad/Haus_3Tafel/Teile/HolzTest.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/rolf/cad/Haus_3Tafel/Teile/EsszimmerMoebel.dxf",1);
    // if (gObjectManager)
    // gObjectManager->FileOpen("/home/rolf/cad/Haus_3Tafel/Grundstueck.dxf",1);
    if (gObjectManager)
      gObjectManager->FileOpen("/home/rolf/d/cad/Wohnmobil/Wohnmobil.dxf", 1);
#endif
  } else {
    if (gObjectManager)
      gObjectManager->FileOpen(argv[1], 1);
  }

  LookAtLaden(gLookAtIx = -1);

#ifdef WIN32
  Fl_Window *w1 = Fl::first_window();
  if (w1) {
    HWND hwnd = fl_xid(w1);
    char str[200];
    GetWindowText(hwnd, str, 200);
  }
#endif
  extern int bTouchScreen;
  bTouchScreen=1;
  InitFingerTouchService();

  sDXF3DDisplay.show();

  Fl::lock();
  if (0) {
    int ret = Fl::run();
  } else {
    void *next_message;
    int ret = 0; // Fl::run();
    // Fuehre die FLTK-Schleife aus und verarbeite Thread-Nachrichten
#ifdef WIN32
    while (Fl ::wait(10) > 0)
#else
    while (Fl ::wait() > 0)
#endif
    {
      if ((next_message = Fl ::thread_message()) != NULL) {
        /*tMessage * pmsg = (tMessage * )next_message ;
        if (pmsg->receiver) {
          ((CFltkDialogBox*)pmsg->receiver)->WindowProc(WM_COMMAND,pmsg->Code,pmsg->Value);
        }
        */
        if (next_message)
          free(next_message);
        next_message = NULL;
      }
      usleep(10000);
    }

    return ret;
  }
}

#pragma once


// Darf kein  "/GZ" in CPP (Enable Stack Frame Run-Time Error Checking)
// VC2008 Keine Optimation

typedef struct _TOUCHINPUT {
  LONG x;
  LONG y;
  HANDLE hSource;
  DWORD dwID;
  DWORD dwFlags;
  DWORD dwMask;
  DWORD dwTime;
  //ULONG_PTR dwExtraInfo;
  DWORD  * dwExtraInfo;
  DWORD cxContact;
  DWORD cyContact;
} TOUCHINPUT, 
 *PTOUCHINPUT;  




  typedef struct tagGESTUREINFO {
    UINT cbSize;
    DWORD dwFlags;
    DWORD dwID;
    HWND hwndTarget;
    POINTS ptsLocation;
    DWORD dwInstanceID;
    DWORD dwSequenceID;
    ULONGLONG ullArguments;
    UINT cbExtraArgs;
  } GESTUREINFO,*PGESTUREINFO;

  typedef GESTUREINFO const *PCGESTUREINFO;

  typedef struct tagGESTURENOTIFYSTRUCT {
    UINT cbSize;
    DWORD dwFlags;
    HWND hwndTarget;
    POINTS ptsLocation;
    DWORD dwInstanceID;
  } GESTURENOTIFYSTRUCT,*PGESTURENOTIFYSTRUCT;

#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A          
#define GID_BEGIN   1	//A gesture is starting.
#define GID_END     2   //A gesture is ending.
#define GID_ZOOM	3	//The zoom gesture.
#define GID_PAN	    4	//The pan gesture.
#define GID_ROTATE  5	//The rotation gesture.
#define GID_TWOFINGERTAP	6	//The two-finger tap gesture.
#define GID_PRESSANDTAP	7	//The press and tap gesture.


typedef struct tagGESTURECONFIG {
    DWORD dwID;                     // gesture ID
    DWORD dwWant;                   // settings related to gesture ID that are to be turned on
    DWORD dwBlock;                  // settings related to gesture ID that are to be turned off
} GESTURECONFIG, *PGESTURECONFIG;

#define GC_ALLGESTURES                              0x00000001
#define GC_ZOOM                                     0x00000001


// Pan gesture configuration flags - set GESTURECONFIG.dwID to GID_PAN
#define GC_PAN                                      0x00000001
#define GC_PAN_WITH_SINGLE_FINGER_VERTICALLY        0x00000002
#define GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY      0x00000004
#define GC_PAN_WITH_GUTTER                          0x00000008
#define GC_PAN_WITH_INERTIA                         0x00000010

// Rotate gesture configuration flags - set GESTURECONFIG.dwID to GID_ROTATE
#define GC_ROTATE                                   0x00000001

// Two finger tap gesture configuration flags - set GESTURECONFIG.dwID to GID_TWOFINGERTAP
#define GC_TWOFINGERTAP                             0x00000001

// PressAndTap gesture configuration flags - set GESTURECONFIG.dwID to GID_PRESSANDTAP
#define GC_PRESSANDTAP                              0x00000001
#define GC_ROLLOVER                                 GC_PRESSANDTAP




#ifndef WM_TABLET_FIRST 
#define WM_TABLET_FIRST 0x02C0
#endif
#define WM_TABLET_QUERYSYSTEMGESTURESTATUS WM_TABLET_FIRST+12
#define TABLET_DISABLE_PRESSANDHOLD        0x00000001
#define TABLET_DISABLE_PENTAPFEEDBACK      0x00000008
#define TABLET_DISABLE_PENBARRELFEEDBACK   0x00000010
#define TABLET_DISABLE_TOUCHUIFORCEON      0x00000100
#define TABLET_DISABLE_TOUCHUIFORCEOFF     0x00000200
#define TABLET_DISABLE_TOUCHSWITCH         0x00008000
#define TABLET_DISABLE_FLICKS              0x00010000
#define TABLET_ENABLE_FLICKSONCONTEXT      0x00020000
#define TABLET_ENABLE_FLICKLEARNINGMODE    0x00040000
#define TABLET_DISABLE_SMOOTHSCROLLING     0x00080000
#define TABLET_DISABLE_FLICKFALLBACKKEYS   0x00100000
#define TABLET_ENABLE_MULTITOUCHDATA       0x01000000	  
        



typedef struct tagFLICK_DATA {
  //FLICKACTION_COMMANDCODE iFlickActionCommandCode : 5;
  //FLICKDIRECTION          iFlickDirection : 3;
  int iFlickActionCommandCode : 5;
  int          iFlickDirection : 3;
  BOOL                    fControlModifier : 1;
  BOOL                    fMenuModifier : 1;
  BOOL                    fAltGRModifier : 1;
  BOOL                    fWinModifier : 1;
  BOOL                    fShiftModifier : 1;
  INT                     iReserved : 2;
  BOOL                    fOnInkingSurface : 1;
  INT                     iActionArgument : 16;
} FLICK_DATA;

typedef struct tagFLICK_POINT {
  INT x : 16;
  INT y : 16;
} FLICK_POINT;


#define HGESTUREINFO  HANDLE


#ifndef WM_TOUCH 

#define WM_TOUCH 576

#define WM_TOUCHMOVE 576
#define WM_TOUCHDOWN 577
#define WM_TOUCHUP 578

#define TOUCHEVENTF_DOWN	    0x0001
#define TOUCHEVENTF_INRANGE	    0x0008
#define TOUCHEVENTF_MOVE	    0x0002
#define TOUCHEVENTF_NOCOALESCE	0x0020
#define TOUCHEVENTF_PALM	    0x0080
#define TOUCHEVENTF_PEN	        0x0040
#define TOUCHEVENTF_PRIMARY	    0x0010
#define TOUCHEVENTF_UP	        0x0004

#define TOUCHEVENTMASKF_CONTACTAREA	    0x0004
#define TOUCHEVENTMASKF_EXTRAINFO	    0x0002
#define TOUCHEVENTMASKF_TIMEFROMSYSTEM	0x0001


 

typedef BOOL (*tGetTouchInputInfo) (HANDLE hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
typedef BOOL (*tCloseTouchInputHandle)(HANDLE hTouchInput);
typedef BOOL (*tGetGestureInfo)(HGESTUREINFO hGestureInfo,PGESTUREINFO pGestureInfo);
typedef BOOL (*tCloseGestureInfoHandle)(HGESTUREINFO hGestureInfo);
typedef BOOL (*tRegisterTouchWindow)(HWND hWnd, ULONG ulFlags);
typedef BOOL (*tSetGestureConfig)(HWND hwnd,DWORD dwReserved,UINT cIDs,PGESTURECONFIG pGestureConfig,UINT cbSize);                                  // sizeof(GESTURECONFIG)


 


#endif  


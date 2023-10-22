#ifndef RESOURCE_H
#define RESOURCE_H
#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>

class FL_EXPORT Fl_RGB_ImageList : public Fl_RGB_Image
{
public:
  Fl_RGB_ImageList(const Fl_Pixmap *pxm, Fl_Color bg=FL_GRAY);
  virtual ~Fl_RGB_ImageList();
  virtual Fl_Image *copy(int W, int H,int index);
};


#ifdef DREITABLE
class ReseorceInit
{
public:
  ReseorceInit ();
  virtual ~ReseorceInit ();
  static Fl_RGB_Image ic_AppIcon_xpm ;
  static Fl_RGB_Image ic_AppIconSettings_xpm;
  static Fl_Pixmap  TreeViewPics;
  Fl_Image * ButtonImage_List[34];

protected:

private:
};


enum  eImage {imVert,imTee,imVertEnd,imOpenCont,imOpenLast,imCloseCont,imCloseLast,imFolderClose,imFolderOpen,imDokument,imDxfFile,imDxfFilePart,imDxfFileActive,imLayers,imLayersActive,imLayersPart,imLayer,imLayerActive,imNumImage
             };

#else
class ReseorceInit
{
public:
  ReseorceInit ();
  virtual ~ReseorceInit ();
  static Fl_RGB_Image ic_AppIcon_xpm ;
  static Fl_Pixmap  TreeViewPics;
  Fl_Image * ButtonImage_List[35];

protected:

private:
};


enum  eImage {imVert,imTee,imVertEnd,imOpenCont,imOpenLast,imCloseCont,imCloseLast,imFolderClose,imFolderOpen,imDokument,imCompactDsik,
              imDiashow,imPicture,imMusic,imWebBrowser,imRadio,imApllication,iVolumDev,imMusicFolder,imSystem,imNumImage
             };
enum {             imDxfFile=imCompactDsik,imDxfFilePart,imDxfFileActive,imLayers,imLayersActive,imLayersPart,imLayer,imLayerActive};
#endif
/*enum {
iFolderClose,
iDokumentMusic,
iDokumentText
};*/
extern ReseorceInit * R;

enum {
imExit0        =0 ,
imFILEOPEN     =1 ,
imFILESAVE     =2 ,
imUPDATE       =3 ,
imFRAESEN      =4 ,
imDXFSPEZIAL   =5 ,
imBOHREN       =6 ,
imSpeigel      =7 ,
imReset        =8 ,
imNULLPUNKT    =9 ,
imNewDoc       =10,
imCut          =11,
imCopy         =12,
imInsert       =13,
imRefernezfahrt=14,
imZoom2        =15,
imZoomMax      =16,
imZoom1        =17,
imZoomGanz     =18,
imTiefe1       =19,
imTiefe2       =20,
imTiefe3       =21,
imTiefe4       =22,
imTiefe5       =23,
imTiefe6       =24,
imOnline       =26,
imOffline      =27,
imFullSize     =28,
imExit         =29,
imWerkzeug     =30,
imAjust        =31,
imIsel         =32,
imGCode        =33,
imGoCnc        =34
};



#define IDC_FIRSTID             1100
#define IDC_NeuronIDArrived	    1100
#define IDC_Install	            1101
#define IDC_Applc	            1102
#define IDC_Detl	            1103
#define IDC_Adr	                1104
#define IDC_Mng	                1105
#define IDC_VarC	            1106
#define IDC_VarV	            1107
#define IDC_Stat	            1108
#define IDC_OpenData	        1109
#define IDC_WindowFull	        1110
#define IDC_Comm	            1111
#define IDC_WorkBench	        1112
#define IDC_WindowPanorama	    1113
#define IDC_WindowMiniature	    1114
#define IDC_ArrangeWindows	    1115
#define IDC_Visualize	        1116
#define IDC_Connections	        1117
#define IDC_ConfigDownLoad      1118
#define IDC_ScanDomain	        1119
#define IDC_Calculate	        1120
#define IDC_Reporter	        1121
#define IDC_FullSize	        1122
#define IDC_Exit	            1123
#define IDC_Dummy3	            1124
#define IDC_ExpandTree1	        1125
#define IDC_ExpandTree2	        1126
#define IDC_ExpandTree3	        1127
#define IDC_Dummy4	            1128
#define IDC_ExpandTree11	    1129
#define IDC_ExpandTree12	    1130
#define IDC_ExpandTree13	    1131
#define IDC_ExpandTree21	    1132
#define IDC_ExpandTree22	    1133
#define IDC_ExpandTree23	    1134
#define IDC_ShowConnection	    1135
#define IDC_HideConnection	    1136
#define IDC_ShowNode	        1137
#define IDC_HideNode	        1138
#define IDC_SelectAll	        1139
#define IDC_DeleteConnection	1140
#define IDC_CompareConnections	1141
#define IDC_DirectCheck	        1142
#define IDC_RefreshRect	        1143
#define IDC_UnDo	            1144
#define IDC_ReDo	            1145
#define IDC_InsertConnection	1146
#define IDC_Dummy2	            1147
#define IDC_ActivityLog	        1148
#define IDC_ShowModifyList	    1149
#define IDC_ShowUnDoList	    1150
#define IDC_Protocol	        1151
#define IDC_ProtocolVarTest	    1152

#define IDC_Dir0	            1153
#define IDC_NVAddress0	        1154
#define IDC_Prior0		        1155
#define IDC_NeuronIDModify      1156
#define IDC_Turn0	            1170
#define IDC_Service0            1185
#define IDC_primary0	        1190
#define IDC_NVIndex0	        1205
#define IDC_Selector0	        1220
#define IDC_NVValLen0	        1235
#define IDC_NVValue0	        1250

#define IDC_ClearInfo            1264
#define IDC_GetInfo              1265
#define IDC_Fill_in_from_Profile 1266
#define IDC_Send_to_Node         1267
#define IDC_minor_model_num 	 1268
#define IDC_ModelNO	             1269
#define IDC_nv_count             1270
#define IDC_channel_id           1271
#define IDC_STATIC	             1272
#define IDC_location             1273

#define IDC_AddInfo1	1274
#define IDC_AddInfo2	1275
#define IDC_AddInfo3	1276
#define IDC_AddInfo4	1277
#define IDC_AddInfo5	1278
#define IDC_AddInfo6	1279
#define IDC_AddInfo7	1280
#define IDC_AddInfo8	1281
#define IDC_AddInfo9	1282
#define IDC_AddInfo10	1283
#define IDC_AddInfo11	1284
#define IDC_AddInfo12	1285
#define IDC_AddInfo13	1286
#define IDC_AddInfo14	1287
#define IDC_AddInfo15	1288
#define IDC_AddInfo16	1289
#define IDC_address_count	1290
#define IDC_EDIT_AuthKey0 	1291
#define IDC_EDIT_AuthKey1	1292
#define IDC_EDIT_ID0	1293
#define IDC_EDIT_ID1	1294
#define IDC_EDIT_Len0	1295
#define IDC_EDIT_Len1	1296
#define IDC_EDIT_Node0	1297
#define IDC_EDIT_Node1	1298
#define IDC_EDIT_Subnet0	1299
#define IDC_EDIT_Subnet1	1300
#define IDC_EDIT9	1301
#define IDC_EDIT10	1302
#define IDC_EDIT11	1303
#define IDC_EDIT12	1304
#define IDC_EDIT13	1305
#define IDC_EDIT14	1306
#define IDC_EDIT15	1307
#define IDC_EDIT16	1308
#define IDC_Table0	1309
#define IDC_Table1	1310
#define IDC_Table2	1311
#define IDC_Table3	1312
#define IDC_Table4	1313
#define IDC_Table5	1314
#define IDC_Table6	1315
#define IDC_Table7	1316
#define IDC_Table8	1317
#define IDC_Table9	1318
#define IDC_Table10	1319
#define IDC_Table11	1320
#define IDC_Table12	1321
#define IDC_Table13	1322
#define IDC_Table14	1323
#define IDC_UpDate	1324

#define IDC_Application_ID 1225
#define IDC_RouterMode     1226
#define IDC_RouterMode2    1227
#define IDC_RouterFar      1228

#define IDC_GetNXEFile             1229
#define IDC_OnLine                 1230
#define IDC_OffLine                1231
#define IDC_Test                   1232
#define IDC_Reset                  1233
#define IDC_Wink                   1234
#define IDC_Cheksumm               1235
#define IDC_UnConfig               1236
#define IDC_SetConfig              1237
#define IDC_MemoryDisplay          1238
#define IDC_ModelNumber            1239
#define IDC_MemoryDisplay2         1240
#define IDC_NexeFileList           1241
#define IDC_ResetDisplay           1242
#define IDC_DownLoad2              1243
#define IDC_node_state_Application 1244
#define IDC_node_state_Config      1245
#define IDC_node_state_Online      1246
#define IDC_GetNXEFile2            1247
#define IDC_SaveNXEFile            1248
#define IDC_DeviceTypeList         1249
#define IDC_DeviceTypeText         1250
#define IDC_DeviceTypeButton       1251
#define IDC_DelayTrimmTxt          1253
#define IDC_DelayTrimm             1254
#define IDC_NIX1255                1255
#define IDC_NeuronID               1256
#define IDC_DownLoad	           1257
#define IDC_LoadNxeFileFromDisk    1258
#define IDC_ImportNXEFile          1259
#define IDC_GetBIFFile             1260
#define IDC_SaveConnectionList     1261
#define IDC_RegenrateConnection    1262


#define IDC_Temperatur1            1401
#define IDC_Temperatur2            1402
#define IDC_Temperatur3            1403
#define IDC_Temperatur4            1404
#define IDC_Temperatur5            1405
#define IDC_Temperatur6            1406
#define IDC_Temperatur7            1407
#define IDC_Temperatur8            1408
#define IDC_Temperatur9            1409
#define IDC_Temperatur10           1410
#define IDC_Temperatur11           1411
#define IDC_Temperatur12           1412
#define IDC_Temperatur13           1413
#define IDC_Switch14               1414
#define IDC_Switch15               1415
#define IDC_Switch16               1416
#define IDC_Switch17               1417
#define IDC_Switch18               1418
#define IDC_Switch19               1419
#define IDC_Switch20               1420
#define IDC_Switch21               1421
#define IDC_Switch22               1422
#define IDC_Switch23               1423
#define IDC_Switch24               1424
#define IDC_Switch25               1425
#define IDC_Switch26               1426



#define IDC_EinTag                 1463
#define IDC_EineWoche              1464
#define IDC_EinMonat               1465
#define IDC_EinJahr                1466
#define IDC_PlusEinTag             1467
#define IDC_PlusEineWoche          1468
#define IDC_PlusEinMonat           1469
#define IDC_MinusEinTag            1470
#define IDC_MinusEineWoche         1471
#define IDC_MinusEinMonat          1472
#define IDC_StartZeit              1473
#define IDC_StartTime2             1474
#define IDC_EndTime                1475
#define IDC_LoggFile               1476
#define IDC_Reload2                1477
#define IDC_PollNVs                1478
#define IDC_Record1                1479

#define IDC_GetNodeInfo            1480
#define IDC_SaveVar                1481
#define IDC_EditIndex              1482
#define IDC_EditSelector           1483
#define IDC_EditName               1484
#define IDC_EditSNVT               1485
#define IDC_Edit_Min               1486
#define IDC_Edit_Max               1487
#define IDC_GetSDMem               1488
#define IDC_MergDatabase           1489


#define  IDC_LoadFile             2000
#define  IDC_Search               2001
#define  IDC_SaveFile             2002
#define  IDC_CloseFile            2003
#define  IDC_SetSearchText        2004
#define  IDC_GotoNextFound        2005
#define  IDC_GotoPrevEditor       2006
#define  IDC_GotoNextEditor       2007
#define  IDC_GotoNextBookmark     2008
#define  IDC_SetBookmark          2009
#define  IDC_EditSearchText       2010
#define  IDC_BuildProject         2011
#define  IDC_CleanProject         2012
#define  IDC_CloseApplication     2013
#define  IDC_SystemPowerOff       2014
#define  IDC_CamReset             2015
#define  IDC_TreeView             2016
#define  IDC_ModeTreeSelect       2017
#define  IDC_ModeTeleText         2018
#define  IDC_ModeApplication      2019
#define  IDC_ModeTreeContextInfo  2020
#define  IDC_ModeFullscreen       2021
#define  IDC_ExecuteBwindow       2022
#define  IDC_ChangeSysColor       2023
#define  IDC_CD_Eject             2024
#define  IDC_CD_SlowSpeed         2025
#define  IDC_CD_SlowSpeed2        2026
#define  IDC_ShowInfo             2027
#define  IDC_Autorun              2028
#define  IDC_ShowEPGOverview      2029
#define  IDC_ZommFillMode_PS      2030
#define  IDC_ZommFillMode_L       2031
#define  IDC_ButtonPlay           2032
#define  IDC_ButtonPause          2033
#define  IDC_ButtonStop           2034
#define  IDC_ButtonRecord         2035
#define  IDC_ButtonRealtimeStop   2036
#define  IDC_ButtonRealtime       2037
#define  IDC_ButtonReverse        2038
#define  IDC_ButtonForward        2039
#define  IDC_ButtonFastReverse    2040
#define  IDC_ButtonFastForward    2041
#define  IDC_ButtonEject          2042
#define  IDC_ButtonVolumePlus     2043
#define  IDC_ButtonVolumeMinus    2044
#define  IDC_ButtoniSetup         2045
#define  IDC_UpdateScreen         2046
#define  IDC_SystemCtlBox         2047
#define  IDC_AboutBox             2048
#define  IDC_Bright               2049
#define  IDC_Gamma                2050
#define  IDC_Contrast             2051
#define  IDC_Gamma_Dsiplay        2052
#define  IDC_Default              2053
#define  IDC_Save                 2054
#define  IDC_Cancel               2055
#define  IDC_NextPic              2056
#define  IDC_Prev_Pic             2057
#define  IDC_Video_Renderer       2058
#define  IDC_Audio_Renderer       2059
#define  IDC_MPEG_Decoder         2060
#define  IDC_MPEG_Audio_Decoder   2061
// Explorer
#define  IDC_CopyFile             2062
#define  IDC_MoveFileR            2063
#define  IDC_MoveFileL            2064
#define  IDC_DeleteFile           2065
#define  IDC_TouchFile            2066
#define  IDC_ShowDiff             2067
#define  IDC_ShowIntical          2068
#define  IDC_ShowLeftOnly         2069
#define  IDC_ShowRightOnly        2070

#define  IDC_ButtonMute           2071

#define  IDC_SelectTreeItemRecord 2072
#define  IDC_SelectTreeItemMusic  2073
#define  IDC_SelectTreeItemTV     2074
#define  IDC_SelectTreeItemRadio  2075

#define  IDC_SetPlayPosition      2076
#define  IDC_GetPlayPosition      2077
#define  IDC_GetDuration          2078

#define  IDC_TreeModeFile	      2079
#define  IDC_TreeModeProject      2080
#define  IDC_TreeModeVariables    2081
#define  IDC_TreeModeHelp         2082
#define  IDC_StartDebugger        2083
#define  IDC_StopDebugger         2084
#define  IDC_StepDebugger         2085
#define  IDC_StepInToDebugger     2086
#define  IDC_SetBreakPoint        2087
#define  IDC_Browser              2088
#define  IDC_ShowBrowser          2089
#define  IDC_ShowWatch            2090
#define  IDC_ShowBuild            2091
#define  IDC_ShowDebuger          2092
#define  IDC_ShowSystem           2093
#define  IDC_Terminal             2094
#define  IDC_TerminalLogout       2095

#define  IDC_transmission_errors   2096
#define  IDC_transmit_tx_failures  2097
#define  IDC_receive_tx_full       2098
#define  IDC_lost_messages         2099
#define  IDC_missed_messages       2100
#define  IDC_layer2_received       2101
#define  IDC_layer3_received       2102
#define  IDC_layer3_transmitted    2103
#define  IDC_transmit_tx_retries   2104
#define  IDC_backlog_overflows     2105
#define  IDC_late_acknowledgements 2106
#define  IDC_collisions            2107
#define  IDC_Countinues_Test       2108
#define  IDC_error_log             2109
#define  IDC_node_state            2110
#define  IDC_reset_cause           2111
#define  IDC_missed_msgs           2112
#define  IDC_lost_msgs             2113
#define  IDC_rcv_transaction_full  2114
#define  IDC_transaction_timeouts  2115
#define  IDC_xmit_errors           2116
#define  IDC_Retest                2117
#define  IDC_ClearStat             2118
#define  IDC_two_domains           2120
#define  IDC_NV_processing_off          2121
#define  IDC_receive_trans_count        2122
#define  IDC_app_buf_in_size            2123
#define  IDC_app_buf_out_size           2124
#define  IDC_net_buf_in_size            2125
#define  IDC_net_buf_out_size           2126
#define  IDC_app_buf_out_priority_count 2127
#define  IDC_net_buf_out_priority_count 2128
#define  IDC_app_buf_in_count           2129
#define  IDC_app_buf_out_count          2130
#define  IDC_net_buf_in_count           2131
#define  IDC_net_buf_out_count          2132
#define  IDC_eeprom_lock                2133
#define  IDC_read_write_protect         2134
#define  IDC_explicit_addr              2135
#define  IDC_RetestConsts               2136
#define  IDC_nv_count2	                2137
#define  IDC_address_count2          	2138

#define  IDC_NVarConection              2140
#define  IDC_EditNV_Value               2141
#define  IDC_NetVarIx0                  2142
#define  IDC_NetVarIx61                 2203
#define  IDC_NetVarIx63                 2205

#define  IDC_NM_Set_all_selected        2206
#define  IDC_NM_unselected              2207
#define  IDC_NM_Find_unregisterded_Node 2208
#define  IDC_Unregistered_Nodes         2209
//#define  LBS_NOINTEGRALHEIGHT           2210
#define  IDC_Domain0                    2211
#define  IDC_Domain1                    2212
#define  IDC_SendWink                   2213
#define  IDC_NewNode                    2214
#define  IDC_FindUnregistered           2215
#define  IDC_Rectangle1                 2216
#define  IDC_Abbruch                    2217
#define  IDC_Registered_Nodes           2218
#define  IDC_SetNeuronID                2219
#define  IDC_BifFileList                2220
#define  IDC_NewLocation                2221
#define  IDC_NewNodeName                2222
#define  IDC_NewBifFile                 2223
#define  IDC_InsertNewNode              2224
#define  IDC_LoadNodeInfo               2225
#define  IDC_DeleteNode                 2226
#define  IDC_ModifyBif                  2227

#define  IDC_GetBufferFill              2228
#define  IDC_GetPlayerState             2229
#define  IDC_PlayListScanJob            2230
#define  IDC_WatchdogUpdate             2231
#define  IDC_AddTreeItems               2232
#define  IDC_DeleteTreeItems            2233
#define  IDC_GetTreeItems               2234
#define  IDC_SelectTreeItem             2235
#define  IDC_ButtonPrev                 2236
#define  IDC_ButtonNext                 2237
#define  IDC_Button_Red                 2238
#define  IDC_Button_Yellow              2239
#define  IDC_Button_Green               2240
#define  IDC_Button_Blue                2241
#define  IDC_Start_Web_Url              2242
#define  IDC_Start_Youtube_Url          2243



// von Gerber_DXF importiert
#define	BOHR_BlauLoch	2300
#define	BOHR_GelbLoch	2301
#define	BOHR_GruenLoch	2302
#define	BOHR_RotLoch	2303
#define	BOHR_TuerkisLoch	2304
#define	BOHR_WeissLoch	2305
#define	DC_BlauBahn	2306
#define	DC_BlauLoch	2307
#define	DC_GelbBahn	2308
#define	DC_GelbLoch	2309
#define	DC_GruenBahn	2310
#define	DC_GruenLoch	2311
#define	DC_RotBahn	2312
#define	DC_RotLoch	2313
#define	DC_TuerkisBahn	2314
#define	DC_TuerkisLoch	2315
#define	DC_WeissBahn	2316
#define	DC_WeissLoch	2317
#define	DID_progressbar	2318
#define	id_ArbeitsTemp	2319
#define	id_ArbeitsTempT	2320
#define	ID_ASMEDITOOL	2321
#define	ID_AufKopf	2322
#define	id_Aufloesung	2323
#define	id_Aufrauemen	2324
#define	id_BohrTemp	2325
#define	id_BohrTempT	2326
#define	id_Drehen	2327
#define	ID_Einheit	2328
#define	id_EintauchTemp	  2329
#define	id_EintauchTiefe1	2330
#define	id_EintauchTiefe2	2331
#define	id_EintauchTiefe3	2332
#define	id_FraeserDurchm	2333
#define	id_FraeserDurchmT	2334
#define	id_EintauchTempT	2335
#define	id_EintauchTiefe1T	2336
#define	id_StufenTiefe1T	2337
#define	id_EintauchTempo1T	2338
#define	id_BohrTempo1T	2339
#define	id_StufenTiefe1	2340
#define	id_Gehzu	2341
#define	id_GehzuXY	2342
#define	id_GehzuZ	2343
#define	id_Heben	2344
#define	id_HebenT	2345
#define	IDC_WekzeugSet	2346
#define	IDC_WekzeugZDiff  2347
#define	IDC_GehZuNULL   2348
#define	id_Speichern	2349
#define	IDC_WekzeugZOffset 2350
#define	id_VerfahrTemp	2351
#define	id_VerfahrTempT	2352
#define	id_XWerNullT	2353
#define	id_XYNull	2354
#define	id_YWerNullT	2355
#define	id_ZNull	2356
#define	id_ZWerNullT	2357
#define	IDC_WekzeugX	2358
#define	IDC_WekzeugY	2359
#define	IDC_WekzeugZ	2360
//#define	IDC_BlauBahn	2361
//#define	IDC_BlauLoch	2362
#define	IDC_Layer0	2361
#define	IDC_Layer1	2362
#define	IDC_Layer2	2363
#define	IDC_Layer3	2364
#define	IDC_Layer4	2365
#define	IDC_Layer5	2366
#define	IDC_Layer6	2367
#define	IDC_Layer7	2368
#define	IDC_Layer8	2369
#define	IDC_Layer9	2370
#define	IDC_LayerAll	2371
#define	IDC_EditStartX	2372
#define	IDC_EditStartY	2373
#define	IDC_EditTempo	2374
#define	IDC_EditTiefe	2375
#define	IDC_ErsatzbegriffListe	2376
#define	IDC_FILENAME	2377
#define	IDC_FixFang	2378
#define	IDC_FixFangMass	2379
#define	IDC_Fixpunkt	2380
#define	IDC_Fraeser	2381
#define	IDC_GelbBahn	2382
#define	IDC_GelbLoch	2383
#define	IDC_GetFixpunkt	2384
#define	IDC_GetPositionA	2385
#define	IDC_GetPositionE	2386
#define	IDC_GruenBahn	2387
#define	IDC_GruenLoch	2388
#define	IDC_HalbedickeFang	2389
#define	IDC_INFO	2390
#define	IDC_InfoEndpunkt	2391
#define	IDC_InfoKreis	2392
#define	IDC_KeinFang	2393
#define	IDC_Kreisbogen	2394
#define	IDC_DXFDisplay	2395
#define	IDC_Linie	2396
#define	IDC_Loch	2397
#define	IDC_Metrisch	2398
#define	IDM_SetXNull  2399
#define	IDM_SetYNull  2400
#define	IDM_SetZNull	2401
#define	IDC_RADIOOP3	2402
#define	IDC_RADIOOP4	2403
#define	IDC_RotBahn	2404
#define	IDC_RotLoch	2405
#define	IDC_SuchbegriffListe	2406
#define	IDC_Tiefe1	2407
#define	IDC_Tiefe2	2408
#define	IDC_Tiefe3	2409
#define	IDC_Tiefe4	2410
#define	IDC_Tiefe5	2411
#define	IDC_Tiefe6	2412
#define	IDC_TuerkisBahn	2413
#define	IDC_TuerkisLoch	2414
//#define	IDC_UpDate	2415
#define	IDC_WeissBahn	2416
#define	IDC_WeissLoch	2417
#define	idc_XScroll	2418
#define	idc_YScroll	2419
#define	IDC_Zoll	2420
#define	idc_ZScroll	2421
#define	IDD_VERFIRST	2422
#define	IDD_VERLAST	2423
#define	IDI_APPICON	2424
#define	IDI_CHILDICON	2425
#define	IDM_ABOUT	2426
#define	IDM_BOHREN	2427
#define	IDM_BOHRENKEY 2428
#define	IDM_EDITCLEAR	2429
#define	IDM_EDITCOPY	2430
#define	IDM_EDITCOPYALL	2431
#define	IDM_EDITCUT	2432
#define	IDM_EDITPASTE	2433
#define	IDM_EDITUNDO	2434
#define	IDM_EDSCHRIFT	2435
#define	IDM_EINRICHTEN	2436
#define	IDM_EXIT	2437
#define	IDM_FETT	2438
#define	IDM_FILENEW	2439
#define	IDM_FILEPRINT	2441
#define	IDM_FILEPRINTSU	2442
#define	IDM_FILESAVE	2443
#define	IDM_FILESAVEAS	2444
#define	IDM_FIND	2445
#define	IDM_FINDNEXT	2446
#define	IDM_FINDPREV	2447
#define	IDM_FORMATBLAU	2448
#define	IDM_FORMATGRUEN	2449
#define	IDM_FORMATROT	2450
#define	IDM_FRAESEN	2451
#define	IDM_FRAESENKEY 2452
#define	IDM_INSERT	2453
#define	IDM_WIEDERHERSTELLEN	2454
#define	IDM_FAHRGRENZEN  2455


#define	IDM_KURSIV	2455
#define	IDM_OPTIMIZEDXF	2456
#define	IDM_PARAMETER	2457
#define	IDM_Refernezfahrt	2458
#define	IDM_REPLACE	2459
#define	IDM_SAVEDXF	2460
#define	IDM_SCHRIFT	2461
#define	IDM_SCHRIFTMINUS	2462
#define	IDM_SCHRIFTPLUS	2463
#define	IDM_SELECTALL	2464
#define	IDM_SetTextMarke	2465
#define	IDM_STATUSBAR	2466

#define	IDM_Tiefe1	2467
#define	IDM_Tiefe2	2468
#define	IDM_Tiefe3	2469
#define	IDM_Tiefe4	2470
#define	IDM_Tiefe5	2471
#define	IDM_Tiefe6	2472
#define	IDM_FILEOPEN	2473
#define	IDM_DXFSPEZIAL	2474
#define	IDM_STOPP 	2475
#define	IDM_TOOLBAR	2476
#define	IDM_Zoom1	2477
#define	IDM_Zoom2	2478
#define	IDM_ZoomGanz	2479
#define	IDM_ZoomMax	2480
#define	IDM_UPDATE	2481

#define	IDC_AdjustLeft   2482
#define	IDC_AdjustWitdh  2483
#define	IDC_AdjustTop    2484
#define	IDC_AdjustHeight 2485
#define	IDC_CenterX      2486
#define	IDC_CenterY      2487
#define	IDC_CenterZ      2488
#define	IDC_EyeX         2489
#define	IDC_EyeY         2490
#define	IDC_EyeZ         2491
#define	IDC_RotationX    2492
#define	IDC_RotationY    2493
#define	IDC_RotationZ    2494
#define	IDC_Perspective  2495
#define IDC_LightPosX    2496
#define IDC_LightPosY    2497
#define IDC_LightPosZ    2498
#define IDC_LightColorR  2499
#define IDC_LightColorG  2500
#define IDC_LightColorB  2501
#define IDC_Transparency      2502
#define IDC_GrayOut     2503
#define IDC_BackValue    2504
#define	IDC_ShowLeft     2505
#define	IDC_ShowRight    2506
#define	IDC_ShowFront    2507
#define	IDC_ShowBack     2508
#define	IDC_ShowTop      2509
#define	IDC_ShowBottom   2510
#define	IDC_ShowEyeDef   2511
#define	IDC_ShowAnimate  2512
#define	IDC_SetDefault   2513
#define	IDC_GetDefault   2514
#define	IDC_ShowDebug    2515
#define	IDM_Show2D       2516
#define	IDM_Show3D       2517
#define	IDM_DisplayAdj   2518
#define	IDM_ShowDXFTree  2519
#define	IDC_Spindle      2520

#define	IDC_TouchScreen  2521
#define	IDC_SaugerAuto   2522
#define	IDC_GoCnc        2523
#define	IDC_Isel         2524

#define	IDM_Adjust         2525
#define	IDM_DXFColor       2526
#define	IDM_GCode          2527
#define	IDM_LASER	       2528
#define	IDM_NULLPUNKT      2529
#define	IDM_TOPDTOOL       2530
#define	IDM_RUNGCODE       2531
#define	IDC_RapidOverwrite 2532
#define	IDC_FeedOverwrite  2533
#define	IDC_PowerOverwrite 2534
#define	IDC_GcodeRapid     2535
#define	IDC_GcodeFeed      2536
#define	IDC_PowerPercent   2537
#define	IDC_GcodeRapidT    2538
#define	IDC_GcodeFeedT     2539
#define	IDC_PowerPercentT  2540
#define	IDC_LaserPower     2541
#define	IDC_LaserPowerT    2542
#define	IDC_LaserPowerOnF  2543
#define	IDC_FocusPowerOnF  2544
#define	IDC_PowerOff       2545
#define	IDM_LASERENGRAVE   2546
#define	IDC_FocusPower     2547
#define	IDC_FocusPowerT    2548
#define	IDC_LaserSpeed     2549
#define	IDC_LaserSpeedT    2550
#define	IDC_GcodeModusAuto 2551
#define	IDC_GcodeModus1    2552
#define	IDC_GcodeModus5    2553

#define IDC_MoveXYPP1      2554
#define IDC_MoveXYPP10     2555
#define IDC_MoveXYMP10     2556
#define IDC_MoveXYMP1      2557
#define IDC_MoveXP1        2558
#define IDC_MoveXP10       2559
#define IDC_MoveXM10       2560
#define IDC_MoveXM1        2561
#define IDC_MoveYP1        2562
#define IDC_MoveYP10       2563
#define IDC_MoveYM10       2564
#define IDC_MoveYM1        2565
#define IDC_MoveXYPM1      2566
#define IDC_MoveXYPM10     2567
#define IDC_MoveXYMM10     2568
#define IDC_MoveXYMM1      2569
#define IDC_MoveZP1        2570
#define IDC_MoveZP10       2571
#define IDC_MoveZM10       2572
#define IDC_MoveZM1        2573
#define IDC_Move0001       2574
#define IDC_Move0010       2575
#define IDC_Move0100       2576
#define IDC_Move1000       2577
#define IDC_Show3DFace     2578
#define IDC_Background     2579
#define IDC_LookAt1        2580
#define IDC_LookAt2        2581
#define IDC_LookAt3        2582
#define IDC_LookAt4        2583
#define IDC_LookClip       2584

#define IDM_LAYER          2585
#define	IDM_SPINDLE        2586

#define IDC_FraeserType    2587
#define IDC_Material       2588
#define IDM_Calculator     2589
#define IDM_ApplyValues    2590
#define IDM_MaxSpindleRPM  2591
#define IDM_Sauger         2592
#define IDC_AB_SWAP         2593
#define IDC_Drehachse      2594
#define IDC_HideRapidTrans 2595
#define IDC_Durchmesser    2596
#define IDC_GCODEPosStart  2597
#define IDC_GCODEStartTxt  2598
#define IDC_GCODEPosEnd    2599
#define IDC_GCODEEndTxt    2600


#define IDC_FlipDXF         2601
#define IDC_SaveDXF         2602
#define IDC_SetColorRed     2603
#define IDC_SetColorYellow  2604
#define IDC_SetColorGreen   2605
#define IDC_SetColorCyan    2606
#define IDC_SetColorBlue    2607
#define IDC_SetColorMagenta 2608
#define IDC_SelectionAll    2620
#define IDC_SelectionNone   2621
#define IDC_ExpandDXF015    2622
#define IDC_ExpandDXF100    2623
#define IDC_ShrinkDXF015    2624
#define IDC_ShrinkDXF100    2625
#define IDC_MakeDrills      2626
#define IDC_Activate        2627
#define IDC_DeAcitvate      2628
#define IDC_OptimizeDXF     2629


#define IDC_DXFtoCNCTab     2630
#define IDC_AufdemStrich    2631
#define IDC_Gegenlauf       2632
#define IDC_Gleichlauf      2633

#define IDC_MausMoveXY      2634
#define IDC_MausMovez       2635
#define IDC_RotateXY        2636
#define IDC_SelectXY         2637

#define	IDM_GBlock           2638
#define IDC_BlockLaengeS     2639
#define IDC_BlockLaengeE     2640
#define IDC_BlockBreiteS     2641
#define IDC_BlockBreiteE     2642
#define IDC_BlockHoeheS      2643
#define IDC_BlockHoeheE      2644
#define IDC_BlockDurchmesser 2645
#define IDC_AutoUpdate       2646
#define IDC_BlockMaterial    2647
#define IDC_Fortschritt      2648
#define IDC_RundStab         2649
#define IDC_ZeigeBlock       2650
#define IDC_ZeigeGcode       2651
#define IDC_ZeigeBeides      2652
#define IDC_FraeserAuto      2653
#define IDC_FraeserDm        2654
#define IDC_FraeserFormFlach 2655
#define IDC_FraeserFormSpitz 2656
#define IDC_FraeserFormRund  2657
#define IDC_MeisselDurchmesser 2658
#define IDC_StartSimulation  2659



// RadioTelefon
//#define	IDC_ConnectA  2500
//#define	IDC_ConnectB  2501
//#define	IDC_DisConnect 2502
//#define	IDM_Ringtone 2503
//#define	IDM_TelefonList 2504
//#define	IDM_CLIPList 2505
#endif // RESOURCE_H


/*
Copyright (c) 2009-2010 Maximus5
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include "kl_parts.h"
#include "../Common/common.hpp"

#define CES_CMDACTIVE 0x01
#define CES_TELNETACTIVE 0x02
#define CES_FARACTIVE 0x04
#define CES_CONALTERNATIVE 0x08
#define CES_PROGRAMS (0x0F)

//#define CES_NTVDM 0x10 -- common.hpp
#define CES_PROGRAMS2 0xFF

#define CES_FILEPANEL      0x0001
#define CES_FARPANELS      0x000F // �� �������, ������ ��������� ��� ��������� ����� ��������� �������
//#define CES_TEMPPANEL      0x0002
//#define CES_PLUGINPANEL    0x0004
#define CES_EDITOR         0x0010
#define CES_VIEWER         0x0020
#define CES_COPYING        0x0040
#define CES_MOVING         0x0080
#define CES_NOTPANELFLAGS  0xFFF0
#define CES_FARFLAGS       0xFFFF
#define CES_MAYBEPANEL   0x010000
#define CES_WASPROGRESS  0x020000
#define CES_OPER_ERROR   0x040000
//... and so on

// Undocumented console message
#define WM_SETCONSOLEINFO           (WM_USER+201)
// and others
#define SC_RESTORE_SECRET 0x0000f122
#define SC_MAXIMIZE_SECRET 0x0000f032
#define SC_PROPERTIES_SECRET 0x0000fff7
#define SC_MARK_SECRET 0x0000fff2
#define SC_COPY_ENTER_SECRET 0x0000fff0
#define SC_PASTE_SECRET 0x0000fff1
#define SC_SELECTALL_SECRET 0x0000fff5
#define SC_SCROLL_SECRET 0x0000fff3
#define SC_FIND_SECRET 0x0000fff4

#define MAX_TITLE_SIZE 0x400

#define FAR_ALIVE_TIMEOUT gSet.nFarHourglassDelay //1000

#pragma pack(push, 1)


//
//  Structure to send console via WM_SETCONSOLEINFO
//
typedef struct _CONSOLE_INFO
{
    ULONG       Length;
    COORD       ScreenBufferSize;
    COORD       WindowSize;
    ULONG       WindowPosX;
    ULONG       WindowPosY;

    COORD       FontSize;
    ULONG       FontFamily;
    ULONG       FontWeight;
    WCHAR       FaceName[32];

    ULONG       CursorSize;
    ULONG       FullScreen;
    ULONG       QuickEdit;
    ULONG       AutoPosition;
    ULONG       InsertMode;
    
    USHORT      ScreenColors;
    USHORT      PopupColors;
    ULONG       HistoryNoDup;
    ULONG       HistoryBufferSize;
    ULONG       NumberOfHistoryBuffers;
    
    COLORREF    ColorTable[16];

    ULONG       CodePage;
    HWND        Hwnd;

    WCHAR       ConsoleTitle[0x100];

} CONSOLE_INFO;

#pragma pack(pop)

struct ConProcess {
    DWORD ProcessID, ParentPID, InputTID;
    bool  IsFar;
    bool  IsTelnet; // ����� ���� ������� ������ � IsFar, ���� ������� ���������� � ���� ����� ������� ����
    bool  IsNtvdm;  // 16bit ����������
    bool  IsCmd;    // ������ ��� ��������� �������
    bool  NameChecked, RetryName;
    bool  Alive, inConsole;
    TCHAR Name[64]; // ����� ������ ���� �� ������ �������
};

#define MAX_SERVER_THREADS 3
//#define MAX_THREAD_PACKETS 100

class CVirtualConsole;

class CRealConsole
{
#ifdef _DEBUG
    friend class CVirtualConsole;
#endif
public:

    uint TextWidth();
    uint TextHeight();
	uint BufferHeight(uint nNewBufferHeight=0);
    
private:
    HWND    hConWnd;
	//HANDLE  hFileMapping;
	//CESERVER_REQ_CONINFO* pConsoleData;
	//void CloseMapping();

public:
	HWND    ConWnd();

    CRealConsole(CVirtualConsole* apVCon);
    ~CRealConsole();
    
    BOOL PreInit(BOOL abCreateBuffers=TRUE);
    void DumpConsole(HANDLE ahFile);

    BOOL SetConsoleSize(USHORT sizeX, USHORT sizeY, USHORT sizeBuffer=0, DWORD anCmdID=CECMD_SETSIZESYNC);
private:
	BOOL SetConsoleSizeSrv(USHORT sizeX, USHORT sizeY, USHORT sizeBuffer, DWORD anCmdID=CECMD_SETSIZESYNC);
private:
    //void SendConsoleEvent(INPUT_RECORD* piRec);
    DWORD mn_FlushIn, mn_FlushOut;
public:
    void PostConsoleEvent(INPUT_RECORD* piRec);
	void PostConsoleEventPipe(MSG *pMsg);
	LRESULT PostConsoleMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
    BOOL FlushInputQueue(DWORD nTimeout = 500);
    void OnKeyboard(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, wchar_t *pszChars);
    void OnMouse(UINT messg, WPARAM wParam, int x, int y);
    void OnFocus(BOOL abFocused);
    
    void StopSignal();
    void StopThread(BOOL abRecreating=FALSE);
    BOOL isBufferHeight();
	HWND isPictureView(BOOL abIgnoreNonModal=FALSE);
	BOOL isWindowVisible();
    LPCTSTR GetTitle();
    void GetConsoleScreenBufferInfo(CONSOLE_SCREEN_BUFFER_INFO* sbi);
    bool GetConsoleSelectionInfo(CONSOLE_SELECTION_INFO *sel);
	bool IsSelectionAllowed();
    void GetConsoleCursorInfo(CONSOLE_CURSOR_INFO *ci);
    DWORD GetConsoleCP() { return con.m_dwConsoleCP; };
    DWORD GetConsoleOutputCP() { return con.m_dwConsoleOutputCP; };
    DWORD GetConsoleMode() { return con.m_dwConsoleMode; };
    void SyncConsole2Window();
    void OnWinEvent(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
    int  GetProcesses(ConProcess** ppPrc);
    DWORD GetFarPID();
    DWORD GetProgramStatus();
	DWORD GetFarStatus();
    DWORD GetServerPID();
    LRESULT OnScroll(int nDirection);
	LRESULT OnSetScrollPos(WPARAM wParam);
    BOOL isConSelectMode();
    BOOL isFar();
    void ShowConsole(int nMode); // -1 Toggle, 0 - Hide, 1 - Show
    BOOL isDetached();
    BOOL AttachConemuC(HWND ahConWnd, DWORD anConemuC_PID);
    BOOL RecreateProcess(RConStartArgs *args);
    void GetData(wchar_t* pChar, WORD* pAttr, int nWidth, int nHeight);
    void OnActivate(int nNewNum, int nOldNum);
    void OnDeactivate(int nNewNum);
    BOOL CheckBufferSize();
    //LRESULT OnConEmuCmd(BOOL abStarted, DWORD anConEmuC_PID);
    BOOL BufferHeightTurnedOn(CONSOLE_SCREEN_BUFFER_INFO* psbi);
    void UpdateScrollInfo();
    void SetTabs(ConEmuTab* tabs, int tabsCount);
    BOOL GetTab(int tabIdx, /*OUT*/ ConEmuTab* pTab);
    int GetTabCount();
    BOOL ActivateFarWindow(int anWndIndex);
    DWORD CanActivateFarWindow(int anWndIndex);
    void SwitchKeyboardLayout(WPARAM wParam,DWORD_PTR dwNewKeybLayout);
    void CloseConsole();
    void Paste();
    void LogString(LPCSTR asText, BOOL abShowTime = FALSE);
	bool isActive();
	bool isFilePanel(bool abPluginAllowed=false);
	bool isEditor();
	bool isViewer();
	bool isVisible();
	bool isNtvdm();
	//bool isPackets();
	LPCWSTR GetCmd();
	LPCWSTR GetDir();
	short GetProgress(BOOL* rpbError);
	void UpdateFarSettings(DWORD anFarPID=0);
	int CoordInPanel(COORD cr);
	void GetPanelRect(BOOL abRight, RECT* prc);
	bool isAdministrator();
	BOOL isMouseButtonDown();
	void OnConsoleLangChange(DWORD_PTR dwNewKeybLayout);
	DWORD GetConsoleStates();
    void ChangeBufferHeightMode(BOOL abBufferHeight); // ���������� �� TabBar->ConEmu
	void RemoveFromCursor();
	bool isAlive();

public:
    // ���������� �� CVirtualConsole
    BOOL PreCreate(RConStartArgs *args);
		//(BOOL abDetached, LPCWSTR asNewCmd = NULL, BOOL abAsAdmin = FALSE);
    BOOL IsConsoleThread();
    void SetForceRead();
    //DWORD WaitEndUpdate(DWORD dwTimeout=1000);
    LPCWSTR GetConStatus();

protected:
    CVirtualConsole* mp_VCon; // ��������������� ����������� �������
    DWORD mn_ConEmuC_PID, mn_ConEmuC_Input_TID; HANDLE mh_ConEmuC, mh_ConEmuCInput;
	BOOL mb_UseOnlyPipeInput;
    TCHAR ms_ConEmuC_Pipe[MAX_PATH], ms_ConEmuCInput_Pipe[MAX_PATH], ms_VConServer_Pipe[MAX_PATH];
    // ������� ��������� ������� � ��� �������� ��� ��������� (��� ����������� ���������)
    WCHAR Title[MAX_TITLE_SIZE+1], TitleCmp[MAX_TITLE_SIZE+1];
    // � ����� ���������� ��, ��� ������������ � ConEmu (����� ���� ��������� " (Admin)")
    WCHAR TitleFull[MAX_TITLE_SIZE+96];
	// ������������� ������� OnTitleChanged, ��������, ��� ��������� ��������� � �������
	BOOL mb_ForceTitleChanged;
    // ����� ����������� ��������� ���� (� ��������), ����� FAR ����� � ������� ������ (��������� � ��������...).
    WCHAR ms_PanelTitle[CONEMUTABMAX];
    // ����������
    short mn_Progress, mn_PreWarningProgress, mn_ConsoleProgress;
    short CheckProgressInTitle();
    short CheckProgressInConsole(const wchar_t* pszCurLine);

    BOOL AttachPID(DWORD dwPID);
    BOOL StartProcess();
    BOOL StartMonitorThread();
    BOOL mb_NeedStartProcess;

    // ���� ���������� �� ��������
    static DWORD WINAPI MonitorThread(LPVOID lpParameter);
    HANDLE mh_MonitorThread; DWORD mn_MonitorThreadID;
    // ��� ��������� ������� ����� � �������
    //static DWORD WINAPI InputThread(LPVOID lpParameter);
    //HANDLE mh_InputThread; DWORD mn_InputThreadID;
    
    HANDLE mh_TermEvent, mh_MonitorThreadEvent, mh_ApplyFinished;
    BOOL mb_FullRetrieveNeeded; //, mb_Detached;
	RConStartArgs m_Args;
    //wchar_t* ms_SpecialCmd;
	//BOOL mb_RunAsAdministrator;
    

    void Box(LPCTSTR szText);

    //BOOL RetrieveConsoleInfo(/*BOOL bShortOnly,*/ UINT anWaitSize);
	BOOL WaitConsoleSize(UINT anWaitSize, DWORD nTimeout);
    BOOL InitBuffers(DWORD OneBufferSize);
	BOOL LoadDataFromMap(DWORD CharCount);
private:
    // ��� ���������� ���������������� � RetrieveConsoleInfo()
	MSection csCON; //DWORD ncsT;
    struct {
        CONSOLE_SELECTION_INFO m_sel;
        CONSOLE_CURSOR_INFO m_ci;
        DWORD m_dwConsoleCP, m_dwConsoleOutputCP, m_dwConsoleMode;
        CONSOLE_SCREEN_BUFFER_INFO m_sbi;
        USHORT nTopVisibleLine; // ����� ���������� �� m_sbi.srWindow.Top, ���� ��������� �������������
        wchar_t *pConChar;
        WORD  *pConAttr;
		CESERVER_REQ_CONINFO_DATA *pCopy, *pCmp;
        int nTextWidth, nTextHeight, nBufferHeight;
        int nChange2TextWidth, nChange2TextHeight;
        BOOL bBufferHeight; // TRUE, ���� ���� ���������
        //DWORD nPacketIdx;
        DWORD_PTR dwKeybLayout;
        BOOL bRBtnDrag; // � ������� ���������� ���� ������ ������� (��������� � FAR)
        COORD crRBtnDrag;
		BOOL bInSetSize; HANDLE hInSetSize;
		int DefaultBufferHeight;
    } con;
    // 
    MSection csPRC; //DWORD ncsTPRC;
    std::vector<ConProcess> m_Processes;
    int mn_ProcessCount;
    //
    DWORD mn_FarPID, mn_FarInputTID, mn_LastSetForegroundPID;
    //
    ConEmuTab* mp_tabs;
	MSection   msc_Tabs;
    int mn_tabsCount, mn_MaxTabs, mn_ActiveTab;
	BOOL mb_TabsWasChanged;
    void CheckPanelTitle();
    //
    //void ProcessAdd(DWORD addPID);
    //void ProcessDelete(DWORD addPID);
    void ProcessUpdate(const DWORD *apPID, UINT anCount);
    void ProcessUpdateFlags(BOOL abProcessChanged);
    void ProcessCheckName(struct ConProcess &ConPrc, LPWSTR asFullFileName);
    DWORD mn_ProgramStatus, mn_FarStatus;
	BOOL mb_IgnoreCmdStop; // ��� ������� 16bit ���������� �� ���������� ������ �������! ��� ������� OnWinEvent
    BOOL isShowConsole;
    BOOL mb_ConsoleSelectMode;
    static DWORD WINAPI ServerThread(LPVOID lpvParam);
    HANDLE mh_ServerThreads[MAX_SERVER_THREADS], mh_ActiveServerThread;
    DWORD  mn_ServerThreadsId[MAX_SERVER_THREADS];
    HANDLE mh_ServerSemaphore, mh_GuiAttached;
    void SetBufferHeightMode(BOOL abBufferHeight, BOOL abLock=FALSE);
    BOOL mb_BuferModeChangeLocked;

    void ServerThreadCommand(HANDLE hPipe);
    //void ApplyConsoleInfo(CESERVER_REQ* pInfo);
    void SetHwnd(HWND ahConWnd);
    WORD mn_LastVKeyPressed;
    BOOL GetConWindowSize(const CONSOLE_SCREEN_BUFFER_INFO& sbi, int& nNewWidth, int& nNewHeight, BOOL* pbBufferHeight=NULL);
    int mn_Focused; //-1 ����� �������, 1 - � ������, 0 - �� � ������
    DWORD mn_InRecreate; // Tick, ����� ������ ������������
    BOOL mb_ProcessRestarted;
    // ����
    BYTE m_UseLogs;
    HANDLE mh_LogInput; wchar_t *mpsz_LogInputFile/*, *mpsz_LogPackets*/; //UINT mn_LogPackets;
    void CreateLogFiles();
    void CloseLogFiles();
    void LogInput(INPUT_RECORD* pRec);
    //void LogPacket(CESERVER_REQ* pInfo);
    BOOL RecreateProcessStart();
    // ����� � ��������� �������
    //MSection csPKT; //DWORD ncsTPKT;
    //DWORD mn_LastProcessedPkt; //HANDLE mh_PacketArrived;
    //std::vector<CESERVER_REQ*> m_Packets;
    //CESERVER_REQ* m_PacketQueue[(MAX_SERVER_THREADS+1)*MAX_THREAD_PACKETS];
    //void PushPacket(CESERVER_REQ* pPkt);
    //CESERVER_REQ* PopPacket();
	HANDLE mh_FileMapping, mh_FileMappingData;
	wchar_t ms_HeaderMapName[64], ms_DataMapName[64];
	CESERVER_REQ_CONINFO_HDR *mp_ConsoleInfo;
	CESERVER_REQ_CONINFO_DATA *mp_ConsoleData; // Mapping
	DWORD mn_LastConsoleDataIdx, mn_LastConsolePacketIdx, mn_LastFarReadIdx;
	DWORD mn_LastFarReadTick;
	BOOL OpenMapHeader();
	void CloseMapData();
	BOOL ReopenMapData();
	void CloseMapHeader();
	void ApplyConsoleInfo();
	BOOL mb_DataChanged;
    //
    BOOL PrepareOutputFile(BOOL abUnicodeText, wchar_t* pszFilePathName);
    HANDLE PrepareOutputFileCreate(wchar_t* pszFilePathName);
    // ���� ��� dblclick � ���������
    MOUSE_EVENT_RECORD m_LastMouse;
	//
	wchar_t ms_Editor[32], ms_EditorRus[32], ms_Viewer[32], ms_ViewerRus[32];
	wchar_t ms_TempPanel[32], ms_TempPanelRus[32];
	//
	BOOL mb_PluginDetected; DWORD mn_FarPID_PluginDetected, mn_Far_PluginInputThreadId;
	void CheckFarStates();
	void OnTitleChanged();
	DWORD mn_LastInvalidateTick;
	//
	HWND hPictureView; BOOL mb_PicViewWasHidden;
	// ���������� ������� � ��������
	RECT mr_LeftPanel, mr_RightPanel; BOOL mb_LeftPanel, mb_RightPanel;
	void FindPanels();
	// 
	BOOL mb_MouseButtonDown;
	//
	SHELLEXECUTEINFO *mp_sei;
	//
	HWND FindPicViewFrom(HWND hFrom);
	//
	wchar_t ms_ConStatus[80];
	void SetConStatus(LPCWSTR asStatus);
};

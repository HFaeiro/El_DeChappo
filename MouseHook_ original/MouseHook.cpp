// MouseHook.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MouseHook.h"
#define GETTEXT(A, B) (SendMessage(A, WM_GETTEXT, _ARRAYSIZE(B), (LONG_PTR)B))


#define MAX_LOADSTRING 100
HHOOK mHook;
HHOOK kHook;
BOOL DOWN = NULL;
BOOL RBUTTON = NULL;
HWND START = NULL;
HWND AIMING = NULL;
int recoil = NULL;
int TBS = NULL;
BOOL TAB = NULL;
int Primary, Secondary;
BOOL CLASS = NULL;
BOOL FOURX = NULL;
BOOL TWOX = NULL;

//Check Box BOOLS//
BOOL AKM = FALSE;
BOOL UZI = FALSE;
BOOL M16 = FALSE;
BOOL M4 = FALSE;
BOOL SCAR = FALSE;
BOOL TOM = FALSE;
BOOL VEC = FALSE;
BOOL UMP = FALSE;
BOOL PTWOX = FALSE;
BOOL STWOX = FALSE;
BOOL PFOURX = FALSE;
BOOL SFOURX = FALSE;
BOOL DMR = FALSE;
BOOL RELOAD = TRUE; ///// maybe add button? 
BOOL EXT = FALSE;
int estyle;


int GUN = NULL;
int reload = NULL;

HFONT hFontSm = NULL;

double t = NULL;
BOOL HOLD = FALSE;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd = NULL;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MouseHandler(int, WPARAM, LPARAM);
LRESULT CALLBACK KeyboardHandler(int, WPARAM, LPARAM);


int SCREEN_HEIGHT;
int SCREEN_WIDTH;

BOOL SELECTEDHANDLER(LPDRAWITEMSTRUCT, int, BOOL*, int, int);
void UNCHECK(int, BOOL);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	
	kHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardHandler, NULL, NULL);
   
	RECT d;
	HWND hD = GetDesktopWindow();
	GetWindowRect(hD, &d);

	SCREEN_HEIGHT = d.bottom;
	SCREEN_WIDTH = d.right;
	
	// Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOUSEHOOK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOUSEHOOK));

    MSG msg;

    // Main message loop:

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSEHOOK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOUSEHOOK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  // CW_USEDEFAULT, 0, 500, 500, nullptr, nullptr, hInstance, nullptr);
  hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED ,szWindowClass, szTitle, NULL ,
      CW_USEDEFAULT, 0, 250, 40, nullptr, nullptr, hInstance, nullptr);




   if (!hWnd)
   {
      return FALSE;
   }
   SetLayeredWindowAttributes(hWnd, RGB(0x2C, 0x2C, 0x2C), 100, LWA_ALPHA);

   SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
	estyle = GetWindowLong(hWnd, GWL_EXSTYLE);

   ShowWindow(hWnd, nCmdShow);

   UpdateWindow(hWnd);





   return TRUE;

}

static void ModRecoil() {

	if (RELOAD) {
		switch (GUN)
		{
		case IDC_AKM:
		case IDC_M4:
		case IDC_M16:
		case IDC_SCAR:
			reload = 30;
			break;
		case IDC_UZI:
			if (!EXT) {
				reload = 25;
				break;
			}
			reload = 35;
			break;
		case IDC_TOM:
			if (!EXT) {
				reload = 30;
				break;
			}
			reload = 50;
			break;
		case IDC_UMP:
			if (!EXT) {
				reload = 30;
				break;
			}
			reload = 40;
			break;
		case IDC_VEC:
			if (!EXT) {
				reload = 13;
				break;
			}
			reload = 25;
			break;



		}
	}


	std::clock_t t = std::clock();
	int r = recoil;
	int e = NULL;
	int i = 3;
	if (M16)
	{
		i = 2;
	}
	if (FOURX)
		i = 2;
	if (TBS) {
		do {

			double elapsed_secs = double(std::clock() - t) / CLOCKS_PER_SEC;
			if (((elapsed_secs - (double(TBS) / 1000)*i) * 1000) > TBS)
			{
				if(r - recoil < 18)
					r += 1;
				t = std::clock();
				
			}
			e++;
			if (e == reload)
			{
				INPUT i;
				i.type = INPUT_KEYBOARD;
				i.ki.time = 0;
	//			i.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				i.ki.wVk = 0x52;
				SendInput(1, &i, sizeof(i));
				SetWindowText(AIMING, L"Reload");
				break;
			}
			//std::stringstream ss;
			//ss << r;
			//std::string s = ss.str();
			//std::wstring wstr(s.begin(), s.end());

			//SetWindowText(AIMING, wstr.c_str());
			//ShowWindow(AIMING, SW_SHOW);

			RECT dRect;
			GetClientRect(GetDesktopWindow(), &dRect);
			POINT p;
			GetCursorPos(&p);
			p.y = p.y + r;
			p.x += 1;
			INPUT input;
			input.type = INPUT_MOUSE;
			input.mi.time = 0;
			input.mi.dx = p.x * 0xFFFF / SCREEN_WIDTH;
			input.mi.dy = p.y * 0xFFFF / SCREEN_HEIGHT;
			input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;// | MOUSEEVENTF_VIRTUALDESK;
			SendInput(1, &input, sizeof(input));
			Sleep(TBS);


		} while (DOWN);
	}
}

LRESULT CALLBACK KeyboardHandler(int nCode, WPARAM wParam, LPARAM lParam) {

	if(nCode == HC_ACTION){

		if (GetAsyncKeyState(0x47))
		{
			DOWN = FALSE;
			RBUTTON = FALSE;
		}
		if (GetAsyncKeyState(0x52))
		{
			if (RBUTTON)
			{
				DOWN = FALSE;
				RBUTTON = FALSE;
				ShowWindow(AIMING, SW_HIDE);
			}
		}

		if (CLASS > 2)
		{


			if (GetAsyncKeyState(0x31)) {
				UNCHECK(Primary, FALSE);
				if ((PTWOX && !TWOX) || ((TWOX) && (STWOX && !PTWOX)) || (TWOX && !PTWOX))
				{
					DRAWITEMSTRUCT pDIS = { NULL };
					pDIS.hDC = GetDC(hWnd);
					pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
					pDIS.CtlID = IDC_2X;
					GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
					pDIS.itemAction = 401;
					pDIS.itemState = 17;
					SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
				}
				if ((PFOURX && !FOURX) || ((FOURX) && (SFOURX && !PFOURX)) || (FOURX && !PFOURX))
				{
					DRAWITEMSTRUCT pDIS = { NULL };
					pDIS.hDC = GetDC(hWnd);
					pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
					pDIS.CtlID = IDC_4X;
					GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
					pDIS.itemAction = 401;
					pDIS.itemState = 17;
					SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
				}
					

			}
			if (GetAsyncKeyState(0x32)) {
				UNCHECK(Secondary, FALSE);
				if ((!TWOX && STWOX) || ((TWOX) && (PTWOX && !STWOX)) || (TWOX && !STWOX))
				{
				DRAWITEMSTRUCT pDIS = { NULL };
					pDIS.hDC = GetDC(hWnd);
					pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
					pDIS.CtlID = IDC_2X;
					GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
					pDIS.itemAction = 401;		
					pDIS.itemState = 17;
					SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
				}
				if ((SFOURX && !FOURX) || ((FOURX) && (PFOURX && !SFOURX)) || (FOURX && !SFOURX))
				{
					DRAWITEMSTRUCT pDIS = { NULL };
					pDIS.hDC = GetDC(hWnd);
					pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
					pDIS.CtlID = IDC_4X;
					GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
					pDIS.itemAction = 401;
					pDIS.itemState = 17;
					SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
				}


			}

		}


		//if ((GetAsyncKeyState(VK_TAB)) && (!GetAsyncKeyState(VK_MENU)))
		//{
		//	if (GetActiveWindow() != hWnd)
		//	{
		//		if (TAB) {
		//			TAB = FALSE;
		//			ShowWindow(AIMING, SW_HIDE);
		//		}
		//		else {
		//			TAB = TRUE;
		//			RBUTTON = FALSE;
		//			SetWindowText(AIMING, L"TAB");
		//			ShowWindow(AIMING, SW_SHOW);
		//		}
		//	}
		//}

		if (GetAsyncKeyState(VK_MENU))
		{
			if (GetAsyncKeyState(VK_F1))
			{
				DRAWITEMSTRUCT pDIS = { NULL };
				pDIS.hDC = GetDC(hWnd);
				pDIS.hwndItem = GetDlgItem(hWnd, IDB_START);
				pDIS.CtlID = IDB_START;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = ODA_SELECT;
				pDIS.itemState = 17;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}
			if (GetAsyncKeyState(VK_ESCAPE))
				SendMessage(hWnd, WM_COMMAND, IDM_EXIT, NULL);
			if (GetAsyncKeyState(0x31) || GetAsyncKeyState(0x61))
				UNCHECK(IDC_AKM, FALSE);
			if (GetAsyncKeyState(0x32) || GetAsyncKeyState(0x62))
				UNCHECK(IDC_M4, FALSE);
			if (GetAsyncKeyState(0x33) || GetAsyncKeyState(0x63))
				UNCHECK(IDC_M16, FALSE);
			if (GetAsyncKeyState(0x34) || GetAsyncKeyState(0x64))
				UNCHECK(IDC_SCAR, FALSE);
			if (GetAsyncKeyState(0x35) || GetAsyncKeyState(0x65))
				UNCHECK(IDC_UZI, FALSE);
			if (GetAsyncKeyState(0x36) || GetAsyncKeyState(0x66))
				UNCHECK(IDC_TOM, FALSE);
			if (GetAsyncKeyState(0x37) || GetAsyncKeyState(0x67))
				UNCHECK(IDC_UMP, FALSE);
			if (GetAsyncKeyState(0x38) || GetAsyncKeyState(0x68))
				UNCHECK(IDC_VEC, FALSE);
			if (GetAsyncKeyState(VK_OEM_3))
				UNCHECK(IDC_CLASS, FALSE);
			if (GetAsyncKeyState(0x51))
			{
				DRAWITEMSTRUCT pDIS = { NULL };
				pDIS.hDC = GetDC(hWnd);
				pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
				pDIS.CtlID = IDC_2X;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = 401;
				pDIS.itemState = 17;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}
			if (GetAsyncKeyState(0x45))
			{
				DRAWITEMSTRUCT pDIS = { NULL };
				pDIS.hDC = GetDC(hWnd);
				pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
				pDIS.CtlID = IDC_4X;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = 401;
				pDIS.itemState = 17;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}
			if (GetAsyncKeyState(0x46))
			{
				DRAWITEMSTRUCT pDIS = { NULL };
				pDIS.hDC = GetDC(hWnd);
				pDIS.hwndItem = GetDlgItem(hWnd, IDC_EXT);
				pDIS.CtlID = IDC_EXT;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = 401;
				pDIS.itemState = 17;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}
			if (GetAsyncKeyState(0x53))
			{
				DRAWITEMSTRUCT pDIS = { NULL };
				pDIS.hDC = GetDC(hWnd);
				pDIS.hwndItem = GetDlgItem(hWnd, IDC_DMR);
				pDIS.CtlID = IDC_DMR;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = 401;
				pDIS.itemState = 17;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}

			
		}
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			if (TAB || RBUTTON)
			{
				TAB = RBUTTON = FALSE;
				ShowWindow(AIMING, SW_HIDE);
			}
		}
	}
	if (GetAsyncKeyState(0x61))
		UNCHECK(IDC_AKM, FALSE);
	if ( GetAsyncKeyState(0x62))
		UNCHECK(IDC_M4, FALSE);
	if (GetAsyncKeyState(0x63))
		UNCHECK(IDC_M16, FALSE);
	if (GetAsyncKeyState(0x64))
		UNCHECK(IDC_SCAR, FALSE);
	if (GetAsyncKeyState(0x65))
		UNCHECK(IDC_UZI, FALSE);
	if (GetAsyncKeyState(0x66))
		UNCHECK(IDC_TOM, FALSE);
	if (GetAsyncKeyState(0x67))
		UNCHECK(IDC_UMP, FALSE);
	if (GetAsyncKeyState(0x68))
		UNCHECK(IDC_VEC, FALSE);

		return ::CallNextHookEx(kHook, nCode, wParam, lParam);


	
}
LRESULT CALLBACK MouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	


	switch (wParam)
	{

	case WM_LBUTTONDOWN:
	{
		if (RBUTTON)
		{
			DOWN = TRUE;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ModRecoil, NULL, NULL, NULL);
			//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, input.mi.dx, input.mi.dy, NULL, NULL);
		}
		break;

	}
	case WM_LBUTTONUP:
		DOWN = FALSE;
		break;
	case WM_RBUTTONDOWN: 
	{
		
		if (RBUTTON) {
			RBUTTON = FALSE;
			ShowWindow(AIMING, SW_HIDE);
		}
		else if(!TAB && !RBUTTON){
			RBUTTON = TRUE;
		
			t = std::clock();
			SetWindowText(AIMING, L"AIMING");
			ShowWindow(AIMING, SW_SHOW);
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		if (RBUTTON && !TAB)
		{
			
			double elapsed_secs = double(std::clock() - t) / CLOCKS_PER_SEC;
		
			if (elapsed_secs >.25) {
				RBUTTON = FALSE;
				ShowWindow(AIMING, SW_HIDE);
			}
		}
		break;
	}
	default:
		return ::CallNextHookEx(mHook, nCode, wParam, lParam);
	}


	return 0;
}


void UNCHECK(int notThis, BOOL type)
{

	DRAWITEMSTRUCT pDIS = { NULL };
	pDIS.hDC = GetDC(hWnd);
	if (type) {
		for (int i = 401; i <= 410; i++)
		{
			if (i != notThis) {
				pDIS.hwndItem = GetDlgItem(hWnd, i);
				pDIS.CtlID = i;
				GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
				pDIS.itemAction = 400;
				SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
			}
		}
	}
	else {
		pDIS.hwndItem = GetDlgItem(hWnd, notThis);
		pDIS.CtlID = notThis;
		GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
		pDIS.itemAction = 401;
		pDIS.itemState = 17;
		SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
		UNCHECK(notThis, TRUE);


	}
}



void SETCHECK(int This, int rec, int tbs)
{

		UNCHECK(This, TRUE);
		//SETBOOL(This, TRUE);

		if (CLASS == 1) {
			Primary = This;
			if (TWOX)
				PTWOX = This + IDC_2X;
			if (FOURX)
				PFOURX = This + IDC_4X;

			SetWindowText(AIMING, L"Secondary");
			ShowWindow(AIMING, SW_SHOW);
			CLASS = 2;
		}
		else if (CLASS == 2) {
			Secondary = This;
			if (TWOX)
				STWOX = This + IDC_2X;
			if (FOURX)
				SFOURX = This + IDC_4X;

			ShowWindow(AIMING, SW_HIDE);
			CLASS = 3;
		}
		recoil = rec;
		TBS = tbs;

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_NCHITTEST:
	{
		UINT uHitTest;

		uHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		return uHitTest;
		break;
	}
	case WM_CREATE:
	{


		START = CreateWindow(L"BUTTON", L"Off", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 5, 5, 20, 12, hWnd, (HMENU)IDB_START, hInst, NULL);
		AIMING = CreateWindow(L"STATIC", L"AIMING", WS_CHILD | BS_OWNERDRAW, 5, 20, 35, 16, hWnd, (HMENU)IDS_AIMING, hInst, NULL);

		CreateWindow(L"BUTTON", L"AK", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 45, 5, 15, 13, hWnd, (HMENU)IDC_AKM, hInst, NULL);
		CreateWindow(L"BUTTON", L"M4", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 70, 5, 15, 13, hWnd, (HMENU)IDC_M4, hInst, NULL);
		CreateWindow(L"BUTTON", L"M16", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 95, 5, 20, 13, hWnd, (HMENU)IDC_M16, hInst, NULL);
		CreateWindow(L"BUTTON", L"SCAR", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 122, 5, 30, 13, hWnd, (HMENU)IDC_SCAR, hInst, NULL);
		CreateWindow(L"BUTTON", L"UZI", BS_OWNERDRAW | WS_CHILD | WS_VISIBLE, 45, 20, 18, 13, hWnd, (HMENU)IDC_UZI, hInst, NULL);
		CreateWindow(L"BUTTON", L"TOM", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 68, 20, 22, 13, hWnd, (HMENU)IDC_TOM, hInst, NULL);
		CreateWindow(L"BUTTON", L"UMP", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 97, 20, 22, 13, hWnd, (HMENU)IDC_UMP, hInst, NULL);
		CreateWindow(L"BUTTON", L"VEC", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 124, 20, 20, 13, hWnd, (HMENU)IDC_VEC, hInst, NULL);
		CreateWindow(L"BUTTON", L"Single", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 157, 5, 32, 13, hWnd, (HMENU)IDC_DMR, hInst, NULL);

		CreateWindow(L"BUTTON", L"2X", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 200, 5, 15, 13, hWnd, (HMENU)IDC_2X, hInst, NULL);
		CreateWindow(L"BUTTON", L"4X",  WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 217, 5, 15, 13, hWnd, (HMENU)IDC_4X, hInst, NULL);
		CreateWindow(L"BUTTON", L"Class",  WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 202, 20, 30, 13, hWnd, (HMENU)IDC_CLASS, hInst, NULL);
		CreateWindow(L"BUTTON", L"Ext", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 157, 20, 15, 13, hWnd, (HMENU)IDC_EXT, hInst, NULL);



		hFontSm = CreateFont(14, 5, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI ");

		break;
	}
	case WM_CTLCOLORSTATIC:
	{

		HDC hdcButton = (HDC)wParam;
		SetBkColor(hdcButton, RGB(0x2C, 0x2C, 0x2C));
		SetTextColor(hdcButton, RGB(0, 200, 0));
		SelectObject(hdcButton, hFontSm);
		return (LRESULT)CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C));


	}
	case WM_CTLCOLORBTN:
	{

		HDC hdcButton = (HDC)wParam;
		SetBkColor(hdcButton,RGB(0x2C, 0x2C, 0x2C));
		SelectObject(hdcButton, hFontSm);
		return (LRESULT)CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C));


	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
		switch (pDIS->CtlID)
		{
		case IDC_UZI:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &UZI, 3, 48);
		}
		case IDC_AKM:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &AKM, 4, 100);
		}
		case IDC_M4:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &M4, 4, 86);
		}
		case IDC_M16:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &M16, 3, 76);
		}
		case IDC_SCAR:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &SCAR, 4, 96);
		}
		case IDC_UMP://
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &UMP, 3, 92);
		}
		case IDC_TOM:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &TOM, 3, 86);
		}
		case IDC_VEC:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &VEC, 3, 55);
		}
		case IDC_DMR:
		{
			return SELECTEDHANDLER(pDIS, pDIS->CtlID, &DMR, 0, 0);
		}
		case IDC_EXT:
		{
			switch (pDIS->itemAction)
			{
			case 401:
			{
				WCHAR staticText[99];
				int len = GETTEXT(pDIS->hwndItem, staticText);
				if (EXT)
				{
					EXT = FALSE;
				}
				else EXT = TRUE;
				SetWindowText(pDIS->hwndItem, staticText);
				return TRUE;
			}
			case ODA_SELECT:
				if (pDIS->itemState & ODS_SELECTED)
				{

					WCHAR staticText[99];
					int len = GETTEXT(pDIS->hwndItem, staticText);
					if (EXT)
					{
						EXT = FALSE;
					}
					else EXT = TRUE;
					break;
				}
			}
			WCHAR staticText[99];
			int len = GETTEXT(pDIS->hwndItem, staticText);
			if (EXT)
				SetTextColor(pDIS->hDC, RGB(0, 255, 0));
			else
				SetTextColor(pDIS->hDC, RGB(255, 76, 0));
			TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
			return TRUE;
		}
		
		case IDC_CLASS:
		{
			switch (pDIS->itemAction)
			{
			case 401:
			{
				if (CLASS) {
					CLASS = FALSE;
					PFOURX = FALSE;
					PTWOX = FALSE;
					SFOURX = FALSE;
					STWOX = FALSE;
					UNCHECK(NULL, TRUE);
					ShowWindow(AIMING, SW_HIDE);
					if (TWOX)
					{
						DRAWITEMSTRUCT pDIS = { NULL };
						pDIS.hDC = GetDC(hWnd);
						pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
						pDIS.CtlID = IDC_2X;
						GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
						pDIS.itemAction = 401;
						pDIS.itemState = 17;
						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
					}
					if (FOURX) {
						DRAWITEMSTRUCT pDIS = { NULL };
						pDIS.hDC = GetDC(hWnd);
						pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
						pDIS.CtlID = IDC_4X;
						GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
						pDIS.itemAction = 401;
						pDIS.itemState = 17;
						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);

					}
				}
					else {

						SetWindowText(AIMING, L"Primary");
						ShowWindow(AIMING, SW_SHOW);
						CLASS = TRUE;
						SFOURX = FALSE;
						STWOX = FALSE;
						UNCHECK(NULL, TRUE);
						if (TWOX)
						{
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
							pDIS.CtlID = IDC_2X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
						}
						if (FOURX) {
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
							pDIS.CtlID = IDC_4X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
							ShowWindow(AIMING, SW_HIDE);
						}
					}
					WCHAR staticText[99];
					int len = GETTEXT(pDIS->hwndItem, staticText);
					if (CLASS)
						SetTextColor(pDIS->hDC, RGB(0, 255, 0));
					else
						SetTextColor(pDIS->hDC, RGB(255, 76, 0));
					SetWindowText(pDIS->hwndItem, staticText);
					return TRUE;
			}
			case ODA_SELECT:
				if (pDIS->itemState & ODS_SELECTED)
				{

					WCHAR staticText[99];
					int len = GETTEXT(pDIS->hwndItem, staticText);
					if (CLASS) {
					//	SetTextColor(pDIS->hDC, RGB(255, 76, 0));
						CLASS = FALSE;
						PFOURX = FALSE;
						PTWOX = FALSE;
						SFOURX = FALSE;
						STWOX = FALSE;
						UNCHECK(NULL, TRUE);
						ShowWindow(AIMING, SW_HIDE);
						if (TWOX)
						{
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
							pDIS.CtlID = IDC_2X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
						}
						if (FOURX) {
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
							pDIS.CtlID = IDC_4X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
							
						}
					}
					else {
					//	SetTextColor(pDIS->hDC, RGB(0, 255, 0));
						SetWindowText(AIMING, L"Primary");
						ShowWindow(AIMING, SW_SHOW);
						CLASS = TRUE;
						SFOURX = FALSE;
						STWOX = FALSE;
						UNCHECK(NULL, TRUE);
						if (TWOX)
						{
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_2X);
							pDIS.CtlID = IDC_2X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
						}
						if (FOURX) {
							DRAWITEMSTRUCT pDIS = { NULL };
							pDIS.hDC = GetDC(hWnd);
							pDIS.hwndItem = GetDlgItem(hWnd, IDC_4X);
							pDIS.CtlID = IDC_4X;
							GetWindowRect(pDIS.hwndItem, &pDIS.rcItem);
							pDIS.itemAction = 401;
							pDIS.itemState = 17;
							SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
							ShowWindow(AIMING, SW_HIDE);
						}
					}
					//TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
					break;
				}
			}
			WCHAR staticText[99];
			int len = GETTEXT(pDIS->hwndItem, staticText);
			if (CLASS)
				SetTextColor(pDIS->hDC, RGB(0, 255, 0));
			else
				SetTextColor(pDIS->hDC, RGB(255, 76, 0));
			TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
			return TRUE;
		}
		case IDC_2X:
		{
			switch (pDIS->itemAction)
			{
			case 401:
			{
				WCHAR staticText[99];
				int len = GETTEXT(pDIS->hwndItem, staticText);
				if (TWOX) {
					TWOX = FALSE;
					recoil /= 2;
				}
				else {
					recoil *= 2;
					TWOX = TRUE;
				}
				SetWindowText(pDIS->hwndItem, staticText);
				return TRUE;
			}
			case ODA_SELECT:
				if (pDIS->itemState & ODS_SELECTED)
				{

					WCHAR staticText[99];
					int len = GETTEXT(pDIS->hwndItem, staticText);
					if (TWOX) {
						TWOX = FALSE;
						recoil /= 2;
					}
					else {
						recoil *= 2;
						TWOX = TRUE;
					}
					break;
				}
			}
			WCHAR staticText[99];
			int len = GETTEXT(pDIS->hwndItem, staticText);
			if (TWOX)
				SetTextColor(pDIS->hDC, RGB(0, 255, 0));
			else
				SetTextColor(pDIS->hDC, RGB(255, 76, 0));
			TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
			return TRUE;

		}
		case IDC_4X:
		{
			switch (pDIS->itemAction)
			{
			case 401:
			{
				WCHAR staticText[99];
				int len = GETTEXT(pDIS->hwndItem, staticText);
				if (FOURX) {
					FOURX = FALSE;
					recoil /= 4;
				}
				else {
					recoil = recoil * 4;
					FOURX = TRUE;
				}
				SetWindowText(pDIS->hwndItem, staticText);
				return TRUE;
			}
			case ODA_SELECT:
				if (pDIS->itemState & ODS_SELECTED)
				{

					WCHAR staticText[99];
					int len = GETTEXT(pDIS->hwndItem, staticText);
					if (FOURX) {
						FOURX = FALSE;
						recoil /= 4;
					}
					else {
						recoil = recoil * 4;
						FOURX = TRUE;
					}
					break;
				}
			}
				WCHAR staticText[99];
				int len = GETTEXT(pDIS->hwndItem, staticText);
				if (FOURX)
					SetTextColor(pDIS->hDC, RGB(0, 255, 0));
				else
					SetTextColor(pDIS->hDC, RGB(255, 76, 0));
				TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
			return TRUE;
		}
		case IDB_START:
		{
			switch (pDIS->itemAction)
			{
			case ODA_SELECT:
				if (pDIS->itemState & ODS_SELECTED)
				{
					if (mHook == NULL) {
						DOWN = FALSE;
						TAB = FALSE;
						ShowWindow(AIMING, SW_HIDE);
						mHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseHandler, NULL, NULL);
						SetWindowText(START, L"On");
					}
					else {
						DOWN = FALSE;
						TAB = FALSE;
						ShowWindow(AIMING, SW_HIDE);
						UnhookWindowsHookEx(mHook);

						mHook = NULL;
						SetWindowText(START, L"Off");
					}
				}
				break;

			}
			WCHAR staticText[99];
			HWND tHwnd = GetDlgItem(hWnd, IDB_START);
			int len = GETTEXT(tHwnd, staticText);

			if (mHook != NULL)
			{
				SetWindowLong(hWnd, GWL_EXSTYLE, estyle | WS_EX_TRANSPARENT);
				SetTextColor(pDIS->hDC, RGB(0, 255, 0));
			}
			else {
				SetWindowLong(hWnd, GWL_EXSTYLE, estyle );
				SetTextColor(pDIS->hDC, RGB(255, 76, 00));

			}
			//SelectObject(pDIS->hDC, hFontSm);
			TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);


			return TRUE;
		}
		return FALSE;
		}
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL SELECTEDHANDLER(LPDRAWITEMSTRUCT pDIS, int This, BOOL *is, int r, int tbs)
{

	switch (pDIS->itemAction)
	{
	case 401:
	{
		WCHAR staticText[99];
		int len = GETTEXT(pDIS->hwndItem, staticText);
		if (*is) {
			*is = FALSE;
			recoil = NULL;
			TBS = NULL;
		}
		else {
			*is = TRUE;
			GUN = This;
			SETCHECK(pDIS->CtlID, r, tbs);

		}
		SetWindowText(pDIS->hwndItem, staticText);
		return TRUE;
	}
	case 400:
	{
		if (*is) {
			WCHAR staticText[99];
			int len = GETTEXT(pDIS->hwndItem, staticText);
			*is = FALSE;
			SetWindowText(pDIS->hwndItem, staticText);
		}
		return TRUE;
	}

	case ODA_SELECT:
	{
		if (pDIS->itemState & ODS_SELECTED)
		{

			WCHAR staticText[99];
			int len = GETTEXT(pDIS->hwndItem, staticText);
			if (*is) {
				*is = FALSE;
				recoil = NULL;
				TBS = NULL;
			}
			else {
				*is = TRUE;
				GUN = This;
				SETCHECK(pDIS->CtlID, r, tbs);
			}
			return TRUE;
		}
	}
	case ODA_DRAWENTIRE:
	{
		WCHAR staticText[99];
		int len = GETTEXT(pDIS->hwndItem, staticText);
		if (*is)
			SetTextColor(pDIS->hDC, RGB(0, 255, 0));
		else
			SetTextColor(pDIS->hDC, RGB(255, 76, 0));
		TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);

		return TRUE;
	}


	}

}

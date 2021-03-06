// El_DeChappo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "El_DeChappo.h"
#include "Controller.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR CBUTTON[MAX_LOADSTRING];
WCHAR SLIDER[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int estyle;
void Checks(BOOL, Controller::MData* mData, HWND, Controller::D::WeaponData&, LPARAM);




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ELDECHAPPO, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BUTTON, CBUTTON, MAX_LOADSTRING);
	LoadStringW(hInstance, IDB_SLIDER, SLIDER, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	RECT d;
	HWND hD = GetDesktopWindow();
	GetWindowRect(hD, &d);

	Controller::SCREEN_HEIGHT = d.bottom;
	Controller::SCREEN_WIDTH = d.right;
	SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)Controller::KeyHook, NULL, NULL);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ELDECHAPPO));
	//AllocConsole();
	//FILE* pCout;
	//freopen_s(&pCout, "conout$", "w", stdout); 
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


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ELDECHAPPO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW); 
    wcex.hbrBackground  = CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ELDECHAPPO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	Controller::RClass(CS_HREDRAW | CS_VREDRAW, Controller::bProc, hInstance,
		LoadCursor(nullptr, IDC_HAND), LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ELDECHAPPO)),
		CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C)), MAKEINTRESOURCEW(IDC_BUTTON), CBUTTON, sizeof(Controller::CData));
	
	Controller::RClass(CS_HREDRAW | CS_VREDRAW, Controller::SliderProc, hInstance,
		LoadCursor(nullptr, IDC_HAND), LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ELDECHAPPO)),
		CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C)), MAKEINTRESOURCEW(IDB_SLIDER), SLIDER, sizeof(Controller::SData));


    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

 Controller::MAINHWND = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, szWindowClass, szTitle, NULL,
	   CW_USEDEFAULT, 0, 250, 40, nullptr, nullptr, hInstance, nullptr);

	Controller::SLIDERHWND = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, SLIDER, szTitle, NULL,
		CW_USEDEFAULT, 0, 250, 40, Controller::MAINHWND, nullptr, hInstance, nullptr);

  if (!Controller::MAINHWND)
  {
	  return FALSE;
  }
  SetLayeredWindowAttributes(Controller::MAINHWND, RGB(0x2C, 0x2C, 0x2C), 100, LWA_ALPHA);
  SetLayeredWindowAttributes(Controller::SLIDERHWND, RGB(0x2C, 0x2C, 0x2C), 100, LWA_ALPHA);
  SetWindowLong(Controller::MAINHWND, GWL_STYLE, WS_BORDER);
  estyle = GetWindowLong(Controller::MAINHWND, GWL_EXSTYLE);

   ShowWindow(Controller::MAINHWND, nCmdShow);
   UpdateWindow(Controller::MAINHWND);

   SetWindowLong(Controller::SLIDERHWND, GWL_STYLE, WS_BORDER);
   estyle = GetWindowLong(Controller::SLIDERHWND, GWL_EXSTYLE);
   //ShowWindow(Controller::SLIDERHWND, nCmdShow);
   //UpdateWindow(Controller::SLIDERHWND);
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
void InitializeWeaponData(Controller::D::WeaponData &wData, int Clip, int Ext, int recoil, int tbs, BOOL modRecoil)
{
	wData.ClipSize = Clip;
	wData.ExtSize = Ext;
	if(!modRecoil)
		wData.recoil = recoil;
	wData.tbs = tbs;
}
void InitializeWeaponData(Controller::MData* mData)
{
	BOOL modRecoil = Controller::GetGunTxt(*mData);
	
	InitializeWeaponData(mData->WData.AKM, 30, 10, 4, 100, modRecoil);
	InitializeWeaponData(mData->WData.UZI, 25, 10, 3, 48, modRecoil);
	InitializeWeaponData(mData->WData.M4, 30, 10, 4, 86, modRecoil);
	InitializeWeaponData(mData->WData.M16, 30, 10, 3, 75, modRecoil);
	InitializeWeaponData(mData->WData.SCAR, 30, 10, 4, 96, modRecoil);
	InitializeWeaponData(mData->WData.UMP, 30, 10, 3, 92, modRecoil);
	InitializeWeaponData(mData->WData.TOM, 30, 20, 3, 86, modRecoil);
	InitializeWeaponData(mData->WData.VEC, 13, 12, 3, 55, modRecoil);

		return;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Controller::MData* mData = (Controller::MData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);



    switch (message)
    {
	case SET_RECOIL:
	{
		HMENU ActiveMenu = GetMenu(mData->HWND.ACTIVE);
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(ActiveMenu), lParam);
		break;
	}
	case SHOW_SLIDER:
	{
		if (mData->HWND.ACTIVE != NULL) {
			if (mData->Slider)
				mData->Slider = FALSE;
			else mData->Slider = TRUE;
			ShowWindow(Controller::SLIDERHWND, mData->Slider ? SW_HIDE : SW_SHOWNOACTIVATE);
		}
		break;
	}
	case IDF_GETHDATA:
	{
		
		return (LRESULT)&mData->MHData;
		break;
	}
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

		mData = (Controller::MData*)malloc(sizeof(Controller::MData));
		if (mData == NULL)
			return FALSE;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)mData);
		mData->HWND.MAINWINDOW = hWnd;

		mData->MHData.RBUTTON = FALSE;
		mData->MHData.t = NULL;
		mData->MHData.TAB = FALSE;
		mData->MHData.WData.tbs = NULL;
		mData->MHData.WData.EXT = FALSE;
		mData->MHData.CLASS = FALSE;
		mData->start = FALSE;
		InitializeWeaponData(mData);
		RECT r = { NULL };
		r.left = 14;
		r.right = 5;
		mData->HWND.ACTIVE = NULL;
		mData->HWND.Start = Controller::CWindow(r, L"Segoe UI", L"CBUTTON", L"Off", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 5, 5, 20, 12, hWnd, (HMENU)IDB_START, hInst);
		mData->MHData.AIMING = mData->HWND.AIM = Controller::CWindow(r, L"Segoe UI", L"CBUTTON", L"AIMING", WS_CHILD | BS_OWNERDRAW, 5, 20, 35, 16, hWnd, (HMENU)IDS_AIMING, hInst);

		mData->HWND.AK = Controller::CWindow(r, L"Segoe UI", L"CBUTTON", L"AK", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 45, 5, 15, 13, hWnd, (HMENU)IDC_AKM, hInst);
		mData->HWND.M4 = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"M4", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 70, 5, 15, 13, hWnd, (HMENU)IDC_M4, hInst);
		mData->HWND.M16 = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"M16", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 95, 5, 20, 13, hWnd, (HMENU)IDC_M16, hInst);
		mData->HWND.SCAR = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"SCAR", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 122, 5, 30, 13, hWnd, (HMENU)IDC_SCAR, hInst);
		mData->HWND.UZI = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"UZI", BS_OWNERDRAW | WS_CHILD | WS_VISIBLE, 45, 20, 18, 13, hWnd, (HMENU)IDC_UZI, hInst);
		mData->HWND.TOM = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"TOM", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 68, 20, 22, 13, hWnd, (HMENU)IDC_TOM, hInst);
		mData->HWND.UMP = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"UMP", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 97, 20, 22, 13, hWnd, (HMENU)IDC_UMP, hInst);
		mData->HWND.VEC = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"VEC", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 124, 20, 20, 13, hWnd, (HMENU)IDC_VEC, hInst);
		mData->HWND.Single = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"Single", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 157, 5, 32, 13, hWnd, (HMENU)IDC_DMR, hInst);

		//mData->HWND.TWOX = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"2X", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 200, 5, 15, 13, hWnd, (HMENU)IDC_2X, hInst);
		//mData->HWND.FOURX = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"4X", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 217, 5, 15, 13, hWnd, (HMENU)IDC_4X, hInst);
		mData->HWND.Class = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"Class", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 202, 20, 30, 13, hWnd, (HMENU)IDC_CLASS, hInst);
		mData->HWND.Ext = Controller::CWindow(r, L"Segoe UI",L"CBUTTON", L"Ext", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 157, 20, 15, 13, hWnd, (HMENU)IDC_EXT, hInst);
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDB_START:
		{
			if (!mData->start)
			{
				mData->start = TRUE;
				SendMessage(mData->HWND.Start, WM_SETTEXT, NULL, (LPARAM)L"On\0");
				if(lParam)
					SendMessage(mData->HWND.Start, WM_MOUSEACTIVATE, NULL, 1);

				mData->MHData.MHOOK = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)Controller::MouseHook, 0, 0);
				SetWindowLong(hWnd, GWL_EXSTYLE, estyle | WS_EX_TRANSPARENT);
			}
			else
			{
				mData->start = FALSE;
				SendMessage(mData->HWND.Start, WM_SETTEXT, NULL, (LPARAM)L"Off\0");
				UnhookWindowsHookEx(mData->MHData.MHOOK);
				if(lParam)
					SendMessage(mData->HWND.Start, WM_MOUSEACTIVATE, NULL, 1);
				SetWindowLong(hWnd, GWL_EXSTYLE, estyle);
			}
			break;
		}
		case IDC_UZI:
		{

			Checks(mData->MHData.WData.EXT,mData, mData->HWND.UZI, mData->WData.UZI, lParam);
			break;
		}
		case IDC_AKM:
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.AK, mData->WData.AKM, lParam);
			break;
		}
		case IDC_M4:
		{


			Checks(mData->MHData.WData.EXT, mData, mData->HWND.M4, mData->WData.M4, lParam);
			break;
		}
		case IDC_M16:
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.M16, mData->WData.M16, lParam);
			break;
		}
		case IDC_SCAR:
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.SCAR, mData->WData.SCAR, lParam);
			break;
		}
		case IDC_UMP://
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.UMP, mData->WData.UMP, lParam);
			break;
		}
		case IDC_TOM:
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.TOM, mData->WData.TOM, lParam);
			break;
		}
		case IDC_VEC:
		{

			Checks(mData->MHData.WData.EXT, mData, mData->HWND.VEC, mData->WData.VEC, lParam);

			break;
		}
		case IDC_EXT:
		{
			if (!mData->MHData.WData.EXT)
				mData->MHData.WData.EXT = TRUE;
			else
				mData->MHData.WData.EXT = FALSE;

			if (LOWORD(lParam))
				SendMessage(mData->HWND.Ext, WM_MOUSEACTIVATE, 0, 1);
			if (mData->MHData.CLASS > 2 )
				{
					BOOL FOCUS = SendMessage(mData->HWND.Ext, IDF_GETFOCUS, 0, 0);
					if (SendMessage(mData->MHData.PRIMARY.GUN, IDF_GETFOCUS, 0, 0))
						mData->MHData.PRIMARY.EXT = (FOCUS ? 1 : 0);
					else
						mData->MHData.SECONDARY.EXT = FOCUS ? 1 : 0;
				}
			
			break;
		}
		case IDC_DMR:
		{
			Checks(mData->MHData.WData.EXT, mData, mData->HWND.Single, mData->WData.DMR, lParam);
			break;
		}
		case IDC_CLASS:
		{
			if (lParam)
				SendMessage(mData->HWND.Class, WM_MOUSEACTIVATE, 0, 1);
			if (mData->MHData.CLASS)
			{
				mData->MHData.CLASS = FALSE;
				mData->MHData.PRIMARY = { FALSE };
				mData->MHData.SECONDARY = { FALSE };
			}
			else
			{
				mData->MHData.CLASS = TRUE;
				if (mData->HWND.ACTIVE == NULL) {
					SetWindowText(mData->HWND.AIM, L"Primary");
					ShowWindow(mData->HWND.AIM, SW_SHOW);
				}
				else
				{
					mData->MHData.CLASS = 2;
					mData->MHData.PRIMARY.MENU = GetMenu(mData->HWND.ACTIVE);
					mData->MHData.PRIMARY.GUN = mData->HWND.ACTIVE;
					mData->MHData.PRIMARY.EXT = mData->MHData.WData.EXT;
					SendMessage(mData->HWND.AIM, WM_SETTEXT, NULL, (LPARAM)L"Second\0");
					ShowWindow(mData->HWND.AIM, SW_SHOW);
				}
			}

			break;
		}
		case IDM_EXIT:
			if(mData->MHData.MHOOK)
				UnhookWindowsHookEx(mData->MHData.MHOOK);
			Controller::UpdateGunTxt(*mData);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
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

void Checks(BOOL IFEXT, Controller::MData* mData, HWND GUN, Controller::D::WeaponData& wData, LPARAM kb)
{
	int LW = LOWORD(kb);
	int HW = HIWORD(kb);
	if (!HW)
	{
		if (mData->HWND.ACTIVE != NULL && mData->HWND.ACTIVE != GUN)
			SendMessage(mData->HWND.ACTIVE, WM_MOUSEACTIVATE, 1, 1);

		if (mData->HWND.ACTIVE != GUN)
		{
			if (IFEXT)
				wData.EXT = TRUE;
			else
				wData.EXT = FALSE;
			if (LW)
				SendMessage(GUN, WM_MOUSEACTIVATE, 1, 1);
			if (mData->MHData.CLASS)
			{
				if (mData->MHData.CLASS < 2)
				{
					mData->MHData.PRIMARY.MENU = GetMenu(GUN);
					mData->MHData.PRIMARY.GUN = GUN;
					mData->MHData.PRIMARY.EXT = IFEXT;
					mData->MHData.CLASS = 2;
					SetWindowText(mData->HWND.AIM, L"2ndary");
					InvalidateRect(mData->HWND.AIM, 0, TRUE);
				}
				else if (mData->MHData.CLASS == 2)
				{
					mData->MHData.SECONDARY.MENU = GetMenu(GUN);
					mData->MHData.SECONDARY.GUN = GUN;
					mData->MHData.SECONDARY.EXT = IFEXT;
					ShowWindow(mData->HWND.AIM, SW_HIDE);
					mData->MHData.CLASS = 3;
				}
				else
				{
					if (GUN != mData->MHData.SECONDARY.GUN && GUN != mData->MHData.PRIMARY.GUN)
					{
						BOOL FOCUS = SendMessage(mData->HWND.Ext, IDF_GETFOCUS, 0, 0);
						if (mData->HWND.ACTIVE == mData->MHData.PRIMARY.GUN)
						{
							mData->MHData.PRIMARY.EXT = (FOCUS ? 1 : 0);
							mData->MHData.PRIMARY.MENU = GetMenu(GUN);
							mData->MHData.PRIMARY.GUN = GUN;
						}
						else
						{
							mData->MHData.SECONDARY.EXT = FOCUS ? 1 : 0;
							mData->MHData.SECONDARY.MENU = GetMenu(GUN);
							mData->MHData.SECONDARY.GUN = GUN;
						}
					}
					//mData->MHData.CLASS = 3;
				}

			}
			mData->HWND.ACTIVE = GUN;
			mData->MHData.WData = wData;
			SendMessage(Controller::SLIDERHWND, UPDATE_SLIDER, 0, 0);
		}
		else
		{
			if (LW)
				SendMessage(GUN, WM_MOUSEACTIVATE, 1, 1);
			mData->MHData.WData = { NULL };
			mData->HWND.ACTIVE = NULL;
			SendMessage(Controller::SLIDERHWND, UPDATE_SLIDER, 0, 1);
		}

	}
	else
	{
		
		wData.recoil = HW;
		mData->MHData.WData = wData;
		//now write to txt file
	}


	return;
}
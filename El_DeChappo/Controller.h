#pragma once
#define GWL_USERDATA -21
#define IDF_GETFOCUS (WM_USER + 200)
#define IDF_GETHDATA (WM_USER + 201)
#define IDC_AKM					401
#define IDC_UZI					402
#define IDC_M16					403
#define IDC_M4					404
#define IDC_SCAR				405
#define IDC_TOM					406
#define IDC_VEC					407
#define IDC_UMP					408
#define IDC_DMR					409
#include "Resource.h"
#define FILE_NAME L"GunData.txt"
//#define SMESSAGE(A,B) (SendMessage(A, WM_COMMAND, ))

namespace Controller
{
	extern HWND MAINHWND;
	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;
	extern HWND SLIDERHWND;
	void DrawTextC(RECT, HDC, WCHAR*, int);
	void DrawCircle(HDC, POINT, int);
	RECT GetRect(HWND hWnd);

	LRESULT CALLBACK bProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK SliderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND CWindow(RECT, LPCWSTR, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);
	HANDLE CreateCFont(RECT, LPCWSTR);
	ATOM RClass(UINT style, WNDPROC wP, HINSTANCE hInstance, HCURSOR hCur, HICON hIcon, HBRUSH hbBG, LPCWSTR menuName, LPCWSTR ClassName, int size);
	LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK KeyHook(int nCode, WPARAM wParam, LPARAM lParam);

	void rtnl(std::fstream&, int&);


	namespace D {
		struct SliderData {
			BOOL LBDOWN;
			POINT p;
			int x;
			HANDLE sThread;
			int recoil;
		};
		struct ControlData {
			HWND hWnd; // Handle to control
			HWND hFocus;
			WCHAR OGT[99];
			WCHAR Text[99];
			int len;
			int hMenu;
			BOOL FOCUS;
			int DRAWTYPE;
			BOOL BMT;
			HANDLE font;
		};
		struct WeaponData {
			int ClipSize; //
			int ExtSize;
			int recoil;
			int tbs;//Time Between Shots
			BOOL EXT;
		};
		struct CLASSDATA {
			HWND GUN;
			BOOL EXT;
			HMENU MENU;
			//add scopes here
		};
		struct MHOOKDATA {
			HHOOK MHOOK;
			BOOL RBUTTON;
			BOOL DOWN;
			BOOL TAB;
			HWND AIMING;
			BOOL RELOAD;
			BOOL GUN;
			WeaponData WData;
			CLASSDATA PRIMARY;
			CLASSDATA SECONDARY;
			BOOL CLASS;
			int t;
		};

		struct MainData {
			struct HANDLES {
				HWND ACTIVE;
				HWND MAINWINDOW;
				HWND Start;
				HWND AK;
				HWND M4;
				HWND M16;
				HWND SCAR;
				HWND Single;
				HWND TWOX;
				HWND FOURX;
				HWND UZI;
				HWND TOM;
				HWND UMP;
				HWND VEC;
				HWND Ext;
				HWND Class;
				HWND AIM;
			}HWND;
			struct WepDat {
				WeaponData AKM;
				WeaponData UZI;
				WeaponData M16;
				WeaponData M4; 
				WeaponData TOM;
				WeaponData UMP;
				WeaponData VEC;
				WeaponData DMR;
				WeaponData SCAR;
			}WData;
			BOOL Slider;
			BOOL start;
			MHOOKDATA MHData;
			
		};

	};
	static void ModRecoil(D::MHOOKDATA&);
	static void SliderThread(D::SliderData);
	typedef struct D::ControlData CData;
	typedef struct D::MainData MData;
	typedef struct D::SliderData SData;
	


	void UpdateGunTxt(Controller::MData&);
	BOOL GetGunTxt(Controller::MData&);

};
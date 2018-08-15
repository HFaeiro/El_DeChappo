#include "stdafx.h"
#include "Controller.h"
namespace Controller
{
	HWND MAINHWND;
	HWND SLIDERHWND;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
};
void Controller::DrawTextC(RECT r, HDC hdc, WCHAR* staticText, int len)
{

	UINT format = DT_CENTER;// | DT_VCENTER;
	RECT rt = r;
	int height = DrawText(hdc, staticText, len, &rt, format | DT_CALCRECT);
	rt.right = r.right;
	if (rt.bottom < r.bottom)
		OffsetRect(&rt, 0, (r.bottom - rt.bottom) / 2);
	DrawText(hdc, staticText, len, &rt, format);

}
void Controller::DrawCircle(HDC hdc, POINT p, int radius)
{
	Ellipse(hdc, p.x - radius, p.y - radius, p.x + radius, p.y + radius);

}
RECT Controller::GetRect(HWND hWnd)
{
	RECT r = { 0 };
	GetWindowRect(hWnd, &r);
	r.right = r.right - r.left;
	r.left = 0;
	r.bottom = r.bottom - r.top;
	r.top = 0;
	return r;
}

LRESULT Controller::bProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL LBD = FALSE;

	CData* pData = (CData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	int menu = (int)GetMenu(hWnd);
	switch (message)
	{
	case IDF_GETFOCUS:
	{
		BOOL IB = (!pData->FOCUS ? 0 : 1);
		return IB;
	}
	case WM_MOUSEACTIVATE:
	{
		

		if (pData->FOCUS) {
			pData->FOCUS = FALSE;
			pData->BMT = FALSE;

			InvalidateRect(hWnd, 0, TRUE);
		}
		else
		{
			pData->FOCUS = TRUE;
			pData->BMT = FALSE;
			InvalidateRect(hWnd, 0, TRUE);
		}

		HWND parent = GetParent(hWnd);
		if(lParam != 1)
			SendMessage(parent, WM_COMMAND, LOWORD(menu), 0);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (!pData->BMT)
		{
			TRACKMOUSEEVENT TME = { NULL };
			TME.cbSize = sizeof(TRACKMOUSEEVENT);
			TME.dwFlags = TME_LEAVE;
			TME.hwndTrack = hWnd;
			TME.dwHoverTime = NULL;
			pData->BMT = TrackMouseEvent(&TME);
			InvalidateRect(hWnd, 0, TRUE);
		}
		//
		int i = 1;

		break;
	}
	case WM_MOUSELEAVE:
	{

		pData->BMT = FALSE;
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}

	case WM_CREATE:
	{
		if (pData == NULL)
			return FALSE;
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT r = GetRect(hWnd);
		HPEN hp;


		int menu = (int)GetMenu(hWnd);
		SetTextColor(hdc, pData->BMT || pData->FOCUS ?  RGB(0, 255, 0) : RGB(255, 76, 0) );
		WCHAR staticText[99];
		int len = SendMessage(hWnd, WM_GETTEXT,
			_ARRAYSIZE(staticText), (LPARAM)staticText);


		SelectObject(hdc, pData->font);
		SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));
		DrawTextC(r, hdc, staticText, len);


		ReleaseDC(hWnd, hdc);

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
void Controller::SliderThread(D::SliderData sData)
{
	while (sData.LBDOWN){
		RECT r = GetRect(Controller::SLIDERHWND);
		//int center = ((r.bottom - r.top) / 2) - 3;
		if (sData.p.y < r.bottom && sData.p.y > r.top)
		{
			if (sData.p.x > r.left && sData.p.x < r.right)
				if (sData.x != sData.p.x) {
					sData.x = sData.p.x;
					sData.recoil = sData.x / 10;
					InvalidateRect(Controller::SLIDERHWND, 0, TRUE);
				}
		}
		Sleep(1);
		GetCursorPos(&sData.p);
		ScreenToClient(Controller::SLIDERHWND, &sData.p);
	} 
}
LRESULT Controller::SliderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	SData* sData = (SData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	D::MHOOKDATA * MHData = (D::MHOOKDATA*)SendMessage(Controller::MAINHWND, IDF_GETHDATA, 0, 0);

	switch (message)
	{
	case UPDATE_SLIDER:
	{
		if (!lParam) {
			sData->x = MHData->WData.recoil * 10;
			sData->recoil = sData->x / 10;
			InvalidateRect(hWnd, 0, TRUE);
		}
		else
			ShowWindow(hWnd, SW_HIDE);
		break;
	}
	case WM_SHOWWINDOW:
	{
		RECT r;
		GetWindowRect(Controller::MAINHWND, &r);
		int i = ((GetActiveWindow() != hWnd/* && GetActiveWindow() != hWnd*/) ? 1 : 0);
		SetWindowPos(hWnd, 0, r.left, r.top, 0, 0, SWP_NOSIZE | (!i ? NULL :SWP_NOACTIVATE));
		sData->x = MHData->WData.recoil * 10;
		sData->recoil = sData->x / 10;
	}

	case WM_NCHITTEST:
	{

		if (sData->LBDOWN) {
			sData->p = { LOWORD(lParam), HIWORD(lParam) };
			int st = WaitForSingleObject(sData->sThread, 0);
			if (st < 1) {
				sData->sThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SliderThread, (LPVOID)sData, NULL, NULL);
			}
		}
		return  DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
	}
	case WM_MOUSELEAVE:
	{
		sData->LBDOWN = FALSE;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		sData->LBDOWN = TRUE;
		POINT p;

		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);
		LPARAM lp = MAKELPARAM(p.x, p.y);
		SendMessage(hWnd, WM_NCHITTEST, 0, lp);
		break;
	}
	case WM_LBUTTONUP:
	{
		sData->LBDOWN = FALSE;
		//MHData->WData.recoil = sData->recoil;
		LPARAM LP = MAKELPARAM(0, sData->recoil);
		SendMessage(Controller::MAINHWND, SET_RECOIL, 0, LP);
		break;
	}
	case WM_CREATE:
	{

		sData = (SData*)malloc(sizeof(SData*));
		if (sData == NULL)
			return FALSE;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)sData);

		sData->LBDOWN = FALSE;
		//sData->x = MHData->WData.recoil * 10;
		sData->sThread = NULL;
		//sData->recoil = sData->x / 10;
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT r = GetRect(hWnd);
		std::wstring wstr;
		std::wstringstream wss;

		HPEN hp = CreatePen(PS_SOLID, 4, RGB(200, 200, 200));
		HPEN hpOLD = (HPEN)SelectObject(hdc, hp);
			
		int y = ((r.bottom - r.top) / 2) -2;
		int width = r.right - r.left;

		MoveToEx(hdc, 5, y, 0);
		LineTo(hdc, width - 13, y);

		SelectObject(hdc, hpOLD);





		HBRUSH hbr = CreateSolidBrush(RGB(0x00, 0xb1, 0x21));
		SelectObject(hdc, hbr);
		POINT p = { NULL };
		p.x = sData->x;
		p.y = y;
		DrawCircle(hdc, p, 10);
		
		wss << sData->recoil;
		wstr = wss.str();
		SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));
		SetTextColor(hdc, RGB(0, 255, 0));

		
		TextOut(hdc, p.x - 5, p.y -7, wstr.c_str(), wstr.length());


		DeleteObject(hp);
		DeleteObject(hpOLD);
		DeleteObject(hbr);
		ReleaseDC(hWnd, hdc);

		EndPaint(hWnd, &ps);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HWND Controller::CWindow(RECT r, LPCWSTR wstr, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
{

	HWND hWnd = CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, nullptr);
	CData* pData = (CData*)malloc(sizeof(CData));
	if (pData == NULL)
		return FALSE;
	SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pData);
	pData->hWnd = hWnd;
	pData->hMenu = (int)GetMenu(hWnd);
	pData->FOCUS = FALSE;

	pData->len = SendMessage(hWnd, WM_GETTEXT,
		_ARRAYSIZE(pData->OGT), (LPARAM)pData->OGT);

	wcscpy_s(pData->Text, pData->OGT);
	pData->BMT = FALSE;
	pData->font = CreateCFont(r, wstr);
	if (dwStyle == WS_VISIBLE)
		ShowWindow(hWnd, SW_SHOW);
	return hWnd;
}

HANDLE Controller::CreateCFont(RECT r, LPCWSTR wstr)
{
	return CreateFont(r.left, r.right, r.top, r.bottom, FW_EXTRABOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, wstr);
}

ATOM Controller::RClass(UINT style, WNDPROC wP, HINSTANCE hInstance, HCURSOR hCur, HICON hIcon, HBRUSH hbBG, LPCWSTR menuName, LPCWSTR className, int size)
{
	WNDCLASS wc;

	wc.style = style;
	wc.lpfnWndProc = wP;
	wc.hInstance = hInstance;
	wc.hCursor = hCur;
	wc.hIcon = hIcon;
	wc.hbrBackground = hbBG;
	wc.lpszMenuName = menuName;
	wc.lpszClassName = className;
	wc.cbClsExtra = size;
	wc.cbWndExtra = 0;


	return RegisterClassW(&wc);
}

LRESULT Controller::MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	D::MHOOKDATA *MHData = (D::MHOOKDATA*)SendMessage(MAINHWND, IDF_GETHDATA, 0, (LPARAM)&MHData);

	switch (wParam)
	{

	case WM_LBUTTONDOWN:
	{
		if (MHData->RBUTTON)
		{
			MHData->DOWN = TRUE;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ModRecoil, (LPVOID)MHData, NULL, NULL);
			//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, input.mi.dx, input.mi.dy, NULL, NULL);
		}
		break;

	}
	case WM_LBUTTONUP:
		MHData->DOWN = FALSE;
		break;
	case WM_RBUTTONDOWN:
	{

		if (MHData->RBUTTON) {
			MHData->RBUTTON = FALSE;
			ShowWindow(MHData->AIMING, SW_HIDE);
		}
		else if (!MHData->TAB && !MHData->RBUTTON) {
			MHData->RBUTTON = TRUE;

			MHData->t = std::clock();
			SetWindowText(MHData->AIMING, L"AIMING");
			ShowWindow(MHData->AIMING, SW_SHOW);
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		if (MHData->RBUTTON && !MHData->TAB)
		{

			double elapsed_secs = double(std::clock() - MHData->t) / CLOCKS_PER_SEC;

			if (elapsed_secs >.25) {
				MHData->RBUTTON = FALSE;
				ShowWindow(MHData->AIMING, SW_HIDE);
			}
		}
		break;
	}
	default:
		return ::CallNextHookEx(NULL, nCode, wParam, lParam);
	}


	return 0;
}

static void Controller::ModRecoil(D::MHOOKDATA& MHData) {



	//fclose(pCout);

	std::clock_t t = std::clock();
	int r = MHData.WData.recoil;
	int e = NULL;
	int i = 3;
	int ClipSize = MHData.WData.ClipSize;

	if (MHData.WData.EXT)
		ClipSize += MHData.WData.ExtSize;
	//if (M16)
	//{
	//	i = 2;
	//}
	//if (FOURX)
	//	i = 2;
	if (MHData.WData.tbs > 0) {

		do {
			SetWindowText(MHData.AIMING, L"Aiming");
			InvalidateRect(MHData.AIMING, 0, TRUE);
			double elapsed_secs = double(std::clock() - t) / CLOCKS_PER_SEC;
			if (((elapsed_secs - (double(MHData.WData.tbs) / 1000)*i) * 1000) > MHData.WData.tbs)
			{
				if (r - MHData.WData.recoil < 10)
					r++;

				t = std::clock();

			}
			e++;
			if (e == ClipSize)
			{
				INPUT i;
				i.type = INPUT_KEYBOARD;
				i.ki.time = 0;
				i.ki.wVk = 0x52;
				SendInput(1, &i, sizeof(i));
				SetWindowText(MHData.AIMING, L"Reload");
				InvalidateRect(MHData.AIMING, 0, TRUE);
				break;
			}

			RECT dRect;
			GetClientRect(GetDesktopWindow(), &dRect);
			POINT p;
			GetCursorPos(&p);
			p.y = p.y + r*2;
			p.x += 1;
			INPUT input;
			input.type = INPUT_MOUSE;
			input.mi.time = 0;
			input.mi.dx = p.x * 0xFFFF / SCREEN_WIDTH;
			input.mi.dy = p.y * 0xFFFF / SCREEN_HEIGHT;
			input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
			SendInput(1, &input, sizeof(input));
			Sleep(MHData.WData.tbs);

			//printf("%d", MHData.DOWN);

		} while (MHData.DOWN > 0 && MHData.RBUTTON);
	}
		return;
}

LRESULT CALLBACK Controller::KeyHook(int nCode, WPARAM wParam, LPARAM lParam) {

	HWND hWnd = Controller::MAINHWND;
	D::MHOOKDATA *MHData = (D::MHOOKDATA*)SendMessage(MAINHWND, IDF_GETHDATA, 0, (LPARAM)&MHData);
	HWND EXT = GetDlgItem(MAINHWND,IDC_EXT);
	if (nCode == HC_ACTION) {

		if (GetAsyncKeyState(0x47))
		{
			MHData->DOWN = FALSE;
			MHData->RBUTTON = FALSE;
		}
		if (GetAsyncKeyState(0x52))
		{
			if (MHData->RBUTTON)
			{
				MHData->DOWN = FALSE;
				MHData->RBUTTON = FALSE;
				ShowWindow(MHData->AIMING, SW_HIDE);
			}
		}

		if (MHData->CLASS > 2)
		{

				BOOL FOCUS = SendMessage(EXT, IDF_GETFOCUS, 0, 0);

				if (GetAsyncKeyState(0x31) && (!GetAsyncKeyState(VK_MENU))) {
					SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(MHData->PRIMARY.MENU), 1);
					//if (MHData->PRIMARY.EXT && !MHData->WData.EXT)
					if ((MHData->PRIMARY.EXT && !FOCUS) || ((FOCUS && !MHData->PRIMARY.EXT))) {
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));

					}
					else if (MHData->PRIMARY.EXT && !FOCUS)
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));
					else if (!SendMessage(MHData->PRIMARY.GUN, IDF_GETFOCUS, 0, 0) && FOCUS)
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));
					/*if ((PTWOX && !TWOX) || ((TWOX) && (STWOX && !PTWOX)) || (TWOX && !PTWOX))
					{1212

						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
					}
					if ((PFOURX && !FOURX) || ((FOURX) && (SFOURX && !PFOURX)) || (FOURX && !PFOURX))
					{

						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
					}*/


				}
				if (GetAsyncKeyState(0x32) && (!GetAsyncKeyState(VK_MENU))) {
					SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(MHData->SECONDARY.MENU), 1);

					if ((MHData->SECONDARY.EXT && !FOCUS) || ((FOCUS && !MHData->SECONDARY.EXT))) {
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));
					}
					else if (MHData->SECONDARY.EXT && !FOCUS)
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));
					else if (!SendMessage(MHData->SECONDARY.GUN, IDF_GETFOCUS, 0, 0) && FOCUS)
						SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), LOWORD(1));
					/*if ((!TWOX && STWOX) || ((TWOX) && (PTWOX && !STWOX)) || (TWOX && !STWOX))
					{

						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
					}
					if ((SFOURX && !FOURX) || ((FOURX) && (PFOURX && !SFOURX)) || (FOURX && !SFOURX))
					{

						SendMessage(hWnd, WM_DRAWITEM, NULL, (LPARAM)&pDIS);
					}*/


				}
			}
		}
		
			if (GetAsyncKeyState(VK_CAPITAL))
			{
				SendMessage(Controller::MAINHWND, SHOW_SLIDER, 0, 0);
			}
			//if ((GetAsyncKeyState(VK_TAB)) && (!GetAsyncKeyState(VK_MENU)))
			//{
			//	/*if (GetActiveWindow() != hWnd && GetActiveWindow () != Controller::SLIDERHWND)
			//	{
			//		if (MHData->TAB) {
			//			MHData->TAB = FALSE;
			//			ShowWindow(MHData->AIMING, SW_HIDE);
			//		}
			//		else {
			//			MHData->TAB = TRUE;
			//			MHData->RBUTTON = FALSE;
			//			SetWindowText(MHData->AIMING, L"TAB");
			//			ShowWindow(MHData->AIMING, SW_SHOW);
			//		}
			//	}*/
			//	
			//}

		if (GetAsyncKeyState(VK_MENU))
		{
			if (GetAsyncKeyState(VK_F1))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDB_START), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(VK_ESCAPE))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDM_EXIT), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x31) || GetAsyncKeyState(0x61))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_AKM), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x32) || GetAsyncKeyState(0x62))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_M4), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x33) || GetAsyncKeyState(0x63))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_M16), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x34) || GetAsyncKeyState(0x64))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_SCAR), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x35) || GetAsyncKeyState(0x65))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_UZI), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x36) || GetAsyncKeyState(0x66))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_TOM), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x37) || GetAsyncKeyState(0x67))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_UMP), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(0x38) || GetAsyncKeyState(0x68))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_VEC), (LPARAM)LOWORD(1));
			if (GetAsyncKeyState(VK_OEM_3))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_CLASS), (LPARAM)LOWORD(1));
			/*if (GetAsyncKeyState(0x51))
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
			}*/
			if (GetAsyncKeyState(0x46))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_EXT), (LPARAM)LOWORD(1));

			if (GetAsyncKeyState(0x53))
				SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_DMR), (LPARAM)LOWORD(1));


		
	}
	//	if (GetAsyncKeyState(VK_ESCAPE))
	//	{
	//		if (TAB || RBUTTON)
	//		{
	//			TAB = RBUTTON = FALSE;
	//			ShowWindow(AIMING, SW_HIDE);
	//		}
	//	}
	//}
	if (GetAsyncKeyState(0x61))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_AKM), (LPARAM)LOWORD(1));

	if (GetAsyncKeyState(0x62))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_M4), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x63))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_M16), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x64))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_SCAR), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x65))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_UZI), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x66))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_TOM), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x67))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_UMP), (LPARAM)LOWORD(1));
	if (GetAsyncKeyState(0x68))
		SendMessage(hWnd, WM_COMMAND, (WPARAM)LOWORD(IDC_VEC), (LPARAM)LOWORD(1));

	return ::CallNextHookEx(NULL, nCode, wParam, lParam);



}
void Controller::rtnl(std::fstream& fs, int& i)
{
	char c = NULL;
	std::string tmpstr;
	while (fs.peek() != '\n' && fs.peek() != EOF)
	{
		fs.read(&c, 1);
		tmpstr += c;
	}
	i = std::stoi(tmpstr, NULL, 10);
	return;

}
void Controller::UpdateGunTxt(Controller::MData& mData)
{
	std::fstream fs(FILE_NAME, std::fstream::out);
	if (!fs)
		return;
	int counter = NULL;
	char t = NULL;
	fs << "//GunData.txt\n"
	<<	"//Feel free to modify the numbers all you want.\n"
	<<	"//DO NOT add a space between the number and the colon character or there will be undefined behavior.\n"
	<<	"//DO NOT change the order of the guns or delete any lines 	^^\n"
	<<	"//If you want to reset these values to default just delete this file\n\n\n";

	fs << "AKM\t:" << mData.WData.AKM.recoil
	<< "\nM4\t:" <<  mData.WData.M4.recoil
	<< "\nM16\t:" << mData.WData.M16.recoil
	<< "\nSCAR\t:" << mData.WData.SCAR.recoil
	<< "\nUZI\t:" << mData.WData.UZI.recoil
	<< "\nTOM\t:" << mData.WData.TOM.recoil
	<< "\nUMP\t:" << mData.WData.UMP.recoil
	<< "\nVEC\t:" << mData.WData.VEC.recoil;
}
BOOL Controller::GetGunTxt(Controller::MData& mData)
{
	
	std::fstream fs(FILE_NAME);
	if (!fs.is_open())
		return FALSE;
	int counter = NULL;
	char t = NULL;

	while(fs.peek() == '/' && fs.peek() == '\n')
	{
#undef max
		fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	while (fs.peek() != EOF)
	{
		fs.ignore(std::numeric_limits<std::streamsize>::max(), ':');
		switch (counter)
		{
		case 0:
			fs >> mData.WData.AKM.recoil;
			break;
		case 1:
			fs >> mData.WData.M4.recoil;
			break;
		case 2:
			fs >> mData.WData.M16.recoil;
			break;
		case 3:
			fs >> mData.WData.SCAR.recoil;
			break;
		case 4:
			fs >> mData.WData.UZI.recoil;
			break;
		case 5:
			fs >> mData.WData.TOM.recoil;
			break;
		case 6:
			fs >> mData.WData.UMP.recoil;
			break;
		case 7:
			fs >> mData.WData.VEC.recoil;
			break;
		default:
			fs.close();
			return TRUE;

		}
		counter++;




	}

#define max
	fs.close();
	return TRUE;

}

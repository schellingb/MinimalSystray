//--------------------------------------------//
// MinimalSystray                             //
// License: Public Domain (www.unlicense.org) //
//--------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

static DWORD CALLBACK SystrayThread(LPVOID)
{
	struct Wnd
	{
		static LRESULT CALLBACK Proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
		{
			enum EButtons { IDM_NONE, IDM_EXIT };
			static UINT s_WM_TASKBARRESTART;
			if (Msg == WM_COMMAND && wParam == IDM_EXIT) { NOTIFYICONDATAA i; ZeroMemory(&i, sizeof(i)); i.cbSize = sizeof(i); i.hWnd = hWnd; Shell_NotifyIconA(NIM_DELETE, &i); ExitProcess(0); }
			if (Msg == WM_USER && (LOWORD(lParam) == WM_LBUTTONUP || LOWORD(lParam) == WM_RBUTTONUP))
			{
				HMENU hPopMenu = CreatePopupMenu();
				InsertMenuA(hPopMenu,0xFFFFFFFF,MF_STRING|MF_GRAYED,IDM_NONE, "Minimal Systray");
				InsertMenuA(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_NONE,NULL);
				InsertMenuA(hPopMenu,0xFFFFFFFF,MF_STRING,IDM_EXIT,"Exit");
				SetForegroundWindow(hWnd); //cause the popup to be focused
				POINT lpClickPoint;
				GetCursorPos(&lpClickPoint);
				TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
			}
			if (Msg == WM_CREATE || Msg == s_WM_TASKBARRESTART)
			{
				if (Msg == WM_CREATE) s_WM_TASKBARRESTART = RegisterWindowMessageA("TaskbarCreated");
				NOTIFYICONDATAA nid;
				ZeroMemory(&nid, sizeof(nid));
				nid.cbSize = sizeof(nid); 
				nid.hWnd = hWnd;
				nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
				nid.hIcon = LoadIconA(GetModuleHandleA(NULL), "ICN");
				nid.uCallbackMessage = WM_USER; 
				strcpy_s(nid.szTip, sizeof(nid.szTip), "Minimal Systray");
				Shell_NotifyIconA(NIM_ADD, &nid);
			}
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		}
	};
	MSG Msg;
	WNDCLASSA c;
	ZeroMemory(&c, sizeof(c));
	c.lpfnWndProc = Wnd::Proc;
	c.hInstance = GetModuleHandleA(NULL);
	c.lpszClassName = "MINIMALSYSTRAY";
	if (!RegisterClassA(&c) || !CreateWindowA(c.lpszClassName, 0, 0, 0, 0, 0, 0, 0, 0, c.hInstance, 0)) return 1;
	while (GetMessageA(&Msg, 0, 0, 0) > 0) { TranslateMessage(&Msg); DispatchMessageA(&Msg); }
	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE,LPSTR, int)
{
	CreateThread(NULL, 0, SystrayThread, NULL, 0, NULL);
	for (;;) Sleep(10);
}

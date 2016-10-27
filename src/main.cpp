#include "ProgramManager.hpp"

#define __DEBUG

#pragma comment (lib, "gdiplus.lib")

using namespace Gdiplus;    

WCHAR     title_[MAX_LOADSTRING];         //        
WCHAR     wndClassName_[MAX_LOADSTRING];  // ������ � ProgramManager
ProgramManager programManager;
Textures *textures;
Image *background;
Image *table;
Image *cue;
Image *balls[NUMBER_OF_BALLS];

ATOM                MyRegisterClass();
HWND                InitInstance(INT);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND                EnableConsole();
VOID                DoFrame(HWND, Gdiplus::Graphics*, HDC, HBITMAP, HBITMAP);

INT APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     INT       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef __DEBUG 
	if(!EnableConsole()) return EXITS::CONSOLECREATE_FAILED;
#endif

	GdiplusStartupInput gfd;
	ULONG_PTR token = NULL;
	Status st = GdiplusStartup(&token, &gfd, NULL);
    if (st != NULL) MessageBox(NULL, L"", L"FATAL ERROR", MB_OK | MB_ICONERROR);

	programManager.setHINSTANCE(hInstance);
	programManager.loadTitle(title_);
	programManager.loadWndClassName(wndClassName_);

    if(!MyRegisterClass()) return EXITS::WNDCLASS_FAILED;

	HWND hWnd = nullptr;
    if (!(hWnd = InitInstance(nCmdShow))) return EXITS::WNDCREATE_FAILED;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    textures = new Textures();

	background = new Image(L"../src/Images/Background.jpg");
	table = new Image(L"../src/Images/Table.jpg");
	cue = new Image(L"../src/Images/Cue.png");

	for(size_t i = 0; i < NUMBER_OF_BALLS; i++)
	{
		WCHAR wstr[MAX_LOADSTRING] = L"";
		swprintf_s(wstr, L"../src/Images/%d.png", i); // Long

		balls[i] = new Image(wstr);
	}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BILLIARDS));

    MSG msg = { };

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	
		if(GetAsyncKeyState(27)) return EXITS::ESCAPE;
    }

    delete(textures);
	delete(background);
	delete(table);
	delete(cue);

	for(size_t i = 0; i < NUMBER_OF_BALLS; i++) delete(balls[i]);

	GdiplusShutdown(token);

#ifdef __DEBUG 
	PAUSE
#endif
    return static_cast<INT>(msg.wParam);
}

ATOM MyRegisterClass()
{
	//HBITMAP background = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = programManager.getHINSTANCE();
    wcex.hIcon          = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_BILLIARDS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = /*(background)? CreatePatternBrush(background) : */reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BILLIARDS);
    wcex.lpszClassName  = wndClassName_;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HWND InitInstance(INT nCmdShow)
{
   HWND hWnd = CreateWindowW(wndClassName_, title_, WS_OVERLAPPEDWINDOW,
      10, 10, programManager.getMemDCWindow().width, programManager.getMemDCWindow().height, nullptr, nullptr, programManager.getHINSTANCE(), nullptr);

   programManager.setHWND(hWnd);

   return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		SetTimer(hWnd, NULL, 60, NULL);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
            case IDM_ABOUT:
				DialogBox(programManager.getHINSTANCE(), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_MOUSEMOVE:      
		programManager.setMouse(lParam, 0); 
		break;
	case WM_SIZE:
		{
			RECT tmpRect = {};
			GetClientRect(hWnd, &tmpRect); 

			programManager.setMemDCWindow(tmpRect);
		}
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, false);
		break;
    case WM_PAINT:
		{
			programManager.onPAINT(background, table, cue, balls);

			if(programManager.stopBalls()) PostQuitMessage(EXITS::BALLS_STOPPED); 
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

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return static_cast<INT_PTR>(TRUE);

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return static_cast<INT_PTR>(TRUE);
        }
        break;
    }
    return static_cast<INT_PTR>(FALSE);
}

HWND EnableConsole()
{
	if(AllocConsole()) 
	{
		int hCrt = _open_osfhandle(reinterpret_cast<LONG>(GetStdHandle(STD_OUTPUT_HANDLE)), 4);
		if(!hCrt) return FALSE;
  
		*stdout = *_fdopen(hCrt, "w");
		setvbuf(stdout, NULL, _IONBF, 0);

		*stderr = *_fdopen(hCrt, "w");
		setvbuf(stderr, NULL, _IONBF, 0);

		std::ios::sync_with_stdio();
	}
	
	HWND console = GetConsoleWindow();
	SetWindowPos(console, NULL, 0, 0, 700, 500, SWP_NOSIZE | SWP_NOZORDER);

	return console;
}

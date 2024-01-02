#include<windows.h>
#include<math.h> 
#include<stdio.h>
#include<sapi.h>
#include<sphelper.h>
#include "window.h"
#include <ctype.h>
#include<tchar.h>
#include<wchar.h>
#include "HomeWorkClassFactoryDllServerWithRegFile.h"


//Global callback function prototype
#define TEXT_FILE_NUM_CHAR 1048 
#define MAX_MATH 2000


 int selectedVoiceIndex = -1;
 HWND hListBox;
 HFONT g_hFont = NULL;
 BOOL hasExportedWav = FALSE;
 ISpObjectToken* voices[2]; 
 IEnumSpObjectTokens* pIEnumSpObjectTokens = NULL;
 ISpObjectToken* pISpObjectToken = NULL;
 ISpVoice* pISpVoice = NULL;
 ISpStream* pISpStream = NULL;
 ISineWave* pISineWave = NULL;
 BOOL soundOff = FALSE;


 WCHAR wszFirstName[256];
 WCHAR wszSurname[256];
 WCHAR wszWavFileName[512];


 FILE* LogFile = NULL; 
 FILE* RFile = NULL; 


 BOOL g_UserRegistered = FALSE;
 BOOL g_dataRegister =   FALSE;
 BOOL bCatchFirstName_Error = FALSE;
 BOOL bCatchSurname_Error = FALSE;
 BOOL g_ListBox =  FALSE;
 BOOL g_bRadioButtonChecked = FALSE;
 BOOL g_CheckboxChecked = FALSE;
 BOOL g_ExportCheck = FALSE;
 BOOL myFlag =  FALSE;
 BOOL g_LogFileRead = FALSE;

 int currentX = 0;
 int result;
 double phase;

 void SetSelectedVoice(HWND hDlg);
 void InitializeVoices();
 void EnumerateVoices(HWND hDlg);
 void ExportAudioToWav2(ISpObjectToken* pISpObjectToken);
 void OnExportButtonClick2(HWND hDlg);

 INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
 INT_PTR CALLBACK DialogProcAbout(HWND, UINT, WPARAM, LPARAM);
 INT_PTR CALLBACK DialogProcRegister(HWND, UINT, WPARAM, LPARAM);
 BOOL ContainsSpecialCharactersOrNumbers(const TCHAR* str);
 

  int noOfWaves = 1;
  int PointCount = 500;
  int noOfFreq = 5.0;
  HWND hwnd;

 COLORREF g_ChosenColor = RGB(255, 128, 0);
 int plotPointX[NUM_POINTS];
 int plotPointY[NUM_POINTS];

 int finalPlotPointX[NUM_POINTS];
 int finalPlotPointY[NUM_POINTS];
 LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

  double Angle = 0.0;
  double Amplitude = 50.0;

  TCHAR szFileName[512];
  TCHAR szFirstName[256];
  TCHAR szSurname[256];

  char szMultibyteFileName[512];
  SYSTEMTIME lt;

HRESULT hr = S_OK;
ULONG numVoices = 0L;
TCHAR str[255];
WCHAR* DescriptionString = NULL;
CSpStreamFormat audioFormat;
char textBuffer[TEXT_FILE_NUM_CHAR];
TCHAR textBufferWideChar[TEXT_FILE_NUM_CHAR];


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR lpszCmdLine, int iCmdShow)

{
 //variable declarations
	WNDCLASSEX  wndclass;
	TCHAR szClassName[] = TEXT("MyFirstWindow");
	HWND hwnd;
	MSG msg;
	HRESULT hr = S_OK;

	

	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("COM Library Cannot be Initialized.\nProgram Will Now Exit"), TEXT("COM Error"), MB_OK);
		exit(0);
	}

	//code
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(SRI_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(SRI_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;


	//Register window class
	RegisterClassEx(&wndclass);


	hwnd = CreateWindow(szClassName,
		TEXT("Akash Kakade : Sine Wave Application"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, //window left top x-cordinate
		CW_USEDEFAULT, // left-top y-cordinate CW(Create Window)
		CW_USEDEFAULT, // width of window in pixels
		CW_USEDEFAULT, // windows height in pixels
		NULL, // handle of parentdesktop chi window
		NULL,//handel to default menu of this window
		hInstance,
		NULL);


	//show the window
	ShowWindow(hwnd, iCmdShow);

	//Update the window
	UpdateWindow(hwnd);

	//Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		 TranslateMessage(&msg);
		 DispatchMessage(&msg);
	}
	CoUninitialize();
	return((int)msg.wParam);

}

//callback window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{


	//Variable declarations
	HDC hdc = NULL;
	HBRUSH hBrush = NULL;
	CSpStreamFormat audioFormat;
	 WCHAR textBuffer[TEXT_FILE_NUM_CHAR];
	 WCHAR textBufferWideChar[TEXT_FILE_NUM_CHAR];
	 ULONG numVoices = 0L;
	 WCHAR* DescriptionString = NULL;
	 PAINTSTRUCT ps;
	static RECT rect;
	static int CenterX = 0;
	static int CenterY = 0;
	int x;
	int y;
	int PointX = 0;
	int PointY = 0;
	HRESULT hr;
	


	hr = CoCreateInstance(CLSID_SumSubtract, NULL, CLSCTX_INPROC_SERVER, IID_ISineWave, (void**)&pISineWave);
	if (FAILED(hr))
	{
		MessageBox(hwnd, TEXT("ISum Interface Can Not Be Obtained"), TEXT(" COM Error"), MB_OK);
		DestroyWindow(hwnd);
	}
	//code
	switch (iMsg)
	{
	
	case WM_CREATE:

		 hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pISpVoice);
		 if (FAILED(hr))
		{
			 MessageBox(hwnd, TEXT("ISpVoice Interface Can Not Be Obtained"), TEXT(" COM Error"), MB_OK);
			 DestroyWindow(hwnd);
	 	}
		pISpVoice->Speak(L"Press Space Bar to change settings of Sine Wave Simulator and Register User", SPF_ASYNC, NULL);

		for (int i = 0; i < NUM_POINTS; i++) {
			
			plotPointX[i] = -(i * POINT_SIZE);
		}
		for (int i = 0; i < NUM_POINTS; i++) {
			plotPointY[i] = -(i * POINT_SIZE);
		}
		GetClientRect(hwnd, &rect);
		x = rect.left;
	
		SetTimer(hwnd, SRI_TIMER, TIME_INTERVAL_IN_MS, NULL);
		break;

	case WM_PAINT:
		//Step1: Get the painter "HDC"
		hdc = BeginPaint(hwnd, &ps);
		hBrush = CreateSolidBrush(g_ChosenColor); // Use the chosen color
		SelectObject(hdc, hBrush);

		switch (noOfWaves) {
		case 1:
			for (int i = 0; i < NUM_POINTS; i++) {
				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if (i < PointCount) {
					finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
				}
			}

			break;
		case 2:
			for (int i = 0; i < NUM_POINTS; i++) {
				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if (i < PointCount) {
					finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

					finalPlotPointY[i] = CenterY + plotPointY[i] + (1 * WAVE_LEVEL_Y_MULTIPLE); // above center level 1
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
				}
			}

			break;
		case 3:
			for (int i = 0; i < NUM_POINTS; i++) {
				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if (i < PointCount) {
					 finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

					finalPlotPointY[i] = CenterY + plotPointY[i] + (1 * WAVE_LEVEL_Y_MULTIPLE); // above center level 1
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

					finalPlotPointY[i] = CenterY + plotPointY[i] + (2 * WAVE_LEVEL_Y_MULTIPLE); // above center level 1
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
				}
				
			}

			break;
		case 4: 
			for (int i = 0; i < NUM_POINTS; i++) {

				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if (i < PointCount) {
					
					finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

					finalPlotPointY[i] = CenterY + plotPointY[i] + (1 * WAVE_LEVEL_Y_MULTIPLE); // above center level 1
			     	Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);


				   finalPlotPointY[i] = CenterY + plotPointY[i] + (2 * WAVE_LEVEL_Y_MULTIPLE); // above center level 2
				   Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);


				   finalPlotPointY[i] = CenterY + plotPointY[i] + (3 * WAVE_LEVEL_Y_MULTIPLE); // above center level 2
				   Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
					
				}
			}
			break;

		case 5:


			for (int i = 0; i < NUM_POINTS; i++) {

				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if (i < PointCount) {
					   finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (1 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);


						finalPlotPointY[i] = CenterY + plotPointY[i] + (2 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (3 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (4 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
					
				}
			}
			break;

		case 6:
			
			for (int i = 0; i < NUM_POINTS; i++) {

				phase = ((double)i / noOfFreq);
				pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
				finalPlotPointX[i] = plotPointX[i];
				finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
				if ( i < PointCount) {
					       finalPlotPointY[i] = CenterY + plotPointY[i] + (0 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (1 * WAVE_LEVEL_Y_MULTIPLE);
						 Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);


						 finalPlotPointY[i] = CenterY + plotPointY[i] + (2 * WAVE_LEVEL_Y_MULTIPLE);
						 Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (3 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);

						finalPlotPointY[i] = CenterY + plotPointY[i] + (4 * WAVE_LEVEL_Y_MULTIPLE);
						Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
					}
				}
			break;

		default:break;
			}
			
		for (int i = 0; i <  NUM_POINTS; i++) {
			phase = ((double)i / noOfFreq);
			pISineWave->PointOnSineWave(Amplitude, Angle, phase, &plotPointX[i], &plotPointY[i]);
			finalPlotPointX[i] = plotPointX[i];
			finalPlotPointY[i] = CenterY + plotPointY[i] +  (0 * WAVE_LEVEL_Y_MULTIPLE);
			if (i < PointCount) {
				for (int j = 0; j < noOfWaves; j++) {
					Ellipse(hdc, finalPlotPointX[i], finalPlotPointY[i], finalPlotPointX[i] + POINT_SIZE, finalPlotPointY[i] + POINT_SIZE);
				}
			}
			
		}
		if (hBrush) {

			DeleteObject(hBrush);
			hBrush = NULL;
		}
		if (hdc) {

			EndPaint(hwnd, &ps);
			hdc = NULL;
		}
		break;

	case WM_TIMER:
		 KillTimer(hwnd, SRI_TIMER);
		 // animate point along a circular horizontally
		 Angle = Angle + 0.05;
		 if (Angle > 360.0) {
			 Angle = Angle + 0.0;
		 }
		 if((Angle * 20) > rect.right){
			  Angle = 0.0;
		 }
		 InvalidateRect(hwnd, NULL, TRUE);
		 SetTimer(hwnd, SRI_TIMER, TIME_INTERVAL_IN_MS, NULL);

		break;
	case WM_KEYDOWN:
		switch (LOWORD(wParam)) {
		case  VK_ESCAPE:
			 DestroyWindow(hwnd);
			
			break;

		 case VK_SPACE:
			
		     DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG), hwnd, DialogProc);
			
			break;
		default: break;
		}
		break;

	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		CenterX = (rect.right - rect.left) / 2;
		CenterY = (rect.bottom - rect.top) / 2;
		break;

	case WM_DESTROY:
		fclose(LogFile);
		fclose(RFile);
		PostQuitMessage(0);
		break;
	default : break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


// Dialog Procedure
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	 HRESULT hr;

	 CSpStreamFormat audioFormat;
	 char textBuffer[TEXT_FILE_NUM_CHAR];
	 WCHAR textBufferWideChar[TEXT_FILE_NUM_CHAR];
	 ULONG numVoices = 0L;
	 WCHAR* DescriptionString = NULL;


    hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pISpVoice);
	if (FAILED(hr))
	{
		MessageBox(hwnd, TEXT("ISpVoice Interface Can Not Be Obtained"), TEXT(" COM Error"), MB_OK);
		DestroyWindow(hwnd);
	}

	switch (message)
	{
		HWND hListBox;
		HWND ListBox;
	
	case WM_INITDIALOG:
		
		InitializeVoices();
		EnableWindow(GetDlgItem(hDlg, ID_RED), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_YELLOW), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_GREEN), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_BLUE), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_ORANGE), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_MAGENTA), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_CYAN), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_WHITE), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_VOILET), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_PINK), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_INCPOINTS), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_DECPOINTS), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_INCWAVE), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_DECWAVE), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_INCAMP), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_DECAMP), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_INCFREQ), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, ID_DECFREQ), g_UserRegistered);
		EnableWindow(GetDlgItem(hDlg, IDC_LISTBOX), g_ListBox);
		EnableWindow(GetDlgItem(hDlg, IDC_LISTBOX_AUDIO), g_CheckboxChecked);
		EnableWindow(GetDlgItem(hDlg, ID_REWAVE), g_UserRegistered);
	


	
		SendDlgItemMessage(hDlg, ID_VOLUME_OFF, BM_SETCHECK, BST_CHECKED, 0);
		

		hListBox = GetDlgItem(hDlg, IDC_LISTBOX);
		SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)TEXT("Microsoft David Desktop"));
		SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)TEXT("Microsoft Zira Desktop"));
		//SetSelectedVoice(hDlg);

		ListBox = GetDlgItem(hDlg, IDC_LISTBOX_AUDIO);
		 SendDlgItemMessage(hDlg, IDC_LISTBOX_AUDIO, LB_ADDSTRING, 0, (LPARAM)TEXT("Microsoft David Desktop"));
		 SendDlgItemMessage(hDlg, IDC_LISTBOX_AUDIO, LB_ADDSTRING, 0, (LPARAM)TEXT("Microsoft Zira Desktop"));
		// SetSelectedVoice(hDlg);


		 return (INT_PTR)TRUE;

	case WM_CTLCOLORDLG:
	{
	HDC hdcDlg = (HDC)wParam;
	SetBkColor(hdcDlg, RGB(173, 216, 230)); 
	return (INT_PTR)CreateSolidBrush(RGB(173, 216, 230));
	}

	case WM_CTLCOLORSTATIC:
	{
		HDC hdcDlg = (HDC)wParam;
		SetBkColor(hdcDlg, RGB(173, 216, 230)); 
		return (INT_PTR)CreateSolidBrush(RGB(173, 216, 230));
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case  ID_VOLUME_ON:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				
				g_bRadioButtonChecked = IsDlgButtonChecked(hDlg, ID_VOLUME_ON) == BST_CHECKED;

				EnableWindow(GetDlgItem(hDlg, IDC_LISTBOX), g_bRadioButtonChecked);
			}
		
			break;

		case  ID_VOLUME_OFF:

			if (HIWORD(wParam) == BN_CLICKED)
			{
	
				g_bRadioButtonChecked = IsDlgButtonChecked(hDlg, ID_VOLUME_ON) == BST_CHECKED;

				EnableWindow(GetDlgItem(hDlg, IDC_LISTBOX), g_bRadioButtonChecked);
			}
			
			break;

		case ID_EXPORT_LOGS:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_CheckboxChecked = IsDlgButtonChecked(hDlg, ID_EXPORT_LOGS) == BST_CHECKED;
				EnableWindow(GetDlgItem(hDlg, IDC_LISTBOX_AUDIO), g_CheckboxChecked);

			}

			break;

		case IDOK:
			EndDialog(hDlg, IDOK);
			break;

		case ID_ABOUT:
			
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hDlg, DialogProcAbout);
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL); 
			break;

		case ID_EXIT:
	
		  result = MessageBox( hDlg, TEXT("Do you want to really exit the application?"), TEXT("Exit Application"), MB_YESNO | MB_ICONQUESTION);

			if (result ==  6) {

				 fclose(LogFile);
				 if (!g_LogFileRead) {

					 MultiByteToWideChar(CP_ACP, 0, szFirstName, -1, wszFirstName, sizeof(wszFirstName) / sizeof(wszFirstName[0]));
					 MultiByteToWideChar(CP_ACP, 0, szSurname, -1, wszSurname, sizeof(wszSurname) / sizeof(wszSurname[0]));

					 // Create a wide string for the .txt file name

					 swprintf_s(wszWavFileName, L"%s_%s_%02d-%02d-%02d_%02d_hr_%02d_min_%02d_sec.txt",
						 wszFirstName, wszSurname,
						 lt.wDay, lt.wMonth, lt.wYear % 100,
						 lt.wHour, lt.wMinute, lt.wSecond);

					 // Convert wide string to multibyte
					 char szMultibyteWavFileName[512];
					 WideCharToMultiByte(CP_ACP, 0, wszWavFileName, -1, szMultibyteWavFileName, sizeof(szMultibyteWavFileName), NULL, NULL);
					 RFile = _wfopen(wszWavFileName, L"r");
					 OnExportButtonClick2(hDlg);
					  g_LogFileRead = true;
				 }
					fclose(RFile);
				exit(0);
			}
			else if (result == 7) { 
				return(0);
			}


			
			EndDialog(hDlg, ID_EXIT);
			break;

		case ID_RED:
			g_ChosenColor = RGB(255, 0, 0); 
			 SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Red\n");
			pISpVoice->Speak(L"User Changed color to Red ", SPF_ASYNC, NULL);
			break;

		case ID_YELLOW:
			g_ChosenColor = RGB(255, 255, 0);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Yellow\n");
			pISpVoice->Speak(L"User Changed color to Yellow ", SPF_ASYNC, NULL);
			break;

		case ID_GREEN:
			g_ChosenColor = RGB(0, 255, 0);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Green\n");
			pISpVoice->Speak(L"User Changed color to Green ", SPF_ASYNC, NULL);
			 break;

		case ID_BLUE:
			g_ChosenColor = RGB(0, 0, 255);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Blue\n");
			pISpVoice->Speak(L"User Changed color to Blue ", SPF_ASYNC, NULL);
			break;

		case ID_ORANGE:
			g_ChosenColor = RGB(255, 128, 0);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Orange\n");
			pISpVoice->Speak(L"User Changed color to Orange ", SPF_ASYNC, NULL);
			break;

		case ID_MAGENTA:
			g_ChosenColor = RGB(255, 0, 255);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Magenta\n");
			pISpVoice->Speak(L"User Changed color to Magenta ", SPF_ASYNC, NULL);
			break;

		case ID_CYAN:
			g_ChosenColor = RGB(0, 255, 255);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Cyan\n");
			pISpVoice->Speak(L"User Changed color to Cyan ", SPF_ASYNC, NULL);
			break;

		case  ID_WHITE:
			g_ChosenColor = RGB(255, 255, 255);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to White\n");
			pISpVoice->Speak(L"User Changed color to White ", SPF_ASYNC, NULL);
			break;

		case ID_VOILET:
			g_ChosenColor = RGB(148, 0, 211);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Violet\n");
			pISpVoice->Speak(L"User Changed color to Violet ", SPF_ASYNC, NULL);
			break;

		case ID_PINK:
			g_ChosenColor = RGB(255, 192, 203);
			SetSelectedVoice(hDlg);
			fprintf(LogFile, " User Changed color to Pink\n");
			pISpVoice->Speak(L"User Changed color to Pink ", SPF_ASYNC, NULL);
			break;

		case ID_REGISTER_USER:
			// Handle the "Register User" button click here
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_REGISTER), GetActiveWindow(), DialogProcRegister);
			SendMessage(hDlg, WM_INITDIALOG, 0, 0);
			break;
		case ID_REWAVE:
			for (int i = 0; i <  NUM_POINTS; i++) {
				 plotPointX[i] = -(i * POINT_SIZE);
				
			}
			 SetSelectedVoice(hDlg);
			 fprintf(LogFile, " User has Restarted Wave \n");
			 pISpVoice->Speak(L"User has Restarted Wave ", SPF_ASYNC, NULL);
			 SetTimer(GetActiveWindow(), SRI_TIMER, TIME_INTERVAL_IN_MS, NULL);
			break;

		case ID_INCWAVE:
			if (noOfWaves <= 5) {
				noOfWaves = noOfWaves + 1;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Increased Wave to : %d\n", noOfWaves);
				pISpVoice->Speak(L"User Increased Wave", SPF_ASYNC, NULL);
			}
			break;

		case ID_DECWAVE:
			if (noOfWaves > 1) {
				noOfWaves = noOfWaves - 1;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Decreased Wave to : %d\n", noOfWaves);
				pISpVoice->Speak(L"User Decreased Wave", SPF_ASYNC, NULL);
			}
			break;

		case ID_INCAMP:
			if (Amplitude < 300) {
				Amplitude = Amplitude + 10;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Increased Amplitude to : %.1lf\n", Amplitude);
				pISpVoice->Speak(L"User Increased Amplitude", SPF_ASYNC, NULL);
				
			}
			break;

		case ID_DECAMP:
			if (Amplitude > 50) {
				Amplitude = Amplitude - 10;
				
			}
			 SetSelectedVoice(hDlg);
			 fprintf(LogFile, " User Decreased Amplitude to : %.1lf\n", Amplitude);
			 pISpVoice->Speak(L"User Decreased Amplitude", SPF_ASYNC, NULL);

			break;
		case ID_INCPOINTS:
			if (PointCount < 3000) {
				PointCount = PointCount + 100;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Increased Points to: %d\n ", PointCount);
				pISpVoice->Speak(L"User Increased Points", SPF_ASYNC, NULL);
			}
		
			break;
		case ID_DECPOINTS:
			if (PointCount > 100) {
				PointCount = PointCount - 100;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Decreased Points to: %d\n ", PointCount);
				pISpVoice->Speak(L"User Decreased Points", SPF_ASYNC, NULL);

			}
			break;
		case ID_INCFREQ:
			if (noOfFreq < 12) {
				noOfFreq= noOfFreq - 1;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Decreased Frequency to: %d\n ", noOfFreq);
				pISpVoice->Speak(L"User Increased Frequency", SPF_ASYNC, NULL);

			}
			break;
		case ID_DECFREQ:
			if (noOfFreq > 2) {
				noOfFreq = noOfFreq + 1;
				SetSelectedVoice(hDlg);
				fprintf(LogFile, " User Increased Frequency to: %d\n ", noOfFreq);
				pISpVoice->Speak(L"User Decreased Frequency", SPF_ASYNC, NULL);

			}

			break;
		case IDC_LISTBOX:

				  if (HIWORD(wParam) == LBN_SELCHANGE) {
					 SetSelectedVoice(hDlg);

					}
			break;
		case IDC_LISTBOX_AUDIO:
			TCHAR str[512];
			 if (HIWORD(wParam) == LBN_SELCHANGE) {
				  int selectedIndex = SendDlgItemMessage(hDlg, IDC_LISTBOX_AUDIO, LB_GETCURSEL, 0, 0);

				  if (!myFlag) {
					  selectedIndex = 1;
					  hr = pISpVoice->SetVoice(voices[selectedIndex]);

				 }
				 else {
					  selectedIndex = 0;
					   hr = pISpVoice->SetVoice(voices[selectedIndex]);
				 }

				 // if (selectedIndex >= 0 && selectedIndex < 2) {
					//  hr = pISpVoice->SetVoice(voices[selectedIndex]);
				 //}
				 // else {
					// // Handle an invalid index
					//  MessageBox(hwnd, TEXT("Invalid voice index"), TEXT("Error"), MB_OK);
				 // }
				
		 	}
			break;
		 case  WM_ERASEBKGND:
			 return 1; // Indicates that the background has been erased (prevents flickering)
			break;

		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}


INT_PTR CALLBACK DialogProcRegister(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	static BOOL isValidFirstName = TRUE;
	// Validate surname
	static BOOL isValidSurname = TRUE;
	
	COLORREF g_TextColor = RGB(255, 0, 0); 
	

	switch (message)
	{

	case  WM_INITDIALOG:

		EnableWindow(GetDlgItem(hDlg,  ID_REGISTER), g_dataRegister);
		return (INT_PTR)TRUE;

	case  WM_CTLCOLORDLG:
	{
		 HDC hdcDlg = (HDC)wParam;
		 SetBkColor(hdcDlg, RGB(255, 250, 205)); // Light Blue color
		 return (INT_PTR)CreateSolidBrush(RGB(255, 250, 205));
	}
	
	case  WM_CTLCOLORSTATIC: {
		
			 HDC hdcStatic = (HDC)wParam;


			 if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_FIRST_NAME))
			 {
				if (!isValidFirstName) 
				{
				
					SetTextColor(hdcStatic, RGB(255, 0, 0)); 
					SetBkColor(hdcStatic, RGB(255, 250, 205));
					g_dataRegister = FALSE;
					
				}
				else 
					SetTextColor(hdcStatic, RGB( 0, 0, 0)); 
					SetBkColor(hdcStatic, RGB(255, 250, 205));
					g_dataRegister = TRUE;
			 }

			 if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_SURNAME))
			 {
				 if (!isValidSurname)
				 {

					 SetTextColor(hdcStatic, RGB(255, 0, 0));
					 SetBkColor(hdcStatic, RGB(255, 250, 205));
					 g_dataRegister =  FALSE;
				 }
				 else
					 SetTextColor(hdcStatic, RGB(0, 0, 0));
					 SetBkColor(hdcStatic, RGB(255, 250, 205));
					 g_dataRegister = TRUE;
			 }
			 SetBkColor(hdcStatic, RGB(255, 250, 205));
			 return (INT_PTR)CreateSolidBrush(RGB(255, 250, 205));
			 break;
			
	}
	case  WM_COMMAND:
		if(HIWORD(wParam) == EN_KILLFOCUS){
			GetDlgItemText(hDlg, IDC_EDIT_FIRST_NAME, szFirstName, sizeof(szFirstName) / sizeof(szFirstName[0]));

			GetDlgItemText(hDlg, IDC_EDIT_SURNAME, szSurname, sizeof(szSurname) / sizeof(szSurname[0]));
			// Validate first name
			isValidFirstName = !ContainsSpecialCharactersOrNumbers(szFirstName);

			// Validate surname
			isValidSurname = !ContainsSpecialCharactersOrNumbers(szSurname);

			SendDlgItemMessage(hDlg, IDC_STATIC_FIRST_NAME, WM_ENABLE, TRUE, 0L);
			SendDlgItemMessage(hDlg, IDC_STATIC_SURNAME, WM_ENABLE, TRUE, 0L);

			EnableWindow(GetDlgItem(hDlg, ID_REGISTER), isValidFirstName && isValidSurname);
		}

		switch (LOWORD(wParam))
		{
		case ID_REGISTER:

			
			 GetDlgItemText(hDlg, IDC_EDIT_FIRST_NAME, szFirstName, sizeof(szFirstName) / sizeof(szFirstName[0]));
			 GetDlgItemText(hDlg, IDC_EDIT_SURNAME, szSurname, sizeof(szSurname) / sizeof(szSurname[0]));
			 GetLocalTime(&lt);  
			
			  wsprintf(szFileName, TEXT("%s_%s_%02d-%02d-%02d_%02d_hr_%02d_min_%02d_sec.txt"),
			 	szFirstName, szSurname,
				lt.wDay, lt.wMonth, lt.wYear % 100, 
				lt.wHour, lt.wMinute, lt.wSecond); 

			  LogFile = fopen(szFileName, "w");
	
        	fprintf(LogFile, " User Full Name is : %s %s\n", szFirstName, szSurname);
			fprintf(LogFile, " User Registration Date : %.02d - %.02d - %.02d\n", lt.wDay, lt.wMonth, lt.wYear);
			fprintf(LogFile, " User Registration Time : %.02d Hours %.02d Minutes %.02d seconds\n", lt.wHour, lt.wMinute, lt.wSecond);





			g_UserRegistered = TRUE;
			
			
			EndDialog(hDlg, TRUE);
			break;
		case IDS_CLOSE:
		
			EndDialog(hDlg, IDS_CLOSE);
			break;

		case  IDCANCEL:
			 EndDialog(hDlg, IDCANCEL);
			break;

		default:
			return FALSE;
		}

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

 INT_PTR CALLBACK DialogProcAbout(HWND hwndhDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	 HBRUSH hbrBkgnd = NULL;
	 HICON hIcon;
	switch (message)
	{
	case WM_INITDIALOG:
		// Initialization code for the "About" dialog


		 g_hFont = CreateFontW(
			20,                   // Font height
			2,                    // Font width (auto)
			0,                    // Rotation angle
			0,                    // Orientation angle
			900,                  // Font weight (700 is a higher value than FW_BOLD)
			FALSE,                // Italic
			FALSE,                // Underline
			0,                    // Strikeout
			ANSI_CHARSET,         // Character set
			OUT_DEFAULT_PRECIS,   // Output precision
			CLIP_DEFAULT_PRECIS,  // Clipping precision
			DEFAULT_QUALITY,      // Quality
			DEFAULT_PITCH,        // Pitch and family
			L"Arial"              // Font face name
		);
		 // Set the font for specific controls (change IDC_EDIT1 to the ID of your control)
		 SendDlgItemMessage(hwndhDlg, IDD_DIALOG_ABOUT, WM_SETFONT, (WPARAM)g_hFont, TRUE);
		 SetDlgItemText(hwndhDlg, IDC_STATIC_TITLE, "SINE WAVE SIMULATOR");
		 SetDlgItemText(hwndhDlg, IDC_STATIC_PROJECT, " A PROJECT BY : ASTROMEDICOMP");
		 SetDlgItemText(hwndhDlg, IDC_STATIC_GUIDED, " GUIDED BY : PRADNYA VIJAY GOKHALE");
		 SetDlgItemText(hwndhDlg, IDC_STATIC_DEVELOP, " DEVELOPED BY : Akash Avinash Kakade");
		
		 // Load and set the icon
		 hIcon =  LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(SRI_ICON)); 
		 if (hIcon != NULL);
		 {
			 // Set the icon for the IDC_STATIC_ICON control
			  SendDlgItemMessage(hwndhDlg, IDC_STATIC_ICON, STM_SETICON,  (WPARAM)hIcon, 0);
		 }
		return (INT_PTR)TRUE;

	case  WM_CTLCOLORSTATIC:
	{
		 HDC hdcStatic = (HDC)wParam;
		 SetTextColor(hdcStatic, RGB(0, 255, 0));
		 SetBkColor(hdcStatic, RGB(0, 0, 0));

		 if (hbrBkgnd == NULL)
		{
			 hbrBkgnd = CreateSolidBrush(RGB(0, 0, 0));
		}
		 return (INT_PTR)hbrBkgnd;
	}

	case WM_CTLCOLORDLG:
	{
		HDC hdcDlg = (HDC)wParam;
		SetBkColor(hdcDlg, RGB(0, 0, 0)); // Black 
		return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			 EndDialog(hwndhDlg, LOWORD(wParam));
			 return (INT_PTR)TRUE;
			break;

		case IDCANCEL:
			// Close the "About" dialog
			EndDialog(hwndhDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

			break;

		}
		break;


	}

	return FALSE;
}

   BOOL ContainsSpecialCharactersOrNumbers(const TCHAR* str) {
	while (*str) {
		if (!isalpha(*str)) {
			return TRUE; 
			// Contains special characters or numbers
		}
		str++;
	}
	return FALSE; // Does not contain special characters or numbers
}


   void  InitializeVoices()
   {
	   HRESULT hr;

	   hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pIEnumSpObjectTokens);

	   if (FAILED(hr))
	   {
		   MessageBox(hwnd, TEXT("Error initializing voice tokens"), TEXT("Error"), MB_OK);
		   DestroyWindow(hwnd);
	   }

	   // Assuming IDC_LISTBOX has 2 items (David and Zira)
	   for (int i = 0; i < 2; ++i)
	   {
		   pIEnumSpObjectTokens->Next(1, &voices[i], NULL);
	   }
   }


   void SetSelectedVoice(HWND hDlg)
   {
	   // Get the selected index from the list box
	   int selectedIndex = SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_GETCURSEL, 0, 0);

	   // Set the voice based on the selected index
	   if (selectedIndex >= 0 && selectedIndex < 2) // Assuming 2 voices
	   {
		   hr = pISpVoice->SetVoice(voices[selectedIndex]);
		   if (FAILED(hr))
		   {
			   MessageBox(hwnd, TEXT("Failed to set the selected voice"), TEXT("Error"), MB_OK);
		   }
	   }
   }



//*******************************************************************************************************************************
void OnExportButtonClick2(HWND hDlg)

{
	TCHAR str[512];
	ULONG  numVoices = 0L;
	HWND hListBox = GetDlgItem(hDlg, IDC_LISTBOX_AUDIO);
	
	 int  itemCount = SendDlgItemMessage(hDlg, IDC_LISTBOX_AUDIO, LB_GETCOUNT, 0, 0);

	 if (itemCount < 0)
	 {
		  MessageBox(hwnd, TEXT("List box is empty"), TEXT("Error"), MB_OK);
		 return;
	 }

	 // Get the selected item index
	  int selectedIndex = SendDlgItemMessage(hDlg, IDC_LISTBOX_AUDIO, LB_GETCURSEL, 0, 0);

	if (selectedIndex != LB_ERR && selectedIndex >= 0 && selectedIndex < 2)
	{
		 selectedVoiceIndex = selectedIndex;
	     ExportAudioToWav2(voices[selectedVoiceIndex]);
		
	}
	else
	{
		MessageBox(hwnd, TEXT("No voice selected"), TEXT("Error"), MB_OK);
		return;
	}
}

void  ExportAudioToWav2(ISpObjectToken* pISpObjectToken)
{
	
	CSpStreamFormat audioFormat;
	char textBuffer[TEXT_FILE_NUM_CHAR];
	WCHAR textBufferWideChar[TEXT_FILE_NUM_CHAR];
	WCHAR* DescriptionString = NULL;
	HRESULT hr = S_OK;
	ULONG numVoices = 0L;
	
	
	if (!pISpObjectToken)
	{
		MessageBox(hwnd, TEXT("Invalid voice token"), TEXT("Error"), MB_OK);
		return;
	}

	if (selectedVoiceIndex == -1)
	{
		MessageBox(hwnd, TEXT("No voice selected1202"), TEXT("Error"), MB_OK);
		return;
	}

	     ISpObjectToken* pSelectedToken = NULL;
	  
	   SpGetDescription(pISpObjectToken, &DescriptionString);
	 

	   audioFormat.AssignFormat(SPSF_44kHz16BitStereo);
	   // Convert TCHAR strings to wide strings
	     WCHAR wszFirstName[256];
	     WCHAR wszSurname[256];
	     MultiByteToWideChar(CP_ACP, 0, szFirstName, -1, wszFirstName, sizeof(wszFirstName) / sizeof(wszFirstName[0]));
	     MultiByteToWideChar(CP_ACP, 0, szSurname, -1, wszSurname, sizeof(wszSurname) / sizeof(wszSurname[0]));

	   // Create a wide string for the .wav file name
	     WCHAR wszWavFileName[512];
	     swprintf_s(wszWavFileName, L"%s_%s_%02d-%02d-%02d_%02d_hr_%02d_min_%02d_sec.wav",
		     wszFirstName, wszSurname,
		     lt.wDay, lt.wMonth, lt.wYear % 100,
		     lt.wHour, lt.wMinute, lt.wSecond);

	   // Convert wide string to multibyte
	     char  szMultibyteWavFileName[512];
	     WideCharToMultiByte(CP_ACP, 0, wszWavFileName, -1, szMultibyteWavFileName, sizeof(szMultibyteWavFileName), NULL, NULL);

     hr = SPBindToFile( wszWavFileName, SPFM_CREATE_ALWAYS, &pISpStream, &audioFormat.FormatId(), audioFormat.WaveFormatExPtr());
     if (FAILED(hr))
	{
		MessageBox(hwnd, TEXT("SPBindToFile Can Not Be Obtained"), TEXT("Error"), MB_OK);
		DestroyWindow(hwnd);
	}

	hr = pISpVoice->SetOutput(pISpStream, FALSE);
	if (FAILED(hr))
	{
		MessageBox(hwnd, TEXT("Set output Can Not Be Obtained"), TEXT("Error"), MB_OK);
		DestroyWindow(hwnd);
	}


	if (feof(RFile))
	{
		MessageBox(hwnd, TEXT("End of log file reached"), TEXT("Info"), MB_OK);
		return;
	}

	   fread(&textBuffer, sizeof(char), TEXT_FILE_NUM_CHAR, RFile);

	   MultiByteToWideChar(CP_UTF8, 0, textBuffer, TEXT_FILE_NUM_CHAR, textBufferWideChar, TEXT_FILE_NUM_CHAR);

       pISpVoice->SetVoice(pISpObjectToken);

	  pISpVoice->Speak(textBufferWideChar, SPF_DEFAULT, NULL);
	  if (FAILED(hr))
	  {
		  MessageBox(hwnd, TEXT("Speak Can Not Be Obtained"), TEXT("Error"), MB_OK);
		  DestroyWindow(hwnd);
      }
}





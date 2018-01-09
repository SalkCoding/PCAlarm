#include<Windows.h>
#include"resource.h"
#pragma comment(lib,"winmm") 
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <SDKDDKVer.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include<commdlg.h>
#include<mmsystem.h>
#pragma warning(disable :4244)
#pragma warning(disable :4474)
#pragma warning(disable :4996)
#define WM_FINISH WM_USER+1
#define WM_SHOW WM_USER+2
typedef struct tagTime {
	DWORD Hour;
	DWORD Min;
	DWORD Second;
	UINT Present;
}Time;

INT_PTR CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SettingDlg(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	MiniAlarm(HWND, UINT, WPARAM, LPARAM);
VOID				PlaySong(LPWSTR);
VOID				StopSong();
VOID				InitTime();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), HWND_DESKTOP, MainDlgProc);
	return 0;
}

HWND hDlgMain;									// 메인 윈도우의 핸들입니다.
HWND hMiniAlarm;								// 미니 알람의 핸들입니다.
HINSTANCE hInst;                                // 현재 인스턴스입니다.
TCHAR SoundFilePath[MAX_PATH];					// 사운드 파일의 경로입니다.
TCHAR Path[MAX_PATH];							// 파일 임시경로 입니다.(마구 지워졌다가 저장됫다함)
TCHAR Extend[MAX_PATH];							// 파싱된 확장자를 가지는 변수입니다.
INT TransParent = 255;							// 설정된 투명도 값입니다.
BOOL TopMostCheck = FALSE;						// 탑 모스트에 대한 설정값입니다.
BOOL TimerCheck = FALSE;						// 타이머의 여부입니다.
BOOL MiniCheck = FALSE;							// 최소화 모드의 여부입니다.

Time Alarm;
SYSTEMTIME St;
TCHAR StrTime[30];
BOOL Present;
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		hDlgMain = hDlg;
		InitTime();
		SetTimer(hDlg, 2, 1000, NULL);
		SendMessage(hDlg, WM_TIMER, 2, NULL);
		return TRUE;
	case WM_TIMER: {
		switch (wParam) {
		case 1:
			GetLocalTime(&St);
			if (Alarm.Present == FALSE) {//아침일경우
				if (St.wHour == Alarm.Hour && St.wMinute == Alarm.Min && St.wSecond == Alarm.Second) {
					if (MiniCheck == FALSE) {
						TimerCheck = FALSE;
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						PlaySong(SoundFilePath);
						KillTimer(hDlg, 1);
						MessageBox(hDlg, L"시간이 되었습니다.", L"알람", MB_OK | MB_ICONINFORMATION);
						StopSong();
					}
					else {//어짜피 true면 존재 한다는거니까 핸들 사용가능
						TimerCheck = FALSE;
						KillTimer(hDlg, 1);
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						SendMessage(hMiniAlarm, WM_FINISH, NULL, NULL);// 다이얼로그 프로시저에서 알아서 처리
					}
				}
			}
			else {
				if (St.wHour == (Alarm.Hour + 12) && St.wMinute == Alarm.Min && St.wSecond == Alarm.Second) {
					if (MiniCheck == FALSE) {
						TimerCheck = FALSE;
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						PlaySong(SoundFilePath);
						KillTimer(hDlg, 1);
						MessageBox(hDlg, L"시간이 되었습니다.", L"알람", MB_OK | MB_ICONINFORMATION);
						StopSong();
					}
					else {//어짜피 true면 존재 한다는거니까 핸들 사용가능
						TimerCheck = FALSE;
						KillTimer(hDlg, 1);
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						SendMessage(hMiniAlarm, WM_FINISH, NULL, NULL);// 다이얼로그 프로시저에서 알아서 처리
					}
				}
			}
			if (MiniCheck == TRUE) {
				SendMessage(hMiniAlarm, WM_SHOW, NULL, NULL);
			}
			return TRUE;
		case 2:
			GetLocalTime(&St);
			if (St.wHour - 12 > 0) {
				swprintf_s(StrTime, L"오후 %d 시 %d 분 %d 초", St.wHour - 12, St.wMinute, St.wSecond, sizeof(StrTime));
			}
			else {
				swprintf_s(StrTime, L"오전 %d 시 %d 분 %d 초", St.wHour, St.wMinute, St.wSecond, sizeof(StrTime));
			}
			SetDlgItemText(hDlg, IDC_PRESENTTIME, StrTime);
			return TRUE;
		}
		return TRUE;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDC_MORNING:
			Present = FALSE;
			return TRUE;
		case IDC_AFTERNOON:
			Present = TRUE;
			return TRUE;
		case IDC_STOP:
			if (TimerCheck == TRUE) {
				TimerCheck = FALSE;
				KillTimer(hDlg, 1);
				SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
				SetWindowText(hDlg, L"PC Alarm");
				MessageBox(hDlg, L"알람 작동을 중지했습니다.", L"취소", MB_OK | MB_ICONWARNING);
				return TRUE;
			}
			else {
				MessageBox(hDlg, L"설정된 알람이 없습니다.", NULL, MB_OK | MB_ICONINFORMATION);
				return TRUE;
			}
		case IDC_START: {
			int h, m, s;
			h = GetDlgItemInt(hDlg, IDC_HOUR, FALSE, FALSE);
			m = GetDlgItemInt(hDlg, IDC_MIN, FALSE, FALSE);
			s = GetDlgItemInt(hDlg, IDC_SECOND, FALSE, FALSE);
			if (h < 13 && m < 60 && s < 60) {
				Alarm.Hour = h;
				Alarm.Min = m;
				Alarm.Second = s;
				Alarm.Present = Present;
			}
			else {
				MessageBox(hDlg, L"시간 입력이 잘못되었습니다.", NULL, MB_OK | MB_ICONERROR);
				SetDlgItemInt(hDlg, IDC_HOUR, 0, FALSE);
				SetDlgItemInt(hDlg, IDC_MIN, 0, FALSE);
				SetDlgItemInt(hDlg, IDC_SECOND, 0, FALSE);
				return TRUE;
			}
			TimerCheck = TRUE;
			KillTimer(hDlg, 1);//다시 누를수 있게해서 메모리 누수 방지
			SetTimer(hDlg, 1, 1000, NULL);
			TCHAR Str[30];
			if (Present) {
				swprintf_s(Str, L"오후 %d 시 %d 분 %d 초", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
			}
			else {
				swprintf_s(Str, L"오전 %d 시 %d 분 %d 초", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
			}
			SetWindowText(hDlg, Str);
			SetDlgItemText(hDlg,IDC_SETTIME,Str);
			MessageBox(hDlg, L"알람 작동을 시작합니다.", L"시작", MB_OK | MB_ICONINFORMATION);
			return TRUE;
		}
		case IDM_SETTING:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGDLG), hDlg, SettingDlg);
			return TRUE;
		case ID_MINIMODE:
			MiniCheck = TRUE;
			ShowWindow(hDlg, SW_HIDE);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MINIALARM), hDlg, MiniAlarm);
			return TRUE;
		case IDOK:
		case IDM_EXIT:
			SendMessage(hDlg, WM_CLOSE, NULL, NULL);
		}
		return FALSE;
	}
	return TRUE;
	case WM_CLOSE:
		if (TimerCheck == TRUE) {
			if (MessageBox(hDlg, L"현재 알람이 작동중입니다.\r\n종료 하시겠습니까?.", NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
				EndDialog(hDlg, TRUE);
				KillTimer(hDlg, 1);
				KillTimer(hDlg, 2);
			}
			else
				return TRUE;
		}
		else {
			EndDialog(hDlg, TRUE);
			KillTimer(hDlg, 1);
			KillTimer(hDlg, 2);
		}
		return TRUE;
	}
	return FALSE;
}

INT TempPos;
BOOL Check = FALSE;
INT_PTR CALLBACK SettingDlg(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	switch (iMessage) {
	case WM_INITDIALOG:
		if (Check)
			CheckDlgButton(hDlg, IDC_TOPMOST, TRUE);
		else
			CheckDlgButton(hDlg, IDC_TOPMOST, FALSE);
		SetDlgItemText(hDlg, IDC_SOUNDFILE, Path);
		SetScrollRange(GetDlgItem(hDlg, IDC_TRANSPARENT), SB_CTL, 0, 255, TRUE);
		SetScrollPos(GetDlgItem(hDlg, IDC_TRANSPARENT), SB_CTL, TransParent, TRUE);
		TempPos = TransParent;
		SetDlgItemInt(hDlg, IDC_STATUS, TempPos, FALSE);
		return TRUE;
	case WM_HSCROLL:
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			TempPos = max(0, TempPos - 1);
			return TRUE;
		case SB_LINERIGHT:
			TempPos = min(255, TempPos + 1);
			return TRUE;
		case SB_PAGELEFT:
			TempPos = max(0, TempPos - 10);
			return TRUE;
		case SB_PAGERIGHT:
			TempPos = min(255, TempPos + 10);
			return TRUE;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			return TRUE;
		}
		SetScrollPos(GetDlgItem(hDlg, IDC_TRANSPARENT), SB_CTL, TempPos, TRUE);
		SetDlgItemInt(hDlg, IDC_STATUS, TempPos, FALSE);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_STATUS:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				TempPos = GetDlgItemInt(hDlg, IDC_STATUS, NULL, FALSE);
				SetScrollPos(GetDlgItem(hDlg, IDC_TRANSPARENT), SB_CTL, TempPos, TRUE);
				return TRUE;
			}
			return TRUE;
		case IDC_TOPMOST:
			Check = !Check;
			return TRUE;
		case IDC_OPENSOUND: {
			OPENFILENAME OFN;
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hDlg;
			OFN.lpstrFilter = TEXT("Wav 파일\0*.wav\0")TEXT("MP3 파일\0*.mp3\0");
			OFN.lpstrFileTitle = Extend;
			OFN.nMaxFileTitle = MAX_PATH;
			OFN.nFilterIndex = 2;
			OFN.lpstrFile = Path;
			OFN.nMaxFile = MAX_PATH;
			if (GetOpenFileName(&OFN) != 0) {
				LPWSTR Title;
				Title = _wcstok(Extend, L".");
				while (Title != NULL)
				{
					lstrcpy(Extend, Title);//뒤의 확장자 분리
					Title = _wcstok(NULL, L".");
				}
				SetDlgItemText(hDlg, IDC_SOUNDFILE, Path);
			}
			return TRUE;
		}
		case IDOK:
			if (MessageBox(hDlg, L"설정을 변경하시겠습니까?", L"알림", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
				if (TempPos < 15) {
					if (MessageBox(hDlg, L"투명도가 15이하 입니다.\r\n투명도를 너무 낮출경우 창을 찾거나 보기가 어려울수 있습니다.\r\n투명도를 30으로 맞추시겠습니까?", L"주의!", MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
						TempPos = 30;
					}
				}
				TransParent = TempPos;
				lstrcpy(SoundFilePath, Path);
				TopMostCheck = Check;
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

TCHAR Str[30];
INT_PTR CALLBACK MiniAlarm(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		hMiniAlarm = hDlg;
		if (TimerCheck == FALSE) {
			MessageBox(hDlg, L"알람이 설정되있지 않습니다.\r\n처음 창에서 설정해주세요.", NULL, MB_OK);
			ShowWindow(hDlgMain, SW_RESTORE);
			EndDialog(hDlg, FALSE);
			return FALSE;
		}
		if (TopMostCheck == TRUE) {
			SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hDlg, RGB(0, 0, 255), TransParent, LWA_ALPHA | LWA_COLORKEY);
		if (Alarm.Present == 0) {
			swprintf_s(Str, L"오전 %d 시 %d 분 %d 초", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
		}
		else {
			swprintf_s(Str, L"오후 %d 시 %d 분 %d 초", Alarm.Hour - 12, Alarm.Min, Alarm.Second, sizeof(Str));
		}
		SetDlgItemText(hDlg, IDC_SETALRAM, Str);
		return TRUE;
	case WM_NCHITTEST: {
		int nHit = DefWindowProc(hDlg, iMessage, wParam, lParam);
		if (nHit == HTCLIENT) {
			nHit = HTCAPTION;
		}
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, nHit);
		return TRUE;
	}
	case WM_WINDOWPOSCHANGING: {
		LPWINDOWPOS wp;
		RECT rt;
		POINT Mainrt;
		wp = (LPWINDOWPOS)lParam;
		Mainrt.x = GetSystemMetrics(SM_CXSCREEN);
		Mainrt.y = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(hDlg, &rt);
		int height = rt.bottom - rt.top;
		int weight = rt.right - rt.left;
		if (abs(wp->x) < 30)wp->x = 0;
		if (abs(wp->y) < 30)wp->y = 0;
		if (abs(wp->x + weight) > Mainrt.x - 30)wp->x = Mainrt.x - weight;
		if (abs(wp->y + height) > Mainrt.y - 70)wp->y = (Mainrt.y - height) - 40;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_MAXMODE:
			MiniCheck = FALSE;
			ShowWindow(hDlgMain, SW_RESTORE);
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		return TRUE;
	case WM_SHOW:
		GetLocalTime(&St);
		if (St.wHour - 12 > 0) {
			swprintf_s(Str, L"오후 %d 시 %d 분 %d 초", St.wHour - 12, St.wMinute, St.wSecond, sizeof(Str));
		}
		else {
			swprintf_s(Str, L"오전 %d 시 %d 분 %d 초", St.wHour, St.wMinute, St.wSecond, sizeof(Str));
		}
		SetDlgItemText(hDlg, IDC_PRESENT, Str);
		return TRUE;
	case WM_FINISH:
		ShowWindow(hDlgMain, SW_RESTORE);
		SetDlgItemText(hDlgMain, IDC_SETTIME, L"0 : 0 : 0");
		EndDialog(hDlg, TRUE);
		PlaySong(SoundFilePath);//노래 시작
		MessageBox(hDlgMain, L"시간이 되었습니다.", L"알람", MB_OK | MB_ICONINFORMATION);// 끄기 전까지 리턴을 안함
		StopSong();// 눌러지면 확인 누른거니 노래끔
		return TRUE;
	}
	return FALSE;
}

VOID PlaySong(LPWSTR File) {
	if (!lstrcmp(Extend, L"mp3")) {
		TCHAR Path[MAX_PATH];
		wsprintf(Path, L"open %s type mpegvideo alias MediaFile", File);
		mciSendString(Path, NULL, 0, NULL);//love.mp3는 플레이할 음악 파일
		mciSendString(L"play MediaFile repeat", NULL, 0, NULL);//배경음 재생
	}
	else {
		PlaySound(File, NULL, SND_SYNC | SND_FILENAME | SND_LOOP);//wav파일은 반복
	}
}

VOID StopSong() {
	if (!lstrcmp(Extend, L"mp3")) {
		mciSendString(L"stop MediaFile", NULL, 0, NULL);
	}
	else {
		PlaySound(NULL, NULL, NULL);
	}
}

VOID InitTime() {
	GetLocalTime(&St);
	if (St.wHour - 12 > 0) {
		CheckDlgButton(hDlgMain, IDC_AFTERNOON, TRUE);
		SetDlgItemInt(hDlgMain, IDC_HOUR, St.wHour - 12, FALSE);
		Present = 1;
	}
	else {
		CheckDlgButton(hDlgMain, IDC_MORNING, TRUE);
		SetDlgItemInt(hDlgMain, IDC_HOUR, St.wHour, FALSE);
		Present = 0;
	}
	SetDlgItemInt(hDlgMain, IDC_MIN, St.wMinute, FALSE);
	SetDlgItemInt(hDlgMain, IDC_SECOND, St.wSecond, FALSE);
}
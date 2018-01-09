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

HWND hDlgMain;									// ���� �������� �ڵ��Դϴ�.
HWND hMiniAlarm;								// �̴� �˶��� �ڵ��Դϴ�.
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
TCHAR SoundFilePath[MAX_PATH];					// ���� ������ ����Դϴ�.
TCHAR Path[MAX_PATH];							// ���� �ӽð�� �Դϴ�.(���� �������ٰ� ����̴���)
TCHAR Extend[MAX_PATH];							// �Ľ̵� Ȯ���ڸ� ������ �����Դϴ�.
INT TransParent = 255;							// ������ ���� ���Դϴ�.
BOOL TopMostCheck = FALSE;						// ž ��Ʈ�� ���� �������Դϴ�.
BOOL TimerCheck = FALSE;						// Ÿ�̸��� �����Դϴ�.
BOOL MiniCheck = FALSE;							// �ּ�ȭ ����� �����Դϴ�.

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
			if (Alarm.Present == FALSE) {//��ħ�ϰ��
				if (St.wHour == Alarm.Hour && St.wMinute == Alarm.Min && St.wSecond == Alarm.Second) {
					if (MiniCheck == FALSE) {
						TimerCheck = FALSE;
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						PlaySong(SoundFilePath);
						KillTimer(hDlg, 1);
						MessageBox(hDlg, L"�ð��� �Ǿ����ϴ�.", L"�˶�", MB_OK | MB_ICONINFORMATION);
						StopSong();
					}
					else {//��¥�� true�� ���� �Ѵٴ°Ŵϱ� �ڵ� ��밡��
						TimerCheck = FALSE;
						KillTimer(hDlg, 1);
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						SendMessage(hMiniAlarm, WM_FINISH, NULL, NULL);// ���̾�α� ���ν������� �˾Ƽ� ó��
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
						MessageBox(hDlg, L"�ð��� �Ǿ����ϴ�.", L"�˶�", MB_OK | MB_ICONINFORMATION);
						StopSong();
					}
					else {//��¥�� true�� ���� �Ѵٴ°Ŵϱ� �ڵ� ��밡��
						TimerCheck = FALSE;
						KillTimer(hDlg, 1);
						SetDlgItemText(hDlg, IDC_SETTIME, L"0 : 0 : 0");
						SendMessage(hMiniAlarm, WM_FINISH, NULL, NULL);// ���̾�α� ���ν������� �˾Ƽ� ó��
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
				swprintf_s(StrTime, L"���� %d �� %d �� %d ��", St.wHour - 12, St.wMinute, St.wSecond, sizeof(StrTime));
			}
			else {
				swprintf_s(StrTime, L"���� %d �� %d �� %d ��", St.wHour, St.wMinute, St.wSecond, sizeof(StrTime));
			}
			SetDlgItemText(hDlg, IDC_PRESENTTIME, StrTime);
			return TRUE;
		}
		return TRUE;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
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
				MessageBox(hDlg, L"�˶� �۵��� �����߽��ϴ�.", L"���", MB_OK | MB_ICONWARNING);
				return TRUE;
			}
			else {
				MessageBox(hDlg, L"������ �˶��� �����ϴ�.", NULL, MB_OK | MB_ICONINFORMATION);
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
				MessageBox(hDlg, L"�ð� �Է��� �߸��Ǿ����ϴ�.", NULL, MB_OK | MB_ICONERROR);
				SetDlgItemInt(hDlg, IDC_HOUR, 0, FALSE);
				SetDlgItemInt(hDlg, IDC_MIN, 0, FALSE);
				SetDlgItemInt(hDlg, IDC_SECOND, 0, FALSE);
				return TRUE;
			}
			TimerCheck = TRUE;
			KillTimer(hDlg, 1);//�ٽ� ������ �ְ��ؼ� �޸� ���� ����
			SetTimer(hDlg, 1, 1000, NULL);
			TCHAR Str[30];
			if (Present) {
				swprintf_s(Str, L"���� %d �� %d �� %d ��", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
			}
			else {
				swprintf_s(Str, L"���� %d �� %d �� %d ��", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
			}
			SetWindowText(hDlg, Str);
			SetDlgItemText(hDlg,IDC_SETTIME,Str);
			MessageBox(hDlg, L"�˶� �۵��� �����մϴ�.", L"����", MB_OK | MB_ICONINFORMATION);
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
			if (MessageBox(hDlg, L"���� �˶��� �۵����Դϴ�.\r\n���� �Ͻðڽ��ϱ�?.", NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
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
			OFN.lpstrFilter = TEXT("Wav ����\0*.wav\0")TEXT("MP3 ����\0*.mp3\0");
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
					lstrcpy(Extend, Title);//���� Ȯ���� �и�
					Title = _wcstok(NULL, L".");
				}
				SetDlgItemText(hDlg, IDC_SOUNDFILE, Path);
			}
			return TRUE;
		}
		case IDOK:
			if (MessageBox(hDlg, L"������ �����Ͻðڽ��ϱ�?", L"�˸�", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
				if (TempPos < 15) {
					if (MessageBox(hDlg, L"������ 15���� �Դϴ�.\r\n������ �ʹ� ������ â�� ã�ų� ���Ⱑ ������ �ֽ��ϴ�.\r\n������ 30���� ���߽ðڽ��ϱ�?", L"����!", MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
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
			MessageBox(hDlg, L"�˶��� ���������� �ʽ��ϴ�.\r\nó�� â���� �������ּ���.", NULL, MB_OK);
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
			swprintf_s(Str, L"���� %d �� %d �� %d ��", Alarm.Hour, Alarm.Min, Alarm.Second, sizeof(Str));
		}
		else {
			swprintf_s(Str, L"���� %d �� %d �� %d ��", Alarm.Hour - 12, Alarm.Min, Alarm.Second, sizeof(Str));
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
			swprintf_s(Str, L"���� %d �� %d �� %d ��", St.wHour - 12, St.wMinute, St.wSecond, sizeof(Str));
		}
		else {
			swprintf_s(Str, L"���� %d �� %d �� %d ��", St.wHour, St.wMinute, St.wSecond, sizeof(Str));
		}
		SetDlgItemText(hDlg, IDC_PRESENT, Str);
		return TRUE;
	case WM_FINISH:
		ShowWindow(hDlgMain, SW_RESTORE);
		SetDlgItemText(hDlgMain, IDC_SETTIME, L"0 : 0 : 0");
		EndDialog(hDlg, TRUE);
		PlaySong(SoundFilePath);//�뷡 ����
		MessageBox(hDlgMain, L"�ð��� �Ǿ����ϴ�.", L"�˶�", MB_OK | MB_ICONINFORMATION);// ���� ������ ������ ����
		StopSong();// �������� Ȯ�� �����Ŵ� �뷡��
		return TRUE;
	}
	return FALSE;
}

VOID PlaySong(LPWSTR File) {
	if (!lstrcmp(Extend, L"mp3")) {
		TCHAR Path[MAX_PATH];
		wsprintf(Path, L"open %s type mpegvideo alias MediaFile", File);
		mciSendString(Path, NULL, 0, NULL);//love.mp3�� �÷����� ���� ����
		mciSendString(L"play MediaFile repeat", NULL, 0, NULL);//����� ���
	}
	else {
		PlaySound(File, NULL, SND_SYNC | SND_FILENAME | SND_LOOP);//wav������ �ݺ�
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
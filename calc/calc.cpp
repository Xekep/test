#include "stdafx.h"
#include "calc.h"

#pragma comment(lib, "comctl32.lib")

#define ActionNull 0
#define ActionSub 1
#define ActionAdd 2
#define ActionDiv 3
#define ActionMul 4


class Calculator
{
private:
	double result;
public:
	bool Compute(UINT ActionType, double op1, double op2)
	{
		if (ActionType == ActionNull)
				return false;

		switch (ActionType)
		{
		case ActionAdd:
			result = op1 + op2;
			break;
		case ActionSub:
			result = op1 - op2;
			break;
		case ActionMul:
			result = op1 * op2;
			break;
		case ActionDiv:
			if (op2 != 0.0)
				result = op1 / op2;
			else
				return false;
			break;
		}
		return true;
	};
	double Result()
	{
		return result;
	};
};

Calculator *Calc;

INT_PTR CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK NewWndProcEdit(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
void HandlerFunction(HWND hDlg, UINT ActionType);
void ShowErrMsg(HWND hDlg, LPWSTR szText);
void Log(LPWSTR szText);
bool SaveResult(LPWSTR szText);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), HWND_DESKTOP, WndProc);  
    return 0;
}

INT_PTR CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
		Calc = new Calculator();
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), NewWndProcEdit, 0, 0);
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT2), NewWndProcEdit, 0, 0);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			delete Calc;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDDIV:
			HandlerFunction(hDlg, ActionDiv);
			return (INT_PTR)TRUE;
		case IDMUL:
			HandlerFunction(hDlg, ActionMul);
			return (INT_PTR)TRUE;
		case IDSUB:
			HandlerFunction(hDlg, ActionSub);
			return (INT_PTR)TRUE;
		case IDADD:
			HandlerFunction(hDlg, ActionAdd);
			return (INT_PTR)true;
		}
    }
    return (INT_PTR)FALSE;
}

void HandlerFunction(HWND hDlg, UINT ActionType)
{
	wchar_t szBuff1[256], szBuff2[256];

	if (GetDlgItemText(hDlg, IDC_EDIT1, szBuff1, sizeof(szBuff1)) > 0 &&
		GetDlgItemText(hDlg, IDC_EDIT2, szBuff2, sizeof(szBuff2)) > 0)
	{
		if (Calc->Compute(ActionType, _wtof(szBuff1), _wtof(szBuff2)))
		{
			swprintf_s(szBuff1, L"%.10f", Calc->Result());
			SetDlgItemText(hDlg, IDC_EDIT3, szBuff1);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK1))
			{
				if (!SaveResult(szBuff1))
				{
					CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
					ShowErrMsg(hDlg, L"Не удалось записать результат в файл.");
				}
			}
		}
		else
		{
			ShowErrMsg(hDlg, L"Деление на ноль невозможно.");
		}
	}
	else
	{
		ShowErrMsg(hDlg, L"Один из операндов не введен.");
	}

}

void ShowErrMsg(HWND hDlg,LPWSTR szText)
{
	Log(szText);
	MessageBox(hDlg, szText, nullptr, MB_ICONWARNING);
}

void Log(LPWSTR szText)
{
	FILE* pFile;
	time_t now = time(0);
	wchar_t szTime[256];

	if (_wfopen_s(&pFile, L"reports.txt", L"a+, ccs=UTF-8") != 0)
		return;

	_wctime_s(szTime, sizeof(szTime), &now);

	fwprintf(pFile, L"%s: %s\r\n", szTime, szText);
	fclose(pFile);
}

bool SaveResult(LPWSTR szText)
{
	FILE* pFile;
	
	if (_wfopen_s(&pFile, L"results.txt", L"a+") != 0)
		return false;

	//fputws(szText, pFile);
	fwprintf(pFile, L"%s\r\n", szText);
	fclose(pFile);
	return true;
}

LRESULT CALLBACK NewWndProcEdit(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CHAR:
		if ((wParam >= 0x30 && wParam <= 0x39) ||
			wParam == '.' || wParam == VK_BACK)
			break;
		else
			return TRUE;
	case WM_PASTE:
	case WM_CONTEXTMENU:
		return TRUE;

	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
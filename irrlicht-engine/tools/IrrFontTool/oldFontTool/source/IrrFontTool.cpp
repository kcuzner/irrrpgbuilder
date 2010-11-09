// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

// This is the Font tool for the Irrlicht Engine. It only runs and compiles
// with windows.
// I hacked this program within a few minutes, so the code is currently not 
// documentated and very C.

#include "stdafx.h"
#include "resource.h"
#include "stdio.h"
#include "stdlib.h"

struct SCharData
{
	wchar_t character;
	int width;
	int height;

	int posX;
	int posY;
};


int CALLBACK EnumFamCallBack(CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD FontType, LPARAM lpData) 
{ 
	SendDlgItemMessage ((HWND)lpData, IDC_LIST1, LB_ADDSTRING, 0, (LONG)lplf->lfFaceName); 
	return 1;
}

void updateFontPreview(HWND hWnd)
{
	int sizeSel = SendDlgItemMessage(hWnd, IDC_LIST2, LB_GETCURSEL, 0, 0);
	int sel = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
	int bold = SendDlgItemMessage(hWnd, IDC_CHECK1, BM_GETCHECK, 0, 0);
	int italic = SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, 0, 0);

	if (sel != LB_ERR && sizeSel != LB_ERR)
	{
		char str[1024]; 

		SendDlgItemMessage(hWnd, IDC_LIST2, LB_GETTEXT, sizeSel, (LPARAM) (LPCTSTR)str);
		int fontSize = atoi(str);

		SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, sel, (LPARAM) (LPCTSTR)str);

		HDC dc = GetDC(hWnd);

		HFONT font = CreateFont(
			-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,0,0,
			bold ? FW_BOLD : 0,
			italic,
			0,0,0,0,0,0,0,
			str);

		ReleaseDC(hWnd, dc);

		RECT r;
		HWND preview = GetDlgItem(hWnd, IDC_PREVIEW);
		GetClientRect(preview, &r);

		LOGBRUSH lbrush;
		lbrush.lbColor = GetSysColor(COLOR_3DFACE);
		lbrush.lbHatch = 0;
		lbrush.lbStyle = BS_SOLID;
		
		HBRUSH brush = CreateBrushIndirect(&lbrush);
		HPEN pen = CreatePen(PS_NULL, 0, 0);

		dc = GetDC(preview);

		HGDIOBJ oldfont = SelectObject(dc, font);
		HGDIOBJ oldpen = SelectObject(dc, pen);
		HGDIOBJ oldbrush = SelectObject(dc, brush);

		const char *text = "ABCDEF abcdef 12345";

		SetBkMode(dc, OPAQUE);
		SetBkColor(dc, GetSysColor(COLOR_3DFACE));
		Rectangle(dc, 0,0, r.right, r.bottom);

		DrawText(dc, text, strlen(text), &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

		SelectObject(dc, oldfont);
		SelectObject(dc, oldpen);
		SelectObject(dc, oldbrush);

		ReleaseDC(preview, dc);

		DeleteObject(font);
		DeleteObject(brush);
		DeleteObject(pen);
	}
}


void FillFontList(HWND hWnd)
{
	// Fill list with font names

	SendDlgItemMessage(hWnd, IDC_LIST1, LB_RESETCONTENT, 0, 0); 
	HDC dc = GetDC(hWnd);
	EnumFonts(dc, 0, &EnumFamCallBack, (LPARAM)hWnd);
	ReleaseDC(hWnd, dc);

	SendDlgItemMessage (hWnd, IDC_LIST1, LB_SETCURSEL, 0, 0);

	// Fill list with font sizes

	char buf[128];
	int sc[] = {4,6,8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,56,68,72,0};
	for (int i=0; sc[i] != 0; ++i)
		SendDlgItemMessage (hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LONG)itoa(sc[i], buf, 10)); 

	SendDlgItemMessage (hWnd, IDC_LIST2, LB_SETCURSEL, 4, 0);

	// Fill texture size list

	int tc[] = {64,128,256,512,1024,2048,0};
	for (int t=0; tc[t] != 0; ++t)
	{
		sprintf(buf, "%d pixels width", tc[t]);
		SendDlgItemMessage (hWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LONG)buf);
	}

	SendDlgItemMessage (hWnd, IDC_COMBO1, CB_SETCURSEL, 2, 0);
}



inline int getTextureSizeFromSurfaceSize(int size)
{
	int ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}



void copyFontToClipBoard(HWND hWnd, int lastChar)
{
	int surfaceSel = SendDlgItemMessage(hWnd, IDC_COMBO1, CB_GETCURSEL, 0, 0);
	int sizeSel = SendDlgItemMessage(hWnd, IDC_LIST2, LB_GETCURSEL, 0, 0);
	int sel = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);	
	int bold = SendDlgItemMessage(hWnd, IDC_CHECK1, BM_GETCHECK, 0, 0);
	int italic = SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, 0, 0);

	if (sel == LB_ERR || sizeSel == LB_ERR || surfaceSel == CB_ERR)
		return;

	int i;
	int firstChar = 32;
	int characterCount = lastChar - firstChar;
	SCharData *asciiTable = new SCharData[characterCount];
	wchar_t* testString = new wchar_t[characterCount + 1];
	for (i=0; i<characterCount; ++i)
		testString[i] = i + firstChar;

	testString[characterCount] = 0;

	// We use the font render preview area as dc

	HWND preview = GetDlgItem(hWnd, IDC_PREVIEW);
	HDC dc = GetDC(preview);

	char str[1024]; 

	SendDlgItemMessage(hWnd, IDC_LIST2, LB_GETTEXT, sizeSel, (LPARAM) (LPCTSTR)str);
	int fontSize = atoi(str);

	SendDlgItemMessage(hWnd, IDC_COMBO1, CB_GETLBTEXT, surfaceSel, (LPARAM) (LPCTSTR)str);
	int surfaceWidth = atoi(str);

	SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, sel, (LPARAM) (LPCTSTR)str);

	HFONT font = CreateFont(
			-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,0,0,
			bold ? FW_BOLD : 0,
			italic,
			0,0,0,0,0,0,0,
			str);

	HGDIOBJ oldfont = SelectObject(dc, font);

	// calculate text extents.

	SIZE size;
	size.cx = 0;
	size.cy = 0;
	int posx = 0;
	int posy = 0;

	for (i=0; i<characterCount; ++i)
	{
		GetTextExtentPoint32W(dc, &testString[i], 1, &size);
		++size.cx;

		if (posx + size.cx > surfaceWidth)
		{
			posx = 0;
			posy += size.cy+1;
		}

		asciiTable[i].posX = posx;
		asciiTable[i].posY = posy;
		asciiTable[i].character = testString[i];
		asciiTable[i].width = size.cx;
		asciiTable[i].height = size.cy;		

		posx += size.cx;
	}

	int surfaceHeight = asciiTable[characterCount-1].posY + asciiTable[characterCount-1].height + 1;

	surfaceHeight = getTextureSizeFromSurfaceSize(surfaceHeight);

	// draw characters

	HBITMAP bmp = CreateCompatibleBitmap(dc, surfaceWidth, surfaceHeight);
	HDC bmpdc = CreateCompatibleDC(dc);

	LOGBRUSH lbrush;
	lbrush.lbColor = RGB(0,0,0);
	lbrush.lbHatch = 0;
	lbrush.lbStyle = BS_SOLID;
	
	HBRUSH brush = CreateBrushIndirect(&lbrush);
	HPEN pen = CreatePen(PS_NULL, 0, 0);

	HGDIOBJ oldbmp = SelectObject(bmpdc, bmp);
	HGDIOBJ oldbmppen = SelectObject(bmpdc, pen);
	HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
	HGDIOBJ oldbmpfont = SelectObject(bmpdc, font);

	SetTextColor(bmpdc, RGB(255,255,255));

	Rectangle(bmpdc, 0,0,surfaceWidth,surfaceHeight);
	SetBkMode(bmpdc, TRANSPARENT);

	SCharData* d;

	for (i=0; i<characterCount; ++i)
	{
		d = &asciiTable[i];

		TextOutW(bmpdc, d->posX, d->posY, &d->character, 1);
		SetPixel(bmpdc, d->posX, d->posY, RGB(255,255,0));// left upper corner mark
		SetPixel(bmpdc, d->posX+ d->width -1, d->posY + d->height, RGB(255,0,0));// right lower corner mark
	}

	// draw index pixels
	SetPixel(bmpdc, 0,0, RGB(255,255,0)); // left upper corner mark
	SetPixel(bmpdc, 1,0, RGB(255,0,0)); // right lower corner mark
	SetPixel(bmpdc, 2,0, RGB(0,0,0)); // background color mark

	// copy to clipboard

	OpenClipboard(hWnd);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, bmp);
	CloseClipboard();	
	
	SelectObject(bmpdc, oldbmp);
	SelectObject(bmpdc, oldbmppen);
	SelectObject(bmpdc, oldbmpbrush);
	SelectObject(bmpdc, oldbmpfont);

	SelectObject(dc, oldfont);
	ReleaseDC(preview, dc);

	DeleteDC(bmpdc);
	DeleteObject(font);
	DeleteObject(brush);
	DeleteObject(pen);
	DeleteObject(bmp);
}


BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) 
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		{
			int wNotifyCode = HIWORD(wParam);
			int wID = LOWORD(wParam);
			HWND hwndCtl = (HWND)lParam;

			switch(wID)
			{
			case IDOK:
			case IDCANCEL:
				EndDialog(hwndDlg, wNotifyCode);
				PostQuitMessage(0);
				return TRUE;
			case IDC_CHECK1:
			case IDC_CHECK2:
			case IDC_LIST1:
			case IDC_LIST2: 
				updateFontPreview(hwndDlg);
				return TRUE;
			case IDC_BUTTON2:
				MessageBox(hwndDlg, "IrrFontTool version 1.1 (c) 2003-2005 by N.Gebhardt.\n\n"\
					"This tools creates bitmap fonts for using them with the Irrlicht Engine.\n"\
					"You can download the Engine and (newer) versions of this tool from "\
					"http://irrlicht.sourceforge.net.",
					"About IrrFontTool", MB_OK | MB_ICONINFORMATION);
				return TRUE;
			case IDC_BUTTON1:
				copyFontToClipBoard(hwndDlg, 381);
				return TRUE;
			}
		}
		break;
	}

	return FALSE;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);

	ShowWindow(hWnd , SW_SHOW);

	FillFontList(hWnd);

	MSG msg;
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (msg.message != WM_QUIT);

	return 0;
}




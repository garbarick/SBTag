//----------------------------------------------------------------------
#include <windows.h>
#include <string.h>
#include <shellapi.h>
#include <shlobj.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>

//----------------------------------------------------------------------
void TRAC(char *s, char *t=NULL){MessageBox(NULL, s, t, 0);}
void IRAC(int i, char *t=NULL){char s[20]; itoa(i,s,10); MessageBox(NULL, s, t, 0);}

//----------------------------------------------------------------------

#include "ini.h"
#include "SBTag.h"
#include "IDTag.h"
#include "MP3Info.h"
#include "SandP.h"

//----------------------------------------------------------------------
HWND hMain, hFile, hExec, hTagInfo, hTip, hGrRen, hGrTag, hNameTxt, hIntegr, hAbout;
HINSTANCE hInS;

COLORREF ColorParam,ColorFolder=0;
HBRUSH hBrushParam;

HBITMAP hBitmap=NULL;
char *LastDir=NULL;
char *StartCmd=NULL;

HICON hSelAllIcon, hDesAllIcon, hAllIcon, hExecIcon, hAboutIcon, hUpDateIcon, hTrimIcon, hLeftIcon, hRightIcon, hClearIcon, hRefreshIcon;

BYTE LastTab = 0;
int Width, Height;
int ColSel=0;
BOOL ReadTrue=TRUE;

struct PData{BYTE type; BOOL pb;};

int *rndI=NULL;
HINSTANCE hToolH32=NULL;

//----------------------------------------------------------------------
static BOOL CALLBACK MainFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK FileFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK ExecFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK TagInfoFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK GrRenFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK GrTagFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK NameTxtFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK IntegrFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static BOOL CALLBACK AboutDialogFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK ProcOnNotify(HWND hWnd, int idFrom, NMHDR* pnmhdr);

LRESULT CALLBACK  AutoCheckBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT (CALLBACK  *AutoCheckBoxDef)(HWND, UINT, WPARAM, LPARAM)=NULL;

char* GetFolder();
int CALLBACK  BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);

void InitializeApp();
void InsItem(char *name,int indx);

void SetItemSel(char *name=NULL);
void SelectItemPos(int i);
int GetItemSel();
void DeSelectItemPos(int i);

void ReadAndDrowFolder(BOOL H,BOOL B);
void FindFileExt(char *curdir, char *ext, BOOL *B);

void SelectedLast();
void ListCreate(BOOL F=TRUE);

void DrawButtonWithIcon(DRAWITEMSTRUCT *di, HICON hi, BOOL bUsFont=FALSE);

void DragFunc(WPARAM wParam);
void ParsCmd(char *Cmd);

void SearchF();

void ReadMediaFile(char *name);
void ClearInfo(BYTE n);
void Enable();
void Check(HWND hM, int IDCH, int IDEN);

void AddToolTip(HWND hWnd, UINT nIDTool, LPCTSTR szText);
char* TextTransForm(char *T,BYTE *c,BYTE *r);
void TextTransToEdit(HWND H,int ID,int IDCH,char *T,BYTE *c,BYTE *r);
void strRUS(char *n,BOOL UP);

void Execute(struct PData *PD);
void CopyTag12(char *path);
void CopyTag21(char *path);
void ExecuteTagInfo(char *path);
BOOL IfCheckChar(HWND hM, int IDCH, int IDED,char **EdText,char *Default,BYTE *c,BYTE *r);

void UpdateNameList();
void CreatNameListItem(char *path,int pos);
char *ConvertName(char *path,int pos);

void UpdateTagsList(char *name,BOOL save);
void ConvertNameToTag(char *Name, char *Base[6]);

void TrimStart();
void TrimEnd();
void CountExec();

int GetCountMemo(char *n);
char* GetStrMemo(char *n,int I);

void CreateLabel();
char *GetLabelPath();
void CreateLink(LPCSTR lpszPathObj,LPCSTR lpszPathLink, LPCSTR lpszDesc);

void EnterFolder();

void CreateLanguageMenu();
void Language();

char *GetTextSubItem(HWND hwnd, int ID, int ipos, int i, int imax);

//----------------------------------------------------------------------
void DrawButtonWithIcon(DRAWITEMSTRUCT *di, HICON hi, BOOL bUsFont=FALSE)
{
if (hi==NULL) return;

HBRUSH hBr=GetSysColorBrush(0);
FillRect(di->hDC, &di->rcItem, hBr);

DWORD dwstate=SendMessage(di->hwndItem, BM_GETSTATE, 0, 0);
int iedge=EDGE_RAISED; if (dwstate & BST_PUSHED || dwstate & BST_CHECKED) iedge=EDGE_SUNKEN;

SetBkMode(di->hDC, TRANSPARENT);
char s[100]; GetWindowText(di->hwndItem, s, 100);

RECT r=di->rcItem;
int h=r.bottom-4;
if (iedge == EDGE_SUNKEN) {InflateRect(&r, -1, -1); OffsetRect(&r, 1, 1);}

DrawIconEx(di->hDC, r.left+2, r.top+2, hi, h, h, 0, hBr, DI_NORMAL);
InflateRect(&r, -16, 0); OffsetRect(&r, 8, 0);

HFONT hFontOld=NULL;
if (bUsFont)
	{
	HFONT hFont = (HFONT)SendMessage(hExec, WM_GETFONT, 0, 0);
	hFontOld=SelectObject(di->hDC, hFont);
	}
DrawText(di->hDC, s, strlen(s), &r, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
if (hFontOld) SelectObject(di->hDC, hFontOld);

DrawEdge(di->hDC, &di->rcItem, iedge, BF_RECT);
DeleteObject(hBr);
}
//----------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
fileini=CreatePathFile(NULL, "SBTag.ini");

Width = GetKeyInt("Main", "Width", 580);
Height = GetKeyInt("Main", "Height", 560);

hInS=hinst;
hMain	=	CreateDialog(hInS,MAKEINTRESOURCE(IDMAINDIALOG), NULL, (DLGPROC) MainFunc);
hFile	=	CreateDialog(hInS,MAKEINTRESOURCE(IDFILEDIALOG), hMain, (DLGPROC) FileFunc);
hExec	=	CreateDialog(hInS,MAKEINTRESOURCE(IDEXECDIALOG), hMain, (DLGPROC) ExecFunc);
hTagInfo=	CreateDialog(hInS,MAKEINTRESOURCE(IDTAGINFODIALOG), hExec, (DLGPROC) TagInfoFunc);
hGrRen	=	CreateDialog(hInS,MAKEINTRESOURCE(IDGRRENDIALOG), hExec, (DLGPROC) GrRenFunc);
hGrTag	=	CreateDialog(hInS,MAKEINTRESOURCE(IDGRTAGDIALOG), hExec, (DLGPROC) GrTagFunc);
hNameTxt=	CreateDialog(hInS,MAKEINTRESOURCE(IDNAMETXTDIALOG), hExec, (DLGPROC) NameTxtFunc);
hIntegr	=	CreateDialog(hInS,MAKEINTRESOURCE(IDINTEGRDIALOG), hExec, (DLGPROC) IntegrFunc);

if (lpCmdLine && strlen(lpCmdLine)) StartCmd=strdup(lpCmdLine);

InitializeApp();
SetWindowPos(hMain,NULL,((GetSystemMetrics(SM_CXSCREEN)-Width)/2),((GetSystemMetrics(SM_CYSCREEN)-Height)/2),Width,Height,SWP_SHOWWINDOW);

HACCEL hFileAcc=LoadAccelerators(hInS, MAKEINTRESOURCE(IDFILEACC));
HACCEL hExecAcc=LoadAccelerators(hInS, MAKEINTRESOURCE(IDEXECACC));

MSG msg;
while (GetMessage(&msg,NULL,0,0))
	{
	if (IsDialogMessage(hTagInfo, &msg)) continue;
	if (IsDialogMessage(hGrRen, &msg)) continue;
	if (IsDialogMessage(hGrTag, &msg)) continue;
	if (IsDialogMessage(hNameTxt, &msg)) continue;
	if (IsDialogMessage(hIntegr, &msg)) continue;
	if (TranslateAccelerator(hFile, hFileAcc, &msg)) continue;
	if (TranslateAccelerator(hExec, hExecAcc, &msg)) continue;

	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
return msg.wParam;
}
//----------------------------------------------------------------------
void InitializeApp()
{
HINSTANCE hToolH32=LoadLibrary("KERNEL32.DLL");
if (hToolH32) pGetLongPathName = GetProcAddress(hToolH32, "GetLongPathNameA");

CreateLanguageMenu();
Language();

//-----hMain--------
ColorParam = 14286847;
hBrushParam = CreateSolidBrush(ColorParam);

HICON hMainIcon = LoadImage(hInS,MAKEINTRESOURCE(IDAPPLICON),IMAGE_ICON,16,16,0);
SendMessage(hMain,WM_SETICON,0,(LPARAM)hMainIcon);
//-----hMain--------

//-----hExec--------
hSelAllIcon =		LoadImage(hInS,MAKEINTRESOURCE(IDSELALLICON),IMAGE_ICON,16,16,0);
hDesAllIcon =		LoadImage(hInS,MAKEINTRESOURCE(IDDESALLICON),IMAGE_ICON,16,16,0);
hAllIcon =			LoadImage(hInS,MAKEINTRESOURCE(IDALLICON),IMAGE_ICON,16,16,0);
hExecIcon =			LoadImage(hInS,MAKEINTRESOURCE(IDEXECICON),IMAGE_ICON,16,16,0);
hRefreshIcon =		LoadImage(hInS,MAKEINTRESOURCE(IDREFRESHICON),IMAGE_ICON,16,16,0);

CheckDlgButton(hExec,IDALL,GetKeyInt("Exec","All",0));
SendDlgItemMessage(hExec, IDALL, WM_SETFONT, SendMessage(hExec, WM_GETFONT, 0, 0), MAKELPARAM(0, 0));
HWND hidall=GetDlgItem(hExec,IDALL); (LONG)AutoCheckBoxDef = GetWindowLong(hidall, GWL_WNDPROC); SetWindowLong(hidall, GWL_WNDPROC, (LONG)AutoCheckBoxProc);
//-----hExec--------

//-----hFile--------
SendDlgItemMessage(hFile,IDSFOLDER,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadImage(hInS,MAKEINTRESOURCE(IDSFOLDERICON),IMAGE_ICON,16,16,0));
hAboutIcon =		LoadImage(hInS,MAKEINTRESOURCE(IDABOUTICON),IMAGE_ICON,16,16,0);

SendDlgItemMessage(hFile,IDFILELIST,LVM_SETBKCOLOR,0,(LPARAM)ColorParam);
SendDlgItemMessage(hFile,IDFILELIST,LVM_SETTEXTBKCOLOR,0,(LPARAM)ColorParam);
SendDlgItemMessage(hFile,IDFILELIST,LVM_SETTEXTCOLOR,0,(LPARAM)0);

SendDlgItemMessage(hFile,IDFILELIST,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_UNDERLINEHOT|LVS_EX_ONECLICKACTIVATE);

HIMAGELIST himlSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),ILC_COLOR8,0,1);
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL1ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL2ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL3ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL4ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL5ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL6ICON)));
ImageList_AddIcon(himlSmall, LoadIcon(hInS,MAKEINTRESOURCE(IDL7ICON)));

SendDlgItemMessage(hFile,IDFILELIST,LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL,(LPARAM)himlSmall);
//-----hFile--------

//-----hTagInfo-----
SendDlgItemMessage(hTagInfo,IDT12,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadImage(hInS,MAKEINTRESOURCE(IDT12ICON),IMAGE_ICON,36,11,0));
SendDlgItemMessage(hTagInfo,IDT21,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadImage(hInS,MAKEINTRESOURCE(IDT21ICON),IMAGE_ICON,36,11,0));
//-----hTagInfo-----

//-----hGrRen-------
CheckDlgButton(hGrRen,IDTRIM,GetKeyInt("GrRen","Trim",1));
CheckDlgButton(hGrRen,IDRANDOM,GetKeyInt("GrRen","Random",0));

SendDlgItemMessage(hGrRen,IDNAMELIST,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
SendDlgItemMessage(hGrRen,IDNAMELIST,LVM_SETBKCOLOR,0,(LPARAM)ColorParam);
SendDlgItemMessage(hGrRen,IDNAMELIST,LVM_SETTEXTBKCOLOR,0,(LPARAM)ColorParam);
SendDlgItemMessage(hGrRen,IDNAMELIST,LVM_SETTEXTCOLOR,0,(LPARAM)0);

SendDlgItemMessage(hGrRen,IDUDSTART,UDM_SETBUDDY,(WPARAM)GetDlgItem(hGrRen,IDSTARTCOUNT),0);
SendDlgItemMessage(hGrRen,IDUDSTART,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)0));
SendDlgItemMessage(hGrRen,IDUDSTART,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("GrRen","Start",0), 0));

SendDlgItemMessage(hGrRen,IDUDSTEP,UDM_SETBUDDY,(WPARAM)GetDlgItem(hGrRen,IDSTEPCOUNT),0);
SendDlgItemMessage(hGrRen,IDUDSTEP,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hGrRen,IDUDSTEP,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("GrRen","Step",1), 0));

SendDlgItemMessage(hGrRen,IDUDCOL,UDM_SETBUDDY,(WPARAM)GetDlgItem(hGrRen,IDCOLCOUNT),0);
SendDlgItemMessage(hGrRen,IDUDCOL,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hGrRen,IDUDCOL,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("GrRen","Col",1), 0));
//-----hGrRen-------

//-----hGrTag-------
CheckDlgButton(hGrTag,IDUSEPATH,GetKeyInt("GrTag","UsePath",0));
//-----hGrTag-------

//-----hNameTxt-------
hUpDateIcon	=		LoadImage(hInS,MAKEINTRESOURCE(IDUPDATEICON),IMAGE_ICON,16,16,0);
hTrimIcon	=		LoadImage(hInS,MAKEINTRESOURCE(IDTRIMICON),IMAGE_ICON,16,16,0);
hLeftIcon	=		LoadImage(hInS,MAKEINTRESOURCE(IDLEFTICON),IMAGE_ICON,16,16,0);
hRightIcon	=		LoadImage(hInS,MAKEINTRESOURCE(IDRIGHTICON),IMAGE_ICON,16,16,0);
hClearIcon	=		LoadImage(hInS,MAKEINTRESOURCE(IDCLEARICON),IMAGE_ICON,16,16,0);

SendDlgItemMessage(hNameTxt,IDUDSTART,UDM_SETBUDDY,(WPARAM)GetDlgItem(hNameTxt,IDSTARTCOUNT),0);
SendDlgItemMessage(hNameTxt,IDUDSTART,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)0));
SendDlgItemMessage(hNameTxt,IDUDSTART,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("NameTxt","Start",0), 0));

SendDlgItemMessage(hNameTxt,IDUDSTEP,UDM_SETBUDDY,(WPARAM)GetDlgItem(hNameTxt,IDSTEPCOUNT),0);
SendDlgItemMessage(hNameTxt,IDUDSTEP,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hNameTxt,IDUDSTEP,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("NameTxt","Step",1), 0));

SendDlgItemMessage(hNameTxt,IDUDCOL,UDM_SETBUDDY,(WPARAM)GetDlgItem(hNameTxt,IDCOLCOUNT),0);
SendDlgItemMessage(hNameTxt,IDUDCOL,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hNameTxt,IDUDCOL,UDM_SETPOS,0,(LPARAM)MAKELONG((short)GetKeyInt("NameTxt","Col",1), 0));

SendDlgItemMessage(hNameTxt,IDUDSRT,UDM_SETBUDDY,(WPARAM)GetDlgItem(hNameTxt,IDSRT),0);
SendDlgItemMessage(hNameTxt,IDUDSRT,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hNameTxt,IDUDSRT,UDM_SETPOS,0,(LPARAM)MAKELONG((short)1, 0));

SendDlgItemMessage(hNameTxt,IDUDEND,UDM_SETBUDDY,(WPARAM)GetDlgItem(hNameTxt,IDEND),0);
SendDlgItemMessage(hNameTxt,IDUDEND,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)32767, (short)1));
SendDlgItemMessage(hNameTxt,IDUDEND,UDM_SETPOS,0,(LPARAM)MAKELONG((short)1, 0));

char *sret=GetKey("NameTxt", "TextAfter", ". ");
if (sret)
	{
	SBStrTrim(&sret, "#");
	SetDlgItemText(hNameTxt, IDTXTAFTER, sret);
	free((char *) sret);
	}
//-----hNameTxt-------

char K[60];
for (int i=IDSTR7; i<=IDSTR9;i++)
	{
	LoadString(hInS,i,K,60);
	SendDlgItemMessage(hGrRen,IDTAG,CB_ADDSTRING,0,(LPARAM)K);
	SendDlgItemMessage(hGrTag,IDTAG,CB_ADDSTRING,0,(LPARAM)K);
	}

SendDlgItemMessage(hGrRen,IDTAG,CB_SETCURSEL,GetKeyInt("GrRen","Tag",0),0);
SendDlgItemMessage(hGrTag,IDTAG,CB_SETCURSEL,GetKeyInt("GrTag","Tag",0),0);

for (int i=IDSTR5; i<=IDSTR61;i++)
	{
	LoadString(hInS,i,K,60);
	SendDlgItemMessage(hTagInfo,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)K);
	SendDlgItemMessage(hGrRen,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)K);
	SendDlgItemMessage(hGrTag,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)K);
	}

SendDlgItemMessage(hTagInfo,IDTRANSLIT,CB_SETCURSEL,GetKeyInt("TagInfo","Translit",0),0);
SendDlgItemMessage(hGrRen,IDTRANSLIT,CB_SETCURSEL,GetKeyInt("GrRen","Translit",0),0);
SendDlgItemMessage(hGrTag,IDTRANSLIT,CB_SETCURSEL,GetKeyInt("GrTag","Translit",0),0);

SendDlgItemMessage(hTagInfo,IDREGISTR,CB_SETCURSEL,GetKeyInt("TagInfo","Registr",0),0);
SendDlgItemMessage(hGrRen,IDREGISTR,CB_SETCURSEL,GetKeyInt("GrRen","Registr",0),0);
SendDlgItemMessage(hGrTag,IDREGISTR,CB_SETCURSEL,GetKeyInt("GrTag","Registr",0),0);

for (int i=IDSTR10; i<=IDSTR24;i++)
	{
	LoadString(hInS,i,K,60);
	SendDlgItemMessage(hGrRen,IDMASK,CB_ADDSTRING,0,(LPARAM)K);
	if (i==IDSTR10)
		{
		sret=GetKey("GrRen", "Mask");
		if (sret)
			{
			SBStrTrim(&sret, "#");
			SetDlgItemText(hGrRen, IDMASK, sret);
			free((char *) sret);
			}

		sret=GetKey("GrTag", "Mask");
		if (sret)
			{
			SBStrTrim(&sret, "#");
			SetDlgItemText(hGrTag, IDMASK, sret);
			free((char *) sret);
			}
		}
	if (i<IDSTR19) SendDlgItemMessage(hGrTag,IDMASK,CB_ADDSTRING,0,(LPARAM)K);
	}
LoadString(hInS,IDSTR25,K,60);SendDlgItemMessage(hGrTag,IDMASK,CB_ADDSTRING,0,(LPARAM)K);

SendDlgItemMessage(hGrTag,IDMETOD,CB_SETCURSEL,GetKeyInt("GrTag","Metod",0),0);

for (int i=0;i<SIZE_GANR;i++)
	{
	SendDlgItemMessage(hTagInfo,IDGENRE1,CB_ADDSTRING,0,(LPARAM)GanR[i]);
	SendDlgItemMessage(hTagInfo,IDGENRE2,CB_ADDSTRING,0,(LPARAM)GanR[i]);
	}

if (StartCmd)
	{
	ParsCmd(StartCmd);
	free((char*) StartCmd);
	}
else
	{
	char dir[1000];
	getcwd(dir, 1000);
	sret=GetKey("File", "Folder", dir);
	if (sret)
		{
		char *ndir=GetLongPath(sret);
		if (ndir)
			{
			SetDlgItemText(hFile, IDFOLDER, ndir);
			free((char *) ndir);
			}

		free((char *) sret);
		}
	}
}
//----------------------------------------------------------------------
static BOOL CALLBACK MainFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
switch (msg)
{
case WM_DROPFILES:DragFunc(wParam);SetForegroundWindow(hwnd);break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	GetWindowPlacement(hwnd,&WP);char n[10];
	itoa(WP.rcNormalPosition.right-WP.rcNormalPosition.left,n,10); SetKey("Main", "Width", n);
	itoa(WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,n,10); SetKey("Main", "Height", n);

	H = hExec;GetWindowPlacement(H,&WP);
	int HEx = WP.rcNormalPosition.bottom-WP.rcNormalPosition.top;
	MoveWindow(H,0,HIWORD(lParam)-HEx,LOWORD(lParam),HEx,TRUE);

	H = hFile;GetWindowPlacement(H,&WP);
	int TFi = WP.rcNormalPosition.top;
	MoveWindow(H,0,TFi,LOWORD(lParam),HIWORD(lParam)-TFi-HEx,TRUE);
	}
	break;

case WM_CLOSE:
	{
	//HANDLE hProcess = NULL; DWORD ProcId; GetWindowThreadProcessId( hwnd, &ProcId ); if (ProcId>0) hProcess= OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcId); if (hProcess) {TerminateProcess(hProcess, 0);CloseHandle(hProcess);}
	if (ini) free((char *) ini);
	if (fileini) free((char *) fileini);
	if (hToolH32) FreeLibrary(hToolH32);
	ExitProcess(0);
	}
	break;
}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK FileFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,ColorFolder);return (DWORD)hBrushParam;

case WM_NOTIFY:	if ((int)wParam==IDFILELIST) ProcOnNotify(hwnd,(int)wParam,(LPNMHDR)lParam);break;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDRETURN: EnterFolder(); break;

	case IDABOUT:
		if (hAbout==NULL) hAbout = CreateDialog(hInS,MAKEINTRESOURCE(IDABOUTDIALOG), hMain, (DLGPROC) AboutDialogFunc);
		ShowWindow(hAbout,SW_SHOW);
		break;

	case IDSFOLDER:
		{
		char *dir=GetFolder();
		if (dir!=NULL) {SetDlgItemText(hwnd,IDFOLDER,dir);free((char*) dir);}
		}
		break;

	case IDFOLDER:if (HIWORD(wParam)==EN_CHANGE)
		{
		char *dir = GetItemChar(hwnd, LOWORD(wParam));
		SetKey("File", "Folder", dir);
		if (dir)
			{
			if (chdir(dir)==0)
				{
				ColorFolder = 0;
				ListCreate();
				}
			else ColorFolder = 255;
			free((char*) dir);
			}
		else ListCreate(FALSE);

		HWND H = GetDlgItem(hwnd, LOWORD(wParam));
		InvalidateRect(H,0,TRUE); UpdateWindow(H);
		}
		break;
	}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	H = GetDlgItem(hwnd,IDFILELIST);GetWindowPlacement(H,&WP);
	MoveWindow(H,0,WP.rcNormalPosition.top,LOWORD(lParam),HIWORD(lParam)-WP.rcNormalPosition.top,TRUE);

	SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)0,MAKELPARAM((int) LOWORD(lParam) - GetSystemMetrics(SM_CXEDGE)*2 -
	((SendMessage(H,LVM_GETCOUNTPERPAGE,0,0)<SendMessage(H,LVM_GETITEMCOUNT,0,0))*GetSystemMetrics(SM_CXVSCROLL)), 0));

	H = GetDlgItem(hwnd,IDSFOLDER);GetWindowPlacement(H,&WP);
	int wSF = WP.rcNormalPosition.right - WP.rcNormalPosition.left;

	MoveWindow(H,LOWORD(lParam)-wSF,WP.rcNormalPosition.top,wSF,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);

	H = GetDlgItem(hwnd,IDFOLDER);GetWindowPlacement(H,&WP);

	MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,
			LOWORD(lParam)-WP.rcNormalPosition.left-wSF,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	}
	break;

case WM_DRAWITEM:
	{
	HWND H= GetDlgItem(hwnd,IDFILELIST); InvalidateRect(H,0,TRUE); UpdateWindow(H);

	switch (wParam)
		{
		case IDABOUT: DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hAboutIcon); break;
		}
	}
	break;
}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK ExecFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
switch (msg)
{
case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDALL:
		{
		char n[10];	itoa(IsDlgButtonChecked(hwnd,IDALL),n,10); SetKey("Exec", "All", n);
		if (SendDlgItemMessage(hFile, IDFOLDER, WM_GETTEXTLENGTH, 0, 0)) ListCreate();
		else ListCreate(FALSE);
		}
		break;

	case IDSELALL:
		{
		SendMessage(hwnd,WM_COMMAND,IDDESALL,0);
		DeSelectItemPos(0);

		ReadTrue=FALSE;
		int LastSel=-1;

		int c = SendDlgItemMessage(hFile,IDFILELIST, LVM_GETITEMCOUNT,0,0);
		LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM)); lvItem.mask = LVIF_IMAGE;
		for (int i=0;i<c;i++)
			{
			lvItem.iItem = i;
			if (SendDlgItemMessage(hFile,IDFILELIST,LVM_GETITEM,0,(LPARAM)&lvItem) && lvItem.iImage==2)
				{
				if (LastSel>-1) SelectItemPos(LastSel);
				LastSel=i;
				}
			}
		ReadTrue=TRUE;

		if (LastSel>-1) SelectItemPos(LastSel);
		SetFocus(GetDlgItem(hFile,IDFILELIST));
		}
		break;

	case IDDESALL:
		{
		ReadTrue=FALSE;
		int c = SendDlgItemMessage(hFile,IDFILELIST, LVM_GETITEMCOUNT,0,0);
		for (int i=0; i<c; i++) DeSelectItemPos(i);
		ReadTrue=TRUE;
		SelectItemPos(0);
		}
		break;

	case IDEXEC:
		{
		DWORD dwThID;
		struct PData *PD=(struct PData*) malloc(sizeof(struct PData));
		PD->pb=TRUE;

		EnableWindow(hMain,FALSE);

		switch (LastTab)
			{
			case 0:PD->type=3;CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Execute,(PVOID)PD,0,&dwThID);break;
			case 1:PD->type=5;Execute(PD);break;
			case 2:PD->type=6;CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Execute,(PVOID)PD,0,&dwThID);break;
			case 3:PD->type=7;Execute(PD);break;
			case 4:EnableWindow(hMain,TRUE);break;
			}

		}
		break;

	case IDREFRESH:
		{
		ListCreate();
		}
		break;
	}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	H = GetDlgItem(hwnd,IDTABCONTROL);GetWindowPlacement(H,&WP);
	MoveWindow(H,0,WP.rcNormalPosition.top,LOWORD(lParam),HIWORD(lParam)-WP.rcNormalPosition.top,TRUE);

	H = hTagInfo;GetWindowPlacement(H,&WP);
	MoveWindow(hTagInfo,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-5,HIWORD(lParam)-WP.rcNormalPosition.top-4,TRUE);
	MoveWindow(hGrRen,	WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-5,HIWORD(lParam)-WP.rcNormalPosition.top-4,TRUE);
	MoveWindow(hGrTag,	WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-5,HIWORD(lParam)-WP.rcNormalPosition.top-4,TRUE);
	MoveWindow(hNameTxt,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-5,HIWORD(lParam)-WP.rcNormalPosition.top-4,TRUE);
	MoveWindow(hIntegr,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-5,HIWORD(lParam)-WP.rcNormalPosition.top-4,TRUE);

	H = GetDlgItem(hwnd,IDEXEC);GetWindowPlacement(H,&WP);
	MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);

	H = GetDlgItem(hwnd,IDPROGRBAR);GetWindowPlacement(H,&WP);
	MoveWindow(H,0,WP.rcNormalPosition.top,LOWORD(lParam),WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	}
	break;


case WM_DRAWITEM:
	switch (wParam)
		{
		case IDSELALL:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hSelAllIcon); break;
		case IDDESALL:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hDesAllIcon); break;
		case IDEXEC:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hExecIcon); break;
		case IDREFRESH:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hRefreshIcon); break;
		}
	break;

case WM_NOTIFY:
	if ((LOWORD(wParam)==IDTABCONTROL)&&(((LPNMHDR)lParam)->code==TCN_SELCHANGE))
		{
		BYTE iP = SendDlgItemMessage(hwnd,IDTABCONTROL,TCM_GETCURSEL,0,0);
		if (LastTab!=iP)
			{
			HWND hH=NULL,hS=NULL;
			switch (LastTab)
				{
				case 0:hH = hTagInfo;break;
				case 1:hH = hGrRen;break;
				case 2:hH = hGrTag;break;
				case 3:hH = hNameTxt;break;
				case 4:hH = hIntegr;break;
				}
			if (hH) ShowWindow(hH,SW_HIDE);

			LastTab=iP;
			switch (LastTab)
				{
				case 0:hS = hTagInfo;ReadMediaFile(LastDir);Enable();break;
				case 1:hS = hGrRen;UpdateNameList();break;
				case 2:hS = hGrTag;UpdateTagsList(LastDir,FALSE);Enable();break;
				case 3:hS = hNameTxt;break;
				case 4:hS = hIntegr;break;
				}
			if (hS) ShowWindow(hS,SW_SHOW);
			}
		}
	break;

}
return FALSE;
}
//----------------------------------------------------------------------
LRESULT CALLBACK AutoCheckBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
switch (msg)
	{
	case WM_PAINT:
		{
		PAINTSTRUCT PS;
		HDC hDC = BeginPaint(hwnd, &PS);
		if (hDC)
			{
			DRAWITEMSTRUCT di; ZeroMemory(&di, sizeof(DRAWITEMSTRUCT));
			di.hDC=hDC;
			GetClientRect(hwnd, &di.rcItem);
			di.hwndItem=hwnd;
			DrawButtonWithIcon(&di, hAllIcon, TRUE);
			EndPaint(hwnd,&PS);
			}
		return FALSE;
		}
		break;
	}
return CallWindowProc(AutoCheckBoxDef, hwnd, msg, wParam, lParam);
}
//----------------------------------------------------------------------
static BOOL CALLBACK TagInfoFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,0);return (DWORD)hBrushParam;

case WM_CTLCOLORSTATIC:
	{
	char cl[9];
	GetClassName((HWND)lParam,cl,9);
	if ((strcmpi(cl,"Edit")==0)||(strcmpi(cl,"ComboBox")==0)){SetBkColor((HDC)wParam,ColorParam);return (DWORD)hBrushParam;}
	}
	break;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{

	case IDT12:{struct PData *PD=(struct PData*) malloc(sizeof(struct PData));PD->pb=TRUE;PD->type=1;Execute(PD);}break;
	case IDT21:{struct PData *PD=(struct PData*) malloc(sizeof(struct PData));PD->pb=TRUE;PD->type=2;Execute(PD);}break;

	case IDALL1:
		{
		int ic=IsDlgButtonChecked(hwnd, IDALL1);
		for (int i=IDCHTRACK1; i<=IDCHGENRE1; i++) {CheckDlgButton(hwnd, i, ic); SendMessage(hwnd, WM_COMMAND, i, 0);}
		}
		break;

	case IDALL2:
		{
		int ic=IsDlgButtonChecked(hwnd, IDALL2);
		for (int i=IDCHTRACK2; i<=IDCHGENRE2; i++) {CheckDlgButton(hwnd, i, ic); SendMessage(hwnd, WM_COMMAND, i, 0);}
		}
		break;

	case IDREGISTR:if (SetCombo(hwnd,wParam,"TagInfo","Registr")) ReadMediaFile(LastDir);break;
	case IDTRANSLIT:if (SetCombo(hwnd,wParam,"TagInfo","Translit")) ReadMediaFile(LastDir);break;

	case IDCHTRACK1:case IDCHTITLE1:case IDCHARTIST1:case IDCHALBUM1:case IDCHYEAR1:case IDCHGENRE1:case IDCHCOMM1:
	Check(hwnd,LOWORD(wParam),LOWORD(wParam)-8);
	break;

	case IDCHTRACK2:case IDCHTITLE2:case IDCHARTIST2:case IDCHALBUM2:case IDCHYEAR2:case IDCHGENRE2:case IDCHCOMM2:
	case IDCHCOMP2:case IDCHORIG2:case IDCHCOPY2:case IDCHURL2:case IDCHENCOD2:
	Check(hwnd,LOWORD(wParam),LOWORD(wParam)-13);
	break;

	}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	H = GetDlgItem(hwnd,IDTAG2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-2,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDTRACK2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDTITLE2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDARTIST2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDALBUM2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDCOMM2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDCOMP2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDORIG2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDCOPY2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDURL2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDENCOD2);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	}
	break;

}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK ProcOnNotify(HWND hWnd, int idFrom, NMHDR* pnmhdr)
{
switch(pnmhdr->code)
{
case LVN_ITEMCHANGED:
	{
	int LastColSel=ColSel;
	ColSel=0;
	int c = SendDlgItemMessage(hFile,IDFILELIST, LVM_GETITEMCOUNT,0,0);

	LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
	lvItem.mask = LVIF_IMAGE|LVIF_STATE;
	lvItem.stateMask=LVIS_SELECTED;

	for (int i=0;i<c;i++)
		{
		lvItem.iItem = i;
		if (SendDlgItemMessage(hFile,IDFILELIST,LVM_GETITEM,0,(LPARAM)&lvItem) && lvItem.iImage==2 && lvItem.state==LVIS_SELECTED) ColSel++;
		}

	if	(((NM_LISTVIEW *)pnmhdr)->uChanged & LVIF_STATE)
		{
		if (ReadTrue)
			switch (LastTab)
				{
				case 0:Enable();break;
				case 2:Enable();break;
				}

		if (((NM_LISTVIEW *)pnmhdr)->uNewState & LVIS_SELECTED)
			{
			char path[1000];

			LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
			lvItem.mask = LVIF_TEXT;
			lvItem.cchTextMax = 1000;

			if (IsDlgButtonChecked(hExec,IDALL)==BST_CHECKED) lvItem.pszText = path;
			else
				{
				getcwd(path,1000);
				if (path[strlen(path)-1]!='\\') strcat(path,"\\");
				lvItem.pszText = path+strlen(path);
				}

			int y = SendDlgItemMessage(hFile,IDFILELIST, LVM_GETITEMTEXT,((NM_LISTVIEW *)pnmhdr)->iItem,(LPARAM)&lvItem);
			if (y)
				{
				if (LastDir) {free((char*) LastDir);LastDir = NULL;}LastDir=strdup(path);

				if (ReadTrue)

					switch (LastTab)
						{
						case 0:ReadMediaFile(LastDir);break;
						case 1:UpdateNameList();break;
						case 2:UpdateTagsList(LastDir,FALSE);break;
						}
				}
			else
				{
				switch (LastTab)
					{
					case 0:	ClearInfo(0);break;
					case 2:	ClearInfo(4);break;
					default: ClearInfo(5);
					}
				}
			}
		}
	}
	break;

case NM_DBLCLK: EnterFolder(); break;
}
return FALSE;
}
//----------------------------------------------------------------------
void EnterFolder()
{
char dir[1000]; getcwd(dir,1000);
if (LastDir) free((char*) LastDir); LastDir=strdup(dir);

if (dir[strlen(dir)-1]!='\\') strcat(dir,"\\");

LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));

lvItem.mask = LVIF_TEXT	| LVIF_IMAGE;
lvItem.iItem = GetItemSel();
lvItem.cchTextMax = 1000;
lvItem.pszText = dir + strlen(dir);

if (SendDlgItemMessage(hFile,IDFILELIST,LVM_GETITEM,0,(LPARAM)&lvItem))
	{
	switch (lvItem.iImage)
		{
		case 1:
			{
			char *pos = strrchr(dir,'\\');if (pos) pos[0]=0;
			pos = strrchr(dir,'\\');
			if (pos) {if (pos>dir+2) pos[0]=0; else {pos[0]='\\'; pos[1]=0;}}
			else dir[0]=0;
			}
			break;

		case 3:
		case 4:
		case 5:
		case 6:
			{
			char *pos = strrchr(dir,'\\');
			if (pos) {strcpy(dir,pos+1); strcat(dir,"\\");}
			}
			break;
		}

	if (lvItem.iImage!=2) SetDlgItemText(hFile,IDFOLDER,dir);
	}
}
//----------------------------------------------------------------------
static BOOL CALLBACK GrRenFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
static WORD SRen=0,ERen=0;

switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,0);return (DWORD)hBrushParam;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDTRIM:{char n[2];itoa(IsDlgButtonChecked(hwnd,LOWORD(wParam)),n,10);SetKey("GrRen","Trim",n);UpdateNameList();}break;
	case IDRANDOM: {char n[2];itoa(IsDlgButtonChecked(hwnd,LOWORD(wParam)),n,10);SetKey("GrRen","Random",n);UpdateNameList();}break;

	case IDREGISTR:SetCombo(hwnd,wParam,"GrRen","Registr");UpdateNameList();break;
	case IDTRANSLIT:SetCombo(hwnd,wParam,"GrRen","Translit");UpdateNameList();break;
	case IDTAG:SetCombo(hwnd,wParam,"GrRen","Tag");UpdateNameList();break;

	case IDFIND:if (HIWORD(wParam)==EN_CHANGE) UpdateNameList(); break;
	case IDPAST:if (HIWORD(wParam)==EN_CHANGE) UpdateNameList(); break;

	case IDMASK:
		{
		switch (HIWORD(wParam))
			{
			case CBN_KILLFOCUS: break;
			case CBN_SETFOCUS: SendDlgItemMessage(hwnd, IDMASK, CB_SETEDITSEL, 0, MAKELPARAM(SRen,ERen)); break;

			case CBN_EDITCHANGE:
				{
				char *n = GetItemChar(hwnd, IDMASK);
				if (n)
					{
					char *MaskRen=(char*) malloc((strlen(n)+3)*sizeof(char));
					sprintf(MaskRen, "#%s#", n);
					free((char*) n);

					SetKey("GrRen", "Mask", MaskRen); free((char*) MaskRen);
					UpdateNameList();
					}
				}
				break;

			case CBN_SELCHANGE:
				{
				char *n = GetItemCombo(hwnd, IDMASK);
				if (n)
					{
					SetDlgItemText(hwnd, IDMASK, n); free((char*) n);
					SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDMASK, CBN_EDITCHANGE), 0);
					}
				}
				break;

			default: SendDlgItemMessage(hwnd, IDMASK, CB_GETEDITSEL, (WPARAM)&SRen, (LPARAM)&ERen);
			}
		}
		break;

	case IDSTARTCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("GrRen","Start",n); if (n) free((char *)n);	UpdateNameList();} break;
	case IDSTEPCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("GrRen","Step",n); if (n) free((char *)n); UpdateNameList();} break;
	case IDCOLCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("GrRen","Col",n); if (n) free((char *)n);	UpdateNameList();} break;

	case IDMTRACK: case IDMTITLE: case IDMARTIST: case IDMALBUM: case IDMYEAR: case IDMCOMM: case IDMNAME: case IDMCOUNT:
		{
		char n[3]; GetDlgItemText(hwnd,LOWORD(wParam),n,3); n[2]=0;

		char *s = GetItemChar(hwnd, IDMASK);
		if (s==NULL) s=strdup("");

		char *sta = strdup(s); sta[SRen]=0;
		char *end = strdup(s + ERen);

		char *sbuf=(char *)malloc((strlen(sta) + strlen(end) + 3)*sizeof(char));
		sprintf(sbuf, "%s%s%s", sta, n, end);

		SetDlgItemText(hwnd, IDMASK, sbuf);
		SRen+=2; ERen=SRen;

		free((char*) sbuf);
		free((char*) end);
		free((char*) sta);
		free((char*) s);

		SetFocus(GetDlgItem(hwnd, IDMASK));
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDMASK, CBN_EDITCHANGE), 0);
		}
		break;
	}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	H = GetDlgItem(hwnd,IDNAMELIST);GetWindowPlacement(H,&WP);
	MoveWindow(H,0,WP.rcNormalPosition.top,LOWORD(lParam),HIWORD(lParam)-WP.rcNormalPosition.top,TRUE);

	int Ws =  LOWORD(lParam) - GetSystemMetrics(SM_CXEDGE)*2 - GetSystemMetrics(SM_CXVSCROLL);
	int Ws0 = Ws/2.;
	int Ws1 = Ws - Ws0;
	int i=0;
	SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)i++,MAKELPARAM(0, 0));
	SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)i++,MAKELPARAM(0, 0));
	SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)i++,MAKELPARAM(Ws0, 0));
	SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)i++,MAKELPARAM(Ws1, 0));

	ShowWindow(H,SW_HIDE),ShowWindow(H,SW_SHOW);
	}
	break;

case WM_DRAWITEM:
	{
	HWND H= GetDlgItem(hwnd,IDNAMELIST); InvalidateRect(H,0,TRUE); UpdateWindow(H);
	}
	break;
}

return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK GrTagFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
static WORD STag=0,ETag=0;

switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,0);return (DWORD)hBrushParam;

case WM_CTLCOLORSTATIC:
	{
	char cl[9];
	GetClassName((HWND)lParam,cl,9);
	if ((strcmpi(cl,"Edit")==0)||(strcmpi(cl,"ComboBox")==0)){SetBkColor((HDC)wParam,ColorParam);return (DWORD)hBrushParam;}
	}
	break;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDUSEPATH:
		{
		char n[2];itoa(IsDlgButtonChecked(hwnd,LOWORD(wParam)),n,10);SetKey("GrTag","UsePath",n);
		UpdateTagsList(LastDir,FALSE);
		}
		break;

	case IDREGISTR:SetCombo(hwnd,wParam,"GrTag","Registr");UpdateTagsList(LastDir,FALSE);break;
	case IDTRANSLIT:SetCombo(hwnd,wParam,"GrTag","Translit");UpdateTagsList(LastDir,FALSE);break;
	case IDTAG:SetCombo(hwnd,wParam,"GrTag","Tag");UpdateTagsList(LastDir,FALSE);break;
	case IDMETOD:SetCombo(hwnd,wParam,"GrTag","Metod");UpdateTagsList(LastDir,FALSE);break;

	case IDFIND: case IDPAST: if (HIWORD(wParam)==EN_CHANGE) UpdateTagsList(LastDir,FALSE); break;

	case IDCHTRACK1:case IDCHTITLE1:case IDCHARTIST1:case IDCHALBUM1:case IDCHYEAR1:case IDCHCOMM1:	Check(hwnd,LOWORD(wParam),LOWORD(wParam)-8); break;

	case IDMASK:
		{
		switch (HIWORD(wParam))
			{
			case CBN_KILLFOCUS: break;
			case CBN_SETFOCUS: SendDlgItemMessage(hwnd, IDMASK, CB_SETEDITSEL, 0, MAKELPARAM(STag,ETag));break;

			case CBN_EDITCHANGE:
				{
				char *n = GetItemChar(hwnd, IDMASK);
				if (n)
					{
					char *MaskTag=(char*) malloc((strlen(n)+3)*sizeof(char));
					sprintf(MaskTag, "#%s#", n);
					free((char*) n);

					SetKey("GrTag", "Mask", MaskTag);
					UpdateTagsList(LastDir, FALSE);
					free((char*) MaskTag);
					}
				}
				break;

			case CBN_SELCHANGE:
				{
				char *n = GetItemCombo(hwnd, IDMASK);
				if (n)
					{
					SetDlgItemText(hwnd, IDMASK, n); free((char*) n);
					SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDMASK, CBN_EDITCHANGE), 0);
					}
				}
				break;

			default:SendDlgItemMessage(hwnd, IDMASK, CB_GETEDITSEL, (WPARAM)&STag, (LPARAM)&ETag);
			}
		}
		break;

	case IDMTRACK: case IDMTITLE: case IDMARTIST: case IDMALBUM: case IDMYEAR: case IDMCOMM:
		{
		char n[3]; GetDlgItemText(hwnd,LOWORD(wParam),n,3); n[2]=0;

		char *s = GetItemChar(hwnd, IDMASK);
		if (s==NULL) s=strdup("");

		char *sta = strdup(s); sta[STag]=0;
		char *end = strdup(s + ETag);

		char *sbuf=(char *)malloc((strlen(sta) + strlen(end) + 3)*sizeof(char));
		sprintf(sbuf, "%s%s%s", sta, n, end);

		SetDlgItemText(hwnd, IDMASK, sbuf);
		STag+=2;ETag=STag;

		free((char*) sbuf);
		free((char*) end);
		free((char*) sta);
		free((char*) s);

		SetFocus(GetDlgItem(hwnd, IDMASK));
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDMASK, CBN_EDITCHANGE), 0);
		}
		break;

	}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H;

	H = GetDlgItem(hwnd,IDTAG3);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-2,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDTRACK1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDTITLE1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDARTIST1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDALBUM1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDYEAR1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	H = GetDlgItem(hwnd,IDCOMM1);GetWindowPlacement(H,&WP);MoveWindow(H,WP.rcNormalPosition.left,WP.rcNormalPosition.top,LOWORD(lParam)-WP.rcNormalPosition.left-6,WP.rcNormalPosition.bottom-WP.rcNormalPosition.top,TRUE);
	}
	break;

}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK NameTxtFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,0);return (DWORD)hBrushParam;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDSTARTCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("NameTxt","Start",n); if (n) free((char *) n);} break;
	case IDSTEPCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("NameTxt","Step",n); if (n) free((char *) n);}break;
	case IDCOLCOUNT:if (HIWORD(wParam)==EN_CHANGE) {char *n=GetItemChar(hwnd, LOWORD(wParam)); SetKey("NameTxt","Col",n); if (n) free((char *) n);}break;

	case IDTXTAFTER: if (HIWORD(wParam)==EN_CHANGE)
		{
		char *s = NULL;
		char *n = GetItemChar(hwnd, LOWORD(wParam));
		if (n)
			{
			s=(char*) malloc((strlen(n)+3)*sizeof(char));
			sprintf(s, "#%s#", n);
			free((char*) n);
			}
		SetKey("NameTxt", "TextAfter", s);
		if (s) free((char *) s);
		}
	break;

	case IDTRIM2:
		{
		char *n = GetItemChar(hwnd, IDMEMO);
		if (n)
			{
			int L1;
			n = SearchAndPast(n,"\t"," ");
			int L=strlen(n);
			L1=0; while (L>0 && L!=L1) {L1=L; n = SearchAndPast(n,"\r\n\r\n","\r\n"); L=strlen(n);}
			L1=0; while (L>0 && L!=L1) {L1=L; n = SearchAndPast(n,"  "," "); L=strlen(n);}
			L1=0; while (L>0 && L!=L1) {L1=L; n = SearchAndPast(n," \r\n","\r\n"); L=strlen(n);}
			L1=0; while (L>0 && L!=L1) {L1=L; n = SearchAndPast(n,"\r\n ","\r\n"); L=strlen(n);}

			SBStrTrim(&n, " \r\n");
			SetDlgItemText(hwnd, IDMEMO, n);

			free((char*) n);
			}
		}
		break;

	case IDTRIMST: TrimStart(); break;
	case IDTRIMED: TrimEnd(); break;
	case IDCOUNT: CountExec(); break;
	case IDCLEAR: SetDlgItemText(hwnd,IDMEMO,NULL);break;

	case IDFINDPAST:
		{
		char *n = GetItemChar(hwnd, IDMEMO);
		if (n)
			{
			char *find = GetItemChar(hwnd, IDFIND);
			char *past = GetItemChar(hwnd, IDPAST);

			n = SearchAndPast(n, find, past);
			if (find) free((char *) find);
			if (past) free((char *) past);

			SetDlgItemText(hwnd, IDMEMO, n);
			free((char*) n);
			}
		}
		break;

	}
	break;

case WM_DRAWITEM:
	switch (wParam)
		{
		case IDFINDPAST:DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hUpDateIcon); break;
		case IDCOUNT:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hUpDateIcon); break;
		case IDTRIM2:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hTrimIcon); break;
		case IDTRIMST:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hLeftIcon); break;
		case IDTRIMED:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hRightIcon); break;
		case IDCLEAR:	DrawButtonWithIcon((LPDRAWITEMSTRUCT)lParam, hClearIcon); break;
		}
	break;

case WM_SIZE:
	{
	WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
	HWND H = GetDlgItem(hwnd,IDMEMO);GetWindowPlacement(H,&WP);
	MoveWindow(H,0,WP.rcNormalPosition.top,LOWORD(lParam),HIWORD(lParam)-WP.rcNormalPosition.top,TRUE);
	}
	break;

}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK IntegrFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
switch (msg)
{
case WM_CTLCOLOREDIT:SetBkColor((HDC)wParam,ColorParam);SetTextColor((HDC)wParam,0);return (DWORD)hBrushParam;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDLANG:
		if (HIWORD(wParam)==CBN_SELCHANGE)
			{
			char n[20];
			SendDlgItemMessage(hwnd,LOWORD(wParam), CB_GETLBTEXT, SendDlgItemMessage(hwnd,LOWORD(wParam),CB_GETCURSEL,0,0), (LPARAM)n);

			char *oldn=GetKey("Main", "ini", "rus.ini");
			if (oldn)
				{
				if (strcmpi(oldn,n)!=0)
					{
					SetKey("Main", "ini", n);
					char my[1000]; GetModuleFileName(NULL,my,1000); ShellExecute(NULL,"open",my,NULL,NULL,SW_RESTORE);
					SendMessage(hMain,WM_CLOSE,0,0);
					}
				free((char *) oldn);
				}
			}
		break;

	case IDCRERL: CreateLabel();break;

	case IDFILE:
		{
		HKEY hk;

		char T[128];DWORD LT=sizeof(T);
		if (RegOpenKey(HKEY_CLASSES_ROOT,".mp3",&hk)==ERROR_SUCCESS)
				{if ((RegQueryValueEx(hk,NULL,NULL,NULL,T,&LT)!=ERROR_SUCCESS)||(strlen(T)==0)) {strcpy(T,"mp3file");RegSetValue(hk,NULL,REG_SZ,T,strlen(T)+1);}}
		else	{RegCreateKey(HKEY_CLASSES_ROOT,".mp3",&hk);strcpy(T,"mp3file");RegSetValue(hk,NULL,REG_SZ,T,strlen(T)+1);}

		if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)
			{
			char *shell = GetItemChar(hwnd, IDEDFILE);
			if (shell)
				{
				char command[1000]; GetModuleFileName(NULL, command, 1000); strcat(command," %1");

				strcat(T,"\\shell\\SBTag");RegCreateKey(HKEY_CLASSES_ROOT,T,&hk);RegSetValue(hk,NULL,REG_SZ,shell,strlen(shell)+1);
				strcat(T,"\\command");RegCreateKey(HKEY_CLASSES_ROOT,T,&hk);RegSetValue(hk,NULL,REG_SZ,command,strlen(command)+1);

				free((char *) shell);
				}
			}
		else
			{
			strcat(T,"\\shell\\SBTag");RegOpenKey(HKEY_CLASSES_ROOT,T,&hk);RegDeleteKey(hk,"command");
			T[strlen(T)-6]=0;RegOpenKey(HKEY_CLASSES_ROOT,T,&hk);RegDeleteKey(hk,"SBTag");
			}
		RegCloseKey(hk);
		}
		break;

	case IDDIR:
		{
		HKEY hk;

		if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)
			{
			char *shell = GetItemChar(hwnd, IDEDDIR);
			if (shell)
				{
				RegCreateKey(HKEY_CLASSES_ROOT,"Directory\\shell\\SBTag",&hk);RegSetValue(hk,NULL,REG_SZ,shell,strlen(shell)+1);
				RegCreateKey(HKEY_CLASSES_ROOT,"Directory\\shell\\SBTag\\command",&hk);

				char command[1000]; GetModuleFileName(NULL,command,1000); strcat(command," %0");
				RegSetValue(hk,NULL,REG_SZ,command,strlen(command)+1);

				free((char *) shell);
				}
			}
		else
			{
			RegOpenKey(HKEY_CLASSES_ROOT,"Directory\\shell\\SBTag",&hk);RegDeleteKey(hk,"command");
			RegOpenKey(HKEY_CLASSES_ROOT,"Directory\\shell",&hk);RegDeleteKey(hk,"SBTag");
			}
		RegCloseKey(hk);
		}
		break;

	case IDEDFILE: case IDEDDIR: if (HIWORD(wParam)==EN_CHANGE)
			{
			char *n = GetItemChar(hwnd, LOWORD(wParam));
			if (n)
				{
				switch (LOWORD(wParam))
					{
					case IDEDFILE: SetKey("Integr", "EdFile", n);break;
					case IDEDDIR: SetKey("Integr", "EdDir", n);break;
					}
				free((char *) n);
				}
			}
		break;
	}
	break;
}
return FALSE;
}
//----------------------------------------------------------------------
static BOOL CALLBACK AboutDialogFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
static HCURSOR hC=NULL;
static HBRUSH hBrushBackStatic=NULL;
static HFONT hFontParam=NULL,hFontBig=NULL;

switch (msg)
{
case WM_CTLCOLORSTATIC:
	{
	int ID = GetDlgCtrlID((HWND)lParam);
	if (ID == IDVERSION) SetTextColor((HDC)wParam,128);
	if ((ID == IDMAILTO)||(ID == IDHTTPTO)) SetTextColor((HDC)wParam,10485760);
	SetBkColor((HDC)wParam,GetSysColor(COLOR_BTNFACE));
	return (DWORD)hBrushBackStatic;
	}
	break;

case WM_INITDIALOG:
	{
	hBrushBackStatic = GetSysColorBrush(COLOR_BTNFACE);

	HFONT hFont = (HFONT)SendMessage(hwnd,WM_GETFONT,0,0);
	LOGFONT lFont;
	GetObject(hFont, sizeof(LOGFONT), (LPSTR)&lFont);
	lFont.lfWeight = FW_BOLD;
	hFontParam = CreateFontIndirect(&lFont);
	lFont.lfHeight = (long)(lFont.lfHeight*1.2);
	lFont.lfUnderline=TRUE;
	hFontBig = CreateFontIndirect(&lFont);

	hC = LoadCursor(NULL, IDC_HAND);

	SendDlgItemMessage(hwnd,IDVERSION,WM_SETFONT,(WPARAM)hFontParam,FALSE);
	SendDlgItemMessage(hwnd,IDMAILTO,WM_SETFONT,(WPARAM)hFontBig,FALSE);
	SendDlgItemMessage(hwnd,IDHTTPTO,WM_SETFONT,(WPARAM)hFontBig,FALSE);

	SetForegroundWindow(hwnd);
	}
	break;

case WM_MOUSEMOVE:
	{
	POINT cur;cur.x=LOWORD(lParam);cur.y=HIWORD(lParam);
	int ID = GetDlgCtrlID(ChildWindowFromPoint(hwnd,cur));
	if ((ID==IDMAILTO)||(ID==IDHTTPTO)) SetCursor(hC);
	}
	break;

case WM_LBUTTONDOWN:
	{
	POINT cur;cur.x=LOWORD(lParam);cur.y=HIWORD(lParam);
	int ID = GetDlgCtrlID(ChildWindowFromPoint(hwnd,cur));
	switch (ID)
		{
		case IDMAILTO:
			{
			char *T="?Subject=";
			char *B="&body=ѕривет,Serbis";
			char *M=GetItemChar(hwnd,IDMAILTO);
			char *S=GetItemChar(hwnd,IDVERSION);
			char *D=GetItemChar(hwnd,IDDATE);


			char *Sh=(char*)malloc((strlen(M)+strlen(T)+strlen(S)+strlen(S)+strlen(B)+3)*sizeof(char));
			sprintf(Sh, "%s%s%s(%s)%s", M, T, S, D, B);

			free((char*) M);
			free((char*) S);
			free((char*) D);

			ShellExecute(0, "Open",Sh, NULL, NULL, 1);free((char*) Sh);
			}
			break;

		case IDHTTPTO:
			{
			char *M=GetItemChar(hwnd, IDHTTPTO);
			ShellExecute(0, "Open",M, NULL, NULL, 1);
			free((char*) M);
			}
			break;
		}
	 }
	break;

case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDOK: SendMessage(hwnd,WM_CLOSE,0,0);break;
	}
	break;

case WM_CLOSE:

	if (hFontParam) DeleteObject(hFontParam);
	if (hFontBig) DeleteObject(hFontBig);
	if (hC) DeleteObject(hC);
	if (hBrushBackStatic) DeleteObject(hBrushBackStatic);

	hAbout=NULL;
	EndDialog(hwnd,0);
	DestroyWindow(hwnd);
	break;
}
return FALSE;
}
//----------------------------------------------------------------------
void SelectedLast()
{
if (LastDir)
	{
	if (IsDlgButtonChecked(hExec,IDALL)==BST_CHECKED) SetItemSel(LastDir);
	else {char *pos = strrchr(LastDir,'\\');if (pos) SetItemSel(pos+1);}
	}
else SelectItemPos(0);
SetFocus(GetDlgItem(hFile,IDFILELIST));
}
//---------------------------------------------------------------------------
char* GetFolder()
{
char dir[1000];
BROWSEINFO bi = {0};

bi.hwndOwner = hMain;
bi.pidlRoot = NULL;
bi.lpszTitle = "”кажите каталог";
bi.ulFlags = BIF_RETURNONLYFSDIRS;
bi.lpfn = BrowseCallbackProc;
bi.lParam = 0;

LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

if ( (pidlSel) && (SHGetPathFromIDList(pidlSel,dir)) ) return strdup(dir);
return NULL;
}
//----------------------------------------------------------------------
int CALLBACK  BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
switch(uMsg)
{
case BFFM_INITIALIZED:
	{
	char dir[1000];getcwd(dir,1000);
	SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)dir);
	}
    break;
}
return 0;
}
//----------------------------------------------------------------------
void ListCreate(BOOL F=TRUE)
{
SendDlgItemMessage(hFile,IDFILELIST,LVM_DELETEALLITEMS,0,0);

if (F)
	{
	ReadAndDrowFolder(TRUE,IsDlgButtonChecked(hExec,IDALL));
	SendDlgItemMessage(hFile,IDLISTTEMP,LB_RESETCONTENT,0,0);
	SelectedLast();
	}
else
	{
	BYTE nPos = 0;
	char sDrive[] = "?:";

	DWORD dwDriveList = GetLogicalDrives ();

	while (dwDriveList)
		{
		if (dwDriveList & 1)
			{
			sDrive[0]='A' + nPos;
			switch (GetDriveType(sDrive))
				{
				case DRIVE_REMOVABLE: InsItem(sDrive,3);break;
				case DRIVE_FIXED: case DRIVE_RAMDISK: InsItem(sDrive,4);break;
				case DRIVE_CDROM: InsItem(sDrive,5);break;
				case DRIVE_REMOTE: InsItem(sDrive,6);break;
				}
			}
		dwDriveList >>= 1;
		nPos++;
		}

	char curdir[10];getcwd(curdir,10);curdir[2]=0;
	SetItemSel(curdir);
	}

WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);
HWND H = GetDlgItem(hFile,IDFILELIST);GetWindowPlacement(H,&WP);
SendMessage(H,LVM_SETCOLUMNWIDTH,(WPARAM)0,MAKELPARAM(WP.rcNormalPosition.right - GetSystemMetrics(SM_CXEDGE)*2 -
	((SendMessage(H,LVM_GETCOUNTPERPAGE,0,0)<SendMessage(H,LVM_GETITEMCOUNT,0,0))*GetSystemMetrics(SM_CXVSCROLL)), 0));

}
//----------------------------------------------------------------------
void ReadAndDrowFolder(BOOL H, BOOL B)
{
char curdir[1000]; getcwd(curdir,1000);
int l=strlen(curdir)-1; if (curdir[l]!='\\') {curdir[l+1]='\\';curdir[l+2]=0;}

if (H) InsItem("",1);

WIN32_FIND_DATA file; ZeroMemory(&file, sizeof(WIN32_FIND_DATA));
HANDLE fs=FindFirstFile("*.*", &file);
if (fs != INVALID_HANDLE_VALUE)
do	{
	if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	if (! (!strcmpi(file.cFileName,".") || !strcmpi(file.cFileName,"..") ))
		{
		if (B)
			{
			if (!chdir(file.cFileName))
				{
				ReadAndDrowFolder(FALSE, B);
				chdir(curdir);
				}
			}
		else SendDlgItemMessage(hFile, IDLISTTEMP, LB_ADDSTRING, 0, (LPARAM)file.cFileName);
		}
	}while (FindNextFile(fs,&file));
FindClose(fs);

if (!B)
	{
	int c = SendDlgItemMessage(hFile,IDLISTTEMP,LB_GETCOUNT,0,0);
	for (int i=0;i<c;i++)
		{
		char ncurdir[1000];
		SendDlgItemMessage(hFile,IDLISTTEMP, LB_GETTEXT, i, (LPARAM)ncurdir);
		InsItem(ncurdir,0);
		}
	SendDlgItemMessage(hFile, IDLISTTEMP, LB_RESETCONTENT, 0, 0);
	}

FindFileExt(curdir, "*.mp3", &B);
FindFileExt(curdir, "*.ogg", &B);

if (H)
	{
	int c = SendDlgItemMessage(hFile, IDLISTTEMP, LB_GETCOUNT, 0, 0);
	for (int i=0;i<c;i++)
		{
		char ncurdir[1000];
		SendDlgItemMessage(hFile, IDLISTTEMP, LB_GETTEXT, i, (LPARAM)ncurdir);
		InsItem(ncurdir,2);
		}
	SendDlgItemMessage(hFile, IDLISTTEMP, LB_RESETCONTENT, 0, 0);
	}
}
//----------------------------------------------------------------------
void FindFileExt(char *curdir, char *ext, BOOL *B)
{
WIN32_FIND_DATA file; ZeroMemory(&file, sizeof(WIN32_FIND_DATA));
HANDLE fs=FindFirstFile(ext, &file);
if (fs != INVALID_HANDLE_VALUE)
do	{
	if (file.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY)
		{
		if (*B)
			{
			char ncurdir[1000]; sprintf(ncurdir,"%s%s", curdir, file.cFileName);
			SendDlgItemMessage(hFile,IDLISTTEMP,LB_ADDSTRING,0,(LPARAM)ncurdir);
			}
		else SendDlgItemMessage(hFile,IDLISTTEMP,LB_ADDSTRING,0,(LPARAM)file.cFileName);
		}
	}while (FindNextFile(fs,&file));
FindClose(fs);
}
//----------------------------------------------------------------------
void InsItem(char *name,int indx)
{
int c = SendDlgItemMessage(hFile,IDFILELIST,LVM_GETITEMCOUNT,0,0);

LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
lvItem.iItem=c;
lvItem.pszText = name;
lvItem.iImage = indx;

SendDlgItemMessage(hFile,IDFILELIST,LVM_INSERTITEM,0,(LPARAM)&lvItem);
}
//----------------------------------------------------------------------
void SetItemSel(char *name=NULL)
{
if (name)
	{
	LV_FINDINFO lvFind; ZeroMemory(&lvFind, sizeof(LV_FINDINFO));
	lvFind.flags = LVFI_STRING;
	lvFind.psz = name;
	int i = SendDlgItemMessage(hFile,IDFILELIST,LVM_FINDITEM,-1,(LPARAM)&lvFind);
	if (i>-1) SelectItemPos(i); else SelectItemPos(0);
	}
else SelectItemPos(0);
}
//----------------------------------------------------------------------
void DragFunc(WPARAM wParam)
{
int len=DragQueryFile((HANDLE)wParam,0,0,0);
if (len>0)
	{
	char *dir=(char *)malloc((len+1)*sizeof(char));
	if (dir)
		{
		DragQueryFile((HANDLE)wParam,0,dir,len+1);
		ParsCmd(dir);
		free((char*) dir);
		}
	}
}
//----------------------------------------------------------------------
void ParsCmd(char *Cmd)
{
if (Cmd && strlen(Cmd))
	{
	char *sCmd=strdup(Cmd);
	SBStrTrim(&sCmd,"\" ");

	char *ndir=GetLongPath(sCmd);
	if (ndir)
		{
		if (chdir(ndir)==0) SetDlgItemText(hFile, IDFOLDER, ndir);
		else
			{
			char *pos=strrchr(ndir,'\\');
			if (pos)
				{
				if (strlen(pos+1))
					{
					if (LastDir) free((char*) LastDir);
					LastDir=strdup(ndir);
					}
				pos[0]=0;
				SetDlgItemText(hFile, IDFOLDER, ndir);
				}
			}
		free((char*) ndir);
		}
	free((char*) sCmd);
	}
}
//----------------------------------------------------------------------
void ReadMediaFile(char *name)
{
if (name==NULL) return;
char *ext=strrchr(name,'.');

if (ext)
{
BYTE type=0;
if (strcmpi(ext+1,"mp3")==0) type=1;
if (strcmpi(ext+1,"ogg")==0) type=2;

ID3v1 TagV1;		ZeroMemory(&TagV1,sizeof(ID3v1));
ID3v2 TagV2;		ZeroMemory(&TagV2,sizeof(ID3v2));
MP3Info TagInf;	ZeroMemory(&TagInf,sizeof(MP3Info));

BYTE с = (SendDlgItemMessage(hTagInfo,IDTRANSLIT,CB_GETCURSEL,0,0));
BYTE r = (SendDlgItemMessage(hTagInfo,IDREGISTR,CB_GETCURSEL,0,0));

if (ReadFromFile2(&TagV2,name))
	{
	if(TagV2.Exists)
		{
		CheckDlgButton(hTagInfo,IDCHTAG2,BST_CHECKED);

		TextTransToEdit(hTagInfo, IDTRACK2, IDCHTRACK2, TagV2.Track, &с, &r);
		TextTransToEdit(hTagInfo, IDTITLE2, IDCHTITLE2, TagV2.Title, &с, &r);
		TextTransToEdit(hTagInfo, IDARTIST2, IDCHARTIST2, TagV2.Artist, &с, &r);
		TextTransToEdit(hTagInfo, IDALBUM2, IDCHALBUM2, TagV2.Album, &с, &r);
		TextTransToEdit(hTagInfo, IDYEAR2, IDCHYEAR2, TagV2.Year, &с, &r);
		TextTransToEdit(hTagInfo, IDGENRE2, IDCHGENRE2, TagV2.Genre, &с, &r);
		TextTransToEdit(hTagInfo, IDCOMM2, IDCHCOMM2, TagV2.Comment, &с, &r);
		TextTransToEdit(hTagInfo, IDCOMP2, IDCHCOMP2, TagV2.Composer, &с, &r);
		TextTransToEdit(hTagInfo, IDORIG2, IDCHORIG2, TagV2.OrigArtist, &с, &r);
		TextTransToEdit(hTagInfo, IDCOPY2, IDCHCOPY2, TagV2.Copyright, &с, &r);
		TextTransToEdit(hTagInfo, IDURL2, IDCHURL2, TagV2.Url, &с, &r);
		TextTransToEdit(hTagInfo, IDENCOD2, IDCHENCOD2, TagV2.Encoder, &с, &r);
		}
	else ClearInfo(2);
	Clear2(&TagV2);
	}
else ClearInfo(2);

if (ReadFromFile1(&TagV1,name))
	{
	CheckDlgButton(hTagInfo,IDCHTAG1,BST_CHECKED);

	if (IsDlgButtonChecked(hTagInfo,IDCHTRACK1)==BST_UNCHECKED) SetDlgItemInt(hTagInfo,IDTRACK1,TagV1.Track,FALSE);
	if (IsDlgButtonChecked(hTagInfo,IDCHYEAR1)==BST_UNCHECKED) SetDlgItemText(hTagInfo,IDYEAR1,TagV1.Year);

	if (IsDlgButtonChecked(hTagInfo,IDCHGENRE1)==BST_UNCHECKED) SendDlgItemMessage(hTagInfo,IDGENRE1,CB_SETCURSEL,
		SendDlgItemMessage(hTagInfo,IDGENRE1,CB_FINDSTRING,-1,(LPARAM)GanR[TagV1.Genre]),0);

	TextTransToEdit(hTagInfo, IDTITLE1, IDCHTITLE1, TagV1.Title, &с, &r);
	TextTransToEdit(hTagInfo, IDARTIST1, IDCHARTIST1, TagV1.Artist, &с, &r);
	TextTransToEdit(hTagInfo, IDALBUM1, IDCHALBUM1, TagV1.Album, &с, &r);
	TextTransToEdit(hTagInfo, IDCOMM1, IDCHCOMM1, TagV1.Comment, &с, &r);
	}
else ClearInfo(1);

ClearInfo(3);

if (type==1)
	{
	if (loadInfoMP3(&TagInf,name))
		{
		SetDlgItemInt(hTagInfo,IDSIZE,TagInf.fileSize,FALSE);

		SetDlgItemText(hTagInfo,IDVERSION,getVersionMP3(&TagInf));
		SetDlgItemText(hTagInfo,IDLENTH,getFormattedLengthMP3(&TagInf));

		char B[20];itoa(getBitrateMP3(&TagInf),B,10);
		if (TagInf.VBitRate) strcat(B," (Variable)");else strcat(B," (Constant)");
		SetDlgItemText(hTagInfo,IDBITRATE,B);

		SetDlgItemInt(hTagInfo,IDFREQ,getFrequencyMP3(&TagInf),FALSE);
		SetDlgItemText(hTagInfo,IDMOD,getModeMP3(&TagInf));
		SetDlgItemInt(hTagInfo,IDFRAMES,getNumberOfFramesMP3(&TagInf),FALSE);
		}
	}

else if (type==2)
	{
	FILE *ifile;
	if ((ifile=fopen(name, "rb"))!=NULL)
		{
		//размер
		fseek(ifile,0,SEEK_END);
		SetDlgItemInt(hTagInfo,IDSIZE,ftell(ifile),FALSE);

		fseek(ifile,0,SEEK_SET);

		//capture_pattern 0-3(4 b); stream_structure_version 4
		char capture_pattern[10];fread(capture_pattern,1,4,ifile);capture_pattern[4]=0;
		char stream_structure_version;fread(&stream_structure_version,1,1,ifile);
		sprintf(capture_pattern+4," 1.%d",stream_structure_version);
		SetDlgItemText(hTagInfo,IDVERSION,capture_pattern);

		fclose(ifile);
		}
	}

}
}
//----------------------------------------------------------------------
void ClearInfo(BYTE n)
{
switch (n)
{
case 0:
case 1:
	CheckDlgButton(hTagInfo,IDCHTAG1,BST_UNCHECKED);
	if (IsDlgButtonChecked(hTagInfo,IDCHGENRE1)==BST_UNCHECKED) SendDlgItemMessage(hTagInfo,IDGENRE1,CB_SETCURSEL,-1,0);
	for (int i=IDTRACK1; i<=IDCOMM1; i++)	if (IsDlgButtonChecked(hTagInfo,i+8)==BST_UNCHECKED)	SetDlgItemText(hTagInfo,i,"");
	if (n==1) break;

case 2:
	CheckDlgButton(hTagInfo,IDCHTAG2,BST_UNCHECKED);
	if (IsDlgButtonChecked(hTagInfo,IDCHGENRE2)==BST_UNCHECKED) SendDlgItemMessage(hTagInfo,IDGENRE2,CB_SETCURSEL,-1,0);
	for (int i=IDTRACK2; i<=IDENCOD2; i++)	if (IsDlgButtonChecked(hTagInfo,i+13)==BST_UNCHECKED)	SetDlgItemText(hTagInfo,i,"");
	if (n==1) break;break;

case 3:for (int i=IDSIZE; i<=IDFRAMES; i++) SetDlgItemText(hTagInfo,i,"");if (n==3) break;

case 4:
	for (int i=IDTRACK1; i<=IDCOMM1; i++)	if (IsDlgButtonChecked(hGrTag,i+8)==BST_UNCHECKED)	SetDlgItemText(hGrTag,i,"");
	break;

}
SendDlgItemMessage(hExec,IDPROGRBAR,PBM_SETPOS,0,0);
}
//----------------------------------------------------------------------
void Enable()
{
if (LastTab==0)
	{
	for (int i=IDTRACK1; i<=IDGENRE1; i++)	Check(hTagInfo, i+8, i);
	for (int i=IDTRACK2; i<=IDGENRE2; i++)	Check(hTagInfo, i+13, i);
	}
else
	for (int i=IDTRACK1; i<=IDCOMM1; i++)	Check(hGrTag, i+8, i);
}
//----------------------------------------------------------------------
void Check(HWND hM, int IDCH, int IDEN)
{
if ((IsDlgButtonChecked(hM,IDCH)==BST_UNCHECKED)&&(ColSel>1)) EnableWindow(GetDlgItem(hM,IDEN),FALSE);
else EnableWindow(GetDlgItem(hM,IDEN),TRUE);
}
//----------------------------------------------------------------------
void AddToolTip(HWND hWnd, UINT nIDTool, LPCTSTR szText)
{
TOOLINFO ti;
ZeroMemory(&ti,sizeof(TOOLINFO));
ti.cbSize = sizeof(TOOLINFO);
ti.hwnd   = hWnd;
ti.uFlags = TTF_SUBCLASS;
ti.uId    = nIDTool;
ti.hinst  = hInS;
ti.lpszText = szText;

RECT pr; GetClientRect(hWnd, &pr);
memcpy(&ti.rect, &pr, sizeof(RECT));

SendMessage(hTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}
//-------------------------------------------------------------
void SelectItemPos(int i)
{
LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));

lvItem.state = LVIS_SELECTED|LVIS_FOCUSED;
lvItem.stateMask = LVIS_SELECTED|LVIS_FOCUSED;

SendDlgItemMessage(hFile,IDFILELIST,LVM_SETITEMSTATE,i,(LPARAM)&lvItem);
SendDlgItemMessage(hFile,IDFILELIST,LVM_ENSUREVISIBLE,i,(LPARAM)TRUE);
}
//-------------------------------------------------------------
int GetItemSel(){return SendDlgItemMessage(hFile,IDFILELIST,LVM_GETNEXTITEM,-1,MAKELPARAM(LVNI_SELECTED,0));}
//-------------------------------------------------------------
void DeSelectItemPos(int i)
{
LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
lvItem.state = !LVIS_SELECTED;
lvItem.stateMask = LVIS_SELECTED;
SendDlgItemMessage(hFile,IDFILELIST,LVM_SETITEMSTATE,i,(LPARAM)&lvItem);
}
//-------------------------------------------------------------
char* TextTransForm(char *T, BYTE *c, BYTE *r)
{
if (T && strlen(T))
{
char *n=strdup(T);
n=TransLit(n,*c-1);

switch (*r)
	{
	case 1:strlwr(n);strRUS(n,FALSE);break;
	case 2:strupr(n);strRUS(n,TRUE);break;

	case 3: case 4:
		strlwr(n);strRUS(n,FALSE);
		char s[2];strncpy(s,n,1);s[1]=0;strupr(s);strRUS(s,TRUE);n[0]=s[0];
		if (*r==3) break;

		char bukv[]="qQwWeErRtTyYuUiIoOpPaAsSdDfFgGhHjJkKlLzZxXcCvVbBnNmMй…ц÷у”к е≈нЌг√шЎщўз«х’ъЏф‘ыџв¬ајпѕр–оќлЋдƒж∆эЁ€яч„с—мћи»т“ь№бЅюёЄ®";
		int ln = strlen(n)-1;
		for(int i=0;i<ln;i++)
		if ((strchr(bukv,n[i])==0) && (strchr(bukv,n[i+1])))
			{
			strncpy(s,n+i+1,1);s[1]=0;
			strupr(s);strRUS(s,TRUE);
			n[i+1]=s[0];
			}
		break;
	}
return n;
}
return NULL;
}
//-------------------------------------------------------------
void TextTransToEdit(HWND H,int ID,int IDCH,char *T,BYTE *c,BYTE *r)
{
if (IsDlgButtonChecked(H,IDCH)==BST_UNCHECKED)
{char *n=TextTransForm(T,c,r);SetDlgItemText(H,ID,n);if (n) free((char *) n);}
}
//-------------------------------------------------------------
void strRUS(char *n,BOOL UP)
{
char big[]="®…÷” ≈Ќ√Ўў«’Џ‘џ¬јѕ–ќЋƒ∆Ёя„—ћ»“№Ѕё";
char sml[]="Єйцукенгшщзхъфывапролджэ€чсмитьбю";

int U=strlen(big);

if (UP)	for (int j=0;j<U;j++){char *p = strchr(n,sml[j]);while (p) {p[0]=big[j];p = strchr(n,sml[j]);}}
else	for (int j=0;j<U;j++){char *p = strchr(n,big[j]);while (p) {p[0]=sml[j];p = strchr(n,big[j]);}}
}
//-------------------------------------------------------------
void Execute(struct PData *PD)
{
//PD->type==1 копирование 1 тега во 2
//PD->type==2 копирование 2 тега в 1
//PD->type==3 выполнение hTagInfo
//PD->type==4 создание списка переименовани€
//PD->type==5 переименование использую€ список переименовани€
//PD->type==6 генераци€ тегов
//PD->type==7 переименование из текста

//BYTE type,BOOL pb

int countprogress=0;
SendDlgItemMessage(hExec,IDPROGRBAR,PBM_SETPOS,0,0);

if ((PD->pb)&&(ColSel>0)) SendDlgItemMessage(hExec,IDPROGRBAR,PBM_SETRANGE32,0,ColSel);

int sc = SendDlgItemMessage(hFile,IDFILELIST,LVM_GETSELECTEDCOUNT,0,0);
if (sc==0) return;

int c = SendDlgItemMessage(hFile,IDFILELIST, LVM_GETITEMCOUNT,0,0);

if ((IsDlgButtonChecked(hGrRen,IDRANDOM)==BST_CHECKED)&&(PD->type==4))
	{
	rndI = (int*) malloc(sc*sizeof(int)); ZeroMemory(rndI,sc*sizeof(int));
	srand((unsigned)time(NULL));
	rndI[0]=(rand()%sc);
	for(int i=1;i<sc;i++)
		{
		rndI[i]=(rand()%sc);
		for (int j=0;j<i;j++) if (rndI[i]==rndI[j]) {rndI[i]=(rand()%sc); j=-1;}
		}
	}

LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
lvItem.stateMask=LVIS_SELECTED;
lvItem.cchTextMax = 1000;

for (int i=0;i<c;i++)
	{
	char path[1000];

	lvItem.iItem = i;

	if (IsDlgButtonChecked(hExec,IDALL)==BST_CHECKED) lvItem.pszText = path;
	else {getcwd(path,1000);if (path[strlen(path)-1]!='\\') strcat(path,"\\");lvItem.pszText = path+strlen(path);}

	if (SendDlgItemMessage(hFile,IDFILELIST,LVM_GETITEM,0,(LPARAM)&lvItem) && lvItem.iImage==2 && lvItem.state==LVIS_SELECTED)
		{
		switch (PD->type)
			{
			case 1:CopyTag12(path);break;
			case 2:CopyTag21(path);break;
			case 3:ExecuteTagInfo(path);break;
			case 4:CreatNameListItem(path, countprogress);break;

			case 5:
				{
				char *sdir=GetTextSubItem(hGrRen, IDNAMELIST, countprogress, 0, 1000);
				char *sext=GetTextSubItem(hGrRen, IDNAMELIST, countprogress, 1, 5);
				char *sold=GetTextSubItem(hGrRen, IDNAMELIST, countprogress, 2, 1000);
				char *snew=GetTextSubItem(hGrRen, IDNAMELIST, countprogress, 3, 1000);

				char *nameold=(char *)malloc((strlen(sdir) + strlen(sold) + strlen(sext) + 1)*sizeof(char));
				char *namenew=(char *)malloc((strlen(sdir) + strlen(snew) + strlen(sext) + 1)*sizeof(char));
				sprintf(nameold, "%s%s%s", sdir, sold, sext);
				sprintf(namenew, "%s%s%s", sdir, snew, sext);

				rename(nameold, namenew);

				free((char *)sdir);
				free((char *)sext);
				free((char *)sold);
				free((char *)snew);
				free((char *)nameold);
				free((char *)namenew);
				}
				break;

			case 6:UpdateTagsList(path,TRUE);break;

			case 7:
				{
				char *n = GetItemChar(hNameTxt, IDMEMO);
				if (n)
					{
					int C=GetCountMemo(n);
					if (countprogress<C)
						{
						char *str= GetStrMemo(n,countprogress+1);
						if (str)
							{
							if (strlen(str))
								{//удаление запрещенных символов
								char wro[]="\\/:*\"<>|\n\r\?";	int lw = strlen(wro);
								for (int i=0;i<lw;i++) {char Buf[]="0";Buf[0]=wro[i];str = SearchAndPast(str,Buf,"");}
								}
							if (strlen(str))
								{
								char *dir=strdup(path);
								char *p = strrchr(dir,'\\')+1; strcpy(p,"\0");
								char *new=(char*) malloc((strlen(dir)+strlen(str)+5)*sizeof(char));
								strcpy(new,dir);free((char*) dir);strcat(new,str);strcat(new,".mp3");
								rename(path,new);
								}
							free((char*) str);
							}
						}
					free((char*) n);
					}
				}
				break;
			}
		countprogress++;
		if (PD->pb) SendDlgItemMessage(hExec,IDPROGRBAR,PBM_SETPOS,countprogress,0);
		}
	}
SendDlgItemMessage(hExec,IDPROGRBAR,PBM_SETPOS,0,0);

switch (PD->type)
	{
	case 1: case 2: case 3:ReadMediaFile(LastDir);break;
	case 5: case 7: ListCreate();UpdateNameList();break;
	case 6: UpdateTagsList(LastDir,FALSE);
	}

if (rndI) free((int*) rndI); rndI=NULL;

if (PD) free((struct PData*) PD);
EnableWindow(hMain,TRUE);
}
//-------------------------------------------------------------
char *GetTextSubItem(HWND hwnd, int ID, int ipos, int i, int imax)
{
LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
lvItem.cchTextMax = imax;
char *sRet=(char *)malloc(imax*sizeof(char));
lvItem.pszText = sRet;
lvItem.iSubItem = i;
sRet[SendDlgItemMessage(hwnd, ID, LVM_GETITEMTEXT, ipos, (LPARAM)&lvItem)]=0;
return sRet;
}
//-------------------------------------------------------------
void CopyTag12(char *path)
{
ID3v1 TagV1;		ZeroMemory(&TagV1,sizeof(ID3v1));
ID3v2 TagV2;		ZeroMemory(&TagV2,sizeof(ID3v2));

if (ReadFromFile1(&TagV1,path))
	{
	ReadFromFile2(&TagV2,path);

	SetTagItem(&TagV2.Title,TagV1.Title);
	SetTagItem(&TagV2.Artist,TagV1.Artist);
	SetTagItem(&TagV2.Album,TagV1.Album);
	char tr[10];itoa(TagV1.Track,tr,10);SetTagItem(&TagV2.Track,tr);
	SetTagItem(&TagV2.Year,TagV1.Year);
	SetTagItem(&TagV2.Genre,GanR[TagV1.Genre]);
	SetTagItem(&TagV2.Comment,TagV1.Comment);

	SaveToFile2(&TagV2,path);
	Clear2(&TagV2);
	}
}
//-------------------------------------------------------------
void CopyTag21(char *path)
{
ID3v1 TagV1;		ZeroMemory(&TagV1,sizeof(ID3v1));
ID3v2 TagV2;		ZeroMemory(&TagV2,sizeof(ID3v2));

if (ReadFromFile2(&TagV2,path))
	{
	if (TagV2.Exists)
		{
		ReadFromFile1(&TagV1,path);
		Set(TagV1.Title,TagV2.Title,30);
		Set(TagV1.Artist,TagV2.Artist,30);
		Set(TagV1.Album,TagV2.Album,30);
		Set(TagV1.Year,TagV2.Year,4);
		Set(TagV1.Comment,TagV2.Comment,28);
		if (TagV2.Track) TagV1.Track=atoi(TagV2.Track);
		if (TagV2.Genre) TagV1.Genre=GetNumGanr(TagV2.Genre);
		SaveToFile1(&TagV1,path);
		}
	Clear2(&TagV2);
	}
}
//-------------------------------------------------------------
void ExecuteTagInfo(char *path)
{
ID3v1 TagV1;		ZeroMemory(&TagV1,sizeof(ID3v1));
ID3v2 TagV2;		ZeroMemory(&TagV2,sizeof(ID3v2));

BOOL WriteY;
BYTE c = (SendDlgItemMessage(hTagInfo,IDTRANSLIT,CB_GETCURSEL,0,0));
BYTE r = (SendDlgItemMessage(hTagInfo,IDREGISTR,CB_GETCURSEL,0,0));

ReadFromFile2(&TagV2,path);
if (IsDlgButtonChecked(hTagInfo,IDCHTAG2)==BST_CHECKED)
	{
	WriteY=0;
	char *Temp=NULL;

	if (IfCheckChar(hTagInfo, IDCHTRACK2, IDTRACK2, &Temp, TagV2.Track, &c, &r))		{SetTagItem(&TagV2.Track,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHTITLE2, IDTITLE2, &Temp, TagV2.Title, &c, &r))		{SetTagItem(&TagV2.Title,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHARTIST2, IDARTIST2, &Temp, TagV2.Artist, &c, &r))		{SetTagItem(&TagV2.Artist,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHALBUM2, IDALBUM2, &Temp, TagV2.Album, &c, &r))		{SetTagItem(&TagV2.Album,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHYEAR2, IDYEAR2, &Temp, TagV2.Year, &c, &r))			{SetTagItem(&TagV2.Year,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHGENRE2, IDGENRE2, &Temp, TagV2.Genre, &c, &r))		{SetTagItem(&TagV2.Genre,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHCOMM2, IDCOMM2, &Temp, TagV2.Comment, &c, &r))		{SetTagItem(&TagV2.Comment,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHCOMP2, IDCOMP2, &Temp, TagV2.Composer, &c, &r))		{SetTagItem(&TagV2.Composer,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHORIG2, IDORIG2, &Temp, TagV2.OrigArtist, &c, &r))		{SetTagItem(&TagV2.OrigArtist,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHCOPY2, IDCOPY2, &Temp, TagV2.Copyright, &c, &r))		{SetTagItem(&TagV2.Copyright,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHURL2, IDURL2, &Temp, TagV2.Url, &c, &r))				{SetTagItem(&TagV2.Url,Temp); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHENCOD2, IDENCOD2, &Temp, TagV2.Encoder, &c, &r))		{SetTagItem(&TagV2.Encoder,Temp); WriteY=TRUE;}

	if (Temp) free((char *) Temp);
	if (WriteY) SaveToFile2(&TagV2,path);
	}
else if (TagV2.Exists) RemoveFromFile2(&TagV2,path);
Clear2(&TagV2);

ReadFromFile1(&TagV1,path);
if (IsDlgButtonChecked(hTagInfo,IDCHTAG1)==BST_CHECKED)
	{
	WriteY=0;
	char *Temp=NULL;

	char tr[10];itoa(TagV1.Track,tr,10);

	if (IfCheckChar(hTagInfo, IDCHTRACK1, IDTRACK1, &Temp, tr, &c, &r))					{if (Temp) TagV1.Track=atoi(Temp); else TagV1.Track=0;  WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHTITLE1, IDTITLE1, &Temp, TagV1.Title, &c, &r))		{Set(TagV1.Title,Temp,30); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHARTIST1, IDARTIST1, &Temp, TagV1.Artist, &c, &r))		{Set(TagV1.Artist,Temp,30); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHALBUM1, IDALBUM1, &Temp, TagV1.Album, &c, &r))		{Set(TagV1.Album,Temp,30); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHYEAR1, IDYEAR1, &Temp, TagV1.Year, &c, &r))			{Set(TagV1.Year,Temp,4); WriteY=TRUE;}
	if (IfCheckChar(hTagInfo, IDCHCOMM1, IDCOMM1, &Temp, TagV1.Comment, &c, &r))		{Set(TagV1.Comment,Temp,28); WriteY=TRUE;}

	c=0;r=0;
	if (IfCheckChar(hTagInfo, IDCHGENRE1, IDGENRE1, &Temp, GanR[TagV1.Genre], &c, &r)) {TagV1.Genre=GetNumGanr(Temp); WriteY=TRUE;}

	if (Temp) free((char *) Temp);
	if (WriteY) SaveToFile1(&TagV1, path);
	}
else if (TagV1.Exists) RemoveFromFile1(&TagV1,path);
}
//-------------------------------------------------------------
BOOL IfCheckChar(HWND hM, int IDCH, int IDED, char **EdText, char *Default, BYTE *c, BYTE *r)
{
if (IsDlgButtonChecked(hM,IDCH)==BST_CHECKED || ColSel==1)
	{
	*EdText=GetItemChar(hM,IDED);
	return TRUE;
	}

*EdText=TextTransForm(Default, c, r);
if (*c>0 || *r>0) return TRUE;

if (*EdText) free((char *) *EdText); *EdText=NULL;

return FALSE;
}
//-------------------------------------------------------------
void UpdateNameList()
{
SendDlgItemMessage(hGrRen,IDNAMELIST,LVM_DELETEALLITEMS,0,0);
struct PData *PD=(struct PData*) malloc(sizeof(struct PData));PD->type=4;PD->pb=FALSE;
Execute(PD);
}
//-------------------------------------------------------------
void CreatNameListItem(char *path,int pos)
{
LV_ITEM lvItem; ZeroMemory(&lvItem, sizeof(LV_ITEM));
lvItem.mask = LVIF_TEXT;
lvItem.iItem = pos;

char *ext=strdup(strrchr(path,'.'));
char *text=strdup(path); PathRemoveExtension(text); PathStripPath(text);
char *conv=ConvertName(path, pos);

PathRemoveFileSpec(path); PathAddBackslash(path);

lvItem.pszText = path;
int i = SendDlgItemMessage(hGrRen, IDNAMELIST, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

lvItem.iSubItem = 1; lvItem.pszText = ext;
SendDlgItemMessage(hGrRen, IDNAMELIST, LVM_SETITEMTEXT, i, (LPARAM)&lvItem);

lvItem.iSubItem = 2; lvItem.pszText = text;
SendDlgItemMessage(hGrRen, IDNAMELIST, LVM_SETITEMTEXT, i, (LPARAM)&lvItem);

lvItem.iSubItem = 3; lvItem.pszText = conv;
SendDlgItemMessage(hGrRen, IDNAMELIST, LVM_SETITEMTEXT, i, (LPARAM)&lvItem);

free((char *) ext);
free((char *) text);
free((char *) conv);
}
//-------------------------------------------------------------
char *ConvertName(char *path, int pos)
{
char *sfile=strdup(path); PathRemoveExtension(sfile); PathStripPath(sfile);
char *FileName=GetItemChar(hGrRen, IDMASK);
if (FileName==NULL) FileName=strdup(sfile);
else
	{
	char *Base[8];		ZeroMemory(&Base,sizeof(Base));

	ID3v1 TagV1;		ZeroMemory(&TagV1,sizeof(ID3v1));
	ID3v2 TagV2;		ZeroMemory(&TagV2,sizeof(ID3v2));

	BYTE tag = SendDlgItemMessage(hGrRen, IDTAG, CB_GETCURSEL, 0, 0);
	BOOL Convert = FALSE;

	switch (tag)
		{
		case 2:
		case 1:
			if (ReadFromFile2(&TagV2,path))
				{
				if (TagV2.Exists)
					{
					if ((TagV2.Track)&&(strlen(TagV2.Track)))		Base[0]=strdup(TagV2.Track);
					if ((TagV2.Title)&&(strlen(TagV2.Title)))		Base[1]=strdup(TagV2.Title);
					if ((TagV2.Artist)&&(strlen(TagV2.Artist)))		Base[2]=strdup(TagV2.Artist);
					if ((TagV2.Album)&&(strlen(TagV2.Album)))		Base[3]=strdup(TagV2.Album);
					if ((TagV2.Year)&&(strlen(TagV2.Year)))			Base[4]=strdup(TagV2.Year);
					if ((TagV2.Comment)&&(strlen(TagV2.Comment)))	Base[5]=strdup(TagV2.Comment);
					}
				Clear2(&TagV2);
				}
			if (tag==1) break;

		case 0:
			if (ReadFromFile1(&TagV1,path))
				{
				if ((Base[0]==NULL)&&(TagV1.Track>0))			{Base[0]=strdup("00");sprintf(Base[0],"%02d",TagV1.Track);}
				if ((Base[1]==NULL)&&(strlen(TagV1.Title)))		Base[1]=strdup(TagV1.Title);
				if ((Base[2]==NULL)&&(strlen(TagV1.Artist)))	Base[2]=strdup(TagV1.Artist);
				if ((Base[3]==NULL)&&(strlen(TagV1.Album)))		Base[3]=strdup(TagV1.Album);
				if ((Base[4]==NULL)&&(strlen(TagV1.Year)))		Base[4]=strdup(TagV1.Year);
				if ((Base[5]==NULL)&&(strlen(TagV1.Comment)))	Base[5]=strdup(TagV1.Comment);
				}
			break;
		}

	Base[6]=sfile;

	//тримминг
	if (IsDlgButtonChecked(hGrRen,IDTRIM)==BST_CHECKED)
	for (int i=0;i<7;i++) if (Base[i]) SBStrTrim(&Base[i], " ");

	//создание счетчика
	char *sCol=GetItemChar(hGrRen, IDCOLCOUNT);
	if (sCol)
		{
		int iCol=strlen(sCol);
		char *format = (char*) malloc((iCol+3)*sizeof(char));
		sprintf(format, "%%0%sd", sCol);

		int Start = GetDlgItemInt(hGrRen, IDSTARTCOUNT, NULL, FALSE);
		int Step = GetDlgItemInt(hGrRen, IDSTEPCOUNT, NULL, FALSE);

		int npos=pos; if (IsDlgButtonChecked(hGrRen,IDRANDOM)==BST_CHECKED && rndI) npos=rndI[pos];
		npos = npos*Step+Start;
		char s[20]; itoa(npos, s, 10);
		iCol=max(iCol, strlen(s));

		Base[7]=(char*) malloc((iCol+1)*sizeof(char));
		sprintf(Base[7], format, npos);

		free((char *) format);
		free((char *) sCol);
		}

	//замена Base строк
	char cyf[]="01234567";
	for (int i=0;i<8;i++)
		{
		char Buf[]="%0"; Buf[1]=cyf[i];
		if (Base[i])
			{
			char *FileNameOld = strdup(FileName);
			FileName = SearchAndPast(FileName, Buf, Base[i]);
			if (strcmp(FileNameOld,FileName)!=0) Convert=TRUE;
			free((char *) FileNameOld);
			}
		}

	if (!Convert) {if (FileName) free((char *) FileName); FileName=strdup(Base[6]);}
	for (int i=0; i<8; i++) if (Base[i]) free((char *) Base[i]);
	}

//удаление запрещенных символов
if (strlen(FileName))
	{
	char wro[]="\\/:*\"<>|\n\r\?";	int lw = strlen(wro);
	for (int i=0;i<lw;i++) {char Buf[]="0"; Buf[0]=wro[i]; FileName = SearchAndPast(FileName,Buf,"");}
	}

//поиск и замена
if (strlen(FileName))
	{
	char *find = GetItemChar(hGrRen, IDFIND);
	char *past = GetItemChar(hGrRen, IDPAST);
	FileName = SearchAndPast(FileName, find, past);
	if (find) free((char *) find);
	if (past) free((char *) past);
	}

//регистр и транслит
BYTE c = (SendDlgItemMessage(hGrRen, IDTRANSLIT, CB_GETCURSEL, 0, 0));
BYTE r = (SendDlgItemMessage(hGrRen, IDREGISTR, CB_GETCURSEL, 0, 0));
char *n = TextTransForm(FileName, &c, &r);
if (n)
	{
	free((char *) FileName);
	FileName=n;
	}

return FileName;
}
//---------------------------------------------------------------------------
void UpdateTagsList(char *name, BOOL save)
{
char *Base[6]={NULL,NULL,NULL,NULL,NULL,NULL};
char *User[6]={NULL,NULL,NULL,NULL,NULL,NULL};

ID3v1 TagV1;	ZeroMemory(&TagV1,sizeof(ID3v1));
ID3v2 TagV2;	ZeroMemory(&TagV2,sizeof(ID3v2));

ConvertNameToTag(name, Base);

BYTE m = (SendDlgItemMessage(hGrTag, IDMETOD, CB_GETCURSEL, 0, 0));
BYTE tag = SendDlgItemMessage(hGrTag, IDTAG, CB_GETCURSEL, 0 ,0);

switch (m)
	{
	case 0: case 1: case 2:
		{
		switch (tag)
			{
			case 2:
			case 1: if (ReadFromFile2(&TagV2, name))
					{
					if (TagV2.Exists)
						{
						if (TagV2.Track && strlen(TagV2.Track))			User[0]=strdup(TagV2.Track);
						if (TagV2.Title && strlen(TagV2.Title))			User[1]=strdup(TagV2.Title);
						if (TagV2.Artist && strlen(TagV2.Artist))		User[2]=strdup(TagV2.Artist);
						if (TagV2.Album && strlen(TagV2.Album))			User[3]=strdup(TagV2.Album);
						if (TagV2.Year && strlen(TagV2.Year))			User[4]=strdup(TagV2.Year);
						if (TagV2.Comment && strlen(TagV2.Comment))		User[5]=strdup(TagV2.Comment);
						}
					}
				if (tag==1) break;

			case 0: if (ReadFromFile1(&TagV1, name))
						{
						if (User[0]==NULL && TagV1.Track>0)				{char s[20]; itoa(TagV1.Track, s, 10); User[0]=strdup(s);}
						if (User[1]==NULL && strlen(TagV1.Title))		User[1]=strdup(TagV1.Title);
						if (User[2]==NULL && strlen(TagV1.Artist))		User[2]=strdup(TagV1.Artist);
						if (User[3]==NULL && strlen(TagV1.Album))		User[3]=strdup(TagV1.Album);
						if (User[4]==NULL && strlen(TagV1.Year))		User[4]=strdup(TagV1.Year);
						if (User[5]==NULL && strlen(TagV1.Comment))		User[5]=strdup(TagV1.Comment);
						}
			}

		for (int i=0; i<6; i++)
			switch (m)
				{
				case 0: if (User[i] && strlen(User[i])) {}
						else
							{
							if (User[i]) free((char *) User[i]); User[i]=NULL;
							if (Base[i]) User[i]=strdup(Base[i]);
							}
						break;

				case 1: if (Base[i])
							{
							if (User[i]) free((char *)User[i]); User[i]=NULL;
							if (Base[i]) User[i]=strdup(Base[i]);
							}
						break;

				case 2: if (Base[i])
							{
							if (User[i] && strlen(User[i])) {}
							else
								{
								if (User[i]) free((char *)User[i]); User[i]=NULL;
								if (Base[i]) User[i]=strdup(Base[i]);
								}
							}
				}
		}
		break;

	case 3:
		{
			for (int i=0; i<6; i++) if (Base[i]) User[i]=strdup(Base[i]);
			switch (tag)
			{
				case 1: case 2: ReadFromFile2(&TagV2, name);
			}
		}
		break;
	}

for (int i=0; i<6; i++)
if (IsDlgButtonChecked(hGrTag, IDCHTRACK1+i)==BST_UNCHECKED)
	{
	if (!save) SetDlgItemText(hGrTag,IDTRACK1+i,User[i]);
	}
else if (save) User[i]=GetItemChar(hGrTag, IDTRACK1+i);

if (save)
switch (tag)
	{
	case 2:
	case 1:
		{
		SetTagItem(&TagV2.Track, User[0]);
		SetTagItem(&TagV2.Title, User[1]);
		SetTagItem(&TagV2.Artist, User[2]);
		SetTagItem(&TagV2.Album, User[3]);
		SetTagItem(&TagV2.Year, User[4]);
		SetTagItem(&TagV2.Comment, User[5]);

		SaveToFile2(&TagV2, name);
		}
		if (tag==1) break;

	case 0:
		if (User[0]) TagV1.Track=atoi(User[0]);
		Set(TagV1.Title, User[1], 30);
		Set(TagV1.Artist, User[2], 30);
		Set(TagV1.Album, User[3], 30);
		Set(TagV1.Year, User[4], 4);
		Set(TagV1.Comment, User[5], 28);

		SaveToFile1(&TagV1, name);
	}

Clear2(&TagV2);

for (int i=0; i<6; i++) if (Base[i]) free((char *) Base[i]);
for (int i=0; i<6; i++) if (User[i]) free((char *) User[i]);
}
//---------------------------------------------------------------------------
void ConvertNameToTag(char *Name, char *Base[6])
{
if (Name==NULL) return;

char *FoolName=strdup(Name); PathRemoveExtension(FoolName);
if (IsDlgButtonChecked(hGrTag, IDUSEPATH)==BST_UNCHECKED) PathStripPath(FoolName);

char *Form=GetItemChar(hGrTag, IDMASK);
if (Form)
	{
	//заполнение полей Base
	char *sFoolName=FoolName;
	char *sForm=Form;

	char cyfr[]="012345";

	int L=strlen(sForm);
	int Ln=strlen(sFoolName);
	int i=0;
	while (i<L-1 && i<Ln)
		{
		if (sForm[i]=='%')
		if (strchr(cyfr, sForm[i+1]))
			{
			//---n - номер Base
			char sn[2]; sn[0]=sForm[i+1]; sn[1]=0; int n = atoi(sn);

			//---создание Base[n]
			if (Base[n]) free((char *) Base[n]); Base[n] = strdup(sFoolName+i);

			i++;
			if (i+1<L) //---если символ в пределах строки
				{
				i++;
				//---поиск символа закрывающего Base[n]
				char *p=strchr(Base[n], sForm[i]);
				if (p)
					{
					int Lb=strlen(p);
					if (Lb>0) //---если есть длина оставшегос€ куска
						{
						strcpy(sFoolName, p+1); //---копируем оставшийс€ кусок в им€
						p[0]=0; //---закрываем строку

						i++;
						if (i>=L) break; //---если строка формата кончилась - завершаем

						sForm += i;
						L=strlen(sForm);
						Ln=strlen(sFoolName);

						i=-1;
						}
					}
				else break; //---если закрывающий символ не найден, дальнейший посик не нужен
				}
			}
		i++;
		}
	}

char *find = GetItemChar(hGrTag, IDFIND);
char *past = GetItemChar(hGrTag, IDPAST);

BYTE c = (SendDlgItemMessage(hGrTag, IDTRANSLIT, CB_GETCURSEL, 0, 0));
BYTE r = (SendDlgItemMessage(hGrTag, IDREGISTR, CB_GETCURSEL, 0, 0));

for (int i=0;i<6;i++)
	{
	if (Base[i] && strlen(Base[i]))
		{
		//поиск и замена
		Base[i] = SearchAndPast(Base[i], find, past);

		//регистр и транслит
		char *n = TextTransForm(Base[i], &c, &r);
		if (n) {free((char *) Base[i]); Base[i]=n;}
		}
	}

if (find) free((char *) find);
if (past) free((char *) past);
if (Form) free((char *) Form);
if (FoolName) free((char *) FoolName);
}
//---------------------------------------------------------------------------
void TrimStart()
{
char *n = GetItemChar(hNameTxt, IDMEMO);
if (n)
	{
	int C=GetCountMemo(n);
	int s = GetDlgItemInt(hNameTxt, IDSRT, NULL, FALSE);
	char *r = (char*) malloc((strlen(n)+1)*sizeof(char)); strcpy(r,"\0");

	for(int i=0;i<C;i++)
		{
		char *str= GetStrMemo(n,i+1);
		if (strlen(str)>=s) strcat(r,str+s);
		free((char*) str);
		if (i<(C-1)) strcat(r,"\r\n");
		}

	SetDlgItemText(hNameTxt, IDMEMO, r);
	free((char*) n);
	free((char*) r);
	}
}
//---------------------------------------------------------------------------
void TrimEnd()
{
char *n = GetItemChar(hNameTxt, IDMEMO);
if (n)
	{
	int C=GetCountMemo(n);
	int e = GetDlgItemInt(hNameTxt,IDEND,NULL,FALSE);
	char *r = (char*) malloc((strlen(n)+1)*sizeof(char));strcpy(r,"\0");

	for(int i=0;i<C;i++)
		{
		char *str= GetStrMemo(n,i+1);
		int l=strlen(str);if (l>=e) {str[l-e]=0; strcat(r,str);}
		free((char*) str);
		if (i<(C-1)) strcat(r,"\r\n");
		}

	SetDlgItemText(hNameTxt,IDMEMO,r);
	free((char*) n);
	free((char*) r);
	}
}
//---------------------------------------------------------------------------
void CountExec()
{
char *n = GetItemChar(hNameTxt, IDMEMO);
if (n)
	{
	int C=GetCountMemo(n);
	char *r = NULL;

	int Col = GetDlgItemInt(hNameTxt, IDCOLCOUNT, NULL, FALSE);
	int Start = GetDlgItemInt(hNameTxt, IDSTARTCOUNT, NULL, FALSE);
	int Step = GetDlgItemInt(hNameTxt, IDSTEPCOUNT, NULL, FALSE);

	char *after = GetItemChar(hNameTxt,IDTXTAFTER);	if (after==NULL) after=strdup("");

	char *format=NULL;
	int iCol=0;
	char *sCol=GetItemChar(hNameTxt, IDCOLCOUNT);
	if (sCol)
		{
		iCol=strlen(sCol);
		format = (char*) malloc((iCol+3)*sizeof(char));
		sprintf(format, "%%0%sd", sCol);
		free((char *) sCol);
		}

	int pos=0;
	if (format)
	for(int i=0; i<C; i++)
		{
		char *str= GetStrMemo(n, i+1);
		int npos=pos*Step+Start;
		char s[20]; itoa(npos, s, 10); iCol=max(iCol, strlen(s));
		char *count=(char*) malloc((iCol+1)*sizeof(char));
		sprintf(count, format, npos);

		char *conv = (char*) malloc((strlen(count) + strlen(after) + strlen(str) + 3)*sizeof(char));
		sprintf(conv, "%s%s%s%s", count, after, str, ((i<C-1)?"\r\n":"")); free((char*) str);

		if (r==NULL) r=conv;
		else
			{
			char *buf = (char*)malloc((strlen(r) + strlen(conv) + 1)*sizeof(char));
			sprintf(buf, "%s%s", r, conv);
			free((char*) conv);
			free((char*) r);
			r=buf;
			}

		if (count) free((char *) count);
		pos++;
		}

	SetDlgItemText(hNameTxt, IDMEMO, r);
	if (r) free((char*) r);

	if (format) free((char *) format);
	if (after) free((char *) after);
	free((char*) n);
	}
}
//---------------------------------------------------------------------------
int GetCountMemo(char *n)
{
int C=0;
char *f=n;
char *p=strstr(f,"\r\n");
while (p) {C++;f=p+2;p=strstr(f,"\r\n");}
if (f) C++;
return C;
}
//---------------------------------------------------------------------------
char* GetStrMemo(char *n,int I)
{
char *str=NULL;
int Cf=0;
char *f=n;
char *p=strstr(f,"\r\n");
while (p)
	{
	Cf++;
	if (Cf==I)
		{
		int r=(p-f);
		str = (char*) malloc((r+1)*sizeof(char));
		strncpy(str,f,r);str[r]=0;
		break;
		}
	f=p+2;
	p=strstr(f,"\r\n");
	}
if ((str==NULL)&&(f)&&((Cf+1)==I)) str=strdup(f);
return str;
}
//---------------------------------------------------------------------------
void CreateLabel()
{
char *path=GetLabelPath();
char file[1000]; GetModuleFileName(NULL, file, 1000);
CreateLink((LPCSTR)file,(LPCSTR)path,"ѕрограмма дл€ создани€ и переименовани€ “эгов в файлах *.mp3\nпереименовани€ файлов *.mp3\nSerbisSoft");
free((char*)path);
}
//----------------------------------------------------------------------
char *GetLabelPath()
{
HKEY hk;char T[1000];int L=1000;
RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",0,KEY_READ,&hk);
if (RegQueryValueEx(hk,"Desktop",NULL,NULL,T,&L)==ERROR_SUCCESS)
RegCloseKey(hk);strcat(T,"\\SBTag.lnk");
return strdup(T);
}
//----------------------------------------------------------------------
void CreateLink(LPCSTR lpszPathObj,LPCSTR lpszPathLink, LPCSTR lpszDesc)
{
IShellLink *psl;
CoInitialize(NULL);
HRESULT hres = CoCreateInstance(&CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,&IID_IShellLink,(void **)&psl);

if(SUCCEEDED(hres))
	{
	IPersistFile *ppf;
	psl->SetPath(lpszPathObj);psl->SetDescription(lpszDesc);
	hres = psl->QueryInterface(&IID_IPersistFile, (void**)&ppf);
	if(SUCCEEDED(hres))
		{
		wchar_t wsz[1000];
		MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz,1000);
		ppf->Save(wsz, TRUE);
		ppf->Release();
		}
	psl->Release();
	CoUninitialize();
	}
}
//---------------------------------------------------------------------------
void CreateLanguageMenu()
{
char *sret=GetKey("Main", "ini", "rus.ini");
if (sret)
	{
	char *cur = CreatePathFile("Language");

	if (chdir(cur)==0)
		{
		WIN32_FIND_DATA file; ZeroMemory(&file, sizeof(WIN32_FIND_DATA));
		HANDLE fs=FindFirstFile("*.ini",&file);
		if (fs != INVALID_HANDLE_VALUE)
		do	{
			SendDlgItemMessage(hIntegr, IDLANG, CB_ADDSTRING, 0, (LPARAM)file.cFileName);
				}while (FindNextFile(fs,&file));
		FindClose(fs);
		}

	free((char*)cur);

	int i=SendDlgItemMessage(hIntegr, IDLANG, CB_FINDSTRING, -1, (LPARAM)sret); if (i<0) i=0;
	SendDlgItemMessage(hIntegr, IDLANG, CB_SETCURSEL, i, 0);

	ini = CreatePathFile("Language", sret);
	free((char*)sret);
	}
}
//---------------------------------------------------------------------------
void Language()
{
char R[80];
DWORD Size = 80;

char K[10];

//hMain---------------------------
for (int i=IDTRANSLITTEXT; i<=IDREGISTRTEXT; i++)
	{
	itoa(i,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
	SetDlgItemText(hTagInfo,i,R);
	SetDlgItemText(hGrRen,i,R);
	SetDlgItemText(hGrTag,i,R);
	}
for (int i=IDFINDTEXT; i<=IDPASTTEXT; i++)
	{
	itoa(i,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
	SetDlgItemText(hGrRen,i,R);
	SetDlgItemText(hGrTag,i,R);
	SetDlgItemText(hNameTxt,i,R);
	}

itoa(0,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);

SendDlgItemMessage(hTagInfo,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)R);
SendDlgItemMessage(hTagInfo,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);

SendDlgItemMessage(hGrRen,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)R);
SendDlgItemMessage(hGrRen,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);

SendDlgItemMessage(hGrTag,IDTRANSLIT,CB_ADDSTRING,0,(LPARAM)R);
SendDlgItemMessage(hGrTag,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);

for (int i=10; i<=13;i++)
	{
	itoa(i,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
	SendDlgItemMessage(hTagInfo,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);
	SendDlgItemMessage(hGrRen,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);
	SendDlgItemMessage(hGrTag,IDREGISTR,CB_ADDSTRING,0,(LPARAM)R);
	}

for (int i=IDSTARTTEXT; i<=IDCOLTEXT; i++)
	{
	itoa(i,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
	SetDlgItemText(hGrRen,i,R);
	SetDlgItemText(hNameTxt,i,R);
	}
//hMain---------------------------

//hExec---------------------------
TC_ITEM tcI;tcI.mask = TCIF_TEXT;
for (int i=0; i<5; i++)
	{	itoa(i,K,10);if (!GetPrivateProfileString("EXEC",K,NULL,R,Size,ini)) strcpy(R,K);
		tcI.pszText = R;SendDlgItemMessage(hExec,IDTABCONTROL,TCM_INSERTITEM,i,(LPARAM)&tcI);}

for (int i=IDSELALL; i<=IDREFRESH; i++)
	{itoa(i,K,10);if (!GetPrivateProfileString("EXEC",K,NULL,R,Size,ini)) strcpy(R,K); SetDlgItemText(hExec,i,R);}

SetDlgItemText(hNameTxt,IDFINDPAST,R);
SetDlgItemText(hNameTxt,IDCOUNT,R);
//hExec---------------------------

//hGrRen--------------------------
for (int i=IDMASKTEXT; i<=IDCOUNTTEXT; i++)
	{itoa(i,K,10);if (!GetPrivateProfileString("GRREN",K,NULL,R,Size,ini)) strcpy(R,K);SetDlgItemText(hGrRen,i,R);}

itoa(IDMNAME,K,10);strcpy(R,"%6 "); if (!GetPrivateProfileString("GRREN",K,NULL,R+3,Size,ini)) strcpy(R+3,K);
SetDlgItemText(hGrRen,IDMNAME,R);
itoa(IDMCOUNT,K,10);strcpy(R,"%7 "); if (!GetPrivateProfileString("GRREN",K,NULL,R+3,Size,ini)) strcpy(R+3,K);
SetDlgItemText(hGrRen,IDMCOUNT,R);

LV_COLUMN lvColumn;lvColumn.mask = LVCF_SUBITEM;lvColumn.iSubItem = 0;
SendDlgItemMessage(hFile,IDFILELIST, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvColumn);

lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;lvColumn.fmt = LVCFMT_LEFT;
WINDOWPLACEMENT WP;WP.length = sizeof(WINDOWPLACEMENT);GetWindowPlacement(GetDlgItem(hGrRen,IDNAMELIST),&WP);
int Ws =  WP.rcNormalPosition.right - WP.rcNormalPosition.left - GetSystemMetrics(SM_CXEDGE)*2 - GetSystemMetrics(SM_CXVSCROLL);
int Ws0 = Ws/2.;int Ws1 = Ws - Ws0;

int i=0;
lvColumn.cx = 0;lvColumn.pszText = "Dir";lvColumn.iSubItem = i;
SendDlgItemMessage(hGrRen,IDNAMELIST, LVM_INSERTCOLUMN, (WPARAM)i++, (LPARAM)&lvColumn);

lvColumn.pszText = "Ext";lvColumn.iSubItem = i;
SendDlgItemMessage(hGrRen,IDNAMELIST, LVM_INSERTCOLUMN, (WPARAM)i++, (LPARAM)&lvColumn);

itoa(0,K,10);if (!GetPrivateProfileString("GRREN",K,NULL,R,Size,ini)) strcpy(R,K);
lvColumn.cx = Ws0;lvColumn.pszText = R;lvColumn.iSubItem = i;
SendDlgItemMessage(hGrRen,IDNAMELIST, LVM_INSERTCOLUMN, (WPARAM)i++, (LPARAM)&lvColumn);

itoa(1,K,10);if (!GetPrivateProfileString("GRREN",K,NULL,R,Size,ini)) strcpy(R,K);
lvColumn.cx = Ws1;lvColumn.pszText = R;lvColumn.iSubItem = i;
SendDlgItemMessage(hGrRen,IDNAMELIST, LVM_INSERTCOLUMN, (WPARAM)i++, (LPARAM)&lvColumn);
//hGrRen--------------------------

//hGrTag--------------------------
for (int i=IDMASKTEXT2; i<=IDTAG3; i++)
	{itoa(i,K,10);if (!GetPrivateProfileString("GRTAG",K,NULL,R,Size,ini)) strcpy(R,K); SetDlgItemText(hGrTag,i,R);}

for (int i=0; i<=3;i++)
	{
	itoa(i,K,10);if (!GetPrivateProfileString("GRTAG",K,NULL,R,Size,ini)) strcpy(R,K);
	SendDlgItemMessage(hGrTag,IDMETOD,CB_ADDSTRING,0,(LPARAM)R);
	}
//hGrTag--------------------------

//hNameTxt------------------------
for (int i=IDSTATIC1; i<=IDCLEAR; i++)
	{itoa(i,K,10);if (!GetPrivateProfileString("NAMETXT",K,NULL,R,Size,ini)) strcpy(R,K);SetDlgItemText(hNameTxt,i,R);}
//hNameTxt------------------------

//hIntegr-------------------------
for (int i=IDSTATIC5; i<=IDCRERL; i++)
	{itoa(i,K,10);if (!GetPrivateProfileString("INTEGR",K,NULL,R,Size,ini)) strcpy(R,K);SetDlgItemText(hIntegr,i,R);}

itoa(0,K,10); if (!GetPrivateProfileString("INTEGR",K,NULL,R,Size,ini)) strcpy(R,K);

char *sret=GetKey("Integr","EdFile", R);
if (sret) {SetDlgItemText(hIntegr, IDEDFILE, sret); free((char *)sret);}
sret=GetKey("Integr","EdDir", R);
if (sret) {SetDlgItemText(hIntegr, IDEDDIR, sret); free((char *)sret);}
//hIntegr-------------------------

//--------hTip--------
InitCommonControls();
hTip = CreateWindowEx(0,TOOLTIPS_CLASS,0,0,0,0,0,0,hMain,0,0,0);
SendMessage(hTip, TTM_SETDELAYTIME,TTDT_INITIAL, 0);
SendMessage(hTip, TTM_SETDELAYTIME,TTDT_AUTOPOP,20000);

itoa(20,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hFile,IDABOUT),IDABOUT,R);

itoa(21,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hFile,IDSFOLDER),IDSFOLDER,R);

itoa(22,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hExec,IDSELALL),IDSELALL,R);

itoa(23,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hExec,IDDESALL),IDDESALL,R);

itoa(24,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hExec,IDALL),IDALL,R);

itoa(25,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hExec,IDEXEC),IDEXEC,R);

itoa(26,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hTagInfo,IDT12),IDT12,R);

itoa(27,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hTagInfo,IDT21),IDT21,R);

itoa(28,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hGrRen,IDTRIM),IDTRIM,R);

itoa(29,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hNameTxt,IDTRIM2),IDTRIM2,R);

itoa(30,K,10);if (!GetPrivateProfileString("MAIN",K,NULL,R,Size,ini)) strcpy(R,K);
AddToolTip(GetDlgItem(hExec,IDREFRESH),IDREFRESH,R);
//--------hTip--------
}
//---------------------------------------------------------------------------


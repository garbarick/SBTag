char *ini=NULL;
char *fileini=NULL;
DWORD (WINAPI * pGetLongPathName) (LPCTSTR lpszShortPath,LPTSTR lpszLongPath,DWORD cchBuffer)=NULL;

//----------------------------------------------------------------------
char *CreatePathFile(char *fold=NULL, char *file=NULL, char *ext=NULL);
char *GetKey(char *AppName, char *KeyName, char *Default = NULL);
int GetKeyInt(char *AppName, char *KeyName, int Default);
void SetKey(char *AppName, char *KeyName, char *String);
BOOL SetCombo(HWND H, WPARAM wParam, char *AppName, char *KeyName);
char *GetLongPath(char *Path);
char *GetItemChar(HWND hwnd, int ID);
char *GetItemCombo(HWND hwnd, int ID);
void SBStrTrim(char **src, char *trim);

//----------------------------------------------------------------------
char *CreatePathFile(char *fold=NULL, char *file=NULL, char *ext=NULL)
{
char path[1000];
GetModuleFileName(NULL, path, 1000);
PathRemoveFileSpec(path);
if (fold) PathAppend(path, fold);
if (file) PathAppend(path, file);
if (ext) PathAddExtension(path, ext);

return strdup(path);
}
//---------------------------------------------------------------------------
char *GetKey(char *AppName, char *KeyName, char *Default = NULL)
{
char Ret[1000];
DWORD Size = 1000;
DWORD ret = GetPrivateProfileString(AppName, KeyName, Default, Ret, Size, fileini);
if (ret>0) return strdup(Ret);
else if (Default) return strdup(Default);
return NULL;
}
//---------------------------------------------------------------------------
int GetKeyInt(char *AppName, char *KeyName, int Default=0)
{
return GetPrivateProfileInt(AppName, KeyName, Default, fileini);
}
//----------------------------------------------------------------------
void SetKey(char *AppName, char *KeyName, char *String)
{
WritePrivateProfileString(AppName,KeyName,String,fileini);
}
//----------------------------------------------------------------------
BOOL SetCombo(HWND H, WPARAM wParam, char *AppName, char *KeyName)
{
if (HIWORD(wParam)==CBN_SELCHANGE)
	{
	char n[20];
	itoa(SendDlgItemMessage(H, LOWORD(wParam), CB_GETCURSEL, 0, 0), n, 10);
	SetKey(AppName, KeyName, n);
	return TRUE;
	}
return FALSE;
}
//----------------------------------------------------------------------
char *GetLongPath(char *Path)
{
if (pGetLongPathName && strchr(Path,'~'))
	{
	int l=pGetLongPathName(Path, NULL, 0);
	char *srez=(char *)malloc(l*sizeof(char));
	pGetLongPathName(Path, srez, l);
	return srez;
	}
return strdup(Path);
}
//----------------------------------------------------------------------
char *GetItemChar(HWND hwnd, int ID)
{
int l=SendDlgItemMessage(hwnd, ID, WM_GETTEXTLENGTH, 0, 0);
if (l==0) return NULL;

char *sret=(char *)malloc((l+1)*sizeof(char));
SendDlgItemMessage (hwnd, ID, WM_GETTEXT, l+1, (LPARAM)sret);
return sret;
}
//----------------------------------------------------------------------
char *GetItemCombo(HWND hwnd, int ID)
{
int c = SendDlgItemMessage(hwnd, ID, CB_GETCURSEL, 0, 0);
if (c<0) return NULL;
int l = SendDlgItemMessage(hwnd, ID, CB_GETLBTEXTLEN, c, 0);
if (l==0) return NULL;

char *sret=(char *)malloc((l+1)*sizeof(char));
SendDlgItemMessage(hwnd, ID, CB_GETLBTEXT, c, (LPARAM)sret);
return sret;
}
//----------------------------------------------------------------------
void SBStrTrim(char **src, char *trim)
{
char *dst=*src;
if (dst && strlen(dst))
	{
	while(dst && strchr(trim, dst[0])) dst++;

	int l=strlen(dst)-1;
	while(l>-1 && strchr(trim, dst[l])) {dst[l]=0; l=strlen(dst)-1;}

	*src=dst;
	}
}
//----------------------------------------------------------------------


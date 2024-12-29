#ifndef SEARCHANDPAST_H
#define SEARCHANDPAST_H

//---------------------------------------------------------------------------
char *SearchAndPast(char *N,char *sch,char *pst)
{
//����� � ������
char *Res=N;
char *bpst=pst;

int lsch,lpst;

BOOL F=TRUE;
BOOL freepst=FALSE;

if ((Res==NULL)||(sch==NULL)) return Res;
if (sch) {lsch = strlen(sch);if (lsch==0) return Res;} else return Res;
if (!bpst) {bpst=strdup("\0");freepst=TRUE;}

lpst = strlen(bpst);

char *R,*S;
R = strdup("\0");

while (F)
{
char *p=strstr(Res,sch);

if (p)
	{
	S = (char*)malloc((strlen(R)+(p-Res)+lpst+1)*sizeof(char));
	strcpy(S,R);strncat(S,Res,p-Res);strcat(S,bpst);free((char *) R);R=S;
	strcpy(Res,p+lsch);
	}
else
	{
	S = (char*)malloc((strlen(R)+strlen(Res)+1)*sizeof(char));
	strcpy(S,R);strcat(S,Res);free((char *) R);R=S;
	F=FALSE;
	}
}

if (freepst) free((char *) bpst);
if (Res) free((char *) Res);
Res=R;
return Res;
}
//---------------------------------------------------------------------------
char *TransLit(char *N,BYTE E2R)
{
char *Res=N;

switch (E2R)
	{
	case 0: case 1:
		{
		char *RusSmall[37]={"�",  "�", "�", "�", "�", "�", "���", "�",  "�", "�",  "�",  "�", "�", "�",  "�", "�",  "�",    "�",  "�", "��", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�",  "�", "�",  "�", "�", "�"};
		char *EngSmall[37]={"ja", "a", "b", "v", "g", "d",   "x", "eh", "e", "jo", "zh", "z", "i", "ju", "j", "sh", "shch", "kh", "h", "q",  "k", "l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "ch", "c", "~",  "y", "'", "w"};

		char *RusBig[37]=  {"�",  "�", "�", "�", "�", "�", "���", "�",  "�", "�",  "�",  "�", "�", "�",  "�", "�",  "�",    "�",  "�", "��", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�",  "�", "�",  "�", "�", "�"};
		char *EngBig[37]=  {"JA", "A", "B", "V", "G", "D",   "X", "EH", "E", "JO", "ZH", "Z", "I", "JU", "J", "SH", "SHCH", "KH", "H", "Q",  "K", "L", "M", "N", "O", "P", "R", "S", "T", "U", "F", "CH", "C", "~",  "Y", "'", "W"};

		for (int i=0;i<37;i++)
			{
			if (E2R){Res = SearchAndPast(Res,EngSmall[i],RusSmall[i]);Res = SearchAndPast(Res,EngBig[i],RusBig[i]);}
			else	{Res = SearchAndPast(Res,RusSmall[i],EngSmall[i]);Res = SearchAndPast(Res,RusBig[i],EngBig[i]);}
			}
		}
	break;

	case 2:
		{
		char *RusISOSmall[33]={"�","�","�","�","�","�","�", "�", "�","�","�","�","�","�","�","�","�","�","�","�","�","�","�", "�", "�", "�", "�",   "�","�","�","�","�", "�"};
		char *EngISOSmall[33]={"a","b","v","g","d","e","yo","zh","z","i","y","k","l","m","n","o","p","r","s","t","u","f","kh","ts","ch","sh","shch","'","y","'","e","yu","ya"};

		char *RusISOBig[33]={"�","�","�","�","�","�","�", "�", "�","�","�","�","�","�","�","�","�","�","�","�","�","�","�", "�", "�", "�", "�",   "�","�","�","�","�", "�"};
		char *EngISOBig[33]={"A","B","V","G","D","E","YO","ZH","Z","I","Y","K","L","M","N","O","P","R","S","T","U","F","KH","TS","CH","SH","SHCH","'","Y","'","E","YU","YA"};

		for (int i=0;i<33;i++)
			{
			Res = SearchAndPast(Res,RusISOSmall[i],EngISOSmall[i]);Res = SearchAndPast(Res,RusISOBig[i],EngISOBig[i]);
			}
		}
		break;
	}
return Res;
}
//---------------------------------------------------------------------------

#endif


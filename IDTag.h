#ifndef _TID3V2_
#define _TID3V2_
//---------------------------------------------------------------------------
#include "frame.h"
//---------------------------------------------------------------------------
#define TAG_VERSION_2_2 2 // Code for ID3v2.2.x tag
#define TAG_VERSION_2_3 3 // Code for ID3v2.3.x tag
#define TAG_VERSION_2_4 4 // Code for ID3v2.4.x tag
#define ID3V2_ID "ID3"
#define ID3V1_ID "TAG"
#define SIZE_GANR 149
//---------------------------------------------------------------------------
char *GanR[SIZE_GANR]={
"Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge","Hip-Hop",
"Jazz","Metal","New Age","Oldies","Other","Pop","R&B","Rap",
"Reggae","Rock","Techno","Industrial","Alternative","Ska","Death Metal","Pranks",
"Soundtrack","Euro-Techno","Ambient","Trip-Hop","Vocal","Jazz+Funk","Fusion","Trance",
"Classical","Instrumental","Acid","House","Game","Sound Clip","Gospel","Noise",
"Alt. Rock","Bass","Soul","Punk","Space","Meditative","Instrumental Pop","Instrumental Rock",
"Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance","Dream",
"Southern Rock","Comedy","Cult","Gangsta Rap","Top 40","Christian Rap","Pop/Funk","Jungle",
"Native American","Cabaret","New Wave","Psychedelic","Rave","Showtunes","Trailer","Lo-Fi",
"Tribal","Acid Punk","Acid Jazz","Polka","Retro","Musical","Rock & Roll","Hard Rock",
"Folk","Folk/Rock","National Folk","Swing","Fast-Fusion","Bebob","Latin","Revival",
"Celtic","Bluegrass","Avantgarde","Gothic Rock","Progressive Rock","Psychedelic Rock","Symphonic Rock","Slow Rock",
"Big Band","Chorus","Easy Listening","Acoustic","Humour","Speech","Chanson","Opera",
"Chamber Music","Sonata","Symphony","Booty Bass","Primus","Porn Groove","Satire","Slow Jam",
"Club","Tango","Samba","Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle",
"Duet","Punk Rock","Drum Solo","A Cappella","Euro-House","Dance Hall","Goa","Drum & Bass",
"Club-House","Hardcore","Terror","Indie","BritPop","Negerpunk","Polsk Punk","Beat",
"Christian Gangsta Rap","Heavy Metal","Black Metal","Crossover","Contemporary Christian","Christian Rock","Merengue","Salsa",
"Thrash Metal","Anime","JPop","Synthpop",""
};
char **sGanR[SIZE_GANR];
BOOL bsGanR=TRUE;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------ID3v1-------------------------------------
//---------------------------------------------------------------------------
typedef struct taginfov1// ID3v1 header data
{
char ID[3];// Always "TAG"
char Title[30];
char Artist[30];
char Album[30];
char Year[4];
char Comment[28];
unsigned char Version;// Version number
unsigned char Track;
unsigned char Genre;
} TagInfoV1;
//---------------------------------------------------------------------------
typedef struct id3v1
{
char Title[31];
char Artist[31];
char Album[31];
char Year[5];
char Comment[29];
int Track;
BYTE Genre;
BOOL Exists;
} ID3v1;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Set(char* t, char* s, int r)
{
memset(t,0,r);
if (s)
	{
	r=min(r, strlen(s));
	strncpy(t,s,r);
	t[r]=0;
	SBStrTrim(&t, " ");
	}
}
//---------------------------------------------------------------------------
void Clear1(ID3v1 *idS)// Reset all data
{
ZeroMemory(idS, sizeof(ID3v1)); idS->Genre=148;
}
//---------------------------------------------------------------------------
BOOL ReadFromFile1(ID3v1 *idS, char *FileName)// Load tag
{
TagInfoV1 Tag; ZeroMemory(&Tag, sizeof(TagInfoV1));
FILE *SourceFile;

Clear1(idS);
if ((SourceFile=fopen(FileName, "rb"))==NULL) return FALSE;

fseek(SourceFile, 0, SEEK_END);
fseek(SourceFile, ftell(SourceFile)-128, SEEK_SET);

fread(&Tag,1,128,SourceFile);

fclose(SourceFile);

if (strstr(Tag.ID,ID3V1_ID)==NULL) return FALSE;
idS->Exists=TRUE;

Set(idS->Title,Tag.Title,30);
Set(idS->Artist,Tag.Artist,30);
Set(idS->Album,Tag.Album,30);
Set(idS->Year,Tag.Year,4);
Set(idS->Comment,Tag.Comment,28);
idS->Track=Tag.Track;
if (Tag.Genre>(SIZE_GANR-1)) idS->Genre=148; else idS->Genre=Tag.Genre;

return TRUE;
}
//---------------------------------------------------------------------------
BOOL SaveToFile1(ID3v1 *idS, char *FileName)// Save tag
{
TagInfoV1 Tag; ZeroMemory(&Tag, sizeof(TagInfoV1));
FILE *SourceFile;

// Prepare tag data and save to file
strncpy(Tag.ID,ID3V1_ID,3);
strncpy(Tag.Title,idS->Title,30);
strncpy(Tag.Artist,idS->Artist,30);
strncpy(Tag.Album,idS->Album,30);
strncpy(Tag.Year,idS->Year,4);
strncpy(Tag.Comment,idS->Comment,28);
Tag.Version=0;
Tag.Track = idS->Track;
Tag.Genre = idS->Genre;

if (idS->Exists)
	{
	if ((SourceFile=fopen(FileName, "r+b"))==NULL) return FALSE;

	fseek(SourceFile, 0, SEEK_END);
	fseek(SourceFile, ftell(SourceFile)-128, SEEK_SET);
	}
else
	{
	if ((SourceFile=fopen(FileName, "ab"))==NULL) return FALSE;
	}

fwrite(&Tag, 1, 128, SourceFile);
fclose(SourceFile);
return TRUE;
}
//---------------------------------------------------------------------------
BOOL RemoveFromFile1(ID3v1 *idS, char *FileName)// Delete tag
{
if (!idS->Exists) return TRUE;
int Source; if((Source=open(FileName,_O_RDWR,_S_IREAD|_S_IWRITE))==-1) return FALSE;

int rez = chsize(Source, filelength(Source)-128);
close(Source); if (rez!=0) return FALSE;

return TRUE;
}
//---------------------------------------------------------------------------
//---------------------------------ID3v2-------------------------------------

//---------------------------------------------------------------------------
union pole{unsigned char CH[4];unsigned int IG;};
//---------------------------------------------------------------------------
typedef struct frameheader// Frame header (ID3v2.3.x & ID3v2.4.x)
{
unsigned char ID[4];// Frame ID
unsigned int Size;// Size excluding header
} FrameHeader;
//---------------------------------------------------------------------------
typedef struct taginfov2// ID3v2 header data
{
char ID[3];//Always "ID3"
unsigned char Version;//Version number
unsigned char Revision;//Revision number
unsigned char Flags;//Flags of tag
unsigned char Size[4];//Tag size excluding header
int FileSize;//File size (bytes)
char *Frame[ID3V2_FRAME_COUNT];//Information from frames
} TagInfoV2;
//---------------------------------------------------------------------------
typedef struct id3v2
{
char *Title;
char *Artist;
char *Album;
char *Track;
char *Year;
char *Genre;
char *Comment;
char *Composer;
char *Encoder;
char *Copyright;
char *Language;
char *Url;
char *OrigYear;
char *OrigArtist;
char *OrigTitle;
char *OrigAlbum;
BOOL Exists;
unsigned char VersionID;
unsigned int  Size;
unsigned int  FileSize;
} ID3v2;
//---------------------------------------------------------------------------
void SetTagItem(char **TagName, char *TagValue)
{
if (*TagName) free((char*) *TagName);
if (TagValue) *TagName=strdup(TagValue); else *TagName=NULL;
}
//---------------------------------------------------------------------------
void Clear2(ID3v2 *idS)// Reset all data
{
idS->Exists = FALSE;
idS->VersionID = 0;
idS->Size = 0;
idS->FileSize = 0;

SetTagItem(&idS->Title, NULL);
SetTagItem(&idS->Artist, NULL);
SetTagItem(&idS->Album, NULL);
SetTagItem(&idS->Track, NULL);
SetTagItem(&idS->Year, NULL);
SetTagItem(&idS->Genre, NULL);
SetTagItem(&idS->Comment, NULL);
SetTagItem(&idS->Composer, NULL);
SetTagItem(&idS->Encoder, NULL);
SetTagItem(&idS->Copyright, NULL);
SetTagItem(&idS->Language, NULL);
SetTagItem(&idS->Url, NULL);
SetTagItem(&idS->OrigYear, NULL);
SetTagItem(&idS->OrigArtist, NULL);
SetTagItem(&idS->OrigTitle, NULL);
SetTagItem(&idS->OrigAlbum, NULL);
}
//---------------------------------------------------------------------------
BOOL ReadHeader(char *FileName, TagInfoV2 *Tag)
{
FILE *SourceFile;
BYTE Transferred;

if ((SourceFile=fopen(FileName, "rb"))==NULL) return FALSE;

Transferred=fread(Tag, 1, 10, SourceFile);
fseek(SourceFile, 0, SEEK_END);
Tag->FileSize=ftell(SourceFile);
fclose(SourceFile);

if (Transferred<10) return FALSE;
return TRUE;
}
//---------------------------------------------------------------------------
unsigned int GetTagSize(TagInfoV2 *Tag)
{
unsigned int Result=0;

if (strstr(Tag->ID,ID3V2_ID) && Tag->Version>TAG_VERSION_2_2)
	{
	Result = Tag->Size[0] * 0x200000 + Tag->Size[1] * 0x4000 + Tag->Size[2] * 0x80 + Tag->Size[3] + 10;
	if (Tag->Flags == 10) Result += 10;
	if (Result > (unsigned int)Tag->FileSize) Result = 0;
	}

return Result;
}
//---------------------------------------------------------------------------
unsigned int Swap32(unsigned int size)
{
union pole tmp1,tmp2;
tmp2.IG=size;
tmp1.CH[0]=tmp2.CH[3];
tmp1.CH[1]=tmp2.CH[2];
tmp1.CH[2]=tmp2.CH[1];
tmp1.CH[3]=tmp2.CH[0];
return tmp1.IG;
}
//---------------------------------------------------------------------------
int gstrcmp(char ***s1, char ***s2) {return strcmp(**s1, **s2);}
int bstrcmp(char *s1, char ***s2) {return strcmp(s1, **s2);}
//---------------------------------------------------------------------------
int GetNumGanr(char *ganrstr)
{
if (bsGanR)
	{
	for (int i=0; i<SIZE_GANR; i++) sGanR[i]=&GanR[i];
	qsort(sGanR, SIZE_GANR,	sizeof(char ***), gstrcmp);
	bsGanR=FALSE;
	}
if (ganrstr)
	{
	char ***p=bsearch(ganrstr, sGanR, SIZE_GANR, sizeof(char ***), bstrcmp);
	if (p) return *p-GanR;
	}
return (SIZE_GANR-1);
}
//---------------------------------------------------------------------------
BOOL NoValidFram(char *c)
{
BOOL bRet=FALSE;
for (int i=0; i<4; i++) if ((c[i]>='A' && c[i]<='Z') || (c[i]>='0' && c[i]<='9')); else {bRet=TRUE; break;}
return bRet;
}
//---------------------------------------------------------------------------
int GetNumFrame(char *sframe)
{
char *p=bsearch(sframe, ID3V2_FRAME, ID3V2_FRAME_COUNT, ID3V2_FRAME_ITEM, strcmp);
if (p==NULL) return -1;
return (p-ID3V2_FRAME[0])/ID3V2_FRAME_ITEM;
}
//---------------------------------------------------------------------------
char *GetFrame(char **Frame, char *sframe)
{
int i=GetNumFrame(sframe);
if (i>-1) return Frame[i];
return NULL;
}
//---------------------------------------------------------------------------
void SetFrame(char **Frame, char *sframe, char *svalue)
{
int i=GetNumFrame(sframe);
if (i>-1) Frame[i]=svalue;
}
//---------------------------------------------------------------------------
BOOL ifNeedFrame(char *sframe)
{
return (bsearch(sframe, NEED_FRAME, NEED_FRAME_COUNT, ID3V2_FRAME_ITEM, strcmp)!=NULL);
}
//---------------------------------------------------------------------------
void ClearFrame(char **Frame, char *sframe)
{
int i=GetNumFrame(sframe);
if (i>-1 && Frame[i]) {free((char *) Frame[i]); Frame[i]=NULL;}
}
//---------------------------------------------------------------------------
void ReadFrames(char *FileName, TagInfoV2 *Tag)
{
FILE *SourceFile;
FrameHeader Frame;
int DataPosition, DataSize;
int PozEndTag = GetTagSize(Tag);

if ((SourceFile=fopen(FileName, "rb"))!=NULL)
	{
	fseek(SourceFile,10,SEEK_CUR);
	do	{
		fread(&Frame,8,1,SourceFile);
		DataSize=Swap32(Frame.Size); if (DataSize<0) DataSize=0;

		fseek(SourceFile,2,SEEK_CUR);
		int cod = fgetc(SourceFile);

		BOOL tagread=FALSE;

		if (DataSize>0)
			{
			if (NoValidFram(Frame.ID)) break;
			if (ifNeedFrame(Frame.ID))
				{
				int i=GetNumFrame(Frame.ID);
				if (i>-1)
					{
					if (!strcmp(Frame.ID,"COMM"))	{fseek(SourceFile,4,SEEK_CUR); DataSize-=4;}

					char *sbuf=(char *)malloc((DataSize+1)*sizeof(char));
					int lread=fread(sbuf, sizeof(char), DataSize-1, SourceFile); sbuf[lread]=0;
					if (lread==0) {free((char *) sbuf); sbuf=NULL;}

					if (sbuf && cod==1)
						{
						while (sbuf[0]!='ÿ' && lread>0) {lread--; sbuf++;} lread/=2;
						if (lread>0)
							{
							WCHAR *wbuf=(WCHAR *)sbuf; wbuf[lread]=L'\0'; wbuf++;

							int lbuf=WideCharToMultiByte(CP_ACP, 0, wbuf, -1, NULL, 0, NULL, NULL);
							if (lbuf>0)
								{
								char *sbyte=(char *)malloc((lbuf+1)*sizeof(char));
								lbuf=WideCharToMultiByte(CP_ACP, 0, wbuf, -1, sbyte, lbuf, NULL, NULL);
								sbyte[lbuf]=0;
								free((char *) sbuf);
								sbuf=sbyte;
								}
							}
						else {free((char *) sbuf); sbuf=NULL;}
						}

					Tag->Frame[i]=sbuf;

					if (Tag->Frame[i] && !strcmp(Frame.ID,"TCON"))
						{
						char *PozSkob1,*PozSkob2;
						BYTE NGanr;

						PozSkob1=strchr(Tag->Frame[i],'(');
						PozSkob2=strchr(Tag->Frame[i],')');

						if (PozSkob1 && PozSkob2)
							{
							NGanr=atoi(PozSkob1+1);
							if (NGanr>=0 && NGanr<SIZE_GANR)
								{
								free((char *) Tag->Frame[i]);
								Tag->Frame[i]=strdup(GanR[NGanr]);
								}
							else strcpy(Tag->Frame[i], PozSkob2+1);
							}
						}
					tagread=TRUE;
					}
				}
			}

		if (!tagread) fseek(SourceFile,DataSize-1,SEEK_CUR);
		DataPosition=ftell(SourceFile);
		}
	while (DataPosition<(PozEndTag+100) && !feof(SourceFile));
	fclose(SourceFile);
	}
}
//---------------------------------------------------------------------------
BOOL ReadFromFile2(ID3v2 *idS, char *FileName)
{
TagInfoV2 Tag; ZeroMemory(&Tag,sizeof(TagInfoV2));
BOOL Result=FALSE;

// Reset data and load header from file to variable
Clear2(idS);
Result = ReadHeader(FileName, &Tag);
if (Result) idS->FileSize = Tag.FileSize;
// Process data if loaded and header valid

if (Result && strstr(Tag.ID,ID3V2_ID))
	{
	idS->Exists = TRUE;

	// Fill properties with header data
	idS->VersionID = Tag.Version;
	idS->Size = GetTagSize(&Tag);

	// Get information from frames if version supported
	//IRAC(idS->VersionID); IRAC(idS->Size);
	if ((idS->VersionID > TAG_VERSION_2_2) && (idS->Size > 0))
		{
		ReadFrames(FileName, &Tag);

		idS->Title=		GetFrame(Tag.Frame, "TIT2");

		idS->Artist=	GetFrame(Tag.Frame, "TPE1");
		//if (idS->Artist==NULL) idS->Artist=	GetFrame(Tag.Frame, "TPE2"); else ClearFrame(Tag.Frame, "TPE2");

		idS->Album=		GetFrame(Tag.Frame, "TALB");
		idS->Track=		GetFrame(Tag.Frame, "TRCK");
		idS->Year=		GetFrame(Tag.Frame, "TYER");
		idS->Genre=		GetFrame(Tag.Frame, "TCON");
		idS->Comment=	GetFrame(Tag.Frame, "COMM");
		idS->Composer=	GetFrame(Tag.Frame, "TCOM");
		idS->Encoder=	GetFrame(Tag.Frame, "TENC");
		idS->Copyright=	GetFrame(Tag.Frame, "TCOP");
		idS->Language=	GetFrame(Tag.Frame, "TLAN");
		idS->Url=		GetFrame(Tag.Frame, "WXXX");
		idS->OrigYear=	GetFrame(Tag.Frame, "TDRC");
		idS->OrigArtist=GetFrame(Tag.Frame, "TOPE");
		idS->OrigTitle=	GetFrame(Tag.Frame, "TIT1");
		idS->OrigAlbum=	GetFrame(Tag.Frame, "TOAL");
    	}
	}
return Result;
}
//---------------------------------------------------------------------------
void BuildHeader(TagInfoV2 *Tag)
{
unsigned int TagSize;
int numganr;
char ganr[10];

// Build tag header
strcpy(Tag->ID,ID3V2_ID);
Tag->Version = TAG_VERSION_2_3;
Tag->Revision = 0;
Tag->Flags = 0;
TagSize = 0;

for (int i=0;i<ID3V2_FRAME_COUNT;i++)
if (Tag->Frame[i] && strlen(Tag->Frame[i]))
	{
	TagSize+=(strlen(Tag->Frame[i])+11);

	if (!strcmp(ID3V2_FRAME[i], "COMM")) TagSize+=4;

	if (Tag->Frame[i] && !strcmp(ID3V2_FRAME[i], "TCON"))
		{
		numganr=GetNumGanr(Tag->Frame[i]); if (numganr==SIZE_GANR-1) numganr=-1;
		itoa(numganr, ganr, 10);
		if (numganr>-1) TagSize+=(strlen(ganr)+2);
		}
	}
//  Convert tag size
Tag->Size[0] = TagSize/0x200000;
Tag->Size[1] = TagSize/0x4000;
Tag->Size[2] = TagSize/0x80;
Tag->Size[3] = TagSize%0x80;
}
//---------------------------------------------------------------------------
BOOL SaveTag(char *FileName, TagInfoV2 *Tag, unsigned int PozEndTagOld, unsigned int SizeByte)
{
int numganr;
char ganr[10];

union pole Frame;
FILE *Source, *Destination;

char *ManyByte=NULL;
while (ManyByte==NULL) {SizeByte/=2;ManyByte=(char*) malloc((SizeByte+1)*sizeof(char));}

unsigned int ByteRead;

// Rebuild file with tag data
if ((Source=fopen(FileName, "rb"))==NULL) return FALSE;

// Create file streams
char *BufferName=(char*) malloc((strlen(FileName)+2)*sizeof(char));
strcpy(BufferName,FileName);
strcat(BufferName,"~");

if ((Destination=fopen(BufferName, "wb"))==NULL) {free((char*)BufferName);return FALSE;}

if (Tag!=NULL)
	{
	BuildHeader(Tag);
	fwrite(Tag,1,10,Destination);

	for (int i=0;i<ID3V2_FRAME_COUNT;i++)
		{
		int lF=0;
		if (Tag->Frame[i]) lF=strlen(Tag->Frame[i]);
		if (lF)
			{
			fputs(ID3V2_FRAME[i],Destination);
			Frame.IG=lF+1;

			if (!strcmp(ID3V2_FRAME[i],"COMM"))
				{
				Frame.IG+=4;Frame.IG=Swap32(Frame.IG);
				fwrite(Frame.CH,1,4,Destination);fwrite("\0\0\0\0\0\0\0",1,7,Destination);
				}
			else if (!strcmp(ID3V2_FRAME[i],"WXXX"))
				{
				Frame.IG+=1;Frame.IG=Swap32(Frame.IG);
				fwrite(Frame.CH,1,4,Destination);fwrite("\0\0\0\0",1,4,Destination);
				}
			else if (!strcmp(ID3V2_FRAME[i],"TCON"))
				{
				numganr=GetNumGanr(Tag->Frame[i]);  if (numganr==SIZE_GANR-1) numganr=-1;
				itoa(numganr, ganr, 10);
				if (numganr>-1) Frame.IG+=(strlen(ganr)+2);
				Frame.IG=Swap32(Frame.IG);
				fwrite(Frame.CH,1,4,Destination);fwrite("\0\0\0",1,3,Destination);
				if (numganr>-1) {fputc('(',Destination);fputs(ganr,Destination);fputc(')',Destination);}
				}
			else
				{
				Frame.IG=Swap32(Frame.IG);
				fwrite(Frame.CH,1,4,Destination);fwrite("\0\0\0",1,3,Destination);
				}

			fputs(Tag->Frame[i],Destination);
			}
		}
	}

// Copy data blocks
fseek(Source,PozEndTagOld,SEEK_SET);

ByteRead=fread(ManyByte,1,SizeByte,Source);
while (ByteRead>0)
	{
	fwrite(ManyByte,1,ByteRead,Destination);
	ByteRead=fread(ManyByte,1,SizeByte,Source);
	}
fclose(Source);
fclose(Destination);
free((char*)ManyByte);

if (remove(FileName)==0) rename(BufferName,FileName);
else {remove(BufferName);free((char*)BufferName);return FALSE;}

free((char*)BufferName);
return TRUE;
}
//---------------------------------------------------------------------------
BOOL SaveToFile2(ID3v2 *idS, char *FileName)
{
TagInfoV2 Tag; ZeroMemory(&Tag, sizeof(TagInfoV2));

SetFrame(Tag.Frame, "TIT2", idS->Title);
SetFrame(Tag.Frame, "TPE1", idS->Artist);
SetFrame(Tag.Frame, "TALB", idS->Album);
SetFrame(Tag.Frame, "TRCK", idS->Track);
SetFrame(Tag.Frame, "TYER", idS->Year);
SetFrame(Tag.Frame, "TCON", idS->Genre);
SetFrame(Tag.Frame, "COMM", idS->Comment);
SetFrame(Tag.Frame, "TCOM", idS->Composer);
SetFrame(Tag.Frame, "TENC", idS->Encoder);
SetFrame(Tag.Frame, "TCOP", idS->Copyright);
SetFrame(Tag.Frame, "TLAN", idS->Language);
SetFrame(Tag.Frame, "WXXX", idS->Url);
SetFrame(Tag.Frame, "TDRC", idS->OrigYear);
SetFrame(Tag.Frame, "TOPE", idS->OrigArtist);
SetFrame(Tag.Frame, "TIT1", idS->OrigTitle);
SetFrame(Tag.Frame, "TOAL", idS->OrigAlbum);

return SaveTag(FileName, &Tag, idS->Size, idS->FileSize);
}
//---------------------------------------------------------------------------
BOOL RemoveFromFile2(ID3v2 *idS, char *FileName)
{return SaveTag(FileName,NULL,idS->Size,idS->FileSize);}

//---------------------------------------------------------------------------
#endif

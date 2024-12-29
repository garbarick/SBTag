#ifndef MP3INFO_H
#define MP3INFO_H
//---------------------------------------------------------------------------
BOOL loadHeaderV(int *frames,char inputheader[12])
{
if (memcmp(inputheader, "Xing", 4)){*frames = -1;return FALSE;}

int flags = (int)	(
					( (inputheader[4] & 255) << 24) |
					( (inputheader[5] & 255) << 16) |
					( (inputheader[6] & 255) <<  8) |
					( (inputheader[7] & 255)      )
					);

if (flags & 0x0001)
	{
	*frames = (int)	(
					( (inputheader[ 8] & 255) << 24) |
					( (inputheader[ 9] & 255) << 16) |
					( (inputheader[10] & 255) <<  8) |
					( (inputheader[11] & 255)      )
					);
	}
else *frames = -1;
return TRUE;
}
//---------------------------------------------------------------------------
unsigned long loadHeader(char c[4])
{
return (unsigned long)	(((c[0]&255)<<24) | ((c[1]&255)<<16) | ((c[2]&255)<<8 ) | ((c[3]&255)));
}
//---------------------------------------------------------------------------
int getFrameSync(unsigned long *bithdr)		{return ((*bithdr>>21) & 2047);}
int getVersionIndex(unsigned long *bithdr)	{return ((*bithdr>>19) & 3);}
int getLayerIndex(unsigned long *bithdr)	{return ((*bithdr>>17) & 3);}
int getProtectionBit(unsigned long *bithdr)	{return ((*bithdr>>16) & 1);}
int getBitrateIndex(unsigned long *bithdr)	{return ((*bithdr>>12) & 15);}
int getFrequencyIndex(unsigned long *bithdr){return ((*bithdr>>10) & 3);}
int getPaddingBit(unsigned long *bithdr)	{return ((*bithdr>> 9) & 1);}
int getPrivateBit(unsigned long *bithdr)	{return ((*bithdr>> 8) & 1);}
int getModeIndex(unsigned long *bithdr)		{return ((*bithdr>> 6) & 3);}
int getModeExtIndex(unsigned long *bithdr)	{return ((*bithdr>> 4) & 3);}
int getCoprightBit(unsigned long *bithdr)	{return ((*bithdr>> 3) & 1);}
int getOrginalBit(unsigned long *bithdr)	{return ((*bithdr>> 2) & 1);}
int getEmphasisIndex(unsigned long *bithdr)	{return ((*bithdr    ) & 3);}
//---------------------------------------------------------------------------
BOOL isValidHeader(unsigned long *bithdr)
{
return(	((getFrameSync(bithdr)		& 2047)==2047) &&
		((getVersionIndex(bithdr)	&    3)!=   1) &
		((getLayerIndex(bithdr)		&    3)!=   0) &&
		((getBitrateIndex(bithdr)	&   15)!=   0) &&
		((getBitrateIndex(bithdr)	&   15)!=  15) &&
		((getFrequencyIndex(bithdr)	&    3)!=   3) &&
		((getEmphasisIndex(bithdr)	&    3)!=   2)		);
}
//---------------------------------------------------------------------------
float getVersion(unsigned long *bithdr){float table[4] = {2.5,0.0,2.0,1.0};return table[getVersionIndex(bithdr)];}
//---------------------------------------------------------------------------
int getLayer(unsigned long *bithdr){return (4-getLayerIndex(bithdr));}
//---------------------------------------------------------------------------
int getBitrate(unsigned long *bithdr)
{
const int table[2][3][16]={
		{         //MPEG 2 & 2.5
			{0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer III
			{0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer II
			{0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,0}  //Layer I
		},{       //MPEG 1
			{0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,0}, //Layer III
			{0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,0}, //Layer II
			{0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,0}  //Layer I
		}};
return table[(getVersionIndex(bithdr) & 1)][(getLayerIndex(bithdr) - 1)][getBitrateIndex(bithdr)];
}
//---------------------------------------------------------------------------
int getFrequency(unsigned long *bithdr)
{
int table[4][3] = {
		{32000, 16000,  8000}, //MPEG 2.5
		{    0,     0,     0}, //reserved
		{22050, 24000, 16000}, //MPEG 2
		{44100, 48000, 32000}  //MPEG 1
		};
return table[getVersionIndex(bithdr)][getFrequencyIndex(bithdr)];
}
//---------------------------------------------------------------------------
void getMode(unsigned long *bithdr, char* input)
{
switch(getModeIndex(bithdr))
	{
	case 1:		strcpy(input, "Joint Stereo");break;
	case 2:		strcpy(input, "Dual Channel");break;
	case 3:		strcpy(input, "Single Channel");break;
	default:	strcpy(input, "Stereo");break;
    }
}
//---------------------------------------------------------------------------
typedef struct mp3info
{
unsigned long header;
int vbr;

BOOL VBitRate;
BOOL Tagged;

int fileSize;
char versionchar[32];
char FormattedLength[16];
char modechar[32];
} MP3Info;
//---------------------------------------------------------------------------
BOOL loadInfoMP3(MP3Info *mp3, char *FileName)
{
FILE *ifile;
if ((ifile=fopen(FileName, "rb"))==NULL) return FALSE;

fseek(ifile,0,SEEK_END);
mp3->fileSize = ftell(ifile);

int pos = 0;
char headerchars[4];

do	{
	if ( pos>(1024*200) || feof(ifile) )	{fclose(ifile);return FALSE;}

	fseek(ifile,pos,SEEK_SET);
	fread(headerchars,1,4,ifile);
	pos++;
	mp3->header = loadHeader(headerchars);
	}while (!isValidHeader(&mp3->header));

pos += 3;
char vbrchars[12];

if (getVersionIndex(&mp3->header)==3)		{if (getModeIndex(&mp3->header)==3) pos += 17;else pos += 32;}
									else	{if (getModeIndex(&mp3->header)==3) pos +=  9;else pos += 17;}

fseek(ifile,pos,SEEK_SET);
fread(vbrchars,1,12,ifile);

mp3->VBitRate = loadHeaderV(&mp3->vbr,vbrchars);

fclose(ifile);
return TRUE;
}
//---------------------------------------------------------------------------
int getNumberOfFramesMP3(MP3Info *mp3)
{
if (!(mp3->VBitRate))
	{
	float medFrameSize = (float)(
								((getLayerIndex(&mp3->header)==3) ? 12 : 144 )*
								((1000.0 * (float)getBitrate(&mp3->header)) / (float)getFrequency(&mp3->header))
								);
	return (int)(mp3->fileSize/medFrameSize);
	}
	else return mp3->vbr;
}
//---------------------------------------------------------------------------
int getBitrateMP3(MP3Info *mp3)
{
if (mp3->VBitRate)
	{
	float medFrameSize = (float)(mp3->fileSize)/(float)getNumberOfFramesMP3(mp3);
	return (int)(
				( medFrameSize * (float)getFrequency(&mp3->header) ) /
				( 1000.0 * ( (getLayerIndex(&mp3->header)==3) ? 12.0 : 144.0))
				);
	}
	else return getBitrate(&mp3->header);
}
//---------------------------------------------------------------------------
char* getVersionMP3(MP3Info *mp3)
{
char tempchar2[4];

float ver = getVersion(&mp3->header);

int i;
for( i=0; i<getLayer(&mp3->header); i++ ) tempchar2[i] = 'I';
tempchar2[i] = '\0';

sprintf(mp3->versionchar,"MPEG %g Layer %s", (double)ver, tempchar2);
return mp3->versionchar;
}
//---------------------------------------------------------------------------
int getFrequencyMP3(MP3Info *mp3)	{return getFrequency(&mp3->header);};
//---------------------------------------------------------------------------
char *getModeMP3(MP3Info *mp3)	{getMode(&mp3->header,mp3->modechar);return mp3->modechar;}
//---------------------------------------------------------------------------
int getLengthInSecondsMP3(MP3Info *mp3)
{
int kiloBitFileSize = (8 * mp3->fileSize) / 1000;
return (int)(kiloBitFileSize/getBitrateMP3(mp3));
}
//---------------------------------------------------------------------------
char *getFormattedLengthMP3(MP3Info *mp3)
{
int s  = getLengthInSecondsMP3(mp3);
int ss = s%60;
int m  = (s-ss)/60;
int mm = m%60;
int h = (m-mm)/60;

if (h>0)	sprintf(mp3->FormattedLength,"%02d:%02d:%02d", h,mm,ss);
else		sprintf(mp3->FormattedLength,"%02d:%02d",   mm,ss);

return mp3->FormattedLength;
}
//---------------------------------------------------------------------------

#endif

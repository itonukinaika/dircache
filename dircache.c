#include <stdio.h>
#include <windows.h>

char DIRName[_MAX_PATH];
int MaxAge;
char StrMaxAge[32] = "86400";
char FilePath[_MAX_PATH];
int FileSize;

SYSTEMTIME CurrentSystemTime;
FILETIME CurrentLocalFileTime;
FILETIME CurrentFileTime;
__int64 * nCurrentFileTime = (__int64*)&CurrentFileTime;

HANDLE hFile;
LPSTR FileBuffer;
DWORD wReadSize;

char WithWildCard[3]="\\*";

void CacheFile(char path[])
{
	printf("Cache:%s",path);
	hFile = CreateFile(path , GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);
	FileSize = GetFileSize(hFile , NULL);
	FileBuffer = (LPSTR)malloc(FileSize);
	ReadFile(hFile , FileBuffer , FileSize , &wReadSize , NULL);
	free(FileBuffer);
	printf(" %d bytes\n", wReadSize);
	return;
}

void ScanDIR(char path[])
{
    char subpath[_MAX_PATH];
    char temp[_MAX_PATH];
    WIN32_FIND_DATA lp;
	FILETIME FileTime;
	__int64 * nFileTime = (__int64*)&FileTime;
	__int64 nDiffSec;

    strcpy(temp,path);
    HANDLE h=FindFirstFile(path,&lp);

    temp[strlen(temp)-1]='\0';//remove trailing asterisk
    do
    {      
        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && strcmp(lp.cFileName,"..")!=0 && strcmp(lp.cFileName,".")!=0)
        {            
           //extract full path
            wsprintf(subpath,"%s%s\\*",temp,lp.cFileName);
            ScanDIR(subpath);
        }
        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
        {
           wsprintf(FilePath,"%s%s",temp,lp.cFileName);
        	FileTime=lp.ftLastWriteTime;
        	nDiffSec = (*nCurrentFileTime - *nFileTime) / 10000 / 1000;
        	if ((MaxAge ==0)||(nDiffSec < MaxAge)){
        		CacheFile(FilePath);
        	}
        }        
    } while(FindNextFile(h,&lp));
 
        FindClose(h);
}

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , PSTR lpCmdLine , int nCmdShow ) {
		int i=0;
	for (i = 0; i < __argc; i++) {
		switch(i){
			case 1:
				strcpy(DIRName, __argv[1]);
				break;
			case 2:
				strcpy(StrMaxAge, __argv[2]);
				break;
		}
	}
	MaxAge=atoi(StrMaxAge);
	GetLocalTime(&CurrentSystemTime);
	SystemTimeToFileTime(&CurrentSystemTime, &CurrentLocalFileTime);
	LocalFileTimeToFileTime(&CurrentLocalFileTime, &CurrentFileTime);
	if(GetFileAttributes(DIRName) & FILE_ATTRIBUTE_DIRECTORY){
		strcat(DIRName,WithWildCard);
		ScanDIR(DIRName);
	}else{
		CacheFile(DIRName);
	}

	

}

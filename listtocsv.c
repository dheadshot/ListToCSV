//Compile with kernel32
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

DWORD ListFiles(char *szDir, char *fargptr, char *fext, int recurs);

int streq_(char *a, char *b)
{
  unsigned long la, lb;
  la = strlen(a);
  lb = strlen(b);
  if (la != lb) return 0;
  if (memcmp(a,b,la)==0) return 1; else return 0;
}

int endeq_(char *a, char *b, int casesensitive)
{
  unsigned long la, lb, oa=0, ob=0, i, j;
  la = strlen(a);
  lb = strlen(b);
  if (la > lb) oa = la - lb;
  if (lb > la) ob = lb - la;
  j = ob;
  for (i=oa; i<la; i++)
  {
    if (a[i] != b[j] && (casesensitive || (toupper(a[i]) != toupper(b[j])))) return 0;
	j++;
  }
  return 1;
}

long FindChar(char *astr, long maxlen, char fchar)
{
  long i;
  for (i=0;i<maxlen && astr[i]!=0;i++)
  {
    if (astr[i] == fchar) return i;
  }
  return -1;
}

int main(int argc, char *argv[])
{
  HANDLE hFind = INVALID_HANDLE_VALUE;
  char szDir[MAX_PATH];
  char *fargptr, *fext = NULL;
  int recurs = 0, fen = 2;
  DWORD dwError=0;
  
  if((argc < 2 || argc > 4) || ((argc > 3) && (streq_(argv[1],"-r"))==0) || (streq_(argv[1],"/?")))
  {
      printf("\nDHSC ListToCSV 1.1\nCopyright (c) 2018 DH's Software Creations\nMIT Licence\nTwitter @DHeadshot\n");
	  printf("Usage: %s [-r] <directory name> [<fileextension>]\n", argv[0]);
	  printf("\t-r\tGoes through the directories recursively within the path.  \n");
	  printf("\t\tWithout this option, it lists only files in the specified \n\t\tdirectory and not in subdirectories.\n");
	  printf("\t<fileextension>\tSpecify the file extension to only list files of \n\t\tthat type, e.g. \"pdf\" to just list PDF files.\n");
	  return (1);
  }
  if (streq_(argv[1],"-r"))
  {
    fargptr = argv[2];
	fen = 3;
	recurs = 1;
  }
  else fargptr = argv[1];
  if (argc>fen) fext = argv[fen];
  if (strlen(fargptr) > MAX_PATH - 3 || (fext && (strlen(fargptr) > MAX_PATH - 3 - strlen(fext))))
  {
    fprintf(stderr,"\nDirectory path is too long.\n");
    return (2);
  }
  strcpy(szDir, fargptr);
  strcat(szDir, "\\*");
  printf("FileName, Directory\n");
  dwError = ListFiles(szDir, fargptr, fext, recurs);
  return dwError;
}

DWORD ListFiles(char *szDir, char *fargptr, char *fext, int recurs)
{
  HANDLE hFind = INVALID_HANDLE_VALUE;
  DWORD dwError=0;
  WIN32_FIND_DATA ffd;
  hFind = FindFirstFile(szDir, &ffd);
  char chDir[MAX_PATH *2], nchDir[MAX_PATH *2];
  
  if (INVALID_HANDLE_VALUE == hFind) 
  {
    //DisplayErrorBox(TEXT("FindFirstFile"));
	fprintf(stderr,"\nError Finding File.\n");
    dwError = GetLastError();
    return dwError;
  } 
  long ic = 0;
  int changed = 0;
  char afn[MAX_PATH] = "\0";
  do
  {
	if (fext)
	{
	  strcpy(afn,".");
	  strcat(afn, fext);
	  if (endeq_(afn, ffd.cFileName, 0)) printf("%s, %s\n",ffd.cFileName, fargptr);
	}
	else printf("%s, %s\n",ffd.cFileName, fargptr);
	
	if (recurs && (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (ffd.cFileName[0] != '.'))
	{
	  strcpy(chDir,fargptr);
	  strcat(chDir,"\\");
	  strcat(chDir, ffd.cFileName);
	  strcpy(nchDir,chDir);
	  strcat(nchDir,"\\*");
	  dwError = ListFiles(nchDir,chDir, fext, recurs);
	  if (dwError != ERROR_NO_MORE_FILES) 
	  {
	    fprintf(stderr, "Error recursing in %s!\n",chDir);
	  }
	}
  } while (FindNextFile(hFind, &ffd) != 0);
  dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES) 
  {
     fprintf(stderr, "Error finding files!\n");
  }
  
  FindClose(hFind);
  return dwError;
}
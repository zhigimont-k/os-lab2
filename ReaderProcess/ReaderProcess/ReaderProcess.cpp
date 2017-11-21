#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

int _tmain()
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		printf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		printf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	HANDLE hFileCopy = CreateFile((LPCSTR)"D://copy.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFileCopy == INVALID_HANDLE_VALUE) {
		printf(TEXT("Could not create file copy (%d).\n"),
			GetLastError());
		return 1;
	}

	UnmapViewOfFile(pBuf);
	ReleaseMutex(mutex);
	CloseHandle(hMapFile);

	return 0;
}

#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>

using namespace std;

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[] = TEXT("Message from first process.");


int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
	HANDLE hFile = CreateFile((LPCSTR)"D://test.txt", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "CreateFile failed with error" << GetLastError() << endl;
	}
	else cout << "CreateFile succeed" << endl;
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	hMapFile = CreateFileMapping(
		hFile,    // use paging file
		NULL,                    // default security
		PAGE_READONLY,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}


	CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

	UnmapViewOfFile(pBuf);
	ReleaseMutex(mutex);
	CloseHandle(hMapFile);


	getchar();
	return 0;
}

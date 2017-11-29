#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <strsafe.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

using namespace std;

int _tmain()
{
	HANDLE hFileMapping;
	LPCTSTR pBuf;
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	__try {
		hFileMapping = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			szName);               // name of mapping object

		if (hFileMapping == NULL)
		{
			cerr << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "ReaderProcess: OpenFileMapping success" << endl;
	

	pBuf = (LPCTSTR)MapViewOfFile(hFileMapping, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		0);
	CloseHandle(hFileMapping);

	if (pBuf == NULL)
	{
		cerr << "ReaderProcess: MapViewOfFile failed with error " << GetLastError() << endl;
		UnmapViewOfFile(pBuf);
		__leave;
	}

	HANDLE hFileCopy = CreateFile(L"D://copy.docx", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFileCopy == INVALID_HANDLE_VALUE) {
		cout << "Could not create file copy (%d). Finished with error " << GetLastError() << endl;
	}
	else cout << "ReaderProcess: CreateFileCopy success" << endl;

	HANDLE hFile = CreateFile(L"D://test.docx", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "ReaderProcess: CreateFile failed with error " << GetLastError() << endl;
		__leave;
	}
	else cout << "ReaderProcess: CreateFile success" << endl;
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "ReaderProcess: GetFileSize failed with error" << GetLastError() << endl;
		__leave;
	}
	CloseHandle(hFile);

	//bool success = WriteFile(hFileCopy, pBuf, strlen((const char*)pBuf) * sizeof(unsigned char*), 0, 0);
	char buffer[200];
	memcpy(buffer, pBuf, 100);

	bool success = WriteFile(hFileCopy, buffer, 100, 0, 0);
	if (!success) {
		cerr << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
		__leave;
	}
	else cout << "ReaderProcess: WriteFile success" << endl;
	CloseHandle(hFileCopy);
	}

	__finally {

	ReleaseMutex(mutex);

	getchar();
	return 0;
	}
}

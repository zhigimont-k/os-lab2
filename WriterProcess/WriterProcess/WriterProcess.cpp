#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>

using namespace std;

//#define BUF_SIZE 256

TCHAR szName[] = TEXT("MyFileMappingObject");

using namespace std;

int _tmain()
{
	HANDLE hMapFile;
	unsigned char* pBuf;
	HANDLE hFile = CreateFile(L"D://test.docx", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "CreateFile failed with error " << GetLastError() << endl;
	}
	else cout << "CreateFile succeeded" << endl;
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "GetFileSize failed with error" << GetLastError() << endl;
		//return nullptr;
	}
	else cout << "WriterProcess: GetFileSize success" << endl;

	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	hMapFile = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, 0,
		szName);
	if (hMapFile == nullptr) {
		cerr << "CreateFileMapping failed with error" << GetLastError() << endl;
		//return 0;
	}
	else cout << "CreateWriterProcess: CreateFileMapping success" << endl;
	CloseHandle(hFile);

	pBuf = (unsigned char*)MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		dwFileSize);
	if (pBuf == nullptr) {
		cerr << "MapViewOfFile failed with error" << GetLastError() << endl;
		//CloseHandle(hMapFile);
		//return nullptr;
	}
	else cout << "WriterProcess: MapViewOfFile success" << endl;


	//cout << "pBuf: " << pBuf << endl;

	UnmapViewOfFile(pBuf);
	//CloseHandle(hMapFile);
	//CloseHandle(hFile);
	ReleaseMutex(mutex);


	getchar();
	return 0;
}



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
TCHAR szMsg[] = TEXT("Message from first process.");

using namespace std;

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hMapFile;
	unsigned char* pBuf;
	HANDLE hFile = CreateFile(L"D://test.txt", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "CreateFile failed with error " << GetLastError() << endl;
	}
	else cout << "CreateFile succeeded" << endl;
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "GetFileSize failed with error" << GetLastError() << endl;
		CloseHandle(hFile);
		//return nullptr;
	}
	else cout << "WriterProcess: GetFileSize success" << endl;

	hMapFile = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, 0,
		szName);
	if (hMapFile == nullptr) {
		cerr << "CreateFileMapping failed with error" << GetLastError() << endl;
		CloseHandle(hFile);
		//return 0;
	}
	else cout << "CreateWriterProcess: CreateFileMapping success" << endl;

	pBuf = (unsigned char*)MapViewOfFile(hMapFile,
		FILE_MAP_READ,
		0,
		0,
		dwFileSize);
	if (pBuf == nullptr) {
		cerr << "MapViewOfFile failed with error" << GetLastError() << endl;
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		//return nullptr;
	}
	else cout << "WriterProcess: MapViewOfFile success" << endl;


	//cout << "pBuf: " << pBuf << endl;

	UnmapViewOfFile(pBuf);
	//CloseHandle(hMapFile);
	CloseHandle(hFile);
	ReleaseMutex(mutex);


	getchar();
	return 0;
}



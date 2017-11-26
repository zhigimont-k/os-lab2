#include "stdafx.h"
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
TCHAR szName[] = TEXT("MyFileMappingObject");


using namespace std;

void runWriterProcedure() {
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
}

int _tmain()
{

	//runWriterProcedure();
	HANDLE hMapFile;
	unsigned char* pBuf;
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		cerr << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
	}
	else cout << "ReaderProcess: OpenFileMapping success" << endl;

	pBuf = (unsigned char*)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		cerr << "ReaderProcess: MapViewOfFile failed with error " << GetLastError() << endl;
		CloseHandle(hMapFile);
	}

	//MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	HANDLE hFileCopy = CreateFile(L"D://copy.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFileCopy == INVALID_HANDLE_VALUE) {
		cout << "Could not create file copy (%d). Finished with error " << GetLastError() << endl;
	}
	else cout << "ReaderProcess: CreateFileCopy success" << endl;

	//bool success = WriteFile(hFileCopy, pBuf, strlen((const char*)pBuf) * sizeof(unsigned char*), 0, 0);
	bool success = WriteFile(hFileCopy, pBuf, strlen((const char*)pBuf), 0, 0);
	if (!success) {
		cerr << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
		//return;
	}
	else cout << "ReaderProcess: WriteFile success" << endl;
	CloseHandle(hFileCopy);

	UnmapViewOfFile(pBuf);
	ReleaseMutex(mutex);
	CloseHandle(hMapFile);

	getchar();
	return 0;
}

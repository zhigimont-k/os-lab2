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
TCHAR szName[] = TEXT("MyFileMappingObject");


using namespace std;


void runWriter() {
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

	//HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
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
	//ReleaseMutex(mutex);

}

int _tmain()
{

	//runWriter();
	HANDLE hFileMapping;
	unsigned char* pBuf;
	//HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	hFileMapping = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hFileMapping == NULL)
	{
		cerr << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
	}
	else cout << "ReaderProcess: OpenFileMapping success" << endl;

	pBuf = (unsigned char*)MapViewOfFile(hFileMapping, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		cerr << "ReaderProcess: MapViewOfFile failed with error " << GetLastError() << endl;
		CloseHandle(hFileMapping);
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
	}
	else cout << "ReaderProcess: CreateFile succeeded" << endl;
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "ReaderProcess: GetFileSize failed with error" << GetLastError() << endl;
		//return nullptr;
	}
	CloseHandle(hFile);

	//bool success = WriteFile(hFileCopy, pBuf, strlen((const char*)pBuf) * sizeof(unsigned char*), 0, 0);
	bool success = WriteFile(hFileCopy, pBuf, (size_t)dwFileSize, 0, 0);
	if (!success) {
		cerr << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
		//return;
	}
	else cout << "ReaderProcess: WriteFile success" << endl;
	CloseHandle(hFileCopy);

	UnmapViewOfFile(pBuf);
	//ReleaseMutex(mutex);
	CloseHandle(hFileMapping);

	getchar();
	return 0;
}

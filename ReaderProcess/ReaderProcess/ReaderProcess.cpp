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

int _tmain(int argc, TCHAR *argv[])
{
	int bytesToWrite = 128;
	HANDLE hFileMapping;
	LPCTSTR pBuf; 
	HANDLE mutex;
	DWORD dBytesWritten;  // number of bytes written
	DWORD dwFileSize;     // temporary storage for file sizes
	DWORD dwFileMapSize;  // size of the file mapping
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view
	DWORD dwSysGran;      // system allocation granularity
	SYSTEM_INFO SysInfo;  // system information; used to get granularity
	LPVOID lpMapAddress;  // pointer to the base address of the
						  // memory-mapped region
	char * pData;         // pointer to the data
	LPCTSTR iData;            // on success contains the first int of data
	int iViewDelta;       // the offset into the view where the data
						  //shows up
	int fileMapStart = 0;

	__try {

		if (argc == 1) { cout << "Couldn't retrieve command line arguments" << endl; __leave; }


		HANDLE hFileCopy = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileCopy == INVALID_HANDLE_VALUE) {
			cout << "Could not create file copy (%d). Finished with error " << GetLastError() << endl;
		}
		else cout << "ReaderProcess: CreateFileCopy success" << endl;

		HANDLE hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			cerr << "ReaderProcess: CreateFile failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "ReaderProcess: CreateFile success" << endl;
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) {
			cerr << "ReaderProcess: GetFileSize failed with error " << GetLastError() << endl;
			__leave;
		}
		cout << "dwFileSize: " << dwFileSize << endl;
		CloseHandle(hFile);

		GetSystemInfo(&SysInfo);
		dwSysGran = SysInfo.dwAllocationGranularity;
		dwFileMapStart = 0;

		while (dwFileMapStart < dwFileSize) {
			//dwFileMapStart = (fileMapStart / dwSysGran) * dwSysGran;
			//dwMapViewSize = (fileMapStart % dwSysGran) + BUF_SIZE;
			if (dwFileMapStart + bytesToWrite > dwFileSize) {
				bytesToWrite = dwFileSize - dwFileMapStart;
			}
			//dwMapViewSize = (fileMapStart % dwSysGran) + BUF_SIZE;
			dwMapViewSize = BUF_SIZE;
			if (dwMapViewSize > dwFileSize - dwFileMapStart) {
				dwMapViewSize = dwFileSize - dwFileMapStart;
			}

			// How large will the file mapping object be?
			dwFileMapSize = BUF_SIZE;
			if (dwFileMapSize > dwFileSize - dwFileMapStart) {
				dwFileMapSize = dwFileSize - dwFileMapStart;
			}

			// The data of interest isn't at the beginning of the
			// view, so determine how far into the view to set the pointer.
			iViewDelta = fileMapStart - dwFileMapStart;

			mutex = CreateMutex(0, TRUE, _T("Global\\mutex"));
			if (GetLastError() == 0) {
				cout << "ReaderProcess: CreateMutex success" << endl;
				ReleaseMutex(mutex);
			}

			mutex = OpenMutex(MUTEX_ALL_ACCESS, false, _T("Global\\mutex"));
			if (GetLastError() == 0) {
				cout << "ReaderProcess: OpenMutex success" << endl;
			}

			hFileMapping = OpenFileMapping(
				FILE_MAP_ALL_ACCESS,   // read/write access
				FALSE,                 // do not inherit the name
				szName);               // name of mapping object

			if (hFileMapping == NULL)
			{
				do {
					hFileMapping = OpenFileMapping(
						FILE_MAP_ALL_ACCESS,
						FALSE,
						szName);
				} while (hFileMapping == NULL);
			}
			else cout << "ReaderProcess: OpenFileMapping success" << endl;


			pBuf = (LPCTSTR)MapViewOfFile(hFileMapping, // handle to map object
				FILE_MAP_ALL_ACCESS,  // read/write permission
				0,
				dwFileMapStart,
				dwMapViewSize);

			if (pBuf == NULL)
			{
				cerr << "ReaderProcess: MapViewOfFile failed with error " << GetLastError() << endl;
				UnmapViewOfFile(pBuf);
				__leave;
			}

			//pData = (char *)pBuf + iViewDelta;
			//iData = *(LPCTSTR *)pData;



			char buffer[BUF_SIZE];
			memcpy(buffer, pBuf, bytesToWrite);


			DWORD dwPtr = SetFilePointer(hFileCopy, 0, NULL, FILE_END); //set pointer position to the end of the file
			bool success = WriteFile(hFileCopy, buffer, bytesToWrite, &dwPtr, 0);
			if (!success) {
				cerr << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
				__leave;
			}
			else cout << "ReaderProcess: WriteFile success" << endl;

			ReleaseMutex(mutex);
			UnmapViewOfFile(pBuf);
			dwFileMapStart += bytesToWrite;
			cout << "fileMapStart: " << fileMapStart << endl;
			cout << "(int)dwFileSize: " << (int)dwFileSize << endl;

		}



		
	//CloseHandle(mutex);
	//CloseHandle(hFileMapping);
	CloseHandle(hFileCopy);
	}

	__finally {


	getchar();
	return 0;
	}
}

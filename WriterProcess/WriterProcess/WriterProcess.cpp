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

#define BUF_SIZE 256

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
//TCHAR globalMutex[] = TEXT("Global\\globalMutex");

using namespace std;

int _tmain(int argc, TCHAR *argv[])
{

	int bytesToWrite = 128;
	HANDLE hMapFile;
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

		if (argc == 1) {
			cout << "Couldn't retrieve command line arguments" << endl;
			__leave;
		}

		HANDLE hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			cerr << "CreateFile failed with error " << GetLastError() << endl;
			CloseHandle(hFile);
			__leave;
		}
		else cout << "WriterProcess: CreateFile success" << endl;
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE || dwFileSize == 0) {
			cerr << "GetFileSize failed with error" << GetLastError() << endl;
			CloseHandle(hFile);
			__leave;
		}
		else cout << "WriterProcess: GetFileSize success" << endl;


		GetSystemInfo(&SysInfo);
		dwSysGran = SysInfo.dwAllocationGranularity;
		dwFileMapStart = 0;

		while (dwFileMapStart <= dwFileSize) {
			if (dwFileMapStart > dwFileSize - bytesToWrite) {
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


			mutex = CreateMutex(NULL, TRUE, _T("Global\\mutex"));
			if (GetLastError() == 0) {
				cout << "WriterProcess: CreateMutex success" << endl;
			}
			if (GetLastError() != 0) {
				mutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, _T("Global\\mutex"));
				cout << "WriterProcess: OpenMutex success" << endl;
			}



			hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0,
				szName);
			if (GetLastError() != 0) {
				hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
				if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE) {
					do {
						hMapFile = OpenFileMapping(
							FILE_MAP_ALL_ACCESS,
							FALSE,
							szName);
					} while (hMapFile == NULL);
				}

			}
			else cout << "WriterProcess: CreateFileMapping success" << endl;
			CloseHandle(hFile);

			pBuf = (LPCTSTR)MapViewOfFile(hMapFile,
				FILE_MAP_ALL_ACCESS,
				0,
				dwFileMapStart,
				dwMapViewSize);
			if (pBuf == NULL) {
				cerr << "WriterProcess: MapViewOfFile failed with error" << GetLastError() << endl;
				CloseHandle(hMapFile);
				__leave;
			}
			else cout << "WriterProcess: MapViewOfFile success" << endl;

			ReleaseMutex(mutex);
			//_getch();
			UnmapViewOfFile(pBuf);
			//CloseHandle(hMapFile); 
			dwFileMapStart += bytesToWrite;
			cout << "fileMapStart: " << fileMapStart << endl;
		}

	}

	__finally {
		getchar();
		return 0;
	}

}



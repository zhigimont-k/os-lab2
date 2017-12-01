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
	HANDLE hMapFile;
	LPCTSTR pBuf;
	HANDLE mutex;
	__try {

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
					cerr << "OpenFileMapping failed with error" << GetLastError() << endl;
					CloseHandle(hMapFile);
					__leave;
			}
			
			}
			else cout << "WriterProcess: CreateFileMapping success" << endl;
			CloseHandle(hFile);

			pBuf = (LPCTSTR)MapViewOfFile(hMapFile,
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				dwFileSize);
			if (pBuf == NULL) {
				cerr << "MapViewOfFile failed with error" << GetLastError() << endl;
				CloseHandle(hMapFile);
				__leave;
			}
			else cout << "WriterProcess: MapViewOfFile success" << endl;

			ReleaseMutex(mutex);
			_getch();
			UnmapViewOfFile(pBuf);
			//CloseHandle(hMapFile);

	}

	__finally {
		getchar();
		return 0;
	}

}



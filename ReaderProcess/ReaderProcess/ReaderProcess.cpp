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

#define BUF_SIZE 4096
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR mutexName[] = TEXT("Global\\mutexwithuniquename87458u568u45y69546");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR szMsg[];

using namespace std;

int _tmain(int argc, TCHAR *argv[])
{
	int bytesToWrite = BUF_SIZE;
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
	//int fileMapStart = 0;

	__try {

		if (argc == 1) { cout << "Couldn't retrieve command line arguments" << endl; __leave; }

		mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
		if (mutex == NULL) {
			cout << "ReaderProcess: OpenMutex failed with error " << GetLastError() << endl;
			__leave;
		}
		cout << "ReaderProcess: OpenMutex success" << endl;


		HANDLE hFileCopy = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
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
		int fileMapStart = 0; 
		
		/*hFileMapping = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			szName);               // name of mapping object

		if (hFileMapping == NULL)
		{
			cout << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "ReaderProcess: OpenFileMapping success" << endl;*/

		HANDLE hSharedMemory = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			sharedMemoryName);               // name of mapping object

		if (hSharedMemory == NULL)
		{
			cout << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "ReaderProcess: OpenFileMapping success" << endl;


		while (dwFileMapStart < dwFileSize) {
			DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
			switch (dwWaitResult)
			{
				// The thread got ownership of the mutex
			case WAIT_OBJECT_0:
				__try {
					cout << "ReaderProcess: current owner of mutex" << endl;

					//dwFileMapStart = (fileMapStart / dwSysGran) * dwSysGran;
					if (dwFileMapStart + bytesToWrite > dwFileSize) {
						bytesToWrite = dwFileSize - dwFileMapStart;
					}
					cout << "Bytes to write: " << bytesToWrite << endl;
					//dwMapViewSize = (fileMapStart % dwSysGran) + BUF_SIZE;
					dwMapViewSize = BUF_SIZE;
					if (dwMapViewSize > dwFileSize - dwFileMapStart) {
						dwMapViewSize = dwFileSize - dwFileMapStart;
					}

					/*else {
					while (GetLastError() != 0) {
					mutex = OpenMutex(MUTEX_ALL_ACCESS, false, _T("Global\\mutex")); 
					}
					}*/

					pBuf = (LPCTSTR)MapViewOfFile(hSharedMemory, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						dwMapViewSize);

					if (pBuf == NULL)
					{
						cerr << "ReaderProcess: buffer is empry, waiting for WriterProcess" << endl;
						ReleaseMutex(mutex);
						WaitForSingleObject(mutex, INFINITE);
						continue;
					}

					char buffer[BUF_SIZE];
					memcpy(buffer, pBuf, bytesToWrite);
					cout << buffer << endl;


					DWORD dwPtr = SetFilePointer(hFileCopy, 0, NULL, FILE_END); //set pointer position to the end of the file
					bool success = WriteFile(hFileCopy, buffer, bytesToWrite, &dwPtr, 0);
					if (!success) {
						cerr << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
						__leave;
					}
					else cout << "ReaderProcess: WriteFile success" << endl;

					UnmapViewOfFile(pBuf);
					cout << "dwFileMapStart: " << dwFileMapStart << endl;
					cout << "bytesToWrite: " << bytesToWrite << endl;
					dwFileMapStart += bytesToWrite;
				}

				__finally {
					ReleaseMutex(mutex);
					if (GetLastError() != 0) {
						cout << "ReaderProcess: ReleaseMutex failed with error " << GetLastError() << endl;
					}

				}
				break;

				// The thread got ownership of an abandoned mutex
				// The database is in an indeterminate state
			case WAIT_ABANDONED: {
				cout << "WAIT_ABANDONED" << endl;
				return FALSE;
			}
			case WAIT_FAILED: {
				cout << "WAIT_FAILED, error "<< GetLastError() << endl;
				return FALSE;
			}


			case WAIT_TIMEOUT: {
				cout << "WAIT_TIMEOUT" << endl;
				return FALSE;
			}
			}
		}




			

		}

	__finally {


	getchar();
	return 0;
	}
}

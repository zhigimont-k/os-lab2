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

#define BUF_SIZE 4096

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR mutexName[] = TEXT("Global\\mutexwithuniquename87458u568u45y69546");
//TCHAR globalMutex[] = TEXT("Global\\globalMutex");


using namespace std;

int _tmain(int argc, TCHAR *argv[])
{

	int bytesToWrite = BUF_SIZE;
	HANDLE hMapFile;
	LPCTSTR pBuf;
	LPCTSTR pBufShared;
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
	__try {

		if (argc == 1) {
			cout << "Couldn't retrieve command line arguments" << endl;
			__leave;
		}

		mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
		if (mutex == NULL) {
			cout << "WriterProcess: OpenMutex failed with error " << GetLastError() << endl;
			__leave;
		}
		cout << "WriterProcess: OpenMutex success" << endl;


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
		//int fileMapStart = 0;
		
		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
		if (hMapFile == INVALID_HANDLE_VALUE || hMapFile == NULL) {
			cout << "WriterProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "WriterProcess: OpenFileMapping success" << endl;

		HANDLE hSharedMemory = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			sharedMemoryName);               // name of mapping object

		if (hSharedMemory == NULL)
		{
			cout << "WriterProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else cout << "WriterProcess: OpenFileMapping success" << endl;


		while (dwFileMapStart < dwFileSize) {
				DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
				switch (dwWaitResult)
				{
					// The thread got ownership of the mutex
				case WAIT_OBJECT_0:
					__try {
						cout << "WriterProcess: current owner of mutex" << endl;
						
						//dwFileMapStart = (fileMapStart / dwSysGran) * dwSysGran;

						if (dwFileMapStart + bytesToWrite > dwFileSize) {
							bytesToWrite = dwFileSize - dwFileMapStart;
						}
						//dwMapViewSize = (fileMapStart % dwSysGran) + BUF_SIZE;
						dwMapViewSize = BUF_SIZE;
						if (dwMapViewSize > dwFileSize - dwFileMapStart) {
							dwMapViewSize = dwFileSize - dwFileMapStart;
						}

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

						pBufShared = (LPCTSTR)MapViewOfFile(hSharedMemory,
							FILE_MAP_ALL_ACCESS,
							0,
							0,
							dwMapViewSize);
						if (pBufShared == NULL) {
							cerr << "WriterProcess: MapViewOfFile failed with error" << GetLastError() << endl;
							CloseHandle(hMapFile);
							__leave;
						}
						else cout << "WriterProcess: MapViewOfFile success" << endl;

						//write to shared resource

						char buffer[BUF_SIZE];
						memcpy(buffer, pBuf, bytesToWrite);

						cout << "buffer: " << buffer << endl;
						memcpy((PVOID)pBufShared, buffer, bytesToWrite);


						UnmapViewOfFile(pBuf);
						cout << "dwFileMapStart: " << dwFileMapStart << endl;
						cout << "bytesToWrite: " << bytesToWrite << endl;
						dwFileMapStart += bytesToWrite;
					}

					__finally {
						ReleaseMutex(mutex);
						if (GetLastError() != 0) {
							cout << "WriterProcess: ReleaseMutex failed with error " << GetLastError() << endl;
							__leave;
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
					cout << "WAIT_FAILED, error " << GetLastError() << endl;
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



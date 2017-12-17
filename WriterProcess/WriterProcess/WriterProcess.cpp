#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <chrono>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>

using namespace std;
#define BUF_SIZE 4096

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR readingFromMemoryProcessing[] = TEXT("Global\\readingFromMemoryProcessingEvent4589tu4efr");
TCHAR writingToMemoryProcessing[] = TEXT("Global\\writingToMemoryProcessingEvent4589tu4efr");


using namespace std;

int _tmain(int argc, TCHAR *argv[])
{

	int bytesToWrite = BUF_SIZE;
	HANDLE hMapFile;
	LPCTSTR pBuf;
	LPCTSTR pBufShared;
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view

	ofstream logfile;
	logfile.open("writerLog.txt");
	__try {

		if (argc == 1) {
			logfile << "Couldn't retrieve command line arguments" << endl;
			__leave;
		}

		HANDLE writeEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, writingToMemoryProcessing);
		HANDLE readEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, readingFromMemoryProcessing);

		if (writeEvent == NULL || readEvent == NULL) {
			logfile << "WriterProcess: OpenEvent failed with error " << GetLastError() << endl;
			__leave;
		}
		else { 
			logfile << "WriterProcess: OpenEvent success" << endl;
		}

		HANDLE hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			logfile << "CreateFile failed with error " << GetLastError() << endl;
			CloseHandle(hFile);
			__leave;
		}
		else logfile << "WriterProcess: CreateFile success" << endl;
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE || dwFileSize == 0) {
			logfile << "GetFileSize failed with error" << GetLastError() << endl;
			CloseHandle(hFile);
			__leave;
		}
		else logfile << "WriterProcess: GetFileSize success" << endl;

		dwFileMapStart = 0;
		
		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
		if (hMapFile == INVALID_HANDLE_VALUE || hMapFile == NULL) {
			logfile << "WriterProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else logfile << "WriterProcess: OpenFileMapping success" << endl;

		HANDLE hSharedMemory = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			sharedMemoryName);               // name of mapping object

		if (hSharedMemory == NULL)
		{
			logfile << "WriterProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else logfile << "WriterProcess: OpenFileMapping success" << endl;

		
		while (dwFileMapStart < dwFileSize) {
				DWORD dwWaitResult = WaitForSingleObject(writeEvent, INFINITE);
				switch (dwWaitResult)
				{
					// The thread got ownership of the mutex
				case WAIT_OBJECT_0:
					__try {
						logfile << "WriterProcess: active" << endl;
						SYSTEMTIME st;

						GetSystemTime(&st);
						logfile << "The system time is: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl;
						if (dwFileMapStart + bytesToWrite > dwFileSize) {
							bytesToWrite = dwFileSize - dwFileMapStart;
						}
						dwMapViewSize = BUF_SIZE;
						if (dwMapViewSize > dwFileSize - dwFileMapStart) {
							dwMapViewSize = dwFileSize - dwFileMapStart;
						}


						pBuf = (LPCTSTR)MapViewOfFile(hMapFile,
							FILE_MAP_ALL_ACCESS,
							0,
							0,
							dwFileSize);
						if (pBuf == NULL) {
							logfile << "WriterProcess: MapViewOfFile failed with error" << GetLastError() << endl;
							CloseHandle(hMapFile);
							__leave;
						}
						else logfile << "WriterProcess: MapViewOfFile success" << endl;


						pBufShared = (LPCTSTR)MapViewOfFile(hSharedMemory,
							FILE_MAP_ALL_ACCESS,
							0,
							0,
							dwMapViewSize);
						if (pBufShared == NULL) {
							logfile << "WriterProcess: MapViewOfFile failed with error " << GetLastError() << endl;
							CloseHandle(hMapFile);
							__leave;
						}
						else logfile << "WriterProcess: MapViewOfFile success" << endl;

						//write to shared resource

						unsigned char buffer[BUF_SIZE];
						memcpy(buffer, (unsigned char*)pBuf + dwFileMapStart, bytesToWrite);

						memcpy((PVOID)pBufShared, buffer, bytesToWrite);


						UnmapViewOfFile(pBuf);
						//UnmapViewOfFile(pBufShared);
						logfile << "dwFileMapStart: " << dwFileMapStart << endl;
						logfile << "bytesToWrite: " << bytesToWrite << endl;
						dwFileMapStart += bytesToWrite;
					}

					__finally {
						ResetEvent(writeEvent);
						SetEvent(readEvent);
						logfile << "WriterProcess: Reset events" << endl;
						SYSTEMTIME st;

						GetSystemTime(&st);
						logfile << "The system time is: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl;
						if (GetLastError() != 0) {
							__leave;
						}

					}
					break;
				case WAIT_ABANDONED: {
					logfile << "WAIT_ABANDONED" << endl;
					return FALSE;
				}

				case WAIT_FAILED: {
					logfile << "WAIT_FAILED, error " << GetLastError() << endl;
					return FALSE;
				}

				case WAIT_TIMEOUT: {
					logfile << "WAIT_TIMEOUT" << endl;
					return FALSE;
				}
				}
		}
}

	__finally {
		logfile.close();
		getchar();
		return 0;
	}

}



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
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR readingFromMemoryProcessing[] = TEXT("Global\\readingFromMemoryProcessingEvent4589tu4efr");
TCHAR writingToMemoryProcessing[] = TEXT("Global\\writingToMemoryProcessingEvent4589tu4efr");

using namespace std;

int _tmain(int argc, TCHAR *argv[])
{
	int bytesToWrite = BUF_SIZE;
	LPCTSTR pBuf;
	HANDLE hFileCopy;
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view
	ofstream logfile;
	logfile.open("readerLog.txt");

	__try {

		if (argc == 1) { logfile << "Couldn't retrieve command line arguments" << endl; __leave; }
		
		HANDLE writeEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, writingToMemoryProcessing);
		HANDLE readEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, readingFromMemoryProcessing);


		if (writeEvent == NULL || readEvent == NULL) {
			logfile << "WriterProcess: OpenEvent failed with error " << GetLastError() << endl;
			__leave;
		}
		else {
			logfile << "WriterProcess: OpenEvent success" << endl;
		}
		
		hFileCopy = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileCopy == INVALID_HANDLE_VALUE) {
			logfile << "Could not create file copy (%d). Finished with error " << GetLastError() << endl;
		}
		else logfile << "ReaderProcess: CreateFileCopy success" << endl;

		HANDLE hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			logfile << "ReaderProcess: CreateFile failed with error " << GetLastError() << endl;
			__leave;
		}
		else logfile << "ReaderProcess: CreateFile success" << endl;
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) {
			logfile << "ReaderProcess: GetFileSize failed with error " << GetLastError() << endl;
			__leave;
		}
		logfile << "dwFileSize: " << dwFileSize << endl;
		CloseHandle(hFile);

		dwFileMapStart = 0;
		int fileMapStart = 0; 
		

		HANDLE hSharedMemory = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			sharedMemoryName);               // name of mapping object


		if (hSharedMemory == NULL)
		{
			logfile << "ReaderProcess: OpenFileMapping failed with error " << GetLastError() << endl;
			__leave;
		}
		else logfile << "ReaderProcess: OpenFileMapping success" << endl;

		
		while (dwFileMapStart < dwFileSize) {
			DWORD dwWaitResult = WaitForSingleObject(readEvent, INFINITE);
			switch (dwWaitResult)
			{
			case WAIT_OBJECT_0:
				__try {
					logfile << "ReaderProcess: active" << endl;
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

					pBuf = (LPCTSTR)MapViewOfFile(hSharedMemory, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						dwMapViewSize);

					unsigned char* buffer = (unsigned char*)pBuf;
					DWORD dwPtr = SetFilePointer(hFileCopy, 0, NULL, FILE_END); //set pointer position to the end of the file
					bool success = WriteFile(hFileCopy, buffer, bytesToWrite, &dwPtr, 0);
					if (!success) {
						logfile << "ReaderProcess: WriteFile failed with error " << GetLastError() << endl;
						__leave;
					}
					else logfile << "ReaderProcess: WriteFile success" << endl;

					UnmapViewOfFile(pBuf);
					logfile << "dwFileMapStart: " << dwFileMapStart << endl;
					logfile << "bytesToWrite: " << bytesToWrite << endl;
					dwFileMapStart += bytesToWrite;
				}

				__finally {

					ResetEvent(readEvent);
					SetEvent(writeEvent);
					logfile << "ReaderProcess: Reset events" << endl;
					SYSTEMTIME st;

					GetSystemTime(&st);
					logfile << "The system time is: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl;
				}
				break;

			case WAIT_ABANDONED: {
				logfile << "WAIT_ABANDONED" << endl;
				return FALSE;
			}
			case WAIT_FAILED: {
				logfile << "WAIT_FAILED, error "<< GetLastError() << endl;
				return FALSE;
			}

			case WAIT_TIMEOUT: {
				logfile << "WAIT_TIMEOUT" << endl;
				return FALSE;
			}
			}
		}
		CloseHandle(hFileCopy);
}

	__finally {


	getchar();
	return 0;
	}
}

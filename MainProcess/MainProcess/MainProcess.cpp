#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <strsafe.h>
#include <conio.h>

#define BUF_SIZE 4096

using namespace std;
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR writerPath[] = TEXT("..\\..\\WriterProcess\\Debug\\WriterProcess.exe");
TCHAR readerPath[] = TEXT("..\\..\\ReaderProcess\\Debug\\ReaderProcess.exe");

TCHAR readingFromMemoryProcessing[] = TEXT("Global\\readingFromMemoryProcessingEvent4589tu4efr");
TCHAR writingToMemoryProcessing[] = TEXT("Global\\writingToMemoryProcessingEvent4589tu4efr");

int _tmain(int argc, TCHAR *argv[])
{
	PROCESS_INFORMATION piWriter, piReader;
	STARTUPINFO si = { sizeof(si) };
	STARTUPINFO siWriter = { sizeof(siWriter) };
	STARTUPINFO siReader = { sizeof(siReader) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	ZeroMemory(&si, sizeof(si));
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;
	ofstream logfile;
	logfile.open("..\\..\\mainLog.txt");

	if (argc < 3) {
		logfile << "Not enough arguments" << endl;
		//return 0;
	}

	logfile << "->Start of parent execution." << endl;

	// EVENTS CREATION
	HANDLE events[2];
	events[0] = CreateEvent(NULL, TRUE, TRUE, writingToMemoryProcessing);
	events[1] = CreateEvent(NULL, TRUE, FALSE, readingFromMemoryProcessing);
	if (events[0] == INVALID_HANDLE_VALUE || events[1] == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: CreateEvent failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateEvent success" << endl;
	events[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, writingToMemoryProcessing);
	events[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, readingFromMemoryProcessing);
	if (events[0] == INVALID_HANDLE_VALUE || events[1] == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: OpenEvent failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: OpenEvent success" << endl;

	HANDLE hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: CreateFile failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFile success" << endl;

	DWORD dwFileSize = GetFileSize(hFile, nullptr);

	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0,
		szName);
	if (hMapFile == NULL) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE,
		sharedMemoryName);
	if (hSharedMemory == NULL) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);
	if (hMapFile == NULL) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;
	
	if (!CreateProcess(NULL, writerPath, &saProcess, &saThread, FALSE, CREATE_NO_WINDOW, NULL, NULL, &siWriter, &piWriter) ||
		!CreateProcess(NULL, readerPath, &saProcess, &saThread, FALSE, CREATE_NO_WINDOW, NULL, NULL, &siReader, &piReader)) {
		logfile << "CreateChildProcesses failed" << endl;
	}
	else {
		logfile << "CreateWriterProcess success" << endl << "CreateReaderProcess success" << endl;
	}




	DWORD dwWaitResult = WaitForMultipleObjects(2, events, TRUE, INFINITE);
	switch (dwWaitResult) {
	case WAIT_OBJECT_0: {
		logfile << "MainProcess: child processes finished successfully" << endl;
		cout << "Done!" << endl;
	}
	}
	logfile.close();
	CloseHandle(hMapFile);
	CloseHandle(hSharedMemory);
	CloseHandle(piWriter.hProcess);
	CloseHandle(piWriter.hThread);
	CloseHandle(piReader.hProcess);
	CloseHandle(piReader.hThread);

	logfile << "->End of parent execution." << endl;
	getchar();
	return 0;
}
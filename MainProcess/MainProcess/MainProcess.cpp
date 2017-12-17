#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>

#define BUF_SIZE 4096

using namespace std;
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");

TCHAR readingFromMemoryProcessing[] = TEXT("Global\\readingFromMemoryProcessingEvent4589tu4efr");
TCHAR writerPath[] = TEXT("..\\..\\writerProcess.bat");
TCHAR readerPath[] = TEXT("..\\..\\readerProcess.bat");
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
	logfile.open("mainLog.txt");

	if (argc < 3) {
		logfile << "Not enough arguments" << endl;
	}

	TCHAR *fileNameArgs[2];
	fileNameArgs[0] = argv[1];
	fileNameArgs[1] = argv[2];
	TCHAR *readerArgs;
	wsprintf(readerArgs, fileNameArgs[0], _T(" "), fileNameArgs[1]);
	cout << readerArgs << endl;

	logfile << "->Start of parent execution." << endl;
	
	HANDLE hWriterEvent = CreateEvent(NULL, TRUE, TRUE, writingToMemoryProcessing);
	HANDLE hReaderEvent = CreateEvent(NULL, TRUE, FALSE, readingFromMemoryProcessing);
	if (hWriterEvent == INVALID_HANDLE_VALUE || hReaderEvent == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: CreateEvent failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateEvent success" << endl;
	hWriterEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, writingToMemoryProcessing);
	hReaderEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, readingFromMemoryProcessing);
	if (hWriterEvent == INVALID_HANDLE_VALUE || hReaderEvent == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: OpenEvent failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: OpenEvent success" << endl;

	HANDLE hFile = CreateFile(fileNameArgs[0], GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		logfile << "MainProcess: CreateFile failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFile success" << endl;

	DWORD dwFileSize = GetFileSize(hFile, nullptr);

	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0,
		szName);
	if (GetLastError() != 0) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE,
		sharedMemoryName);
	if (GetLastError() != 0) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);
	if (GetLastError() != 0) {
		logfile << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else logfile << "MainProcess: CreateFileMapping success" << endl;
	

	if ( !CreateProcess(writerPath, fileNameArgs[0], &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siWriter, &piWriter) ||
		!CreateProcess(readerPath, readerArgs, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siReader, &piReader)) {
		logfile << "CreateChildProcesses failed" << endl;
	}
	else { 
		logfile << "CreateWriterProcess success" << endl << "CreateReaderProcess success" << endl;
	}
	logfile << "->End of parent execution." << endl;

	logfile.close();
	CloseHandle(piWriter.hProcess);
	CloseHandle(piWriter.hThread);
	CloseHandle(piReader.hProcess);
	CloseHandle(piReader.hThread);

	getchar();
	return 0;
}


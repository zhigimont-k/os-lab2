#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <conio.h>

#define BUF_SIZE 4096

using namespace std;
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR mutexName[] = TEXT("Global\\mutexwithuniquename87458u568u45y69546");
TCHAR sharedMemoryName[] = TEXT("Global\\SharedMemory");
TCHAR pos[] = TEXT("Global\\positioninfilekjdnfkvjnskdcndncds");

TCHAR readingFromMemoryProcessing[] = TEXT("Global\\readingFromMemoryProcessingEvent4589tu4efr");
TCHAR writingToMemoryProcessing[] = TEXT("Global\\writingToMemoryProcessingEvent4589tu4efr");

int _tmain()
{
	PROCESS_INFORMATION pi, piWriter, piReader;
	HANDLE mutex;
	STARTUPINFO si = { sizeof(si) };
	STARTUPINFO siWriter = { sizeof(siWriter) };
	STARTUPINFO siReader = { sizeof(siReader) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	TCHAR szPath[] = TEXT("cmd.exe");
	ZeroMemory(&si, sizeof(si));
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	cout << "->Start of parent execution." << endl;
	boolean success = CreateProcess(NULL, szPath, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "MainProcess: CreateMainProcess failed with error" << GetLastError() << endl;
		cout << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateMainProcess success" << endl;

	mutex = CreateMutex(0, FALSE, mutexName);
	if (mutex == INVALID_HANDLE_VALUE || mutex == NULL) {
		cout << "MainProcess: CreateMutex failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateMutex success" << endl;
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
	if (mutex == INVALID_HANDLE_VALUE || mutex == NULL) {
		cout << "MainProcess: OpenMutex failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: OpenMutex success" << endl;



	// EVENTS CREATION
	HANDLE hWriterEvent = CreateEvent(NULL, TRUE, FALSE, writingToMemoryProcessing);
	HANDLE hReaderEvent = CreateEvent(NULL, TRUE, FALSE, readingFromMemoryProcessing);
	if (hWriterEvent == INVALID_HANDLE_VALUE || hReaderEvent == INVALID_HANDLE_VALUE) {
		cout << "MainProcess: CreateEvent failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateEvent success" << endl;
	OpenEvent(EVENT_MODIFY_STATE, TRUE, writingToMemoryProcessing);
	OpenEvent(EVENT_MODIFY_STATE, FALSE, readingFromMemoryProcessing);

	HANDLE hFile = CreateFile(L"D://test.txt", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "MainProcess: CreateFile failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateFile success" << endl;

	DWORD dwFileSize = GetFileSize(hFile, nullptr);

	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0,
		szName);
	if (GetLastError() != 0) {
		cout << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE,
		sharedMemoryName);
	if (GetLastError() != 0) {
		cout << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateFileMapping success" << endl;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);
	if (GetLastError() != 0) {
		cout << "MainProcess: CreateFileMapping failed with error " << GetLastError() << endl;
	}
	else cout << "MainProcess: CreateFileMapping success" << endl;




	

	//add batnik
	//two shared memory objects (one for file, one for sharing between processes)
	//fix opening shared memory


	if ( !CreateProcess(_T("..\\..\\WriterProcess\\Debug\\WriterProcess.exe"), L" D://test.txt", &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siWriter, &piWriter) || 
		!CreateProcess(_T("..\\..\\ReaderProcess\\Debug\\ReaderProcess.exe"), L" D://test.txt D://copy.txt", &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siReader, &piReader)) {
		cerr << "CreateChildProcesses failed" << endl;
	}
	else { 
		cout << "CreateWriterProcess success" << endl << "CreateReaderProcess success" << endl;
	}



	cout << "->End of parent execution." << endl;

	CloseHandle(piWriter.hProcess);
	CloseHandle(piWriter.hThread);
	CloseHandle(piReader.hProcess);
	CloseHandle(piReader.hThread);
	CloseHandle(mutex);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	getchar();
	return 0;
}


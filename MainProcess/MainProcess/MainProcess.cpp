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


int _tmain()
{
	//SECURITY_ATTRIBUTES saAttr;
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	TCHAR szPath[] = TEXT("cmd.exe");
	//TCHAR szPath[MAX_PATH];
	//LPWSTR szPath = L"calc.exe";
	ZeroMemory(&si, sizeof(si));
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	printf("\n->Start of parent execution.\n");
	boolean success = CreateProcess(NULL, szPath, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "CreateMainProcess failed with error" << GetLastError() << endl;
		cout << GetLastError() << endl;
	}
	else cout << "CreateMainProcess success" << endl;

	
	//size_t fileSize = (size_t)dwFileSize;

	bool writeSuccess = CreateProcess(_T(".\\WriterProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!writeSuccess) {
		cerr << "CreateWriterProcess failed" << endl;
	}
	else cout << "CreateWriterProcess success" << endl;

	bool readSuccess = CreateProcess(_T(".\\ReaderProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!readSuccess) {
		cerr << "CreateReaderProcess failed" << endl;
	}
	else cout << "CreateReaderProcess success" << endl;

	//WaitForMultipleObjects(2, hChildProcesses, TRUE, INFINITE);

	printf("\n->End of parent execution.\n");


	//CloseHandle(hMapping);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//getch();
	getchar();
	return 0;
}


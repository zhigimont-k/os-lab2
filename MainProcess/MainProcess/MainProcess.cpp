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
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

int _tmain()
{
	//SECURITY_ATTRIBUTES saAttr;
	PROCESS_INFORMATION pi, piWriter, piReader;
	STARTUPINFO si = { sizeof(si) };
	STARTUPINFO siWriter = { sizeof(siWriter) };
	STARTUPINFO siReader = { sizeof(siReader) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	TCHAR szPath[] = TEXT("cmd.exe");
	//ZeroMemory(&si, sizeof(si));
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	cout << "->Start of parent execution." << endl;
	boolean success = CreateProcess(NULL, szPath, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "CreateMainProcess failed with error" << GetLastError() << endl;
		cout << GetLastError() << endl;
	}
	else cout << "CreateMainProcess success" << endl;
		
	//bool writeSuccess = CreateProcess(_T("C:\\Users\\Karina\\source\\repos\\ConsoleApplication1\\WriterProcess\\Debug\\WriterProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	bool writeSuccess = CreateProcess(_T(".\\WriterProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siWriter, &piWriter);
	if (!writeSuccess) {
		cerr << "CreateWriterProcess failed" << endl;
	}
	else cout << "CreateWriterProcess success" << endl;

	//bool readSuccess = CreateProcess(_T("C:\\Users\\Karina\\source\\repos\\ConsoleApplication1\\ReaderProcess\\Debug\\ReaderProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	bool readSuccess = CreateProcess(_T(".\\ReaderProcess.exe"), szPath, &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siReader, &piReader);
	if (!readSuccess) {
		cerr << "CreateReaderProcess failed" << endl;
	}
	else cout << "CreateReaderProcess success" << endl;

	//WaitForMultipleObjects(2, hChildProcesses, TRUE, INFINITE);

	cout << "->End of parent execution." << endl;


	//CloseHandle(hMapping);
	//CloseHandle(hFile);
	CloseHandle(piWriter.hProcess);
	CloseHandle(piWriter.hThread);
	CloseHandle(piReader.hProcess);
	CloseHandle(piReader.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//getch();
	getchar();
	return 0;
}


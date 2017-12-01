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
//TCHAR globalMutex[] = TEXT("Global\\mutex");

int _tmain()
{
	//SECURITY_ATTRIBUTES saAttr;
	PROCESS_INFORMATION pi, piWriter, piReader;
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
		cerr << "CreateMainProcess failed with error" << GetLastError() << endl;
		cout << GetLastError() << endl;
	}
	else cout << "CreateMainProcess success" << endl;

	//HANDLE mutex = CreateMutex(0, false, _T("Global\\mutex"));

	if ( !CreateProcess(_T(".\\WriterProcess.exe"), L" D://test.docx", &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siWriter, &piWriter) || 
		!CreateProcess(_T(".\\ReaderProcess.exe"), L" D://test.docx D://copy.docx", &saProcess, &saThread, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siReader, &piReader)) {
		cerr << "CreateChildProcessese failed" << endl;
	}
	else { 
		cout << "CreateWriterProcess success" << endl << "CreateReaderProcess success" << endl;
	}


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


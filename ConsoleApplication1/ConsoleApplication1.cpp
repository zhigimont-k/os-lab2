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


HANDLE hMapping; 
HANDLE hFile;

struct FileMapping {
	HANDLE hFile;
	HANDLE hMapping;
	size_t fsize;
	unsigned char* dataPtr;
};

FileMapping* CreateWriterProcess(HANDLE hFile, TCHAR* szPath, SECURITY_ATTRIBUTES saProcess, SECURITY_ATTRIBUTES saThread, STARTUPINFO si, PROCESS_INFORMATION pi) {
	bool success = CreateProcess(NULL, szPath, &saProcess, &saThread, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "CreateWriterProcess failed" << endl;
	} else cout << "CreateWriterProcess success" << endl;

	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "fileMappingCreate - GetFileSize failed"
			<< std::endl;
		CloseHandle(hFile);
		//return nullptr;
	}
	else cout << "CreateWriterProcess: GetFileSize success" << endl;

	hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0,
		nullptr);
	if (hMapping == nullptr) {
		cerr << "CreateFileMapping failed"
			<< endl;
		CloseHandle(hFile);
		//return 0;
	}
	else cout << "CreateWriterProcess: CreateFileMapping success" << endl;

	unsigned char* dataPtr = (unsigned char*)MapViewOfFile(hMapping,
		FILE_MAP_READ,
		0,
		0,
		dwFileSize);
	if (dataPtr == nullptr) {
		std::cerr << "MapViewOfFile failed"
			<< std::endl;
		CloseHandle(hMapping);
		CloseHandle(hFile);
		//return nullptr;
	}
	else cout << "CreateWriterProcess: MapViewOfFile success" << endl;

	FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));
	if (mapping == nullptr) {
		std::cerr << "malloc failed"
			<< std::endl;
		UnmapViewOfFile(dataPtr);
		CloseHandle(hMapping);
		CloseHandle(hFile);
		//return nullptr;
	}
	else cout << "CreateWriterProcess: FileMapping success" << endl;
	mapping->hFile = hFile;
	mapping->hMapping = hMapping;
	mapping->dataPtr = dataPtr;
	mapping->fsize = (size_t)dwFileSize;
	return mapping;
};

void CreateReaderProcess(FileMapping* mapping, TCHAR* szPath, SECURITY_ATTRIBUTES saProcess, SECURITY_ATTRIBUTES saThread, STARTUPINFO si, PROCESS_INFORMATION pi) {
	bool success = CreateProcess(NULL, szPath, &saProcess, &saThread, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "readerProcessCreate - CreateReaderProcess failed" << endl;
		//CloseHandle(hChildProcesses[1]);
		//return nullptr;
	}
	HANDLE hFileCopy = CreateFile(L"D://copy.jpg", GENERIC_READ, 0, nullptr, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFileCopy == nullptr) {
		cerr << "readerProcessCreate - CreateFile failed" << endl;
		return;
	}
	bool success2 = WriteFile(hFileCopy, mapping->dataPtr, mapping->fsize, 0, 0);
	if (!success2) {
		cerr << "readerProcessCreate - WriteFile failed" << endl;
		cout << GetLastError() << endl;
		//CloseHandle(hChildProcesses[1]);
		//return nullptr;
		return;
	}
}

int _tmain(int argc, TCHAR *argv[])
{

	SECURITY_ATTRIBUTES saAttr;
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
		cerr << "CreateMainProcess failed" << endl;
		cout << GetLastError() << endl;
	}
	else cout << "CreateMainProcess success" << endl;

	hFile = CreateFile(L"D://14979674615650.jpg", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "CreateFile failed" << endl;
		return 0;
	}
	
	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	FileMapping* mapping = CreateWriterProcess(hFile, szPath, saProcess, saThread, si, pi);
	ReleaseMutex(mutex);

	HANDLE mutex2 = CreateMutex(NULL, FALSE, NULL);
	CreateReaderProcess(mapping, szPath, saProcess, saThread, si, pi);
	ReleaseMutex(mutex2);
	//WaitForMultipleObjects(2, hChildProcesses, TRUE, INFINITE);

	printf("\n->End of parent execution.\n");


	CloseHandle(hMapping);
	CloseHandle(hFile);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//getch();
	getchar();
	return 0;
};


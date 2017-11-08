#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>



using namespace std;

SECURITY_ATTRIBUTES saAttr;
PROCESS_INFORMATION pi; 
STARTUPINFO si;
HANDLE* hChildProcesses = new HANDLE[2];
HANDLE hMapping; 
HANDLE hFile;
LPCWSTR fname = (LPCWSTR) "D://14979674615650.jpg";
LPCWSTR fnamecopy = (LPCWSTR) "D://14979674615650 copy.jpg";

struct FileMapping {
	HANDLE hFile;
	HANDLE hMapping;
	size_t fsize;
	unsigned char* dataPtr;
};

FileMapping* CreateWriterProcess() {
	bool success = CreateProcess(NULL, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (hChildProcesses[0] == nullptr) {
		cerr << "CreateWriterProcess failed" << endl;
		//CloseHandle(hChildProcesses[0]);
		//return 0;
	}

	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		std::cerr << "fileMappingCreate - GetFileSize failed, fname =  "
			<< fname << std::endl;
		CloseHandle(hFile);
		//return nullptr;
	}

	hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0,
		nullptr);
	if (hMapping == nullptr) {
		cerr << "CreateFileMapping failed, fname =  "
			<< fname << endl;
		CloseHandle(hFile);
		//return 0;
	}

	unsigned char* dataPtr = (unsigned char*)MapViewOfFile(hMapping,
		FILE_MAP_READ,
		0,
		0,
		dwFileSize);
	if (dataPtr == nullptr) {
		std::cerr << "MapViewOfFile failed, fname =  "
			<< fname << std::endl;
		CloseHandle(hMapping);
		CloseHandle(hFile);
		//return nullptr;
	}

	FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));
	if (mapping == nullptr) {
		std::cerr << "malloc failed, fname = "
			<< fname << std::endl;
		UnmapViewOfFile(dataPtr);
		CloseHandle(hMapping);
		CloseHandle(hFile);
		//return nullptr;
	}
	mapping->hFile = hFile;
	mapping->hMapping = hMapping;
	mapping->dataPtr = dataPtr;
	mapping->fsize = (size_t)dwFileSize;
	return mapping;
};

void CreateReaderProcess(FileMapping* mapping) {
	bool success = CreateProcess(NULL, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!success) {
		cerr << "readerProcessCreate - CreateReaderProcess failed" << endl;
		//CloseHandle(hChildProcesses[1]);
		//return nullptr;
	}
	HANDLE hFileCopy = CreateFile(fnamecopy, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	bool success2 = WriteFile(hFileCopy, mapping->dataPtr, mapping->fsize, 0, 0);
	if (!success2) {
		cerr << "readerProcessCreate - WriteFile failed" << endl;
		//CloseHandle(hChildProcesses[1]);
		//return nullptr;
	}
}

int _tmain(int argc, TCHAR *argv[])
{
	hFile = CreateFile(fname, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		cerr << "CreateFile failed, fname =  "
			<< fname << endl;
		return 0;
	}

	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	if (dwFileSize == INVALID_FILE_SIZE) {
		cerr << "GetFileSize failed, fname =  "
			<< fname << endl;
		CloseHandle(hFile);
		return 0;
	}


	


	printf("\n->Start of parent execution.\n");


	HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
	FileMapping* mapping = CreateWriterProcess();
	ReleaseMutex(mutex);

	HANDLE mutex2 = CreateMutex(NULL, FALSE, NULL);
	CreateReaderProcess(mapping);
	ReleaseMutex(mutex2);
	//WaitForMultipleObjects(2, hChildProcesses, TRUE, INFINITE);

	printf("\n->End of parent execution.\n");


	CloseHandle(hMapping);
	CloseHandle(hFile);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
};


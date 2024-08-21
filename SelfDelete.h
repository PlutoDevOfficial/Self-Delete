#pragma comment(lib, "Shlwapi.lib")

#include <Windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>

#define DS_STREAM_RENAME L":abcde" // Name of data stream (keep the ":")

static HANDLE ds_open_handle(PWCHAR pwPath) {
	return CreateFileW(pwPath, DELETE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static BOOL ds_rename_handle(HANDLE hHandle) {
	FILE_RENAME_INFO fRename;
	RtlSecureZeroMemory(&fRename, sizeof(fRename));
	
	LPWSTR lpwStream = DS_STREAM_RENAME;
	fRename.FileNameLength = sizeof(lpwStream);
	RtlCopyMemory(fRename.FileName, lpwStream, sizeof(lpwStream));

	return SetFileInformationByHandle(hHandle, FileRenameInfo, &fRename, sizeof(fRename) + sizeof(lpwStream));
}

static BOOL ds_deposite_handle(HANDLE hHandle) {

	FILE_DISPOSITION_INFO fDelete;
	RtlSecureZeroMemory(&fDelete, sizeof(fDelete));

	fDelete.DeleteFile = TRUE;

	return SetFileInformationByHandle(hHandle, FileDispositionInfo, &fDelete, sizeof(fDelete));
}

bool SelfDelete() {

	WCHAR wcPath[MAX_PATH + 1];
	RtlSecureZeroMemory(wcPath, sizeof(wcPath));

	if (GetModuleFileNameW(NULL, wcPath, MAX_PATH) == 0)
	{
		return 0;
	}

	HANDLE hCurrent = ds_open_handle(wcPath);
	if (hCurrent == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (!ds_rename_handle(hCurrent))
	{
		return 0;
	}

	CloseHandle(hCurrent);

	hCurrent = ds_open_handle(wcPath);
	if (hCurrent == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (!ds_deposite_handle(hCurrent))
	{
		return 0;
	}

	CloseHandle(hCurrent);

	if (PathFileExistsW(wcPath))
	{
		return 1;
	}
}

#include "mem.h"
#include <iostream>
#include <psapi.h>





DWORD GetProcId(const wchar_t* procName) {

	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE) {

		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry)) {
			do {
				wchar_t wideProcName[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, procEntry.szExeFile, -1, wideProcName, MAX_PATH);

				if (!_wcsicmp(wideProcName, procName)) {
					procId = procEntry.th32ProcessID;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}


uintptr_t GetBaseAddress(DWORD procId) {

	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	std::cout << hSnap << " <-- snap\n";
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(modEntry);

	if (Module32First(hSnap, &modEntry))
	{
		CloseHandle(hSnap);
		return (DWORD_PTR)modEntry.modBaseAddr;
	}

	CloseHandle(hSnap);
	return 1;
}


uintptr_t GetModuleBaseAddress(DWORD processId, LPCWSTR ModuleTarget) {

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

	if (snapshotHandle == INVALID_HANDLE_VALUE) {
		std::cout << "!!!!!!!!!";
		return NULL;
	}
	MODULEENTRY32W moduleEntry = { };
	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	if (Module32FirstW(snapshotHandle, &moduleEntry)) {

		do {
			std::cout << moduleEntry.szModule << "\n";
			if (_wcsicmp(moduleEntry.szModule, ModuleTarget) == 0) {

				CloseHandle(snapshotHandle);
				return reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
			}



		} while (Module32NextW(snapshotHandle, &moduleEntry));



	}

	CloseHandle(snapshotHandle);
	return NULL;

}

void ListModules(DWORD processId) {
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

	if (processHandle == NULL) {
		return;

	}

	HMODULE moduleHandles[1024];
	DWORD bytesNeeded;

	if (EnumProcessModules(processHandle, moduleHandles, sizeof(moduleHandles), &bytesNeeded)) {
		DWORD moduleCount = bytesNeeded / sizeof(HMODULE);

		for (DWORD i = 0; i < moduleCount; i++) {
			TCHAR moduleName[MAX_PATH];
			if (GetModuleFileNameEx(processHandle, moduleHandles[i], moduleName, MAX_PATH)) {

			}
		}
	}

	CloseHandle(processHandle);
}
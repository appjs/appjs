#include <windows.h>

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR szArgs, int nCmdShow){
  size_t iMyCounter = 0, ret = 0, iPos = 0; 
  DWORD code = 0; 
  STARTUPINFOW startupInfo; 
  PROCESS_INFORMATION processInfo; 
  memset(&startupInfo, 0, sizeof(startupInfo)); 
  memset(&processInfo, 0, sizeof(processInfo)); 
  startupInfo.cb = sizeof(startupInfo); 

  if (CreateProcess(L".\\bin\\node.exe", L" app.js",
                     0, 0, false, CREATE_NO_WINDOW, 0, 0,
                     &startupInfo, &processInfo) != false)

    code = WaitForSingleObject(processInfo.hProcess, 1000); 
  else
    ret = GetLastError(); 


  CloseHandle(processInfo.hProcess); 
  CloseHandle(processInfo.hThread); 

  return ret; 
}

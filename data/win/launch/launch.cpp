#include <windows.h>
#include <string>
#include <resource.h>
#include <direct.h>

using namespace std;

// #############################
// ### Conversion to WString ###
// #############################

wstring ToWString(const char* utf8){
  if (utf8 == NULL || *utf8 == '\0') {
    return wstring();
  } else {
    const int utf8len = static_cast<int>(strlen(utf8));
    const int utf16len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, utf8len, NULL, 0);
    if (utf16len == 0) {
      return wstring();
    } else {
      wstring utf16;
      utf16.resize(utf16len);
      if (!::MultiByteToWideChar(CP_UTF8, 0, utf8, utf8len, &utf16[0], utf16len)) {
        return wstring();
      } else {
        return utf16;
      }
    }
  }
}

wstring ToWString(const string& utf8){
  return ToWString(utf8.data());
}

wstring ToWString(const wstring& utf16){
  return utf16;
}

wstring ToWString(const wchar_t* utf16){
  return wstring(utf16);
}



// #############################
// ### Conversion to String ###
// #############################

string ToString(const wchar_t* utf16){
  if (utf16 == NULL || *utf16 == L'\0') {
    return string();
  } else {
    const int utf16len = static_cast<int>(wcslen(utf16));
    const int utf8len = ::WideCharToMultiByte(CP_UTF8, 0, utf16, utf16len, NULL, 0, NULL, NULL);
    if (utf8len == 0) {
      return string();
    } else {
      string utf8;
      utf8.resize(utf8len);
      if (!::WideCharToMultiByte(CP_UTF8, 0, utf16, utf16len, &utf8[0], utf8len, NULL, NULL)) {
        return string();
      } else {
        return utf8;
      }
    }
  }
}

string ToString(const wstring& utf16){
  return ToString(utf16.data());
}

string ToString(const string& utf8){
  return utf8;
}

string ToString(const char* utf8){
  return string(utf8);
}



// ###########################
// ### Conversion to WChar ###
// ###########################

wchar_t* ToWChar(const char* utf8){
  if (utf8 == NULL || *utf8 == L'\0') {
    return new wchar_t[0];
  } else {
    const int utf8len = static_cast<int>(strlen(utf8));
    const int utf16len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, utf8len, NULL, 0);
    if (utf16len == 0) {
      return new wchar_t[0];
    } else {
      wchar_t* utf16 = new wchar_t[utf16len];
      if (!::MultiByteToWideChar(CP_UTF8, 0, utf8, utf8len, utf16, utf16len)) {
        return new wchar_t[0];
      } else {
        return utf16;
      }
    }
  }
}

wchar_t* ToWChar(const wchar_t* utf16){
  size_t len = wcslen(utf16) + 1;
  wchar_t* data = new wchar_t[len];
  wcscpy_s(data, len, utf16);
  return data;
}

wchar_t* ToWChar(const string& utf8){
  return ToWChar(utf8.data());
}

wchar_t* ToWChar(const wstring& utf16){
  return ToWChar(utf16.data());
}


// ##########################
// ### Conversion to Char ###
// ##########################

char* ToChar(const wchar_t* utf16){
  if (utf16 == NULL || *utf16 == L'\0') {
    return new char[0];
  } else {
    const int utf16len = static_cast<int>(wcslen(utf16));
    const int utf8len = ::WideCharToMultiByte(CP_UTF8, 0, utf16, utf16len, NULL, 0, NULL, NULL);
    if (utf8len == 0) {
      return new char[0];
    } else {
      char* utf8 = new char[utf8len];
      if (!::WideCharToMultiByte(CP_UTF8, 0, utf16, utf16len, utf8, utf8len, NULL, NULL)) {
        return new char[0];
      } else {
        return utf8;
      }
    }
  }
}

char* ToChar(const char* utf8){
  size_t len = strlen(utf8) + 1;
  char* data = new char[len];
  strcpy_s(data, len, utf8);
  return data;
}

char* ToChar(const wstring& utf16){
  return ToChar(utf16.data());
}

char* ToChar(const string& utf8){
  return ToChar(utf8.data());
}



size_t Launch(wstring exe, wstring params) {
  if (params.size() != 0 && params[0] != L' ') {
    params.insert(0, L" ");
  }
  
  size_t ret = 0, lastSlash = 0;
  DWORD exitCode = 0;
  wstring temp = L"";

  wchar_t* paramCopy = new wchar_t[params.size() + 1];
  if (paramCopy == 0)
    return 1;

  const wchar_t* transfer = params.c_str();
  wcscpy_s(paramCopy, params.size() + 1, transfer);

  STARTUPINFOW startup;
  PROCESS_INFORMATION process;
  memset(&startup, 0, sizeof(startup));
  memset(&process, 0, sizeof(process));
  startup.cb = sizeof(startup);


  if (CreateProcessW(const_cast<LPCWSTR>(exe.c_str()), paramCopy, 0, 0, false,
	                 CREATE_NO_WINDOW, 0, 0, &startup, &process) != false) {
    exitCode = WaitForSingleObject(process.hProcess, 1000);
  } else {
    ret = GetLastError();
  }
  
  delete[]paramCopy;
  paramCopy = 0;
  CloseHandle(process.hProcess);
  CloseHandle(process.hThread);
  return ret;
}

wstring ExecutablePath(){
  WCHAR* buffer = (wchar_t*)malloc(sizeof(WCHAR) * MAX_PATH);
  if (GetModuleFileNameW(NULL, buffer, MAX_PATH) > 0) {
    return ToWString(buffer);
  } else {
	return wstring();
  }
}

wstring GetString(int id){
  wchar_t temp[100];
  LoadStringW(GetModuleHandle(NULL), id, temp, 100);
  return ToWString(temp);
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPTSTR args, int nCmdShow){
  wstring workingdirectory= ExecutablePath().substr(0, ExecutablePath().find_last_of(L"\\") );
  //_wchdir(workingdirectory.c_str());	
  wstring nodePath = workingdirectory;
  nodePath.append(GetString(NODE_EXECUTABLE));	

  wstring path = ExecutablePath();
  path.resize(path.length() - 4);//cut off .exe
  path.append(L".js");//add .js
  path.erase(0, path.find_last_of(L"\\") + 1);//cut off path
  path.insert(0, GetString(DEFAULT_PARAMS));//add in default params.
  path.append(L" ");
  path.append(ToWString(args));

  
  return Launch(nodePath, path);
}

#include "util_win.h"

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

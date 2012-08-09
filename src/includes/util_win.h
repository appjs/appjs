#ifndef APPJS_UTIL_WIN
#define APPJS_UTIL_WIN

#include <windows.h>
#include <string>

using namespace std;

wstring ToWString(const string& utf8);
wstring ToWString(const wstring& utf16);
wstring ToWString(const wchar_t* utf16);
string ToString(const wchar_t* utf16);
string ToString(const wstring& utf16);
string ToString(const string& utf8);
string ToString(const char* utf8);
wchar_t* ToWChar(const char* utf8);
wchar_t* ToWChar(const wchar_t* utf16);
wchar_t* ToWChar(const string& utf8);
wchar_t* ToWChar(const wstring& utf16);
char* ToChar(const wchar_t* utf16);
char* ToChar(const char* utf8);
char* ToChar(const wstring& utf16);
char* ToChar(const string& utf8);


#endif

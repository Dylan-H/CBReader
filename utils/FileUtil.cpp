/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

#include "BaseUtil.h"
#include "FileUtil.h"

// cf. http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// A convenient way to grab the same value as HINSTANCE passed to WinMain
HINSTANCE GetInstance() { return (HINSTANCE)&__ImageBase; }

namespace path {

bool IsSep(WCHAR c) { return '\\' == c || '/' == c; }

bool IsSep(char c) { return '\\' == c || '/' == c; }

// Note: returns pointer inside <path>, do not free
const WCHAR *GetBaseName(const WCHAR *path) {
    const WCHAR *fileBaseName = path + str::Len(path);
    for (; fileBaseName > path; fileBaseName--) {
        if (IsSep(fileBaseName[-1]))
            break;
    }
    return fileBaseName;
}

// Note: returns pointer inside <path>, do not free
const char *GetBaseName(const char *path) {
    const char *fileBaseName = path + str::Len(path);
    for (; fileBaseName > path; fileBaseName--) {
        if (IsSep(fileBaseName[-1]))
            break;
    }
    return fileBaseName;
}

// Note: returns pointer inside <path>, do not free
const WCHAR *GetExt(const WCHAR *path) {
    const WCHAR *ext = path + str::Len(path);
    for (; ext > path && !IsSep(*ext); ext--) {
        if (*ext == '.')
            return ext;
    }
    return path + str::Len(path);
}

// Caller has to free()
WCHAR *GetDir(const WCHAR *path) {
    const WCHAR *baseName = GetBaseName(path);
    if (baseName == path) // relative directory
        return str::Dup(L".");
    if (baseName == path + 1) // relative root
        return str::DupN(path, 1);
    if (baseName == path + 3 && path[1] == ':') // local drive root
        return str::DupN(path, 3);
    if (baseName == path + 2 && str::StartsWith(path, L"\\\\")) // server root
        return str::Dup(path);
    // any subdirectory
    return str::DupN(path, baseName - path - 1);
}

WCHAR *Join(const WCHAR *path, const WCHAR *fileName) {
    if (IsSep(*fileName))
        fileName++;
    WCHAR *sepStr = nullptr;
    if (!IsSep(path[str::Len(path) - 1]))
        sepStr = L"\\";
    return str::Join(path, sepStr, fileName);
}

char *JoinUtf(const char *path, const char *fileName, Allocator *allocator) {
    if (IsSep(*fileName))
        fileName++;
    char *sepStr = nullptr;
    if (!IsSep(path[str::Len(path) - 1]))
        sepStr = "\\";
    return str::Join(path, sepStr, fileName, allocator);
}

// Normalize a file path.
//  remove relative path component (..\ and .\),
//  replace slashes by backslashes,
//  convert to long form.
//
// Returns a pointer to a memory allocated block containing the normalized string.
//   The caller is responsible for freeing the block.
//   Returns nullptr if the file does not exist or if a memory allocation fails.
//
// Precondition: the file must exist on the file system.
//
// Note:
//   - the case of the root component is preserved
//   - the case of rest is set to the way it is stored on the file system
//
// e.g. suppose the a file "C:\foo\Bar.Pdf" exists on the file system then
//    "c:\foo\bar.pdf" becomes "c:\foo\Bar.Pdf"
//    "C:\foo\BAR.PDF" becomes "C:\foo\Bar.Pdf"
WCHAR *Normalize(const WCHAR *path) {
    // convert to absolute path, change slashes into backslashes
    DWORD cch = GetFullPathName(path, 0, nullptr, nullptr);
    if (!cch)
        return str::Dup(path);
    ScopedMem<WCHAR> fullpath(AllocArray<WCHAR>(cch));
    GetFullPathName(path, cch, fullpath, nullptr);
    // convert to long form
    cch = GetLongPathName(fullpath, nullptr, 0);
    if (!cch)
        return fullpath.StealData();
    ScopedMem<WCHAR> normpath(AllocArray<WCHAR>(cch));
    GetLongPathName(fullpath, normpath, cch);
    if (cch <= MAX_PATH)
        return normpath.StealData();

    // else mark the path as overlong
    if (str::StartsWith(normpath.Get(), L"\\\\?\\"))
        return normpath.StealData();
    return str::Join(L"\\\\?\\", normpath);
}





static bool MatchWildcardsRec(const WCHAR *fileName, const WCHAR *filter) {
#define AtEndOf(str) (*(str) == '\0')
    switch (*filter) {
        case '\0':
        case ';':
            return AtEndOf(fileName);
        case '*':
            filter++;
            while (!AtEndOf(fileName) && !MatchWildcardsRec(fileName, filter))
                fileName++;
            return !AtEndOf(fileName) || AtEndOf(filter) || *filter == ';';
        case '?':
            return !AtEndOf(fileName) && MatchWildcardsRec(fileName + 1, filter + 1);
        default:
            return towlower(*fileName) == towlower(*filter) &&
                   MatchWildcardsRec(fileName + 1, filter + 1);
    }
#undef AtEndOf
}

/* matches the filename of a path against a list of semicolon
   separated filters as used by the common file dialogs
   (e.g. "*.pdf;*.xps;?.*" will match all PDF and XPS files and
   all filenames consisting of only a single character and
   having any extension) */
bool Match(const WCHAR *path, const WCHAR *filter) {
    path = GetBaseName(path);
    while (str::FindChar(filter, ';')) {
        if (MatchWildcardsRec(path, filter))
            return true;
        filter = str::FindChar(filter, ';') + 1;
    }
    return MatchWildcardsRec(path, filter);
}


// returns the path to either the %TEMP% directory or a
// non-existing file inside whose name starts with filePrefix
WCHAR *GetTempPath(const WCHAR *filePrefix) {
    WCHAR tempDir[MAX_PATH - 14];
    DWORD res = ::GetTempPath(dimof(tempDir), tempDir);
    if (!res || res >= dimof(tempDir))
        return nullptr;
    if (!filePrefix)
        return str::Dup(tempDir);
    WCHAR path[MAX_PATH];
    if (!GetTempFileName(tempDir, filePrefix, 0, path))
        return nullptr;
    return str::Dup(path);
}

// returns a path to the application module's directory
// with either the given fileName or the module's name
// (module is the EXE or DLL in which path::GetAppPath resides)
WCHAR *GetAppPath(const WCHAR *fileName) {
    WCHAR modulePath[MAX_PATH];
    modulePath[0] = '\0';
    GetModuleFileName(GetInstance(), modulePath, dimof(modulePath));
    modulePath[dimof(modulePath) - 1] = '\0';
    if (!fileName)
        return str::Dup(modulePath);
    ScopedMem<WCHAR> moduleDir(path::GetDir(modulePath));
    return path::Join(moduleDir, fileName);
}
}

namespace file{

bool Exists(const WCHAR *filePath) {
    if (nullptr == filePath)
        return false;

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    BOOL res = GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo);
    if (0 == res)
        return false;

    if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return false;
    return true;
}

// returns -1 on error (can't use INVALID_FILE_SIZE because it won't cast right)
int64 GetSize(const WCHAR *filePath) {
    CrashIf(!filePath);
    if (!filePath)
        return -1;

	FILE*fp= nullptr;
	int err= _wfopen_s(&fp,filePath, L"rb");// localfile文件名
	if(_fseeki64(fp,0,SEEK_END))
	{
		fclose(fp);
		return -1;
	}
	int64 ret = _ftelli64(fp); 
	fclose(fp);
	return ret;

}

char *ReadAll(const WCHAR *filePath, size_t *fileSizeOut, Allocator *allocator) {
    int64 size64 = GetSize(filePath);
    if (size64 < 0)
        return nullptr;
    size_t size = (size_t)size64;
#ifdef _WIN64
    CrashIf(size != (size_t)size64);
#else
    if (size != size64)
        return nullptr;
#endif

    // overflow check
    if (size + 1 + sizeof(WCHAR) < sizeof(WCHAR) + 1)
        return nullptr;
    /* allocate one character more and zero-terminate just in case it's a
       text file we'll want to treat as C string. Doesn't hurt for binary
       files (note: three byte terminator for UTF-16 files) */
    char *data = (char *)Allocator::Alloc(allocator, size + sizeof(WCHAR) + 1);
    if (!data)
        return nullptr;

    if (!ReadN(filePath, data, size)) {
        Allocator::Free(allocator, data);
        return nullptr;
    }

    // zero-terminate for convenience
    data[size] = data[size + 1] = data[size + 2] = '\0';

    if (fileSizeOut)
        *fileSizeOut = size;
    return data;
}

char *ReadAllUtf(const char *filePath, size_t *fileSizeOut, Allocator *allocator) {
    WCHAR buf[512];
    str::Utf8ToWcharBuf(filePath, str::Len(filePath), buf, dimof(buf));
    return ReadAll(buf, fileSizeOut, allocator);
}

// buf must be at least toRead in size (note: it won't be zero-terminated)
bool ReadN(const WCHAR *filePath, char *buf, size_t toRead) {
	FILE*fp = nullptr;
	_wfopen_s(&fp, filePath, L"rb");// localfile文件名
	int n = fread(buf, 1, toRead, fp); //读文件
	fclose(fp); // 关闭文件
	return n == toRead;

}

bool WriteAll(const WCHAR *filePath, const void *data, size_t dataLen) {

	FILE*fp = nullptr;
	_wfopen_s(&fp, filePath, L"wb");
	int n  = fwrite(data,1, dataLen, fp );
	fclose(fp);
	return n == dataLen;
}

bool WriteAllUtf(const char *filePath, const void *data, size_t dataLen) {
    WCHAR buf[512];
    str::Utf8ToWcharBuf(filePath, str::Len(filePath), buf, dimof(buf));
    return WriteAll(buf, data, dataLen);
}

// Return true if the file wasn't there or was successfully deleted
bool Delete(const WCHAR *filePath) {
    BOOL ok = DeleteFile(filePath);
    return ok || GetLastError() == ERROR_FILE_NOT_FOUND;
}


// return true if a file starts with string s of size len
bool StartsWithN(const WCHAR *filePath, const char *s, size_t len) {
    ScopedMem<char> buf(AllocArray<char>(len));
    if (!buf)
        return false;

    if (!ReadN(filePath, buf, len))
        return false;
    return memeq(buf, s, len);
}

// return true if a file starts with null-terminated string s
bool StartsWith(const WCHAR *filePath, const char *s) {
    return file::StartsWithN(filePath, s, str::Len(s));
}

}

namespace dir {

bool Exists(const WCHAR *dir) {
    if (nullptr == dir)
        return false;

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    BOOL res = GetFileAttributesEx(dir, GetFileExInfoStandard, &fileInfo);
    if (0 == res)
        return false;

    if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

// Return true if a directory already exists or has been successfully created
bool Create(const WCHAR *dir) {
    BOOL ok = CreateDirectory(dir, nullptr);
    if (ok)
        return true;
    return ERROR_ALREADY_EXISTS == GetLastError();
}

// creates a directory and all its parent directories that don't exist yet
bool CreateAll(const WCHAR *dir) {
    ScopedMem<WCHAR> parent(path::GetDir(dir));
    if (!str::Eq(parent, dir) && !Exists(parent))
        CreateAll(parent);
    return Create(dir);
}
}

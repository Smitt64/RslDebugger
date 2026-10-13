#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <QObject>
#include <rsfname.h>
#include <Shlobj.h>

#ifndef CSIDL_APPDATA
#define  CSIDL_APPDATA      0x001A      // Application Data, new for NT4
#endif

// -----------------------------------------------------------------------------
#ifndef CSIDL_FLAG_CREATE
#define  CSIDL_FLAG_CREATE  0x8000      // new for Win2K, or this in to force creation of folder
#endif

#include <Shlobj.h>
#include <Shlwapi.h>

static QString  RslProductName;
static QString  RslProductVersion;
static bool     Inited;

int CorrectPath(char *buff)
{
    int    retVal = 0;
    char  *ptr    = buff;


    // Scip witespaces and leading '\\'
    while(*ptr == ' ' || *ptr == '\t' || *ptr == '\\')
        ++ptr;


    while((ptr = strchr(ptr, '\\')) != NULL)
    {
        ++ptr;

        while(*ptr == '\\')
        {
            memmove(ptr, ptr + 1, strlen(ptr + 1) + 1);

            ++retVal;
        }
    }

    return retVal;
}

int access_rs(const char *fileName, int mode)
{
    return _access(fileName, mode);
}

int mkdir_rs(const char *dirName)
{
    return _mkdir(dirName);
}

int RsMkDir(const char *dir)
{
    if(access_rs(dir, 0) == 0)
        return 0;

    const char  *p;


    if(*dir == '\\' && *(dir + 1) == '\\') // UNC path  \\server\share
    {
        p = strchr(dir + 2, '\\');

        if(!p)
            return -1;

        ++p;
    }
    else
    {
        p = strpbrk(dir, "\\/");

        if(p && (p == dir || *(p - 1) == ':')) // root dir
        {
            ++p;

            if(*p == '/')   // btrv://user/file
                ++p;
        }
        else
            p = dir;
    }

    char  buff[_MAX_PATH];

    do
    {
        p = strpbrk(p, "\\/");

        if(p)
        {
            memcpy(buff, dir, p - dir);
            buff[p - dir] = '\0';

            ++p;
        }
        else
            strcpy(buff, dir);

        if(access_rs(buff, 0) != 0)
        {
            if(mkdir_rs(buff) == -1)
                return -1;
        }
    } while(p);

    return 0;
}

const char *RslGetProductName(void)
{
    return RslProductName.toLocal8Bit().data();
}

const char *RslGetProductVersion(void)
{
    return RslProductVersion.toLocal8Bit().data();
}

static int MyReadProc(char *key, char *val, void *data)
{
    if(strcmp(key, "PRODUCT") == 0)
        RslProductName = val;
    else if(strcmp(key, "VERSION") == 0)
    {
        char   buff[128];
        int    verHi = 0;
        int    verLo = 0;
        // We need only first 2 numbers from 1.00.096.0
        char  *ptr = strchr(val, '.');


        if(ptr)
        {
            *ptr = '\0';

            verHi = atoi(val);

            val = ptr + 1;
            ptr = strchr(val, '.');

            if(ptr)
                *ptr = '\0';

            verLo = atoi(val);
        }
        else
            verHi = atoi(val);

        sprintf(buff, "%d.%d", verHi, verLo);

        RslProductVersion = buff;
    }

    return 0;
}

void InitProductConfig()
{
    if(!Inited)
    {
        char  buff[_MAX_PATH];
        strcpy(buff, "product.ini");
        ReadConfigEx(RunPath(buff), MyReadProc, NULL, NULL);
        Inited = true;
    }
}

static bool GetProductFileNameImpl(const char *appName, const char *fileName, char *buffer, int nFolder)
{
    const char  *company = "R-Style Softlab";
    const char  *prod;
    const char  *ver;

    InitProductConfig();

    prod = RslGetProductName();
    ver  = RslGetProductVersion();

    if(!prod)
        prod = "";

    if(!ver)
        ver = "";

    if(!appName)
        appName = "";

    char  path[_MAX_PATH];


    if(SUCCEEDED(SHGetFolderPathA(NULL, nFolder | CSIDL_FLAG_CREATE, NULL, 0, path)))
    {
        sprintf(buffer, "%s\\%s\\%s\\%s\\%s\\", path, company, prod, ver, appName);

        CorrectPath(buffer);

        if(RsMkDir(buffer) != -1)
        {
            if(fileName && *fileName)
                strcat(buffer, fileName);

            return true;
        }
    }

    return false;

#if 0
 if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, buffer)))
  {
   if(PathAppendA(buffer, company) && (CreateDirectoryA(buffer, NULL) || GetLastError() == ERROR_ALREADY_EXISTS))
    {
     if(prod && *prod)
      {
       if(!PathAppendA(buffer, prod) || !(CreateDirectoryA(buffer, NULL) || GetLastError() == ERROR_ALREADY_EXISTS))
         return false;

       if(ver && *ver)
        {
         if(!PathAppendA(buffer, ver) || !(CreateDirectoryA(buffer, NULL) || GetLastError() == ERROR_ALREADY_EXISTS))
           return false;
        }
      }

     if(appName && *appName)
      {
       if(!PathAppendA(buffer, appName) || !(CreateDirectoryA(buffer, NULL) || GetLastError() == ERROR_ALREADY_EXISTS))
         return false;
      }

     if(fileName && *fileName && !PathAppendA(buffer, fileName))
       return false;

     return true;
    }
  }

 return false;
#endif
}

// -----------------------------------------------------------------------------
bool GetProductFileName(const char *appName, const char *fileName, char *buffer)
{
    return GetProductFileNameImpl(appName, fileName, buffer, CSIDL_APPDATA);
}

// -----------------------------------------------------------------------------
bool GetProductFileNameCommon(const char *appName, const char *fileName, char *buffer)
{
    return GetProductFileNameImpl(appName, fileName, buffer, CSIDL_COMMON_APPDATA);
}

// -----------------------------------------------------------------------------
const char *GetPersonalFolder(char *path)
{
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path)))
        return path;

    *path = '\0';

    return NULL;
}

/*** File handling functions ***/

/*  Copyright (C) 2010, 2012, Michal Szymaniak
    http://sc0ty.pl

    This file is part of VSMTools.

    VSMTools is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VSMTools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VSMTools; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "files.h"
#include <cstdio>

bool fsave(const char *name, void *buf, uint32_t sz)
{
    FILE *f = fopen(name, "wb");

    if (f == NULL) return 0;

    if (fwrite(buf, sz, 1, f) <= 0)
    {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
} // bool fsave(const char *name, void *buf, uint32_t sz)

int fload(const char *name, void *buf, uint32_t sz)
{
    FILE *f = fopen(name, "rb");
    if (f == NULL) return -1;

    uint32_t x = 0;
    int r;

    while ((!feof(f)) && (x < sz))
    {
        r = fread((char*)buf+x, 1, sz-x, f);

        if (r < 0)
        {
            fclose(f);
            return r;
        }

        x += r;
    }

    fclose(f);

    return x;
} // int fload(const char *name, void *buf, uint32_t sz)

bool fcopy(const char *from, const char *to)
{
    FILE *ff = fopen(from, "rb");
    if (ff == NULL) return 0;

    FILE *ft = fopen(to, "wb");
    if (ft == NULL) return 0;

    char buf[16384];
    int x;

    while (!feof(ff))
    {
        x = fread(buf, 1, sizeof(buf), ff);
        fwrite(buf, 1, x, ft);
    }

    fclose(ff);
    fclose(ft);

    return 1;
} // bool fcopy(const char *from, const char *to)

char *fname(const char *path)
{
    char *n, *last;
    n = last = (char *)path;

    while (n[0] != 0)
    {
        if (n[0] == '\\') last = n+1;
        n++;
    }

    return last;
} // char *fname(const char *path)

int createdir(const char *path)
{
    int i = CreateDirectory(path, NULL);

    if (i > 0) return 1;
    else if (i < 0) return -1;

    if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    else return -1;
} // int createdir(const char *path)

char *FLFindFirst(TFileList *TFL, const char *path)
{
    TFL->hFind = FindFirstFile(path, &TFL->fd);
    if(TFL->hFind  == INVALID_HANDLE_VALUE) return NULL;

    return TFL->fd.cFileName;
} // char *FLFindFirst(TFileList *TFL, const char *path)

char *FLFind(TFileList *TFL)
{
    while (FindNextFile(TFL->hFind, &TFL->fd))
    {
        if ((TFL->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        return TFL->fd.cFileName;
    }

    FindClose(TFL->hFind);
    return NULL;
} // char *FLFind(TFileList *TFL)

bool FileExist(const char *path)
{
    FILE *f;
    f = fopen(path, "rb");

    if (f)
    {
        fclose(f);
        return 1;
    }

    return 0;
} // bool FileExist(const char *path)

bool isSlash(const char *path)
{
    int len = strlen(path) -1;
    return ((path[len] == '\\') || (path[len] == '/'));
} // bool isSlash(const char *path)

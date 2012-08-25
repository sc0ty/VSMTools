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

#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include <stdint.h>
#include <windows.h>

using namespace std;

// Listing file control structure
struct TFileList
{
    WIN32_FIND_DATA fd;
    HANDLE hFind;
};

// Save data from buf to file, returns true if succeeded
bool fsave(const char *name, void *buf, uint32_t sz);

// Read data from file to buf, returns no of readed bytes or negative error no
int fload(const char *name, void *buf, uint32_t sz);

// copy file, return true if succeeded
bool fcopy(const char *from, const char *to);

// Extract filename from path
char *fname(const char *path);

// Create directory, returns 1 if succeeded, -1 if fail,
// 0 if directory already exists
int createdir(const char *path);

// List files in directory - find first file
// returns filename or NULL if there is no files
char *FLFindFirst(TFileList *TFL, const char *path);

// List files in directory - find next file
// returns filename or NULL if there is no files
char *FLFind(TFileList *TFL);

// Return true if file exist
bool FileExist(const char *path);

// Return true if given path ends up with slash or backslash
bool isSlash(const char *path);


#endif // FILES_H_INCLUDED

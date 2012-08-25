/*** Information of VSM resources ***/

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

#include "vsmreslist.h"
#include <cstring>
#include <cstdio>

TVMSLInfo::TVMSLInfo() : loaded(0)
{
} // TVMSLInfo::TVMSLInfo()

bool TVMSLInfo::load()
{
    FILE *f = fopen("resources.txt", "rt");

    // can't open file
    if (f == NULL) return 0;

    TVSMInfo info;
    char text[20], tdescr[256];

    rl.clear();

    // file read
    while (!feof(f))
    {
        fscanf(f, "%hx %s %hu %s", &info.id, text, &info.len, tdescr);

        info.ext = new char[strlen(text)+1];
        strcpy(info.ext, text);

        info.descr = new char[strlen(tdescr)+1];
        strcpy(info.descr, tdescr);

        rl.push_back(info);
        loaded = 1;
    }

    fclose(f);
    iter = rl.begin();

    return 1;
} // bool TVMSLInfo::load()

bool TVMSLInfo::find(uint16_t id)
{
    // list wasn't loaded
    if (loaded == 0) return 0;

    iter = rl.begin();

    while (iter->id != id)
    {
        iter++;

        // there is no such id in database
        if (iter == rl.end()) return 0;
    }

    return 1;
} // bool TVMSLInfo::find(uint16_t id)

TVSMInfo &TVMSLInfo::get()
{
    return *iter;
} // TVSMInfo &TVMSLInfo::get()

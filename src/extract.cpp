/*** Extracting VSM files class ***/

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

#include "extract.h"

TVSMFile::TVSMFile() : footer(NULL), f(NULL)
{
} // TVSMFile::TVSMFile()

bool TVSMFile::open(const char *name)
{
    pos = 0;
    f = fopen(name, "rb");

    if (f == NULL) return 0;
    if (fread(&header, sizeof(header), 1, f) <= 0) return 0;

    return (header.signature == VSMSignature);
} // bool TVSMFile::open(const char *name)

void TVSMFile::close()
{
    if (f) fclose(f);
} // void TVSMFile::close()

int TVSMFile::read(TVSMResource *res)
{
    if (pos & 1)
    {
        pos++;
        fseek(f, 1, SEEK_CUR);
    }

    // read resource
    if (pos < header.ressize)
    {
        if (fread(res, RHSIZE, 1, f) <= 0) return -4;

        if (res->size == 0)
        {
            pos += RHSIZE;
            return 0;
        }

        res->data = new char[res->size];
        if (fread(res->data, res->size, 1, f) <= 0) return -2;

        pos += RHSIZE + res->size;
        return res->size;
    }

    // read footer data
    else
    {
        // footer address
        uint32_t footadr = sizeof(header) + header.ressize;
        while(footadr & 3) footadr++;

        // file size
        fseek(f, 0, SEEK_END);
        uint32_t sz = ftell(f);
        fseek(f, footadr, SEEK_SET);

        // if there is no footer
        if (footadr >= sz)
        {
            footer_sz = 0;
            return -1;
        }

        // footer size
        footer_sz = sz - footadr;
        footer = new char[footer_sz];

        if (fread(footer, footer_sz, 1, f) <= 0) return -3;
        return -1;
    }

    return -1;
} // int TVSMFile::read(TVSMResource *res)


uint32_t TVSMFile::computeChecksum()
{
    // rewind file to resources begining
    int p = ftell(f);
    fseek(f, sizeof(TVSMHeader), SEEK_SET);

    char buf[8192];
    TCRC sum;
    sum.newCRC32();

    // remaining bytes to read
    int n = header.ressize;
    int i;

    while (n > 0)
    {
        i = sizeof(buf);
        if (i > n) i = n;

        i = fread(&buf, 1, i, f);
        sum.add(buf, i);
        n -= i;
    }

    fseek(f, p, SEEK_SET);
    return sum.getCRC32() ^ 0xffffffff;
} // uint32_t TVSMFile::computeChecksum()

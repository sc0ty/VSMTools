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

#ifndef EXTRACT_H_INCLUDED
#define EXTRACT_H_INCLUDED

#include "vsmstruct.h"
#include "crc.h"
#include <cstdio>

class TVSMFile
{
public:
    TVSMHeader header;  // VSM file header
    void *footer;       // VSM file footer
    uint32_t footer_sz; // footer size

    TVSMFile();

    // Open file and read footer, return true if succeeded
    bool open(const char *name);

    // Close file
    void close();

    // Read next resource, return resource size, negative error number
    // or -1 if there is no resources left
    int read(TVSMResource *res);

    // Compute resources checksum
    uint32_t computeChecksum();

private:
    FILE *f;
    uint32_t pos;
}; // class TVSMFile

#endif // EXTRACT_H_INCLUDED

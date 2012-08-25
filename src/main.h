/*** BlackBerry VSM editor - main functionality ***/

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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

using namespace std;

#include "vsmstruct.h"
#include <stdint.h>

/*** Program functions ***/

// extract command
void vsmextract(int argc, char *argv[]);

// create command
void vsmcreate(int argc, char *argv[]);

// info command
void vsminfo(int argc, char *argv[]);

// modify command
void vsmmodify(int argc, char *argv[]);

// show info screen
void info(int argc, char *argv[]);

/*** Helper functions ***/

// show error message and exit
void errexit(const char *err);

// show file information from given header and checksum
void fileinfo(TVSMHeader *header, uint32_t csum);

// show footer infrmation
void footerinfo(void *footer, uint32_t len);

// replace any special chars with dash '-'
void removespecialchars(char *str);

// extract resource number from filename, return extracted number
// or negative error code
int ParseResName(const char *rname);

// convert one hex char to int, return -1 if given incorrect char
int hex2int(char hex);

// convert ascii hex string to binary format
// return bytes after conversion
int hex2bin(const char *hex, char *buf, int len);

#endif // MAIN_H_INCLUDED

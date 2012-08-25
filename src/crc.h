/*** Checksum CRC32 ***/

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

#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#include <stdint.h>

/*** CRC32 computing class ***/
class TCRC
{
public:
    TCRC();

    // start computing
    void newCRC32();

    // get last computed crc
    uint32_t getCRC32();

    // add data to compute crc32, return computed crc
    uint32_t add(const void *buf, uint32_t len);
    uint32_t add(uint8_t data);

private:
    uint32_t crc32;
};

#endif // CRC_H_INCLUDED

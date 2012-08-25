/*** VSM file structures ***/

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

#ifndef VMSSTRUCT_H_INCLUDED
#define VMSSTRUCT_H_INCLUDED

#include <stdint.h>


/*** Definitions ***/

// VSM file signature
#define VSMSignature    0xBC000001

// VSM footer signature
#define VSMFSignature   0xD7C82D1F

// Max resource size
#define VSMMaxResSize   0xffff


/*** Data signatures ***/

// VSM file header
struct TVSMHeader
{
    uint32_t signature;
    uint32_t ressize;       // all resources size
    uint32_t checksum;
    uint16_t vendor_id;     // Vendor ID
    uint16_t zero;          // always zero
    uint8_t  version[4];    // file version
    uint32_t reserved[2];
};

// VSM file footer
struct TVSMFooter
{
    uint32_t signature;
    uint32_t keyno;         // signature key number
    uint32_t length;        // signature length
};

// VSM file resource
struct TVSMResource
{
    uint16_t id;    // resource id
    uint16_t size;  // resource size
    char *data;     // resource data
};

// Resource header size (without data section)
#define RHSIZE          (2*sizeof(uint16_t))

#endif // VMSSTRUCT_H_INCLUDED

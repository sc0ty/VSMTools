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

#ifndef VSMRESLIST_H_INCLUDED
#define VSMRESLIST_H_INCLUDED

#include <list>
#include <stdint.h>

using namespace std;

/*** Resurce information ***/
struct TVSMInfo
{
    uint16_t id;
    char *ext;      // extension of resource file
    char *descr;    // file description
    uint16_t len;   // default file length (zero if undefined)
}; // struct TVSMInfo


/*** Resources information list ***/
class TVMSLInfo
{
public:
    TVMSLInfo();

    // Read list from "resources.txt" file, return true if succeeded
    bool load();

    // Find resource by ID, return true if found
    bool find(uint16_t id);

    // Return last found resource
    TVSMInfo &get();

private:
    bool loaded;        // if list is loaded
    list<TVSMInfo> rl;  // resources list
    list<TVSMInfo>::iterator iter;  // resources list iterator
}; // class TVMSLInfo

#endif // VSMRESLIST_H_INCLUDED

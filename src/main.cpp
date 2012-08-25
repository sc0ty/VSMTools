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

#include "main.h"
#include "files.h"
#include "extract.h"
#include "vsmreslist.h"
#include "crc.h"

#include <conio.h>
#include <cstdio>
#include <cstring>
#include <windows.h>

int main(int argc, char *argv[])
{
    puts("\n\t\tBlackBerry VSMTools v0.7\n"
         "Created by sc0ty - sc0typl@gmail.com - http://sc0ty.pl/\n");

    // no args - information screen
    if (argc <= 2) info(argc, argv);

    // extract mode:
    else if (strcasecmp(argv[1], "-e") == 0) vsmextract(argc, argv);

    // create mode:
    else if (strcasecmp(argv[1], "-c") == 0) vsmcreate(argc, argv);

    // info mode:
    else if (strcasecmp(argv[1], "-i") == 0) vsminfo(argc, argv);

    // modification mode:
    else if (strcasecmp(argv[1], "-m") == 0) vsmmodify(argc, argv);

    // incorrect arg
    else info(argc, argv);

    return 0;
} // int main(int argc, char *argv[])

void vsmextract(int argc, char *argv[])
{
    TVSMFile f;         // VSM file
    TVSMResource m;     // processed resource
    char *fdir;         // working directory
    char *fn;           // filename
    int resno = 1;      // resource number
    int x;
    char ch;
    char tmp[1024];

    // get filename
    fn = fname(argv[2]);

    // can't open file or wrong format
    if (f.open(fn) == 0) errexit("Incorrect VSM file!\n");

    // load resources list
    TVMSLInfo rlist;
    if (rlist.load() == 0) puts("Can't load 'resources.txt' file!\n");
    TVSMInfo *rinfo;

    // generating output directory name
    if (argc <= 3)
    {
        fdir = new char[strlen(argv[2]) + 6];
        strcpy(fdir, argv[2]);
        removespecialchars(fname(fdir));
        strcat(fdir, "_OUT");
    }
    else fdir = argv[3];
    x = createdir(fdir);
    if (x < 0) errexit("Can't create output folder!");

    // file information
    printf("VSM file:       %s\n", fn);
    printf("Output folder:  %s\n", fdir);

    // directory already exists
    if (x == 0)
    {
        puts("\nOutput directory already exist. Files in this folder "
             "may be overwritten. Continue? (y/N)\n");
        ch = getch();
        if ((ch != 'y') && (ch != 'Y')) return;
    }

    fileinfo(&f.header, f.computeChecksum());
    puts("");

    // write header into file
    sprintf(tmp, "%s\\header.bin", fdir);
    fsave(tmp, &f.header, sizeof(f.header));

    // read resources
    while ( (x = f.read(&m)) > -1 )
    {
        // get resource info
        if (rlist.find(m.id))
        {
            rinfo = &rlist.get();
            printf("resource %2d:  0x%04X 0x%04X (%s) - %s\n",
                   resno, m.id, m.size, rinfo->ext, rinfo->descr);
            sprintf(tmp, "%s\\res%04X_%s.%s",
                    fdir, m.id, rinfo->descr, rinfo->ext);
        }

        // there is no such resource on the list
        else
        {
            printf("resource %2d:  0x%04X 0x%04X (??\?) - UNKNOWN\n",
                   resno, m.id, m.size);
            sprintf(tmp, "%s\\res%04X.bin", fdir, m.id);
        }

        if (fsave(tmp, m.data, m.size) == 0)
            printf("Can't write file '%s'!", tmp);

        delete [] m.data;
        resno++;
    }

    // error during resource read
    if (x < -1) errexit("Incorrect VSM file!\n");

    // if footer exists
    if (f.footer_sz)
    {
        // save footer to file
        puts("");
        sprintf(tmp, "%s\\footer.bin", fdir);
        if (fsave(tmp, f.footer, f.footer_sz) == 0) puts("Can't write file 'footer.bin'!");

        // show footer info
        footerinfo(f.footer, f.footer_sz);
    }

    f.close();
    delete [] fdir;
}

void vsmcreate(int argc, char *argv[])
{
    TVSMHeader head;    // VSM file header
    TVSMResource res;   // resource
    char *fdir = NULL;  // working directory
    char *fn = NULL;    // target file name
    TCRC csum;          // checksum
    int i;
    FILE *f;
    char *tmp, *tmp2;
    char ch;

    // command line parameters
    int vendor_id = -1;
    bool dont_load_header = false;
    bool dont_incl_footer = false;
    int ins_version = 0;
    unsigned short int version[4];

    // process arguments passed by command line
    for (i=2; i<argc; i++)
    {
        // -vid
        if (strcasecmp(argv[i], "-vid") == 0)
        {
            // there is no argument after -vid
            if (i+1 >= argc) errexit("Wrong parameter format '-vid'");
            sscanf(argv[++i], "%u", &vendor_id);
        }

        // -dh
        else if (strcasecmp(argv[i], "-dh") == 0)
            dont_load_header = true;

        // -df
        else if (strcasecmp(argv[i], "-df") == 0)
            dont_incl_footer = true;

        // -v
        else if (strcasecmp(argv[i], "-v") == 0)
        {
            if (i+1 >= argc) errexit("Wrong parameter format '-v'");

            ins_version = 1;
            if (sscanf(argv[++i], "%hd.%hd.%hd.%hd", &version[3], &version[2],
                       &version[1], &version[0]) != 4)
            errexit("Wrong parameter format '-v'\nShould be '-v a.b.c.d'");
        }

        // File or directory name
        else if (i+2 >= argc)
        {
            if (fdir == NULL) fdir = argv[i];
            else fn = argv[i];
        }

        // Incorrect parameter
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            errexit("Wrong parameter format");
        }
    }

    // Directory not specified
    if (fdir == NULL) errexit("Input folder not specified");

    // Target file not specified - generating filename from input folder
    if (fn == NULL)
    {
        fn = new char[strlen(fdir) + 5];
        strcpy(fn, fdir);
        if (isSlash(fn)) fn[strlen(fn) -1] = '\0';
        strcat(fn, ".vsm");
    }

    // End directory path by '\' char:
    if (isSlash(fdir) == 0)
    {
        tmp = new char[strlen(fdir) + 2];
        strcpy(tmp, fdir);
        strcat(tmp, "\\");
        fdir = tmp;
    }

    // File information
    printf("Input  folder:  %s\n", fdir);
    printf("Output file:    %s\n", fn);

    // Load header
    if (dont_load_header == false)
    {
        // open file
        tmp = new char[strlen(fdir) + 12];
        strcpy(tmp, fdir);
        strcat(tmp, "header.bin");
        i = fload(tmp, &head, sizeof(head));
        delete [] tmp;

        // file doesn't exists
        if (i < 0)
            errexit("\nCan't open file 'header.bin'.\n"
                    "Use '-dh' switch to generate default header");
        else if (i != sizeof(head))
            errexit("\nIncorrect file 'header.bin'.\n"
                    "Use '-dh' switch to generate default header");
    }

    // generate header
    else
    {
        memset(&head, 0, sizeof(head));
        head.signature = VSMSignature;
    }

    // change vendor id
    if (vendor_id >= 0) head.vendor_id = vendor_id;

    // change version number
    if (ins_version)
    {
        for (i=0; i<4; i++)
        head.version[i] = version[i];
    }

    // Show header info
    printf("Vendor ID:      %d\n\n", head.vendor_id);

    // Check if output file already exists
    if (FileExist(fn))
    {
        printf("File %s already exist. Overwrite? (y/N)\n\n", fn);
        ch = getch();
        if ((ch != 'y') && (ch != 'Y')) exit(0);
    }

    // Open output file
    f = fopen(fn, "wb");
    if (f == NULL) errexit("Can't create output file!");

    // Write header
    puts("writing header");
    if (fwrite(&head, sizeof(head), 1, f) <= 0)
    {
        fclose(f);
        errexit("Write error!");
    }

    // Prepare to listing files
    tmp = new char[8192];
    strcpy(tmp, fdir);
    tmp2 = tmp + strlen(tmp);
    strcat(tmp, "res*");
    TFileList fl;
    char *fname = FLFindFirst(&fl, tmp);
    res.data = new char[VSMMaxResSize+1];
    csum.newCRC32();
    head.ressize = 0;

    // Write file
    while (fname)
    {
        i = ParseResName(fname);

        // given file is not a resource file - skip
        if (i < 0) continue;

        res.id = i;
        printf("writing resource 0x%04x from %s\n", res.id, fname);

        // read resource file
        strcpy(tmp2, fname);
        i = fload(tmp, res.data, VSMMaxResSize+1);
        if (i < 0) errexit("Can't read resource file!");
        res.size = i;
        if (i >= VSMMaxResSize+1) errexit("Resource file is too big!");

        // write to output file
        if (fwrite(&res, RHSIZE, 1, f) <= 0) errexit("Write error!");
        if (fwrite(res.data, res.size, 1, f) <= 0) errexit("Write error!");
        csum.add(&res, RHSIZE);
        csum.add(res.data, res.size);
        head.ressize += RHSIZE + res.size;

        // make sure that file end up with even address
        if (head.ressize & 1)
        {
            if (fwrite("\0", 1, 1, f) <= 0) errexit("Write error!");
            head.ressize++;
            csum.add(0);
        }

        fname = FLFind(&fl);
    }

    // Writing footer
    if (dont_incl_footer == false)
    {
        // footer address (divided by 4)
        i = head.ressize & 3;
        if (i)
        {
            i = 4 - i;
            if (fwrite("\0\0\0", i, 1, f) <= 0) errexit("Write error!");
        }

        // load footer
        strcpy(tmp2, "footer.bin");
        i = fload(tmp, res.data, VSMMaxResSize);

        // if footer exists
        if (i != -1)
        {
            puts("writing footer");

            // can't read footer.bin
            if (i < 0) errexit("\nCan't open file 'footer.bin'.\n"
                               "Use '-df' switch if you don't need footer");

            // footer.bin length is 0
            if (i == 0) printf("\nFile 'footer.bin' is empty.");

            // write footer and check for errors
            else if (fwrite(res.data, i, 1, f) <= 0) errexit("Write error!");
        }
    }

    // Update header checksum
    head.checksum = csum.getCRC32() ^ 0xffffffff;
    puts("updating header (checksum and resources size)\n");
    fseek(f, 0, SEEK_SET);
    if (fwrite(&head, sizeof(head), 1, f) <= 0) errexit("Write error!");

    fclose(f);
    fileinfo(&head, head.checksum);

    delete [] tmp;
    delete [] res.data;
} // void vsmcreate(int argc, char *argv[])

void vsminfo(int argc, char *argv[])
{
    // filename
    char *fn = argv[2];

    // load file
    TVSMFile f;
    if (!f.open(fn))
    errexit("Can't open file or it is not correct VSM file!");

    // file info
    printf("VSM file:       %s\n", fn);
    fileinfo(&f.header, f.computeChecksum());

    // load resources info
    TVMSLInfo linfo;
    if (linfo.load() == 0) puts("\nCan't load file 'resources.txt'");
    TVSMInfo *info;

    // resources list
    int i = 0;
    TVSMResource res;
    int r = f.read(&res);

    puts("\nResource list:");
    puts("  #no   id       size   default file extension and description");
    puts("  ---  ------    ----   --------------------------------------");

    while (r >= 0)
    {
        i++;
        printf(" %4d  0x%04X  %6u   ", i, res.id, res.size);

        if (linfo.find(res.id))
        {
            info = &linfo.get();
            printf("(%s)\t%s\n", info->ext, info->descr);
        }

        else puts("<UNKNOWN RESOURCE ID>");

        delete [] res.data;
        r = f.read(&res);
    }

    // show footer info (if footer exists)
    if (f.footer_sz)
    {
        puts("");
        footerinfo(f.footer, f.footer_sz);
    }

    // there isno footer section
    else puts("\nThere is no footer included in this file.");

    delete [] res.data;
    f.close();
} // void vsminfo(int argc, char *argv[])


void vsmmodify(int argc, char *argv[])
{
    int i;

    if (argc <= 3)
    {
        puts("Specific what to modificate\n");
        info(argc, argv);
    }

    // filename
    char *fn = argv[argc-1];

    // parameters passed by command line arguments
    int vendor_id = -1;
    bool recalculate_checksum = false;
    bool no_backup = false;
    bool set_version = false;
    unsigned short int version[4];
    char *header_file = NULL;

    // process arguments passed by command line
    for (i=2; i<argc-1; i++)
    {
        // -vid
        if (strcasecmp(argv[i], "-vid") == 0)
        {
            if (i+1 >= argc) errexit("Wrong parameter format '-vid'");
            sscanf(argv[++i], "%u", &vendor_id);
        }

        // -cs
        else if (strcasecmp(argv[i], "-cs") == 0)
            recalculate_checksum = true;

        // -db
        else if (strcasecmp(argv[i], "-db") == 0)
            no_backup = true;

        // -v
        else if (strcasecmp(argv[i], "-v") == 0)
        {
            if (i+1 >= argc) errexit("Wrong parameter format '-v'");

            set_version = true;
            if (sscanf(argv[++i], "%hd.%hd.%hd.%hd", &version[3], &version[2],
                       &version[1], &version[0]) != 4)
            errexit("Wrong parameter format '-v'\nShould be '-v a.b.c.d'");
        }

        // -h
        else if (strcasecmp(argv[i], "-h") == 0)
        {
            if (i+1 >= argc) errexit("Wrong parameter format '-h'");
            header_file = argv[++i];
        }

        // incorrect argument
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            errexit("Wrong parameter format");
        }
    }

    // Backup file
    if (!no_backup)
    {
        puts("Making backup...");
        char *bn = new char[strlen(fn) + 5];
        strcpy(bn, fn);
        strcat(bn, ".bak");

        // file already exists
        if (FileExist(bn))
            printf("Backup file already exist (file: %s)\n\n", bn);

        // copy file
        else
        {
            if (fcopy(fn, bn))
                printf("Backup file created (file: %s)\n\n", bn);

            else
            {
                printf("Can't create backup file (file: %s)!\n\n", bn);
                errexit("To skip backuping use '-db' switch");
            }
        }
    }

    TVSMHeader head;
    bool update_header = 0;

    FILE *f = fopen(fn, "rb+");
    if (f == NULL) errexit("Can't open input file!");

    // load header from original file
    if (header_file == NULL)
    {
        if (fread(&head, sizeof(head), 1, f) <= 0)
        {
            fclose(f);
            errexit("This is not a valid VSM file!");
        }
    }

    // load header from external file
    else
    {
        FILE *fh = fopen(header_file, "rb");
        if (fh == NULL) errexit("Can't read header file!");

        if (fread(&head, sizeof(head), 1, f) <= 0)
        {
            fclose(f);
            fclose(fh);
            errexit("Incorrect header file!");
        }

        fclose(fh);
        printf("Read header from file %s\n", header_file);
        update_header = 1;
    }

    // check header signature
    if (head.signature != VSMSignature)
    {
        fclose(f);
        errexit("Incorrect header signature!");
    }

    // update vendor id
    if (vendor_id >= 0)
    {
        head.vendor_id = vendor_id;
        update_header = 1;
        printf("Updated Vendor ID:    %u\n", vendor_id);
    }

    // update version number
    if (set_version)
    {
        for (i=0; i<4; i++) head.version[i] = version[i];
        update_header = 1;
        printf("Updated Version no:   %u.%u.%u.%u\n",
               version[3], version[2], version[1], version[0]);
    }

    // update checksum
    if (recalculate_checksum)
    {
        fseek(f, sizeof(head), SEEK_SET);
        char *buf = new char[head.ressize];

        if (fread(buf, head.ressize, 1, f) <= 0)
        {
            errexit("Incorrect VSM file! Broken resources section");
            delete [] buf;
            fclose(f);
        }

        TCRC csum;
        csum.newCRC32();
        head.checksum = ~csum.add(buf, head.ressize);
        delete [] buf;

        printf("Updated Checksum:     0x%08X\n", head.checksum);
        update_header = 1;
    }

    // write header
    if (update_header)
    {
        fseek(f, 0, SEEK_SET);
        if (fwrite(&head, sizeof(head), 1, f) >= 1)
        puts("\nHeader updated successful\n");
        else puts("\nCan't write modified header!\n");
    }

    fclose(f);
} // void vsmmodify(int argc, char *argv[])

void removespecialchars(char *str)
{
    int i = 0;

    while (str[i] != 0)
    {
        if (((str[i] < '0') || (str[i] > '9')) &&
            ((str[i] < 'a') || (str[i] > 'z')) &&
            ((str[i] < 'A') || (str[i] > 'Z')) &&
             (str[i] != '-'))
             str[i] = '_';
        i++;
    }
} // void removespecialchars(char *str)

void info(int argc, char *argv[])
{
    int s = 0;

    if (argc >= 1)
    {
        if (strcasecmp(argv[1], "-e") == 0) s = 1;
        if (strcasecmp(argv[1], "-c") == 0) s = 2;
        if (strcasecmp(argv[1], "-i") == 0) s = 3;
        if (strcasecmp(argv[1], "-m") == 0) s = 4;
    }

    if ((s == 0) || (s == 1))
    {
        puts("To extract VSM file use:\n"
             "  vsmtools -e VSM_FILE.VSM [OUTPUT_DIR]\n");
    }

    if ((s == 0) || (s == 2))
    {
        puts("To create VSM file use:\n"
            "  vsmtools -c [options] INPUT_DIR [VSN_FILE.VSM]\n"
            "     options:\n"
            "     -vid x      - set Vendor ID to x (decimal value)\n"
            "     -dh         - generate default header (don't load header.bin)\n"
            "     -df         - don't include footer section\n"
            "     -v x.x.x.x  - set verson file (format: 1.0.0.123)\n");
    }

    if ((s == 0) || (s == 3))
    {
        puts("To show VSM file information use:\n"
             "  vsmtools -i VSN_FILE.VSM\n");
    }

    if ((s == 0) || (s == 4))
    {
        puts("To modify VSM file use:\n"
            "  vsmtools -m [options] VSM_FILE.VSM\n"
            "     options:\n"
            "     -h file.bin - load header form file (header.bin or other VSM file)\n"
            "     -vid x      - set Vendor ID to x (decimal value)\n"
            "     -cs         - calculate and write correct checksum\n"
            "     -v x.x.x.x  - set verson file (format: 1.0.0.123)\n"
            "     -db         - don't backup modified file\n");
    }

    puts("Copyright (C) 2012, Michal Szymaniak\n"
         "This program comes with ABSOLUTELY NO WARRANTY; "
         "for details read LICENSE file");
} // void info(int argc, char *argv[])

void errexit(const char *err)
{
    puts(err);
    exit(1);
} // void errexit(const char *err)

void fileinfo(TVSMHeader *header, uint32_t csum)
{
    // check if signature is valid
    printf("Signature:      0x%8X ", header->signature);
    if (header->signature == VSMSignature) puts("(correct)");
    else puts("(incorrect, it's not valid VSM file)");

    // check checksum
    printf("Checksum:       0x%08X ", header->checksum);
    if (csum == header->checksum) puts("(correct)");
    else printf("(incorrect, should be: 0x%08X)\n", csum);

    printf("Resources size: %u\n", header->ressize);
    printf("Vendor ID:      %d\n", header->vendor_id);
    printf("Version:        %u.%u.%u.%u\n", header->version[3],
           header->version[2], header->version[1], header->version[0]);
} // void fileinfo(TVSMHeader *header, uint32_t csum)

void footerinfo(void *footer, uint32_t len)
{
    if (len < sizeof(TVSMFooter))
    {
        puts("Incorrect footer section (too small)");
        return;
    }

    TVSMFooter *foot = (TVSMFooter *)footer;

    // check if signature is valid
    if (foot->signature != VSMFSignature)
    {
        puts("Incorrect footer section (wrong signature)");
        return;
    }

    printf("Sign key no:    0x%X\n", foot->keyno);
    printf("Sign length:    %d\n", foot->length);
} // void footerinfo(void *footer, uint32_t len)

int ParseResName(const char *rname)
{
    int sz = strlen(rname);
    if (sz < 4) return -1;

    int r = 0;
    int i;
    char d;

    for (i=3; i<sz; i++)
    {
        d = rname[i];

        if ((d >= '0') && (d <= '9'))
            r = 16*r + d - '0';

        else if ((d >= 'a') && (d <= 'f'))
            r = r*16 + d - 'a' + 10;

        else if ((d >= 'A') && (d <= 'F'))
            r = r*16 + d - 'A' + 10;

        // there is no valid number
        else if (i <= 3) return -2;

        // end of string
        else return r;
    }

    return r;
} // int ParseResName(const char *rname)

int hex2int(char hex)
{
    if ((hex >= '0') && (hex <= '9'))
        return hex - '0';

    if ((hex >= 'a') && (hex <= 'z'))
        return hex - 'a' + 10;

    if ((hex >= 'A') && (hex <= 'Z'))
        return hex - 'A' + 10;

    // incorrect char
    return -1;
} // int hex2int(char hex)

int hex2bin(const char *hex, char *buf, int len)
{
    int h = 0, b = 0;
    int x, y;

    while (b < len)
    {
        x =  hex2int( hex[h++] );
        if (x < 0) return b;

        y = hex2int( hex[h++] );
        if (y < 0) return b;

        buf[b++] = 16*x + y;
    }

    return b;
} // int hex2bin(const char *hex, char *buf, int len)

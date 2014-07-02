//
//  main.m
//  transfer
//
//  Created by Philippe Hausler on 10/27/12.
//  Copyright (c) 2012 Apportable. All rights reserved.
//

/*
 WARNING: This only works for certain devices; they must support MTP over usb to use this. However
 test transfer times have been clocked at around 200+ MB applications being transfered in under 3s!
 */


#include <fcntl.h>
#include <string.h>
#include <libmtp.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#define SIZE_BUFSZ 7
static char const SIZE_PREFIXES[] = "kMGTPEZY";

void
format_size(char buf[SIZE_BUFSZ], uint64_t sz)
{
    int pfx = 0;
    uint64_t m, n, rem, hrem;
    uint64_t a;
    if (sz <= 0) {
        memcpy(buf, "0 B", 3);
        return;
    }
    a = sz;
    if (a < 1000) {
        n = a;
        snprintf(buf, SIZE_BUFSZ, "%llu B", n);
        return;
    }
    for (pfx = 0, hrem = 0; ; pfx++) {
        rem = a % 1000ULL;
        a = a / 1000ULL;
        if (!SIZE_PREFIXES[pfx + 1] || a < 1000ULL)
            break;
        hrem |= rem;
    }
    n = a;
    if (n < 10) {
        if (rem >= 950) {
            buf[0] = '1';
            buf[1] = '0';
            buf[2] = ' ';
            buf[3] = SIZE_PREFIXES[pfx];
            buf[4] = 'B';
            buf[5] = '\0';
            return;
        } else {
            m = rem / 100;
            rem = rem % 100;
            if (rem > 50 || (rem == 50 && ((m & 1) || hrem)))
                m++;
            snprintf(buf, SIZE_BUFSZ,
                     "%llu.%llu %cB", n, m, SIZE_PREFIXES[pfx]);
        }
    } else {
        if (rem > 500 || (rem == 500 && ((n & 1) || hrem)))
            n++;
        if (n >= 1000 && SIZE_PREFIXES[pfx + 1]) {
            buf[0] = '1';
            buf[1] = '.';
            buf[2] = '0';
            buf[3] = ' ';
            buf[4] = SIZE_PREFIXES[pfx+1];
            buf[5] = 'B';
            buf[6] = '\0';
        } else {
            snprintf(buf, SIZE_BUFSZ,
                     "%llu %cB", n, SIZE_PREFIXES[pfx]);
        }
    }
}

int progressfunc(uint64_t const sent, uint64_t const total, void const * const data)
{
    char sentStr[SIZE_BUFSZ];
    char totalStr[SIZE_BUFSZ];
    format_size(sentStr, sent);
    format_size(totalStr, total);
    uint64_t percent = (sent*100)/total;
    printf("Progress: %s of %s (%llu%%)                                \r", sentStr, totalStr, percent);
    fflush(stdout);
    return 0;
}

LIBMTP_filetype_t find_filetype(const char * filename)
{
    char *ptype;
    LIBMTP_filetype_t filetype;
    
    ptype = rindex(filename,'.');
    
    if (!ptype) {
        ptype = "";
    } else {
        ++ptype;
    }
    
    if (!strcasecmp (ptype, "wav")) {
        filetype = LIBMTP_FILETYPE_WAV;
    } else if (!strcasecmp (ptype, "mp3")) {
        filetype = LIBMTP_FILETYPE_MP3;
    } else if (!strcasecmp (ptype, "wma")) {
        filetype = LIBMTP_FILETYPE_WMA;
    } else if (!strcasecmp (ptype, "ogg")) {
        filetype = LIBMTP_FILETYPE_OGG;
    } else if (!strcasecmp (ptype, "mp4")) {
        filetype = LIBMTP_FILETYPE_MP4;
    } else if (!strcasecmp (ptype, "wmv")) {
        filetype = LIBMTP_FILETYPE_WMV;
    } else if (!strcasecmp (ptype, "avi")) {
        filetype = LIBMTP_FILETYPE_AVI;
    } else if (!strcasecmp (ptype, "mpeg") || !strcasecmp (ptype, "mpg")) {
        filetype = LIBMTP_FILETYPE_MPEG;
    } else if (!strcasecmp (ptype, "asf")) {
        filetype = LIBMTP_FILETYPE_ASF;
    } else if (!strcasecmp (ptype, "qt") || !strcasecmp (ptype, "mov")) {
        filetype = LIBMTP_FILETYPE_QT;
    } else if (!strcasecmp (ptype, "wma")) {
        filetype = LIBMTP_FILETYPE_WMA;
    } else if (!strcasecmp (ptype, "jpg") || !strcasecmp (ptype, "jpeg")) {
        filetype = LIBMTP_FILETYPE_JPEG;
    } else if (!strcasecmp (ptype, "jfif")) {
        filetype = LIBMTP_FILETYPE_JFIF;
    } else if (!strcasecmp (ptype, "tif") || !strcasecmp (ptype, "tiff")) {
        filetype = LIBMTP_FILETYPE_TIFF;
    } else if (!strcasecmp (ptype, "bmp")) {
        filetype = LIBMTP_FILETYPE_BMP;
    } else if (!strcasecmp (ptype, "gif")) {
        filetype = LIBMTP_FILETYPE_GIF;
    } else if (!strcasecmp (ptype, "pic") || !strcasecmp (ptype, "pict")) {
        filetype = LIBMTP_FILETYPE_PICT;
    } else if (!strcasecmp (ptype, "png")) {
        filetype = LIBMTP_FILETYPE_PNG;
    } else if (!strcasecmp (ptype, "wmf")) {
        filetype = LIBMTP_FILETYPE_WINDOWSIMAGEFORMAT;
    } else if (!strcasecmp (ptype, "ics")) {
        filetype = LIBMTP_FILETYPE_VCALENDAR2;
    } else if (!strcasecmp (ptype, "exe") || !strcasecmp (ptype, "com") ||
               !strcasecmp (ptype, "bat") || !strcasecmp (ptype, "dll") ||
               !strcasecmp (ptype, "sys")) {
        filetype = LIBMTP_FILETYPE_WINEXEC;
    } else if (!strcasecmp (ptype, "aac")) {
        filetype = LIBMTP_FILETYPE_AAC;
    } else if (!strcasecmp (ptype, "mp2")) {
        filetype = LIBMTP_FILETYPE_MP2;
    } else if (!strcasecmp (ptype, "flac")) {
        filetype = LIBMTP_FILETYPE_FLAC;
    } else if (!strcasecmp (ptype, "m4a")) {
        filetype = LIBMTP_FILETYPE_M4A;
    } else if (!strcasecmp (ptype, "doc")) {
        filetype = LIBMTP_FILETYPE_DOC;
    } else if (!strcasecmp (ptype, "xml")) {
        filetype = LIBMTP_FILETYPE_XML;
    } else if (!strcasecmp (ptype, "xls")) {
        filetype = LIBMTP_FILETYPE_XLS;
    } else if (!strcasecmp (ptype, "ppt")) {
        filetype = LIBMTP_FILETYPE_PPT;
    } else if (!strcasecmp (ptype, "mht")) {
        filetype = LIBMTP_FILETYPE_MHT;
    } else if (!strcasecmp (ptype, "jp2")) {
        filetype = LIBMTP_FILETYPE_JP2;
    } else if (!strcasecmp (ptype, "jpx")) {
        filetype = LIBMTP_FILETYPE_JPX;
    } else if (!strcasecmp (ptype, "bin")) {
        filetype = LIBMTP_FILETYPE_FIRMWARE;
    } else if (!strcasecmp (ptype, "vcf")) {
        filetype = LIBMTP_FILETYPE_VCARD3;
    } else {
        filetype = LIBMTP_FILETYPE_UNKNOWN;
    }
    
    return filetype;
}


static void usage()
{
    printf("transfer <file>\n");
}

static int run_command(const char *fmt, ...)
{
    char *buffer = NULL;
    va_list args;
    va_start(args, fmt);
    int r = vasprintf(&buffer, fmt, args);
    va_end(args);
    
    if (r == -1)
    {
        return -1;
    }
    
    r = system(buffer);
    
    free(buffer);
    
    return r;
}

int main(int argc, const char * argv[])
{
    int err = 0;
    LIBMTP_folder_t *folders = NULL;
    LIBMTP_file_t *files = NULL;
    if (argc < 2)
    {
        usage();
        return -1;
    }
    const char *from_path = argv[1];
    
    printf("Searching for devices...\r");
    fflush(stdout);
    LIBMTP_Init();
    LIBMTP_mtpdevice_t *device = LIBMTP_Get_First_Device();
    
    char *adb_command = getenv("ADB_PATH");
    if (adb_command == NULL)
    {
        char *sdk_root = getenv("ANDROID_SDK");
        if (sdk_root != NULL)
        {
            asprintf(&adb_command, "%s/platform-tools/adb", sdk_root);
            if (access(adb_command, F_OK) == -1)
            {
                // File doesn't exist
                free(adb_command);
                adb_command = NULL;
            }
        }
    }
    if (adb_command == NULL)
    {
        adb_command = "adb";
    }
    
    int transfer_succeeded = 0;
    
    if (device != NULL)
    {
        char *filename = basename((char *)from_path);
        printf("Found %s %s (%s)\n", LIBMTP_Get_Manufacturername(device), LIBMTP_Get_Modelname(device), LIBMTP_Get_Serialnumber(device));
        fflush(stdout);
        files = LIBMTP_Get_Filelisting_With_Callback (device, NULL, NULL);
        for (LIBMTP_file_t *file = files; file != NULL; file = file->next)
        {
            if (!file->filename || strcmp(file->filename, filename) == 0)
            {
                printf("Removing previous transfer cache from device.\n");
                LIBMTP_Delete_Object(device, file->item_id);
            }
        }
        folders = LIBMTP_Get_Folder_List(device);
        if (folders == NULL) {
            fprintf(stderr, "Could not get device folders");
            return -1;
        }

        LIBMTP_file_t *f = LIBMTP_new_file_t();
        struct stat sb;
        
        
        if ( stat(from_path, &sb) == -1 ) {
            fprintf(stderr, "%s: not found.", from_path);
            return -1;
        }
        
        f->filesize = sb.st_size;
        f->filename = strdup(filename);
        f->filetype = find_filetype(filename);
        f->parent_id = folders->parent_id;
        f->storage_id = folders->storage_id;
        
        int fd = open(argv[1], O_RDONLY);
        int send_succeeded = LIBMTP_Send_File_From_File_Descriptor(device, fd, f, &progressfunc, NULL) == 0;
        LIBMTP_Release_Device(device);
        if (send_succeeded)
        {
            printf("Transfer complete.                                                          \n"); // clear out the progress too
            //sleep(1);  // install can fail to find device if it comes too soon after the transfer
            
            err = run_command("\"%s\" shell pm install -r /sdcard/%s", adb_command, filename);
            if (err == 0)
            {
                printf("Install complete!\n");
                transfer_succeeded = 1;
            }
            else
            {
                printf("Install failed.\n");
            }
            if (run_command("\"%s\" shell rm /sdcard/%s", adb_command, filename) != 0)
            {
                printf("Failed to remove intermediate transfer file. Try 'adb shell rm /sdcard/%s'\n", filename);
            }
        }
        if (!transfer_succeeded)
        {
            printf("Transfer installation failed. Trying to use adb instead. Use MTP=no to skip trying transfer\n");
        }
    }
    if (device == NULL)
    {
        printf("No mtp compatible devices found: Falling back to slow path\n");
    }
    if (!transfer_succeeded)
    {
        err = run_command("\"%s\" install -r %s", adb_command, from_path);
    }

    return err;
}


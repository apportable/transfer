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


int main(int argc, const char * argv[])
{
    int err = 0;
    LIBMTP_folder_t *folders = NULL;
    LIBMTP_file_t *files = NULL;

    const char *from_path = argv[1];
    printf("Searching for devices...\r");
    fflush(stdout);
    LIBMTP_Init();
    LIBMTP_mtpdevice_t *device = LIBMTP_Get_First_Device();
    if (device != NULL)
    {
        char *filename = basename((char *)from_path);
        printf("Found %s %s (%s)\n", LIBMTP_Get_Manufacturername(device), LIBMTP_Get_Modelname(device), LIBMTP_Get_Serialnumber(device));
        fflush(stdout);
        files = LIBMTP_Get_Filelisting_With_Callback (device, NULL, NULL);
        for (LIBMTP_file_t *file = files; file != NULL; file = file->next)
        {
            if (strcmp(file->filename, filename) == 0)
            {
                printf("Removing previous transfer cache from device.\n");
                LIBMTP_Delete_Object(device, file->item_id);
            }
        }
        folders = LIBMTP_Get_Folder_List(device);

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
        if(LIBMTP_Send_File_From_File_Descriptor(device, fd, f, &progressfunc, NULL) == 0)
        {
            printf("Transfer complete.                                                          \n"); // clear out the progress too
            char *sdk_root = getenv("ANDROID_SDK");
            char *cmd = (char *)calloc(1, 1024);
            if (sdk_root == NULL)
            {
                sprintf(cmd, "adb shell pm install -r /sdcard/%s", filename);
            }
            else
            {
                sprintf(cmd, "%s/platform-tools/adb shell pm install -r /sdcard/%s", sdk_root, filename);
            }
            system(cmd);
            free(cmd);
            printf("Install complete.\n");
        }
        else
        {
            printf("Transfer failed.\n");
            err = -2;
        }
        LIBMTP_Release_Device(device);
    }
    else
    {
        printf("No mtp compatable devices found: Falling back to slow path\n");
        char *sdk_root = getenv("ANDROID_SDK");
        char *cmd = (char *)calloc(1, 1024);
        if (sdk_root == NULL)
        {
            sprintf(cmd, "adb install -r %s", from_path);
        }
        else
        {
            sprintf(cmd, "%s/platform-tools/adb install -r %s", sdk_root, from_path);
        }
        system(cmd);
        free(cmd);
        err = -3;
    }

    return err;
}


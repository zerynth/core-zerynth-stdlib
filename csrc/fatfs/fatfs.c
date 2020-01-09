
#include "zerynth.h"
#include "ff.h"
#include "diskio.h"

// #include "vbl.h"
//
// #define printf(...) vbl_printf_stdout(__VA_ARGS__)


FATFS FatFs[_VOLUMES];
uint32_t mnt_cnt = 0;

FIL fil[4];
DIR dir[4];

#define GET_NAMED_PYPATH(arg,name) \
    uint8_t *__##name = PSEQUENCE_BYTES(arg);  \
    int __##name##len = PSEQUENCE_ELEMENTS(arg); \
    uint8_t *name = gc_malloc(__##name##len+1);  \
    memcpy(name,__##name,__##name##len);           \
    name[__##name##len]=0

#define GET_PYPATH(arg) GET_NAMED_PYPATH(arg,path)

#define DEL_NAMED_PYPATH(name) gc_free(name)

#define DEL_PYPATH() DEL_NAMED_PYPATH(path)
// Volume Management

C_NATIVE(__f_mount) {
    NATIVE_UNWARN();
    FRESULT fr;
    GET_PYPATH(args[0]);
    printf("mounting path %i %s\n",__pathlen,path);
    if (mnt_cnt < _VOLUMES) {
        fr = f_mount(&FatFs[mnt_cnt], path, 0);
        if (fr != 0) {
            DEL_PYPATH();
            return ERR_VALUE_EXC;
        }
        mnt_cnt += 1;
        DEL_PYPATH();
        return ERR_OK;
    }
    DEL_PYPATH();
    return ERR_VALUE_EXC;
}

// File Access

C_NATIVE(__f_open) {
    NATIVE_UNWARN();
    FRESULT fr;
    GET_PYPATH(args[0]);
    uint8_t flag  = (uint8_t)PSMALLINT_VALUE(args[1]);
    uint8_t n     = (uint8_t)PSMALLINT_VALUE(args[2]);
    printf("opening file %i %s\n",__pathlen,path);

    fr = f_open(&fil[n], path, flag);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
    }
    gc_free(path);
    return ERR_OK;
}

C_NATIVE(__f_close) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t n = (uint8_t)PSMALLINT_VALUE(args[0]);
    fr = f_close(&fil[n]);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
    }
    return ERR_OK;
}

C_NATIVE(__f_size) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t n = (uint8_t)PSMALLINT_VALUE(args[0]);
    *res = PSMALLINT_NEW(f_size(&fil[n]));
    return ERR_OK;
}

C_NATIVE(__f_tell) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t n = (uint8_t)PSMALLINT_VALUE(args[0]);
    *res = PSMALLINT_NEW(f_tell(&fil[n]));
    return ERR_OK;
}

C_NATIVE(__f_read) {
    NATIVE_UNWARN();
    FRESULT fr;          /* FatFs function common result code */
    UINT br;         /* File read/write count */
    uint16_t to_read = PSMALLINT_VALUE(args[0]);
    uint8_t mode = (uint8_t)PSMALLINT_VALUE(args[1]);
    uint8_t n = (uint8_t)PSMALLINT_VALUE(args[2]);

    PObject *buffer = pbytes_new(to_read, NULL);
    fr = f_read(&fil[n], PSEQUENCE_BYTES(buffer), to_read, &br);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
    }
    else {
        if (mode == 1) {
            *res =  (PObject *)pbytes_new(br, PSEQUENCE_BYTES(buffer));
        }
        else {
            *res = (PObject *)pstring_new(br, PSEQUENCE_BYTES(buffer));
        }
    }
    return ERR_OK;
}

// C_NATIVE(__f_readline) {
//     NATIVE_UNWARN();
//     BYTE buffer[100];   /* File copy buffer */
//     FRESULT fr;          /* FatFs function common result code */
//     UINT br;         /* File read/write count */
//     int cnt = -1;
//     uint8_t n = PSMALLINT_VALUE(args[0]);
//     do {
//         cnt++;
//         fr = f_read(&fil[n], &buffer[cnt], 1, &br);
//     } while (cnt < 99 && br != 0 && buffer[cnt] != '\n');
//     *res = (PObject *)pstring_new(cnt, buffer);
//     return ERR_OK;
// }

C_NATIVE(__f_write) {
    NATIVE_UNWARN();
    FRESULT fr;          /* FatFs function common result code */
    UINT bw;         /* File read/write count */
    uint32_t len = PSEQUENCE_ELEMENTS(args[0]);
    uint8_t sync = PBOOL_VALUE(args[1]);
    uint8_t n    = (uint8_t)PSMALLINT_VALUE(args[2]);
    fr = f_write(&fil[n], (BYTE*)PSEQUENCE_BYTES(args[0]), len, &bw);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
        return ERR_OK;
    }
    else {
        *res = PSMALLINT_NEW(bw);
    }
    if (sync) {
        fr = f_sync(&fil[n]);
    }
    return ERR_OK;
}

C_NATIVE(__f_seek) {
    NATIVE_UNWARN();
    FRESULT fr;          /* FatFs function common result code */
    DWORD pos = PSMALLINT_VALUE(args[0]);
    uint8_t n    = (uint8_t)PSMALLINT_VALUE(args[1]);
    fr = f_lseek(&fil[n], pos);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
        return ERR_OK;
    }
    return ERR_OK;
}

C_NATIVE(__f_truncate) {
    NATIVE_UNWARN();
    FRESULT fr;          /* FatFs function common result code */
    uint8_t n    = (uint8_t)PSMALLINT_VALUE(args[0]);
    fr = f_truncate(&fil[n]);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
        return ERR_OK;
    }
    return ERR_OK;
}

C_NATIVE(__f_eof) {
    NATIVE_UNWARN();
    uint8_t n    = (uint8_t)PSMALLINT_VALUE(args[0]);
    int is_eof;
    is_eof = f_eof(&fil[n]);
    if (is_eof == 0) {
        *res = PBOOL_FALSE();
    }
    else {
        *res = PBOOL_TRUE();
    }
    return ERR_OK;
}

// Directory Access

C_NATIVE(__f_opendir) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t *lpath = PSEQUENCE_BYTES(args[0]);
    uint8_t n     = (uint8_t)PSMALLINT_VALUE(args[1]);
    int pathlen = PSEQUENCE_ELEMENTS(args[0]);
    uint8_t *path = gc_malloc(pathlen+1);
    memcpy(path,lpath,pathlen);
    path[pathlen]=0;
    printf("opening file %i %s\n",pathlen,path);
    fr = f_opendir(&dir[n], path);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_closedir) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t n     = (uint8_t)PSMALLINT_VALUE(args[0]);
    fr = f_closedir(&dir[n]);
    if (fr != 0) {
        *res = PSMALLINT_NEW(-1);
    }
    return ERR_OK;
}

// C_NATIVE(__f_listdir) {
//     NATIVE_UNWARN();
//     FRESULT fr;
//     uint8_t n     = PSMALLINT_VALUE(args[0]);
//     static FILINFO fno;
//
//     // TODO: malloc??
//     PObject *entries[50];
//     int n_entries = 0;
//
//     while (n_entries < 50) {
//         fr = f_readdir(&dir[n], &fno);                   /* Read a directory item */
//         if (fr != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
//         entries[n_entries] = pstring_new(strlen(fno.fname), fno.fname);
//         n_entries += 1;
//     }
//     *res = plist_new(n_entries, entries);
//     return ERR_OK;
// }

C_NATIVE(__f_readdir) {
    NATIVE_UNWARN();
    FRESULT fr;
    uint8_t n     = (uint8_t)PSMALLINT_VALUE(args[0]);
    FILINFO fno;

    fr = f_readdir(&dir[n], &fno);                   /* Read a directory item */
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    else {
        if (fno.fname[0] == 0) {
            *res = PSMALLINT_NEW(0);
        }
        else {
            *res = pstring_new(strlen(fno.fname), fno.fname);
        }
    }
    return ERR_OK;
}

// File/Directory Management

C_NATIVE(__f_exists) {
    NATIVE_UNWARN();
    FRESULT fr;
    FILINFO fno;
    GET_PYPATH(args[0]);
    printf("check exists file %i %s\n",__pathlen,path);
    fr = f_stat(path, &fno);
    if (fr == FR_OK) {
        *res = PBOOL_TRUE();
    }
    else {
        *res = PBOOL_FALSE();
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_isdir) {
    NATIVE_UNWARN();
    FRESULT fr;
    FILINFO fno;
    GET_PYPATH(args[0]);
    printf("isdir %i %s\n",__pathlen,path);
    fr = f_stat(path, &fno);
    if (fr == FR_OK) {
        if (fno.fattrib & AM_DIR) {
            *res = PBOOL_TRUE();
        }
        else {
            *res = PBOOL_FALSE();
        }
    }
    else {
        *res = PBOOL_FALSE();
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_copy) {
    NATIVE_UNWARN();
    FRESULT fr_1, fr_2;          /* FatFs function common result code */
    UINT br, bw;         /* File read/write count */
    BYTE *buffer;   /* File copy buffer */
    // uint8_t* src = PSEQUENCE_BYTES(args[0]);
    // uint8_t* dst = PSEQUENCE_BYTES(args[1]);
    uint32_t n_src = (uint8_t)PSMALLINT_VALUE(args[2]);
    uint32_t n_dst = (uint8_t)PSMALLINT_VALUE(args[3]);
    GET_NAMED_PYPATH(args[0],src);
    GET_NAMED_PYPATH(args[1],dst);
    fr_1 = f_open(&fil[n_src], src, FA_READ);
    fr_2 = f_open(&fil[n_dst], dst, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr_1 != FR_OK || fr_2 != FR_OK) {
        *res = PSMALLINT_NEW(-1);
        DEL_NAMED_PYPATH(src);
        DEL_NAMED_PYPATH(dst);
        return ERR_OK;
    }
    buffer = malloc(128 * sizeof(BYTE));
    if (buffer == NULL) {
        DEL_NAMED_PYPATH(src);
        DEL_NAMED_PYPATH(dst);
        return ERR_VALUE_EXC;
    }
    for (;;) {
        fr_1 = f_read(&fil[n_src], buffer, sizeof buffer, &br);  /* Read a chunk of source file */
        if (fr_1 || br == 0) break; /* error or eof */
        fr_2 = f_write(&fil[n_dst], buffer, br, &bw);            /* Write it to the destination file */
        if (fr_2 || bw < br) break; /* error or disk full */
    }
    free(buffer);

    /* Close open files */
    f_close(&fil[n_src]);
    f_close(&fil[n_dst]);

    if (fr_1 != FR_OK || fr_2 != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_NAMED_PYPATH(src);
    DEL_NAMED_PYPATH(dst);
    return ERR_OK;
}

C_NATIVE(__f_unlink) {
    NATIVE_UNWARN();
    FRESULT fr;
    GET_PYPATH(args[0]);
    printf("unlink %i %s\n",__pathlen,path);
    fr = f_unlink(path);
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_rename) {
    NATIVE_UNWARN();
    FRESULT fr;
    // uint8_t *src_path = PSEQUENCE_BYTES(args[0]);
    // uint8_t *dst_path = PSEQUENCE_BYTES(args[1]);
    GET_NAMED_PYPATH(args[0],src_path);
    GET_NAMED_PYPATH(args[1],dst_path);
    fr = f_rename(src_path, dst_path);
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_NAMED_PYPATH(src_path);
    DEL_NAMED_PYPATH(dst_path);
    return ERR_OK;
}

C_NATIVE(__f_mkdir) {
    NATIVE_UNWARN();
    FRESULT fr;
    GET_PYPATH(args[0]);
    printf("mkdir %i %s\n",__pathlen,path);
    fr = f_mkdir(path);
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_chdir) {
    NATIVE_UNWARN();
    FRESULT fr;
    GET_PYPATH(args[0]);
    printf("chdir %i %s\n",__pathlen,path);
    fr = f_chdir(path);
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    DEL_PYPATH();
    return ERR_OK;
}

C_NATIVE(__f_getcwd) {
    NATIVE_UNWARN();
    FRESULT fr;
    TCHAR *buffer;
    uint16_t max_l = PSMALLINT_VALUE(args[0]);
    buffer = malloc(max_l * sizeof(TCHAR));
    if (buffer == NULL) {
        return ERR_VALUE_EXC;
    }
    fr = f_getcwd(buffer, max_l);
    if (fr != FR_OK) {
        *res = PSMALLINT_NEW(-1);
    }
    *res = (PObject *)pstring_new(strlen(buffer), buffer);
    free(buffer);

    return ERR_OK;
}

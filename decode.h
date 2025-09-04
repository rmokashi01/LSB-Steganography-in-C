#include <stdlib.h>
#include <stdio.h>
#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "common.h"


typedef struct _DecodeInfo
{
    

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output file Info */
    char output_fname[MAX_OUTPUT_FILENAME];  // Optional name, default = "decoded.txt"
    FILE *fptr_output;

    /* Decoded data info */
    char extn_secret_file[10]; // e.g. .txt
    long size_secret_file;

} DecodeInfo;

/* Function prototype */
Status do_decoding(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif

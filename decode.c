#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    printf("INFO: Opening required files\n");

    // Open stego image
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (!decInfo->fptr_stego_image) 
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->stego_image_fname);

    // Skip BMP header
    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0) 
    {
        perror("fseek");
        fprintf(stderr, "ERROR: Cannot skip BMP header\n");
        return e_failure;
    }

    // Decode magic string
    printf("INFO: Decoding Magic String Signature\n");
    if (decode_magic_string(decInfo) != e_success) 
    {
        fprintf(stderr, "ERROR: Magic string verification failed\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    // Decode file extension
    printf("INFO: Decoding Output File Extension\n");
    if (decode_secret_file_extn(decInfo) != e_success) 
    {
        fprintf(stderr, "ERROR: Failed to decode file extension\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    // Create output filename
    if(strlen(decInfo->output_fname) == 0) 
    {
        snprintf(decInfo->output_fname, MAX_OUTPUT_FILENAME, 
                 "decoded%s", decInfo->extn_secret_file);
        printf("INFO: Output File not mentioned. Creating %s as default\n", 
               decInfo->output_fname);
    }

    // Open output file
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");
    if (!decInfo->fptr_output) 
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot create %s\n", decInfo->output_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->output_fname);
    printf("INFO: Done. Opened all required files\n");

    // Decode file size
    printf("INFO: Decoding %s File Size\n", decInfo->output_fname);
    if (decode_secret_file_size(decInfo) != e_success) 
    {
        fprintf(stderr, "ERROR: Failed to decode file size\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    // Decode file content
    printf("INFO: Decoding %s File Data\n", decInfo->output_fname);
    if (decode_secret_file_data(decInfo) != e_success) 
    {
        fprintf(stderr, "ERROR: Failed to decode file content\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    
    
    // Cleanup
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);
    ;
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    const char *magic = MAGIC_STRING;
    for (int i = 0; magic[i]; i++)
    {
        uint8_t decoded_char = 0;
        for (int bit = 7; bit >= 0; bit--) 
        {
            uint8_t byte;
            if (fread(&byte, 1, 1, decInfo->fptr_stego_image) != 1) 
                return e_failure;
            decoded_char |= ((byte & 1) << bit);
        }
        if (decoded_char != magic[i]) 
        {
            fprintf(stderr, "ERROR: Expected '%c' got '%c'\n", magic[i], decoded_char);
            return e_failure;
        }
    }
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Decode extension length (1 byte)
    uint8_t extn_len = 0;
    for (int bit = 7; bit >= 0; bit--) 
    {
        uint8_t byte;
        if (fread(&byte, 1, 1, decInfo->fptr_stego_image) != 1) 
            return e_failure;
        extn_len |= ((byte & 1) << bit);
    }
    printf("DEBUG: Decoded extension length: %d\n", extn_len); // Add this

    // Validate length
    if (extn_len == 0 || extn_len >= sizeof(decInfo->extn_secret_file)) 
    {
        fprintf(stderr, "ERROR: Invalid extension length %d\n", extn_len);
        return e_failure;
    }

    // Decode extension
    for (int i = 0; i < extn_len; i++) 
    {
        uint8_t decoded_char = 0;
        for (int bit = 7; bit >= 0; bit--) 
        {
            uint8_t byte;
            if (fread(&byte, 1, 1, decInfo->fptr_stego_image) != 1) 
                return e_failure;
            decoded_char |= ((byte & 1) << bit);
        }
        decInfo->extn_secret_file[i] = decoded_char;
    }
    decInfo->extn_secret_file[extn_len] = '\0';
    
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    uint32_t size = 0;
    for (int i = 0; i < 4; i++) 
    {
        uint8_t decoded_byte = 0;
        for (int bit = 7; bit >= 0; bit--) 
        {
            uint8_t byte;
            if (fread(&byte, 1, 1, decInfo->fptr_stego_image) != 1) 
                return e_failure;
            decoded_byte |= ((byte & 1) << bit);
        }
        size = (size << 8) | decoded_byte;
    }
    decInfo->size_secret_file = size;
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    for (uint32_t i = 0; i < decInfo->size_secret_file; i++) 
    {
        uint8_t decoded_byte = 0;
        for (int bit = 7; bit >= 0; bit--) 
        {
            uint8_t byte;
            if (fread(&byte, 1, 1, decInfo->fptr_stego_image) != 1) 
                return e_failure;
            decoded_byte |= ((byte & 1) << bit);
        }
        if (fwrite(&decoded_byte, 1, 1, decInfo->fptr_output) != 1)
            return e_failure;
    }
    return e_success;
}
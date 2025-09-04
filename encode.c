#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <sys/stat.h>


Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    if ((encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb")) == NULL ||
        (encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb")) == NULL ||
        (encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb")) == NULL)
    {
        perror("Error opening files");
        return e_failure;
    }

    
    printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    if(encInfo->size_secret_file > 0) 
        printf("INFO: Done. Not Empty\n");
    else 
    {
        printf("INFO: Done. Empty\n");
        return e_failure;
    }

    if(check_capacity(encInfo) != e_success)
        return e_failure;

    printf("INFO: Copying Image Header\n");

    char header[54];
    fread(header, 1, 54, encInfo->fptr_src_image);
    fwrite(header, 1, 54, encInfo->fptr_stego_image);
    printf("INFO: Done\n");

    printf("INFO: Encoding Magic String Signature\n");
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
        return e_failure;
    printf("INFO: Done\n");

    printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
        return e_failure;
    printf("INFO: Done\n");

    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
        return e_failure;
    printf("INFO: Done\n");

    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
    if (encode_secret_file_data(encInfo) == e_failure)
        return e_failure;
    printf("INFO: Done\n");

    printf("INFO: Copying Left Over Data\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;
    printf("INFO: Done\n");

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);


    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    while (*magic_string)
    {
        char buffer[8];

        // Read 8 bytes from source image
        if (fread(buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            fprintf(stderr, "ERROR: Unable to read 8 bytes for magic string encoding\n");
            return e_failure;
        }

        // Encode the character into LSBs of buffer
        if (encode_byte_to_lsb_to_buffer(*magic_string, buffer) == e_failure)
        {
            fprintf(stderr, "ERROR: Failed to encode magic string character '%c'\n", *magic_string);
            return e_failure;
        }

        // Write 8 modified bytes to stego image
        if (fwrite(buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Failed to write encoded magic string to stego image\n");
            return e_failure;
        }

        magic_string++;
    }

    return e_success;
}

Status encode_byte_to_lsb_to_buffer(char data, char *buffer)
{
    for (int i = 7; i >= 0; i--)
    {
        buffer[7 - i] = (buffer[7 - i] & 0xFE) | ((data >> i) & 1);
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *src, FILE *dest)
{
    char buffer[8];
    fread(buffer, 1, size, src);

    for (int i = 0; i < size; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | (data[i] & 1);
    }

    fwrite(buffer, 1, size, dest);
    return e_success;
}

Status encode_size_to_lsb(int size, FILE *src, FILE *dest)
{
    for (int i = 3; i >= 0; i--)
    {
        char byte = (size >> (i * 8)) & 0xFF;

        if (encode_byte_to_lsb_file(byte, src, dest) == e_failure)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int extn_len = strlen(file_extn);
    printf("DEBUG: Writing extension length: %d\n", extn_len);
    
    // CHANGE THIS: Encode as 1 byte instead of 4 bytes
    if (encode_byte_to_lsb_file((char)extn_len, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;

    for (int i = 0; i < extn_len; i++)
    {
        if (encode_byte_to_lsb_file(file_extn[i], encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
            return e_failure;
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    return encode_size_to_lsb(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        char data;
        fread(&data, 1, 1, encInfo->fptr_secret);

        if (encode_byte_to_lsb_file(data, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
            return e_failure;
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    // Assign file names from command line
    encInfo->src_image_fname  = argv[2];
    encInfo->secret_fname     = argv[3];

    if (argc == 4)
    {
        // Output file name not provided
        printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
        encInfo->stego_image_fname = "steged_img.bmp";
    }
    else
    {
        encInfo->stego_image_fname = argv[4];
    }

    // Open all files and validate
    if (open_files(encInfo) == e_failure)
    {
        fprintf(stderr, "ERROR: Failed to open required files\n");
        return e_failure;
    }

    // Get secret file size
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);

    if (encInfo->size_secret_file <= 0)
    {
        fprintf(stderr, "ERROR: Secret file is empty\n");
        return e_failure;
    }

    // Extract file extension from secret file
    char *dot = strrchr(encInfo->secret_fname, '.');
    if (dot == NULL)
    {
        fprintf(stderr, "ERROR: Secret file has no extension\n");
        return e_failure;
    }

    strncpy(encInfo->extn_secret_file, dot, MAX_FILE_SUFFIX); // Include the dot
    encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';
    printf("DEBUG: Encoding extension: '%s' (length: %zu)\n", encInfo->extn_secret_file, strlen(encInfo->extn_secret_file));

    return e_success;
}


Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open source image file %s\n", encInfo->src_image_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->src_image_fname);

    // Open secret file in read-binary mode
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open secret file %s\n", encInfo->secret_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->secret_fname);

    // Open stego image file in write-binary mode
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to create stego image file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    printf("INFO: Done\n");
    return e_success;
}

Status encode_byte_to_lsb_file(char data, FILE *src, FILE *dest)
{
    char buffer[8];
    fread(buffer, 1, 8, src);
    encode_byte_to_lsb_to_buffer(data, buffer);
    fwrite(buffer, 1, 8, dest);
    return e_success;
}

// Add these functions:
uint get_image_size_for_bmp(FILE *fptr_image)
{
    struct stat st;
    fstat(fileno(fptr_image), &st);
    return st.st_size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    uint required_bytes = 54 +  // Header
                         (strlen(MAGIC_STRING) + 
                          strlen(encInfo->extn_secret_file) + 
                          1 + 4 + 
                         encInfo->size_secret_file) * 8;
    
    printf("INFO: Checking for %s capacity to handle secret file\n", 
           encInfo->src_image_fname);
    
    if(image_size >= required_bytes)
    {
        printf("INFO: Done. Found OK\n");
        return e_success;
    }
    printf("ERROR: Insufficient image capacity\n");
    return e_failure;
}
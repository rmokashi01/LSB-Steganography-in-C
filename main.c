#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if(argc < 2) 
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <bmp file> [output file]\n");
        return 1;
    }



    
    // Detect operation type
    if (strcmp(argv[1], "-e") == 0)
    {
        if(argc < 4) 
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
            return 1;
        }

        EncodeInfo encInfo = {0};
        encInfo.stego_image_fname = (argc > 4) ? argv[4] : "steged_img.bmp";
        if(argc == 4)
            printf("INFO: Output File not mentioned. Creating %s as default\n",
                   encInfo.stego_image_fname);

        if (read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
        {
            if (do_encoding(&encInfo) == e_success)
                printf("INFO: ## Encoding Done Successfully ##\n");
            else
                printf("ERROR: Encoding failed.\n");
        }
        else
        {
            printf("ERROR: Validation failed for encode arguments\n");
        }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        printf("INFO: Selected decoding\n");

        DecodeInfo decInfo;
        decInfo.stego_image_fname = argv[2];

        // Optional output file name
        if (argc >= 4)
            strcpy(decInfo.output_fname, argv[3]);
        else
            decInfo.output_fname[0] = '\0';  // default name used inside decode.c

        if (do_decoding(&decInfo) == e_success)
            printf("INFO: ## Decoding Done Successfully ##\n");
        else
            printf("ERROR: Decoding failed.\n");
    }
    else
    {
        printf("ERROR: Unsupported operation %s\n", argv[1]);
        return 1;
    }

    return 0;
}

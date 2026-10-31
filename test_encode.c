#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;

    // 1. Check operation type
    if (check_operation_type(argv) == e_encode)
    {
        printf("Selected Encoding....\n");

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Successfully read and validate the encode arguments\n");

            if (do_encoding(&encInfo) == e_success)
            {
                printf("completed the encoding\n");
            }
            else
            {
                printf("Problem occurred while encoding\n");
            }
        }
        else
        {
            printf("Failed to read and validate encode arguments\n");
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        printf("Selected Decoding...\n");

        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Successfully validated decode arguments\n");

            if (do_decoding(&decInfo) == e_success)
            {
            printf("Decoding completed successfully\n");
            }
            else
            {
             printf("Decoding failed\n");
            }
        }
        else
        {
            printf("Failed to validate decode arguments\n");
        }
    }
    else
    {
        printf("Invalid Option\n");
        printf("--------------------Usage--------------------\n");
        printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("Decoding: ./a.out -d stego.bmp output.txt\n");
        printf("---------------------------------------------\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    // Check whether option is -e or -d
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

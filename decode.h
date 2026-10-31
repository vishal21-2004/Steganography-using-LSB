
#ifndef DECODE_H
#define DECODE_H

#include "types.h"   // Contains Status and other user-defined types
#include "common.h"  // Contains common definitions, including MAGIC_STRING
#include <stdio.h>   // Provides FILE and file-handling functions

/*
 * Maximum number of image bytes required to decode one character.
 * One character contains 8 bits, so 8 image bytes are required.
 */
#define MAX_DECODE_BUF_SIZE 8

/*
 * Maximum space for the secret file extension.
 * ".txt" requires 5 bytes including the null terminator '\0'.
 */
#define MAX_FILE_SUFFIX1 5


/*
 * Structure to store information required for decoding
 * hidden data from a stego image.
 */
typedef struct _DecodeInfo
{
    /* Stego Image Info */

    char *stego_image_fname;
    // Stores the input stego image filename.

    FILE *fptr_stego_image;
    // File pointer used to read the stego image.


    /* Output File Info */

    char *output_fname;
    // Stores the output filename for the decoded secret data.

    FILE *fptr_output;
    // File pointer used to write the extracted secret data.


    /* Secret File Info */

    char extn_secret_file[MAX_FILE_SUFFIX1];
    // Stores the extracted secret file extension, such as ".txt".

    long size_secret_file;
    // Stores the original secret file size in bytes.


    /* Temporary Decoding Buffer */

    char image_data[MAX_DECODE_BUF_SIZE];
    // Temporarily stores 8 image bytes.
    // Their LSBs are combined to recover one character.

} DecodeInfo;


/* Decoding Function Prototypes */


/* Read and validate the decoding command-line arguments */
Status read_and_validate_decode_args(char *argv[],
                                     DecodeInfo *decInfo);


/* Open the stego image and output file */
Status open_files_decode(DecodeInfo *decInfo);


/* Decode and verify the hidden magic string */
Status decode_magic_string(DecodeInfo *decInfo);


/* Extract an integer from the LSBs of image bytes */
Status decode_size_from_lsb(char *image_buffer, int *size);


/* Decode the length of the secret file extension */
Status decode_secret_file_extn_size(DecodeInfo *decInfo,
                                    int *size);


/* Decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo, int size);


/* Decode the original secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);


/* Extract one character from 8 image bytes */
Status skip_bmp_header(DecodeInfo *decInfo);  


/* Decode the secret file contents and write them to the output file */
Status decode_secret_file_data(DecodeInfo *decInfo);


/* Perform the complete decoding process */
Status do_decoding(DecodeInfo *decInfo);


#endif

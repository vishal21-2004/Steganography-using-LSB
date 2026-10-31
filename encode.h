
#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"   // Contains user-defined data types

/*
 * Structure to store information required for encoding
 * a secret file into a source image.
 * Information about input, output, and intermediate
 * data is also stored in this structure.
 */

/* Maximum secret data stored temporarily at a time */
#define MAX_SECRET_BUF_SIZE 1

/* Eight image bytes are required to encode one character */
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

/* Maximum space allocated for the secret file suffix */
#define MAX_FILE_SUFFIX 4


typedef struct _EncodeInfo
{
    /* Source Image Info */

    char *src_image_fname;
    // Stores the source image filename passed through
    // the command-line arguments.

    FILE *fptr_src_image;
    // File pointer used to read the original source image.

    uint image_capacity;
    // Stores the image capacity available for encoding
    // the secret data.

    uint bits_per_pixel;
    // Stores the number of bits used for each pixel.

    char image_data[MAX_IMAGE_BUF_SIZE];
    // Temporary buffer used to store image bytes
    // while encoding secret data into their LSBs.


    /* Secret File Info */

    char *secret_fname;
    // Stores the secret file filename.

    FILE *fptr_secret;
    // File pointer used to read the secret file.

    char extn_secret_file[MAX_FILE_SUFFIX];
    // Stores the extension of the secret file,
    // for example, ".txt".

    char secret_data[MAX_SECRET_BUF_SIZE];
    // Temporarily stores the secret file data
    // before encoding it into the image.

    long size_secret_file;
    // Stores the size of the secret file in bytes.


    /* Stego Image Info */

    char *stego_image_fname;
    // Stores the output stego image filename.

    FILE *fptr_stego_image;
    // File pointer used to write the encoded image.

} EncodeInfo;


/* Encoding Function Prototypes */


/* Check whether the requested operation is encoding or decoding */
OperationType check_operation_type(char *argv[]);


/* Read and validate the encoding command-line arguments */
Status read_and_validate_encode_args(char *argv[],
                                     EncodeInfo *encInfo);


/* Perform the complete encoding process */
Status do_encoding(EncodeInfo *encInfo);


/* Open the source image, secret file, and output stego image */
Status open_files(EncodeInfo *encInfo);


/* Check whether the source image has enough capacity */
Status check_capacity(EncodeInfo *encInfo);


/* Get the image size or pixel-data capacity from a BMP image */
uint get_image_size_for_bmp(FILE *fptr_image);


/* Get the size of a file in bytes */
uint get_file_size(FILE *fptr);


/* Copy the BMP header from the source image to the stego image */
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_dest);


/* Encode the magic string into the image */
Status encode_magic_string(const char *magic_string,
                           EncodeInfo *encInfo);


/* Encode the secret file extension into the image */
Status encode_secret_file_extn(const char *file_extn,
                               EncodeInfo *encInfo);


/* Encode the secret file size into the image */
Status encode_secret_file_size(long file_size,
                               EncodeInfo *encInfo);


/* Encode the actual secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo);


/*
 * Encode each character of the supplied data into the source image.
 * The encoded image bytes are written to the stego image.
 */
Status encode_data_to_image(const char *data, int size,
                            FILE *fptr_src_image,
                            FILE *fptr_stego_image,
                            EncodeInfo *encInfo);


/* Encode one character into the LSBs of eight image bytes */
Status encode_byte_to_lsb(char data, char *image_buffer);


/* Encode an integer size into the LSBs of image bytes */
Status encode_size(int size, FILE *fptr_src,
                   FILE *fptr_stego);


/* Encode the integer size into the LSBs of an image buffer */
Status encode_size_to_lsb(char *image_buf, int size);


/* Copy the remaining source image bytes after encoding */
Status copy_remaining_img_data(FILE *fptr_src,
                               FILE *fptr_dest);

#endif

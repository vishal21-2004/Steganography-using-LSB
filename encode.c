#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{

    // Check if .bmp is passed or not
    if (argv[2] != NULL &&
        strcmp(strrchr(argv[2], '.'), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
        return e_failure;

    // Check .txt file is passed
    if (argv[3] != NULL &&
        strcmp(strrchr(argv[3], '.'), ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
        return e_failure;

    // Check for output file
    if (argv[4] != NULL)
        encInfo->stego_image_fname = argv[4];
    else
        encInfo->stego_image_fname = "stego.bmp";

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Create a mask to select the MSB
    unsigned char mask = 1 << 7;

    // Process all 8 bits of the character
    for (int i = 0; i < 8; i++)
    {
        // Clear the LSB and insert one data bit
        image_buffer[i] = (image_buffer[i] & 0xFE) |
                          ((data & mask) >> (7 - i));

        // Move mask to the next bit
        mask = mask >> 1;
    }

    return e_success;
}




Status encode_data_to_image(const char *data, int size,
                            FILE *fptr_src, FILE *fptr_stego,
                            EncodeInfo *encInfo)
{
    // Encode each character of the data one by one
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the source image
        fread(encInfo->image_data, 8, sizeof(char), fptr_src);

        // Hide 1 character (8 bits) in those 8 image bytes
        encode_byte_to_lsb(data[i], encInfo->image_data);

        // Write the modified 8 image bytes to the output image
        fwrite(encInfo->image_data, 8, sizeof(char), fptr_stego);
    }

    return e_success;
}
Status encode_size_to_lsb(char *image_buffer, int size)
{
    // Initialize mask to select the MSB (bit 31) of the size
    unsigned int mask = 1U << 31;

    // Process all 32 bits of the integer size
    for (int i = 0; i < 32; i++)
    {
        // Clear the LSB of the current image byte
        // Extract one bit from size and insert it into the image byte's LSB
        image_buffer[i] = (image_buffer[i] & 0xFE) |
                          ((size & mask) >> (31 - i));

        // Shift the mask right to select the next bit
        mask = mask >> 1;
    }

    // Return success after encoding all 32 bits
    return e_success;
}

Status encode_size(int size, FILE *fptr_src, FILE *fptr_stego)
{
    // Declare a buffer to store 32 image bytes
    char image_data[32];

    // Read 32 bytes from the source image
    fread(image_data, 32, sizeof(char), fptr_src);

    // Encode the given size into the LSBs of the image data
    encode_size_to_lsb(image_data, size);

    // Write the modified 32 bytes into the stego image
    fwrite(image_data, 32, sizeof(char), fptr_stego);

    // Return success after encoding the size
    return e_success;
}
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    // Encode every character of the magic string into the image
    // using the encode_data_to_image() function.
    encode_data_to_image(magic_str,strlen(MAGIC_STRING),
                         encInfo->fptr_src_image,
                         encInfo->fptr_stego_image,encInfo
                      );

    // Return success after calling the encoding function
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    char header[54];
    fseek(fptr_src, 0,SEEK_SET);
    fread(header, 54, sizeof(char), fptr_src);
    fwrite(header, 54, sizeof(char), fptr_stego);

    return e_success;
} 
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encInfo)
{
    // Assign the secret file extension as ".txt"
    file_ext = ".txt";

    // Encode the file extension into the source image's LSBs
    // strlen(file_ext) gives the number of characters in ".txt"
    // The function writes the encoded data into the stego image
    encode_data_to_image(file_ext, strlen(file_ext),
                         encInfo->fptr_src_image,
                         encInfo->fptr_stego_image,
                         encInfo);

    // Return success after encoding the extension
    return e_success;
}
Status encode_secret_file_size(long int file_size, EncodeInfo *encInfo)
{
    // Buffer to store 32 bytes from the source image
    char image_data[32];

    // Read 32 bytes from the source image
    fread(image_data, 32, sizeof(char), encInfo->fptr_src_image);

    // Encode the secret file size into the LSBs of the image bytes
    encode_size_to_lsb(image_data, file_size);

    // Write the modified 32 bytes to the stego image
    fwrite(image_data, 32, sizeof(char), encInfo->fptr_stego_image);

    // Return success after encoding the file size
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    // Bring the file pointer to the beginning of the secret file
    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    // Loop through each character of the secret file
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read one character from the secret file
        fread(&ch, 1, sizeof(char), encInfo->fptr_secret);

        // Read 8 bytes from the source image
        fread(encInfo->image_data, 8, sizeof(char),
              encInfo->fptr_src_image);

        // Encode the character into the LSBs of 8 image bytes
        encode_byte_to_lsb(ch, encInfo->image_data);

        // Write the modified 8 bytes into the stego image
        fwrite(encInfo->image_data, 8, sizeof(char),
               encInfo->fptr_stego_image);
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;  // Temporary variable to store one byte

    // Read one byte at a time from the source image
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        // Write the byte into the stego image
        fwrite(&ch, 1, 1, fptr_stego);
    }

    // Return success after copying all remaining bytes
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("Successfully opened all the files\n");

         if (check_capacity(encInfo) == e_success)
        {
        printf("Image has enough size to do the encoding\n");
              if (copy_bmp_header(encInfo->fptr_src_image,
                    encInfo->fptr_stego_image) == e_success)
              {
              printf("Successfully copied the header\n");

                    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
                    {
                        printf("Encoded magic string successfully\n");
                        if (encode_size(strlen(".txt"), encInfo->fptr_src_image,
                          encInfo->fptr_stego_image) == e_success)
                        {
                            printf("Successfully encoded the secret file extension size\n");
                                  if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                                  {
                                  printf("Successfully encoded the secret file extension\n");
                                         if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                                        {
                                        printf("Successfully encoded secret file size\n");
                                              if (encode_secret_file_data(encInfo) == e_success)
                                              {
                                              printf("Encoded secret data successfully\n");

                                                     if (copy_remaining_img_data(encInfo->fptr_src_image,
                                                         encInfo->fptr_stego_image) == e_success)
                                                     {
                                                     printf("Copied remaining RGB data successfully\n");
                                                     }
                                                     else
                                                     {
                                                      printf("Failed to copy remaining RGB data\n");
                                                      return e_failure;
                                                     }
                                              }
                                              else
                                              {
                                              printf("Failed to encode secret data\n");
                                              return e_failure;
                                              }
                                        }
                                        else
                                        {
                                          printf("Failed to encode secret file size\n");
                                          return e_failure;
                                        }
                                  }
                                  else
                                 {
                                 printf("Failed to encode the secret file extension\n");
                                 return e_failure;
                                 }
                        }
                        else
                        {
                        printf("Failed to encode the secret file extension size\n");
                         return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to encode the magic string\n");
                        return e_failure;
                    }
              }
              else
            {
              printf("Failure!!!! Not copied the header\n");
              return e_failure;
            }
        }
        else
        {
        printf("Image does not have enough size to do the encoding\n");
        return e_failure;
        }
    }
    else
    {
        printf("Failed to open all the files\n");
        return e_failure;
    }

    return e_success;
}



Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity =
        get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file =
        get_file_size(encInfo->fptr_secret);

    if (encInfo->image_capacity >
        (54 + (2 + 4 + 4 + 4 + encInfo->size_secret_file) * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0, SEEK_END);
    return ftell(fptr_secret);
}
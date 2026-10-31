#include "decode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

/* Read and validate decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] != NULL && strstr(argv[2], ".bmp") != NULL)
        decInfo->stego_image_fname = argv[2];
    else
    {
        printf("Failed: Invalid stego image file\n");
        return e_failure;
    }

    if (argv[3] != NULL)
        decInfo->output_fname = argv[3];
    else
    {
        printf("Failed: Output file not provided\n");
        return e_failure;
    }

    printf("Decode arguments are valid\n");
    return e_success;
}

/* Open files for decoding */
Status open_files_decode(DecodeInfo *decInfo)
{
    // Open the stego image in read mode
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    // Check whether the stego image opened successfully
    if (decInfo->fptr_stego_image == NULL)
    {
        // Display the error message if opening fails
        perror("fopen");
        return e_failure;
    }

    // Open the output file in write mode
    // Creates the file if it does not exist
    // Overwrites existing contents if the file already exists
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    // Check whether the output file opened successfully
    if (decInfo->fptr_output == NULL)
    {
        // Display the error message if opening fails
        perror("fopen");
        return e_failure;
    }

    // Both files have been opened successfully
    printf("Opened stego image and output file successfully\n");

    return e_success;
}

/* Skip BMP header (first 54 bytes) */
Status skip_bmp_header(DecodeInfo *decInfo)
{
    // Move the file pointer to byte 54
    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
    {
        printf("Failed to skip BMP header\n");
        return e_failure;
    }

    printf("Skipped 54 bytes BMP header in stego image\n");
    return e_success;
}

/* Decode one byte from 8 bytes of image data */
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1);
    }

    *data = ch;

    return e_success;
}

/* Decode the magic string from the stego image */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic[sizeof(MAGIC_STRING)] = {0};

    for (int i = 0; i < (int)strlen(MAGIC_STRING); i++)
    {
        if (fread(decInfo->image_data, 1, 8,
                  decInfo->fptr_stego_image) != 8)
        {
            printf("Failed to read image data\n");
            return e_failure;
        }

        decode_byte_from_lsb(decInfo->image_data, &magic[i]);

        // Debug statement
        printf("Decoded magic[%d] = %c, value = %u\n",
               i, magic[i], (unsigned char)magic[i]);
    }

    if (strcmp(magic, MAGIC_STRING) != 0)
    {
        printf("Expected magic string: %s\n", MAGIC_STRING);
        printf("Decoded magic string: %s\n", magic);
        fprintf(stderr, "ERROR: Magic string not found.\n");
        return e_failure;
    }

    printf("Successfully decoded the magic string\n");
    return e_success;
}

Status decode_size_from_lsb(char *image_buffer, int *size)
{
    int value = 0;

    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (image_buffer[i] & 1);
    }

    *size = value;

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *size)
{
    char image_buffer[32];

    // Read 32 bytes from stego.bmp
    if (fread(image_buffer, 1, 32,
              decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    // Extract the size from the LSBs
    if (decode_size_from_lsb(image_buffer, size) != e_success)
    {
        return e_failure;
    }

    // Debugging
    printf("Decoded extension size: %d\n", *size);

    return e_success;
}
/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo, int size)
{
    // Loop through each character of the secret file extension
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the stego image
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);

        // Decode one character using the LSBs of the 8 image bytes
        decode_byte_from_lsb(decInfo->image_data, &decInfo->extn_secret_file[i]);
    }

    // Add null character to terminate the extension string
    decInfo->extn_secret_file[size] = '\0';

    // Print the decoded secret file extension
    printf("Decoded secret file extension: %s\n", decInfo->extn_secret_file);

    // Return success after decoding the extension
    return e_success;
}
/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Read 32 bytes from the stego image
    fread(decInfo->image_data, 32, 1, decInfo->fptr_stego_image);

    // Declare a variable to store the decoded size
    int size;

    // Extract the file size from the LSBs of the image bytes
    decode_size_from_lsb(decInfo->image_data, &size);

    // Store the decoded size as a long integer
    decInfo->size_secret_file = (long)size;

    // Print the decoded secret file size
    printf("Decoded secret file size: %ld bytes\n",
           decInfo->size_secret_file);

    // Return success after decoding
    return e_success;
}
/* Decode secret file data exactly as in secret.txt */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Declare a variable to store one decoded character
    char ch;

    // Loop until all bytes of the secret file are decoded
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        // Read 8 bytes from the stego image
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);

        // Decode one character from the LSBs of the 8 image bytes
        decode_byte_from_lsb(decInfo->image_data, &ch);

        // Write the decoded character into the output file
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }

    // Display a success message after decoding all secret data
    printf("Decoded secret file data successfully\n");

    // Return success
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
      // Step 1: Open files

       if (open_files_decode(decInfo) == e_success)
        {
          printf("Files opened successfully\n");
        }
        else
        {
         printf("Failed to open files\n");
        return e_failure;
        }

     // Step 2: Skip BMP header
        if (skip_bmp_header(decInfo) == e_success)
        {
         printf("BMP header skipped successfully\n");
        }
        else
        {
        printf("Failed to skip BMP header\n");
        return e_failure;
        }

        // Step 3: Decode magic string
       if (decode_magic_string(decInfo) == e_success)
       {
        printf("Magic string decoded successfully\n");
       }
       else
       {
       printf("Magic string decoding failed\n");
       return e_failure;
       }

       // Decode secret file extension size

      int extn_size = 0;
      if (decode_secret_file_extn_size(decInfo, &extn_size) == e_success)
      {
      printf("Successfully decoded extension size: %d\n", extn_size);
      }
      else
      {
      printf("Failed to decode extension size\n");
      return e_failure;
      }

      // Decode secret file extension
      if (decode_secret_file_extn(decInfo, extn_size) == e_success)
      {
      printf("Successfully decoded secret file extension\n");
      }
      else
      {
      printf("Failed to decode secret file extension\n");
      return e_failure;
       }

       // Decode secret file size
      if (decode_secret_file_size(decInfo) == e_success)
     {
       printf("Successfully decoded secret file size\n");
     }
     else
     {
      printf("Failed to decode secret file size\n");
      return e_failure;
     }

     // Decode secret file data
     if (decode_secret_file_data(decInfo) == e_success)
     {
      printf("Successfully decoded secret file data\n");
     }
     else
     {
     printf("Failed to decode secret file data\n");
     return e_failure;
     }
     
     return e_success;
}


# Steganography Using LSB in BMP Images

##  Project Overview

This project implements **Steganography using the Least Significant Bit (LSB) technique** in BMP image files using the C programming language.

Steganography is a technique used to hide secret information inside another file, such as an image, without creating noticeable changes to the original file.

In this project, secret text data is encoded into the pixel data of a BMP image by modifying the least significant bits. The hidden message can later be extracted from the encoded image.

##  Objectives

- Understand the concept of steganography.
- Implement LSB-based data hiding in BMP images.
- Encode secret text into an image.
- Decode and retrieve hidden information.
- Understand file handling and bitwise operations in C.

## Technologies Used

- **Programming Language:** C
- **Image Format:** BMP (Bitmap)
- **Compiler:** GCC
- **Development Environment:** VS Code
- **Concepts:** File Handling, Pointers, Structures, Bitwise Operations, Dynamic Memory Allocation.

##  Features

- Encode secret text into a BMP image.
- Decode hidden text from an encoded BMP image.
- Preserve the original BMP header.
- Check image capacity before encoding.
- Support secret file extensions.
- Use a magic string to identify encoded images.
- Perform encoding and decoding through command-line arguments.

## Project Structure


STEGANOGRAPHY/
│
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
├── test_encode.c
├── beautiful.bmp
├── secret.txt
├── output.txt
├── stego.bmp
└── .gitignore


##  Working Principle

### 1. Encoding Process

1. Read the source BMP image.
2. Open the secret text file.
3. Check whether the image has sufficient capacity.
4. Copy the BMP header into the output image.
5. Encode the magic string.
6. Encode the secret file extension and size.
7. Encode the secret message using LSB.
8. Copy the remaining image data.
9. Generate the stego image.

### 2. Decoding Process

1. Open the encoded BMP image.
2. Validate the magic string.
3. Extract the secret file extension.
4. Extract the secret file size.
5. Decode the hidden message.
6. Store the extracted message in the output file.

##  LSB Technique

LSB stands for **Least Significant Bit**.

It is the rightmost bit of a binary number. By modifying this bit, we can hide information with minimal changes to the original pixel data.

Example:


Original Pixel :  11001010
Secret Bit     :         1
Modified Pixel :  11001011


Only the least significant bit changes, making the modification difficult to notice visually.

##  How to Compile

Compile the source files using GCC:


gcc *.c -o steganography

## How to Run

### Encoding


./steganography -e beautiful.bmp secret.txt stego.bmp


### Decoding


./steganography -d stego.bmp output.txt


**Note:** The command-line arguments should match the argument handling implemented in your `main()` function.

##  Concepts Learned

- File handling in C (`fopen`, `fread`, `fwrite`, `fclose`).
- Bitwise operators.
- Structures and pointers.
- Dynamic memory allocation.
- Command-line arguments (`argc` and `argv`).
- Modular programming using header and source files.
- Image data manipulation.


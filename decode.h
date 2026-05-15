#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>


/*
  Structure: DecodeInfo
  Purpose : Stores all information required during decoding
            from the stego (encoded) BMP image.
 
  Contents:
  1. stego_image_fname  → Name of encoded BMP file
  2. fptr_stego_image  → File pointer for reading stego image
 
  3. secret_fname   → Name of output secret file
  4. fptr_secret  → File pointer for writing secret data
 
  5. magic   → Buffer to store decoded magic string
  6. file_extn_size   → Size of the secret file extension (.txt)
  7. file_extn    → Secret file extension string
 
  8. secret_file_size    → Size of hidden secret file inside BMP
  9. data_buffer   → Temporary buffer to store 8 bytes (1 character)
 */
typedef struct _DecodeInfo
{
    /* Encoded image file */
    char *stego_image_fname;      // Name of stego BMP file
    FILE *fptr_stego_image;       // File pointer for reading encoded image

    /* Output secret text file */
    char *secret_fname;        // Constructed after decoding extension
    FILE *fptr_secret;            // File pointer to write decoded secret data

    /* Magic string buffer */
    char magic[32];               // Magic string extracted from image

    /* Secret file extension details */
    uint file_extn_size;          // Length of extension (.txt)
    char file_extn[10];           // File extension

    /* Secret file size */
    long secret_file_size;        // Number of bytes of secret data

    /* General buffer used for decoding each byte (8 bits) */
    char data_buffer[8];          // Holds 8 LSB bytes before decoding

} DecodeInfo;


/*
 * Function Prototypes for Decoding Steps
 * --------------------------------------
 * Each stage corresponds to a specific part of extraction.
 */


/*
 * Validate command-line arguments for decoding.
 * Expected usage:
 *      ./a.out -d stego.bmp
 */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/*
 * Main controlling function for decoding process.
 * It performs:
 *      - File opening
 *      - Skip header
 *      - Decode magic string
 *      - Decode extension size
 *      - Decode extension
 *      - Decode secret file size
 *      - Decode secret file data
 */
Status do_decoding(DecodeInfo *decInfo);

/*
 * Open stego BMP file for reading and prepare output file.
 */
Status open_decode_files(DecodeInfo *decInfo);

/*
 * Decode the magic string embedded in the BMP.
 * Used to verify valid encoded image.
 */
Status decode_magic_string(DecodeInfo *decInfo);

/*
 * Extract 1 byte (8 bits) from 8 LSBs of image bytes.
 *
 * image_buffer → contains 8 bytes from BMP pixel data
 * data         → resulting decoded character
 */
Status decode_byte_from_lsb(char *image_buffer, char *data);

/*
 * Decode 32 bits of data to extract:
 *      - extension size
 *      - file size
 */
Status decode_size_from_lsb(uint *size, DecodeInfo *decInfo);

/*
 * Decode actual extension (e.g., ".txt").
 */
Status decode_file_extn(DecodeInfo *decInfo);

/*
 * Decode size of secret file.
 */
Status decode_secret_file_size(DecodeInfo *decInfo);

/*
 * Decode all secret file bytes and write them into output file.
 */
Status decode_secret_file_data(DecodeInfo *decInfo);

/*
 * Skip 54-byte BMP header before reading pixel data.
 */
Status skip_bmp_header(FILE *fptr);

#endif   // DECODE_H

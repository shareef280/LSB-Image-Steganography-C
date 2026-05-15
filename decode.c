#include<stdio.h>
#include"decode.h"
#include "encode.h"
#include"types.h"
#include<string.h>
#include"common.h"

/*
 * Function: read_and_validate_decode_args
 * Purpose : Validate input arguments for decoding
 *           - Check if input file is BMP
 *           - Assign output filename
 */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo){

    // Check whether input file is BMP format
    if(strstr(argv[2],".bmp")!=NULL){
       // printf(GREEN "Input BMP file validated\n" RESET);

        // Store stego image filename
        decInfo->stego_image_fname=argv[2];
    }else{
        printf(RED "Invalid input file (BMP required)\n" RESET);
        return e_failure;
    }

    // Check if output file name is provided
    if( argv[3]!=NULL){
        //printf(GREEN "Output text file validated\n" RESET);

        // Extract filename without extension
        decInfo->secret_fname=strtok(argv[3],".");
    }else{
        // Default filename if not provided
        decInfo->secret_fname="decode";
    }

    return e_success;
}

/*
 * Function: do_decoding
 * Purpose : Main decoding controller
 * Steps   :
 *   1. Open files
 *   2. Validate magic string
 *   3. Decode extension size & extension
 *   4. Decode secret file size
 *   5. Decode actual data
 */
Status do_decoding(DecodeInfo *decInfo){

    printf(YELLOW "-----decoding started----->\n" RESET);

    // Open input stego image file
    if(open_decode_files(decInfo)==e_success){
        printf(GREEN "All files opened successfully\n" RESET);

    }else{
        printf(RED "Failed to open files\n" RESET);
        return e_failure;
    }

    // Validate magic string (authentication)
    if(decode_magic_string(decInfo)==e_success){
        printf(GREEN "Magic string matched\n" RESET);

    }else{
        printf(RED "Magic string mismatch\n" RESET);
        return e_failure;
    }

    // Decode extension size
    if( decode_size_from_lsb(&decInfo->file_extn_size,decInfo)==e_success){
        printf(GREEN "File extension size decoded successfully\n" RESET);
    }else{
        printf(RED "Failed to decode file extension size\n" RESET);
    }

    // Decode extension
    if(decode_file_extn(decInfo)==e_success){
        printf(GREEN "Decoding of secret file extension is successful\n" RESET);

    }else{
        printf(RED "Failed to decode secret file extension\n" RESET);
        return e_failure;
    }

    // Decode secret file size
    if(decode_secret_file_size(decInfo)==e_success){
        printf(GREEN "Secret file size decoded successfully\n" RESET);

    }else{
        printf(RED "Failed to decode secret file size\n" RESET);
        return e_failure;
    }

    // Decode actual secret data
    if(decode_secret_file_data(decInfo)==e_success){
        printf(GREEN "Secret file data decoded successfully\n" RESET);
    }else{
        printf(RED "Failed to decode secret file data\n" RESET);
        return e_failure;
    }

    // Close opened files
    fclose(decInfo->fptr_secret);
    fclose(decInfo->fptr_stego_image);

    return e_success;
}

/*
 * Function: open_decode_files
 * Purpose : Open stego image file in read mode
 */
Status open_decode_files(DecodeInfo *decInfo){

    // Open BMP file
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");

    // Check if file opened successfully
    if(decInfo->fptr_stego_image==NULL){
        printf(RED "Unable to open BMP file\n" RESET);
        return e_failure;
    }

    return e_success;
}
/*
 * Function: skip_bmp_header
 * Purpose : Skip first 54 bytes of BMP header
 *           (Pixel data starts after header)
 */
Status skip_bmp_header(FILE *fptr)
{
    fseek(fptr, 54, SEEK_SET);
    return e_success;
}

/*
 * Function: decode_magic_string
 * Purpose : Extract and verify magic string
 *           to ensure correct encoded file
 */
Status decode_magic_string(DecodeInfo *decInfo)
{
    // Skip BMP header
    skip_bmp_header(decInfo->fptr_stego_image);

    char decoded[32];
    int i;

    // Read characters one by one from image
    for(i = 0; i < 31; i++)
    {
        fread(decInfo->data_buffer, 8, 1, decInfo->fptr_stego_image);

        // Decode one character using LSB
        decode_byte_from_lsb(decInfo->data_buffer, &decoded[i]);

        // Stop if null character found
        if(decoded[i] == '\0')
            break;
    }

    decoded[i] = '\0';

    // Compare decoded string with original magic string
    if(strcmp(decoded, decInfo->magic) != 0)
    {
        return e_failure;
    }

    return e_success;
}

/*
 * Function: decode_byte_from_lsb
 * Purpose : Extract one byte from 8 bytes of image data
 *           using Least Significant Bit (LSB)
 */
Status decode_byte_from_lsb(char *image_buffer, char *data){

    char ch=0;

    // Extract each bit from LSB and rebuild character
    for (int i = 0; i < 8; i++)
    {
        ch |= ((image_buffer[i] & 1) << 7-i); // LSB extraction
    }

    *data = ch;

    return e_success;
}

/*
 * Function: decode_size_from_lsb
 * Purpose : Extract 32-bit integer (size) from image
 */
Status decode_size_from_lsb(uint *size, DecodeInfo *decInfo){

    char buffer[32];
    uint val=0;

    // Read 32 bytes
    if(fread(buffer,32,1,decInfo->fptr_stego_image)!=1){
        return e_failure;
    }

    // Convert LSB bits into integer
    for(int i=0;i<32;i++){
        val=val |((buffer[i] & 1)<<(31-i));
    }

    *size=val;

    return e_success;
}

/*
 * Function: decode_file_extn
 * Purpose : Decode hidden file extension
 *           and create output file
 */
Status decode_file_extn(DecodeInfo *decInfo){

    // Decode each character of extension
    for(int i=0;i<decInfo->file_extn_size;i++){
        fread(decInfo->data_buffer,8,1,decInfo->fptr_stego_image);

        decode_byte_from_lsb(decInfo->data_buffer, &decInfo->file_extn[i]);
    }

    // Append extension to filename
    decInfo->secret_fname= strcat(decInfo->secret_fname,decInfo->file_extn);

    // Open output file
    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");

    return e_success;
}

/*
 * Function: decode_secret_file_size
 * Purpose : Retrieve size of hidden file
 */
Status decode_secret_file_size(DecodeInfo *decInfo){

    uint size;

    decode_size_from_lsb(&size,decInfo);

    decInfo->secret_file_size=size;

    return e_success;
}

/*
 * Function: decode_secret_file_data
 * Purpose : Extract hidden data and write to output file
 */
Status decode_secret_file_data(DecodeInfo *decInfo){

    char ch;

    // Read each character and write to file
    for(int i=0;i<decInfo->secret_file_size;i++){
        fread(decInfo->data_buffer,8,1,decInfo->fptr_stego_image);

        decode_byte_from_lsb(decInfo->data_buffer, &ch);

        fwrite(&ch,1,1,decInfo->fptr_secret);
    }

    return e_success;
}
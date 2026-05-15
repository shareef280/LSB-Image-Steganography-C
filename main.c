/*
name : shareef sab
date : 4/5/2026
project no : 02
project name : LSB image stegnography
batch no : 26001B(084)
description : This project implements LSB Image Steganography to securely hide confidential data inside a BMP image without 
               visibly altering it. The least significant bits of image pixels are modified to embed secret information such as text files.
              It supports both encoding (hiding data) and decoding (retrieving data) operations. A magic string is used for authentication 
              during extraction. This ensures secure and undetectable communication of sensitive data.
*/

#include<stdio.h>
#include"encode.h"
#include"types.h"
#include<string.h>
#include"decode.h"
#include"common.h"

int main(int argc,char *argv[]){

    // Check if no arguments are passed
    if(argc==1){
        printf(RED "Pass correct argument\n" RESET);
        printf(YELLOW "./a.out -e beautiful.bmp secret.txt stego.bmp --> for encoding\n" RESET);
        printf(YELLOW "./a.out -d stego.bmp --> for decoding\n" RESET);
        return e_failure;
    }

    // Check if user selected encoding operation
    if(check_operation_type(argc,argv)==e_encode){

        // Inform user encoding is selected
        printf(GREEN "you choosen encodeing\n" RESET);

        // Validate minimum required arguments for encoding
        if(argc < 4){
            printf(RED "ERROR: Insufficient arguments for encoding\n" RESET);
            printf(YELLOW "Usage: ./a.out -e <input.bmp> <secret.txt> <output.bmp>\n" RESET);
            return e_failure;
        }

        // Structure to hold encoding information
        EncodeInfo encInfo;

        // Read and validate encoding arguments
        if(read_and_validate_encode_args(argc,argv,&encInfo)==e_success){

            printf(GREEN "reading  and validate successful\n" RESET);

            // Take magic string input from user
            printf(BLUE "enter magic string:" RESET);
            scanf(" %[^\n]",encInfo.magic);

            // Perform encoding process
            if(do_encoding(&encInfo)==e_success){
                printf(GREEN "encoding is successfully done\n" RESET);

            }else{
                printf(RED "Encoding is unsuccessful \n" RESET);
                return e_failure;
            }

        }else{
            // Argument validation failed
            printf(RED "Read and validate is Not successful\n" RESET);
            return e_failure;
        }

        // Close all opened files after encoding
        fclose(encInfo.fptr_src_image);
        fclose(encInfo.fptr_secret);
        fclose(encInfo.fptr_stego_image);

    }

    // Check if user selected decoding operation
    else if(check_operation_type(argc,argv)==e_decode){

        // Inform user decoding is selected
        printf(GREEN "you choosen decoding\n" RESET);

        // Validate minimum required arguments for decoding
        if(argc < 3){
            printf(RED "ERROR: Insufficient arguments for decoding\n" RESET);
            printf(YELLOW "Usage: ./a.out -e <input.bmp> ----> decoding\n" RESET);
            return e_failure;
        }

        // Structure to hold decoding information
        DecodeInfo decInfo;

        // Read and validate decoding arguments
        if(read_and_validate_decode_args(argc,argv,&decInfo)==e_success){

            printf(GREEN "reading and validating is successfull\n" RESET);

            // Take magic string input from user
            printf(BLUE "enter magic string:" RESET);
            scanf(" %[^\n]",decInfo.magic);

            // Perform decoding process
            if(do_decoding(&decInfo)==e_success){
                printf(GREEN "decoding successful\n" RESET);
            }else{
                printf(RED "decoding unsuccessful\n" RESET);
                return e_failure;
            }

        }else{
            // Argument validation failed
            printf(RED "reading and validating is unsuccessful\n" RESET);
            return e_failure;
        }

    }

    // If invalid option is provided
    else{
        printf(RED "Pass correct argument\n" RESET);
        printf(YELLOW "./a.out -e beautiful.bmp secret.txt stego.bmp --> for encoding\n" RESET);
        printf(YELLOW "./a.out -d stego.bmp --> for decoding\n" RESET);
        return e_failure;
    }

    return e_success;
}

/*
 * Function: check_operation_type
 * Purpose : Determine operation type from command-line argument
 *           - "-e" → Encoding
 *           - "-d" → Decoding
 *           - otherwise → Unsupported
 */
OperationType check_operation_type(int argc,char *argv[]){

    // Check for encoding option
    if((strcmp(argv[1],"-e"))==0){
        return e_encode;
    }

    // Check for decoding option
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }

    // Unsupported option
    else
        return e_unsupported;
}
#include<stdio.h>
#include"encode.h"
#include"types.h"
#include<string.h>
#include"common.h"

/*
 * Function: read_and_validate_encode_args
 * Purpose : Validate command-line arguments for encoding
 *           - Check input BMP file
 *           - Check secret text file
 *           - Set output stego image file
 */
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo){

    // Validate input BMP image
    if(argc>2 && (strstr(argv[2],".bmp")!=NULL)){
       // printf(GREEN "Input BMP file validated\n" RESET);
        encInfo->src_image_fname=argv[2]; 
    }else{
        printf(RED "Invalid BMP file\n" RESET);
        return e_failure;
    }

    // Validate secret file (.txt)
    if(argc>3 &&(strstr(argv[3],".txt")!=NULL)){
       // printf(GREEN "Secret file validated\n" RESET);
        encInfo->secret_fname=argv[3];
    }else{
        printf(RED "Invalid secret file\n" RESET);
        return e_failure;
    }

    // Validate output BMP file (optional)
    if(argc >4 &&(strstr(argv[4],".bmp"))){
       // printf(GREEN "Output BMP file set\n" RESET);
        encInfo->stego_image_fname=argv[4];
    }else{
        // Default output file
        encInfo->stego_image_fname="stego.bmp";
    }

    return e_success;   
}

/*
 * Function: do_encoding
 * Purpose : Main encoding controller
 * Steps   :
 *   1. Open files
 *   2. Check capacity
 *   3. Copy BMP header
 *   4. Encode magic string
 *   5. Encode file extension & size
 *   6. Encode file data
 *   7. Copy remaining image data
 */
Status do_encoding(EncodeInfo *encInfo){

    printf(YELLOW "-------encoding files started-------\n" RESET);

    // Open all required files
    if( open_files(encInfo)==e_success){
        printf(GREEN "All files opened successfully\n" RESET);
    }else{
        return e_failure;
    }

    // Check if image can hold secret data
    if(check_capacity(encInfo)==e_success){
        printf(GREEN "Capacity check successful\n" RESET);
    }else{
        printf(RED "Capacity check failed\n" RESET);
        return e_failure;
    }

    // Copy BMP header (first 54 bytes)
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        printf(GREEN "Copying BMP header completed\n" RESET);
    }else{
        printf(RED "Failed to copy BMP header\n" RESET);
    }

    // Encode magic string for authentication
    if( encode_magic_string(encInfo->magic,encInfo)==e_success){
        printf(GREEN "Magic string encoded successfully\n" RESET);
    }else{
        printf(RED "Magic string encoding failed\n" RESET);
        return e_failure;
    }

    // Encode file extension size
    if(encode_size_to_lsb(strlen(strchr(encInfo->secret_fname,'.')), encInfo)==e_success){
        printf(GREEN "File extension size encoded successfully\n" RESET);
    }else{
        printf(RED "File extension size encoding failed\n" RESET);
        return e_failure;
    }

    // Encode file extension (e.g., .txt)
    if(encode_secret_file_extn(strchr(encInfo->secret_fname,'.'),encInfo)==e_success){
        printf(GREEN "Secret file extension encoded successfully\n" RESET);
    }else{
        printf(RED "Secret file extension encoding failed\n" RESET);
        return e_failure;
    }

    // Encode secret file size
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success){
        printf(GREEN "Secret file size encoded successfully\n" RESET);
    }else{
        printf(RED "Secret file size encoding failed\n" RESET);
        return e_failure;
    }

    // Encode actual secret data
    if(encode_secret_file_data(encInfo)==e_success){
        printf(GREEN "Secret file data encoded successfully\n" RESET);
    }else{
        printf(RED "Secret file data encoding failed\n" RESET);
        return e_failure;
    }

    // Copy remaining image data after encoding
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        printf(GREEN "Remaining image data copied successfully\n" RESET);
    }else{
        printf(RED "Failed to copy remaining image data\n" RESET);
        return e_failure;
    }

    return e_success;
}
/*
 * Function: open_files
 * Purpose : Open source image, secret file, and output file
 */
Status open_files(EncodeInfo *encInfo){

    // Open source BMP image
    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"r");
    if(encInfo->fptr_src_image==NULL){
        printf(RED "Source image file not found\n" RESET);
        return e_failure;
    }

    // Open secret file
    encInfo->fptr_secret=fopen(encInfo->secret_fname,"r");
    if(encInfo->fptr_secret==NULL){
        printf(RED "Secret file not found\n" RESET);
        return e_failure;
    }

    // Open output stego image
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");

    return e_success;
}

/*
 * Function: check_capacity
 * Purpose : Ensure image can hold secret data
 */
Status check_capacity(EncodeInfo *encInfo){

    // Get image size
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get secret file size
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    // Compare capacity with required space
    if(encInfo->image_capacity >
       (strlen(encInfo->magic)*8 +32+32+32+encInfo->size_secret_file*8)){
        return e_success;
    }else{
        return e_failure;
    }
}

/*
 * Function: get_image_size_for_bmp
 * Purpose : Calculate total image data size (width × height × 3)
 */
uint get_image_size_for_bmp(FILE *fptr_image){

    int wid,len;

    // Read width and height from BMP header
    fseek(fptr_image,18,SEEK_SET);
    fread(&wid,4,1,fptr_image);
    fread(&len,4,1,fptr_image);

    return wid*len*3;
}

/*
 * Function: get_file_size
 * Purpose : Get size of file
 */
uint get_file_size(FILE *fptr){

    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/*
 * Function: copy_bmp_header
 * Purpose : Copy first 54 bytes (header)
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){

    rewind(fptr_src_image);

    char buffer[54];

    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);

    return e_success;
}

/*
 * Function: encode_magic_string
 * Purpose : Encode magic string into image
 */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo){

    if(encode_data_to_image(magic_string, strlen(magic_string)+1,encInfo)==e_success){
        return e_success;
    }

    return e_failure;
}

/*
 * Function: encode_data_to_image
 * Purpose : Encode data byte-by-byte into image using LSB
 */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo){

    for(int i=0;i<size;i++){

        // Read 8 bytes from image
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);

        // Encode one byte into 8 bytes
        encode_byte_to_lsb(data[i],encInfo->image_data);

        // Write modified bytes
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }

    return e_success;
}

/*
 * Function: encode_byte_to_lsb
 * Purpose : Encode one character into 8 bytes using LSB
 */
Status encode_byte_to_lsb(char data, char *image_buffer){

    for(int i=7;i>=0;i--){
        image_buffer[7-i]=(image_buffer[7-i]& 0xFE)|((data &(1<<i))>>i);
    }

    return e_success;
}

/*
 * Function: encode_secret_file_extn
 * Purpose : Encode file extension (e.g., .txt)
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo){

    if(encode_data_to_image(file_extn, strlen(file_extn),encInfo)==e_success){
        return e_success;
    }

    return e_failure;
}

/*
 * Function: encode_secret_file_size
 * Purpose : Encode size of secret file
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){

    encode_size_to_lsb(file_size, encInfo);
    return e_success;
}

/*
 * Function: encode_size_to_lsb
 * Purpose : Encode 32-bit size into image using LSB
 */
Status encode_size_to_lsb(long file_size, EncodeInfo *encInfo){

    char buffer[32];

    fread(buffer,32,1,encInfo->fptr_src_image);

    for(int i=31;i>=0;i--){
        buffer[31-i]=(buffer[31-i]&0xFE)|((file_size & (1<<i))>>i);
    }

    fwrite(buffer,32,1,encInfo->fptr_stego_image);

    return e_success;
}

/*
 * Function: encode_secret_file_data
 * Purpose : Encode actual file content
 */
Status encode_secret_file_data(EncodeInfo *encInfo){

    char buffer[encInfo->size_secret_file];

    rewind(encInfo->fptr_secret);

    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);

    encode_data_to_image(buffer, strlen(buffer),encInfo);

    return e_success;
}

/*
 * Function: copy_remaining_img_data
 * Purpose : Copy remaining data after encoding
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){

    char ch[1024];

    while(fread(&ch,1,1,fptr_src)==1){
        fwrite(&ch,1,1,fptr_dest);
    }

    return e_success;
}
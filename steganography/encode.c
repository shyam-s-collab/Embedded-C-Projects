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
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
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
    if(strstr(argv[2], ".bmp") != NULL)
    {
        encInfo -> src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
    if(strstr(argv[3], ".txt") != NULL)
    {
        encInfo -> secret_fname=argv[3];
    }
    else
    {
        return e_failure;
    }
    if(argv[4] != NULL)
    {
        encInfo -> stego_image_fname=argv[4];
    }
    else
    {
        encInfo -> stego_image_fname="stego.bmp";
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("Open files is success\n");
    }
    else
    {
        printf("Open files is failure\n");
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
        printf("Check capacity is a success\n");
    }
    else
    {
        printf("Check capacity is a failure\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)==e_success)
    {
        printf("copied bmp header successfully\n");
    }
    else
    {
        printf("failed to copy bmp header\n");
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
    {
        printf("Magic string was encoded successfully\n");
    }
    else
    {
        printf("Faied to encode magic string\n");
        return e_failure;
    }
    if(encode_secret_file_extn_size(4, encInfo)==e_success)
    {
        printf("Encoded secret file extension size successfully\n");
    }
    else
    {
        printf("Encoded secret file extension size is a failure\n");
        return e_failure;
    }
    if(encode_secret_file_extn(".txt", encInfo)==e_success)
    {
        printf("Encoded secret file extension successfully\n");
    }
    else
    {
        printf("Failed to encode secret file extension\n");
        return e_failure;
    }
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo)==e_success)
    {
        printf("Encoded secret file size successfully\n");
    }
    else
    {
        printf("Encoded secret file size is failure\n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Encoded secret file data successfully\n");
    }
    else
    {
        printf("Encoded secret file data is a failure\n");
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)==e_success)
    {
        printf("copied remaning data is successfully\n");
    }
    else
    {
        printf("copied remaining data is failure\n");
        return e_failure;
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch, 1, 1, fptr_src)>0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo -> size_secret_file];
    fseek(encInfo ->fptr_secret, 0, SEEK_SET);
    fread(str,encInfo -> size_secret_file, 1, encInfo->fptr_secret);

    encode_data_to_image(str, encInfo ->size_secret_file, encInfo);
    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);
    return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);
    return e_success;
}
Status encode_size_to_lsb(int data, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 32, 1, encInfo ->fptr_src_image);
    for(int i=0; i<32; i++)
    {
        str[i]=str[i] & 0xFE | ((data>>i)&1);
    }
    fwrite(str, 32, 1, encInfo -> fptr_stego_image);
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);
    return e_success;
}
Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
    {
    fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image);
    encode_byte_to_lsb(data[i],encInfo->image_data);
    fwrite(encInfo -> image_data, 8, 1, encInfo ->fptr_stego_image);
    }
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
        image_buffer[i]=(image_buffer[i] & 0xFE) |((data>>i)&1);
    }

}

Status check_capacity(EncodeInfo *encInfo)
{
    //find size of source file(beautiful.bmp)
    encInfo -> image_capacity= get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo -> image_capacity > (16+32+32+32+(encInfo -> size_secret_file*8)))
    {
        return e_success;
    }
    else
    {
       return e_failure; 
    }
         
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);
    char str[54];
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;

}
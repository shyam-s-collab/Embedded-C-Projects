#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Read and validate decode args */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if stego image file is provided and has .bmp extension
    if (argv[2] != NULL && strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // Check if output file name is provided
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output";
    }

    return e_success;
}

/* Open files for decoding */
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open stego image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

/* Skip BMP header (54 bytes) */
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

/* Decode size from LSB - matches encode_size_to_lsb */
Status decode_size_from_lsb(int *data, DecodeInfo *decInfo)
{
    char image_buffer[32];
    
    // Read 32 bytes from image
    if (fread(image_buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        return e_failure;
    }

    *data = 0;
    // Extract bits from LSB to MSB (matching encoding logic)
    for (int i = 0; i < 32; i++)
    {
        *data = *data | ((image_buffer[i] & 1) << i);
    }

    return e_success;
}

/* Decode byte from LSB - matches encode_byte_to_lsb */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    // Extract bits from LSB to MSB (matching encoding logic)
    for (int i = 0; i < 8; i++)
    {
        *data = *data | ((image_buffer[i] & 1) << i);
    }
    return e_success;
}

/* Decode data from image */
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
    char image_buffer[8];
    
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from stego image
        if (fread(image_buffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }

        // Decode byte from LSB
        decode_byte_from_lsb(&decInfo->secret_data[0], image_buffer);
        
        // Write to output file
        fwrite(&decInfo->secret_data[0], 1, 1, decInfo->fptr_output);
    }

    return e_success;
}

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];
    int magic_len = strlen(MAGIC_STRING);
    char image_buffer[8];
    
    // Decode magic string
    for (int i = 0; i < magic_len; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&magic_string[i], image_buffer);
    }
    magic_string[magic_len] = '\0';
    
    if (strcmp(magic_string, MAGIC_STRING) == 0)
    {
        printf("Magic string decoded successfully: %s\n", magic_string);
        return e_success;
    }
    else
    {
        printf("ERROR: Invalid magic string. This may not be an encoded image.\n");
        return e_failure;
    }
}

/* Decode secret file extension size - return the size */
int decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    int extn_size;
    decode_size_from_lsb(&extn_size, decInfo);
    printf("Extension size decoded: %d\n", extn_size);
    return extn_size;
}

/* Decode secret file extension - takes size as parameter */
Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo)
{
    char image_buffer[8];
    
    for (int i = 0; i < extn_size; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decInfo->extn_output_file[i], image_buffer);
    }
    
    decInfo->extn_output_file[extn_size] = '\0';
    printf("Extension decoded: %s\n", decInfo->extn_output_file);
    
    return e_success;
}
/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    int file_size;
    
    if (decode_size_from_lsb(&file_size, decInfo) == e_success)
    {
        decInfo->size_secret_file = file_size;
        printf("File size decoded: %ld bytes\n", decInfo->size_secret_file);
        return e_success;
    }
    
    return e_failure;
}

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Decoding secret file data...\n");
    
    if (decode_data_from_image(decInfo->size_secret_file, decInfo) == e_success)
    {
        printf("Secret file data decoded successfully\n");
        return e_success;
    }
    
    return e_failure;
}

/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    // Open files
    if (open_decode_files(decInfo) == e_failure)
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }
    printf("Files opened successfully\n");

    // Skip BMP header
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to skip BMP header\n");
        return e_failure;
    }
    printf("Skipped BMP header\n");

    // Decode magic string
    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("ERROR: Magic string verification failed\n");
        return e_failure;
    }

    // Decode file extension size
    int extn_size = decode_secret_file_extn_size(decInfo);
    if (extn_size <= 0)
    {
        printf("ERROR: Failed to decode file extension size\n");
        return e_failure;
    }

// Decode file extension
    if (decode_secret_file_extn(extn_size, decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode file extension\n");
        return e_failure;
    }
    // Decode file size
    if (decode_secret_file_size(decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode file size\n");
        return e_failure;
    }

    // Create output file with proper extension
    char output_filename[100];
    sprintf(output_filename, "%s%s", decInfo->output_fname, decInfo->extn_output_file);
    decInfo->fptr_output = fopen(output_filename, "w");
    
    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR: Failed to create output file\n");
        return e_failure;
    }
    printf("Created output file: %s\n", output_filename);

    // Decode secret data
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode secret data\n");
        return e_failure;
    }

    // Close files
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}
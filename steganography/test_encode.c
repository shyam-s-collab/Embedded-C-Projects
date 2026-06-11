#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *argv[]);
int main(int argc, char **argv)
{
    if(check_operation_type(argv) == e_encode)
    {
        printf("Selected encoding\n");
        EncodeInfo encode;
        if(read_and_validate_encode_args(argv, &encode) == e_success)
        {
            printf("Read and validate encode arguments is succes\n");
            printf("<------started encoding-------->");
            if(do_encoding(&encode)==e_success)
            {
                printf("Encoding is Successful\n");
            }
            else
            {
                printf("failed to perform encoding\n");
                return -2;
            }
        }
        else
        {
            printf("Read and validate encode arguments is a failure\n");
            return -1;
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        printf("Selected decoding\n");
        DecodeInfo decode;
        
        if (read_and_validate_decode_args(argv, &decode) == e_success)
        {
            printf("Read and validate decode arguments is success\n");
            printf("<------Started decoding-------->\n");
            
            if (do_decoding(&decode) == e_success)
            {
                printf("Decoding is Successful\n");
            }
            else
            {
                printf("Failed to perform decoding\n");
                return -2;
            }
        }
        else
        {
            printf("Read and validate decode arguments is a failure\n");
            return -1;
        }
    }
    else
    {
        printf("Invalid Input\n");
        printf("Usage:\n");
        printf("Encoding: ./a.out -e <src_image.bmp> <secret_file> [output.bmp]\n");
        printf("Decoding: ./a.out -d <stego_image.bmp> [output_filename]\n");
        return -1;
    }
    
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if (argv[1] == NULL)
    {
        return e_unsupported;
    }
    
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}


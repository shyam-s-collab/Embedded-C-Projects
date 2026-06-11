#include <stdio.h>
#include <string.h>
#include "error_handling.h"

/**
 * @brief Prints an application error message.
 *
 * @param message Error text to display.
 */
void display_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message ? message : "Unknown error");
}

/**
 * @brief Checks whether the file contains ID3v2 or ID3v1 metadata.
 *
 * @param filename MP3 filename.
 * @return 1 if an ID3 tag is present, otherwise 0.
 */
int check_id3_tag_presence(const char *filename) {
    FILE *fp;
    unsigned char header[10];
    unsigned char footer[128];

    fp = fopen(filename, "rb");
    if (!fp) {
        return 0;
    }

    if (fread(header, 1, sizeof(header), fp) == sizeof(header) && memcmp(header, "ID3", 3) == 0) {
        fclose(fp);
        return 1;
    }

    if (fseek(fp, -128L, SEEK_END) == 0 &&
        fread(footer, 1, sizeof(footer), fp) == sizeof(footer) &&
        memcmp(footer, "TAG", 3) == 0) {
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

/**
 * @file id3_writer.c
 * @brief Implementation of functions for writing and editing ID3 tags in MP3 files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_writer.h"
#include "id3_reader.h"
#include "id3_utils.h"

static void uint_to_syncsafe(unsigned int value, unsigned char out[4]) {
    out[0] = (value >> 21) & 0x7F;
    out[1] = (value >> 14) & 0x7F;
    out[2] = (value >> 7) & 0x7F;
    out[3] = value & 0x7F;
}

static void uint_to_be(unsigned int value, unsigned char out[4]) {
    out[0] = (value >> 24) & 0xFF;
    out[1] = (value >> 16) & 0xFF;
    out[2] = (value >> 8) & 0xFF;
    out[3] = value & 0xFF;
}

static unsigned int syncsafe_to_uint(const unsigned char bytes[4]) {
    return ((unsigned int)bytes[0] << 21) |
           ((unsigned int)bytes[1] << 14) |
           ((unsigned int)bytes[2] << 7) |
           (unsigned int)bytes[3];
}

static unsigned int text_frame_size(const char *value) {
    return value ? (unsigned int)(10 + 1 + strlen(value)) : 0;
}

static unsigned int comment_frame_size(const char *value) {
    return value ? (unsigned int)(10 + 1 + 3 + 1 + strlen(value)) : 0;
}

static void encode_frame_size(unsigned int value, int major_version, unsigned char out[4]) {
    if (major_version == 4) {
        uint_to_syncsafe(value, out);
    } else {
        uint_to_be(value, out);
    }
}

static int write_text_frame(FILE *fp, const char id[4], const char *value, int major_version) {
    unsigned char header[10] = {0};
    unsigned char size[4];
    unsigned int payload_size;

    if (!value || value[0] == '\0') {
        return 0;
    }

    payload_size = (unsigned int)(1 + strlen(value));
    memcpy(header, id, 4);
    encode_frame_size(payload_size, major_version, size);
    memcpy(header + 4, size, 4);

    if (fwrite(header, 1, sizeof(header), fp) != sizeof(header)) {
        return 1;
    }
    fputc(0, fp);
    if (fwrite(value, 1, strlen(value), fp) != strlen(value)) {
        return 1;
    }
    return 0;
}

static int write_comment_frame(FILE *fp, const char *value, int major_version) {
    unsigned char header[10] = {0};
    unsigned char size[4];
    unsigned int payload_size;

    if (!value || value[0] == '\0') {
        return 0;
    }

    payload_size = (unsigned int)(1 + 3 + 1 + strlen(value));
    memcpy(header, "COMM", 4);
    encode_frame_size(payload_size, major_version, size);
    memcpy(header + 4, size, 4);

    if (fwrite(header, 1, sizeof(header), fp) != sizeof(header)) {
        return 1;
    }
    fputc(0, fp);
    if (fwrite("eng", 1, 3, fp) != 3) {
        return 1;
    }
    fputc(0, fp);
    if (fwrite(value, 1, strlen(value), fp) != strlen(value)) {
        return 1;
    }
    return 0;
}

static long find_audio_offset(FILE *fp) {
    unsigned char header[10];

    rewind(fp);
    if (fread(header, 1, sizeof(header), fp) == sizeof(header) && memcmp(header, "ID3", 3) == 0) {
        return 10L + (long)syncsafe_to_uint(header + 6);
    }
    return 0L;
}

/**
 * @brief Writes the ID3 tags to an MP3 file.
 * 
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return 0 on success, non-zero on failure.
 */
int write_id3_tags(const char *filename, const TagData *data) {
    FILE *in;
    FILE *out;
    char temp_name[512];
    char backup_name[512];
    long audio_offset;
    unsigned int tag_size;
    int output_version;
    const char *year_frame_id;
    unsigned char header[10] = {'I', 'D', '3', 3, 0, 0, 0, 0, 0, 0};
    unsigned char size_bytes[4];
    unsigned char buffer[4096];
    size_t bytes_read;

    if (!filename || !data) {
        return 1;
    }

    output_version = (data->major_version == 4) ? 4 : 3;
    year_frame_id = (output_version == 4) ? "TDRC" : "TYER";
    header[3] = (unsigned char)output_version;

    in = fopen(filename, "rb");
    if (!in) {
        return 1;
    }

    audio_offset = find_audio_offset(in);
    if (audio_offset < 0 || fseek(in, audio_offset, SEEK_SET) != 0) {
        fclose(in);
        return 1;
    }

    snprintf(temp_name, sizeof(temp_name), "%s.tmp", filename);
    snprintf(backup_name, sizeof(backup_name), "%s.bak", filename);
    out = fopen(temp_name, "wb");
    if (!out) {
        fclose(in);
        return 1;
    }

    tag_size = text_frame_size(data->title) +
               text_frame_size(data->artist) +
               text_frame_size(data->album) +
               text_frame_size(data->year) +
               comment_frame_size(data->comment) +
               text_frame_size(data->genre);

    uint_to_syncsafe(tag_size, size_bytes);
    memcpy(header + 6, size_bytes, 4);
    if (fwrite(header, 1, sizeof(header), out) != sizeof(header) ||
        write_text_frame(out, "TIT2", data->title, output_version) ||
        write_text_frame(out, "TPE1", data->artist, output_version) ||
        write_text_frame(out, "TALB", data->album, output_version) ||
        write_text_frame(out, year_frame_id, data->year, output_version) ||
        write_comment_frame(out, data->comment, output_version) ||
        write_text_frame(out, "TCON", data->genre, output_version)) {
        fclose(in);
        fclose(out);
        remove(temp_name);
        return 1;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, bytes_read, out) != bytes_read) {
            fclose(in);
            fclose(out);
            remove(temp_name);
            return 1;
        }
    }

    fclose(in);
    if (fclose(out) != 0) {
        remove(temp_name);
        return 1;
    }

    if (rename(filename, backup_name) != 0) {
        remove(temp_name);
        return 1;
    }

    if (rename(temp_name, filename) != 0) {
        rename(backup_name, filename);
        remove(temp_name);
        return 1;
    }

    remove(backup_name);

    return 0;
}

int edit_tag(const char *filename, const char *tag, const char *value) {
    TagData *data = read_id3_tags(filename);
    int status = 0;

    if (!data) {
        return 1;
    }

    if (strcmp(tag, "-t") == 0) {
        status = set_tag_string(&data->title, value);
    } else if (strcmp(tag, "-T") == 0 || strcmp(tag, "-a") == 0) {
        status = set_tag_string(&data->artist, value);
    } else if (strcmp(tag, "-A") == 0) {
        status = set_tag_string(&data->album, value);
    } else if (strcmp(tag, "-y") == 0) {
        status = set_tag_string(&data->year, value);
    } else if (strcmp(tag, "-c") == 0) {
        status = set_tag_string(&data->comment, value);
    } else if (strcmp(tag, "-g") == 0) {
        status = set_tag_string(&data->genre, value);
    } else {
        status = 1;
    }

    if (status == 0) {
        status = write_id3_tags(filename, data);
    }

    free_tag_data(data);
    return status;
}

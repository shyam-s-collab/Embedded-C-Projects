/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_reader.h"
#include "error_handling.h"

static unsigned int syncsafe_to_uint(const unsigned char bytes[4]) {
    return ((unsigned int)bytes[0] << 21) |
           ((unsigned int)bytes[1] << 14) |
           ((unsigned int)bytes[2] << 7) |
           (unsigned int)bytes[3];
}

static unsigned int be_to_uint(const unsigned char *bytes, int count) {
    unsigned int value = 0;
    int i;

    for (i = 0; i < count; i++) {
        value = (value << 8) | bytes[i];
    }
    return value;
}

static char *copy_trimmed_text(const unsigned char *src, unsigned int len) {
    unsigned int start = 0;
    unsigned int end = len;
    char *out;
    unsigned int i;

    if (!src || len == 0) {
        return NULL;
    }

    if (src[0] <= 3) {
        start = 1;
    }

    while (start < end && (src[start] == '\0' || src[start] == ' ')) {
        start++;
    }
    while (end > start && (src[end - 1] == '\0' || src[end - 1] == ' ')) {
        end--;
    }

    out = (char *)malloc(end - start + 1);
    if (!out) {
        return NULL;
    }

    for (i = start; i < end; i++) {
        out[i - start] = (char)(src[i] ? src[i] : ' ');
    }
    out[end - start] = '\0';
    return out;
}

static void set_text_frame(TagData *data, const char *id, const unsigned char *content, unsigned int size) {
    char *value = copy_trimmed_text(content, size);

    if (!value) {
        return;
    }

    if (strcmp(id, "TIT2") == 0 || strcmp(id, "TT2") == 0) {
        set_tag_string(&data->title, value);
    } else if (strcmp(id, "TPE1") == 0 || strcmp(id, "TP1") == 0) {
        set_tag_string(&data->artist, value);
    } else if (strcmp(id, "TALB") == 0 || strcmp(id, "TAL") == 0) {
        set_tag_string(&data->album, value);
    } else if (strcmp(id, "TYER") == 0 || strcmp(id, "TDRC") == 0 || strcmp(id, "TYE") == 0) {
        set_tag_string(&data->year, value);
    } else if (strcmp(id, "TCON") == 0 || strcmp(id, "TCO") == 0) {
        set_tag_string(&data->genre, value);
    }

    free(value);
}

static void set_comment_frame(TagData *data, const unsigned char *content, unsigned int size) {
    unsigned int pos;
    char *value;

    if (!content || size == 0) {
        return;
    }

    if (size > 4) {
        pos = 4;
        while (pos < size && content[pos] != '\0') {
            pos++;
        }
        if (pos < size) {
            pos++;
        }
        value = copy_trimmed_text(content + pos, size - pos);
    } else {
        value = copy_trimmed_text(content, size);
    }

    if (value) {
        set_tag_string(&data->comment, value);
        free(value);
    }
}

static void set_picture_info(TagData *data, const unsigned char *content, unsigned int size) {
    unsigned int i = 1;
    static const char *types[] = {
        "Other", "32x32 icon", "Other icon", "Front cover", "Back cover",
        "Leaflet page", "Media", "Lead artist", "Artist", "Conductor",
        "Band", "Composer", "Lyricist", "Recording location", "During recording",
        "During performance", "Movie capture", "Bright fish", "Illustration",
        "Artist logo", "Publisher logo"
    };

    if (size < 4) {
        return;
    }

    if (content[1] == 'i' && content[2] == 'm' && content[3] == 'a') {
        while (i < size && content[i] != '\0') {
            i++;
        }
        if (i < size) {
            char mime[80];
            unsigned int mime_len = i - 1;
            if (mime_len >= sizeof(mime)) {
                mime_len = sizeof(mime) - 1;
            }
            memcpy(mime, content + 1, mime_len);
            mime[mime_len] = '\0';
            set_tag_string(&data->picture_mime, mime);
        }
        if (i + 1 < size) {
            unsigned int type = content[i + 1];
            if (type < sizeof(types) / sizeof(types[0])) {
                set_tag_string(&data->picture_type, types[type]);
            } else {
                set_tag_string(&data->picture_type, "Unknown");
            }
            data->picture_size = size - (i + 2);
        }
    }
}

static int read_id3v1(FILE *fp, TagData *data) {
    unsigned char tag[128];
    char *value;

    if (fseek(fp, -128L, SEEK_END) != 0 || fread(tag, 1, sizeof(tag), fp) != sizeof(tag)) {
        return 1;
    }
    if (memcmp(tag, "TAG", 3) != 0) {
        return 1;
    }

    set_tag_string(&data->version, "ID3v1");
    data->major_version = 1;
    value = copy_trimmed_text(tag + 3, 30);
    if (value) { set_tag_string(&data->title, value); free(value); }
    value = copy_trimmed_text(tag + 33, 30);
    if (value) { set_tag_string(&data->artist, value); free(value); }
    value = copy_trimmed_text(tag + 63, 30);
    if (value) { set_tag_string(&data->album, value); free(value); }
    value = copy_trimmed_text(tag + 93, 4);
    if (value) { set_tag_string(&data->year, value); free(value); }
    value = copy_trimmed_text(tag + 97, 30);
    if (value) { set_tag_string(&data->comment, value); free(value); }
    return 0;
}

TagData* read_id3_tags(const char *filename) {
    FILE *fp;
    unsigned char header[10];
    TagData *data = create_tag_data();
    unsigned int tag_size;
    unsigned int offset = 0;

    if (!data) {
        return NULL;
    }

    fp = fopen(filename, "rb");
    if (!fp) {
        free_tag_data(data);
        return NULL;
    }

    if (fread(header, 1, sizeof(header), fp) != sizeof(header) || memcmp(header, "ID3", 3) != 0) {
        if (read_id3v1(fp, data) == 0) {
            fclose(fp);
            return data;
        }
        fclose(fp);
        free_tag_data(data);
        return NULL;
    }

    data->major_version = header[3];
    data->minor_version = header[4];
    tag_size = syncsafe_to_uint(header + 6);

    {
        char version[32];
        snprintf(version, sizeof(version), "ID3v2.%d.%d", data->major_version, data->minor_version);
        set_tag_string(&data->version, version);
    }

    while (offset + 10 <= tag_size && data->major_version >= 3) {
        unsigned char frame_header[10];
        char id[5];
        unsigned int frame_size;
        unsigned char *content;

        if (fread(frame_header, 1, 10, fp) != 10) {
            break;
        }
        offset += 10;

        if (frame_header[0] == 0) {
            break;
        }

        memcpy(id, frame_header, 4);
        id[4] = '\0';
        frame_size = (data->major_version == 4) ? syncsafe_to_uint(frame_header + 4) : be_to_uint(frame_header + 4, 4);
        if (frame_size == 0 || offset + frame_size > tag_size) {
            break;
        }

        content = (unsigned char *)malloc(frame_size);
        if (!content || fread(content, 1, frame_size, fp) != frame_size) {
            free(content);
            break;
        }
        offset += frame_size;

        if (strcmp(id, "COMM") == 0) {
            set_comment_frame(data, content, frame_size);
        } else {
            set_text_frame(data, id, content, frame_size);
        }
        if (strcmp(id, "APIC") == 0) {
            set_picture_info(data, content, frame_size);
        }

        free(content);
    }

    while (offset + 6 <= tag_size && data->major_version == 2) {
        unsigned char frame_header[6];
        char id[4];
        unsigned int frame_size;
        unsigned char *content;

        if (fread(frame_header, 1, 6, fp) != 6) {
            break;
        }
        offset += 6;
        if (frame_header[0] == 0) {
            break;
        }
        memcpy(id, frame_header, 3);
        id[3] = '\0';
        frame_size = be_to_uint(frame_header + 3, 3);
        if (frame_size == 0 || offset + frame_size > tag_size) {
            break;
        }
        content = (unsigned char *)malloc(frame_size);
        if (!content || fread(content, 1, frame_size, fp) != frame_size) {
            free(content);
            break;
        }
        offset += frame_size;
        if (strcmp(id, "COM") == 0) {
            set_comment_frame(data, content, frame_size);
        } else {
            set_text_frame(data, id, content, frame_size);
        }
        free(content);
    }

    fclose(fp);
    return data;
}

void display_metadata(const TagData *data) {
    if (!data) {
        return;
    }

    printf("ID3 version  : %s\n", data->version ? data->version : "Unknown");
    if (data->major_version == 4) {
        printf("Note         : ID3v2.4 is detected; viewing and text editing are enabled.\n");
    }
    printf("Title        : %s\n", data->title ? data->title : "Not available");
    printf("Artist       : %s\n", data->artist ? data->artist : "Not available");
    printf("Album        : %s\n", data->album ? data->album : "Not available");
    printf("Year         : %s\n", data->year ? data->year : "Not available");
    printf("Comment      : %s\n", data->comment ? data->comment : "Not available");
    printf("Genre        : %s\n", data->genre ? data->genre : "Not available");
    if (data->picture_mime || data->picture_size > 0) {
        printf("Image type   : %s\n", data->picture_type ? data->picture_type : "Unknown");
        printf("Image MIME   : %s\n", data->picture_mime ? data->picture_mime : "Unknown");
        printf("Image size   : %u bytes\n", data->picture_size);
    }
}

void view_tags(const char *filename) {
    TagData *data = read_id3_tags(filename);
    if (!data) {
        display_error("Failed to read ID3 tags.");
        return;
    }
    printf("File         : %s\n", filename);
    display_metadata(data);
    free_tag_data(data);
}

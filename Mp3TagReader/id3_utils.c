#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_utils.h"

/**
 * @brief Creates an empty TagData object with all string fields initialized.
 *
 * @return Pointer to the new TagData object, or NULL on allocation failure.
 */
TagData* create_tag_data() {
    TagData *data = (TagData *)malloc(sizeof(TagData));
    if (data) {
        data->version = NULL;
        data->title = NULL;
        data->artist = NULL;
        data->album = NULL;
        data->year = NULL;
        data->comment = NULL;
        data->genre = NULL;
        data->picture_mime = NULL;
        data->picture_type = NULL;
        data->picture_size = 0;
        data->major_version = 0;
        data->minor_version = 0;
    }
    return data;
}

/**
 * @brief Frees a TagData object and all strings owned by it.
 *
 * @param data TagData object to free.
 */
void free_tag_data(TagData *data) {
    if (data) {
        free(data->version);
        free(data->title);
        free(data->artist);
        free(data->album);
        free(data->year);
        free(data->comment);
        free(data->genre);
        free(data->picture_mime);
        free(data->picture_type);
        free(data);
    }
}

int set_tag_string(char **field, const char *value) {
    char *copy = NULL;

    if (!field || !value) {
        return 1;
    }

    copy = (char *)malloc(strlen(value) + 1);
    if (!copy) {
        return 1;
    }

    strcpy(copy, value);
    free(*field);
    *field = copy;
    return 0;
}

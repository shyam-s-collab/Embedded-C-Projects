#ifndef ID3_UTILS_H
#define ID3_UTILS_H

/**
 * @brief Structure to hold ID3 tag data.
 */
typedef struct {
    char *version; /**< Version of the ID3 tag */
    char *title;   /**< Title of the song */
    char *artist;  /**< Artist of the song */
    char *album;   /**< Album name */
    char *year;    /**< Year of release */
    char *comment; /**< Comment */
    char *genre;   /**< Genre */
    char *picture_mime; /**< MIME type of embedded picture, if present */
    char *picture_type; /**< Human-readable embedded picture type */
    unsigned int picture_size; /**< Embedded picture payload size */
    int major_version; /**< ID3v2 major version, or 1 for ID3v1 */
    int minor_version; /**< ID3v2 minor version */
} TagData;

/**
 * @brief Creates a new TagData structure.
 * 
 * @return Pointer to the newly created TagData structure.
 */
TagData* create_tag_data();

/**
TODO: Add documention as sample given above
 */
void free_tag_data(TagData *data);

/**
 * @brief Replaces a heap-allocated string field with a copy of value.
 *
 * @param field Address of the string field to update.
 * @param value Null-terminated string to copy.
 * @return 0 on success, non-zero on allocation failure.
 */
int set_tag_string(char **field, const char *value);

#endif // ID3_UTILS_H

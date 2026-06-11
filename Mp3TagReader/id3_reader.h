#ifndef ID3_READER_H
#define ID3_READER_H

#include "id3_utils.h"

/**
 * @brief Reads ID3 tag information from an MP3 file.
 *
 * @param filename MP3 filename.
 * @return Newly allocated TagData object, or NULL on error/no tag.
 */
TagData* read_id3_tags(const char *filename);

/**
 * @brief Prints collected metadata in a readable format.
 *
 * @param data TagData object returned by read_id3_tags().
 */
void display_metadata(const TagData *data);

/**
 * @brief Opens filename, reads tags, prints metadata, and frees resources.
 *
 * @param filename MP3 filename.
 */
void view_tags(const char *filename);

#endif // ID3_READER_H

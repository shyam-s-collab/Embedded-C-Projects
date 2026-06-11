#ifndef ID3_WRITER_H
#define ID3_WRITER_H

#include "id3_utils.h"

/**
 * @brief Writes the ID3 tags to an MP3 file.
 * 
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return 0 on success, non-zero on failure.
 */
int write_id3_tags(const char *filename, const TagData *data);

/**
 * @brief Updates one supported ID3 tag field.
 *
 * @param filename MP3 filename.
 * @param tag Command line tag option (-t, -a, -A, -y, -c, -g).
 * @param value New text value.
 * @return 0 on success, non-zero on failure.
 */
int edit_tag(const char *filename, const char *tag, const char *value);

#endif // ID3_WRITER_H

#ifndef MEDIATLVREADER_H_12000008042011
#define MEDIATLVREADER_H_12000008042011

#include <cstdint>

/**
 * Header length for a TLV entry.
 */
#define TLV_TAG_HEADER_LENGTH   5

/**
 * Retrieve the TLV tag value.
 */
#define TLV_TAG(x)              (*(uint8_t *)(x))

/**
 * Retrieve the TLV length value.
 */
#define TLV_LENGTH(x)           (*(uint32_t *)(((uint8_t *)(x)) + 1))

/**
 * Return a pointer to the TLV data.
 */
#define TLV_VALUE(x)            ((void *)(((uint8_t *)(x)) + TLV_TAG_HEADER_LENGTH))

/**
 * Return a pointer to the next TLV entry given the current TLV entry.
 */
#define TLV_NEXT_TAG(x)         ((void *)((uint8_t *)TLV_VALUE(x) + TLV_LENGTH(x)))

/**
 * Minimum length of a BMT file (contains at least a version tag with one byte).
 */
#define TLV_MIN_BMT_LENGTH      (TAG_HEADER_LENGTH + 1)

#endif

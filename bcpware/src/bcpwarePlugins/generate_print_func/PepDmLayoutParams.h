#ifndef PEPDMLAYOUTPARAMS_H_12000008042011
#define PEPDMLAYOUTPARAMS_H_12000008042011

#include "stdint.h"

namespace MjPrinterSoftware
{

/**
 *  The maximum number of contone planes. 
 */
#define PEP_MAX_CONTONE_PLANES  4

/**
 * The size in bytes of a single line of the dither matrix.
 * when double buffering is enabled.
 */
#define LINES_128_BYTES         0

/**
 * The size in bytes of a single line of the dither matrix.
 * when double buffering is enabled.
 */
#define LINES_256_BYTES         1

/**
 * Maximum size of the Dither Matrix in bytes - 256x256 byte matrix.
 */
#define PEP_MAX_DM_SIZE_BYTES   65536

/**
 * Maximum row width for a dither matrix.
 */
#define DM_MAX_ROW_WIDTH        256

/**
 * Number of bytes that a dither matrix row must be a multile of
 */
#define DM_ROW_MULT             32

/**
 * Defines the layout of the dither matrix.
 */
typedef struct
{
    uint8_t   lowerIndex[PEP_MAX_CONTONE_PLANES];  /**< For each plane the offset in
                                                    *   bytes to the beginning of the
                                                    *   corresponding dither matrix
                                                    *   data line. */
    uint8_t   upperIndex[PEP_MAX_CONTONE_PLANES];  /**< For each plane the offset in
                                                    *   bytes to the end of the
                                                    *   corresponding dither matrix
                                                    *   data line. After reading
                                                    *   data at this location the index
                                                    *   wraps to lowerIndex. */
    uint8_t   startIndex[PEP_MAX_CONTONE_PLANES];  /**< For each plane the offset in
                                                    *   bytes that specifies the first
                                                    *   byte of the corresponding dither
                                                    *   matrix data to use. */
    uint8_t   lineIncrement32;                   /**< The number of 256-bit words
                                                    *   between the start of one
                                                    *   line of the DM and the start
                                                    *   of the next line. */
    uint32_t  lineType;                          /**< size of a single line of the DM. */
} PepDmLayoutParams;

} // namespace MjPrinterSoftware

#endif /* PEPDMLAYOUTPARAMS_H_12000008042011 */


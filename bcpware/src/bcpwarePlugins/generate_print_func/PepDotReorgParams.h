#ifndef PEPDOTREORGPARAMS_H_12000008042011
#define PEPDOTREORGPARAMS_H_12000008042011

#include <cstdint>

namespace MjPrinterSoftware
{

/**
 * PEP HCU dot reorg (IOMapping) has 4 registers per color
 */
#define PEP_NUM_REORG_REG_PER_COLOR     4

/**
 * The maximum number of color channels supported.
 */
#define PEP_MAX_COLORS                  6

/**
 * Dot reorg units are responsible for mapping the contone dithered data,
 * bi-level data, tag data, and HCU's redundancy control channel to output
 * inks in the printhead.
 */
typedef struct
{
                                    /**  The reorg look-up table for all output inks.
                                     *   @note  Rather than defining this array in order of color,
                                     *          it is defined in order of 32-bit words within each
                                     *          color. This is done in order to support xPECs with
                                     *          varying ioMapping sizes. For example, SoPEC has a
                                     *          64-bit ioMapping cf OmniPEC's 128-bit ioMapping.
                                     *
                                     *   The format of the reorg look-up table is as follows.
                                     *
                                     *   ioMapping[0][] -
                                     *      This corresponds to bits 0 - 31 of the dot reorg
                                     *      mapping for all of the output colors. The first
                                     *      32-bit word corresponds to printhead color 0
                                     *      (i.e color plumbed to printhead rows 0 and 1).
                                     *      The last 32-bit word corresponds to printhead color 5.
                                     *      (i.e  color plumbed to printhead rows 10 and 11).
                                     *      For printheads with 10 rows, this last word must be
                                     *      set to zero.
                                     *
                                     *   ioMapping[1][] -
                                     *      This corresponds to bits 32 - 63 of the dot reorg
                                     *      mapping for all of the output colors.
                                     *
                                     *   ioMapping[2][] -
                                     *      This corresponds to bits 64 - 95 of the dot reorg
                                     *      mapping for all of the output colors. These settings
                                     *      are ignored for Sopec.
                                     *
                                     *   ioMapping[3][] -
                                     *      This corresponds to bits 96 - 127 of the dot reorg
                                     *      mapping for all of the output colors. These settings
                                     *      are ignored for Sopec.
                                     */
    uint32_t  ioMapping[PEP_NUM_REORG_REG_PER_COLOR][PEP_MAX_COLORS];

    uint32_t  redunMatrix;          /**< A 4 byte matrix value that contains a redundancy
                                         pattern that can be used by the dot reorg to
                                         alternate a single bi-level or contone plane between
                                         two ink planes.
                                            31:24  redundancy matrix byte 3
                                            23:16  redundancy matrix byte 2
                                            15: 8  redundancy matrix byte 1
                                             7: 0  redundancy matrix byte 0   */
    uint32_t  redunMatrixWidth;     /**< The number of bits in a redunMatrix byte minus 1
                                         that are used by the dot reorg before wrapping. */
    uint32_t  redunMatrixLength;    /**< The number of redunMatrix bytes minus 1 used by
                                         the dot reorg before wrapping.  */
} PepDotReorgParams;

} // namespace MjPrinterSoftware

#endif /* PEPDOTREORGPARAMS_H_12000008042011 */

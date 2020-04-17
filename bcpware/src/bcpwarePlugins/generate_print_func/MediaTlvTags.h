#ifndef MEDIATLVTAGS_H_12000008042011
#define MEDIATLVTAGS_H_12000008042011

/**
 * uint8_t major version.
 */
#define	BMT_MAJOR_VERSION           0

/**
 * uint8_t minor version.
 */
#define	BMT_MINOR_VERSION           1

/**
 * A TLV data block containing all the information for a profile:
 *  BMT_COLOUR_MODE
 *  BMT_RESOLUTION
 *  BMT_FILE_NAME
 *  BMT_DISPLAY_NAME
 *  BMT_COLOR_LUT?
 *  BMT_DITHER_MATRIX+
 *  BMT_PEP_DM_LAYOUT_PARAMS
 *  BMT_PEP_DOT_REORG_PARAMS
 */
#define	BMT_PROFILE                 10

/**
 * uint8_t specifying the colour match mode:
 *  0: match any
 *  1: colour
 *  2: bw
 */
#define	BMT_COLOUR_MODE             11

/**
 * A pair of uint32_t values specifying the horizontal, followed by vertical
 * resolution in dpi. Or 0 to match any dpi.
 */
#define	BMT_RESOLUTION              12

/**
 * Zero terminated utf-8 filename.
 */
#define	BMT_FILE_NAME               13

/**
 * Zero terminated utf-8 name used for display.
 */
#define	BMT_DISPLAY_NAME            14

/**
 * A TLV data block containing all the information
 * for a colour LUT:
 *  BMT_ID
 *  BMT_BIT_DEPTH
 *  BMT_FORMAT
 *  BMT_DATA
 */
#define	BMT_COLOR_LUT               15

/**
 * A TLV data block containing all the information
 * for a dither matrix.
 *  BMT_BIT_DEPTH
 *  BMT_FORMAT
 *  BMT_DATA
 */
#define	BMT_DITHER_MATRIX           16

/**
 * A TLV data block containing all the information
 * for a PepDmLayoutParams.
 *  BMT_DM_LOWER_INDEX
 *  BMT_DM_UPPER_INDEX
 *  BMT_DM_START_INDEX
 *  BMT_DM_LINE_INCREMENT
 *  BMT_DM_LINE_TYPE
 */
#define BMT_PEP_DM_LAYOUT_PARAMS    17

/**
 * A TLV data block containing all the information
 * for a PepDotReorgParams.
 *  BMT_DR_IO_MAPPING
 *  BMT_DR_REDUN_MATRIX
 *  BMT_DR_REDUN_MATRIX_WIDTH
 *  BMT_DR_REDUN_MATRIX_LENGTH
 */
#define	BMT_PEP_DOT_REORG_PARAMS    18

/**
 * A utf-8 sha-1 id string.
 */
#define BMT_ID                      19

/**
 * A uint8_t bit depth.
 */
#define	BMT_BIT_DEPTH               20

/**
 * A utf-8 format string.
 */
#define	BMT_FORMAT                  21

/**
 * A byte array of data.
 */
#define	BMT_DATA                    22

#define BMT_DM_LOWER_INDEX          23
#define BMT_DM_UPPER_INDEX          24
#define BMT_DM_START_INDEX          25
#define BMT_DM_LINE_INCREMENT       26
#define BMT_DM_LINE_TYPE            27

#define BMT_DR_IO_MAPPING           28
#define BMT_DR_REDUN_MATRIX         29
#define BMT_DR_REDUN_MATRIX_WIDTH   30
#define BMT_DR_REDUN_MATRIX_LENGTH  31


/**
 * The major and minor version.
 */
#define MEDIA_FILE_MAJOR_VERSION    2
#define MEDIA_FILE_MINOR_VERSION    0

/**
 * Color mode specifiers.
 */
#define MEDIA_COLOR_MODE_ANY        0
#define MEDIA_COLOR_MODE_COLOR      1
#define MEDIA_COLOR_MODE_BW         2

/**
 * Id to match any resolution.
 */
#define MEDIA_RESOLUTION_ANY        0

/**
 * Id to match any media Id.
 */
#define MEDIA_ID_ANY                0

#endif

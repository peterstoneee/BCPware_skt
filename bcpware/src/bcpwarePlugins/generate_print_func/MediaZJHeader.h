#ifndef MEDIAZJHEADER_H_12000008042011
#define MEDIAZJHEADER_H_12000008042011

#include "stdint.h"

namespace MjPrinterSoftware
{

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Indicates the current version of the structure below. If there
 * are any incompatible changes to this version or the media profile
 * data, then this version must be incremented. The driver and firmware
 * must both be updated.
 *
 * "BMT\x01" First revision of BMT profiles
 * "BMT\x02" Second revision of BMT profiles
 */
#define ZJI_MEDIA_HEADER_ID        "BMT\x02"
#define ZJI_MEDIA_HEADER_ID_LENGTH 4

/**
 * Header for the data block in the ZJI_MEDIA_PROFILE tag. All values are little endian.
 */
struct MediaZJHeader
{
    /**
     * This is the header identifer for the block.
     */
    uint8_t     id[ZJI_MEDIA_HEADER_ID_LENGTH];

    /**
     * 0 to match all media, otherwise an Id for a specific media.
     */
    uint32_t    mediaId;

    /**
     * 0 for temporary (current job only), 1 for persistant (applies to subsequent jobs).
     */
    uint32_t    isPersistent;
};

#ifdef __cplusplus
}
#endif

} // namespace MjPrinterSoftware

#endif /* MEDIAZJHEADER_H_12000008042011 */

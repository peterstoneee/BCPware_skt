#ifndef MEDIATLVWRITER_H_12000008042011
#define MEDIATLVWRITER_H_12000008042011

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "MediaProfile.h"
#include "PepDmLayoutParams.h"
#include "PepDotReorgParams.h"

namespace MjPrinterSoftware
{

/**
 * Typedefs for storing dither arrays.
 */
typedef std::map<const MediaDitherArray *, int> MediaDitherMap;
typedef std::pair<const MediaDitherArray *, int> MediaDitherPair;

/**
 * An entry in the TLV list.
 */
struct MediaTlvWriterEntry
{
    uint8_t tag;
    uint32_t length;
    void    *value;
};

/**
 * Structure used to populate a profile entry for writing into a TLV (BMT) file.
 */
struct MediaTlvProfile
{
    uint8_t         colourMode;
    uint32_t        resolutionHoriz;
    uint32_t        resolutionVert;
    const char      *fileName;
    const char      *displayName;

    const void      *ditherMatrixData;
    uint32_t        ditherMatrixLength;
    const char      *ditherMatrixFormat;
    uint8_t         ditherMatrixBitDepth;

    const void      *colorLutData;
    uint32_t        colorLutLength;
    const char      *colorLutFormat;
    uint8_t         colorLutBitDepth;
    const char      *colorLutId;

    PepDmLayoutParams *dmLayoutParams;
    PepDotReorgParams *dotReorgParams;
};

/**
 * Class for a writing a TLV file (using BMT v2 tags).
 */
class MediaTlvWriter
{
public:
    MediaTlvWriter(void);
    virtual ~MediaTlvWriter(void);

    bool add8bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy = true);
    bool add16bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy = true);
    bool add32bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy = true);
    bool addStringEntry(uint8_t tag, const char *string, bool copy = true);
    bool addTlvEntry(uint8_t tag, MediaTlvWriter &tlvWriter);
    bool addHeader();
    bool addProfile(MediaTlvProfile *profile);
    bool getTlvMemoryBlock(void **buffer, size_t *length);
    bool writeTlvMemoryBlock(void *buffer, size_t *length);
    void clear();

protected:

    bool wideCharToUtf8(std::wstring utf16, std::string &utf8);
    bool addEntry(uint8_t tag, uint8_t wordSize, uint32_t length, const void *value, bool copy = true);

    std::vector<MediaTlvWriterEntry> mEntries;
};

} // namespace MjPrinterSoftware

#endif

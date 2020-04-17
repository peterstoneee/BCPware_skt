//#include "stdafx.h"
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif
#include "MediaTlvWriter.h"
#include "MediaTlvReader.h"
#include "MediaTlvTags.h"
#include <windows.h>
#include <map>

namespace MjPrinterSoftware
{

#define UINT16_BE(x) \
  (((((uint16_t)(x)) & 0x00ffU) <<  8) | \
   ((((uint16_t)(x)) & 0xff00U) >>  8))  \

#define UINT32_BE(x) \
  (((((uint32_t)(x)) & 0x000000ffU) << 24) | \
   ((((uint32_t)(x)) & 0x0000ff00U) <<  8) | \
   ((((uint32_t)(x)) & 0x00ff0000U) >>  8) | \
   ((((uint32_t)(x)) & 0xff000000U) >> 24))

MediaTlvWriter::MediaTlvWriter(void)
{
}

MediaTlvWriter::~MediaTlvWriter(void)
{
    clear();
}

bool MediaTlvWriter::wideCharToUtf8(std::wstring utf16, std::string &utf8)
{
    BOOL usedDefaultChar = false;
    wchar_t *utf16Buffer = &utf16[0];
    int32_t utf16len = utf16.length();
    int utfBufLen = WideCharToMultiByte(CP_UTF8, 0, utf16Buffer, utf16len, NULL, 0, NULL, NULL);
    if (utfBufLen <= 0)
        return false;

    try
    {
        utf8.resize(utfBufLen + 1);
    }
    catch (...)
    {
        return false;
    }
    char *utf8Buffer = &utf8[0];
    return WideCharToMultiByte(CP_UTF8, 0, utf16Buffer, utf16len, utf8Buffer, utfBufLen, NULL, NULL) > 0;
}

bool MediaTlvWriter::addStringEntry(uint8_t tag, const char *string, bool copy)
{
    if (string == NULL)
        return true;

    return MediaTlvWriter::add8bitEntry(tag, strlen(string) + 1, string, copy);
}

bool MediaTlvWriter::addTlvEntry(uint8_t tag, MediaTlvWriter &tlvWriter)
{
    void    *block = NULL;
    size_t  blockLength;

    if (!tlvWriter.getTlvMemoryBlock(&block, &blockLength))
        return false;
    if (blockLength == 0)
        return true;
    if (!add8bitEntry(tag, blockLength, block, false))
    {
        if (block != NULL)
            free(block);
        return false;
    }
    return true;
}

bool MediaTlvWriter::addEntry(uint8_t tag, uint8_t wordSize, uint32_t length, const void *value, bool copy)
{
    MediaTlvWriterEntry  entry;
    entry.tag = tag;
    entry.length = length;

    // Check params
    if (length % wordSize != 0)
        return false;

    if (copy)
    {
        if (length > 0)
        {
            entry.value = (uint8_t *)malloc(length);
            if (entry.value == NULL)
                return false;
            memcpy(entry.value, value, length);

            // Correct endianess
            if (wordSize == 2)
            {
                uint16_t *data = (uint16_t *)entry.value;
                for (uint32_t i = 0; i < length / 2; ++i)
                    data[i] = UINT16_BE(data[i]);
            }
            else if (wordSize == 4)
            {
                uint32_t *data = (uint32_t *)entry.value;
                for (uint32_t i = 0; i < length / 4; ++i)
                    data[i] = UINT32_BE(data[i]);
            }
        }
        else
        {
            entry.value = NULL;
        }
    }
    else
    {
        entry.value = (void *)value;
    }
    mEntries.push_back(entry);
    return true;
}

bool MediaTlvWriter::add8bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy)
{
    return addEntry(tag, 1, length, value, copy);
}

bool MediaTlvWriter::add16bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy)
{
    return addEntry(tag, 2, length, value, copy);
}

bool MediaTlvWriter::add32bitEntry(uint8_t tag, uint32_t length, const void *value, bool copy)
{
    return addEntry(tag, 4, length, value, copy);
}

bool MediaTlvWriter::addHeader()
{
    uint8_t major_version = MEDIA_FILE_MAJOR_VERSION;
    uint8_t minor_version = MEDIA_FILE_MINOR_VERSION;

    if (!add8bitEntry(BMT_MAJOR_VERSION, sizeof(major_version), &major_version))
        return false;
    if (!add8bitEntry(BMT_MINOR_VERSION, sizeof(minor_version), &minor_version))
        return false;
    return true;
}

bool MediaTlvWriter::addProfile(MediaTlvProfile *profile)
{
    if (profile == NULL)
        return false;

    MediaTlvWriter  tlvProfile;

    if (!tlvProfile.add8bitEntry(BMT_COLOUR_MODE, sizeof(profile->colourMode), &profile->colourMode))
        return false;
    uint32_t resolution[2] = {profile->resolutionHoriz, profile->resolutionVert};
    if (!tlvProfile.add32bitEntry(BMT_RESOLUTION, sizeof(resolution), resolution))
        return false;
    if (!tlvProfile.addStringEntry(BMT_FILE_NAME,profile->fileName))
        return false;
    if (!tlvProfile.addStringEntry(BMT_DISPLAY_NAME,profile->displayName))
        return false;

    if (profile->colorLutData != NULL)
    {
        MediaTlvWriter  tlvColourLut;

        if (!tlvColourLut.addStringEntry(BMT_ID,profile->colorLutId))
            return false;
        if (!tlvColourLut.add8bitEntry(BMT_BIT_DEPTH, sizeof(profile->colorLutBitDepth), &profile->colorLutBitDepth))
            return false;
        if (!tlvColourLut.addStringEntry(BMT_FORMAT, profile->colorLutFormat))
            return false;
        if (!tlvColourLut.add8bitEntry(BMT_DATA, profile->colorLutLength, profile->colorLutData))
            return false;
        if (!tlvProfile.addTlvEntry(BMT_COLOR_LUT, tlvColourLut))
            return false;
    }

    if (profile->ditherMatrixData != NULL)
    {
        MediaTlvWriter  tlvDitherMatrix;

        if (!tlvDitherMatrix.add8bitEntry(BMT_BIT_DEPTH, sizeof(profile->ditherMatrixBitDepth), &profile->ditherMatrixBitDepth))
            return false;
        if (!tlvDitherMatrix.addStringEntry(BMT_FORMAT, profile->ditherMatrixFormat))
            return false;
        if (!tlvDitherMatrix.add8bitEntry(BMT_DATA, profile->ditherMatrixLength, profile->ditherMatrixData))
            return false;
        if (!tlvProfile.addTlvEntry(BMT_DITHER_MATRIX, tlvDitherMatrix))
            return false;
    }

    if (profile->dmLayoutParams != NULL)
    {
        MediaTlvWriter  tlvDmLayoutParams;

        if (!tlvDmLayoutParams.add8bitEntry(BMT_DM_LOWER_INDEX, sizeof(profile->dmLayoutParams->lowerIndex), profile->dmLayoutParams->lowerIndex))
            return false;
        if (!tlvDmLayoutParams.add8bitEntry(BMT_DM_UPPER_INDEX, sizeof(profile->dmLayoutParams->upperIndex), profile->dmLayoutParams->upperIndex))
            return false;
        if (!tlvDmLayoutParams.add8bitEntry(BMT_DM_START_INDEX, sizeof(profile->dmLayoutParams->startIndex), profile->dmLayoutParams->startIndex))
            return false;
        if (!tlvDmLayoutParams.add8bitEntry(BMT_DM_LINE_INCREMENT, sizeof(profile->dmLayoutParams->lineIncrement32), &profile->dmLayoutParams->lineIncrement32))
            return false;
        if (!tlvDmLayoutParams.add32bitEntry(BMT_DM_LINE_TYPE, sizeof(profile->dmLayoutParams->lineType), &profile->dmLayoutParams->lineType))
            return false;
        if (!tlvProfile.addTlvEntry(BMT_PEP_DM_LAYOUT_PARAMS, tlvDmLayoutParams))
            return false;
    }

    if (profile->dotReorgParams != NULL)
    {
        MediaTlvWriter  tlvDotReorgParams;

        if (!tlvDotReorgParams.add32bitEntry(BMT_DR_IO_MAPPING, sizeof(profile->dotReorgParams->ioMapping), profile->dotReorgParams->ioMapping))
            return false;
        if (!tlvDotReorgParams.add32bitEntry(BMT_DR_REDUN_MATRIX, sizeof(profile->dotReorgParams->redunMatrix), &profile->dotReorgParams->redunMatrix))
            return false;
        if (!tlvDotReorgParams.add32bitEntry(BMT_DR_REDUN_MATRIX_WIDTH, sizeof(profile->dotReorgParams->redunMatrixWidth), &profile->dotReorgParams->redunMatrixWidth))
            return false;
        if (!tlvDotReorgParams.add32bitEntry(BMT_DR_REDUN_MATRIX_LENGTH, sizeof(profile->dotReorgParams->redunMatrixLength), &profile->dotReorgParams->redunMatrixLength))
            return false;
        if (!tlvProfile.addTlvEntry(BMT_PEP_DOT_REORG_PARAMS, tlvDotReorgParams))
            return false;
    }

    return addTlvEntry(BMT_PROFILE, tlvProfile);
}

bool MediaTlvWriter::getTlvMemoryBlock(void **buffer, size_t *length)
{
    if (buffer == NULL || length == NULL)
        return false;
    if (!writeTlvMemoryBlock(NULL, length))
        return false;
    if (*length == 0)
    {
        *buffer = NULL;
        return true;
    }
    if ((*buffer = malloc(*length)) == NULL)
        return false;
    if (!writeTlvMemoryBlock(*buffer, length))
    {
        free(*buffer);
        *buffer = NULL;
    }
    return true;
}

bool MediaTlvWriter::writeTlvMemoryBlock(void *buffer, size_t *length)
{
    uint32_t i;
    uint32_t totalSize = 0;

    if (length == NULL)
        return false;

    // Get the total size
    for (i = 0; i < mEntries.size(); ++i)
    {
        // Check that the value won't cause an overflow.
        if (UINT_MAX - totalSize <= mEntries[i].length + TLV_TAG_HEADER_LENGTH)
            return false;
        totalSize += mEntries[i].length + TLV_TAG_HEADER_LENGTH;
    };

    if (buffer == NULL)
    {
        *length = totalSize;
        return true;
    }

    if (*length < totalSize)
        return false;

    void *tag = buffer;
    for (i = 0; i < mEntries.size(); ++i)
    {
        TLV_TAG(tag) = mEntries[i].tag;
        TLV_LENGTH(tag) = mEntries[i].length;
        if (mEntries[i].length > 0)
            memcpy(TLV_VALUE(tag), mEntries[i].value, mEntries[i].length);
        void *nextTag = TLV_NEXT_TAG(tag);
        TLV_LENGTH(tag) = UINT32_BE(TLV_LENGTH(tag));
        tag = nextTag;
    };

    *length = totalSize;
    return true;
}

void MediaTlvWriter::clear()
{
    size_t i;
    for (i = 0; i < mEntries.size(); ++i)
    {
        if (mEntries[i].value != NULL)
            free(mEntries[i].value);
    }
    mEntries.clear();
}

} // namespace MjPrinterSoftware

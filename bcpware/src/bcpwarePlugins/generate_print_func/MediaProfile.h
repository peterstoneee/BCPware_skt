#ifndef MEDIAPROFILE_H_12000008042011
#define MEDIAPROFILE_H_12000008042011

#include <vector>
#include <string>
#include <cstdint>

typedef std::vector<uint8_t> MediaData;

struct MediaLutArray
{
    std::wstring        dataType;
    std::wstring        format;
    MediaData           data;
};

struct MediaDitherArray
{
    std::wstring        dataType;
    std::wstring        format;
    int32_t             height;
    int32_t             width;
    MediaData           data;
};

struct MediaLut
{
    std::wstring        ref;
    const MediaLutArray *lutArray;
};

struct MediaDither
{
    std::wstring        channel;
    int32_t             shift;
    std::wstring        ref;
    const MediaDitherArray *ditherArray;
};

#endif

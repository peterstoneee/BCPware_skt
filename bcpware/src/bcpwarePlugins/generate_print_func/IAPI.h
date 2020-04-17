/**
* @file    Northstar/Hammerhead ImageAPI
*
* @brief   IAPI.dll interface description
*
*    NOTE: Any changes to this need to be reflected in IAPI.cs and IAPI.def
*
*    2013 Feb 01 - 1.00- Add IAPI_PRINT_MODE_RAWMONO (default 1600x1375), add IAPI_PERF_PAGE_XRES, IAPI_PERF_PAGE_YRES
*                      - *IMPORTANT* added streamId to IAPI_OutputCallback_t
*    2013 Mar 14 - 1.01- Add IAPI_ForceWhiteSlices
*    2013 Mar 14 - 1.02- Add IAPI_CacheImageData, only works for ZX/ZXM jpeg for now
*    2013 Apr 03 - 1.03- Add IAPI_WritePageHeaderWithNoData for RAWMONO output
*                        Expand IAPI_CacheImageData to RAWMONO
*                      - * IMPORTANT* parameters in Open(), NewPage(), NewPageChannels() generalize from zjItems to parameters and parmCount
*    2013 May 14 - 1.04- Aspen 2.0 Release
     2013 May 17 - 1.05- Default to 1600x1375 for RAWMONO
     2013 May 22 - 1.06- Most ABTTlvC parameters in Open() and NewPageChannels() that should be handled are handled. Add ABT Custom tags
     2013 May 30 - 1.07- Support for new ABTI parameters and ABT_PAGE_COUNT.
     2013 Jun 06 - 1.08- ASP-304 1:1 scaling did not support input pixel stride
     2013 Jun 12 - 1.09- ASP-338 Enforce 5 line multiple for RAWMONO
     2013 Jun 17 - 1.10- ASP-359 IAPI_PERF_RAWMONO_MIN_PAGE_CHUNK - removed in 1.15
     2013 Jun 21 - 1.11- ASP-381 Reduce ABT_PRINT_SPEED fractions before writing
     2013 Jun 27 - 1.12- ASP-425 Support ABT_IAPI_VERSION, ABT_RIP_VERSION, ABT_RIP_CONFIGURATION
     2013 Jul  3 - 1.13- New CCU_IO_MAP values
     2013 Jul  3 - 1.14- Pad last data chunk in ABT page to 16 byte boundary
     2013 Jul 18 - 1.15- Remove IAPI_PERF_RAWMONO_MIN_PAGE_CHUNK, make IAPI_PERF_HARD_STITCH work for RAWMONO
     2013 Aug 01 - 1.16- ASPEN-475 Enforce lower level RAWMONO limitations
     2013 Sep 16 - 1.17- HH-1591 VDP failing with out of bitmaps error
     2013 Sep 23 - 1.18- ASP-928 IAPI_WritePageHeaderWithNoData() should bypass input sanity checks since it does not see image data
     2013 Oct 28 - 1.19- NS-1888 Allow multiple non-muxed streams for ZX_800x800 (or any)
     2013 Nov 08 - 1.20- ASP-1145 Allow ABT print speed fraction to exceed 8 bits (allows all possible resolution)
     2013 Nov 25 - 1.21- ASP-1170 Mismatched channels could cause a crash with release build
	 2014 Feb 26 - 1.22- ASP-1806 ABT_DISPLAYABLE_JOB_NAME included in abt header 
     2014 Mar 19 - 1.23- ASP-1806 Reassign ABT_DISPLAYABLE_JOB_NAME to 199103
                       - ASP-2081 Add ABT_ENCODER_MULTIPLIER_NUM/DEN
     2014 Jul 01 - 1.24- NS-2072 Last band on zx corruption
     2014 Aug 05 - 1.25- ASP-3137 Fix xwhite crasher bug on BLANK plane.
     2015 Mar 02 - 1.26- inconsistency in NewPage parameters corrected - corrected to void* *parameters in IAPI.h
*/

// Copyright (c) Memjet Labels 2014
// 
// Portions of this Software contain copyrighted materials from Memjet North Ryde Pty Ltd 
// and its suppliers, including but not limited to Marvell International Ltd and Monotype
// Imaging Inc. Terms governing use, copying, and distribution of such third party materials
// are set out in the Legal Notice accompanying this Software or as permitted under a written
// agreement with respect to this Software.

#pragma once
#include <crtdefs.h> // for size_t
#include <stdio.h>   // for FILE*
#include <stdint.h>  // for uint32_t, etc.

#ifdef __cplusplus
extern "C"
{
#endif
    
    // --------------------------------------------------------------------------------------------------------------------------------

    #define IAPI_VERSION_MAJOR 1
    #define IAPI_VERSION_MINOR 26

    #define MYDLL_EXPORTS
    #ifdef MYDLL_EXPORTS
    #define MY_DLL_API __declspec( dllexport )
    #else
    #define MY_ALL_API __declspec( dllimport )
    #endif

    // - Maximum output width of 7016 pixels at 800 DPI
    // - May override with IAPI_SetPerformanceOption( IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS, n )
    #define IAPI_MAX_IMAGE_WIDTH_PIXELS 7016
    #define MAX_IMAGE_WIDTH_PIXELS IAPI_MAX_IMAGE_WIDTH_PIXELS   // back compat

    typedef enum {
        IAPI_OK = 0,
        IAPI_ERR
    } IAPI_rc_t;

    typedef enum {
        IAPI_FALSE = 0,
        IAPI_TRUE = 1
    } IAPI_Boolean;

    typedef unsigned int IAPI_hdl_t;

    typedef enum {
        IAPI_PRINT_MODE_ZXM_800x1200_4SPIN = 0,  // Hammerhead (printed at 1600x1200)
        IAPI_PRINT_MODE_ZX_800x800,              // Northstar  (printed at 1600x1600 or 1600x800)
        IAPI_PRINT_MODE_RAWMONO,                 // Atlantic   (default 1600x1375) abt format
    } IAPI_print_mode_t;


    // We guarantee that this will only be extended, never rearranged, at least for IAPI_MAJOR_VERSION 1.
    typedef struct {
        uint32_t    streamId;                // 1 to N, or 0 for metadata
        void       *outputClientData;        // the outputClientData you supplied when registering
    } IAPI_OutputCallbackInfo_t;

    // - Returns the number of bytes written (count) or -1 for error
    typedef int __stdcall IAPI_OutputCallback_t( IAPI_OutputCallbackInfo_t *info, const void *buf, size_t count);

    typedef enum {
        IAPI_DATA_FORMAT_PLANAR = 0,                        // dataEntries = colorPlanes + (stencil if needed)
        IAPI_DATA_FORMAT_INTERLEAVED,                       // dataEntries = 1, even stencil is interleaved if needed
        IAPI_DATA_FORMAT_INTERLEAVED_SEPARATE_STENCIL,      // dataEntries = 2 - NOT SUPPORTED AS INPUT,
        IAPI_DATA_FORMAT_PLANAR_RAWMONO,                    // dataEntries = colorplanes - no support for interleaved, 1 bits are black
    } IAPI_data_format_t;

    #define IAPI_MAX_PLANES 6                               // including stencil
    #define IAPI_MAX_CHANNELS 10                            // including CMYK, 5 SPOT, stencil.

    // - See IAPI_SetVariableMode below for full docs - only valid for zx/zxm modes.
    typedef enum {
        IAPI_VARIABLE_DATA_MODE_NONE = 0,       // The default - each page is separate.
        IAPI_VARIABLE_DATA_MODE_CONSTANT,       // Stencil plane 0xff defines lossy areas that will be replicated on _VARIABLE pages
        IAPI_VARIABLE_DATA_MODE_VARIABLE        // Stencil plane defines lossy/lossless as normal, but only in non _CONSTANT areas.
    } IAPI_variable_data_mode_t;

    // - See IAPI_SetLosslessMode below for full docs  - only valid for zx/zxm modes.
    typedef enum {
        IAPI_LOSSLESS_MODE_LOSSLESS = 0,
        IAPI_LOSSLESS_MODE_LOSSY,
        IAPI_LOSSLESS_MODE_STENCIL,
        IAPI_LOSSLESS_MODE_AUTOSTENCIL,
    } IAPI_lossless_mode_t;


    // --------------------------------------------------------------------------------------------------------------------------------
    // Useful values for zjItems
    // --------------------------------------------------------------------------------------------------------------------------------
#define ZJITEM_PAGECOUNT 0x0000            // IAPI_Open(): Not required for either format, but informational
#define ZJITEM_DMCOPIES  0x0004            // IAPI_Open(): REQUIRED for ZXM_800x1200 - First zjItem, even if only set to 1
#define ZJITEM_RASTER_X  0x000c            // IAPI_Open(): REQUIRED only for ZXM_800x1200 - One zjItem for each page
#define ZJITEM_RASTER_Y  0x000d            // IAPI_Open(): REQUIRED only for ZXM_800x1200 - One zjItem for each page

#define ZJITEM_QUALITY   0x0071            // IAPI_NewPage(): Only needed for ZX_800x800
#define ZJITEM_QUALITY_NORMAL 0xFFFFFFFD   // 12 ips mode value for ZJITEM_QUALITY
#define ZJITEM_QUALITY_BEST   0xFFFFFFFC   // 6 ips mode value for ZJITEM_QUALITY

#define ZJITEM_RIP_ID         0x8605       // IAPI_Open(): Which RIP. 0x100 = TiffPrint, 0x200 = MJMMR, 0x300=[reserved]
#define ZJITEM_RIP_VERSION    0x8606       // IAPI_Open(): 32 bits, depends on RIP
#define ZJITEM_RIP_TIME       0x8607       // IAPI_Open(): build date of RIP, seconds since Jan 1 1970 UTC (unix epoch)
#define ZJITEM_JOB_TIME       0x8608       // IAPI_Open(): (start) time of job, seconds since Jan 1 1970 UTC (unix epoch)

#define ZJITEM_STITCHED_MODE 0x8400        // IAPI_Open(): if constant/variable mode, set to 1
#define ZJITEM_STITCHED_MODE_NUM_EOP_BLANK_LINES 0x8404 // IAPI_Open(): if constant/variable mode, set to 20

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Returns the IAPI.dll version, to check compatibility with your IAPI.h version.
    // - So far back compat is presevered - you are okay as long as the version is higher.
    // - Introduced in v0.5 so earlier DLLs will not have this export.
    // - May be called at any time.
    typedef void __stdcall
    IAPI_GetVersion_t(
        unsigned int *major,                    // output parameter
        unsigned int *minor                     // output parameter
    );
    MY_DLL_API IAPI_GetVersion_t IAPI_GetVersion;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Use to create a new IAPI job
    // - Valid at any time, though normally you would IAPI_Close() a previous instance first.
    // - If the return IAPI_hdl_t is not > 0, use 0 as handle to IAPI_GetLastError() to find the error.
    //
    // - Your outputCallback() will be invoked a different thread from the one calling IAPI_WriteImageData()
    //       in unless you use IAPI_SetPerformanceOptions( IAPI_PERF_MAX_OUTPUT_THREADS, 0 ) before IAPI_Open()
    // - In IAPI_PRINT_MODE_ZXM_800x1200_4SPIN mode, we guarantee that the first outputCallback() invoke will be
    //       the entire zxm header and that each successive outputCallback() will be a complete output packet for
    //       a single output stream and that the first 8 bytes are the zxm packet header. Thus your output is
    //       already demuxed if you are trying to RIP while printing. For ZX_800x800 mode it doesn't matter.
    // - Convention for ZXM_800x1200 files (do not do this for ZX_800x800 files) is that the document
    //       header contains ZJITEM_DMCOPIES first to indicate the x/y page sizes will be coming, then a list 
    //       of output X and Y page sizes for every page in the file for fastest printing.
    //       Add to zjItems array:  ZJITEM_DMCOPIES, 1
    //       Use IAPI_GetOutputPageSize() below to get the output page sizes, then add to zjItems array
    //           ZJITEM_RASTER_X, x_pixels, ZJITEM_RASTER_Y, y_pixels,
    //       for each page before you do the Open() call
    typedef IAPI_hdl_t __stdcall
    IAPI_Open_t(
        IAPI_print_mode_t       mode,
        unsigned int            outputStreams,     // This is the number of OUTPUT streams, not including stencil or transparency.
        const char*             *outputNames,      // CID(:FID)(@ENGINE) - 'CYAN@1' for CYAN on engine 1, or 'CMYK' for single northstar
        void*                   *parameters,       // Specific to mode. For ZX/ZXM this is unsigned int* with tag, value, tag, value
                                                   //                   For RAWMONO this is struct AbtTlvC* - only ABT_OEM_CUSTOM_TAGS should be set
        unsigned int            parmCount,         // parameters count. For ZX/ZXM this is number of PAIRS. For RAWMONO this is number of items.
        IAPI_OutputCallback_t*  outputCallback,    // caller routine to handle the output zx/zxm data
        void                    *outputClientData  // transparently passed back to the outputCallback
    );
    MY_DLL_API IAPI_Open_t IAPI_Open;


    // - Calculate the output size of a page to pass to Open().
    // - Can be called at any time.
    typedef IAPI_rc_t __stdcall
    IAPI_GetOutputPageSize_t(
        IAPI_print_mode_t       mode,
        unsigned int            srcWidth,           // unscaled page size (at srcXRes)
        unsigned int            srcHeight,          // unscaled page size (at srcYRes)
        unsigned int            srcXRes,
        unsigned int            srcYRes,
        unsigned int           *outWidth,           // at native res, for Open() zjItems RASTER_X
        unsigned int           *outHeight           // at native res, for Open() zjItems RASTER_Y
    );
    MY_DLL_API IAPI_GetOutputPageSize_t IAPI_GetOutputPageSize;


    // - Flushes the stream and frees large memory buffers.
    // - It is not an error to close an already closed handle or handle==0 - just ignored.
    // - If the return code is not IAPI_OK use 0 as handle to IAPI_GetLastError() to find the error.
    // - Valid at end of job after last EndPage()
    typedef IAPI_rc_t __stdcall
    IAPI_Close_t(
        IAPI_hdl_t              handle
    );
    MY_DLL_API IAPI_Close_t IAPI_Close;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Valid between Open() and Close() to get the last error for a specific handle.
    // - Can be called with handle 0 at any time to get last global error when handle is invald.
    typedef const char* __stdcall 
    IAPI_GetLastError_t(
        IAPI_hdl_t              handle
    );
    MY_DLL_API IAPI_GetLastError_t IAPI_GetLastError;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Valid before a NewPage() - only valid for zx/zxm modes.
    typedef IAPI_rc_t __stdcall
    IAPI_SetLossyParameters_t(
        IAPI_hdl_t              handle, 
        unsigned int            quality,        // 1 (worst) to 100 (best). 80 is 'normal'.
        unsigned int            xRes,           // Desired jpeg res. (xRes * yRes) > 384000 requires slower printing speeds.
        unsigned int            yRes            // Desired jpeg res. For Hammerhead MUST be same as yres
    );
    MY_DLL_API IAPI_SetLossyParameters_t IAPI_SetLossyParameters;


    typedef enum {
        // This is more of a bitmask than an enum.
        // Suggest just using IAPI_THROUGHPUT_BW
        IAPI_THROUGHPUT_NONE   = 0,
        IAPI_THROUGHPUT_BW     = 1,  // almost no performance impact
        IAPI_THROUGHPUT_CTA    = 2,  // some performance impact (produced ZX must be analyzed)
        IAPI_THROUGHPUT_ALL    = 0xffff
    } IAPI_throughput_t;

    // NOTE: It is suggested Hammerhead use IAPI_EnableIPSTracking() and IAPI_GetIPS() instead.
    // - BW monitors the bandwidth use of larger areas on the page and decides 
    //       which need their rendering parameters tweaked so the print pipeline does not underrun.
    // - CTA monitors the complexity of the output - it has similar effects to BW,
    //       but deals with the processing limits of the OmniPEC.
    // - IAPI_RegisterEventCallback() can notify the caller of all decisions.
    // - IAPI_SetPerformanceParameters() may be used to tune the values.
    //
    // SetOptimize()   LosslessMode() Effect
    // --------------- ------------   ----------------------------------------------------------------
    //  OFF            _LOSSLESS      Entire page rendered lossless (possible underuns)
    //  OFF            _LOSSY         Entire page rendered lossy (possible underuns)
    //  OFF            _STENCIL       Stencil dictates which areas are lossless (possible underuns)
    //  OFF            _AUTOSTENCIL   IAPI decides which areas need to be lossy due to rapidly
    //                                varying local content (possible underruns)
    //  (any)          _LOSSLESS      Will try to render entire page lossless, but BW/CTA
    //                                may override this locally for degenerately complex areas.
    //  (any)          _LOSSY         Entire page rendered lossy, BW/CTA may tweak local parameters.
    //                                If you know the entire page is a photo, this may render faster.
    //  (any)          _STENCIL       BW/CTA will respect the stencil and will only reduce jpeg
    //                                quality in order to reach bandwidth goals.
    //  (any)          _AUTOSTENCIL   IAPI decides which areas need to be lossy due to rapidly
    //                                varying local content.

    // NOTE: It is suggested Hammerhead use IAPI_EnableIPSTracking() and IAPI_GetIPS() instead.
    // - Valid after Open(), before any NewPage(), but makes more sense on or off for the whole job. - only valid for zx/zxm modes.
    typedef IAPI_rc_t __stdcall 
    IAPI_SetOptimize_t(
        IAPI_hdl_t              handle,
        IAPI_throughput_t       mode
    );
    MY_DLL_API IAPI_SetOptimize_t IAPI_SetOptimize;

    // NOTE: It is suggested Hammerhead use IAPI_EnableIPSTracking() and IAPI_GetIPS() instead.
    // Note: Sets IAPI_THROUGHPUT_BW if true, IAPI_THROUGHPUT_NONE if false. Use IAPI_SetOptimize() for finer control
    // - Valid after Open(), before any NewPage(), but makes more sense on or off for the whole job. - only valid for zx/zxm modes.
    typedef IAPI_rc_t __stdcall 
    IAPI_SetBWOptimize_t(
        IAPI_hdl_t              handle,
        IAPI_Boolean            BWOptimize
    );
    MY_DLL_API IAPI_SetBWOptimize_t IAPI_SetBWOptimize;


    // - See table just above
    // - Valid before a NewPage()
    typedef IAPI_rc_t __stdcall 
    IAPI_SetLosslessMode_t(
        IAPI_hdl_t              handle,
        IAPI_lossless_mode_t    mode
    );
    MY_DLL_API IAPI_SetLosslessMode_t IAPI_SetLosslessMode;

    // - Change the value (default 0x00) that designates lossless areas for stencil.
    // - IAPI_SetLosslessModeLosslessValue() can change this polarity.
    // - Valid at any time in a job, though normally before a NewPage(). - only valid for zx/zxm modes.
    typedef IAPI_rc_t __stdcall
    IAPI_SetLosslessModeLosslessValue_t(
        IAPI_hdl_t              handle,
        unsigned char           value
    );
    MY_DLL_API IAPI_SetLosslessModeLosslessValue_t IAPI_SetLosslessModeLosslessValue;

    // backwards compat - use IAPI_SetLosslessMode() instead
    typedef IAPI_rc_t __stdcall IAPI_SetLosslessModeAll_t( IAPI_hdl_t handle, IAPI_Boolean isLossless );
    MY_DLL_API IAPI_SetLosslessModeAll_t IAPI_SetLosslessModeAll;
    // backwards compat - use IAPI_SetLosslessMode() instead
    typedef IAPI_rc_t __stdcall IAPI_SetLosslessModeStencil_t( IAPI_hdl_t handle );
    MY_DLL_API IAPI_SetLosslessModeStencil_t IAPI_SetLosslessModeStencil;

    // --------------------------------------------------------------------------------------------------------------------------------


    // - Valid after Open() - only valid for zx/zxm modes.
    // - Can be set before any NewPage(), but makes more sense on or off for the whole job.
    typedef IAPI_rc_t __stdcall 
    IAPI_SetIPSTracking_t(
        IAPI_hdl_t             handle,
        IAPI_throughput_t      mode      // Default _BW for 800x1200_ZXM, _NONE for 800x800_ZX
    );
    MY_DLL_API IAPI_SetIPSTracking_t IAPI_SetIPSTracking;

    // - Valid after Open() - only valid for zx/zxm modes.
    // - Really only makes sense at the end of the job, but can be queried any time.
    // - This is automatically added to the zxm by IAPI, this call is only informational.
    // - The unsigned int is treated as four bytes. Each byte is either 0 (no recommendation)
    //   or the max speed (in IPS) calculated for a method. The four bytes are:
    //      (ips >> 24) && 0xff: reserved
    //      (ips >> 16) && 0xff: reserved
    //      (ips >>  8) && 0xff: Complexity Threshold recommended IPS
    //      (ips >>  0) && 0xff: Bandwidth recommended IPS
    //   Hammertime will use the minimum non-zero value as the print speed (or max if all zero).
    typedef IAPI_rc_t __stdcall 
    IAPI_GetIPS_t(
        IAPI_hdl_t              handle,
        unsigned int           *ips      // return value
    );
    MY_DLL_API IAPI_GetIPS_t IAPI_GetIPS;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - only valid for zx/zxm modes.
    // - First page in a _CONSTANT/_VARIABLE series must be _CONSTANT.
    //     - Requires IAPI_SetLosslessMode( IAPI_LOSSLESS_MODE_STENCIL ) for the first page.
    //     - Nothing will be printed for this page. 
    //     - Areas marked as lossy will be saved for overlay on _VARIABLE pages
    // - Subsequent pages must be _VARIABLE:
    //     - Must be the *same size* as the CONSTANT. 
    //     - Only areas that are not 'stamped' with the CONSTANT image will be printed. 
    //     - IAPI_SetLosslessMode() works normally in these areas.
    // - You can change the _CONSTANT image by sending nother _CONSTANT page, or turn it off with _NONE pages.
    // - _NONE pages free the _CONSTANT bitmaps, so after a _NONE page you need another _CONSTANT page to do more _VARIABLE 
    // - Valid before a NewPage() - only valid for zx/zxm modes.
    //
    // - For now: Output must run in stitched mode - not a problem for Hammerhead, it is for Northstar
    // - For now: No multiple copies - replaying creating bitmaps when you haven't freed them yet causes a coredump
    typedef IAPI_rc_t __stdcall
    IAPI_SetVariableMode_t(
        IAPI_hdl_t                handle,
        IAPI_variable_data_mode_t mode
     );
    MY_DLL_API IAPI_SetVariableMode_t IAPI_SetVariableMode;

    // - By default, 0x00 in the stencil plane of the _CONSTANT image defines the which sections will be variable data.
    //   That value can be changed with this call.
    // - Valid at any time in a job, though normally before a NewPage(). - only valid for zx/zxm modes.
    typedef IAPI_rc_t __stdcall
    IAPI_SetVariableModeVariableValue_t(
        IAPI_hdl_t                handle,
        unsigned char             value
     );
    MY_DLL_API IAPI_SetVariableModeVariableValue_t IAPI_SetVariableModeVariableValue;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Turn an ICM with Memjet dither information imbedded into media transforms (BMTs) to be send down with the job.
    // - Only the dither information is used, any color transformation must be done before IAPI.
    // - Only valid between Open() and first NewPage().
    // - Tech note: ZXM jobs will return with one entry per colorPlanes, but ZX jobs will only return with
    //   one entry in each array set since they only have one output stream.
    // - For RAWMONO output with contone input, this will provide the dither for IAPI to use. Color transform ignored.
    typedef IAPI_rc_t __stdcall
    IAPI_MediaTransformsFromICMFile_t(
        IAPI_hdl_t              handle,
        const wchar_t*          fileName,           // path to .icm file
        unsigned int            colorPlanes,        // Number of OUTPUT color planes (as in IAPI_Open())
        const char*             *colorNames,        // as in IAPI_Open()
        IAPI_Boolean            sendLUT,            // almost always false (OmniPEC can't do CMYK to CMYK)
        IAPI_Boolean            persistent,         // almost always false - keep transform persistent between jobs?
        unsigned char*          *bmts,              // output bmts, you pass in array of bmts[colorPlanes] NULLs.
        unsigned int            *bmt_sizes          // output bmts, you pass in array of bmt_sizes[colorPlanes].
    );
    MY_DLL_API IAPI_MediaTransformsFromICMFile_t IAPI_MediaTransformsFromICMFile;

    // - Like IAPI_MediaTransformsFromICMFile, but for .icm in memory.
    // - Only valid between Open() and first NewPage().
    // - For RAWMONO output with contone input, this will provide the dither for IAPI to use. Color transform ignored.
    typedef IAPI_rc_t __stdcall
    IAPI_MediaTransformsFromICM_t(
        IAPI_hdl_t              handle,
        unsigned char*          icm,                // ICM data in memory
        unsigned int            icm_size,           // size of ICM data
        unsigned int            colorPlanes,        // Number of OUTPUT color planes (as in IAPI_Open())
        const char*             *colorNames,        // as in IAPI_Open()
        IAPI_Boolean            sendLUT,            // almost always false (OmniPEC can't do CMYK to CMYK)
        IAPI_Boolean            persistent,         // almost always false - keep transform persistent between jobs?
        unsigned char*          *bmts,              // output bmts, you pass in array of bmts[colorPlanes] NULLs.
        unsigned int            *bmt_sizes          // output bmts, you pass in array of bmt_sizes[colorPlanes].
    );
    MY_DLL_API IAPI_MediaTransformsFromICM_t IAPI_MediaTransformsFromICM;

    // - Embed the bmts returned by IAPI_MediaTransformsFromICM[File] in your output stream(s).
    // - Only valid between Open() and first NewPage().
    // - For RAWMONO output with contone input, this will provide the dither for IAPI to use. Color transform ignored.
    typedef IAPI_rc_t __stdcall
    IAPI_EmbedMediaTransforms_t(
        IAPI_hdl_t              handle,
        unsigned int            colorPlanes,
        unsigned char*          *bmts,              // from IAPI_MediaTransformsFromICM[File]
        unsigned int            *bmt_sizes          // from IAPI_MediaTransformsFromICM[File]
    );
    MY_DLL_API IAPI_EmbedMediaTransforms_t IAPI_EmbedMediaTransforms;

    // - Free the bmts returned by IAPI_MediaTransformsFromICM[File].
    // - Valid at any time in a job, but only makes sense just after IAPI_EmbedMediaTransforms().
    typedef IAPI_rc_t __stdcall
    IAPI_FreeMediaTransforms_t(
        IAPI_hdl_t              handle,
        unsigned int            colorPlanes,
        unsigned char*          *bmts,              // from IAPI_MediaTransformsFromICM(File)
        unsigned int            *bmt_sizes          // from IAPI_MediaTransformsFromICM(File)
    );
    MY_DLL_API IAPI_FreeMediaTransforms_t IAPI_FreeMediaTransforms;

    // --------------------------------------------------------------------------------------------------------------------------------
    
    // - Valid after Open(). If used in a page, new slices must be same size as old slices.
    // - ZXM formats only
    // - Tells IAPI to take a vertical slice of the image for a single color plane.
    // - You must have one output plane (IAPI_Open() colorNames) per output slice per channel. Ex:
    //      { "CYAN", "MAGENTA", "YELLOW", "BLACK", "CYAN", "MAGENTA", "YELLOW", "BLACK" }
    // - 'slices' must match the IAPI_Open() 'colorPlanes' value
    // - Slices may overlap or be separated
    // - Slice positions may change between pages to allow for alignment 
    // - For this implementation, all slices must be the same size - may be lifted in the future.
    typedef IAPI_rc_t __stdcall
    IAPI_SetVerticalSlices_t(
        IAPI_hdl_t              handle,
        unsigned int            slices,       // MUST be the same as IAPI_Open() colorPlanes
        int                    *leftPixel,    // array of left pixel in output coordinates for each slice. May be negative (white filled).
        int                    *rightPixel    // array of right pixel+1 in output coordinates for each slice. May be negative (blank slice).
    );
    MY_DLL_API IAPI_SetVerticalSlices_t IAPI_SetVerticalSlices;

    // Forces vertical slices of the print to be white, to create soft margins. Combined with SetVerticalSlices,
    // for instance, this allows a 3 pen wide setup where only half of the outside pens are used.
    // - Valid any time after Open() including during a page.
    // - You may have any number of slices, and that number may change at any time (use 0 to clear).
    // - Slices do not need to be the same size.
    // - Slices may overlap, but that just results in repeated work.
    typedef IAPI_rc_t __stdcall
    IAPI_ForceWhiteSlices_t(
        IAPI_hdl_t              handle,
        unsigned int            slices,       // not the same as SetVerticalSlices
        int                    *leftPixel,    // array of left pixel in output coordinates for each slice
        int                    *rightPixel    // array of right pixel in output coordinates for each slice
        );
    MY_DLL_API IAPI_ForceWhiteSlices_t IAPI_ForceWhiteSlices;

    // --------------------------------------------------------------------------------------------------------------------------------

    // - Informs IAPI a new source image page is coming.
    // - This version is convenience for NorthStar where the color planes are fixed.
    // - Valid after Open() or after EndPage().
    // - For ZX_800x800 mode you will want to supply for zjItems:
    //           ZJITEM_QUALITY, ZJITEM_QUALITY_BEST or _NORMAL
    typedef IAPI_rc_t __stdcall
    IAPI_NewPage_t(
        IAPI_hdl_t              handle,
        unsigned int            srcWidth,           // unscaled page size (at srcXRes)
        unsigned int            srcHeight,          // unscaled page size (at srcYRes)
        unsigned int            srcXRes,
        unsigned int            srcYRes,
        void*                   *parameters,        // same as in IAPI_Open() - should always be zjItems for this call
        unsigned int            paramCount
    );
    MY_DLL_API IAPI_NewPage_t IAPI_NewPage;
    
    // - Informs IAPI a new source image page is coming and what its input planes are.
    // - Valid after Open() or after EndPage().
    // - If using _RAWMONO output and _RAWMONO input, srcXRes must be 1600, srcYRes must be same as output yres,
    //      input size must be exactly same size as output size.
    typedef IAPI_rc_t __stdcall
    IAPI_NewPageChannels_t(
        IAPI_hdl_t              handle,
        unsigned int            channels,           // Total number of channels in INPUT image data
        const char*             *channelNames,      // ASCII strings with channel names, or blank string (i.e. "") when channel 
                                                    //     not present. MUST correspond to channels. For ZX_800x800 this must be same as
                                                    //     Open() channels with "STENCIL" channel on end if used. For ZXM_800x1200_4SPIN
                                                    //     the channels may be in different order (but must be present if in the Open() call)
                                                    //     and the "STENCIL" channel must be labeled if present.
        unsigned int            srcWidth,           // unscaled page size (at srcXRes)
        unsigned int            srcHeight,          // unscaled page size (at srcYRes)
        unsigned int            srcXRes,            // must be 1600 if IAPI_DATA_FORMAT_PLANAR_RAWMONO input
        unsigned int            srcYRes,            // must be output yres if IAPI_DATA_FORMAT_PLANAR_RAWMONO input
        void                    *parameters,        // same as in IAPI_Open() - depends on print mode
        unsigned int            paramCount
        );
    MY_DLL_API IAPI_NewPageChannels_t IAPI_NewPageChannels;

    // - Transform and write another band of image data.
    // - This is only somewhat synced with the outputCallback() output data. Internally the
    //   data is processed in chunks of optimum size, and those are what comes out outputCallback().
    // - For best performance, srcHeight in multiples of 128 lines.
    // - For best ZXM performance, format is IAPI_DATA_FORMAT_PLANAR.
    // - For best ZX  performance, format is IAPI_DATA_FORMAT_INTERLEAVED.
    // - For best RAWMONO performance, format is IAPI_DATA_FORMAT_PLANAR_RAWMONO
    // - Valid between NewPage() and EndPage().
    // - The total srcHeights for a page MUST equal the value given to NewPage().
    typedef IAPI_rc_t __stdcall
    IAPI_WriteImageData_t(
        IAPI_hdl_t          handle,
        IAPI_data_format_t  format,
        unsigned int        dataEntries,
        unsigned char*      *data,                  // One pointer per dataEntries
        unsigned int        srcWidth,               // in pixels
        unsigned int        srcHeight,              // in pixels
        unsigned int        pixelStride             // must be >= srcWidth
    );
    MY_DLL_API IAPI_WriteImageData_t IAPI_WriteImageData;

    // - Only valid for IAPI_PRINT_MODE_RAWMONO
    // - Normally IAPI will accumulate your data and when the page is done it will write the page header and then all the data (required by ABT format)
    // - This call will cause it to write the header for the page - you can then write the data for your entire page. This will save one copy
    //   of the data, which can be substantial.
    // - Your assumptions on data size (bytesPerPlane) and IAPI's assumption on data size must match
    // - As a reminder, your image width, and thus the bytesPerPlane, must be a multiple of 128 bits (IAPI_GetOutputImageSize() will do this for you)
    typedef IAPI_rc_t __stdcall
    IAPI_WritePageHeaderWithNoData_t(
        IAPI_hdl_t          handle,
        unsigned int        bytesPerPlane           // How many bytes of data you will be writing to each stream. This must match IAPI's calculation.
    );
    MY_DLL_API IAPI_WritePageHeaderWithNoData_t IAPI_WritePageHeaderWithNoData;


    // Experimental caching - call AFTER IAPI_NewPage(). 
    //  - In ZM/ZXM output only works with jpeg for now.
    //  - Works with RAWMONO output.
    typedef IAPI_rc_t __stdcall
    IAPI_CacheImageData_t(
        IAPI_hdl_t              handle,
        unsigned int            hash,           // unique hash for the source page
        unsigned int            extra,          // extra parameter, unused so far
        IAPI_Boolean           *imageDataNeeded // if IAPI_FALSE you do not need to supply actual *data to IAPI_WriteImageData
        );
    MY_DLL_API IAPI_CacheImageData_t IAPI_CacheImageData;

    // - Inform IAPI the page is done.
    // - Valid after NewPage() and WriteImageData() has been given the promised number of input lines.
    typedef IAPI_rc_t __stdcall
    IAPI_EndPage_t(
        IAPI_hdl_t          handle
    );
    MY_DLL_API IAPI_EndPage_t IAPI_EndPage;

    // --------------------------------------------------------------------------------------------------------------------------------
    // Register for informational event messages
    //
    // - IAPI_RegisterEventCallback() with your callback address. 
    // - You will get periodically get informational messages - generally at the end of internal band processing.
    // - You may want to treat some of these as user warnings (IAPI_Event_bwopt_to_lossy in IAPI_LOSSLESS_MODE_STENCIL)
    // --------------------------------------------------------------------------------------------------------------------------------
    typedef struct {
        unsigned int        event;       // 8 bits: stream ID, 8 bits: subsystem ID, 16 bits: event ID
        unsigned int        data[3];     // varies by event
    } IAPI_EventMsg_t;

    typedef void __stdcall IAPI_EventCallback_t( void *cbClientData, const IAPI_EventMsg_t *msgs, unsigned int count );
    
    // - Valid between Open() and Close().
    // - Currently only a single callback per instance is allowed.
    typedef IAPI_rc_t __stdcall
    IAPI_RegisterEventCallback_t(
        IAPI_hdl_t               handle,
        IAPI_EventCallback_t    *cb,
        void                    *cbClientData // passed as first parameter to your callback
    );
    MY_DLL_API IAPI_RegisterEventCallback_t IAPI_RegisterEventCallback;

    // Make sure to mask off the stream ID (top 8 bits) of .event before comparing it to IAPI_EventType_t
    typedef enum {
        // bwopt subsytem - only valid for zx/zxm
        IAPI_Event_bwopt                    = 0x10 << 16,
        IAPI_Event_bwopt_newpage,           // data[0]: page #,      data[1]: page budget,   data[2]: base budget per 32-line band
        IAPI_Event_bwopt_endpage,           // data[0]: page #,      data[1]: page budget,   data[2]: page used
        IAPI_Event_bwopt_band,              // data[0]: band y,      data[1]: band used,     data[2]: buffer level
        IAPI_Event_bwopt_Q_reduced,         // data[0]: band y,      data[1]: Q factor,      data[2]: total passes
             // jpeg encoding quality was reduced because the encoded band was too large
        IAPI_Event_bwopt_to_lossy,          // data[0]: y,           data[1]: lines,         data[2]: pixels
             // A lossless area was changed to lossy
        IAPI_Event_bwavg_band,              // data[0]: y,           data[1]: % of base avg, data[2]: running average
        IAPI_Event_bwraw_band,              // data[0]: rle bytes,   data[1]: jpeg bytes,    data[2]: weighted value
        IAPI_Event_bwavg_suggested_ips,     // data[0]: ips,         data[1]: baseline avg,  data[2]: max avg

        // ctaopt subsytem - only valid for zx/zxm
        IAPI_Event_ctaopt                      = 0x11 << 16,
        IAPI_Event_ctaopt_newpage,          // data[0]: page #,      data[1]: page budget,   data[2]: base budget per 32-line band
        IAPI_Event_ctaopt_endpage,          // data[0]: page #,      data[1]: page budget,   data[2]: page used
        IAPI_Event_ctaopt_band,             // data[0]: band y,      data[1]: band used,     data[2]: band size (lines)
        IAPI_Event_ctaopt_Q_reduced,        // data[0]: band y,      data[1]: Q factor,      data[2]: total passes
             // jpeg encoding quality was reduced because the encoded band was too large
        IAPI_Event_ctaopt_to_lossy,         // data[0]: y,           data[1]: lines,         data[2]: pixels
             // A lossless area was changed to lossy
        IAPI_Event_ctaavg_band,             // data[0]: y,           data[1]: % of base avg, data[2]: running average
        IAPI_Event_ctaraw_band,             // data[0]: raw cta,     data[1]: jpeg pixels,   data[2]: stretchblts
        IAPI_Event_ctaavg_suggested_ips,    // data[0]: ips,         data[1]: baseline avg,  data[2]: max avg
    } IAPI_EventType_t;

    // --------------------------------------------------------------------------------------------------------------------------------
    // Internal performance options
    //
    // These must set these before Open() to have any effect.
    // --------------------------------------------------------------------------------------------------------------------------------

    typedef enum {
        // These work in all modes
        IAPI_PERF_MAX_SCALING_THREADS = 0,   // default is 12 - set 0 for non-threaded
        IAPI_PERF_MAX_OUTPUT_THREADS,        // default is 1  - set 0 for non-threaded. More than 1 does nothing.
        IAPI_PERF_PAGE_XRES,                 // non-zero is override - may result in non-functional output. Before Open() or GetOutputPageSize()
        IAPI_PERF_PAGE_YRES,                 // non-zero is override - may result in non-functional output. Before Open() or GetOutputPageSize()

        // These are only valid for _ZX or _ZXM modes.
        IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS=0x80, // default is IAPI_MAX_IMAGE_WIDTH_PIXELS - larger uses linearly more memory
        IAPI_PERF_JPEG_DECOUPLE,             // jpeg band decoupling factor - default 1
        IAPI_PERF_WHITNEY,                   // printing on Whitney engine, different header info - default 0
        IAPI_PERF_HARD_STITCH,               // hard stitch this many pages together as one - default 0 (don't do it). Your zjItems for
                                             //     a NewPage() must have RASTER_X and RASTER_Y set properly for each group of pages.

        // These are only valid for _ZX or _ZXM modes.
        IAPI_PERF_AS_MAPS=0x100,             // Save auto-stencil and bwopt maps as one tiff per channel (false)
        IAPI_PERF_AS_PIXELS_1,               // Auto-stencil run length minimum
        IAPI_PERF_AS_PIXELS_2,               // Auto-stencil straight length to kick into complex mode
        IAPI_PERF_AS_PIXELS_3,               // Auto-stencil run length to revert to lossless mode
        IAPI_PERF_AS_DIAGOUT,                // 0xTTNNNN All output replaced with (TT: 1=rle, 2=straight, 3=stretchblt)s of length NNNN each

        // These are only valid for _ZX or _ZXM modes.
        IAPI_PERF_BWOPT_LOGS=0x200,          // Output BWOptimize debugging info (default: false)
        IAPI_PERF_BWOPT_RATE_NS,             // NorthStar  - Bandwidth per second
        IAPI_PERF_BWOPT_POOL_NS,             // NorthStar  - Starting memory pool size
        IAPI_PERF_BWOPT_RATE_HH,             // Hammerhead - Bandwidth per second per channel
        IAPI_PERF_BWOPT_POOL_HH,             // Hammerhead - Starting printer memory pool size
        IAPI_PERF_BWOPT_TARGET_WEIGHT,       // How strongly usage is 'pulled' back towards full (0-100, default 50)
        IAPI_PERF_BWOPT_RLE_WEIGHT,          // RLE size multipled by this percentage (def 200)
        IAPI_PERF_BWOPT_TRACKING_LENGTH,     // Length of bandwidth running average, in inches, we estimate speed over. 0 = entire file (default 0).

        // These are only valid for _ZX or _ZXM modes.
        // CTA (Complexity Threshold Anaylsis) allocates a certain amount of processing time per activity per band.
        // The CTA is not recommended at the current time, but it is still present, and these parameters are present to tweak it.
        IAPI_PERF_CTAOPT_RLE_BUDGET=0x300,   // Assuming nothing else is happening, pure lossless gets this percentage of the raw CTA value. (default 2500)
        IAPI_PERF_CTAOPT_JPEG_BUDGET,        // The efficiency of the jpeg decode, in percent, based on 40 megapixels per second (default 87%)
        IAPI_PERF_CTAOPT_BAND_OVERHEAD,      // Percent of CTA wasted on other overhead per band (default 0)
        IAPI_PERF_CTAOPT_STRETCHBLT_COST,    // Every 1000 jpeg sections spend this percent of raw CTA budget (default 0)
        IAPI_PERF_CTAOPT_TRACKING_LENGTH,    // Length of CTA running average, in inches, we estimate speed over. 0 = entire file (default 24)

        // Only valid for _RAWMONO modes.
        IAPI_PERF_RAWMONO_FORMAT = 0x400,    // IAPI_perf_rawmono_format_t (default _abt)
    } IAPI_perf_t;

    typedef enum {
        IAPI_PERF_RAWMONO_FORMAT_ABT  = 0,
        IAPI_PERF_RAWMONO_FORMAT_TIFF = 1,
    } IAPI_perf_rawmono_format_t;

    // - Must be set before an Open() to have any effect
    typedef IAPI_rc_t __stdcall
    IAPI_SetPerformanceOption_t(
        IAPI_perf_t         option,
        int                 value
    );
    MY_DLL_API IAPI_SetPerformanceOption_t IAPI_SetPerformanceOption;

    // - Valid at any time
    typedef IAPI_rc_t __stdcall
    IAPI_GetPerformanceOption_t(
        IAPI_perf_t         option,
        int                *value
    );
    MY_DLL_API IAPI_GetPerformanceOption_t IAPI_GetPerformanceOption;


    // --------------------------------------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

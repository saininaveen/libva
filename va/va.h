/*
 * Copyright (c) 2007-2009 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL INTEL AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * Video Acceleration (VA) API Specification
 *
 * Rev. 0.30
 * <jonathan.bian@intel.com>
 *
 * Revision History:
 * rev 0.10 (12/10/2006 Jonathan Bian) - Initial draft
 * rev 0.11 (12/15/2006 Jonathan Bian) - Fixed some errors
 * rev 0.12 (02/05/2007 Jonathan Bian) - Added VC-1 data structures for slice level decode
 * rev 0.13 (02/28/2007 Jonathan Bian) - Added GetDisplay()
 * rev 0.14 (04/13/2007 Jonathan Bian) - Fixed MPEG-2 PictureParameter structure, cleaned up a few funcs.
 * rev 0.15 (04/20/2007 Jonathan Bian) - Overhauled buffer management
 * rev 0.16 (05/02/2007 Jonathan Bian) - Added error codes and fixed some issues with configuration
 * rev 0.17 (05/07/2007 Jonathan Bian) - Added H.264/AVC data structures for slice level decode.
 * rev 0.18 (05/14/2007 Jonathan Bian) - Added data structures for MPEG-4 slice level decode 
 *                                       and MPEG-2 motion compensation.
 * rev 0.19 (08/06/2007 Jonathan Bian) - Removed extra type for bitplane data.
 * rev 0.20 (08/08/2007 Jonathan Bian) - Added missing fields to VC-1 PictureParameter structure.
 * rev 0.21 (08/20/2007 Jonathan Bian) - Added image and subpicture support.
 * rev 0.22 (08/27/2007 Jonathan Bian) - Added support for chroma-keying and global alpha.
 * rev 0.23 (09/11/2007 Jonathan Bian) - Fixed some issues with images and subpictures.
 * rev 0.24 (09/18/2007 Jonathan Bian) - Added display attributes.
 * rev 0.25 (10/18/2007 Jonathan Bian) - Changed to use IDs only for some types.
 * rev 0.26 (11/07/2007 Waldo Bastian) - Change vaCreateBuffer semantics
 * rev 0.27 (11/19/2007 Matt Sottek)   - Added DeriveImage
 * rev 0.28 (12/06/2007 Jonathan Bian) - Added new versions of PutImage and AssociateSubpicture 
 *                                       to enable scaling
 * rev 0.29 (02/07/2008 Jonathan Bian) - VC1 parameter fixes,
 *                                       added VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED
 * rev 0.30 (03/01/2009 Jonathan Bian) - Added encoding support for H.264 BP and MPEG-4 SP and fixes
 *                                       for ISO C conformance.
 * rev 0.31 (09/02/2009 Gwenole Beauchesne) - VC-1/H264 fields change for VDPAU and XvBA backend
 *                                       Application needs to relink with the new library.
 *
 * rev 0.31.1 (03/29/2009) - Data structure for JPEG encode
 *                                      
 * Acknowledgements:
 *  Some concepts borrowed from XvMC and XvImage.
 *  Waldo Bastian (Intel), Matt Sottek (Intel),  Austin Yuan (Intel), and Gwenole Beauchesne (SDS)
 *  contributed to various aspects of the API.
 */

#ifndef _VA_H_
#define _VA_H_

#include <va/va_version.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
Overview 

The VA API is intended to provide an interface between a video decode/encode/display
application (client) and a hardware accelerator (server), to off-load 
video decode/encode/display operations from the host to the hardware accelerator at various 
entry-points.

The basic operation steps are:

- Negotiate a mutually acceptable configuration with the server to lock
  down profile, entrypoints, and other attributes that will not change on 
  a frame-by-frame basis.
- Create a decode context which represents a "virtualized" hardware decode 
  device
- Get and fill decode buffers with picture level, slice level and macroblock 
  level data (depending on entrypoints)
- Pass the decode buffers to the server to decode the current frame

Initialization & Configuration Management 

- Find out supported profiles
- Find out entrypoints for a given profile
- Find out configuration attributes for a given profile/entrypoint pair
- Create a configuration for use by the decoder

*/

typedef void* VADisplay;	/* window system dependent */

typedef int VAStatus;	/* Return status type from functions */
/* Values for the return status */
#define VA_STATUS_SUCCESS			0x00000000
#define VA_STATUS_ERROR_OPERATION_FAILED	0x00000001
#define VA_STATUS_ERROR_ALLOCATION_FAILED	0x00000002
#define VA_STATUS_ERROR_INVALID_DISPLAY		0x00000003
#define VA_STATUS_ERROR_INVALID_CONFIG		0x00000004
#define VA_STATUS_ERROR_INVALID_CONTEXT		0x00000005
#define VA_STATUS_ERROR_INVALID_SURFACE		0x00000006
#define VA_STATUS_ERROR_INVALID_BUFFER		0x00000007
#define VA_STATUS_ERROR_INVALID_IMAGE		0x00000008
#define VA_STATUS_ERROR_INVALID_SUBPICTURE	0x00000009
#define VA_STATUS_ERROR_ATTR_NOT_SUPPORTED	0x0000000a
#define VA_STATUS_ERROR_MAX_NUM_EXCEEDED	0x0000000b
#define VA_STATUS_ERROR_UNSUPPORTED_PROFILE	0x0000000c
#define VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT	0x0000000d
#define VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT	0x0000000e
#define VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE	0x0000000f
#define VA_STATUS_ERROR_SURFACE_BUSY		0x00000010
#define VA_STATUS_ERROR_FLAG_NOT_SUPPORTED      0x00000011
#define VA_STATUS_ERROR_INVALID_PARAMETER	0x00000012
#define VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED 0x00000013
#define VA_STATUS_ERROR_UNIMPLEMENTED           0x00000014
#define VA_STATUS_ERROR_SURFACE_IN_DISPLAYING   0x00000015
#define VA_STATUS_ERROR_INVALID_IMAGE_FORMAT    0x00000016
#define VA_STATUS_ERROR_UNKNOWN			0xFFFFFFFF

/* De-interlacing flags for vaPutSurface() */
#define VA_FRAME_PICTURE        0x00000000 
#define VA_TOP_FIELD            0x00000001
#define VA_BOTTOM_FIELD         0x00000002

/*
 * Clears the drawable with background color.
 * for hardware overlay based implementation this flag
 * can be used to turn off the overlay
 */
#define VA_CLEAR_DRAWABLE       0x00000008 

/* Color space conversion flags for vaPutSurface() */
#define VA_SRC_BT601            0x00000010
#define VA_SRC_BT709            0x00000020

/*
 * Returns a short english description of error_status
 */
const char *vaErrorStr(VAStatus error_status);

/*
 * Initialization:
 * A display must be obtained by calling vaGetDisplay() before calling
 * vaInitialize() and other functions. This connects the API to the 
 * native window system.
 * For X Windows, native_dpy would be from XOpenDisplay()
 */
typedef void* NativeDisplay;	/* window system dependent */

int vaDisplayIsValid(VADisplay dpy);
    
/*
 * Initialize the library 
 */
VAStatus vaInitialize (
    VADisplay dpy,
    int *major_version,	 /* out */
    int *minor_version 	 /* out */
);

/*
 * After this call, all library internal resources will be cleaned up
 */ 
VAStatus vaTerminate (
    VADisplay dpy
);

/*
 * vaQueryVendorString returns a pointer to a zero-terminated string
 * describing some aspects of the VA implemenation on a specific    
 * hardware accelerator. The format of the returned string is vendor
 * specific and at the discretion of the implementer.
 * e.g. for the Intel GMA500 implementation, an example would be:
 * "Intel GMA500 - 2.0.0.32L.0005"
 */
const char *vaQueryVendorString (
    VADisplay dpy
);

typedef int (*VAPrivFunc)();

/*
 * Return a function pointer given a function name in the library.
 * This allows private interfaces into the library
 */ 
VAPrivFunc vaGetLibFunc (
    VADisplay dpy,
    const char *func
);

/* Currently defined profiles */
typedef enum
{
    VAProfileMPEG2Simple		= 0,
    VAProfileMPEG2Main			= 1,
    VAProfileMPEG4Simple		= 2,
    VAProfileMPEG4AdvancedSimple	= 3,
    VAProfileMPEG4Main			= 4,
    VAProfileH264Baseline		= 5,
    VAProfileH264Main			= 6,
    VAProfileH264High			= 7,
    VAProfileVC1Simple			= 8,
    VAProfileVC1Main			= 9,
    VAProfileVC1Advanced		= 10,
    VAProfileH263Baseline		= 11,
    VAProfileJPEGBaseline               = 12
} VAProfile;

/* 
 *  Currently defined entrypoints 
 */
typedef enum
{
    VAEntrypointVLD		= 1,
    VAEntrypointIZZ		= 2,
    VAEntrypointIDCT		= 3,
    VAEntrypointMoComp		= 4,
    VAEntrypointDeblocking	= 5,
    VAEntrypointEncSlice	= 6,	/* slice level encode */
    VAEntrypointEncPicture 	= 7	/* pictuer encode, JPEG, etc */
} VAEntrypoint;

/* Currently defined configuration attribute types */
typedef enum
{
    VAConfigAttribRTFormat		= 0,
    VAConfigAttribSpatialResidual	= 1,
    VAConfigAttribSpatialClipping	= 2,
    VAConfigAttribIntraResidual		= 3,
    VAConfigAttribEncryption		= 4,
    VAConfigAttribRateControl		= 5
} VAConfigAttribType;

/*
 * Configuration attributes
 * If there is more than one value for an attribute, a default
 * value will be assigned to the attribute if the client does not
 * specify the attribute when creating a configuration
 */
typedef struct _VAConfigAttrib {
    VAConfigAttribType type;
    unsigned int value; /* OR'd flags (bits) for this attribute */
} VAConfigAttrib;

/* attribute value for VAConfigAttribRTFormat */
#define VA_RT_FORMAT_YUV420	0x00000001	
#define VA_RT_FORMAT_YUV422	0x00000002
#define VA_RT_FORMAT_YUV444	0x00000004
#define VA_RT_FORMAT_PROTECTED	0x80000000

/* attribute value for VAConfigAttribRateControl */
#define VA_RC_NONE	0x00000001	
#define VA_RC_CBR	0x00000002	
#define VA_RC_VBR	0x00000004	

/*
 * if an attribute is not applicable for a given
 * profile/entrypoint pair, then set the value to the following 
 */
#define VA_ATTRIB_NOT_SUPPORTED 0x80000000

/* Get maximum number of profiles supported by the implementation */
int vaMaxNumProfiles (
    VADisplay dpy
);

/* Get maximum number of entrypoints supported by the implementation */
int vaMaxNumEntrypoints (
    VADisplay dpy
);

/* Get maximum number of attributs supported by the implementation */
int vaMaxNumConfigAttributes (
    VADisplay dpy
);

/* 
 * Query supported profiles 
 * The caller must provide a "profile_list" array that can hold at
 * least vaMaxNumProfile() entries. The actual number of profiles
 * returned in "profile_list" is returned in "num_profile".
 */
VAStatus vaQueryConfigProfiles (
    VADisplay dpy,
    VAProfile *profile_list,	/* out */
    int *num_profiles		/* out */
);

/* 
 * Query supported entrypoints for a given profile 
 * The caller must provide an "entrypoint_list" array that can hold at
 * least vaMaxNumEntrypoints() entries. The actual number of entrypoints 
 * returned in "entrypoint_list" is returned in "num_entrypoints".
 */
VAStatus vaQueryConfigEntrypoints (
    VADisplay dpy,
    VAProfile profile,
    VAEntrypoint *entrypoint_list,	/* out */
    int *num_entrypoints		/* out */
);

/* 
 * Get attributes for a given profile/entrypoint pair 
 * The caller must provide an "attrib_list" with all attributes to be 
 * retrieved.  Upon return, the attributes in "attrib_list" have been 
 * updated with their value.  Unknown attributes or attributes that are 
 * not supported for the given profile/entrypoint pair will have their 
 * value set to VA_ATTRIB_NOT_SUPPORTED
 */
VAStatus vaGetConfigAttributes (
    VADisplay dpy,
    VAProfile profile,
    VAEntrypoint entrypoint,
    VAConfigAttrib *attrib_list, /* in/out */
    int num_attribs
);

/* Generic ID type, can be re-typed for specific implementation */
typedef unsigned int VAGenericID;

typedef VAGenericID VAConfigID;

/* 
 * Create a configuration for the decode pipeline 
 * it passes in the attribute list that specifies the attributes it cares 
 * about, with the rest taking default values.  
 */
VAStatus vaCreateConfig (
    VADisplay dpy,
    VAProfile profile, 
    VAEntrypoint entrypoint, 
    VAConfigAttrib *attrib_list,
    int num_attribs,
    VAConfigID *config_id /* out */
);

/* 
 * Free resources associdated with a given config 
 */
VAStatus vaDestroyConfig (
    VADisplay dpy,
    VAConfigID config_id
);

/* 
 * Query all attributes for a given configuration 
 * The profile of the configuration is returned in "profile"
 * The entrypoint of the configuration is returned in "entrypoint"
 * The caller must provide an "attrib_list" array that can hold at least 
 * vaMaxNumConfigAttributes() entries. The actual number of attributes 
 * returned in "attrib_list" is returned in "num_attribs"
 */
VAStatus vaQueryConfigAttributes (
    VADisplay dpy,
    VAConfigID config_id, 
    VAProfile *profile, 	/* out */
    VAEntrypoint *entrypoint, 	/* out */
    VAConfigAttrib *attrib_list,/* out */
    int *num_attribs 		/* out */
);


/*
 * Contexts and Surfaces
 *
 * Context represents a "virtual" video decode pipeline. Surfaces are render 
 * targets for a given context. The data in the surfaces are not accessible  
 * to the client and the internal data format of the surface is implementatin 
 * specific. 
 *
 * Surfaces will be bound to a context when the context is created. Once
 * a surface is bound to a given context, it can not be used to create  
 * another context. The association is removed when the context is destroyed
 * 
 * Both contexts and surfaces are identified by unique IDs and its
 * implementation specific internals are kept opaque to the clients
 */

typedef VAGenericID VAContextID;

typedef VAGenericID VASurfaceID;

#define VA_INVALID_ID		0xffffffff
#define VA_INVALID_SURFACE	VA_INVALID_ID

/* 
 * vaCreateSurfaces - Create an array of surfaces used for decode and display  
 *  dpy: display
 *  width: surface width
 *  height: surface height
 *  format: VA_RT_FORMAT_YUV420, VA_RT_FORMAT_YUV422 or VA_RT_FORMAT_YUV444
 *  num_surfaces: number of surfaces to be created
 *  surfaces: array of surfaces created upon return
 */
VAStatus vaCreateSurfaces (
    VADisplay dpy,
    int width,
    int height,
    int format,
    int num_surfaces,
    VASurfaceID *surfaces	/* out */
);

    
/*
 * vaDestroySurfaces - Destroy resources associated with surfaces. 
 *  Surfaces can only be destroyed after the context associated has been 
 *  destroyed.  
 *  dpy: display
 *  surfaces: array of surfaces to destroy
 *  num_surfaces: number of surfaces in the array to be destroyed.
 */
VAStatus vaDestroySurfaces (
    VADisplay dpy,
    VASurfaceID *surfaces,
    int num_surfaces
);

#define VA_PROGRESSIVE 0x1
/*
 * vaCreateContext - Create a context
 *  dpy: display
 *  config_id: configuration for the context
 *  picture_width: coded picture width
 *  picture_height: coded picture height
 *  flag: any combination of the following:
 *    VA_PROGRESSIVE (only progressive frame pictures in the sequence when set)
 *  render_targets: render targets (surfaces) tied to the context
 *  num_render_targets: number of render targets in the above array
 *  context: created context id upon return
 */
VAStatus vaCreateContext (
    VADisplay dpy,
    VAConfigID config_id,
    int picture_width,
    int picture_height,
    int flag,
    VASurfaceID *render_targets,
    int num_render_targets,
    VAContextID *context		/* out */
);

/*
 * vaDestroyContext - Destroy a context 
 *  dpy: display
 *  context: context to be destroyed
 */
VAStatus vaDestroyContext (
    VADisplay dpy,
    VAContextID context
);

/*
 * Buffers 
 * Buffers are used to pass various types of data from the
 * client to the server. The server maintains a data store
 * for each buffer created, and the client idenfies a buffer
 * through a unique buffer id assigned by the server.
 */

typedef VAGenericID VABufferID;

typedef enum
{
    VAPictureParameterBufferType	= 0,
    VAIQMatrixBufferType		= 1,
    VABitPlaneBufferType		= 2,
    VASliceGroupMapBufferType		= 3,
    VASliceParameterBufferType		= 4,
    VASliceDataBufferType		= 5,
    VAMacroblockParameterBufferType	= 6,
    VAResidualDataBufferType		= 7,
    VADeblockingParameterBufferType	= 8,
    VAImageBufferType			= 9,
    VAProtectedSliceDataBufferType	= 10,
    VAQMatrixBufferType                 = 11,
/* Following are encode buffer types */
    VAEncCodedBufferType		= 21,
    VAEncSequenceParameterBufferType	= 22,
    VAEncPictureParameterBufferType	= 23,
    VAEncSliceParameterBufferType	= 24,
    VAEncH264VUIBufferType		= 25,
    VAEncH264SEIBufferType		= 26,
} VABufferType;


/* 
 * There will be cases where the bitstream buffer will not have enough room to hold
 * the data for the entire slice, and the following flags will be used in the slice
 * parameter to signal to the server for the possible cases.
 * If a slice parameter buffer and slice data buffer pair is sent to the server with 
 * the slice data partially in the slice data buffer (BEGIN and MIDDLE cases below), 
 * then a slice parameter and data buffer needs to be sent again to complete this slice. 
 */
#define VA_SLICE_DATA_FLAG_ALL		0x00	/* whole slice is in the buffer */
#define VA_SLICE_DATA_FLAG_BEGIN	0x01	/* The beginning of the slice is in the buffer but the end if not */
#define VA_SLICE_DATA_FLAG_MIDDLE	0x02	/* Neither beginning nor end of the slice is in the buffer */
#define VA_SLICE_DATA_FLAG_END		0x04	/* end of the slice is in the buffer */

/* Codec-independent Slice Parameter Buffer base */
typedef struct _VASliceParameterBufferBase
{
    unsigned int slice_data_size;	/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;	/* the offset to the first byte of slice data */
    unsigned int slice_data_flag;	/* see VA_SLICE_DATA_FLAG_XXX definitions */
} VASliceParameterBufferBase;


/****************************
 * JEPG data structure
 ***************************/
typedef struct _VAQMatrixBufferJPEG
{
    int load_lum_quantiser_matrix;
    int load_chroma_quantiser_matrix;
    unsigned char lum_quantiser_matrix[64];
    unsigned char chroma_quantiser_matrix[64];
} VAQMatrixBufferJPEG;

typedef struct _VAEncPictureParameterBufferJPEG
{
    VASurfaceID reconstructed_picture;
    unsigned short picture_width;
    unsigned short picture_height;
    VABufferID coded_buf;
} VAEncPictureParameterBufferJPEG;


/****************************
 * MPEG-2 data structures
 ****************************/
 
/* MPEG-2 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferMPEG2
{
    unsigned short horizontal_size;
    unsigned short vertical_size;
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    /* meanings of the following fields are the same as in the standard */
    int picture_coding_type;
    int f_code; /* pack all four fcode into this */
    union {
        struct {
            unsigned int intra_dc_precision		: 2; 
            unsigned int picture_structure		: 2; 
            unsigned int top_field_first		: 1; 
            unsigned int frame_pred_frame_dct		: 1; 
            unsigned int concealment_motion_vectors	: 1;
            unsigned int q_scale_type			: 1;
            unsigned int intra_vlc_format		: 1;
            unsigned int alternate_scan			: 1;
            unsigned int repeat_first_field		: 1;
            unsigned int progressive_frame		: 1;
            unsigned int is_first_field			: 1; /* indicate whether the current field
                                                              * is the first field for field picture
                                                              */
        } bits;
        unsigned int value;
    } picture_coding_extension;
} VAPictureParameterBufferMPEG2;

/* MPEG-2 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferMPEG2
{
    int load_intra_quantiser_matrix;
    int load_non_intra_quantiser_matrix;
    int load_chroma_intra_quantiser_matrix;
    int load_chroma_non_intra_quantiser_matrix;
    unsigned char intra_quantiser_matrix[64];
    unsigned char non_intra_quantiser_matrix[64];
    unsigned char chroma_intra_quantiser_matrix[64];
    unsigned char chroma_non_intra_quantiser_matrix[64];
} VAIQMatrixBufferMPEG2;

/* MPEG-2 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferMPEG2
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int slice_horizontal_position;
    unsigned int slice_vertical_position;
    int quantiser_scale_code;
    int intra_slice_flag;
} VASliceParameterBufferMPEG2;

/* MPEG-2 Macroblock Parameter Buffer */
typedef struct _VAMacroblockParameterBufferMPEG2
{
    unsigned short macroblock_address;
    /* 
     * macroblock_address (in raster scan order)
     * top-left: 0
     * bottom-right: picture-height-in-mb*picture-width-in-mb - 1
     */
    unsigned char macroblock_type;  /* see definition below */
    union {
        struct {
            unsigned int frame_motion_type		: 2; 
            unsigned int field_motion_type		: 2; 
            unsigned int dct_type			: 1; 
        } bits;
        unsigned int value;
    } macroblock_modes;
    unsigned char motion_vertical_field_select; 
    /* 
     * motion_vertical_field_select:
     * see section 6.3.17.2 in the spec
     * only the lower 4 bits are used
     * bit 0: first vector forward
     * bit 1: first vector backward
     * bit 2: second vector forward
     * bit 3: second vector backward
     */
    short PMV[2][2][2]; /* see Table 7-7 in the spec */
    unsigned short coded_block_pattern;
    /* 
     * The bitplanes for coded_block_pattern are described 
     * in Figure 6.10-12 in the spec
     */
     
    /* Number of skipped macroblocks after this macroblock */
    unsigned short num_skipped_macroblocks;
} VAMacroblockParameterBufferMPEG2;

/* 
 * OR'd flags for macroblock_type (section 6.3.17.1 in the spec)
 */
#define VA_MB_TYPE_MOTION_FORWARD	0x02
#define VA_MB_TYPE_MOTION_BACKWARD	0x04
#define VA_MB_TYPE_MOTION_PATTERN	0x08
#define VA_MB_TYPE_MOTION_INTRA		0x10

/* 
 * MPEG-2 Residual Data Buffer 
 * For each macroblock, there wil be 64 shorts (16-bit) in the 
 * residual data buffer
 */

/****************************
 * MPEG-4 Part 2 data structures
 ****************************/
 
/* MPEG-4 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferMPEG4
{
    unsigned short vop_width;
    unsigned short vop_height;
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    union {
        struct {
            unsigned int short_video_header		: 1; 
            unsigned int chroma_format			: 2; 
            unsigned int interlaced			: 1; 
            unsigned int obmc_disable			: 1; 
            unsigned int sprite_enable			: 2; 
            unsigned int sprite_warping_accuracy	: 2; 
            unsigned int quant_type			: 1; 
            unsigned int quarter_sample			: 1; 
            unsigned int data_partitioned		: 1; 
            unsigned int reversible_vlc			: 1; 
            unsigned int resync_marker_disable		: 1; 
        } bits;
        unsigned int value;
    } vol_fields;
    unsigned char no_of_sprite_warping_points;
    short sprite_trajectory_du[3];
    short sprite_trajectory_dv[3];
    unsigned char quant_precision;
    union {
        struct {
            unsigned int vop_coding_type		: 2; 
            unsigned int backward_reference_vop_coding_type	: 2; 
            unsigned int vop_rounding_type		: 1; 
            unsigned int intra_dc_vlc_thr		: 3; 
            unsigned int top_field_first		: 1; 
            unsigned int alternate_vertical_scan_flag	: 1; 
        } bits;
        unsigned int value;
    } vop_fields;
    unsigned char vop_fcode_forward;
    unsigned char vop_fcode_backward;
    unsigned short vop_time_increment_resolution;
    /* short header related */
    unsigned char num_gobs_in_vop;
    unsigned char num_macroblocks_in_gob;
    /* for direct mode prediction */
    short TRB;
    short TRD;
} VAPictureParameterBufferMPEG4;

/* MPEG-4 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferMPEG4
{
    int load_intra_quant_mat;
    int load_non_intra_quant_mat;
    unsigned char intra_quant_mat[64];
    unsigned char non_intra_quant_mat[64];
} VAIQMatrixBufferMPEG4;

/* MPEG-4 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferMPEG4
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int macroblock_number;
    int quant_scale;
} VASliceParameterBufferMPEG4;

/*
 VC-1 data structures
*/

typedef enum   /* see 7.1.1.32 */
{
    VAMvMode1Mv                        = 0,
    VAMvMode1MvHalfPel                 = 1,
    VAMvMode1MvHalfPelBilinear         = 2,
    VAMvModeMixedMv                    = 3,
    VAMvModeIntensityCompensation      = 4 
} VAMvModeVC1;

/* VC-1 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a picture parameter
 * buffer must be send. Multiple picture parameter buffers may be
 * sent for a single picture. In that case picture parameters will
 * apply to all slice data that follow it until a new picture
 * parameter buffer is sent.
 *
 * Notes:
 *   pic_quantizer_type should be set to the applicable quantizer
 *   type as defined by QUANTIZER (J.1.19) and either
 *   PQUANTIZER (7.1.1.8) or PQINDEX (7.1.1.6)
 */
typedef struct _VAPictureParameterBufferVC1
{
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    /* if out-of-loop post-processing is done on the render
       target, then we need to keep the in-loop decoded 
       picture as a reference picture */
    VASurfaceID inloop_decoded_picture;

    /* sequence layer for AP or meta data for SP and MP */
    union {
        struct {
            unsigned int pulldown	: 1; /* SEQUENCE_LAYER::PULLDOWN */
            unsigned int interlace	: 1; /* SEQUENCE_LAYER::INTERLACE */
            unsigned int tfcntrflag	: 1; /* SEQUENCE_LAYER::TFCNTRFLAG */
            unsigned int finterpflag	: 1; /* SEQUENCE_LAYER::FINTERPFLAG */
            unsigned int psf		: 1; /* SEQUENCE_LAYER::PSF */
            unsigned int multires	: 1; /* METADATA::MULTIRES */
            unsigned int overlap	: 1; /* METADATA::OVERLAP */
            unsigned int syncmarker	: 1; /* METADATA::SYNCMARKER */
            unsigned int rangered	: 1; /* METADATA::RANGERED */
            unsigned int max_b_frames	: 3; /* METADATA::MAXBFRAMES */
        } bits;
        unsigned int value;
    } sequence_fields;

    unsigned short coded_width;		/* ENTRY_POINT_LAYER::CODED_WIDTH */
    unsigned short coded_height;	/* ENTRY_POINT_LAYER::CODED_HEIGHT */
    union {
	struct {
            unsigned int broken_link	: 1; /* ENTRY_POINT_LAYER::BROKEN_LINK */
            unsigned int closed_entry	: 1; /* ENTRY_POINT_LAYER::CLOSED_ENTRY */
            unsigned int panscan_flag	: 1; /* ENTRY_POINT_LAYER::PANSCAN_FLAG */
            unsigned int loopfilter	: 1; /* ENTRY_POINT_LAYER::LOOPFILTER */
	} bits;
	unsigned int value;
    } entrypoint_fields;
    unsigned char conditional_overlap_flag; /* ENTRY_POINT_LAYER::CONDOVER */
    unsigned char fast_uvmc_flag;	/* ENTRY_POINT_LAYER::FASTUVMC */
    union {
        struct {
            unsigned int luma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPY_FLAG */
            unsigned int luma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPY */
            unsigned int chroma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPUV_FLAG */
            unsigned int chroma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPUV */
        } bits;
        unsigned int value;
    } range_mapping_fields;

    unsigned char b_picture_fraction;	/* PICTURE_LAYER::BFRACTION */
    unsigned char cbp_table;		/* PICTURE_LAYER::CBPTAB/ICBPTAB */
    unsigned char mb_mode_table;	/* PICTURE_LAYER::MBMODETAB */
    unsigned char range_reduction_frame;/* PICTURE_LAYER::RANGEREDFRM */
    unsigned char rounding_control;	/* PICTURE_LAYER::RNDCTRL */
    unsigned char post_processing;	/* PICTURE_LAYER::POSTPROC */
    unsigned char picture_resolution_index;	/* PICTURE_LAYER::RESPIC */
    unsigned char luma_scale;		/* PICTURE_LAYER::LUMSCALE */
    unsigned char luma_shift;		/* PICTURE_LAYER::LUMSHIFT */
    union {
        struct {
            unsigned int picture_type		: 3; /* PICTURE_LAYER::PTYPE */
            unsigned int frame_coding_mode	: 3; /* PICTURE_LAYER::FCM */
            unsigned int top_field_first	: 1; /* PICTURE_LAYER::TFF */
            unsigned int is_first_field		: 1; /* set to 1 if it is the first field */
            unsigned int intensity_compensation	: 1; /* PICTURE_LAYER::INTCOMP */
        } bits;
        unsigned int value;
    } picture_fields;
    union {
        struct {
            unsigned int mv_type_mb	: 1; 	/* PICTURE::MVTYPEMB */
            unsigned int direct_mb	: 1; 	/* PICTURE::DIRECTMB */
            unsigned int skip_mb	: 1; 	/* PICTURE::SKIPMB */
            unsigned int field_tx	: 1; 	/* PICTURE::FIELDTX */
            unsigned int forward_mb	: 1;	/* PICTURE::FORWARDMB */
            unsigned int ac_pred	: 1;	/* PICTURE::ACPRED */
            unsigned int overflags	: 1;	/* PICTURE::OVERFLAGS */
        } flags;
        unsigned int value;
    } raw_coding;
    union {
        struct {
            unsigned int bp_mv_type_mb   : 1;    /* PICTURE::MVTYPEMB */
            unsigned int bp_direct_mb    : 1;    /* PICTURE::DIRECTMB */
            unsigned int bp_skip_mb      : 1;    /* PICTURE::SKIPMB */  
            unsigned int bp_field_tx     : 1;    /* PICTURE::FIELDTX */ 
            unsigned int bp_forward_mb   : 1;    /* PICTURE::FORWARDMB */
            unsigned int bp_ac_pred      : 1;    /* PICTURE::ACPRED */   
            unsigned int bp_overflags    : 1;    /* PICTURE::OVERFLAGS */
        } flags;
        unsigned int value;
    } bitplane_present; /* signal what bitplane is being passed via the bitplane buffer */
    union {
        struct {
            unsigned int reference_distance_flag : 1;/* PICTURE_LAYER::REFDIST_FLAG */
            unsigned int reference_distance	: 5;/* PICTURE_LAYER::REFDIST */
            unsigned int num_reference_pictures: 1;/* PICTURE_LAYER::NUMREF */
            unsigned int reference_field_pic_indicator	: 1;/* PICTURE_LAYER::REFFIELD */
        } bits;
        unsigned int value;
    } reference_fields;
    union {
        struct {
            unsigned int mv_mode		: 3; /* PICTURE_LAYER::MVMODE */
            unsigned int mv_mode2		: 3; /* PICTURE_LAYER::MVMODE2 */
            unsigned int mv_table		: 3; /* PICTURE_LAYER::MVTAB/IMVTAB */
            unsigned int two_mv_block_pattern_table: 2; /* PICTURE_LAYER::2MVBPTAB */
            unsigned int four_mv_switch		: 1; /* PICTURE_LAYER::4MVSWITCH */
            unsigned int four_mv_block_pattern_table : 2; /* PICTURE_LAYER::4MVBPTAB */
            unsigned int extended_mv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_MV */
            unsigned int extended_mv_range	: 2; /* PICTURE_LAYER::MVRANGE */
            unsigned int extended_dmv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_DMV */
            unsigned int extended_dmv_range	: 2; /* PICTURE_LAYER::DMVRANGE */
        } bits;
        unsigned int value;
    } mv_fields;
    union {
        struct {
            unsigned int dquant	: 2; 	/* ENTRY_POINT_LAYER::DQUANT */
            unsigned int quantizer     : 2; 	/* ENTRY_POINT_LAYER::QUANTIZER */
            unsigned int half_qp	: 1; 	/* PICTURE_LAYER::HALFQP */
            unsigned int pic_quantizer_scale : 5;/* PICTURE_LAYER::PQUANT */
            unsigned int pic_quantizer_type : 1;/* PICTURE_LAYER::PQUANTIZER */
            unsigned int dq_frame	: 1; 	/* VOPDQUANT::DQUANTFRM */
            unsigned int dq_profile	: 2; 	/* VOPDQUANT::DQPROFILE */
            unsigned int dq_sb_edge	: 2; 	/* VOPDQUANT::DQSBEDGE */
            unsigned int dq_db_edge 	: 2; 	/* VOPDQUANT::DQDBEDGE */
            unsigned int dq_binary_level : 1; 	/* VOPDQUANT::DQBILEVEL */
            unsigned int alt_pic_quantizer : 5;/* VOPDQUANT::ALTPQUANT */
        } bits;
        unsigned int value;
    } pic_quantizer_fields;
    union {
        struct {
            unsigned int variable_sized_transform_flag	: 1;/* ENTRY_POINT_LAYER::VSTRANSFORM */
            unsigned int mb_level_transform_type_flag	: 1;/* PICTURE_LAYER::TTMBF */
            unsigned int frame_level_transform_type	: 2;/* PICTURE_LAYER::TTFRM */
            unsigned int transform_ac_codingset_idx1	: 2;/* PICTURE_LAYER::TRANSACFRM */
            unsigned int transform_ac_codingset_idx2	: 2;/* PICTURE_LAYER::TRANSACFRM2 */
            unsigned int intra_transform_dc_table	: 1;/* PICTURE_LAYER::TRANSDCTAB */
        } bits;
        unsigned int value;
    } transform_fields;
} VAPictureParameterBufferVC1;

/* VC-1 Bitplane Buffer 
There will be at most three bitplanes coded in any picture header. To send 
the bitplane data more efficiently, each byte is divided in two nibbles, with
each nibble carrying three bitplanes for one macroblock.  The following table
shows the bitplane data arrangement within each nibble based on the picture
type.

Picture Type	Bit3		Bit2		Bit1		Bit0
I or BI				OVERFLAGS	ACPRED		FIELDTX
P				MYTYPEMB	SKIPMB		DIRECTMB
B				FORWARDMB	SKIPMB		DIRECTMB

Within each byte, the lower nibble is for the first MB and the upper nibble is 
for the second MB.  E.g. the lower nibble of the first byte in the bitplane
buffer is for Macroblock #1 and the upper nibble of the first byte is for 
Macroblock #2 in the first row.
*/

/* VC-1 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferVC1
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int slice_vertical_position;
} VASliceParameterBufferVC1;

/* VC-1 Slice Data Buffer */
/* 
This is simplely a buffer containing raw bit-stream bytes 
*/

/****************************
 * H.264/AVC data structures
 ****************************/

typedef struct _VAPictureH264
{
    VASurfaceID picture_id;
    unsigned int frame_idx;
    unsigned int flags;
    unsigned int TopFieldOrderCnt;
    unsigned int BottomFieldOrderCnt;
} VAPictureH264;
/* flags in VAPictureH264 could be OR of the following */
#define VA_PICTURE_H264_INVALID			0x00000001
#define VA_PICTURE_H264_TOP_FIELD		0x00000002
#define VA_PICTURE_H264_BOTTOM_FIELD		0x00000004
#define VA_PICTURE_H264_SHORT_TERM_REFERENCE	0x00000008
#define VA_PICTURE_H264_LONG_TERM_REFERENCE	0x00000010

/* H.264 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferH264
{
    VAPictureH264 CurrPic;
    VAPictureH264 ReferenceFrames[16];	/* in DPB */
    unsigned short picture_width_in_mbs_minus1;
    unsigned short picture_height_in_mbs_minus1;
    unsigned char bit_depth_luma_minus8;
    unsigned char bit_depth_chroma_minus8;
    unsigned char num_ref_frames;
    union {
        struct {
            unsigned int chroma_format_idc			: 2; 
            unsigned int residual_colour_transform_flag		: 1; 
            unsigned int gaps_in_frame_num_value_allowed_flag	: 1; 
            unsigned int frame_mbs_only_flag			: 1; 
            unsigned int mb_adaptive_frame_field_flag		: 1; 
            unsigned int direct_8x8_inference_flag		: 1; 
            unsigned int MinLumaBiPredSize8x8			: 1; /* see A.3.3.2 */
            unsigned int log2_max_frame_num_minus4		: 4;
            unsigned int pic_order_cnt_type			: 2;
            unsigned int log2_max_pic_order_cnt_lsb_minus4	: 4;
            unsigned int delta_pic_order_always_zero_flag	: 1;
        } bits;
        unsigned int value;
    } seq_fields;
    unsigned char num_slice_groups_minus1;
    unsigned char slice_group_map_type;
    unsigned short slice_group_change_rate_minus1;
    signed char pic_init_qp_minus26;
    signed char pic_init_qs_minus26;
    signed char chroma_qp_index_offset;
    signed char second_chroma_qp_index_offset;
    union {
        struct {
            unsigned int entropy_coding_mode_flag	: 1;
            unsigned int weighted_pred_flag		: 1;
            unsigned int weighted_bipred_idc		: 2;
            unsigned int transform_8x8_mode_flag	: 1;
            unsigned int field_pic_flag			: 1;
            unsigned int constrained_intra_pred_flag	: 1;
            unsigned int pic_order_present_flag			: 1;
            unsigned int deblocking_filter_control_present_flag : 1;
            unsigned int redundant_pic_cnt_present_flag		: 1;
            unsigned int reference_pic_flag			: 1; /* nal_ref_idc != 0 */
        } bits;
        unsigned int value;
    } pic_fields;
    unsigned short frame_num;
} VAPictureParameterBufferH264;

/* H.264 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferH264
{
    unsigned char ScalingList4x4[6][16];
    unsigned char ScalingList8x8[2][64];
} VAIQMatrixBufferH264;

/* 
 * H.264 Slice Group Map Buffer 
 * When VAPictureParameterBufferH264::num_slice_group_minus1 is not equal to 0,
 * A slice group map buffer should be sent for each picture if required. The buffer
 * is sent only when there is a change in the mapping values.
 * The slice group map buffer map "map units" to slice groups as specified in 
 * section 8.2.2 of the H.264 spec. The buffer will contain one byte for each macroblock 
 * in raster scan order
 */ 

/* H.264 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferH264
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the NAL start code for this slice */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned short slice_data_bit_offset; /* bit offset from NAL start code to the beginning of slice data */
    unsigned short first_mb_in_slice;
    unsigned char slice_type;
    unsigned char direct_spatial_mv_pred_flag;
    unsigned char num_ref_idx_l0_active_minus1;
    unsigned char num_ref_idx_l1_active_minus1;
    unsigned char cabac_init_idc;
    char slice_qp_delta;
    unsigned char disable_deblocking_filter_idc;
    char slice_alpha_c0_offset_div2;
    char slice_beta_offset_div2;
    VAPictureH264 RefPicList0[32];	/* See 8.2.4.2 */
    VAPictureH264 RefPicList1[32];	/* See 8.2.4.2 */
    unsigned char luma_log2_weight_denom;
    unsigned char chroma_log2_weight_denom;
    unsigned char luma_weight_l0_flag;
    short luma_weight_l0[32];
    short luma_offset_l0[32];
    unsigned char chroma_weight_l0_flag;
    short chroma_weight_l0[32][2];
    short chroma_offset_l0[32][2];
    unsigned char luma_weight_l1_flag;
    short luma_weight_l1[32];
    short luma_offset_l1[32];
    unsigned char chroma_weight_l1_flag;
    short chroma_weight_l1[32][2];
    short chroma_offset_l1[32][2];
} VASliceParameterBufferH264;

/****************************
 * Common encode data structures 
 ****************************/
typedef enum
{
    VAEncPictureTypeIntra		= 0,
    VAEncPictureTypePredictive		= 1,
    VAEncPictureTypeBidirectional	= 2,
} VAEncPictureType;

/* Encode Slice Parameter Buffer */
typedef struct _VAEncSliceParameterBuffer
{
    unsigned int start_row_number;	/* starting MB row number for this slice */
    unsigned int slice_height;	/* slice height measured in MB */
    union {
        struct {
            unsigned int is_intra	: 1;
            unsigned int disable_deblocking_filter_idc : 2;
        } bits;
        unsigned int value;
    } slice_flags;
} VAEncSliceParameterBuffer;

/****************************
 * H.264 specific encode data structures
 ****************************/

typedef struct _VAEncSequenceParameterBufferH264
{
    unsigned char seq_parameter_set_id;
    unsigned char level_idc;
    unsigned int intra_period;
    unsigned int intra_idr_period;
    unsigned int picture_width_in_mbs;
    unsigned int picture_height_in_mbs;
    unsigned int bits_per_second;
    unsigned int frame_rate;
    unsigned int initial_qp;
    unsigned int min_qp;
    unsigned int basic_unit_size;
    unsigned char vui_flag;
} VAEncSequenceParameterBufferH264;

typedef struct _VAEncPictureParameterBufferH264
{
    VASurfaceID reference_picture;
    VASurfaceID reconstructed_picture;
    VABufferID coded_buf;
    unsigned short picture_width;
    unsigned short picture_height;
    unsigned char last_picture; /* if set to 1 it indicates the last picture in the sequence */
} VAEncPictureParameterBufferH264;

/****************************
 * H.263 specific encode data structures
 ****************************/

typedef struct _VAEncSequenceParameterBufferH263
{
    unsigned int intra_period;
    unsigned int bits_per_second;
    unsigned int frame_rate;
    unsigned int initial_qp;
    unsigned int min_qp;
} VAEncSequenceParameterBufferH263;

typedef struct _VAEncPictureParameterBufferH263
{
    VASurfaceID reference_picture;
    VASurfaceID reconstructed_picture;
    VABufferID coded_buf;
    unsigned short picture_width;
    unsigned short picture_height;
    VAEncPictureType picture_type;
} VAEncPictureParameterBufferH263;

/****************************
 * MPEG-4 specific encode data structures
 ****************************/

typedef struct _VAEncSequenceParameterBufferMPEG4
{
    unsigned char profile_and_level_indication;
    unsigned int intra_period;
    unsigned int video_object_layer_width;
    unsigned int video_object_layer_height;
    unsigned int vop_time_increment_resolution;
    unsigned int fixed_vop_rate;
    unsigned int fixed_vop_time_increment;
    unsigned int bits_per_second;
    unsigned int frame_rate;
    unsigned int initial_qp;
    unsigned int min_qp;
} VAEncSequenceParameterBufferMPEG4;

typedef struct _VAEncPictureParameterBufferMPEG4
{
    VASurfaceID reference_picture;
    VASurfaceID reconstructed_picture;
    VABufferID coded_buf;
    unsigned short picture_width;
    unsigned short picture_height;
    unsigned int modulo_time_base; /* number of 1s */
    unsigned int vop_time_increment;
    VAEncPictureType picture_type;
} VAEncPictureParameterBufferMPEG4;



/* Buffer functions */

/*
 * Creates a buffer for "num_elements" elements of "size" bytes and 
 * initalize with "data".
 * if "data" is null, then the contents of the buffer data store
 * are undefined.
 * Basically there are two ways to get buffer data to the server side. One is 
 * to call vaCreateBuffer() with a non-null "data", which results the data being
 * copied to the data store on the server side.  A different method that 
 * eliminates this copy is to pass null as "data" when calling vaCreateBuffer(),
 * and then use vaMapBuffer() to map the data store from the server side to the
 * client address space for access.
 *  Note: image buffers are created by the library, not the client. Please see 
 *        vaCreateImage on how image buffers are managed.
 */
VAStatus vaCreateBuffer (
    VADisplay dpy,
    VAContextID context,
    VABufferType type,	/* in */
    unsigned int size,	/* in */
    unsigned int num_elements, /* in */
    void *data,		/* in */
    VABufferID *buf_id	/* out */
);

/*
 * Convey to the server how many valid elements are in the buffer. 
 * e.g. if multiple slice parameters are being held in a single buffer,
 * this will communicate to the server the number of slice parameters
 * that are valid in the buffer.
 */
VAStatus vaBufferSetNumElements (
    VADisplay dpy,
    VABufferID buf_id,	/* in */
    unsigned int num_elements /* in */
);


/*
 * device independent data structure for codedbuffer
 */
typedef  struct _VACodedBufferSegment {
    unsigned int size; /* size of the data buffer in the coded buffer segment, in bytes */
    unsigned int bit_offset;/* bit offset into the data buffer where valid bitstream data begins */
    void *buf; /* pointer to the beginning of the data buffer in the coded buffer segment */
    void *next; /* pointer to the next VACodedBufferSegment */
} VACodedBufferSegment;
     
/*
 * Map data store of the buffer into the client's address space
 * vaCreateBuffer() needs to be called with "data" set to NULL before
 * calling vaMapBuffer()
 *
 * if buffer type is VAEncCodedBufferType, pbuf points to link-list of
 * VACodedBufferSegment, and the list is terminated if "next" is NULL
 */
VAStatus vaMapBuffer (
    VADisplay dpy,
    VABufferID buf_id,	/* in */
    void **pbuf 	/* out */
);

/*
 * After client making changes to a mapped data store, it needs to
 * "Unmap" it to let the server know that the data is ready to be
 * consumed by the server
 */
VAStatus vaUnmapBuffer (
    VADisplay dpy,
    VABufferID buf_id	/* in */
);

/*
 * After this call, the buffer is deleted and this buffer_id is no longer valid
 * Only call this if the buffer is not going to be passed to vaRenderBuffer
 */
VAStatus vaDestroyBuffer (
    VADisplay dpy,
    VABufferID buffer_id
);

/*
Render (Decode) Pictures

A picture represents either a frame or a field.

The Begin/Render/End sequence sends the decode buffers to the server
*/

/*
 * Get ready to decode a picture to a target surface
 */
VAStatus vaBeginPicture (
    VADisplay dpy,
    VAContextID context,
    VASurfaceID render_target
);

/* 
 * Send decode buffers to the server.
 * Buffers are automatically destroyed afterwards
 */
VAStatus vaRenderPicture (
    VADisplay dpy,
    VAContextID context,
    VABufferID *buffers,
    int num_buffers
);

/* 
 * Make the end of rendering for a picture. 
 * The server should start processing all pending operations for this 
 * surface. This call is non-blocking. The client can start another 
 * Begin/Render/End sequence on a different render target.
 */
VAStatus vaEndPicture (
    VADisplay dpy,
    VAContextID context
);

/*

Synchronization 

*/

/* 
 * This function blocks until all pending operations on the render target
 * have been completed.  Upon return it is safe to use the render target for a 
 * different picture. 
 */
VAStatus vaSyncSurface (
    VADisplay dpy,
    VASurfaceID render_target
);

typedef enum
{
    VASurfaceRendering	= 1, /* Rendering in progress */ 
    VASurfaceDisplaying	= 2, /* Displaying in progress (not safe to render into it) */ 
                             /* this status is useful if surface is used as the source */
                             /* of an overlay */
    VASurfaceReady	= 4, /* not being rendered or displayed */
    VASurfaceSkipped	= 8  /* Indicate a skipped frame during encode */
} VASurfaceStatus;

/*
 * Find out any pending ops on the render target 
 */
VAStatus vaQuerySurfaceStatus (
    VADisplay dpy,
    VASurfaceID render_target,
    VASurfaceStatus *status	/* out */
);

/*
 * Images and Subpictures
 * VAImage is used to either get the surface data to client memory, or 
 * to copy image data in client memory to a surface. 
 * Both images, subpictures and surfaces follow the same 2D coordinate system where origin 
 * is at the upper left corner with positive X to the right and positive Y down
 */
#define VA_FOURCC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char) (ch0) | ((unsigned long)(unsigned char) (ch1) << 8) | \
    ((unsigned long)(unsigned char) (ch2) << 16) | ((unsigned long)(unsigned char) (ch3) << 24 ))

/* a few common FourCCs */
#define VA_FOURCC_NV12		0x3231564E
#define VA_FOURCC_AI44		0x34344149
#define VA_FOURCC_RGBA		0x41424752
#define VA_FOURCC_UYVY          0x59565955
#define VA_FOURCC_YUY2          0x32595559
#define VA_FOURCC_AYUV          0x56555941
#define VA_FOURCC_NV11          0x3131564e
#define VA_FOURCC_YV12          0x32315659
#define VA_FOURCC_P208          0x38303250
#define VA_FOURCC_IYUV          0x56555949

/* byte order */
#define VA_LSB_FIRST		1
#define VA_MSB_FIRST		2

typedef struct _VAImageFormat
{
    unsigned int	fourcc;
    unsigned int	byte_order; /* VA_LSB_FIRST, VA_MSB_FIRST */
    unsigned int	bits_per_pixel;
    /* for RGB formats */
    unsigned int	depth; /* significant bits per pixel */
    unsigned int	red_mask;
    unsigned int	green_mask;
    unsigned int	blue_mask;
    unsigned int	alpha_mask;
} VAImageFormat;

typedef VAGenericID VAImageID;

typedef struct _VAImage
{
    VAImageID		image_id; /* uniquely identify this image */
    VAImageFormat	format;
    VABufferID		buf;	/* image data buffer */
    /*
     * Image data will be stored in a buffer of type VAImageBufferType to facilitate
     * data store on the server side for optimal performance. The buffer will be 
     * created by the CreateImage function, and proper storage allocated based on the image
     * size and format. This buffer is managed by the library implementation, and 
     * accessed by the client through the buffer Map/Unmap functions.
     */
    unsigned short	width; 
    unsigned short	height;
    unsigned int	data_size;
    unsigned int	num_planes;	/* can not be greater than 3 */
    /* 
     * An array indicating the scanline pitch in bytes for each plane.
     * Each plane may have a different pitch. Maximum 3 planes for planar formats
     */
    unsigned int	pitches[3];
    /* 
     * An array indicating the byte offset from the beginning of the image data 
     * to the start of each plane.
     */
    unsigned int	offsets[3];

    /* The following fields are only needed for paletted formats */
    int num_palette_entries;   /* set to zero for non-palette images */
    /* 
     * Each component is one byte and entry_bytes indicates the number of components in 
     * each entry (eg. 3 for YUV palette entries). set to zero for non-palette images   
     */
    int entry_bytes; 
    /*
     * An array of ascii characters describing the order of the components within the bytes.
     * Only entry_bytes characters of the string are used.
     */
    char component_order[4];
} VAImage;

/* Get maximum number of image formats supported by the implementation */
int vaMaxNumImageFormats (
    VADisplay dpy
);

/* 
 * Query supported image formats 
 * The caller must provide a "format_list" array that can hold at
 * least vaMaxNumImageFormats() entries. The actual number of formats
 * returned in "format_list" is returned in "num_formats".
 */
VAStatus vaQueryImageFormats (
    VADisplay dpy,
    VAImageFormat *format_list,	/* out */
    int *num_formats		/* out */
);

/* 
 * Create a VAImage structure
 * The width and height fields returned in the VAImage structure may get 
 * enlarged for some YUV formats. Upon return from this function, 
 * image->buf has been created and proper storage allocated by the library. 
 * The client can access the image through the Map/Unmap calls.
 */
VAStatus vaCreateImage (
    VADisplay dpy,
    VAImageFormat *format,
    int width,
    int height,
    VAImage *image	/* out */
);

/*
 * Should call DestroyImage before destroying the surface it is bound to
 */
VAStatus vaDestroyImage (
    VADisplay dpy,
    VAImageID image
);

VAStatus vaSetImagePalette (
    VADisplay dpy,
    VAImageID image,
    /* 
     * pointer to an array holding the palette data.  The size of the array is 
     * num_palette_entries * entry_bytes in size.  The order of the components 
     * in the palette is described by the component_order in VAImage struct    
     */
    unsigned char *palette 
);

/*
 * Retrive surface data into a VAImage
 * Image must be in a format supported by the implementation
 */
VAStatus vaGetImage (
    VADisplay dpy,
    VASurfaceID surface,
    int x,	/* coordinates of the upper left source pixel */
    int y,
    unsigned int width, /* width and height of the region */
    unsigned int height,
    VAImageID image
);

/*
 * Copy data from a VAImage to a surface
 * Image must be in a format supported by the implementation
 * Returns a VA_STATUS_ERROR_SURFACE_BUSY if the surface
 * shouldn't be rendered into when this is called
 */
VAStatus vaPutImage (
    VADisplay dpy,
    VASurfaceID surface,
    VAImageID image,
    int src_x,
    int src_y,
    unsigned int src_width,
    unsigned int src_height,
    int dest_x,
    int dest_y,
    unsigned int dest_width,
    unsigned int dest_height
);

/*
 * Derive an VAImage from an existing surface.
 * This interface will derive a VAImage and corresponding image buffer from
 * an existing VA Surface. The image buffer can then be mapped/unmapped for
 * direct CPU access. This operation is only possible on implementations with
 * direct rendering capabilities and internal surface formats that can be
 * represented with a VAImage. When the operation is not possible this interface
 * will return VA_STATUS_ERROR_OPERATION_FAILED. Clients should then fall back
 * to using vaCreateImage + vaPutImage to accomplish the same task in an
 * indirect manner.
 *
 * Implementations should only return success when the resulting image buffer
 * would be useable with vaMap/Unmap.
 *
 * When directly accessing a surface special care must be taken to insure
 * proper synchronization with the graphics hardware. Clients should call
 * vaQuerySurfaceStatus to insure that a surface is not the target of concurrent
 * rendering or currently being displayed by an overlay.
 *
 * Additionally nothing about the contents of a surface should be assumed
 * following a vaPutSurface. Implementations are free to modify the surface for
 * scaling or subpicture blending within a call to vaPutImage.
 *
 * Calls to vaPutImage or vaGetImage using the same surface from which the image
 * has been derived will return VA_STATUS_ERROR_SURFACE_BUSY. vaPutImage or
 * vaGetImage with other surfaces is supported.
 *
 * An image created with vaDeriveImage should be freed with vaDestroyImage. The
 * image and image buffer structures will be destroyed; however, the underlying
 * surface will remain unchanged until freed with vaDestroySurfaces.
 */
VAStatus vaDeriveImage (
    VADisplay dpy,
    VASurfaceID surface,
    VAImage *image	/* out */
);

/*
 * Subpictures 
 * Subpicture is a special type of image that can be blended 
 * with a surface during vaPutSurface(). Subpicture can be used to render
 * DVD sub-titles or closed captioning text etc.  
 */

typedef VAGenericID VASubpictureID;

/* Get maximum number of subpicture formats supported by the implementation */
int vaMaxNumSubpictureFormats (
    VADisplay dpy
);

/* flags for subpictures */
#define VA_SUBPICTURE_CHROMA_KEYING	0x0001
#define VA_SUBPICTURE_GLOBAL_ALPHA	0x0002
/* 
 * Query supported subpicture formats 
 * The caller must provide a "format_list" array that can hold at
 * least vaMaxNumSubpictureFormats() entries. The flags arrary holds the flag 
 * for each format to indicate additional capabilities for that format. The actual 
 * number of formats returned in "format_list" is returned in "num_formats".
 *  flags: returned value to indicate addtional capabilities
 *         VA_SUBPICTURE_CHROMA_KEYING - supports chroma-keying
 *         VA_SUBPICTURE_GLOBAL_ALPHA - supports global alpha  
 */
VAStatus vaQuerySubpictureFormats (
    VADisplay dpy,
    VAImageFormat *format_list,	/* out */
    unsigned int *flags,	/* out */
    unsigned int *num_formats	/* out */
);

/* 
 * Subpictures are created with an image associated. 
 */
VAStatus vaCreateSubpicture (
    VADisplay dpy,
    VAImageID image,
    VASubpictureID *subpicture	/* out */
);

/*
 * Destroy the subpicture before destroying the image it is assocated to
 */
VAStatus vaDestroySubpicture (
    VADisplay dpy,
    VASubpictureID subpicture
);

/* 
 * Bind an image to the subpicture. This image will now be associated with 
 * the subpicture instead of the one at creation.
 */
VAStatus vaSetSubpictureImage (
    VADisplay dpy,
    VASubpictureID subpicture,
    VAImageID image
);

/*
 * If chromakey is enabled, then the area where the source value falls within
 * the chromakey [min, max] range is transparent
 * The chromakey component format is the following:
 *  For RGB: [0:7] Red [8:15] Blue [16:23] Green   
 *  For YUV: [0:7] V [8:15] U [16:23] Y
 * The chromakey mask can be used to mask out certain components for chromakey
 * comparision
 */
VAStatus vaSetSubpictureChromakey (
    VADisplay dpy,
    VASubpictureID subpicture,
    unsigned int chromakey_min,
    unsigned int chromakey_max,
    unsigned int chromakey_mask
);

/*
 * Global alpha value is between 0 and 1. A value of 1 means fully opaque and 
 * a value of 0 means fully transparent. If per-pixel alpha is also specified then
 * the overall alpha is per-pixel alpha multiplied by the global alpha
 */
VAStatus vaSetSubpictureGlobalAlpha (
    VADisplay dpy,
    VASubpictureID subpicture,
    float global_alpha 
);

/*
 * vaAssociateSubpicture associates the subpicture with target_surfaces.
 * It defines the region mapping between the subpicture and the target  
 * surfaces through source and destination rectangles (with the same width and height).
 * Both will be displayed at the next call to vaPutSurface.  Additional
 * associations before the call to vaPutSurface simply overrides the association.
 */
VAStatus vaAssociateSubpicture (
    VADisplay dpy,
    VASubpictureID subpicture,
    VASurfaceID *target_surfaces,
    int num_surfaces,
    short src_x, /* upper left offset in subpicture */
    short src_y,
    unsigned short src_width,
    unsigned short src_height,
    short dest_x, /* upper left offset in surface */
    short dest_y,
    unsigned short dest_width,
    unsigned short dest_height,
    /*
     * whether to enable chroma-keying or global-alpha
     * see VA_SUBPICTURE_XXX values
     */
    unsigned int flags
);

/*
 * vaDeassociateSubpicture removes the association of the subpicture with target_surfaces.
 */
VAStatus vaDeassociateSubpicture (
    VADisplay dpy,
    VASubpictureID subpicture,
    VASurfaceID *target_surfaces,
    int num_surfaces
);

typedef struct _VARectangle
{
    short x;
    short y;
    unsigned short width;
    unsigned short height;
} VARectangle;

/*
 * Display attributes
 * Display attributes are used to control things such as contrast, hue, saturation,
 * brightness etc. in the rendering process.  The application can query what
 * attributes are supported by the driver, and then set the appropriate attributes
 * before calling vaPutSurface()
 */
/* PowerVR IEP Lite attributes */
typedef enum
{
    VADISPLAYATTRIB_BLE_OFF              = 0x00,
    VADISPLAYATTRIB_BLE_LOW,
    VADISPLAYATTRIB_BLE_MEDIUM,
    VADISPLAYATTRIB_BLE_HIGH,
    VADISPLAYATTRIB_BLE_NONE,
} VADisplayAttribBLEMode;

typedef enum
{ 
    VADISPLAYATTRIB_CSC_FORMAT_YCC_BT601 = 0x00,
    VADISPLAYATTRIB_CSC_FORMAT_YCC_BT709,
    VADISPLAYATTRIB_CSC_FORMAT_YCC_SMPTE_240,
    VADISPLAYATTRIB_CSC_FORMAT_RGB,
    VADISPLAYATTRIB_CSC_FORMAT_NONE,
} VADisplayAttribCSCFormat;

/* attribute value for VADisplayAttribRotation   */
#define VA_ROTATION_NONE        0x00000000
#define VA_ROTATION_90          0x00000001
#define VA_ROTATION_180         0x00000002
#define VA_ROTATION_270         0x00000004

/* attribute value for VADisplayAttribOutOfLoopDeblock */
#define VA_OOL_DEBLOCKING_FALSE 0x00000000
#define VA_OOL_DEBLOCKING_TRUE  0x00000001


/* Currently defined display attribute types */
typedef enum
{
    VADisplayAttribBrightness		= 0,
    VADisplayAttribContrast		= 1,
    VADisplayAttribHue			= 2,
    VADisplayAttribSaturation		= 3,
    /* client can specifiy a background color for the target window */
    VADisplayAttribBackgroundColor      = 4,
    /*
     * this is a gettable only attribute. For some implementations that use the
     * hardware overlay, after PutSurface is called, the surface can not be    
     * re-used until after the subsequent PutSurface call. If this is the case 
     * then the value for this attribute will be set to 1 so that the client   
     * will not attempt to re-use the surface right after returning from a call
     * to PutSurface.
     *
     * Don't use it, use flag VASurfaceDisplaying of vaQuerySurfaceStatus since
     * driver may use overlay or GPU alternatively
     */
    VADisplayAttribDirectSurface       = 5,
    VADisplayAttribRotation            = 6,	
    VADisplayAttribOutofLoopDeblock    = 7,

    /* PowerVR IEP Lite specific attributes */
    VADisplayAttribBLEBlackMode        = 8,
    VADisplayAttribBLEWhiteMode        = 9,
    VADisplayAttribBlueStretch         = 10,
    VADisplayAttribSkinColorCorrection = 11,
    /*
     * For type VADisplayAttribCSCMatrix, "value" field is a pointer to the color
     * conversion matrix. Each element in the matrix is float-point
     */
    VADisplayAttribCSCMatrix           = 12
} VADisplayAttribType;

/* flags for VADisplayAttribute */
#define VA_DISPLAY_ATTRIB_NOT_SUPPORTED	0x0000
#define VA_DISPLAY_ATTRIB_GETTABLE	0x0001
#define VA_DISPLAY_ATTRIB_SETTABLE	0x0002

typedef struct _VADisplayAttribute
{
    VADisplayAttribType type;
    int min_value;
    int max_value;
    int value;	/* used by the set/get attribute functions */
/* flags can be VA_DISPLAY_ATTRIB_GETTABLE or VA_DISPLAY_ATTRIB_SETTABLE or OR'd together */
    unsigned int flags;
} VADisplayAttribute;

/* Get maximum number of display attributs supported by the implementation */
int vaMaxNumDisplayAttributes (
    VADisplay dpy
);

/* 
 * Query display attributes 
 * The caller must provide a "attr_list" array that can hold at
 * least vaMaxNumDisplayAttributes() entries. The actual number of attributes
 * returned in "attr_list" is returned in "num_attributes".
 */
VAStatus vaQueryDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,	/* out */
    int *num_attributes			/* out */
);

/* 
 * Get display attributes 
 * This function returns the current attribute values in "attr_list".
 * Only attributes returned with VA_DISPLAY_ATTRIB_GETTABLE set in the "flags" field
 * from vaQueryDisplayAttributes() can have their values retrieved.  
 */
VAStatus vaGetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,	/* in/out */
    int num_attributes
);

/* 
 * Set display attributes 
 * Only attributes returned with VA_DISPLAY_ATTRIB_SETTABLE set in the "flags" field
 * from vaQueryDisplayAttributes() can be set.  If the attribute is not settable or 
 * the value is out of range, the function returns VA_STATUS_ERROR_ATTR_NOT_SUPPORTED
 */
VAStatus vaSetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,
    int num_attributes
);

#ifdef __cplusplus
}
#endif

#endif /* _VA_H_ */

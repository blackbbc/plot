

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Nov 19 19:11:30 2015
 */
/* Compiler settings for wincodec.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IWICPalette,0x00000040,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapSource,0x00000120,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICFormatConverter,0x00000301,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICPlanarFormatConverter,0xBEBEE9CB,0x83B0,0x4DCC,0x81,0x32,0xB0,0xAA,0xA5,0x5E,0xAC,0x96);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapScaler,0x00000302,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapClipper,0xE4FBCF03,0x223D,0x4e81,0x93,0x33,0xD6,0x35,0x55,0x6D,0xD1,0xB5);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapFlipRotator,0x5009834F,0x2D6A,0x41ce,0x9E,0x1B,0x17,0xC5,0xAF,0xF7,0xA7,0x82);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapLock,0x00000123,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICBitmap,0x00000121,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICColorContext,0x3C613A02,0x34B2,0x44ea,0x9A,0x7C,0x45,0xAE,0xA9,0xC6,0xFD,0x6D);


MIDL_DEFINE_GUID(IID, IID_IWICColorTransform,0xB66F034F,0xD0E2,0x40ab,0xB4,0x36,0x6D,0xE3,0x9E,0x32,0x1A,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICFastMetadataEncoder,0xB84E2C09,0x78C9,0x4AC4,0x8B,0xD3,0x52,0x4A,0xE1,0x66,0x3A,0x2F);


MIDL_DEFINE_GUID(IID, IID_IWICStream,0x135FF860,0x22B7,0x4ddf,0xB0,0xF6,0x21,0x8F,0x4F,0x29,0x9A,0x43);


MIDL_DEFINE_GUID(IID, IID_IWICEnumMetadataItem,0xDC2BB46D,0x3F07,0x481E,0x86,0x25,0x22,0x0C,0x4A,0xED,0xBB,0x33);


MIDL_DEFINE_GUID(IID, IID_IWICMetadataQueryReader,0x30989668,0xE1C9,0x4597,0xB3,0x95,0x45,0x8E,0xED,0xB8,0x08,0xDF);


MIDL_DEFINE_GUID(IID, IID_IWICMetadataQueryWriter,0xA721791A,0x0DEF,0x4d06,0xBD,0x91,0x21,0x18,0xBF,0x1D,0xB1,0x0B);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapEncoder,0x00000103,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapFrameEncode,0x00000105,0xa8f2,0x4877,0xba,0x0a,0xfd,0x2b,0x66,0x45,0xfb,0x94);


MIDL_DEFINE_GUID(IID, IID_IWICPlanarBitmapFrameEncode,0xF928B7B8,0x2221,0x40C1,0xB7,0x2E,0x7E,0x82,0xF1,0x97,0x4D,0x1A);


MIDL_DEFINE_GUID(IID, IID_IWICImageEncoder,0x04C75BF8,0x3CE1,0x473B,0xAC,0xC5,0x3C,0xC4,0xF5,0xE9,0x49,0x99);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapDecoder,0x9EDDE9E7,0x8DEE,0x47ea,0x99,0xDF,0xE6,0xFA,0xF2,0xED,0x44,0xBF);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapSourceTransform,0x3B16811B,0x6A43,0x4ec9,0xB7,0x13,0x3D,0x5A,0x0C,0x13,0xB9,0x40);


MIDL_DEFINE_GUID(IID, IID_IWICPlanarBitmapSourceTransform,0x3AFF9CCE,0xBE95,0x4303,0xB9,0x27,0xE7,0xD1,0x6F,0xF4,0xA6,0x13);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapFrameDecode,0x3B16811B,0x6A43,0x4ec9,0xA8,0x13,0x3D,0x93,0x0C,0x13,0xB9,0x40);


MIDL_DEFINE_GUID(IID, IID_IWICProgressiveLevelControl,0xDAAC296F,0x7AA5,0x4dbf,0x8D,0x15,0x22,0x5C,0x59,0x76,0xF8,0x91);


MIDL_DEFINE_GUID(IID, IID_IWICProgressCallback,0x4776F9CD,0x9517,0x45FA,0xBF,0x24,0xE8,0x9C,0x5E,0xC5,0xC6,0x0C);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapCodecProgressNotification,0x64C1024E,0xC3CF,0x4462,0x80,0x78,0x88,0xC2,0xB1,0x1C,0x46,0xD9);


MIDL_DEFINE_GUID(IID, IID_IWICComponentInfo,0x23BC3F0A,0x698B,0x4357,0x88,0x6B,0xF2,0x4D,0x50,0x67,0x13,0x34);


MIDL_DEFINE_GUID(IID, IID_IWICFormatConverterInfo,0x9F34FB65,0x13F4,0x4f15,0xBC,0x57,0x37,0x26,0xB5,0xE5,0x3D,0x9F);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapCodecInfo,0xE87A44C4,0xB76E,0x4c47,0x8B,0x09,0x29,0x8E,0xB1,0x2A,0x27,0x14);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapEncoderInfo,0x94C9B4EE,0xA09F,0x4f92,0x8A,0x1E,0x4A,0x9B,0xCE,0x7E,0x76,0xFB);


MIDL_DEFINE_GUID(IID, IID_IWICBitmapDecoderInfo,0xD8CD007F,0xD08F,0x4191,0x9B,0xFC,0x23,0x6E,0xA7,0xF0,0xE4,0xB5);


MIDL_DEFINE_GUID(IID, IID_IWICPixelFormatInfo,0xE8EDA601,0x3D48,0x431a,0xAB,0x44,0x69,0x05,0x9B,0xE8,0x8B,0xBE);


MIDL_DEFINE_GUID(IID, IID_IWICPixelFormatInfo2,0xA9DB33A2,0xAF5F,0x43C7,0xB6,0x79,0x74,0xF5,0x98,0x4B,0x5A,0xA4);


MIDL_DEFINE_GUID(IID, IID_IWICImagingFactory,0xec5ec8a9,0xc395,0x4314,0x9c,0x77,0x54,0xd7,0xa9,0x35,0xff,0x70);


MIDL_DEFINE_GUID(IID, IID_IWICImagingFactory2,0x7B816B45,0x1996,0x4476,0xB1,0x32,0xDE,0x9E,0x24,0x7C,0x8A,0xF0);


MIDL_DEFINE_GUID(IID, IID_IWICDevelopRawNotificationCallback,0x95c75a6e,0x3e8c,0x4ec2,0x85,0xa8,0xae,0xbc,0xc5,0x51,0xe5,0x9b);


MIDL_DEFINE_GUID(IID, IID_IWICDevelopRaw,0xfbec5e44,0xf7be,0x4b65,0xb7,0xf8,0xc0,0xc8,0x1f,0xef,0x02,0x6d);


MIDL_DEFINE_GUID(IID, IID_IWICDdsDecoder,0x409cd537,0x8532,0x40cb,0x97,0x74,0xe2,0xfe,0xb2,0xdf,0x4e,0x9c);


MIDL_DEFINE_GUID(IID, IID_IWICDdsEncoder,0x5cacdb4c,0x407e,0x41b3,0xb9,0x36,0xd0,0xf0,0x10,0xcd,0x67,0x32);


MIDL_DEFINE_GUID(IID, IID_IWICDdsFrameDecode,0x3d4c0c61,0x18a4,0x41e4,0xbd,0x80,0x48,0x1a,0x4f,0xc9,0xf4,0x64);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




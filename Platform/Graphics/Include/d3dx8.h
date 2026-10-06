#pragma once
#include "d3dx8math.h"

#define D3DX_DEFAULT 0xffffffffu
#define D3DXASM_DEBUG 1
#define D3DX_FILTER_NONE 1
#define D3DX_FILTER_POINT 2
#define D3DX_FILTER_LINEAR 3
#define D3DX_FILTER_TRIANGLE 4
#define D3DX_FILTER_BOX 5
#define D3DX_FILTER_MIRROR_U (1<<16)
#define D3DX_FILTER_MIRROR_V (2<<16)
#define D3DX_FILTER_MIRROR_W (4<<16)
#define D3DX_FILTER_MIRROR (7<<16)
#define D3DX_FILTER_DITHER (8<<16)

enum D3DXIMAGE_FILEFORMAT {
    D3DXIFF_BMP=0, D3DXIFF_JPG=1, D3DXIFF_TGA=2, D3DXIFF_PNG=3,
    D3DXIFF_DDS=4, D3DXIFF_PPM=5, D3DXIFF_DIB=6, D3DXIFF_FORCE_DWORD=0x7fffffff
};
struct D3DXIMAGE_INFO {
    UINT Width,Height,Depth,MipLevels;
    D3DFORMAT Format;
    D3DRESOURCETYPE ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
};

DEFINE_GUID(IID_ID3DXBuffer,0x932e6a7e,0xc68e,0x45dd,0xa7,0xbf,0x53,0xd1,0x9c,0x86,0xdb,0x1f);
struct ID3DXBuffer : IUnknown {
    virtual void* STDMETHODCALLTYPE GetBufferPointer()=0;
    virtual DWORD STDMETHODCALLTYPE GetBufferSize()=0;
};
using LPD3DXBUFFER = ID3DXBuffer*;

extern "C" {
UINT WINAPI D3DXGetFVFVertexSize(DWORD);
HRESULT WINAPI D3DXGetErrorStringA(HRESULT,char*,UINT);
HRESULT WINAPI D3DXCreateTexture(IDirect3DDevice8*,UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DTexture8**);
HRESULT WINAPI D3DXCreateCubeTexture(IDirect3DDevice8*,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DCubeTexture8**);
HRESULT WINAPI D3DXCreateVolumeTexture(IDirect3DDevice8*,UINT,UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DVolumeTexture8**);
HRESULT WINAPI D3DXCreateTextureFromFileExA(IDirect3DDevice8*,const char*,UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,DWORD,DWORD,D3DCOLOR,D3DXIMAGE_INFO*,D3D8PaletteEntry*,IDirect3DTexture8**);
HRESULT WINAPI D3DXLoadSurfaceFromSurface(IDirect3DSurface8*,const D3D8PaletteEntry*,const RECT*,IDirect3DSurface8*,const D3D8PaletteEntry*,const RECT*,DWORD,D3DCOLOR);
HRESULT WINAPI D3DXLoadSurfaceFromFileA(IDirect3DSurface8*,const D3D8PaletteEntry*,const RECT*,const char*,const RECT*,DWORD,D3DCOLOR,D3DXIMAGE_INFO*);
HRESULT WINAPI D3DXFilterTexture(IDirect3DBaseTexture8*,const D3D8PaletteEntry*,UINT,DWORD);
HRESULT WINAPI D3DXAssembleShader(const void*,UINT,DWORD,ID3DXBuffer**,ID3DXBuffer**,ID3DXBuffer**);
HRESULT WINAPI D3DXAssembleShaderFromFileA(const char*,DWORD,ID3DXBuffer**,ID3DXBuffer**,ID3DXBuffer**);
}
#define D3DXLoadSurfaceFromFile D3DXLoadSurfaceFromFileA
#define D3DXAssembleShaderFromFile D3DXAssembleShaderFromFileA

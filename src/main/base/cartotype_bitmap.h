/*
CARTOTYPE_BITMAP.H
Copyright (C) 2013-2018 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_BITMAP_H__
#define CARTOTYPE_BITMAP_H__

#include <cartotype_color.h>
#include <cartotype_errors.h>
#include <cartotype_stream.h>

namespace CartoType
{

class CBitmap;
class MInputStream;
class MOutputStream;

/** A palette of colors used in a bitmap. */
class CPalette
    {
    public:
    CPalette(const std::vector<TColor>& aColor):
        iColor(aColor)
        {
        }
    const TColor* Color() const { return iColor.data(); }
    size_t ColorCount() const { return iColor.size(); }

    private:
    std::vector<TColor> iColor;
    };

/**
An enumerated type for supported bitmap types.
The number of bits per pixel is held in the low 6 bits.
*/
enum class TBitmapType
    {
    /** A mask for the bits in TBitmapType that represent the number of bits per pixel. */
    KBitsPerPixelMask = 63,
    /**
    The bit in TBitmapType that indicates whether the type is inherently colored,
    which means that its color data is held in the pixel value.
    */
    KColored = 64,
    /**
    The bit in TBitmapType indicating whether the bitmap has a palette.
    If this bit is set, EColored should not also be set.
    */
    KPalette = 128,

    /** One bit per pixel: 1 = foreground color, 0 = background color. */
    A1 = 1,
    /** Eight bits per pixel: 255 = foreground color, 0 = background color. */
    A8 = 8,
    /** 16 bits per pixel, monochrome. */
    A16 = 16,
    /**
    16 bits per pixel, accessed as 16-bit words, not as bytes;
    top 5 bits = red, middle 6 bits = green, low 5 bits = blue.
    */
    RGB16 = KColored | 16,
    /** 24 bits per pixel: first byte blue, second byte green, third byte red. */
    RGB24 = KColored | 24,
    /**
    32 bits per pixel: first byte alpha, second byte blue, second byte green, third byte red.
    The red, green and blue values are premultiplied by the alpha value.
    */
    RGBA32 = KColored | 32,
    /**
    Eight bits per pixel with a 256-entry palette.
    */
    P8 = KPalette | 8
    };

/** A bitmap that does not take ownership of pixel data. */
class TBitmap
    {
    public:
    /** Create a bitmap with a specified type, data, and dimensions. */
    TBitmap(TBitmapType aType,uint8* aData,uint32 aWidth,uint32 aHeight,uint32 aRowBytes,std::shared_ptr<CPalette> aPalette = nullptr):
        iData(aData),
        iPalette(aPalette),
        iWidth(aWidth),
        iHeight(aHeight),
        iRowBytes(aRowBytes),
        iType(aType)
        {
        }
    TBitmap(const CBitmap& aBitmap) = delete;
    TBitmap& operator=(const CBitmap& aBitmap) = delete;

    using TColorFunction = TColor(*)(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    TColorFunction ColorFunction() const;
    CBitmap Copy(int32 aExpansion = 0) const;
    CBitmap Blur(bool aGaussian,TFixed aWidth) const;
    CBitmap Palettize() const;
    CBitmap UnPalettize() const;
    CBitmap Trim(TRect& aBounds) const;
    CBitmap Clip(TRect aClip) const;
    TResult WritePng(MOutputStream& aOutputStream,bool aPalettize) const;
    TResult Write(TDataOutputStream& aOutput) const;

    /** Return the bitmap type, which indicates its depth and whether it is colored. */
    TBitmapType Type() const { return iType; }
    /** Return the bitmap depth: the number of bits used to store each pixel. */
    int32 BitsPerPixel() const { return int32(iType) & int32(TBitmapType::KBitsPerPixelMask); }
    /** Return a constant pointer to the start of the pixel data. */
    const uint8* Data() const { return iData; }
    /** Return a writable pointer to the start of the pixel data. */
    uint8* Data() { return iData; }
    /** Return the palette if any. */
    std::shared_ptr<CPalette> Palette() const { return iPalette; }
    /** Set the palette. */
    void SetPalette(std::shared_ptr<CPalette> aPalette) { iPalette = aPalette; }
    /**
    Return the number of bytes actually used to store the data. This may include padding
    at the ends of rows.
    */
    int32 DataBytes() const { return iHeight * iRowBytes; }
    /** Return the width in pixels. */
    int32 Width() const { return iWidth; }
    /** Return the height in pixels. */
    int32 Height() const { return iHeight; }
    /** Return the number of bytes used to store each horizontal row of pixels. */
    int32 RowBytes() const { return iRowBytes; }
    /** Clear the pixel data to zeroes. */
    void Clear() { memset(iData,0,iHeight * iRowBytes); }
    /** Clear the pixel data to ones (normally white). */
    void ClearToWhite() { memset(iData,0xFF,iHeight * iRowBytes); }

    /** The less-than operator. Assumes that the bitmaps are of the same type. */
    bool operator<(const TBitmap& aOther) const
        {
        if (iWidth < aOther.iWidth)
            return true;
        if (iWidth == aOther.iWidth)
            {
            if (iHeight < aOther.iHeight)
                return true;
            if (iHeight == aOther.iHeight)
                {
                if (memcmp(iData,aOther.iData,DataBytes()) < 0)
                    return true;
                }
            }
        return false;
        }

    /** The equality operator. Assumes that the bitmaps are of the same type. */
    bool operator==(const TBitmap& aOther) const
        {
        return iWidth == aOther.iWidth && iHeight == aOther.iHeight && memcmp(iData,aOther.iData,DataBytes()) == 0;
        }

    protected:
    static TColor Color1BitMono(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color8BitMono(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color8BitPalette(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color16BitMono(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color16BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color24BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color32BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor ColorUnsupported(const TBitmap& aBitmap,uint32 aX,uint32 aY);

    uint8* iData = nullptr;
    std::shared_ptr<CPalette> iPalette;
    uint32 iWidth = 0;
    uint32 iHeight = 0;
    uint32 iRowBytes = 0;
    TBitmapType iType = TBitmapType::A8;
    };

/** A bitmap that owns its data. */
class CBitmap: public TBitmap
    {
    public:
    CBitmap();
    CBitmap(TBitmapType aType,int32 aWidth,int32 aHeight,int32 aRowBytes = 0,std::shared_ptr<CPalette> aPalette = nullptr);
    CBitmap(const CBitmap& aOther);
    CBitmap(CBitmap&& aOther);
    CBitmap(const TBitmap& aOther);
    CBitmap& operator=(const TBitmap& aOther);
    CBitmap& operator=(CBitmap&& aOther);
    static std::unique_ptr<CBitmap> New(TResult& aError,MInputStream& aInputStream);
    static CBitmap Read(TResult& aError,TDataInputStream& aInput);
    
    /** Detach the data, transferring ownership to the caller. */
    std::vector<uint8> DetachData() { iData = nullptr; iWidth = iHeight = iRowBytes = 0; return std::move(iOwnData); }

    private:
    std::vector<uint8> iOwnData;
    };

/** A bitmap and a position to draw it. Used when drawing notices on the map. */
class CPositionedBitmap
    {
    public:
    std::unique_ptr<CBitmap> m_bitmap;
    TPoint m_top_left;
    };

}

#endif

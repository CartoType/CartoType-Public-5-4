/*
CARTOTYPE_COLOR.H
Copyright (C) 2013-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_COLOR_H__
#define CARTOTYPE_COLOR_H__

#include "cartotype_types.h"

namespace CartoType
{

/** Opaque black. */
const uint32 KBlack = 0xFF000000;
/** Opaque dark grey. */
const uint32 KDarkGrey = 0xFF555555;
/** Opaque dark gray. */
const uint32 KDarkGray = 0xFF555555;
/** Opaque dark red. */
const uint32 KDarkRed = 0xFF000080;
/** Opaque dark green. */
const uint32 KDarkGreen = 0xFF008000;
/** Opaque dark yellow. */
const uint32 KDarkYellow = 0xFF008080;
/** Opaque dark blue. */
const uint32 KDarkBlue = 0xFF800000;
/** Opaque dark magenta. */
const uint32 KDarkMagenta = 0xFF800080;
/** Opaque dark cyan. */
const uint32 KDarkCyan = 0xFF808000;
/** Opaque red. */
const uint32 KRed = 0xFF0000FF;
/** Opaque green. */
const uint32 KGreen = 0xFF00FF00;
/** Opaque yellow. */
const uint32 KYellow = 0xFF00FFFF;
/** Opaque blue. */
const uint32 KBlue = 0xFFFF0000;
/** Opaque magenta. */
const uint32 KMagenta = 0xFFFF00FF;
/** Opaque cyan. */
const uint32 KCyan = 0xFFFFFF00;
/** Opaque grey. */
const uint32 KGrey = 0xFFAAAAAA;
/** Opaque gray. */
const uint32 KGray = KGrey;
/** Opaque white. */
const uint32 KWhite = 0xFFFFFFFF;

/** The 'null color' transparent black. */
const uint32 KTransparentBlack = 0x00000000;

/**
A color.

Colors are represented by 32-bit integers containing 8 bits each
of red, green, blue and alpha channel (transparency) data.
*/
class TColor
    {
    public:
    /** Create a color and set it to opaque black. */
    TColor() { }
    /** Create a color from an integer value. */
    TColor(uint32 aValue): iValue(aValue) { }
    /** Create a color from red, green, blue and alpha values. */
    TColor(int32 aRed,int32 aGreen,int32 aBlue,int32 aAlpha = 0xFF):
        iValue(((aAlpha & 0xFF) << 24) | ((aBlue & 0xFF) << 16) | ((aGreen & 0xFF) << 8) | (aRed & 0xFF))
        {
        }
    explicit TColor(const MString& aColor);
    explicit TColor(const CString& aColor);
    /** The equality operator. */
    bool operator==(const TColor& aColor) const { return iValue == aColor.iValue; }
    /** The inequality operator. */
    bool operator!=(const TColor& aColor) const { return iValue != aColor.iValue; }
    /** Return the red component as a value in the range 0...255. */
    int32 Red() const { return (iValue & 0xFF); }
    /** Return the green component as a value in the range 0...255. */
    int32 Green() const { return (iValue & 0xFF00) >> 8; }
    /** Return the blue component as a value in the range 0...255. */
    int32 Blue() const { return (iValue & 0xFF0000) >> 16; }
    /**
    Returns a grey level as a value in the range 0...255, calculated from red, green and blue levels in that range.
    Uses the formula Grey = (Red * 0.299 + Green * 0.587 + Blue * 0.114), derived from the BT.601 standard.
    */
    static int32 Grey(int32 aRed,int32 aGreen,int32 aBlue)
        {
        return (aRed * 77 + aGreen * 150 + aBlue * 29) >> 8;
        }
    /**
    Returns the grey level as a value in the range 0...255.
    Uses the formula Grey = (Red * 0.299 + Green * 0.587 + Blue * 0.114), derived from the BT.601 standard.
    */
    int32 Grey() const { return Grey(Red(),Green(),Blue()); }
    /** A synonym for Grey() */
    int32 Gray() const { return Grey(); }
    /** Return the alpha (transparency) level as a value in the range 0...255: 0 = transparent, 255 = opaque. */
    int32 Alpha() const { return (iValue & 0xFF000000) >> 24; }
    /** Return true if the colour is null. */
    bool IsNull() const { return (iValue & 0xFF000000) == 0; }
    /** Set the alpha (transparency) level to a value in the range 0...255: 0 = transparent, 255 = opaque. */
    void SetAlpha(int32 aAlpha) { iValue = (aAlpha << 24) | (iValue & 0xFFFFFF); }
    void CombineAlpha(int32 aAlpha);
    void PremultiplyAlpha();
    void Blend(const TColor& aOtherColor,int32 aOtherColorAlpha);
    size_t Set(const MString& aColor);

    /**
    The color value, containing, starting with the least significant byte, red, green, blue and alpha levels,
    each stored in eight bits. The color levels represent intensity. The alpha level represents opacity.
    */
    uint32 iValue { KBlack };
    };

}

#endif

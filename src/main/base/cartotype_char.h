/*
CARTOTYPE_CHAR.H
Copyright (C) 2004-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_CHAR_H__
#define CARTOTYPE_CHAR_H__

#include "cartotype_types.h"

namespace CartoType
{

/**
The case of letters and strings in Latin and a few other alphabetic scripts.
*/
enum class TLetterCase
    {
    /** A code used when the letter case is to be left as it is, or is irrelevant. */
    None,
    /** The case of the letters 'a', 'b', c', etc. */
    Lower,
    /** The case of the letters 'A', 'B', 'C, etc. */
    Upper,
    /**
    The case of letters like U+01C8 'Lj' and a few others; also indicates
    that the first letter of each word should be changed to titlecase.
    */
    Title
    };

/**
Flags for the general category of a character as defined in the Unicode Standard
(http://www.unicode.org/versions/Unicode5.0.0/ch04.pdf).
Each constant is a single bit so that masks can be made by combining them.
*/
namespace TCharCategoryFlag
    {
    constexpr uint32 Lu = 1;			// Letter, uppercase
    constexpr uint32 Ll = 2;			// Letter, lowercase
    constexpr uint32 Lt = 4;		    // Letter, titlecase
    constexpr uint32 Lm = 8;			// Letter, modifier
    constexpr uint32 Lo = 0x10;			// Letter, other
    constexpr uint32 Mn = 0x20;			// Mark, nonspacing
    constexpr uint32 Mc = 0x40;			// Mark, spacing combining
    constexpr uint32 Me = 0x80;			// Mark, enclosing
    constexpr uint32 Nd = 0x100;		// Number, decimal digit
    constexpr uint32 Nl = 0x200;		// Number, letter
    constexpr uint32 No = 0x400;		// Number, other
    constexpr uint32 Pc = 0x800;		// Punctuation, connector
    constexpr uint32 Pd = 0x1000;		// Punctuation, dash
    constexpr uint32 Ps = 0x2000;		// Punctuation, open
    constexpr uint32 Pe = 0x4000;		// Punctuation, close
    constexpr uint32 Pi = 0x8000;		// Punctuation, initial quote (may behave like Ps or Pe depending on usage)
    constexpr uint32 Pf = 0x10000;		// Punctuation, final quote (may behave like Ps or Pe depending on usage)
    constexpr uint32 Po = 0x20000;		// Punctuation, other
    constexpr uint32 Sm = 0x40000;		// Symbol, math
    constexpr uint32 Sc = 0x80000;		// Symbol, currency
    constexpr uint32 Sk = 0x100000;		// Symbol, modifier
    constexpr uint32 So = 0x200000;		// Symbol, other
    constexpr uint32 Zs = 0x400000;		// Separator, space
    constexpr uint32 Zl = 0x800000;		// Separator, line
    constexpr uint32 Zp = 0x1000000;	// Separator, paragraph
    constexpr uint32 Cc = 0x2000000;	// Other, control
    constexpr uint32 Cf = 0x4000000;	// Other, format
    constexpr uint32 Cs = 0x8000000;	// Other, surrogate
    constexpr uint32 Co = 0x10000000;	// Other, private use
    constexpr uint32 Cn = 0x20000000;	// Other, not assigned (including noncharacters)

    /**
    A flag to indicate a character outside the categories Lu and Lt that has a lower-case variant.
    It is used internally.
    */
    constexpr uint32 UpperCase = 0x40000000;
    };

/**
The general category of a character as defined in the Unicode Standard
(http://www.unicode.org/versions/Unicode5.0.0/ch04.pdf).
Each constant is a single bit so that masks can be made by combining them.
*/
enum class TCharCategory
    {
    Lu = TCharCategoryFlag::Lu,
    Ll = TCharCategoryFlag::Ll,
    Lt = TCharCategoryFlag::Lt,
    Lm = TCharCategoryFlag::Lm,
    Lo = TCharCategoryFlag::Lo,
    Mn = TCharCategoryFlag::Mn,
    Mc = TCharCategoryFlag::Mc,
    Me = TCharCategoryFlag::Me,
    Nd = TCharCategoryFlag::Nd,
    Nl = TCharCategoryFlag::Nl,
    No = TCharCategoryFlag::No,
    Pc = TCharCategoryFlag::Pc,
    Pd = TCharCategoryFlag::Pd,
    Ps = TCharCategoryFlag::Ps,
    Pe = TCharCategoryFlag::Pe,
    Pi = TCharCategoryFlag::Pi,
    Pf = TCharCategoryFlag::Pf,
    Po = TCharCategoryFlag::Po,
    Sm = TCharCategoryFlag::Sm,
    Sc = TCharCategoryFlag::Sc,
    Sk = TCharCategoryFlag::Sk,
    So = TCharCategoryFlag::So,
    Zs = TCharCategoryFlag::Zs,
    Zl = TCharCategoryFlag::Zl,
    Zp = TCharCategoryFlag::Zp,
    Cc = TCharCategoryFlag::Cc,
    Cf = TCharCategoryFlag::Cf,
    Cs = TCharCategoryFlag::Cs,
    Co = TCharCategoryFlag::Co,
    Cn = TCharCategoryFlag::Cn
    };

namespace TBidiTypeFlag
    {
    constexpr uint32 L = 1;
    constexpr uint32 LRE = 2;
    constexpr uint32 LRO = 4;
    constexpr uint32 R = 8;
    constexpr uint32 AL = 0x10;
    constexpr uint32 RLE = 0x20;
    constexpr uint32 RLO = 0x40;
    constexpr uint32 PDF = 0x80;
    constexpr uint32 EN = 0x100;
    constexpr uint32 ES = 0x200;
    constexpr uint32 ET = 0x400;
    constexpr uint32 AN = 0x800;
    constexpr uint32 CS = 0x1000;
    constexpr uint32 NSM = 0x2000;
    constexpr uint32 BN = 0x4000;
    constexpr uint32 B = 0x8000;
    constexpr uint32 S = 0x10000;
    constexpr uint32 WS = 0x20000;
    constexpr uint32 ON = 0x40000;

    constexpr uint32 StrongLeftToRight = L | LRE | LRO;
    constexpr uint32 StrongRightToLeft = R | AL | RLE | RLO;
    constexpr uint32 Strong = StrongLeftToRight | StrongRightToLeft;
    };

/** The bidirectional type of a character as used in the Unicode Bidirectional Algorithm. */
enum class TBidiType
    {
    L   = TBidiTypeFlag::L,
    LRE = TBidiTypeFlag::LRE,
    LRO = TBidiTypeFlag::LRO,
    R   = TBidiTypeFlag::R,
    AL  = TBidiTypeFlag::AL,
    RLE = TBidiTypeFlag::RLE,
    RLO = TBidiTypeFlag::RLO,
    PDF = TBidiTypeFlag::PDF,
    EN  = TBidiTypeFlag::EN,
    ES  = TBidiTypeFlag::ES,
    ET  = TBidiTypeFlag::ET,
    AN  = TBidiTypeFlag::AN,
    CS  = TBidiTypeFlag::CS,
    NSM = TBidiTypeFlag::NSM,
    BN  = TBidiTypeFlag::BN,
    B   = TBidiTypeFlag::B,
    S   = TBidiTypeFlag::S,
    WS  = TBidiTypeFlag::WS,
    ON  = TBidiTypeFlag::ON
    };

/**
A class to provide Unicode character properties.
A TChar object holds a single character encoded using its Unicode
code point. Surrogates have no meaning in this context. All signed 32-bit
values are valid but not all are defined as characters. Non-character
values give consistent default values: for example, the uppercase version
of a non-character value is the same value.
*/
class TChar
    {
    public:
    /**
    A case variant can be no longer than this number of UTF32 characters.
    For example, the upper-case variant of U+00DF is 'SS' and is
    two characters long.
    */
    static constexpr int KMaxCaseVariantLength = 2;

    /** The start of a range of private use characters to select fonts. */
    static constexpr uint32 KFirstFontSelector = 0xF000;
    /** A private use character to select the default font. */
    static constexpr uint32 KDefaultFontSelector = 0xF0FF;
    /** The end of a range of private use characters to select fonts. */
    static constexpr uint32 KLastFontSelector = KDefaultFontSelector;
    /** The start of a range of private use characters to select box styles (background and outline). */
    static constexpr uint32 KFirstBoxStyleSelector = 0xF100;
    /** The end of a range of private use characters to select box styles (background and outline). */
    static constexpr uint32 KLastBoxStyleSelector = 0xF1FE;
    /** A character to mark the end of a box. */
    static constexpr uint32 KEndOfBox = 0xF1FF;
    /** A character to set the baseline of a box to the baseline of the current line. */
    static constexpr uint32 KBaselineSelector = 0xF200;

    /** Create a TChar with the character code 0. */
    TChar() { }

    /** Construct a TChar from a Unicode code point (without checking its validity as such). */
    TChar(int32 aCode): iCode(aCode) { }

    void GetLowerCase(int32* aText,int32& aTextLength) const;
    void GetTitleCase(int32* aText,int32& aTextLength) const;
    void GetUpperCase(int32* aText,int32& aTextLength) const;
    TCharCategory Category() const;
    TBidiType BidiType() const;
    int32 Mirrored() const;
    int32 AccentStripped() const;
    bool IsAlphanumeric() const;

    /**
    Return true if a character code is in a word. Words are defined as sequences
    of letters and combining marks.
    */
    static bool InWord(int32 aCode)
        {
        if (aCode < 'A' && aCode != '\'' && aCode != ':')
            return false;
        if (aCode <= 'Z')
            return true;
        if (aCode < 'a')
            return false;
        if (aCode <= 'z')
            return true;
        if (aCode <= 0xBF)
            return false;
        if (aCode <= 0x17F)
            return true;
        return aCode == 0x2019 || // right single quote
            (uint32(TChar(aCode).Category()) & (TCharCategoryFlag::Lu | TCharCategoryFlag::Ll | TCharCategoryFlag::Lt | TCharCategoryFlag::Lm |
                                                TCharCategoryFlag::Lo | TCharCategoryFlag::Mn | TCharCategoryFlag::Mc | TCharCategoryFlag::Me)) != 0;
        }

    /** The Unicode code point of the character. */
    int32 iCode { 0 };
    };

}

#endif

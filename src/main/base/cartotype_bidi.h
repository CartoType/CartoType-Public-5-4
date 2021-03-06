/*
CARTOTYPE_BIDI.H
Copyright (C) 2008-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_BIDI_H__
#define CARTOTYPE_BIDI_H__

#include "cartotype_char.h"
#include "cartotype_errors.h"

namespace CartoType
{

/** The override status during bidirectional reordering. */
enum class TBidiOverride
    {
    Neutral,
    LeftToRight,
    RightToLeft
    };

/** A run of text in a certain birectional type. */
class TBidiRun
    {
    public:
    int32 iLength { 0 };
    TBidiType iType { TBidiType::ON };
    int32 iLevel { 0 };
    };

/** The paragraph direction used during bidirectional reordering. */
enum class TBidiParDir
    {
    /**
    Set the paragraph direction from the first strongly directional character.
    If there are none, use left-to-right.
    */
    LeftToRightPreferred,
    /**
    Set the paragraph direction from the first strongly directional character.
    If there are none, use right-to-left.
    */
    RightToLeftPreferred,
    /** Set the paragraph direction to left-to-right whatever its content. */
    LeftToRightForced,
    /** Set the paragraph direction to right-to-left whatever its content. */
    RightToLeftForced
    };

/**
An engine for doing bidirectional reordering and contextual shaping, and storing state
when reordering and shaping a series of lines.
*/
class CBidiEngine
    {
    public:
    CBidiEngine();

    void Order(uint16* aText,size_t aLength,size_t& aNewLength,TBidiParDir aParDir,bool aParStart,int32* aUserData = nullptr);
    static int32 ShapeArabic(uint16* aText,size_t aLength);
    
    /** Return the resolved direction; meaningful only after Order has been called at least once. */
    bool RightToLeft() { return (iStack[iStackLevel].iLevel & 1) != 0; }

    private:
    void operator=(const CBidiEngine&) = delete;
    void operator=(CBidiEngine&&) = delete;
    CBidiEngine(const CBidiEngine&) = delete;
    CBidiEngine(CBidiEngine&&) = delete;

    void Push(bool aRightToLeft,TBidiOverride aOverride);
    static void ShapeAndMirror(uint16* aText,size_t& aNewLength,bool aHaveArabic,int32* aUserData,TBidiRun* aRunStart,TBidiRun* aRunEnd);
    
    static constexpr int KRunArraySize = 8;
    static constexpr int KLevels = 63; // legal levels are 0...62 

    class TStackItem
        {
        public:
        uint8 iLevel;
        uint8 iOverride;
        };

    TStackItem iStack[KLevels];
    int32 iStackLevel { 0 };
    };

}

#endif

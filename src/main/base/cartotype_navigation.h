/*
CARTOTYPE_NAVIGATION.H
Copyright (C) 2013-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_NAVIGATION_H__
#define CARTOTYPE_NAVIGATION_H__

#include <cartotype_path.h>
#include <cartotype_stream.h>
#include <cartotype_road_type.h>
#include <cartotype_map_object.h>

namespace CartoType
{

class CFramework;
class CMap;
class CMapDataBase;
class CMapObject;
class TRouteProfile;
class CProjection;

namespace Router
    {
    class TJunctionInfo;
    }

/** Types of router; used when selecting a router type. */
enum class TRouterType
    {
    /** This router type causes the default router to be selected: the one for which serialized data is available in the map file, or, failing that, StandardAStar. */
    Default,
    /** The A* router, which gives fast performance but takes a lot of memory and cannot create a route going twice through the same junction. */
    StandardAStar,
    /** A version of the A* router with road nodes and turn arcs; slower than StandardAStar, and uses more memory, but can generate routes going twice through the same junction, for more flexible routing. */
    TurnExpandedAStar,
    /**
    The contraction hierarchy router is intended where less RAM is available: for example with large maps on mobile devices.
    It gives the same routes as StandardAStar, but is a little slower and does not support custom route profiles; the route profile is decided at the time of creating the CTM1 file.
    */
    StandardContractionHierarchy
    };

/**
Turn types on a route. Turns at junctions are classified
by dividing the full circle into 45-degree segments,
except for sharp turns left and right, which encompass everything
up to an actual U-turn, and left and right forks, for which there
are special rules to avoid classifying them as ahead.

The actual turn angle is also given in the TTurn class.
*/
enum class TTurnType
    {
    /** No turn exists or is needed. This turn type is used at the start of a route. */
    None,
    /**
    A turn of less than 15 degrees left or right, unless this is a fork with two choices, in which
    case the turn will be bear right or bear left.
    */
    Ahead,
    /**
    A turn between 15 degrees and 45 degrees right
    or a turn through a smaller angle which is the rightmost of a fork with two choices.
    */
    BearRight,
    /** A turn between 45 degrees and 120 degrees right. */
    Right,
    /** A turn between 120 degrees and 180 degrees right. */
    SharpRight,
    /** This turn type is use for U-turns: turns back along the same road. */
    Around,
    /** A turn between 120 degrees and 180 degrees left. */
    SharpLeft,
    /** A turn between 45 degrees and 120 degrees left. */
    Left,
    /**
    A turn between 15 degrees and 45 degrees left.
    or a turn through a smaller angle which is the leftmost of a fork with two choices.
    */
    BearLeft
    };

/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnNone = TTurnType::None;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnAhead = TTurnType::Ahead;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnBearRight = TTurnType::BearRight;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnRight = TTurnType::Right;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnSharpRight = TTurnType::SharpRight;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnAround = TTurnType::Around;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnSharpLeft = TTurnType::SharpLeft;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnLeft = TTurnType::Left;
/** A TTurnType value for backward compatibility. @deprecated */
constexpr TTurnType ETurnBearLeft = TTurnType::BearLeft;

/**
Roundabout states; turns involving roundabouts are marked as such so that
exit numbers can be counted
*/
enum class TRoundaboutState
    {
    /** This junction does not involve a roundabout. */
    None,
    /** This junction enters a roundabout. */
    Enter,
    /** This junction continues around a roundabout. */
    Continue,
    /** This junction exits a roundabout. */
    Exit
    };

/** A TRoundaboutState value for backward compatibility. @deprecated */
constexpr TRoundaboutState ERoundaboutStateNone = TRoundaboutState::None;
/** A TRoundaboutState value for backward compatibility. @deprecated */
constexpr TRoundaboutState ERoundaboutStateEnter = TRoundaboutState::Enter;
/** A TRoundaboutState value for backward compatibility. @deprecated */
constexpr TRoundaboutState ERoundaboutStateContinue = TRoundaboutState::Continue;
/** A TRoundaboutState value for backward compatibility. @deprecated */
constexpr TRoundaboutState ERoundaboutStateExit = TRoundaboutState::Exit;

/** A turn: a choice of route through a node. */
class TTurn
    {
    public:
    void Clear()
        {
        *this = TTurn();
        }

    void SetTurn(double aTurnAngle,int32 aChoices,int32 aLeftAlternatives,int32 aRightAlternatives,bool aIsFork,bool aTurnOff)
        {
        SetTurn(aTurnAngle);
        iChoices = aChoices;
        iLeftAlternatives = aLeftAlternatives;
        iRightAlternatives = aRightAlternatives;
        iIsFork = aIsFork;
        iTurnOff = aTurnOff;
        if (iTurnType == TTurnType::Ahead && (iTurnOff || (iIsFork && iChoices == 2)))
            iTurnType = iLeftAlternatives ? TTurnType::BearRight : TTurnType::BearLeft;
        }

    void SetTurn(double aTurnAngle)
        {
        iTurnAngle = aTurnAngle;
        if (iTurnAngle > 120)
            iTurnType = TTurnType::SharpRight;
        else if (iTurnAngle > 45)
            iTurnType = TTurnType::Right;
        else if (iTurnAngle > 15)
            iTurnType = TTurnType::BearRight;
        else if (iTurnAngle > -15)
            iTurnType = TTurnType::Ahead;
        else if (iTurnAngle > -45)
            iTurnType = TTurnType::BearLeft;
        else if (iTurnAngle > -120)
            iTurnType = TTurnType::Left;
        else
            iTurnType = TTurnType::SharpLeft;
        }

    TResult WriteAsXml(MOutputStream& aOutput) const;

    /** The turn type: ahead, left, right, etc. */
    TTurnType iTurnType = TTurnType::None;
    /** True if this turn is a continuation of the current road and no notification is needed. */
    bool iContinue = true;
    /** The roundabout state. */
    TRoundaboutState iRoundaboutState = TRoundaboutState::None;
    /** The turn angle: 0 = straight ahead; negative = left, positive = right. */
    double iTurnAngle = 0;
    /** The junction to take, counting the current junction as 0, if this junction is part of a roundabout. */
    int32 iExitNumber = 0;
    /** The number of choices at this turning if known; 0 if not known. */
    int32 iChoices = 0;
    /** The number of choices to the left of the turn actually taken; if iChoices is zero, this data is not known. */
    int32 iLeftAlternatives = 0;
    /** The number of choices to the right of the turn actually taken; if iChoices is zero, this data is not known. */
    int32 iRightAlternatives = 0;
    /** True if this turn is a fork: that is, there is an alternative within 22.5 degrees of the turn actually taken. */
    bool iIsFork = false;
    /** True if this choice is a turn off, defined as a turn on to a lower-status road. */
    bool iTurnOff = false;
    /** The name of the junction. */
    CString iJunctionName;
    /** The reference code of the junction. */
    CString iJunctionRef;
    };

/** Information about a route segment. */
class CRouteSegment
    {
    public:
    CRouteSegment():
        iRoadType(TRoadType::UnknownMajor),
        iMaxSpeed(0),
        iDistance(0),
        iTime(0),
        iTurnTime(0),
        iSection(0),
        iRestricted(false)
        {
        }
    TResult WriteAsXml(MOutputStream& aOutput,const CProjection& aProjection) const;
    std::unique_ptr<CRouteSegment> Copy() const;

    /** The road type of the object of which this segment is a part. */
    TRoadType iRoadType;
    /**
    The maximum legal speed in kilometres per hour. A value of zero means there is no known speed limit,
    or the standard speed limit for the type of road applies.
    */
    double iMaxSpeed;
    /** The standard name of the object of which this segment is a part. */
    CString iName;
    /** The road reference of the object of which this segment is a part. */
    CString iRef;
    /** The distance in metres. */
    double iDistance;
    /** The estimated time in seconds, including iTurnTime, taken to traverse the segment. */
    double iTime;
    /** The estimated time in seconds taken to navigate the junction at the start of the segment. */
    double iTurnTime;
    /** The path of this segment in map units. */
    CContour iPath;
    /**
    The section number.
    Routes are divided into sections between waypoints. A simple route has one section,
    a route with an intermediate waypoint has two sections, and so on. Sections
    are numbered from zero upwards.
    */
    int32 iSection;
    /** Information about how to navigate the junction at the start of this segment. */
    TTurn iTurn;
    /** True if this segment is restricted: for example, a private access road. */
    bool iRestricted;
    };

/** Information about the nearest road (in fact, the nearest routable map object) to a certain point. */
class TNearestRoadInfo
    {
    public:
    TNearestRoadInfo():
        iRoadType(TRoadType::UnknownMajor),
        iMaxSpeed(0),
        iDistance(0),
        iHeadingInDegrees(0),
        iOneWay(false)
        {
        }
    void Clear()
        {
        iRoadType = TRoadType::UnknownMajor;
        iMaxSpeed = 0;
        iName.Clear();
        iRef.Clear();
        iNearestPoint = TPoint();
        iDistance = 0;
        iHeadingInDegrees = 0;
        iHeadingVector = TPointFP();
        iPath.Clear();
        iOneWay = false;
        }

    /** The road type. */
    TRoadType iRoadType;
    /**
    The maximum legal speed in kilometres per hour. A value of zero means there is no known speed limit,
    or the standard speed limit for the type of road applies.
    */
    double iMaxSpeed;
    /** The standard name of the road. */
    CString iName;
    /** The road reference of the road. */
    CString iRef;
    /** The nearest point on the road, in map coordinates, to the chosen point. */
    TPoint iNearestPoint;
    /** The distance from the chosen point to iNearestPoint in meters. */
    double iDistance;
    /** The heading of the nearest segment in degrees. */
    double iHeadingInDegrees;
    /** The heading of the nearest segment as a unit vector. */
    TPointFP iHeadingVector;
    /** The road's geometry in map coordinates. The road is guaranteed to be traversible in the direction of iPath. */
    CContour iPath;
    /** True if the road is one-way. */
    bool iOneWay;
    };

/** Information about the nearest route segment to a point. */
class TNearestSegmentInfo
    {
    public:
    TNearestSegmentInfo():
        iSegmentIndex(-1),
        iLineIndex(0),
        iDistanceToRoute(0),
        iDistanceAlongRoute(0),
        iDistanceAlongSegment(0),
        iTimeAlongRoute(0),
        iTimeAlongSegment(0),
        iHeading(0)
        {
        }

    /** The index of the segment in the CRoute object, or -1 if there were no segments. */
    int32 iSegmentIndex;
    /**
    The index of the line within the segment's path:
    line N goes from point N to point N + 1.
    */
    int32 iLineIndex;
    /** The point in the segment's path nearest to the other point, in map coordinates. */
    TPointFP iNearestPoint;
    /** The distance from the other point to iNearestPoint in meters. */
    double iDistanceToRoute;
    /** The distance of the nearest point along the route in meters. */
    double iDistanceAlongRoute;
    /** The distance within the current segment in meters. */
    double iDistanceAlongSegment;
    /** The estimated time of the nearest point, along the route, in seconds. */
    double iTimeAlongRoute;
    /** The estimated time within the current segment, in seconds. */
    double iTimeAlongSegment;
    /** The heading of the nearest line as a map angle taken anti-clockwise from rightwards. */
    double iHeading;
    };

/**
Information about a path from the start or
end of the route to the nearest non-trivial junction.
Used when exporting traffic information.
*/
class CPathToJunction
    {
    public:
    CPathToJunction(): iStartRoadType(TRoadType::UnknownMajor), iEndRoadType(TRoadType::UnknownMajor), iDistance(0) { }
    void Clear()
        {
        iPath.Clear();
        iStartRoadType = iEndRoadType = TRoadType::UnknownMajor;
        iDistance = 0;
        }
    void Reverse()
        {
        iPath.Reverse();
        TRoadType temp(iStartRoadType);
        iStartRoadType = iEndRoadType;
        iEndRoadType = temp;
        }
    void Set(const CPathToJunction& aOther)
        {
        iStartRoadType = aOther.iStartRoadType;
        iEndRoadType = aOther.iEndRoadType;
        iDistance = aOther.iDistance;
        iPath.Clear();
        iPath.AppendPoints(aOther.iPath.Point(),aOther.iPath.Points());
        }

    /** The path between the junction and the start or end of the route. */
    CContour iPath;
    /** The road type at the start of the path. */
    TRoadType iStartRoadType;
    /** The road type at the start of the path. */
    TRoadType iEndRoadType;
    /** The length of the path in meters. */
    double iDistance;
    };

// Bit flags and masks describing an arc, for use in TRouteProfile.
constexpr uint32 KArcRoadTypeMask = 0x1F;                // 5 bits for the road type
constexpr uint32 KArcGradientMask = 0xE0;                // 3 bits for the average road gradient
constexpr uint32 KArcGradientDirectionFlag = 0x80;
constexpr uint32 KArcGradientShift = 5;
constexpr uint32 KArcGradientCount = 8;
constexpr uint32 KArcRoadDirectionMask = 0x300;
constexpr uint32 KArcDriveOnRightRoadDirection = 0;
constexpr uint32 KArcDriveOnLeftRoadDirection = 0x300;
constexpr uint32 KArcOneWayForwardRoadDirection = 0x100;
constexpr uint32 KArcOneWayBackwardRoadDirection = 0x200;
constexpr uint32 KArcRoundaboutFlag = 0x400;
constexpr uint32 KArcTollFlag = 0x800;
constexpr uint32 KArcSpeedLimitMask = 0xFF000;           // 8 bits for the speed limit in kph if known
constexpr uint32 KArcSpeedLimitShift = 12;

/** A mask for access restrictions not including the 'other access' flag. */
constexpr uint32 KArcAccessMask = 0x7FF00000;            
/** This arc goes the wrong way along a one-way road but it can be used by pedestrians and emergency vehicles. */
constexpr uint32 KArcWrongWayFlag = 0x00100000;
/** No access for bicycles. */
constexpr uint32 KArcBicycleAccessFlag = 0x00200000;
/** No access for motorcycles. */
constexpr uint32 KArcMotorCycleAccessFlag = 0x00400000;
/** No access for cars. */
constexpr uint32 KArcCarAccessFlag = 0x00800000;
/** No access for high occupancy vehicles. */
constexpr uint32 KArcHighOccupancyAccessFlag = 0x01000000;
/** No access for light goods vehicles. */
constexpr uint32 KArcLightGoodsAccessFlag = 0x02000000;
/** No access for heavy goods vehicles. */
constexpr uint32 KArcHeavyGoodsAccessFlag = 0x04000000;
/** No access for buses. */
constexpr uint32 KArcBusAccessFlag = 0x08000000;
/** No access for taxis. */
constexpr uint32 KArcTaxiAccessFlag = 0x10000000;
/** No access for pedestrians. */
constexpr uint32 KArcPedestrianAccessFlag = 0x20000000;
/** No access for emergency vehicles. */
constexpr uint32 KArcEmergencyAccessFlag = 0x40000000;
/** Other access restrictions exist such as weight, length, width, height or for vehicles carrying hazardous materials. */
constexpr uint32 KArcOtherAccessFlag = 0x80000000;
/** Bicycle access only. */
constexpr uint32 KArcBicycleAccessOnly = KArcAccessMask & ~(KArcWrongWayFlag | KArcBicycleAccessFlag);
/** Access flags affect all motor vehicles. */
constexpr uint32 KArcAllMotorVehicles = KArcAccessMask & ~(KArcWrongWayFlag | KArcPedestrianAccessFlag | KArcBicycleAccessFlag);
/** Bicycle and pedestrian access only; synonym of KArcAllMotorVehicles. */
constexpr uint32 KArcBicycleAndPedestrianAccessOnly = KArcAllMotorVehicles;
/** No vehicular access. */
constexpr uint32 KArcNoVehicularAccess = KArcAccessMask & ~(KArcWrongWayFlag | KArcPedestrianAccessFlag);
/** Access flags affect all vehicles; synonym of KArcNoVehicularAccess. */
constexpr uint32 KArcAllVehicles = KArcNoVehicularAccess;

// Values for use in KArcRoadTypeMask.
constexpr uint32 KArcMotorway = 0;
constexpr uint32 KArcMotorwayLink = 1;
constexpr uint32 KArcTrunkRoad = 2;
constexpr uint32 KArcTrunkRoadLink = 3;
constexpr uint32 KArcPrimaryRoad = 4;
constexpr uint32 KArcPrimaryRoadLink = 5;
constexpr uint32 KArcSecondaryRoad = 6;
constexpr uint32 KArcSecondaryRoadLink = 7;
constexpr uint32 KArcTertiaryRoad = 8;
constexpr uint32 KArcUnclassifiedRoad = 9;
constexpr uint32 KArcResidentialRoad = 10;
constexpr uint32 KArcTrack = 11;
constexpr uint32 KArcServiceRoad = 12;
constexpr uint32 KArcPedestrianRoad = 13;
constexpr uint32 KArcVehicularFerry = 14;
constexpr uint32 KArcPassengerFerry = 15;
constexpr uint32 KArcLivingStreet = 16;
constexpr uint32 KArcCycleway = 17;
constexpr uint32 KArcPath = 18;
constexpr uint32 KArcFootway = 19;
constexpr uint32 KArcBridleway = 20;
constexpr uint32 KArcSteps = 21;
constexpr uint32 KArcUnknownRoadType = 22;
constexpr uint32 KArcUnpavedRoad = 23;
constexpr uint32 KArcOtherRoadType0 = 24;
constexpr uint32 KArcOtherRoadType1 = 25;
constexpr uint32 KArcOtherRoadType2 = 26;
constexpr uint32 KArcOtherRoadType3 = 27;
constexpr uint32 KArcOtherRoadType4 = 28;
constexpr uint32 KArcOtherRoadType5 = 29;
constexpr uint32 KArcOtherRoadType6 = 30;
constexpr uint32 KArcOtherRoadType7 = 31;
constexpr uint32 KArcRoadTypeCount = 32;

const char * const KArcRoadTypeName[KArcRoadTypeCount] =
    {
    "motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link",
    "tertiary", "unclassified", "residential", "track", "service_road", "pedestrian_road", "vehicular_ferry", "passenger_ferry",
    "living_street", "cycleway", "path", "footway", "bridleway", "steps", "unknown_road_type", "unpaved_road",
    "other_road_type_0", "other_road_type_1", "other_road_type_2", "other_road_type_3",
    "other_road_type_4", "other_road_type_5", "other_road_type_6", "other_road_type_7"
    };

// Index values for use in the gradient arrays in TRouteProfile
constexpr uint32 KArcGradientUp0Index = 0;
constexpr uint32 KArcGradientUp1Index = 1;
constexpr uint32 KArcGradientUp2Index = 2;
constexpr uint32 KArcGradientUp3Index = 3;
constexpr uint32 KArcGradientDown0Index = 4;
constexpr uint32 KArcGradientDown1Index = 5;
constexpr uint32 KArcGradientDown2Index = 6;
constexpr uint32 KArcGradientDown3Index = 7;

const char * const KArcGradientName[KArcGradientCount] =
    {
    "up0", "up1", "up2", "up3", "down0", "down1", "down2", "down3"
    };

/** Constants used to select frequently-used route profiles. */
enum class TRouteProfileType
    {
    /** A profile type for private car navigation. */
    Car,
    /** A profile type for walking. */
    Walk,
    /** A profile type for cycling. */
    Cycle,
    /** A profile type for walking, preferring off-road paths. */
    Hike
    };

/** A route profile type for backward compatibility. @deprecated */
constexpr TRouteProfileType ECarRouteProfile = TRouteProfileType::Car;
/** A route profile type for backward compatibility. @deprecated */
constexpr TRouteProfileType EWalkingRouteProfile = TRouteProfileType::Walk;
/** A route profile type for backward compatibility. @deprecated */
constexpr TRouteProfileType EBicycleRouteProfile = TRouteProfileType::Cycle;
/** A route profile type for backward compatibility. @deprecated */
constexpr TRouteProfileType EHikingRouteProfile = TRouteProfileType::Hike;

/** A vehicle type used in routing. */
class TVehicleType
    {
    public:
    auto Tuple() const { return std::forward_as_tuple(iAccessFlags,iWeight,iAxleLoad,iDoubleAxleLoad,iTripleAxleLoad,iHeight,iWidth,iLength,iHazMat); }
    bool operator==(const TVehicleType& aOther) const { return Tuple() == aOther.Tuple(); }

    /** Normalizes the vehicle type: ensures that KArcOtherAccessFlag is set if and only if vehicle details are given, and that weights and dimensions are non-negative. */
    void Normalize()
        {
        if (iWeight > 0 ||
            iAxleLoad > 0 ||
            iDoubleAxleLoad > 0 ||
            iTripleAxleLoad > 0 ||
            iHeight > 0 ||
            iWidth > 0 ||
            iLength > 0 ||
            iHazMat)
            iAccessFlags |= KArcOtherAccessFlag;
        else
            iAccessFlags &= ~KArcOtherAccessFlag;

        if (iWeight < 0) iWeight = 0;
        if (iAxleLoad < 0) iAxleLoad = 0;
        if (iDoubleAxleLoad < 0) iDoubleAxleLoad = 0;
        if (iTripleAxleLoad < 0) iTripleAxleLoad = 0;
        if (iHeight < 0) iHeight = 0;
        if (iWidth < 0) iWidth = 0;
        if (iLength < 0) iLength = 0;
        }

    /**
    Flags taken from KArcWrongWayFlag ... KArcOtherAccessFlag indicating the vehicle type.
    Arcs with restrictions matching any of these flags will not be taken.
    */
    uint32 iAccessFlags = KArcCarAccessFlag | KArcWrongWayFlag;

    /** The vehicle's weight in metric tons. Values of zero or less mean 'unknown'. */
    double iWeight = 0;

    /** The vehicle's axle load in metric tons. Values of zero or less mean 'unknown'. */
    double iAxleLoad = 0;

    /** The vehicle's double axle load in metric tons. Values of zero or less mean 'unknown'. */
    double iDoubleAxleLoad = 0;

    /** The vehicle's triple axle weight in metric tons. Values of zero or less mean 'unknown'. */
    double iTripleAxleLoad = 0;

    /** The vehicle's height in meters. Values of zero or less mean 'unknown'. */
    double iHeight = 0;

    /** The vehicle's width in meters. Values of zero or less mean 'unknown'. */
    double iWidth = 0;

    /** The vehicle's length in meters. Values of zero or less mean 'unknown'. */
    double iLength = 0;

    /** True if the vehicle is carrying hazardous materials. */
    bool iHazMat = false;
    };

/**
A routing profile: parameters determining the type of route,
including road speeds, bonuses and penalties, and vehicle type.
A road type will not normally be used if its speed plus bonus is equal to or less than zero.

However, if the speed is positive and the bonus is negative, and their sum is zero,
this type of road will be allowed at the beginning or end of a route, allowing
travel over farm tracks, for instance, to leave or to arrive at a destination.

Similarly, restricted road types such as private roads are allowed at the beginning
or end of a route.
*/
class TRouteProfile
    {
    public:
    TRouteProfile(TRouteProfileType aProfileType = TRouteProfileType::Car);
    TResult WriteAsXml(MOutputStream& aOutput) const;
    TResult ReadFromXml(MInputStream& aInput);

    /** The vehicle type, defined using access flags, dimensions, weight, etc. */
    TVehicleType iVehicleType;

    /** Speeds along roads in kilometres per hour. */
    std::array<double,KArcRoadTypeCount> iSpeed;

    /**
    Bonuses or penalties in notional km per hour to be added to road types to make them more or less likely to be used.
    For example, a penalty of 1kph is applied to walking routes along major roads because it is pleasanter to walk along quieter minor roads.
    */
    std::array<double,KArcRoadTypeCount> iBonus;

    /**
    This array of bit masks allows restrictions to be overridden for certain
    types of road. For example, to allow routing of heavy goods vehicles along
    tracks, even if they are forbidden to motor vehicles, set
    iRestrictionOverride[KArcTrack] to KArcHeavyGoodsAccessFlag.
    */
    std::array<uint32,KArcRoadTypeCount> iRestrictionOverride;

    /**
    The estimated time in seconds taken for any turn at a junction that is not
    a slight turn or simply going straight ahead.
    */
    int32 iTurnTime = 4;

    /**
    The estimated time in seconds taken for a U-turn, defined as a turn
    very close to 180 degrees (within 1/32 of a circle = 11.75 degrees).
    */
    int32 iUTurnTime = 300;

    /**
    The estimated time in seconds taken for a turn across the traffic:
    that is, a left turn in drive-on-right countries,
    or a right turn in drive-on-left countries. The time is used when turning
    from the right of way to a lower-rank road. It is doubled when
    turning from a lower-rank road on to the right of way.
    */
    int32 iCrossTrafficTurnTime = 8;

    /**
    The estimated delay in seconds caused by traffic lights.
    */
    int32 iTrafficLightTime = 10;

    /** If true, ignore the speed, bonuses, and turn times when; get the shortest route, not the fastest. */
    bool iShortest = false;

    /**
    The penalty applied to toll roads as a number between zero and one.
    The value 1 means that no toll roads will be used, 0.5 makes toll roads half as desirable as non-toll roads
    of the same road type, and so on. The value 0 means that no penalty is applied to toll roads.
    Values outside the range 0...1 are clamped to that range.
    */
    double iTollPenalty = 0;

    /**
    Speeds added to roads with a particular gradient.
    These values are intended for cycling, walking, hiking and running profiles,
    where speeds are generally the same for different types of road, but
    can change according to the gradient.
    */
    std::array<double,KArcGradientCount> iGradientSpeed;

    /** Bonuses applied to roads with a particular gradient. */
    std::array<double,KArcGradientCount> iGradientBonus;

    /**
    Flags indicating which roads are affected by gradient speeds and bonuses; normally steps, etc., are excluded.
    Each bit is (1 << road type).
    */
    uint32 iGradientFlags = 0xFFFFFFFF & ~((1 << KArcSteps) | (1 << KArcPassengerFerry) | (1 << KArcVehicularFerry));
    };

/** Information about an entire route. */
class CRoute
    {
    public:
    CRoute();
    explicit CRoute(const TRouteProfile& aProfile,double aPointScale);
    void Clear(bool aClearProfile = true);
    bool Empty() const { return iRouteSegment.empty(); }
    TResult ReadFromXml(MInputStream& aInput,const CProjection& aProjection);
    TResult WriteAsXml(MOutputStream& aOutput,const CProjection& aProjection) const;
    TResult WriteAsGpx(MOutputStream& aOutput,const CProjection& aProjection) const;
    void GetNearestSegment(const TPoint& aPoint,TNearestSegmentInfo& aInfo,int32 aSection,double aPreviousDistanceAlongRoute) const;
    void GetPointAtDistance(double aDistanceInMeters,TNearestSegmentInfo& aInfo) const;
    void GetPointAtTime(double aTimeInSeconds,TNearestSegmentInfo& aInfo) const;
    void Append(const CRoute& aRoute);
    std::unique_ptr<CRoute> Copy() const;
    std::unique_ptr<CRoute> CopyWithoutRestrictedSegments() const;
    CString Instructions(CMap& aMap,const char* aLocale) const;
    double TollRoadDistance() const;
    void AppendSegment(const Router::TJunctionInfo& aBestArcInfo,const CString& aJunctionName,const CString& aJunctionRef,const CContour& aContour,
                       const CString& aName,const CString& aRef,TRoadType aRoadType,double aMaxSpeed,double aDistance,double aTime,int32 aSection,bool aRestricted);

    /** An array of route segments representing the route. */
    std::vector<std::unique_ptr<CRouteSegment>> iRouteSegment;
    /** The distance of the route in meters. */
    double iDistance = 0;
    /** The estimated time taken to traverse the route in seconds. */
    double iTime = 0;
    /** The path along the entire route in map units. */
    CContour iPath;
    /**
    The scale used to convert map units from fractional to whole units.
    For example, if the map unit is 32nds of meters this will be 1/32.
    */
    double iPointScale = 1;
    /** The path to the first non-trivial junction before the route: used when creating OpenLR location data. */
    CPathToJunction iPathToJunctionBefore;
    /** The path to the first non-trivial junction after the route: used when creating OpenLR location data. */
    CPathToJunction iPathToJunctionAfter;
    /** The profile used to create the route. */
    TRouteProfile iProfile;

    private:
    void GetPointAlongRouteHelper(const TPoint* aPoint,double* aDistance,double* aTime,
                                  TNearestSegmentInfo& aInfo,int32 aSection,double aPreviousDistanceAlongRoute) const;
    };

/** Turn information for navigation: the base Turn class plus the distance to the turn, road names and turn instruction. */
class TNavigatorTurn: public TTurn
    {
    public:
    TNavigatorTurn():
        iDistance(0),
        iTime(0),
        iFromRoadType(TRoadType::UnknownMajor),
        iToRoadType(TRoadType::UnknownMajor)
        {
        }

    TNavigatorTurn(const CRouteSegment& aPrevSegment,
                   const CRouteSegment& aCurSegment,
                   double aDistance,double aTime):
        TTurn(aCurSegment.iTurn),
        iDistance(aDistance),
        iTime(aTime),
        iFromName(aPrevSegment.iName),
        iFromRef(aPrevSegment.iRef),
        iFromRoadType(aPrevSegment.iRoadType),
        iToName(aCurSegment.iName),
        iToRef(aCurSegment.iRef),
        iToRoadType(aCurSegment.iRoadType)
        {
        const TPoint* end = aPrevSegment.iPath.End();
        if (end)
            iPosition = *end;
        }

    TNavigatorTurn(const CRouteSegment& aPrevSegment,const CRouteSegment* aCurSegment,double aDistance,double aTime):
        TTurn(),
        iDistance(aDistance),
        iTime(aTime),
        iFromName(aPrevSegment.iName),
        iFromRef(aPrevSegment.iRef),
        iFromRoadType(aPrevSegment.iRoadType)
        {
        if (aCurSegment)
            *this = TNavigatorTurn(aPrevSegment,*aCurSegment,aDistance,aTime);
        else
            {
            const TPoint* end = aPrevSegment.iPath.End();
            if (end)
                iPosition = *end;
            }
        }

    void Clear()
        {
        TTurn::Clear();
        iDistance = 0;
        iFromRoadType = TRoadType::UnknownMajor;
        iToRoadType = TRoadType::UnknownMajor;
        iFromName.Clear();
        iFromRef.Clear();
        iToName.Clear();
        iToRef.Clear();
        iPosition.iX = iPosition.iY = 0;
        }

    bool CanMerge(const TNavigatorTurn& aFollowingTurn) const;
    bool MergeIfPossible(const TNavigatorTurn& aFollowingTurn);
    void Merge(const TNavigatorTurn& aFollowingTurn);
    void CreateInstructions(const char* aLocale);

    /** The distance in meters from the current position to the turn or the arrival point. */
    double iDistance;
    /** The estimated time in seconds from the current position to the turn or arrival point. */
    double iTime;
    /** The name, if known, of the road before the turn. */
    CString iFromName;
    /** The reference code, if known, of the road before the turn. */
    CString iFromRef;
    /** The type of the road before the turn. */
    TRoadType iFromRoadType;
    /** The name, if known, of the road after the turn. */
    CString iToName;
    /** The reference code, if known, of the road after the turn. */
    CString iToRef;
    /** The type of the road after the turn. */
    TRoadType iToRoadType;
    /** The position of the turn in map coordinates. */
    TPoint iPosition;
    /** The localized turn instructions. */
    CString iInstructions;
    };

/**
Create an object of a class derived from MNavigatorObserver to handle
navigation events like turn instructions.
*/
class MNavigatorObserver
    {
    public:
    virtual ~MNavigatorObserver() { }

    /**
    This message gives up to three turns.

    aFirstTurn is the first significant turn after the current position, ignoring 'ahead' and 'continue' turns.
    If its type is ENoTurn there are no remaining significant turns.
    aFirstTurn.iDistance is the distance from the current position to the first turn or the arrival point.

    aSecondTurn, if non-null, indicates that there is a significant turn 100 metres or less after aFirstTurn.
    aSecondTurn->iDistance is the distance from the first turn to the second turn.

    aContinuationTurn, if non-null, signals that there is an 'ahead' or 'continue' turn before aFirstTurn, so that
    the navigation system can show that no turn is required at the next junction or adjoining minor road.
    aContinuationTurn->iDistance is the distance from the current position to the continuation turn.
    */
    virtual void OnTurn(const TNavigatorTurn& aFirstTurn,const TNavigatorTurn* aSecondTurn,const TNavigatorTurn* aContinuationTurn) = 0;
    /**
    Turn round at a legal and safe place and go back in the reverse direction.
    This message is issued when the vehicle is on the correct route but going the wrong way.
    */
    virtual void OnTurnRound() = 0;
    /**
    This message is issued when the vehicle is off the route and a new route has been calculated.
    */
    virtual void OnNewRoute() = 0;
    /**
    This message is issued when the position becomes known.
    */
    virtual void OnPositionKnown() = 0;
    /**
    This message is issued when the position becomes unknown.
    */
    virtual void OnPositionUnknown() = 0;
    /**
    This message is issued when an object of interest becomes nearby, according
    to the criteria set by calls to AddNearbyObjectWarning.

    aDistanceToRoute is the distance of the object from the nearest point on the route.
    aDistanceAlongRoute is the distance along the route from the current point to the nearest on-route point to the object.
    */
    virtual void OnAddNearbyObject(const CMapObject* /*aObject*/,double /*aDistanceToRoute*/,double /*aDistanceAlongRoute*/) { }
    /**
    This message is issued when the distance to an object of interest has changed.

    aDistanceToRoute is the distance of the object from the nearest point on the route.
    aDistanceAlongRoute is the distance along the route from the current point to the nearest on-route point to the object.
    */
    virtual void OnUpdateNearbyObject(const CMapObject* /*aObject*/,double /*aDistanceToRoute*/,double /*aDistanceAlongRoute*/) { }
    /**
    This message is issued an object of interest is no longer nearby.
    */
    virtual void OnRemoveNearbyObject(const CMapObject* /*aObject*/) { }
    };

/** Possible states of the navigator. */
enum class TNavigatorState
    {
    /** No route has been created, or navigation is disabled. */
    None,
    /** There is a route but no position has been supplied. */
    NoPosition,
    /** The latest position is on the route. */
    OnRoute,
    /** The latest position is off the route but on a road. */
    OffRoute,
    /** The latest position is not on a road. */
    OffRoad,
    /**
    Re-routing is needed. This state is used only when automatic re-routing is turned
    off, for instance when using a slower routing algorithm.
    */
    ReRouteNeeded,
    /** Re-routing has just been done. */
    ReRouteDone
    };

/** Basic data received from a navigation device such as a GPS receiver. */
class TNavigationData
    {
    public:
    /** Bit values for flags in iValidity. */
    static constexpr uint32 KTimeValid = 1;
    static constexpr uint32 KPositionValid = 2;
    static constexpr uint32 KSpeedValid = 4;
    static constexpr uint32 KCourseValid = 8;
    static constexpr uint32 KHeightValid = 16;
    
    /** Flags indicating validity or availability of data. */
    uint32 iValidity = 0;
    /**
    The time in seconds. Any consistent time representation may be used (e.g., the unix
    timestamp format of the time in seconds since midnight, 1st Jan 1970); the absolute
    time does not matter as long as times can be subtracted to give a difference in seconds.
    */
    double iTime = 0;
    /** Position in degrees latitude and longitude. */
    TPointFP iPosition;
    /** Speed in kilometres per hour. */
    double iSpeed = 0;
    /** True course in degrees. */
    double iCourse = 0;
    /** Height above sea level in metres. */
    double iHeight = 0;
    };

/** Parameters governing navigation behaviour. */
class TNavigatorParam
    {
    public:
    TNavigatorParam():
        iMinimumFixDistance(5),
        iRouteDistanceTolerance(20),
        iRouteTimeTolerance(30),
        iAutoReRoute(true),
        iNavigationEnabled(true)
        {
        }

    /** Minimum distance between location fixes in metres that is taken as an actual move. */
    int32 iMinimumFixDistance;
    /** Maximum distance from the route in metres before the vehicle is deemed off-route. */
    int32 iRouteDistanceTolerance;
    /** Maximum time off route in seconds before a new route needs to calculated. */
    int32 iRouteTimeTolerance;
    /**
    True if route is recalculated automatically when the vehicle goes off route (the default).
    If not, the state EReRouteNeededState is entered and the route is recalculated the next time
    the user provides a navigation fix. This gives the application time to
    issue a warning if the routing algorithm is likely to be slow.
    */
    bool iAutoReRoute;
    /**
    If true, and if there is a route, the position on route is updated and turn information is created, when a position update arrives.
    If false, the vehicle position and speed are updated but other behaviour is the same as if there is no route.
    */
    bool iNavigationEnabled;
    };

/** An iterator allowing a route to be traversed. */
class TRouteIterator
    {
    public:
    TRouteIterator(const CRoute& aRoute);
    /** Move forwards by aDistance. Return false if the end of the path is reached, true if not. */
    bool Forward(double aDistance) { return iPathIter.Forward(aDistance); }
    bool NextContour() { return iPathIter.NextContour(); }
    const TPoint& Position() const { return iPathIter.Position(); }
    double Direction() const { return iPathIter.Direction(); }

    private:
    TPathIterator iPathIter;
    };

/**
Traffic information.
This information is normally used in combination with a location reference.
*/
class CTrafficInfo
    {
    public:
    CTrafficInfo():
        iVehicleTypes(KArcAllVehicles),
        iSpeed(KNoSpeedLimit),
        iWeight(0),
        iWorks(false),
        iToll(false),
        iLanes(0)
        {
        }
    TResult WriteAsXml(MOutputStream& aOutput) const;

    /** Speeds of this value or greater mean there is no speed limit. */
    static constexpr double KNoSpeedLimit = 255;

    /** Vehicle types affected by this information, taken from the KArc... bit definitions; use KArcAllVehicles to select all vehicle types. */
    uint32 iVehicleTypes;
    /** Permitted or expected speed in kph: KNoSpeedLimit or greater means no speed limit; 0 or less means the route is forbidden. */
    double iSpeed;
    /** This information applies to vehicles with this minimum weight in metric tons. */
    double iWeight;
    /** True if there are road works. */
    bool iWorks;
    /** True if a toll applies. */
    bool iToll;
    /** If greater than zero, the number of lanes open. */
    int32 iLanes;
    };

/** The side of the road: used in traffic information. */
enum class TSideOfRoad
    {
    None,
    Right,
    Left,
    Both
    };

/** The orientation of a path along a road: used in traffic information. */
enum class TRoadOrientation
    {
    None,
    Forwards,
    Backwards,
    Both,
    };

/** The type of a location reference used for traffic information. */
enum class TLocationRefType
    {
    /** A line in the route network. */
    Line,
    /** A point on the earth's surface. */
    GeoCoordinate,
    /** A point on a line in the route network. */
    PointAlongLine,
    /** A point on the route network providing access to a nearby POI: the first point is the point on the line, the second is the POI. */
    PointWithAccessPoint,
    /** A circle defined as a point and a radius. */
    Circle,
    /** A rectangle aligned to the grid of latitude and longitude: it is defined using two points at opposite corners. */
    Rectangle,
    /** A polygon defined using a set of points. */
    Polygon,
    /** A closed line in the route network. */
    ClosedLine
    };

/** Parameters used for a location reference when writing traffic information. */
class CLocationRef
    {
    public:
    CLocationRef(TLocationRefType aType,TCoordType aCoordType):
        iType(aType),
        iGeometry(aCoordType),
        iRadiusInMeters(0),
        iSideOfRoad(TSideOfRoad::None),
        iRoadOrientation(TRoadOrientation::None)
        {
        }

    TResult WriteAsXml(CFramework& aFramework,MOutputStream& aOutput) const;

    /** The type of this location reference. */
    TLocationRefType iType;
    /** The arbitrary ID of the location reference. */
    CString iId;
    /** The point or points. */
    CGeometry iGeometry;
    /** The radius, if this is a circle. */
    double iRadiusInMeters;
    /** The side of the road, if relevant. */
    TSideOfRoad iSideOfRoad;
    /** The road orientation, if relevant. */
    TRoadOrientation iRoadOrientation;
    };

}

#endif

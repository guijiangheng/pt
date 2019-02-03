#ifndef PT_PT_H
#define PT_PT_H

namespace pt {

#ifdef PT_FLOAT_AS_DOUBLE
    using Float = double;
#else
    using Float = float;
#endif

constexpr auto RayOriginOffsetEpsilon      = (Float)0.00001;
constexpr auto ShadowEpsilon               = (Float)0.0001;
constexpr auto TriangleIntersctEpsilon     = (Float)0.000001;
constexpr auto TileSize                    = 16;

}

#endif

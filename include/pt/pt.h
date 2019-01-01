#ifndef PT_PT_H
#define PT_PT_H

namespace pt {

#ifdef PT_FLOAT_AS_DOUBLE
    using Float = double;
#else
    using Float = float;
#endif

constexpr Float ShadowEpsilon               = (Float)0.0001;
constexpr Float TriangleIntersctEpsilon     = (Float)0.000001;

}

#endif

#ifndef PT_PT_H
#define PT_PT_H

namespace pt {

#ifdef PT_FLOAT_AS_DOUBLE
    using Float = double;
#else
    using Float = float;
#endif

}

#endif
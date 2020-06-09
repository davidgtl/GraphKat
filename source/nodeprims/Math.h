//
// Created by david on 22/12/2019.
//

#ifndef GRAPHKAT_MATH_H
#define GRAPHKAT_MATH_H

namespace Math {

    template<typename T>
    T linmap(T x, T fstart, T fend, T tstart, T tend) {
        return (x - fstart) / (fend - fstart) * (tend - tstart) + tstart;
    }


}


#endif //GRAPHKAT_MATH_H

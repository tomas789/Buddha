#ifndef _UTILITY_HPP
#define _UTILITY_HPP

template <typename T>
T clamp(const T & lower, const T & upper, const T & value) {
    if (value < lower)
        return lower;
    if (value > upper)
        return upper;
    return value;
}

template <typename T, typename U>
T interpolate(const T & lower, const T & upper, const U & where) {
    return lower + (T)((upper - lower) * where);
}

#endif // _UTILITY_HPP

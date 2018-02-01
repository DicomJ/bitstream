#ifndef __BITSTREAM_FIXED_POINT__
#define __BITSTREAM_FIXED_POINT__

#include <bitstream/field.h>


namespace bitstream {

template <typename T, long size=sizeof(T) * 8 / 2>  // size - a size of fraction part
struct FixedPoint {

    static constexpr auto scaling_factor = (1LL << size);
    static constexpr auto integer_mask = ~(uint64_t(-1LL) << size);

    FixedPoint() {}
    FixedPoint(const T &value): value(value) {}
    FixedPoint(const FixedPoint &fp): value(fp.value) {}

    operator T() const { return value; }
    auto &operator = (const T &value) { this->value = value; return *this; }
    auto &operator = (const FixedPoint &fp) { return *this = fp.value; }

    template <long size_, long offset, bitstream::Endianness endianness, typename signedness>
    auto &operator = (const bitstream::Field<size_, offset, endianness, signedness> &field) { value = T(field); return *this; }

    operator double() const { return double(value) / scaling_factor; }
    auto &operator = (const double value) { this->value = T(value * scaling_factor); }

    T integer() const { return value >> size; }
    T fraction() const { return integer_mask & value; }
    void set(const T &integer, const T&fraction) { value = (integer << size) | (integer_mask & fraction); }

    T value;
};

} // namespace bitstream


#endif // __BITSTREAM_FIXED_POINT__


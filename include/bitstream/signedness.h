#ifndef __BITSTREAM_SIGNEDNESS_H__
#define __BITSTREAM_SIGNEDNESS_H__


namespace bitstream {

template <typename signedness>
struct Signedness;

template <>
struct Signedness<signed> {
    static const char *string;
};

template <>
struct Signedness<unsigned> {
    static const char *string;
};


namespace signedness {

template <bool _signed>
struct FromBoolTo;

template <>
struct FromBoolTo<true> {
    using type = signed;
};

template <>
struct FromBoolTo<false> {
    using type = unsigned;
};

} // namespace signedness

} // namespace bitstream

#endif // __BITSTREAM_SIGNEDNESS_H__


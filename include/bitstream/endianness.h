#ifndef __BITSTREAM_ENDIANNESS_H__
#define __BITSTREAM_ENDIANNESS_H__


namespace bitstream {


enum Endianness {
    little,
    big,
};


} // namespace bitstream


namespace bitstream {
namespace endianness {

extern const char *string[2];

}} // namespace bitstream::endianness



#endif // __BITSTREAM_ENDIANNESS_H__


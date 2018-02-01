#ifndef __BITSTREAM_MACHINE_H__
#define __BITSTREAM_MACHINE_H__

#include <bitstream/endianness.h>


namespace bitstream {
namespace machine {


static const Endianness endianness =
#   if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    Endianness::little
#   else
    Endianness::big
#   endif
; // endianness


template <long offset, long size, Endianness endianness, typename type>
class Value; // if type is not defined either compiler or an architecture is not supportted


namespace integral {


// has to define typename Type<size, signedness>::type_t
template <long size, typename signedness>
struct Type;


} // namespace integral


}} // namespace bitstream::machine


#   if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
#       include <bitstream/machine/x86.h>
#   else
#       error "Processor type is not supported yet"
#   endif


#endif // __BITSTREAM_MACHINE_H__


#ifndef __BITSTREAM_HEADER_H__
#define __BITSTREAM_HEADER_H__


namespace bitstream {

struct Header {
    virtual ~Header() {}

    template <bool verify>
    using const_field = bitstream::Const::Field<verify>;
};


} // namespace bitstream


#endif // __BITSTREAM_HEADER_H__


#ifndef __BITSTREAM_FOOTPRINT_H__
#define __BITSTREAM_FOOTPRINT_H__

#include <bitstream/endianness.h>


namespace bitstream {

struct Ptr {
    Ptr() {}    // Leave it uninitialized
    Ptr(const Ptr &ptr): buffer_(ptr.buffer_) {}
    Ptr(const char *buffer) : buffer_(buffer) {}
    void buffer(const char *buffer) { buffer_ = buffer; }
    const char *buffer() const { return buffer_; }
    char *buffer() { return const_cast<char *>(buffer_); }

    private: const char *buffer_;
};


template <long size_, long offset_=0>
struct Footprint : Ptr {
    static const long size = size_;
    static const long offset = offset_;
    static const long bytes_occupied = (offset + size) / 8 + ((offset + size) % 8 != 0? 1: 0);
    using Ptr::Ptr;
    Footprint(const Ptr &ptr): Ptr(ptr) {}
};


} // namespace bitstream

#endif // __BITSTREAM_FOOTPRINT_H__


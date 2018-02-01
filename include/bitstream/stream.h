#ifndef __BITSTREAM_STREAM_H__
#define __BITSTREAM_STREAM_H__

#include <stdexcept>


namespace bitstream {


struct Blob;


struct Stream {
    virtual ~Stream() {}

    virtual uint64_t offset() const = 0;
    virtual const char *peak(unsigned long size) = 0;
    virtual Blob &peak_blob(unsigned long size) = 0;
    virtual Blob &get_blob(unsigned long size) = 0;

    struct Distance;
    struct EndOfStream;
};


struct Stream::EndOfStream: std::runtime_error {
    using std::runtime_error::runtime_error;
};


struct Stream::Distance {

    uint64_t start;
    Stream &istream;

    operator uint64_t () const { return istream.offset() - start; }
    uint64_t size() const { return istream.offset() - start; }

    Distance(Stream &istream) :
        istream(istream), start(istream.offset()) {}
};


} // namespace bitstream

#endif // __BITSTREAM_STREAM_H__


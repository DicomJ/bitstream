#ifndef __BITSTREAM_BLOB_H__
#define __BITSTREAM_BLOB_H__


namespace bitstream {

struct Blob {
    virtual ~Blob() {}
    virtual unsigned long size() const = 0;
};

} // namespace bitstream

#endif // __BITSTREAM_BLOB_H__


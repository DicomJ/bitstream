#ifndef __BITSTREAM_IHSTREAM_H__
#define __BITSTREAM_IHSTREAM_H__


#include <vector>
#include <bitstream/footprint.h>
#include <bitstream/stream.h>
#include <bitstream/array.h>
#include <bitstream/string.h>


namespace bitstream {
namespace header {

struct Stream {

    struct Stash: std::vector<Ptr*> {
        Stash() { reserve(10); }
    };

    bitstream::Stream &stream;
    Stash stash;

    unsigned long consumed_ = 0;
    const char *data = nullptr;

    Stream(bitstream::Stream &stream)
        : stream(stream) {
    }

    unsigned long consumed() const { return consumed_; }

    void reset() {
        consumed_ = 0;
        //data = nullptr; - optimized
        stash.clear();
    }

    void get(bitstream::Ptr &ptr, long bytes, bool peak_only=false) {
        auto data = stream.peak(this->consumed_ + bytes);
        if (data != this->data) {
            auto offset = data - this->data;
            for (auto &item: stash) {
                item->buffer(item->buffer() + offset);
            }
            this->data = data;
        }
        ptr.buffer(data + this->consumed_);
        if (!peak_only) {
            stash.push_back(&ptr);
            this->consumed_ += bytes;
        }
    }


};

}} // namespace bitstream::header


#endif // __BITSTREAM_IHSTREAM_H__


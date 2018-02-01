#ifndef __BITSTREAM_REMAINDER_H__
#define __BITSTREAM_REMAINDER_H__


namespace bitstream {


struct Remainder {

    Remainder(uint64_t left = uint64_t(-1LL) >> 1) : left(left) {}

    operator uint64_t () const { return left; }

    template <typename lambda>
    void reduce(uint64_t size, lambda overcommitment) {
        if (left < size) {
            throw overcommitment();
        } else {
            left -= size;
        }
    }

    void operator = (uint64_t size) { left = size; }

private:
    uint64_t left;
};


} // namespace bitstream


#endif // __BITSTREAM_REMAINDER_H__


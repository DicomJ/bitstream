#ifndef __BITSTREAM_SSTREAM_H__
#define __BITSTREAM_SSTREAM_H__


#include <sstream>


namespace bitstream {

struct SStream {
    template <typename Type>
    SStream &operator << (const Type &value) {
        ss << value;
        return *this;
    }

    operator std::string() const { return ss.str(); }

private:
    std::ostringstream ss;
};


} // namespace bitstream


#endif // __BITSTREAM_SSTREAM_H__


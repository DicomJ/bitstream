#ifndef __BITSTREAM_STRING_H__
#define __BITSTREAM_STRING_H__

#include <string>
#include <bitstream/array.h>


namespace bitstream {

namespace Static {

template <long items, long offset = 0>
struct String: bitstream::be::UInt8<offset>::Static::template Array<items> {
    using bitstream::be::UInt8<offset>::Static::template Array<items>::Array;

    operator std::string() const {
        return std::string(this->buffer(), this->buffer() + this->items);
    }

    unsigned long size() const { return this->items; }

    void operator = (const std::string &str) {
        assert(str.size() == this->items);
        for (auto i = 0; i < str.size(); ++i) {
            (*this)[i] = str[i];
        }
    }
};


template <long items, long offset = 0>
struct CString: String<items, offset> {
    using String<items, offset>::String;

    operator std::string() const {
        std::string(String<items, offset>::operator std::string ().c_str());
    }

    void operator = (const std::string &str) {
        assert(str.size() + sizeof('\0') == this->items);
        for (auto i = 0; i < str.size(); ++i) {
            (*this)[i] = str[i];
        }
        (*this)[str.size()] = '\0';
    }
};


} // namespace Static

template <long offset = 0>
struct String: bitstream::be::UInt8<offset>::Array {
    using bitstream::be::UInt8<offset>::Array::Array;

    operator std::string() const {
        return std::string(this->buffer(), this->buffer() + this->items);
    }

    unsigned long size() const { return this->items; }

    auto &operator = (const std::string &str) {
        assert(str.size() == this->items);
        for (auto i = 0; i < str.size(); ++i) {
            (*this)[i] = str[i];
        }
        return *this;
    }
};


template <long offset = 0>
struct CString: String<offset> {
    using String<offset>::String;

    operator std::string() const {
        return std::string(String<offset>::operator std::string ().c_str());
    }

    auto &operator = (const std::string &str) {
        assert(str.size() + sizeof('\0') == this->items);
        for (auto i = 0; i < str.size(); ++i) {
            (*this)[i] = str[i];
        }
        (*this)[str.size()] = '\0';
        return *this;
    }
};

} // namespace bitstream


#endif // __BITSTREAM_STRING_H__


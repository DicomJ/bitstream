#ifndef __BITSTREAM_COMPOSER_H__
#define __BITSTREAM_COMPOSER_H__

#include <bitstream/hstream.h>


namespace bitstream {

struct Stream;

struct Composer {
    virtual ~Composer() {}

    Composer(bitstream::Stream &stream)
        : stream(stream), hstream(stream) {}

    bitstream::Stream &stream;
    bitstream::header::Stream hstream;


    template<long size, bool = size % 8>
    struct Bits; // Not declared means it's not multiplied by 8

    template<long size>
    struct Bits<size, 0> {
        static const long to_bytes = size / 8;
    };


    // Allocating bitstream related types
    //

    // Footprint
    template <long bits, long offset>
    auto &get(bitstream::Footprint<bits, offset> &footprint) {
        hstream.get(footprint, Bits<bits>::to_bytes);
        return footprint;
    }

    // Field
    template <long size, long offset, Endianness endianness, typename signedness>
    auto &get(bitstream::Field<size, offset, endianness, signedness> &field) {
        hstream.get(field, Bits<size>::to_bytes);
        return field;
    }

    //Static::Array
    template <typename Field, long items>
    auto &get(bitstream::Static::Array<Field, items> &array) {
        hstream.get(array, Bits<Field::size>::to_bytes * items);    // TODO: make runtime version of to_bytes
        return array;
    }

    // Array
    template <typename Field>
    auto &get(bitstream::Array<Field> &array, long items) {
        hstream.get(array, Bits<Field::size>::to_bytes * items);    // TODO: make runtime version of to_bytes
        array.items = items;
        return array;
    }

    // Static::String
    template <long items, long offset>
    auto &get(bitstream::Static::String<items, offset> &string) {
        hstream.get(string, Footprint<8 * items, offset>::bytes_occupied);
        return string;
    }

    // String
    template <long offset>
    auto &get(bitstream::String<offset> &string, long items) {
        hstream.get(string, items + Footprint<0, offset>::bytes_occupied);
        string.items = items;
        return string;
    }


    // Static::CString
    template <long items, long offset>
    auto &get(bitstream::Static::CString<items, offset> &cstring) {
        hstream.get(cstring, Footprint<8 * items, offset>::bytes_occupied);
        return cstring;
    }

    // CString
    template <long offset>
    auto &get(bitstream::CString<offset> &cstring, long items) {
        hstream.get(cstring, items + Footprint<0, offset>::bytes_occupied);
        cstring.items = items;
        return cstring;
    }


    // Allocate & set
    //

    //Array
    template <typename Field>
    auto &get(bitstream::Array<Field> &array, const typename bitstream::Array<Field>::Vector &value) {
        return get(array, long(value.size())) = value;
    }

    // String
    template <long offset>
    auto &get(bitstream::String<offset> &string, const std::string &value) {
        return (get(string, long(value.size())) = value);
    }

    // CString
    template <long offset>
    auto &get(bitstream::CString<offset> &cstring, const std::string &value) {
        return (get(cstring, long(value.size() + sizeof('\0'))) = value);
    }


    // Variadic allocation
    //

    template <typename Type, typename Value, typename ...Types>
    void get(Type &type, const Value &value, Types &...values) {
        get(type, value);
        get(values...);
    }
};

} // namespace bitstream


#endif // __BITSTREAM_COMPOSER_H__


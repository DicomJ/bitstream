#ifndef __BITSTREAM_OMFTAG_H__
#define __BITSTREAM_OMFTAG_H__

#include <bitstream/field.h>
#include <bitstream/array.h>
#include <bitstream/string.h>
#include <bitstream/omstream.h>


namespace bitstream {
namespace output {
namespace meta {
namespace field {
namespace tag {

struct UInteger {
    UInteger(unsigned long value): value(value) {}
    operator unsigned long () const { return value; }
    unsigned long value;
};

struct Offset: UInteger { using UInteger::UInteger; };
struct Size: UInteger { using UInteger::UInteger; };

struct Type : std::string {
    using std::string::string;
};

struct Format {
    Format(const std::string &how, const std::string &what = "value")
        : what(what), how(how) { }

    std::string what, how;
};

inline void operator << (meta::Stream::Tag &tag, const Format &format) {
    tag.formatters[format.what] = format.how;
}

inline void operator << (meta::Stream::Tag &tag, const Offset &offset) {
    tag.offset = offset;
}

inline void operator << (meta::Stream::Tag &tag, const Type &type) {
    tag.type = type;
}

inline void operator << (meta::Stream::Tag &tag, const Size &size) {
    tag.size = size;
}

inline void operator << (meta::Stream::Tag &tag, const std::string &name) {
    tag.name = name;
}

inline void operator << (meta::Stream::Tag &tag, const char *name) {
    tag.name = name;
}


template <typename Type>
struct From {
    static meta::Stream::Tag type() {
        return meta::Stream::Tag();
    }
};

template <long size, long offset, Endianness endianness, typename signedness>
struct From<Field<size, offset, endianness, signedness>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = size;
        tag.offset = offset;
        return tag;
    }
};


template <long size, long offset, Endianness endianness, typename signedness>
struct From<Array<Field<size, offset, endianness, signedness>>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = size;
        tag.offset = offset;
        return tag;
    }
};

template <long size, long offset, Endianness endianness, typename signedness, long items>
struct From<Static::Array<Field<size, offset, endianness, signedness>, items>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = size;
        tag.offset = offset;
        return tag;
    }
};


template <long offset>
struct From<bitstream::String<offset>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = 8;
        tag.offset = offset;
        tag << Format("string");
        tag << Type("string");
        return tag;
    }
};


template <long items, long offset>
struct From<bitstream::Static::String<items, offset>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = 8;
        tag.offset = offset;
        tag << Format("string");
        tag << Type("string");
        return tag;
    }
};


template <long offset>
struct From<bitstream::CString<offset>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = 8;
        tag.offset = offset;
        tag << Format("string");
        tag << Type("string");
        return tag;
    }
};


template <long items, long offset>
struct From<bitstream::Static::CString<items, offset>> {

    static meta::Stream::Tag
    type() {
        meta::Stream::Tag tag;
        tag.size = 8;
        tag.offset = offset;
        tag << Format("string");
        tag << Type("string");
        return tag;
    }
};


template <typename Type>
struct To {
    static const Type &value(const Type &value) {
        return value;
    }
};

template <long size, long offset, Endianness endianness, typename signedness>
struct To<Field<size, offset, endianness, signedness>> {

    static typename Field<size, offset, endianness, signedness>::type
    value(const Field<size, offset, endianness, signedness> &field) {
        return field;
    }
};

template <long offset>
struct To<bitstream::String<offset>> {

    static typename bitstream::String<offset>::Vector
    value(const bitstream::String<offset> &value) {
        return static_cast<const typename bitstream::String<offset>::Vector &>(value);
    }
};


template <long items, long offset>
struct To<bitstream::Static::String<items, offset>> {

    static typename bitstream::Static::String<offset>::Vector
    value(const bitstream::Static::String<items, offset> &value) {
        return static_cast<const typename bitstream::Static::String<items, offset>::Vector &>(value);
    }
};


template <long offset>
struct To<bitstream::CString<offset>> {

    static typename bitstream::CString<offset>::Vector
    value(const bitstream::CString<offset> &value) {
        return static_cast<const typename bitstream::CString<offset>::Vector &>(value);
    }
};


template <long items, long offset>
struct To<bitstream::Static::CString<items, offset>> {

    static typename bitstream::Static::CString<offset>::Vector
    value(const bitstream::Static::CString<items, offset> &value) {
        return static_cast<const typename bitstream::Static::CString<items, offset>::Vector &>(value);
    }
};


struct Stream {

    meta::field::Stream &stream;
    Stream(meta::field::Stream &stream): stream(stream) {}

    template <typename Type, typename Value = Type>
    struct Tag  {

        template <typename Feature>
        Tag &operator << (const Feature &feature) { tag << feature; return *this; }

        Tag(meta::field::Stream &stream, const Value &value)
            : tag(meta::field::tag::From<Type>::type()), stream(stream), value(value) {
        }

        ~Tag() {
            stream.field(tag, meta::field::tag::To<Value>::value(value));
        }

        meta::Stream::Tag tag;
        meta::field::Stream &stream;
        const Value &value;
    };

    template <typename Value>
    Tag<Value> tag(const Value &value) const {
        return Tag<Value, Value>(stream, value);
    }

    template <typename Type, typename Value>
    Tag<Type, Value> typed_tag(const Value &value) const {
        return Tag<Type, Value>(stream, value);
    }

    // Because of shitty nondeducible types and lasy compiler it's creators
    template <typename Array>
    Tag<Array> array_tag(const Array &value) const {
        auto t = Tag<Array>(stream, value);
        t.tag.offset = Array::template Item<>::offset;
        t.tag.size = Array::template Item<>::size;
        return t;
    }
};


}}}}} // namespace bitstream::output::meta::field::tag


#endif // __BITSTREAM_OMFTAG_H__

#ifndef __BITSTREAM_PARSER_H__
#define __BITSTREAM_PARSER_H__

#include <stdexcept>
#include <bitstream/dcast.h>
#include <bitstream/remainder.h>

#include <bitstream/composer.h>


namespace bitstream {


struct Header;
struct Blob;
struct Stream;


struct Parser: Composer {

    struct Exception;
    struct Event;
    struct Observer;


    Parser(bitstream::Stream &stream, Observer &observer)
        : Composer(stream), observer(observer) {}

    Observer &observer;

    virtual void parse(Remainder = Remainder(), bool raise_eos = false) = 0;


    // Allocating bitstream related types
    //

    // Footprint
    template <long bits, long offset>
    auto &get(bitstream::Footprint<bits, offset> &footprint) {
        return bitstream::Composer::get(footprint);
    }

    // Field
    template <long size, long offset, Endianness endianness, typename signedness>
    auto &get(bitstream::Field<size, offset, endianness, signedness> &field) {
        return bitstream::Composer::get(field);
    }

    //Static::Array
    template <typename Field, long items>
    auto &get(bitstream::Static::Array<Field, items> &array) {
        return bitstream::Composer::get(array);
    }

    // Array
    template <typename Field>
    auto &get(bitstream::Array<Field> &array, long items) {
        return bitstream::Composer::get(array, items);
    }

    // Static::String
    template <long items, long offset>
    auto &get(bitstream::Static::String<items, offset> &string) {
        return bitstream::Composer::get(string);
    }

    // String
    template <long offset>
    auto &get(bitstream::String<offset> &string, long items) {
        return bitstream::Composer::get(string, items);
    }


    // Static::CString
    template <long items, long offset>
    auto &get(bitstream::Static::CString<items, offset> &cstring) {
        return bitstream::Composer::get(cstring);
    }

    // CString
    template <long offset>
    auto &get(bitstream::CString<offset> &cstring, long items) {
        return bitstream::Composer::get(cstring, items);
    }


    // Length based
    //

    // Footprint
    template <long bits, long offset>
    long get(long limit, bitstream::Footprint<bits, offset> &footprint) {
        bitstream::Composer::get(footprint);
        return limit - Bits<bits>::to_bytes;
    }

    // Field
    template <long size, long offset, Endianness endianness, typename signedness>
    long get(long limit, bitstream::Field<size, offset, endianness, signedness> &field) {
        bitstream::Composer::get(field);
        return limit - Bits<size>::to_bytes;
    }

    //Static::Array
    template <typename Field, long items>
    long get(long limit, bitstream::Static::Array<Field, items> &array) {
        bitstream::Composer::get(array);
        return limit - Bits<Field::size>::to_bytes * items;
    }

    // Array
    template <typename Field>
    long get(long limit, bitstream::Array<Field> &array) {
        bitstream::Composer::get(array, long(limit / Bits<Field::size>::to_bytes));
        return limit - Bits<Field::size>::to_bytes * array.items;
    }

    // Static::String
    template <long items, long offset>
    long get(long limit, bitstream::Static::String<items, offset> &string) {
        bitstream::Composer::get(string);
        return limit - items;
    }

    // String
    template <long offset>
    long get(long limit, bitstream::String<offset> &string) {
        bitstream::Composer::get(string, limit);
        return 0;
    }


    // Static::CString
    template <long items, long offset>
    long get(long limit, bitstream::Static::CString<items, offset> &cstring) {
        bitstream::Composer::get(cstring);
        return limit - items;
    }

    // CString
    template <long offset>
    long get(long limit, bitstream::CString<offset> &cstring) {
        auto i = find_char<offset>('\0', limit);
        get(cstring, long(i != -1 ?  i + 1: limit));   // Take all the limit even if terminator is not found
        return limit - cstring.items;
    }

    template <long offset>
    long find_char(char ch, long limit) {
        bitstream::String<offset> payload;
        hstream.get(payload, limit + Footprint<0, offset>::bytes_occupied, true);
        for (long i = 0; i < limit; ++i) {
            if (payload[i] == ch) {
                return i;
            }
        }
        return -1;
    }


    // Variadic get
    //
    template <typename Type, typename ...Types>
    long get(long limit, Type &value, Types &...values) {
        limit = get(limit, value);
        return get(limit, values...);
    }
};

struct Parser::Exception: std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Parser::Event {
    struct Exception;
    struct Header;
    struct Payload;

    Parser &parser;
    Event(Parser &parser) : parser(parser) {}

protected:
    struct _Header;
};

struct Parser::Event::Exception: Parser::Event {
    Exception(Parser &parser);
};

struct Parser::Event::_Header: Parser::Event {
    const bitstream::Header &header;
    _Header(Parser &parser, const bitstream::Header &header)
        : Parser::Event(parser), header(header) {}
};

struct Parser::Event::Header: Parser::Event::_Header {
    Header(Parser &parser, const bitstream::Header &header);
};

struct Parser::Event::Payload: Parser::Event::_Header {
    struct Data;
    struct Boundary;

    Payload(Parser &parser, const bitstream::Header &header)
        : Parser::Event::_Header(parser, header) {}
};

struct Parser::Event::Payload::Data: Parser::Event::Payload {
    const bitstream::Blob &data;
    Data(Parser &parser, const bitstream::Header &header, const bitstream::Blob &data);
};

struct Parser::Event::Payload::Boundary: Parser::Event::Payload {
    struct Begin;
    struct End;
    struct Scope;
    Remainder &remainder;
    Boundary(Parser &parser, const bitstream::Header &header, Remainder &remainder)
        : Parser::Event::Payload(parser, header), remainder(remainder) {}
};

struct Parser::Event::Payload::Boundary::Begin: Parser::Event::Payload::Boundary {
    Begin(Parser &parser, const bitstream::Header &header, bitstream::Remainder &remainder);
};

struct Parser::Event::Payload::Boundary::End: Parser::Event::Payload::Boundary {
    End(Parser &parser, const bitstream::Header &header, bitstream::Remainder &remainder);
};

struct Parser::Event::Payload::Boundary::Scope: Parser::Event::Payload::Boundary {
    Scope(Parser &parser, const bitstream::Header &header, bitstream::Remainder &remainder)
        : Boundary(parser, header, remainder) {
        Begin{parser, header, remainder};
    }
    ~Scope() {
        End{parser, header, remainder};
    }
};

struct Parser::Observer {
    virtual void event(const Event::Exception &) {}
    virtual void event(const Event::Header &) {}
    virtual void event(const Event::Payload::Boundary::Begin &) {}
    virtual void event(const Event::Payload::Data &) {}
    virtual void event(const Event::Payload::Boundary::End &) {}

    template <typename Type>
    static const Type &cast_header(const bitstream::Parser::Event::Header &event) {
        return bitstream::dcast<const Type &>(event.header);
    }
};

inline Parser::Event::Exception::Exception(Parser &parser)
    : Parser::Event(parser) {
    parser.observer.event(*this);
}

inline Parser::Event::Header::Header::Header(Parser &parser, const bitstream::Header &header)
    : Parser::Event::_Header(parser, header) {
    parser.observer.event(*this);
}

inline Parser::Event::Payload::Boundary::Begin::Begin(Parser &parser, const bitstream::Header &header, bitstream::Remainder &remainder)
    : Boundary(parser, header, remainder) {
    parser.observer.event(*this);
}

inline Parser::Event::Payload::Data::Data(Parser &parser, const bitstream::Header &header, const bitstream::Blob &data)
    : Parser::Event::Payload(parser, header), data(data) {
    parser.observer.event(*this);
}

inline Parser::Event::Payload::Boundary::End::End(Parser &parser, const bitstream::Header &header, bitstream::Remainder &remainder)
    : Boundary(parser, header, remainder) {
    parser.observer.event(*this);
}


} // namespace bitstream


#endif // __BITSTREAM_PARSER_H__


#include <iomanip>
#include <cctype>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <bitstream/blob.h>
#include <bitstream/opstream.h>
#include <bitstream/omheader.h>
#include <bitstream/omstream.h>
#include <bitstream/sstream.h>
#include <bitstream/header.h>


namespace bitstream {
namespace output {
namespace print {
namespace indented {

Stream::Stream(std::ostream &out, std::ostream &err, bool indent_top_level)
    : out(out), err(err) {
    _indentation.reserve(40);
    if (indent_top_level) {
        indent(indentation);
    }
}

void Stream::indent(const std::string &pattern, bool in) {
    if (in) {
        _indentation += pattern;
    } else {
        _indentation.erase(_indentation.size() - pattern.size());
    }
}

std::ostream &Stream::ierr() {   // indented err
    err << _indentation;
    return err;
}

std::ostream &Stream::iout(const std::string &pattern) {   // indented out
    if (pattern.empty()) {
        out << _indentation;
    } else if (!_indentation.empty()) { // substitute end of indentation with pattern
        out << _indentation.substr(0, _indentation.size() - pattern.size()) << pattern;
    }
    return out;
}

}}}} // namespace bitstream::output::print::indented


namespace bitstream {
namespace output {
namespace print {


Stream to_stdout(std::cout, std::cerr);

bool Stream::ellipses(long item, long total_items) {
    if (total_items > (ellipses_items.before + ellipses_items.after) &&
            item == ellipses_items.before) {
        iout() << std::endl;
        iout() << "..." << std::endl;
        iout() << std::endl;
        return true;
    }
    if (ellipses_items.before <= item && item < total_items - ellipses_items.after) {
        return true;
    }
    return false;
}

void Stream::event(const Parser::Event::Exception &event) {
    std::ostream &err = (idented_errors ? ierr() : this->err);
    err << "Error: ";
    try {
        throw;
    } catch (const std::exception &e) {
        err << "caught parsing exception: "<< e.what();
    } catch (...) {
        err << "caught unknown/not parsing exception during parsing.";
    }
    err << std::endl;
}

void Stream::event(const Parser::Event::Payload::Boundary::Begin &event) {
    indent(indentation, true);
}

void Stream::event(const Parser::Event::Payload::Boundary::End &event) {
    indent(indentation, false);
}

struct Stream::Meta: meta::header::Stream, meta::field::Stream, meta::payload::Stream {
    print::Stream &ps;
    Meta(print::Stream &ps) : ps(ps) {}

    virtual bool ellipses(long index, long count) {
        return ps.ellipses(index, count);
    }

    virtual void header(const Tag &tag, const char *buffer) {
        auto &stream = ps.iout(ps.branching);
        stream << tag.name << " { // ";
        if (tag.offset != 0) {
            stream << tag.offset << "/";
        }
        stream << "[" << tag.size / 8;
        if (tag.size % 8 != 0) {
            stream << ":" << tag.size % 8;
        }
        stream <<" bytes]";
    }

    virtual void field(const Tag &tag,  uint8_t value) { _field(tag, value); }
    virtual void field(const Tag &tag, uint16_t value) { _field(tag, value); }
    virtual void field(const Tag &tag, uint32_t value) { _field(tag, value); }
    virtual void field(const Tag &tag, uint64_t value) { _field(tag, value); }
    virtual void field(const Tag &tag,   int8_t value) { _field(tag, value); }
    virtual void field(const Tag &tag,  int16_t value) { _field(tag, value); }
    virtual void field(const Tag &tag,  int32_t value) { _field(tag, value); }
    virtual void field(const Tag &tag,  int64_t value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::string &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector< uint8_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector<uint16_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector<uint32_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector<uint64_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector<  int8_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector< int16_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector< int32_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector< int64_t> &value) { _field(tag, value); }
    virtual void field(const Tag &tag, const std::vector<std::string> &value) { _field(tag, value); }

    template <typename Type>
    void _field(const Tag &tag, const Type &value) {
        registry[tag.formatters.get("field")].field(ps, tag, value);
    }

    virtual void payload(const Tag &tag, const bitstream::Blob &) {
        auto &stream = ps.iout();
        stream << tag.name << "[";
        stream << tag.size / 8 << " bytes";
        if (tag.size % 8 != 0) {
            stream << " and "  << tag.size % 8 << " bits";
        }
        stream << "]" << std::endl;
    }
};

struct Stream::Brackets {
    print::Stream &ps;
    const Parser::Event::Header &event;

    Brackets(print::Stream &ps, const Parser::Event::Header &event)
        :ps(ps), event(event) {
        ps.out << " @" << event.parser.stream.offset() << std::endl;
        ps.indent(ps.fields, true);
    }
    ~Brackets() {
        ps.indent(ps.fields, false);
        ps.iout() << "}" << std::endl;
    }
};

void Stream::event(const Parser::Event::Header &event) {
    const bitstream::output::meta::Header *header =
        dynamic_cast<const bitstream::output::meta::Header *>(&event.header);
    if (header) {
        Meta meta(*this);
        if (!header->output_ellipses(meta)) {
            header->output_header(meta);
            Brackets brackets(*this, event);
            header->output_fields(meta);
        }
    } else {
        iout(branching) << "<Header> // @" << event.parser.stream.offset() << std::endl;
    }
}

void Stream::event(const Parser::Event::Payload::Data &event) {

    Meta meta(*this);
    const bitstream::output::meta::Header *header =
        dynamic_cast<const bitstream::output::meta::Header *>(&event.header);
    if (header) {
        header->output_payload(meta, event.data, event.parser);
    } else {
        bitstream::output::meta::Stream::Tag tag = {
            .name = "<Payload>",
            .size = 8 * event.data.size()};
        meta.payload(tag, event.data);
    }
}



void fourcc(std::ostream &stream, const uint32_t value) {
    bool non_printed = false;
    for (int i = sizeof(value) - 1; i >= 0; --i) {
        char ch = char((value >> 8 * i) & uint32_t(0xFF));
        if (std::isprint(ch)) {
            stream << ch;
        } else {
            stream << '?';
            non_printed = true;
        }
    }
    if (non_printed) {
        stream << " (" << value << ")";
    }
}

std::string fourcc(const uint32_t value) {
    std::ostringstream sstream;
    fourcc(sstream, value);
    return sstream.str();
}

namespace formatter {


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { _field(stream, tag, value); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &value) {

        auto &out = stream.iout();
        if (stream.print_field_offset) {
            out << std::setw(stream.field_offset_width) << std::left;
            out << tag.offset << ": ";
        }
        out << std::setw(stream.type_width) << std::left;
        if (tag.type.empty()) {
            registry[tag.formatters.get("type")].field(stream, tag, value);
        } else {
            out << tag.type;
        }
        out << " ";
        out << std::setw(stream.name_width) << std::left;
        registry[tag.formatters.get("name")].field(stream, tag, value);
        out << ": ";
        registry[tag.formatters.get("value")].field(stream, tag, value);
        out << std::endl;
    }

} static field;


struct: Stream::Formatter {
    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { number_type(stream, tag, stream.unsigned_number); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { number_type(stream, tag, stream.unsigned_number); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { number_type(stream, tag, stream.unsigned_number); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { number_type(stream, tag, stream.unsigned_number); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { number_type(stream, tag, stream.signed_number); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { number_type(stream, tag, stream.signed_number); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { number_type(stream, tag, stream.signed_number); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { number_type(stream, tag, stream.signed_number); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) {
        stream.out << (const std::string &)(SStream() << stream.char_type << "[" << value.size() << "]");
    }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { array_type(stream, tag, stream.unsigned_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { array_type(stream, tag, stream.unsigned_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { array_type(stream, tag, stream.unsigned_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { array_type(stream, tag, stream.unsigned_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { array_type(stream, tag, stream.signed_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { array_type(stream, tag, stream.signed_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { array_type(stream, tag, stream.signed_number, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { array_type(stream, tag, stream.signed_number, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { array_type(stream, tag, stream.char_type + "*", value); }

    void number_type(Stream &stream, const Tag &tag, const std::string &type) {
        SStream ss;
        ss << type;
        if (tag.size != 0) {
            ss << tag.size;
        }
        stream.out << (const std::string &)ss;
    }

    template <typename Type>
    void array_type(Stream &stream, const Tag &tag, const std::string &type, const Type &array) {
        SStream ss;
        ss << type;
        if (tag.size != 0) {
            ss << tag.size;
        }
        ss << "[" << array.size() << "]";
        stream.out << (const std::string &)ss;
    }

} static type;


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream, "string"); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream, "array"); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &value) {
        stream.out << double(+value) / (1L << sizeof(value) * 8 / 2);
    }

    void error(Stream &stream, const char *type) {
        stream.out << "ERROR: number formatter is not applicable for '" << type << "' type.";
    }
} static fixed_point;

struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { _field(stream, tag, value); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &value) {
        stream.out << tag.name;
    }

} static name;



struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { number_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { string_field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { array_field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { array_field(stream, tag, value); }

    template <typename Type>
    void number_field(Stream &stream, const Tag &tag, const Type &value) {
        registry[tag.formatters.get("number")].field(stream, tag, value);
    }

    template <typename Type>
    void string_field(Stream &stream, const Tag &tag, const Type &value) {
        registry[tag.formatters.get("string")].field(stream, tag, value);
    }

    template <typename Type>
    void array_field(Stream &stream, const Tag &tag, const Type &value) {
        registry[tag.formatters.get("array", stream.default_array_formatter)].field(stream, tag, value);
    }

} static value;


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream, "string"); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream, "array"); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &value) {
        stream.out << +value;
    }

    void error(Stream &stream, const char *type) {
        stream.out << "ERROR: number formatter is not applicable for '" << type << "' type.";
    }

} static number;


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { error(stream, "number"); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) {
        _field(stream, tag, std::string(value.begin(), value.end()));
    }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) {
        _field(stream, tag, std::string(value.begin(), value.end()));
    }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { error(stream, "array"); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream, "array"); }

    void _field(Stream &stream, const Tag &tag, const std::string &value) {
        stream.out << '"';
        for (auto ch: value) {
            stream.out << (std::isprint(ch) ? ch: stream.non_printing_char);
        }
        stream.out << '"';
    }

    void error(Stream &stream, const char *type) {
        stream.out << "ERROR: string formatter is not applicable for '" << type << "' type.";
    }

} static string;


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { error(stream, "number"); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream, "string"); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { _field(stream, tag, value); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &array) {
        stream.out << "[";
        for (auto i = 0; i < array.size(); ++i){
            if (i != 0) {
                stream.out << ", ";
            }
            const auto &value = array[i];
            registry[tag.formatters.get("array_item")].field(stream, tag, value);
        }
        stream.out << "]";
    }

    void error(Stream &stream, const char *type) {
        stream.out << "ERROR: array formatter is not applicable for '" << type << "' type.";
    }

} static oneline_array;


struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { error(stream, "number"); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { error(stream, "number"); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream, "string"); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { _field(stream, tag, value); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &array) {
        stream.out << "[";
        {
            Stream::Indent indent(stream, stream.multiline_array_item_indent);
            for (auto i = 0; i < array.size(); ++i){
                if (i != 0) {
                    stream.out << ",";
                }
                stream.out << std::endl;
                stream.iout();
                const auto &value = array[i];
                registry[tag.formatters.get("array_item")].field(stream, tag, value);
            }
        }
        stream.out << std::endl;
        stream.iout() << "]";
    }

    void error(Stream &stream, const char *type) {
        stream.out << "ERROR: array formatter is not applicable for '" << type << "' type.";
    }

} static multiline_array;



template <typename T>
static std::string ISO_639_2_T_code(const std::vector<T> &array) {
    std::string code;
    for (auto i = 0; i < array.size(); ++i) {
        static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";//\0
        auto number = array[i];
        if (0 < number && number <= sizeof(alphabet) - 1) { // -1 -> '\0'
            code += alphabet[number - 1];
        } else {
            code += '?';
        }
    }
    return code;
}

static std::string ISO_639_2_T_name(const std::string &code) {
    static std::unordered_map<std::string, std::string> codes = {
        {"eng", "English"},
        {"und", "Undetermined"},
    };
    auto it = codes.find(code);
    return it != codes.end() ? it->second: "";
}

struct: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { error(stream); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream); }

    template <typename Type>
    void _field(Stream &stream, const Tag &tag, const Type &array) {

        if (array.size() != 3) {
            error(stream);
            return;
        }

        std::string code = ISO_639_2_T_code(array);
        if (code.find('?') != std::string::npos) {
            registry[tag.formatters.get("array")].field(stream, tag, array);
        } else {
            stream.out << code;
            std::string name = ISO_639_2_T_name(code);
            if (!name.empty()) {
                stream.out << " (" << name << ")";
            }
        }
    }

    void error(Stream &stream) {
        stream.out << "ERROR: ISO-639-2/T language code formatter is not applicable for any but numeric array types of length 3.";
    }

} static ISO_639_2_T;



template <typename Time>
std::string time1904_to_string(Time time, std::tm* (*convert_time)(const std::time_t * )) {
    using namespace std::chrono;
    static auto t1904 = system_clock::time_point() - duration<unsigned long>(2082844800); // 1904
    std::time_t t = system_clock::to_time_t(t1904 + duration<Time>(time));
    std::string ts = std::asctime(convert_time(&t));
    ts.resize(ts.size() - 1);   // strip trailing newline
    return ts;
}


struct Time1904: Stream::Formatter {

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { _field(stream, tag, value); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream); }

    template <typename Time>
    void _field(Stream &stream, const Tag &tag, Time time) {
        stream.out << time1904_to_string(time, convert_time);
    }

    void error(Stream &stream) {
        stream.out << "ERROR: time formatter is not applicable for any but numeric types.";
    }

    std::tm* (*convert_time)(const std::time_t * );

    Time1904(decltype(std::gmtime) convert_time)
        : convert_time(convert_time) {
    }

} static gmtime1904(std::gmtime), localtime1904(std::localtime);


struct: Stream::Formatter {
    using Stream = bitstream::output::print::Stream;

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) { _field(stream, tag, value); }
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) { error(stream); }

    virtual void field(Stream &stream, const Tag &tag, const std::string &value) { error(stream); }

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) { error(stream); }
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) { error(stream); }

    void _field(Stream &stream, const Tag &tag, uint32_t type) {
        fourcc(stream.out, type);
    }

    void error(Stream &stream) {
        stream.out << "ERROR: fourcc formatter is not applicable for any non-32bit types.";
    }

} static fourcc;
} // namespace formatter


Stream::Formatter& Stream::Formatter::Registry::operator [] (const std::string &name) {
    auto it = formatters.find(name);
    if (it == formatters.end()) {
        throw std::runtime_error(SStream() << "Couldn't find '" << name << "' formatter");
    }
    return *(it->second);
}

void Stream::Formatter::Registry::add(const std::string &name, Stream::Formatter &formatter) {
    formatters[name] = &formatter;
}

Stream::Formatter::Registry::Registry() {
    add("field", formatter::field);
    add("type", formatter::type);
    add("name", formatter::name);
    add("value", formatter::value);
    add("array_item", formatter::value);
    add("number", formatter::number);
    add("string", formatter::string);
    add("array", formatter::oneline_array);
    add("oneline_array", formatter::oneline_array);
    add("multiline_array", formatter::multiline_array);
    add("ISO-639-2/T language code", formatter::ISO_639_2_T);
    add("time since 1904", formatter::gmtime1904);
    add("local time since 1904", formatter::localtime1904);
    add("fourcc", formatter::fourcc);
    add("fixed-point", formatter::fixed_point);
}

Stream::Formatter::Registry registry;


std::string ISO_639_2_T_code(const std::vector<uint8_t> &language) {
    return formatter::ISO_639_2_T_code(language);
}

std::string ISO_639_2_T_name(const std::string &language_code) {
    return formatter::ISO_639_2_T_name(language_code);
}


std::string gtmtime1904_to_string(uint64_t time) {
    return formatter::time1904_to_string(time, std::gmtime);
}

std::string localtime1904_to_string(uint64_t time) {
    return formatter::time1904_to_string(time, std::localtime);
}


}}} // namespace bitstream::output::print


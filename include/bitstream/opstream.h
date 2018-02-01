#ifndef __BITSTREAM_OPSTREAM_H__
#define __BITSTREAM_OPSTREAM_H__


#include <iostream>
#include <unordered_map>
#include <bitstream/parser.h>
#include <bitstream/omstream.h>
#include <bitstream/signedness.h>


namespace bitstream {
namespace output {
namespace print {
namespace indented {

struct Stream {
    std::string indentation = "|  ";
    std::string branching   = "+- ";


    struct Indent;
    void indent(const std::string &pattern, bool in = true);

    std::ostream &ierr();                                   // indented err
    std::ostream &iout(const std::string &pattern = "");    // indented out

    std::ostream &out;
    std::ostream &err;

    Stream(std::ostream &out, std::ostream &err, bool indent_top_level = true);

private:
    std::string _indentation;
};


struct Stream::Indent {
    Stream &stream;
    std::string indent;
    Indent(Stream &stream, const std::string &indent) : stream(stream), indent(indent) {
        stream.indent(indent, true);
    }
    Indent(Stream &stream) : Indent(stream, stream.indentation) { }
    ~Indent() {
        stream.indent(indent, false);
    }
};

}}}} // namespace bitstream::output::print


namespace bitstream {
namespace output {
namespace print {


struct Stream: indented::Stream, Parser::Observer {

    std::string fields      = "    ";

    bool idented_errors     = true;
    bool print_payloads     = true;
    bool print_field_offset = false;

    long type_width = 10;
    long name_width = 20;
    long field_offset_width = 3;
    struct {
        long before = 5;
        long after = 5;
    } ellipses_items;

    std::string unsigned_number = Signedness<unsigned>::string;
    std::string signed_number   = Signedness<signed>::string;
    std::string char_type       = "char";
    char non_printing_char      = '?';
    // <empty> == "array" and "array == "oneline_array" by default
    std::string default_array_formatter;// array | oneline_array | multiline_array

    std::string multiline_array_item_indent = std::string(4, ' ');

    bool ellipses(long item, long total_items);

    struct Formatter;
    using indented::Stream::Stream;

private:
    struct Brackets;
    struct Meta;

protected:  // Parser::Observer interface implementation
    virtual void event(const Parser::Event::Exception &);
    virtual void event(const Parser::Event::Payload::Data &event);
    virtual void event(const Parser::Event::Payload::Boundary::Begin &event);
    virtual void event(const Parser::Event::Payload::Boundary::End &event);
    virtual void event(const Parser::Event::Header &event);

} extern to_stdout;


struct Stream::Formatter {
    using Tag = meta::Stream::Tag;
    struct Registry;

    virtual void field(Stream &stream, const Tag &tag,  uint8_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag, uint16_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag, uint32_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag, uint64_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag,   int8_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag,  int16_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag,  int32_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag,  int64_t value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::string &value) = 0;

    virtual void field(Stream &stream, const Tag &tag, const std::vector< uint8_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint16_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint32_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector<uint64_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector<  int8_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int16_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int32_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector< int64_t> &value) = 0;
    virtual void field(Stream &stream, const Tag &tag, const std::vector<std::string> &value) = 0;
};

struct Stream::Formatter::Registry {
    Registry();
    Stream::Formatter& operator [] (const std::string &name);
    void add(const std::string &name, Stream::Formatter& formatter);
private:
    std::unordered_map<std::string, Stream::Formatter *> formatters;
} extern registry;


std::string ISO_639_2_T_code(const std::vector<uint8_t> &language);
std::string ISO_639_2_T_name(const std::string &language_code);

std::string gtmtime1904_to_string(uint64_t time);
std::string localtime1904_to_string(uint64_t time);

std::string fourcc(const uint32_t value);


inline static std::string plural(const std::string &word) {
    struct Enum: std::string {
        using std::string::string;
        bool has(char ch) { return find(ch) != npos; }
    } static vowel = "aeiou";

    if (word.empty()) {
        return word;
    } else if (word.back() == 'y' && !(word.size() > 1 && vowel.has(*(word.begin() + 1)))) {
        return word.substr(0, word.size() - 1) + "ies";
    } else if (Enum("oszx").has(word.back())) {
        return word + "es";
    }

    return word + 's';
}


inline static std::string pluralize(const std::string &word, long count) {
    return count == 1 ? word : plural(word);
}


}}} // namespace bitstream::output::print


#endif // __BITSTREAM_OPSTREAM_H__


#ifndef __BITSTREAM_OMSTREAM_H__
#define __BITSTREAM_OMSTREAM_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>


namespace bitstream {

struct Blob;

} // namespace bitstream


namespace bitstream {
namespace output {
namespace meta {


struct Stream {
    struct Tag;
};


struct Stream::Tag {
    std::string name;
    unsigned long size = 0;     // Bits
    unsigned long offset = 0;   // Bits
    std::string type;

    struct: std::unordered_map<std::string, std::string> {
        std::string get(const std::string &name, const std::string &default_name = "") const {
            auto it = find(name);
            return (it != end()) ? it->second : (default_name.empty() ? name: default_name);
        }
    } formatters;
};


namespace header {

struct Stream: meta::Stream {
    virtual bool ellipses(long index, long count) = 0;
    virtual void header(const Tag &tag, const char *buffer) = 0;
};

} // namespace header


namespace field {

struct Stream: meta::Stream {

    virtual void field(const Tag &tag,  uint8_t value) = 0;
    virtual void field(const Tag &tag, uint16_t value) = 0;
    virtual void field(const Tag &tag, uint32_t value) = 0;
    virtual void field(const Tag &tag, uint64_t value) = 0;
    virtual void field(const Tag &tag,   int8_t value) = 0;
    virtual void field(const Tag &tag,  int16_t value) = 0;
    virtual void field(const Tag &tag,  int32_t value) = 0;
    virtual void field(const Tag &tag,  int64_t value) = 0;
    virtual void field(const Tag &tag, const std::string &value) = 0;

    virtual void field(const Tag &tag, const std::vector< uint8_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector<uint16_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector<uint32_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector<uint64_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector<  int8_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector< int16_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector< int32_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector< int64_t> &value) = 0;
    virtual void field(const Tag &tag, const std::vector<std::string> &value) = 0;
};

} // namespace field

namespace payload {

struct Stream: meta::Stream {
    virtual void payload(const Tag &tag, const bitstream::Blob &) = 0;
};

} // namespace payload



}}} // namespace bitstream::output::meta


#endif // __BITSTREAM_OMSTREAM_H__


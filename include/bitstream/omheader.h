#ifndef __BITHEADER_OMHEADER_H__
#define __BITHEADER_OMHEADER_H__

#include <stdint.h>
#include <string>
#include <vector>


namespace bitstream { struct Parser; struct Blob; }


namespace bitstream {
namespace output {
namespace meta {


namespace header  { struct Stream; }
namespace field   { struct Stream; }
namespace payload { struct Stream; }


struct Header {

    virtual ~Header() {}

    virtual bool output_ellipses(bitstream::output::meta::header::Stream &) const { return false; }
    virtual void output_header(bitstream::output::meta::header::Stream &) const = 0;
    virtual void output_fields(bitstream::output::meta::field::Stream &) const = 0;
    virtual void output_payload(bitstream::output::meta::payload::Stream &, const bitstream::Blob &, bitstream::Parser &) const {}
};


}}} // namespace bitstream::output::meta


#endif // __BITHEADER_OMHEADER_H__


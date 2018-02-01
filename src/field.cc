#include <cassert>
#include <bitstream/field.h>


namespace bitstream {
namespace Const {


void assert_verification(void *) {
    assert(false);
}

Verification verification = {
    .skip= true,
    .failed = assert_verification,
    .context = nullptr
};


}} // namespace bitstream::Const


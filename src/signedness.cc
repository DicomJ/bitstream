#include <bitstream/signedness.h>


namespace bitstream {


//template <>
const char *Signedness<signed>::string = "int";


//template <>
const char *Signedness<unsigned>::string = "uint";


} // namespace bitstream


#ifndef __BITSTREAM_DCAST_H__
#define __BITSTREAM_DCAST_H__


namespace bitstream {

template <typename ToType, typename BaseType>
static ToType &dcast(BaseType &value) {
    return
        #ifdef NDEBUG
        static_cast
        #else
        dynamic_cast
        #endif
                    <ToType &>(value);
}

} // namespace bitstream

#endif // __BITSTREAM_DCAST_H__


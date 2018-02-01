#ifndef __BITSTREAM_SCALABLE_TIME__
#define __BITSTREAM_SCALABLE_TIME__


namespace bitstream {
namespace scalable {


struct Time {
    uint32_t scale;
    uint64_t time;

    Time() {}
    Time(uint64_t time, uint32_t scale): time(time), scale(scale) {}


    //operator double() const { return seconds(); }
    double seconds() const { return double(time) / scale; }

    bool operator == (const Time &t) const {
        return scale == t.scale ? time == t.time : time * t.scale == scale * t.time;
    }

    Time operator + (const Time &t) const {
        return scale == t.scale ?
            Time{time + t.time, scale} :
            Time{time * t.scale + t.time * scale, scale * t.scale};
    }

    Time operator / (uint32_t value) const { return Time{time/value, scale}; }

    Time scaled(uint32_t new_scale) const {
        return scale == new_scale ?
            *this :
            Time{time * new_scale / scale, new_scale};
    }


    template <typename Stream>
    void operator >> (Stream &stream) const {
        stream << time << "/" << scale << "=" << seconds() << "s";
    }

    template <typename Stream>
    friend auto &operator << (Stream &stream, const Time &time) {
        return time >> stream, stream;
    }

};

}} // namespace bitstream::scalable


#endif // __BITSTREAM_SCALABLE_TIME__


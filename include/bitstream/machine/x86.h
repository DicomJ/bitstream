#ifndef __BITSTREAM_MACHINE_X86_H__
#define __BITSTREAM_MACHINE_X86_H__


namespace bitstream {
namespace machine {
namespace integral {


namespace aligned {

template <long size, typename signedness, long = (size <= 0? 0: size <= 8? 8: size <= 16? 16: size <= 32? 32: size <= 64? 64: 0)>
class Type; // Not defined error means size is not allowed

template <long size> struct Type<size, unsigned, 8 > { using type_t =  uint8_t; };
template <long size> struct Type<size, unsigned, 16> { using type_t = uint16_t; };
template <long size> struct Type<size, unsigned, 32> { using type_t = uint32_t; };
template <long size> struct Type<size, unsigned, 64> { using type_t = uint64_t; };

template <long size> struct Type<size,   signed, 8 > { using type_t =   int8_t; };
template <long size> struct Type<size,   signed, 16> { using type_t =  int16_t; };
template <long size> struct Type<size,   signed, 32> { using type_t =  int32_t; };
template <long size> struct Type<size,   signed, 64> { using type_t =  int64_t; };


} // namespace aligned


template <long size, typename signedness>
struct Type {
    using type_t = typename aligned::Type<size, signedness>::type_t;
};


}}} // namespace bitstream::machine::integral


namespace bitstream {
namespace machine {


template <long i, typename T>
static T byte(const T &data) {
    return data & (T(0xFF) << 8 * i);
}

template <long first, long last, long distance = last - first>
struct Flip {
    template <typename T>
    static T bytes(const T &data) {
        return  (byte<first>(data) << 8 * distance)  |
                Flip<first + 1, last - 1>::bytes(data) |
                (byte<last>(data) >> 8 * distance);
    }
};

template <long first, long last>
struct Flip<first, last, 0> {
    template <typename T>
    static T bytes(const T &data) {
        return byte<first>(data);
    }
};

template <long first, long last>
struct Flip<first, last, -1> {
    template <typename T>
    static T bytes(const T &data) {
        return 0;
    }
};

template <bool flip>
struct Flippable;

template <>
struct Flippable<false> {
    template <long count, typename T>
    static T flip(const T &data) {
        return data;
    }
};

template <>
struct Flippable<true> {
    template <long count, typename T>
    static T flip(const T &data) {
        return Flip<0, count - 1>::template bytes(data);
    }
};


namespace constant {


template <long i, typename T>
struct Byte {
    static const T mask = T(0xFF) << 8 * i;
};

template <typename T, T value, long count,
          long first = 0, long last = count - 1, long distance = last - first>
struct Flip {
    static const T bytes =
        ((Byte<first, T>::mask & value) << 8 * distance)  |
        Flip<T, value, count, first + 1, last - 1>::bytes |
        ((Byte<last, T>::mask & value) >> 8 * distance);
};

template <typename T, T value, long count, long first, long last>
struct Flip<T, value, count, first, last, 0> {
    static const T bytes = Byte<first, T>::mask & value;
};

template <typename T, T value, long count, long first, long last>
struct Flip<T, value, count, first, last, -1> {
    static const T bytes = 0;
};

template <bool flip>
struct Flippable;

template <>
struct Flippable<false> {
    template <typename T, T value, long count>
    struct flip {
        static const T bytes = value;
    };
};

template <>
struct Flippable<true> {
    template <typename T, T value, long count>
    struct flip {
        static const T bytes = Flip<T, value, count>::bytes;
    };
};


} // namespace constant


template<long offset>
struct at {
    template <typename T>
    static T of(const T &buffer) {
        return buffer + offset;
    }
};

template <>
struct at<0> {
    template <typename T>
    static T of(const T &buffer) {
        return buffer;
    }
};

template <long offset>
struct Memory {

    template <typename T>
    static T get(const char *buffer) {
        return *reinterpret_cast<const T *>(at<offset>::of(buffer));
    }

    template <typename T>
    static void set(char *buffer, const T &value) {
        *reinterpret_cast<T *>(at<offset>::of(buffer)) = value;
    }
};

template <long bits>
struct shift {

    template <typename T>
    static T right(const T &value) {
        return T(value >> bits);
    }

    template <typename T>
    static T left(const T &value) {
        return T(value << bits);
    }
};

template <>
struct shift<0> {

    template <typename T>
    static T right(const T &value) {
        return value;
    }

    template <typename T>
    static T left(const T &value) {
        return value;
    }
};


template <long offset, long size, Endianness endianness,
         typename type = typename integral::Type<size, unsigned>::type_t>
struct Value {

    static const long lbits = offset % 8;
    static const long xsize = lbits  + size;
    static const long rbits = (8 - xsize % 8) % 8;
    static const long  bytes =  size / 8 + ( size % 8 ? 1: 0);
    static const long xbytes = xsize / 8 + (xsize % 8 ? 1: 0);
    static const long lsb = endianness == Endianness::little? lbits: rbits;
    static const long msb = endianness == Endianness::big   ? lbits: rbits;
    static const bool extended = xsize > 64;
    static const long word_xsize = extended ? 64 : xsize;

    using  word_t = typename integral::Type<word_xsize, unsigned>::type_t;

    static const word_t mask = ~word_t(0);
    static const word_t mask_size  = mask >> (8 * sizeof(word_t) - size);

    static const bool do_flip = machine::endianness != endianness;
    static const long word_alignment = (
        ((machine::endianness == Endianness::big) ||
        (endianness == Endianness::big && extended)) ? (xbytes - sizeof(word_t)) : 0);


    template <bool extended = (xsize > 64), typename U = void>
    struct Extendable;

    template <typename U>
    struct Extendable<false, U> {

        static const word_t mask_mlsb = word_t(~(mask_size << lsb));
        static const word_t mask_ixbytes = word_t(~(mask >> 8 * (sizeof(word_t) - xbytes)));
        static const word_t mask_fmlsb =  constant::Flippable<do_flip>::template flip<word_t, mask_mlsb, xbytes>::bytes | mask_ixbytes;

        static type get(const char *buffer) {
            word_t word = Memory<word_alignment>::template get<word_t>(buffer);
            word = Flippable<do_flip>::template flip<xbytes>(word);
            word = shift<lsb>::right(word);
            word &= mask_size;
            return word;
        }

        static void set(char *buffer, type value) {
            word_t _word = value;
            _word &= mask_size;
            _word = shift<lsb>::left(_word);
            _word = Flippable<do_flip>::template flip<xbytes>(_word);

            word_t word = Memory<word_alignment>::template get<word_t>(buffer);
            word &= mask_fmlsb;
            word |= _word;
            Memory<word_alignment>::set(buffer, word);
        }
    };

    template <typename U>
    struct Extendable<true, U> {

        static const long extension_alignment = endianness == Endianness::little ? + sizeof(word_t): 0;
        using extension_t = typename integral::Type<8, unsigned>::type_t;
        static const long elsb = (8 * sizeof(word_t) - lsb);
        static const extension_t mask_msb = extension_t(mask << (8 - msb));
        static const word_t mask_lsb = mask >> elsb;
        static const word_t mask_flsb = do_flip ? (mask_lsb << 8 * (sizeof(word_t) - sizeof(extension_t))): mask_lsb;


        static type get(const char *buffer) {
            word_t word = Memory<word_alignment>::template get<word_t>(buffer);
            word = Flippable<do_flip>::template flip<sizeof(word_t)>(word);
            word = shift<lsb>::right(word);

            word_t extension = Memory<extension_alignment>::template get<extension_t>(buffer);
            extension &= ~mask_msb;
            extension = shift<elsb>::left(extension);
            word |= extension;
            return word;
        }

        static void set(char *buffer, type value) {
            word_t _word = value;
            _word &= mask_size;
            extension_t _extension = shift<elsb>::right(_word);
            _word = shift<lsb>::left(_word);
            _word = Flippable<do_flip>::template flip<sizeof(word_t)>(_word);

            word_t word = Memory<word_alignment>::template get<word_t>(buffer);
            word &= mask_flsb;
            word |= _word;
            Memory<word_alignment>::set(buffer, word);

            extension_t extension = Memory<extension_alignment>::template get<extension_t>(buffer);
            extension &= mask_msb;
            extension |= _extension;
            Memory<extension_alignment>::set(buffer, extension);
        }
    };


    static type get(const char *buffer) {
        return Extendable<>::get(at<offset / 8>::of(buffer));
    }

    static void set(char *buffer, type value) {
        Extendable<>::set(at<offset / 8>::of(buffer), value);
    }
};


}} // namespace bitstream::machine


#endif // __BITSTREAM_MACHINE_X86_H__


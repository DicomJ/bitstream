#include <gtest/gtest.h>
#include <bitstream/field.h>


using namespace bitstream;
template <Endianness endianness>


struct EndiannessStr;

template <>
struct EndiannessStr<Endianness::little> {
    static const char *name() {
        return "little";
    }
};

template <>
struct EndiannessStr<Endianness::big> {
    static const char *name() {
        return "big";
    }
};

template <typename signedness>
struct SignednessStr;

template <>
struct SignednessStr<signed> {
    static const char *name() {
        return "signed";
    }
};

template <>
struct SignednessStr<unsigned> {
    static const char *name() {
        return "unsigned";
    }
};

struct Exception {
    long size, offset;
};

void memdump(const char *data, long size) {
    for (auto i = 0; i < size; ++i) {
        if (0 == i % sizeof(uint64_t)) printf("| ");
        printf("%02x ", uint8_t(data[i]));
    } printf("\n");
}

template <typename T>
void hp(const T &v, const char *t=0) {
    if (t) printf(t);
    if (sizeof(v) == 1) {
        printf("%02x", v);
    } else if (sizeof(v) == 2) {
        printf("%04x", v);
    } else if (sizeof(v) == 4) {
        printf("%08x", v);
    } else if (sizeof(v) == 8) {
        printf("%016lx", v);
    }
}

template <typename T>
void hpn(const T &v, const char *t=0) {
    if (t) printf("%s", t);
    if (sizeof(v) == 1) {
        printf("%x", uint8_t(v));
    } else if (sizeof(v) == 2) {
        printf("%x", uint16_t(v));
    } else if (sizeof(v) == 4) {
        printf("%x", uint32_t(v));
    } else if (sizeof(v) == 8) {
        printf("%lx", uint64_t(v));
    }
}


template <Endianness endianness, typename signedness, long offset=1, long size=
    #ifdef COMPREHENSIVE_TESTING
        1
    #else
        8 * sizeof(uint64_t) - 2
    #endif
    , bool max_size = (size > 8 * sizeof(uint64_t)), bool max_offset = offset == 8>
struct test {

    using Field = bitstream::Field<size, offset, endianness, signedness>;

    using type = typename Field::type;
    using utype = typename Field::utype;

    static const utype mask = utype(type(-1)) >> (8 * sizeof(type) - size);
    static const utype sign = utype(1) << (size - 1);

    template <typename signedness_=signedness, typename U = void>
    struct SignExtension;

    template <typename U>
    struct SignExtension<signed, U> {
        static void check(const type &value, const type &value_back) {
            if ((value & sign) && (value | ~mask) != value_back) {
                //printf("signs ext failed\n");
                throw false;
            }
        }
    };

    template <typename U>
    struct SignExtension<unsigned, U> {
        static void check(const type &value, const type &value_back) {
            if ((value_back  & ~mask) != 0) {
                throw false;
            }
        }
    };

    static void against(const type &value, char background = 0) {
        char data[3 * sizeof(uint64_t)]; for (auto i = 0; i < sizeof(data); ++i) data[i] = background;
        memdump(data, sizeof(data));
        Field field(data + sizeof(data) / 3);
        field = value;
        memdump(data, sizeof(data));
        const type value_back = field;
        try {
            SignExtension<>::check(value, value_back);
            if (value != type(value_back & mask)) {
                throw false;
            }
        } catch (...) {
            printf("Failed: size = %ld, offset = %ld (endianness: %s, signedness: %s)\n",
                    size, offset, EndiannessStr<endianness>::name(), SignednessStr<signedness>::name());
            hpn(value, " value to be set = ");
            hpn(value_back, " value obtained back = ");
            printf("\n");
            throw;
        }
    }

    static void execute() {
        utype values[] = {
            utype(0xef123456789abcd & mask),
            utype(1) << size - 1,
            1,
            0,
            utype(type(-1)) & mask
        };
        char backgrounds[] = {
            0,
            -1,
            0x55,
            char(0xAA)
        };
        for (auto value: values) {
            for (auto background: backgrounds) {
                against(value, background);
            }
            printf("\n");
        }

        test<endianness, signedness, offset, size + 1>::execute();
    }
};


template <Endianness endianness, typename signedness, long offset, long size, bool max_offset>
struct test<endianness, signedness, offset, size, true, max_offset> {
    static void execute() {
        test<endianness, signedness, offset + 1>::execute();
    }
};

template <Endianness endianness, typename signedness, long offset, long size, bool max_size>
struct test<endianness, signedness, offset, size, max_size, true> { static void execute() {} };

TEST(Field, comprehensively) {

    try {
        test<Endianness::big, unsigned>::execute();
        test<Endianness::big, signed>::execute();
        test<Endianness::little, unsigned>::execute();
        test<Endianness::little, signed>::execute();
    } catch (...) {
        ASSERT_TRUE(false);
    }
}


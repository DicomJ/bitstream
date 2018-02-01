#ifndef __BITSTREAM_ARRAY_H__
#define __BITSTREAM_ARRAY_H__

#include <cassert>
#include <vector>
#include <bitstream/field.h>


namespace bitstream {

namespace Static {


template <long size, long offset, Endianness endianness, typename signedness, long items_>
struct Array<bitstream::Field<size, offset, endianness, signedness>, items_>: Footprint<size * items_, offset> {

    using Footprint<size * items_, offset>::Footprint;
    static const long items = items_;

    template <long offset_ = offset>
    using Field = bitstream::Field<size, offset_, endianness, signedness>;
    using Type = typename Field<>::type;
    using Vector = std::vector<Type>;


    template <bool aligned = offset % 8 == 0 && size % 8 == 0, typename U = void>
    struct Item {
        char *buffer;
        long offset_;
        Item(char *buffer, long index)
            : buffer(buffer + (offset + index * size) / 8)
            , offset_(        (offset + index * size) % 8) {
            assert(items == 0 || index < items);
        }

        operator Type() const {
            switch (offset_) {
            case 0: return Field<0>{buffer}; break;
            case 1: return Field<1>{buffer}; break;
            case 2: return Field<2>{buffer}; break;
            case 3: return Field<3>{buffer}; break;
            case 4: return Field<4>{buffer}; break;
            case 5: return Field<5>{buffer}; break;
            case 6: return Field<6>{buffer}; break;
            case 7: return Field<7>{buffer}; break;
            default:
                assert(offset_ < 8);
                return Type(0);
            }
        }

        Item &operator = (Type value) {
            switch (offset_) {
            case 0: Field<0>{buffer} = value; break;
            case 1: Field<1>{buffer} = value; break;
            case 2: Field<2>{buffer} = value; break;
            case 3: Field<3>{buffer} = value; break;
            case 4: Field<4>{buffer} = value; break;
            case 5: Field<5>{buffer} = value; break;
            case 6: Field<6>{buffer} = value; break;
            case 7: Field<7>{buffer} = value; break;
            }
            return *this;
        }
    };

    template <typename U>
    struct Item<true, U> {
        char *buffer;
        Item(char *buffer, long index)
            : buffer(buffer + (offset + index * size) / 8) {
            assert(items == 0 || index < items);
        }

        operator Type() const {
            return Field<0>{buffer};
        }
        Item &operator = (Type value) {
            Field<0>{buffer} = value;
            return *this;
        }
    };

    Item<> operator[](long index) {
        return Item<>(this->buffer(), index);
    }
    const Item<> operator[](long index) const {
        return Item<>(const_cast<char*>(this->buffer()), index);
    }

    // TODO: add set method based on runtime index

    template <long index>
    const Field<offset + index * size> at() const {
        AssertRange<index> assert;
        return this->buffer();
    }

    template <long index>
    Field<offset + index * size> at() {
        AssertRange<index> assert;
        return this->buffer();
    }

    operator typename Array::Vector () const {
        typename Array::Vector v;
        v.reserve(items);
        for (auto i = 0; i < items; ++i) {
            v.push_back((*this)[i]);
        }
        return v;
    }

    template <typename Container>
    bool operator == (const Container &v) const {
        if (v.size() != items) {
            return false;
        }
        auto i = 0;
        for (const auto &value: v) {
            if (static_cast<Type>((*this)[i++]) != static_cast<Type>(value)) {
                return false;
            }
        }
        return true;
    }

    template <typename Container>
    auto &operator = (const Container &v) {
        assert(v.size() == items);
        auto i = 0;
        for (const auto &value: v) {
            (*this)[i++] = static_cast<Type>(value);
        }
        return *this;
    }

private:
    template <long index, bool exceeds = (index != 0 && index >= items)>
    struct AssertRange {};

    template <long index>
    struct AssertRange<index, true>; // Exceeds the index
};

} // namespace Static;


template <long size, long offset, Endianness endianness, typename signedness>
struct Array<Field<size, offset, endianness, signedness>>: Static::Array<Field<size, offset, endianness, signedness>, 0> {

    using Type = typename Static::Array<Field<size, offset, endianness, signedness>, 0>::Type;

    unsigned long items;
    Array() {}
    Array(const char *buffer, unsigned long items = 0)
        : Static::Array<Field<size, offset, endianness, signedness>, 0>(buffer), items(items) {}

    operator typename Array::Vector () const {
        typename Array::Vector v;
        v.reserve(items);
        for (auto i = 0; i < items; ++i) {
            v.push_back((*this)[i]);
        }
        return v;
    }

    template <typename Container>
    bool operator == (const Container &v) const {
        if (v.size() != items) {
            return false;
        }
        auto i = 0;
        for (const auto &value: v) {
            if (static_cast<Type>((*this)[i++]) != static_cast<Type>(value)) {
                return false;
            }
        }
        return true;
    }

    template <typename Container>
    auto &operator = (const Container &v) {
        assert(v.size() == items);
        auto i = 0;
        for (const auto &value: v) {
            (*this)[i++] = static_cast<Type>(value);
        }
        return *this;
    }
};


} // namespace bitstream


#endif // __BITSTREAM_ARRAY_H__


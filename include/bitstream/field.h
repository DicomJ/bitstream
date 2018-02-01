#ifndef __BITSTREAM_FIELD_H__
#define __BITSTREAM_FIELD_H__

#include <stdint.h>
#include <initializer_list>
#include <bitstream/footprint.h>
#include <bitstream/machine.h>

namespace bitstream {


namespace Static {

template <typename Field, long items>
struct Array;

} // namespace Static


template <typename Field>
struct Array;


} // namespace bitstream


namespace bitstream {
namespace cached {
namespace field {


template <typename Field>
struct Reference;


template <typename Field>
struct Value;


}}} // namespace bitstream::cached::field


namespace bitstream {


template <long size, long offset, Endianness endianness_, typename signedness_>
struct Field: Footprint<size, offset> {

    using signedness = signedness_;
    static const Endianness endianness = endianness_;

    using  type = typename machine::integral::Type<size, signedness>::type_t;
    using utype = typename machine::integral::Type<size, unsigned   >::type_t;
    using stype = typename machine::integral::Type<size, signed     >::type_t;

    using Footprint<size, offset>::Footprint;

    template <typename T>
    operator T () const {
        return static_cast<typename Field::type>(
            Sign::template Extension<>::cast(
            machine::Value<offset % 8, size, endianness>::get(Field::buffer() + offset / 8)));
    }

    template <typename T>
    Field &operator = (const T &value) {
        machine::Value<offset % 8, size, endianness>::set(
            Field::buffer() + offset / 8,
            static_cast<typename Field::type>(value));
        return *this;
    }

    type value() const { return *this; }
    Field &value(const type &value_) const { *this = value_; return *this; }

    template <typename T>
    bool operator == (const T &value) const {
        return this->value() == value;
    }

    template <typename T>
    friend bool operator == (const T& lhf, const Field& rhf) {
        return lhf == rhf.value();
    }

    template <typename T>
    bool operator != (const T &value) const {
        return this->value() != value;
    }

    template <typename T>
    friend bool operator != (const T& lhf, const Field& rhf) {
        return lhf != rhf.value();
    }

    template <typename T>
    bool operator > (const T &value) const {
        return this->value() > value;
    }

    template <typename T>
    friend bool operator > (const T& lhf, const Field& rhf) {
        return lhf > rhf.value();
    }

    template <typename T>
    bool operator >= (const T &value) const {
        return this->value() >= value;
    }

    template <typename T>
    friend bool operator >= (const T& lhf, const Field& rhf) {
        return lhf >= rhf.value();
    }

    template <typename T>
    bool operator < (const T &value) const {
        return this->value() < value;
    }

    template <typename T>
    friend bool operator < (const T& lhf, const Field& rhf) {
        return lhf < rhf.value();
    }

    template <typename T>
    bool operator <= (const T &value) const {
        return this->value() <= value;
    }

    template <typename T>
    friend bool operator <= (const T& lhf, const Field& rhf) {
        return lhf <= rhf.value();
    }

    template <typename T>
    auto operator + (const T &value) const -> decltype(this->value() + value) {
        return this->value() + value;
    }

    template <typename T>
    friend auto operator + (const T& lhf, const Field& rhf) -> decltype(lhf + Field::type(0)) {
        return lhf + rhf.value();
    }

    template <typename T>
    Field &operator += (const T &value) {
        return *this = this->value() + value;
    }

    template <typename T>
    auto operator - (const T &value) const -> decltype(this->value() - value) {
        return this->value() - value;
    }

    template <typename T>
    friend auto operator - (const T& lhf, const Field& rhf) -> decltype(lhf - Field::type(0)) {
        return lhf - rhf.value();
    }

    template <typename T>
    Field &operator -= (const T &value) {
        return *this = this->value() - value;
    }

    template <typename T>
    Field &operator ++ () {
        return *this = this->value() + 1;
    }

    template <typename T>
    type operator ++ (int) {
        type value = this->value();
        *this = this->value() + 1;
        return value;
    }

    template <typename T>
    Field &operator -- () {
        return *this = this->value() - 1;
    }

    template <typename T>
    type operator -- (int) {
        type value = this->value();
        *this = this->value() - 1;
        return value;
    }

    template <typename T>
    auto operator * (const T &value) const -> decltype(this->value() * value) {
        return this->value() * value;
    }

    template <typename T>
    friend auto operator * (const T& lhf, const Field& rhf) -> decltype(lhf * Field::type(1)) {
        return lhf * rhf.value();
    }

    template <typename T>
    Field &operator *= (const T &value) {
        return *this = this->value() * value;
    }

    template <typename T>
    auto operator / (const T &value) const -> decltype(this->value() / value) {
        return this->value() / value;
    }

    template <typename T>
    friend auto operator / (const T& lhf, const Field& rhf) -> decltype(lhf / Field::type(1)) {
        return lhf / rhf.value();
    }

    template <typename T>
    Field &operator /= (const T &value) {
        return *this = this->value() / value;
    }

    template <typename T>
    auto operator % (const T &value) const -> decltype(this->value() % value) {
        return this->value() % value;
    }

    template <typename T>
    friend auto operator % (const T& lhf, const Field& rhf) -> decltype(lhf % Field::type(0)) {
        return lhf % rhf.value();
    }

    template <typename T>
    Field &operator %= (const T &value) {
        return *this = this->value() % value;
    }

    template <typename T>
    auto operator & (const T &value) const -> decltype(this->value() & value) {
        return this->value() & value;
    }

    template <typename T>
    friend auto operator & (const T& lhf, const Field& rhf) -> decltype(lhf & Field::type(0)) {
        return lhf & rhf.value();
    }

    template <typename T>
    Field &operator &= (const T &value) {
        return *this = this->value() & value;
    }

    template <typename T>
    auto operator | (const T &value) const -> decltype(this->value() | value) {
        return this->value() | value;
    }

    template <typename T>
    friend auto operator | (const T& lhf, const Field& rhf) -> decltype(lhf | Field::type(0)) {
        return lhf | rhf.value();
    }

    template <typename T>
    Field &operator |= (const T &value) {
        return *this = this->value() | value;
    }

    template <typename T>
    auto operator ^ (const T &value) const -> decltype(this->value() ^ value) {
        return this->value() ^ value;
    }

    template <typename T>
    friend auto operator ^ (const T& lhf, const Field& rhf) -> decltype(lhf ^ Field::type(0)) {
        return lhf ^ rhf.value();
    }

    template <typename T>
    Field &operator ^= (const T &value) {
        return *this = this->value() ^ value;
    }

    template <typename T>
    auto operator ~ () const -> decltype(~(this->value())) {
        return ~(this->value());
    }

    template <typename T>
    auto operator >> (const T &value) const -> decltype(this->value() >> value) {
        return this->value() >> value;
    }

    template <typename T>
    friend auto operator >> (const T& lhf, const Field& rhf) -> decltype(lhf >> Field::type(0)) {
        return lhf >> rhf.value();
    }

    template <typename T>
    Field &operator >>= (const T &value) {
        return *this = this->value() >> value;
    }

    template <typename T>
    auto operator << (const T &value) const -> decltype(this->value() << value) {
        return this->value() << value;
    }

    template <typename T>
    friend auto operator << (const T& lhf, const Field& rhf) -> decltype(lhf) {
        return lhf << rhf.value();
    }

    template <typename T>
    Field &operator <<= (const T &value) {
        return *this = this->value() << value;
    }

    template <typename T>
    auto operator && (const T &value) const -> decltype(this->value() && value) {
        return this->value() && value;
    }

    template <typename T>
    friend auto operator && (const T& lhf, const Field& rhf) -> decltype(lhf && Field::type(0)) {
        return lhf && rhf.value();
    }

    template <typename T>
    auto operator || (const T &value) const -> decltype(this->value() || value) {
        return this->value() || value;
    }

    template <typename T>
    friend auto operator || (const T& lhf, const Field& rhf) -> decltype(lhf || Field::type(0)) {
        return lhf || rhf.value();
    }

    template <typename T>
    auto operator ! () const -> decltype(!this->value()) {
        return !this->value();
    }

    struct Sign {

        template <typename = signedness,
                  bool matches_register = (size == 8 || size == 16 || size == 32 || size == 64),
                  typename U  = void>
        struct Extension;

        template<bool matches_register>
        struct Extension<unsigned, matches_register> {
            static utype cast(const utype &value) {
                return value;
            }
        };

        template<typename U>
        struct Extension<signed, true, U> {
            static stype cast(const utype &value) {
                return stype(value);
            }
        };

        template<typename U>
        struct Extension<signed, false, U> {
            static stype cast(const utype &value) {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wnarrowing"
                struct { stype v: size; } v{value};
                #pragma GCC diagnostic pop
                return v.v;
            }
        };
    };

    struct Static { template <long items> using Array = bitstream::Static::Array<Field, items>; };
    using Array = bitstream::Array<Field>;
};




template <long offset, Endianness endianness, typename signedness>
struct Field<0, offset, endianness, signedness>; // 0-size fields are prohibited


} // namespace bitstream


namespace bitstream {
namespace Const {

struct Verification {
    bool skip;
    void (*failed)(void *);
    void *context;
} extern verification;


template <bool verify>
struct Field;

template <>
struct Field<true> {
    template <typename Type, typename Value>
    Field(const Type &field, const Value &value) {
        if (!verification.skip) {
            if (!(field == value)) {
                verification.failed(verification.context);
            }
        }
    }
    template <typename Type, typename Value>
    Field(const Type &field, const std::initializer_list<Value> &value) {
        if (!verification.skip) {
            if (!(field == value)) {
                verification.failed(verification.context);
            }
        }
    }
};

template <>
struct Field<false> {
    template <typename Type, typename Value>
    Field(Type &field, const Value &value) {
        field = value;
    }
    template <typename Type, typename Value>
    Field(Type &field, const std::initializer_list<Value> &value) {
        field = value;
    }
};

}} // namespace bitstream::Const

namespace bitstream {
namespace le {


template <long size, long offset, typename signedness>
using Field = bitstream::Field<size, offset, Endianness::little, signedness>;

template <long size, long offset=0>
using UInt = Field<size, offset, unsigned>;

template <long offset=0>
using UInt8 = UInt<8, offset>;

template <long offset=0>
using UInt16 = UInt<16, offset>;

template <long offset=0>
using UInt32 = UInt<32, offset>;

template <long offset=0>
using UInt64 = UInt<64, offset>;

template <long size, long offset=0>
using Int = Field<size, offset, signed>;

template <long offset=0>
using Int8 = Int<8, offset>;

template <long offset=0>
using Int16 = Int<16, offset>;

template <long offset=0>
using Int32 = Int<32, offset>;

template <long offset=0>
using Int64 = Int<64, offset>;


}} // namespace bitstream::le


namespace bitstream {
namespace be {


template <long size, long offset, typename signedness>
using Field = bitstream::Field<size, offset, Endianness::big, signedness>;

template <long size, long offset=0>
using UInt = Field<size, offset, unsigned>;

template <long offset=0>
using UInt8 = UInt<8, offset>;

template <long offset=0>
using UInt16 = UInt<16, offset>;

template <long offset=0>
using UInt32 = UInt<32, offset>;

template <long offset=0>
using UInt64 = UInt<64, offset>;

template <long size, long offset=0>
using Int = Field<size, offset, signed>;

template <long offset=0>
using Int8 = Int<8, offset>;

template <long offset=0>
using Int16 = Int<16, offset>;

template <long offset=0>
using Int32 = Int<32, offset>;

template <long offset=0>
using Int64 = Int<64, offset>;


}} // namespace bitstream::be


#endif // __BITSTREAM_FIELD_H__


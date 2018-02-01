#ifndef __BITSTREAM_CACHED_FIELD_H__
#define __BITSTREAM_CACHED_FIELD_H__

#include <bitstream/field.h>


namespace bitstream {
namespace cached {
namespace field {


template <typename Field>
struct Reference {
    using Type = typename Field::type;

    explicit Reference(Field &reference): reference(reference) {}

    void update() { this->value = reference; }
    operator Type() const { return this->value; }
    Reference &operator = (const Type &value) {
        reference = this->value = value;
        return *this;
    }

private:
    Field &reference;
    Type value;

public: // Overload operators

    template <typename T>
    bool operator == (const T &value) const {
        return this->value == value;
    }

    template <typename T>
    friend bool operator == (const T& lhf, const Reference& rhf) {
        return lhf == rhf.value;
    }

    template <typename T>
    bool operator != (const T &value) const {
        return this->value != value;
    }

    template <typename T>
    friend bool operator != (const T& lhf, const Reference& rhf) {
        return lhf != rhf.value;
    }

    template <typename T>
    bool operator > (const T &value) const {
        return this->value > value;
    }

    template <typename T>
    friend bool operator > (const T& lhf, const Reference& rhf) {
        return lhf > rhf.value;
    }

    template <typename T>
    bool operator >= (const T &value) const {
        return this->value >= value;
    }

    template <typename T>
    friend bool operator >= (const T& lhf, const Reference& rhf) {
        return lhf >= rhf.value;
    }

    template <typename T>
    bool operator < (const T &value) const {
        return this->value < value;
    }

    template <typename T>
    friend bool operator < (const T& lhf, const Reference& rhf) {
        return lhf < rhf.value;
    }

    template <typename T>
    bool operator <= (const T &value) const {
        return this->value <= value;
    }

    template <typename T>
    friend bool operator <= (const T& lhf, const Reference& rhf) {
        return lhf <= rhf.value;
    }

    template <typename T>
    auto operator + (const T &value) const -> decltype(this->value + value) {
        return this->value + value;
    }

    template <typename T>
    friend auto operator + (const T& lhf, const Reference& rhf) -> decltype(lhf + rhf.value) {
        return lhf + rhf.value;
    }

    template <typename T>
    Reference &operator += (const T &value) {
        return *this = this->value + value;
    }

    template <typename T>
    auto operator - (const T &value) const -> decltype(this->value - value) {
        return this->value - value;
    }

    template <typename T>
    friend auto operator - (const T& lhf, const Reference& rhf) -> decltype(lhf - rhf.value) {
        return lhf - rhf.value;
    }

    template <typename T>
    Reference &operator -= (const T &value) {
        return *this = this->value - value;
    }

    template <typename T>
    Reference &operator ++ () {
        return *this = this->value + 1;
    }

    template <typename T>
    Type operator ++ (int) {
        Type value = this->value;
        *this = this->value + 1;
        return value;
    }

    template <typename T>
    Reference &operator -- () {
        return *this = this->value - 1;
    }

    template <typename T>
    Type operator -- (int) {
        Type value = this->value;
        *this = this->value - 1;
        return value;
    }

    template <typename T>
    auto operator * (const T &value) const -> decltype(this->value * value) {
        return this->value * value;
    }

    template <typename T>
    friend auto operator * (const T& lhf, const Reference& rhf) -> decltype(lhf * rhf.value) {
        return lhf * rhf.value;
    }

    template <typename T>
    Reference &operator *= (const T &value) {
        return *this = this->value * value;
    }

    template <typename T>
    auto operator / (const T &value) const -> decltype(this->value / value) {
        return this->value / value;
    }

    template <typename T>
    friend auto operator / (const T& lhf, const Reference& rhf) -> decltype(lhf / rhf.value) {
        return lhf / rhf.value;
    }

    template <typename T>
    Reference &operator /= (const T &value) {
        return *this = this->value / value;
    }

    template <typename T>
    auto operator % (const T &value) const -> decltype(this->value % value) {
        return this->value % value;
    }

    template <typename T>
    friend auto operator % (const T& lhf, const Reference& rhf) -> decltype(lhf % rhf.value) {
        return lhf % rhf.value;
    }

    template <typename T>
    Reference &operator %= (const T &value) {
        return *this = this->value % value;
    }

    template <typename T>
    auto operator & (const T &value) const -> decltype(this->value & value) {
        return this->value & value;
    }

    template <typename T>
    friend auto operator & (const T& lhf, const Reference& rhf) -> decltype(lhf & rhf.value) {
        return lhf & rhf.value;
    }

    template <typename T>
    Reference &operator &= (const T &value) {
        return *this = this->value & value;
    }

    template <typename T>
    auto operator | (const T &value) const -> decltype(this->value | value) {
        return this->value | value;
    }

    template <typename T>
    friend auto operator | (const T& lhf, const Reference& rhf) -> decltype(lhf | rhf.value) {
        return lhf | rhf.value;
    }

    template <typename T>
    Reference &operator |= (const T &value) {
        return *this = this->value | value;
    }

    template <typename T>
    auto operator ^ (const T &value) const -> decltype(this->value ^ value) {
        return this->value ^ value;
    }

    template <typename T>
    friend auto operator ^ (const T& lhf, const Reference& rhf) -> decltype(lhf ^ rhf.value) {
        return lhf ^ rhf.value;
    }

    template <typename T>
    Reference &operator ^= (const T &value) {
        return *this = this->value ^ value;
    }

    template <typename T>
    auto operator ~ () const -> decltype(~(this->value)) {
        return ~(this->value);
    }

    template <typename T>
    auto operator >> (const T &value) const -> decltype(this->value >> value) {
        return this->value >> value;
    }

    template <typename T>
    friend auto operator >> (const T& lhf, const Reference& rhf) -> decltype(lhf >> rhf.value) {
        return lhf >> rhf.value;
    }

    template <typename T>
    Reference &operator >>= (const T &value) {
        return *this = this->value >> value;
    }

    template <typename T>
    auto operator << (const T &value) const -> decltype(this->value << value) {
        return this->value << value;
    }

    template <typename T>
    friend auto operator << (const T& lhf, const Reference& rhf) -> decltype(lhf << rhf.value) {
        return lhf << rhf.value;
    }

    template <typename T>
    Reference &operator <<= (const T &value) {
        return *this = this->value << value;
    }

    template <typename T>
    auto operator && (const T &value) const -> decltype(this->value && value) {
        return this->value && value;
    }

    template <typename T>
    friend auto operator && (const T& lhf, const Reference& rhf) -> decltype(lhf && rhf.value) {
        return lhf && rhf.value;
    }

    template <typename T>
    auto operator || (const T &value) const -> decltype(this->value || value) {
        return this->value || value;
    }

    template <typename T>
    friend auto operator || (const T& lhf, const Reference& rhf) -> decltype(lhf || rhf.value) {
        return lhf || rhf.value;
    }

    template <typename T>
    auto operator ! () const -> decltype(!this->value) {
        return !this->value;
    }
};


template <typename Field>
struct Value: Field {
    using Type = typename Field::type;
    Value() {}

    void update() { Value::value = static_cast<const Field &>(*this); }
    operator Type() const { return Value::value; }
    Value &operator = (const Type &value) {
        Field::operator = (Value::value = value);
        return *this;
    }

private:
    Type value;

public: // Overload operators

    template <typename T>
    bool operator == (const T &value) const {
        return this->value == value;
    }

    template <typename T>
    friend bool operator == (const T& lhf, const Value& rhf) {
        return lhf == rhf.value;
    }

    template <typename T>
    bool operator != (const T &value) const {
        return this->value != value;
    }

    template <typename T>
    friend bool operator != (const T& lhf, const Value& rhf) {
        return lhf != rhf.value;
    }

    template <typename T>
    bool operator > (const T &value) const {
        return this->value > value;
    }

    template <typename T>
    friend bool operator > (const T& lhf, const Value& rhf) {
        return lhf > rhf.value;
    }

    template <typename T>
    bool operator >= (const T &value) const {
        return this->value >= value;
    }

    template <typename T>
    friend bool operator >= (const T& lhf, const Value& rhf) {
        return lhf >= rhf.value;
    }

    template <typename T>
    bool operator < (const T &value) const {
        return this->value < value;
    }

    template <typename T>
    friend bool operator < (const T& lhf, const Value& rhf) {
        return lhf < rhf.value;
    }

    template <typename T>
    bool operator <= (const T &value) const {
        return this->value <= value;
    }

    template <typename T>
    friend bool operator <= (const T& lhf, const Value& rhf) {
        return lhf <= rhf.value;
    }

    template <typename T>
    auto operator + (const T &value) const -> decltype(this->value + value) {
        return this->value + value;
    }

    template <typename T>
    friend auto operator + (const T& lhf, const Value& rhf) -> decltype(lhf + rhf.value) {
        return lhf + rhf.value;
    }

    template <typename T>
    Value &operator += (const T &value) {
        return *this = this->value + value;
    }

    template <typename T>
    auto operator - (const T &value) const -> decltype(this->value - value) {
        return this->value - value;
    }

    template <typename T>
    friend auto operator - (const T& lhf, const Value& rhf) -> decltype(lhf - rhf.value) {
        return lhf - rhf.value;
    }

    template <typename T>
    Value &operator -= (const T &value) {
        return *this = this->value - value;
    }

    template <typename T>
    Value &operator ++ () {
        return *this = this->value + 1;
    }

    template <typename T>
    Type operator ++ (int) {
        Type value = this->value;
        *this = this->value + 1;
        return value;
    }

    template <typename T>
    Value &operator -- () {
        return *this = this->value - 1;
    }

    template <typename T>
    Type operator -- (int) {
        Type value = this->value;
        *this = this->value - 1;
        return value;
    }

    template <typename T>
    auto operator * (const T &value) const -> decltype(this->value * value) {
        return this->value * value;
    }

    template <typename T>
    friend auto operator * (const T& lhf, const Value& rhf) -> decltype(lhf * rhf.value) {
        return lhf * rhf.value;
    }

    template <typename T>
    Value &operator *= (const T &value) {
        return *this = this->value * value;
    }

    template <typename T>
    auto operator / (const T &value) const -> decltype(this->value / value) {
        return this->value / value;
    }

    template <typename T>
    friend auto operator / (const T& lhf, const Value& rhf) -> decltype(lhf / rhf.value) {
        return lhf / rhf.value;
    }

    template <typename T>
    Value &operator /= (const T &value) {
        return *this = this->value / value;
    }

    template <typename T>
    auto operator % (const T &value) const -> decltype(this->value % value) {
        return this->value % value;
    }

    template <typename T>
    friend auto operator % (const T& lhf, const Value& rhf) -> decltype(lhf % rhf.value) {
        return lhf % rhf.value;
    }

    template <typename T>
    Value &operator %= (const T &value) {
        return *this = this->value % value;
    }

    template <typename T>
    auto operator & (const T &value) const -> decltype(this->value & value) {
        return this->value & value;
    }

    template <typename T>
    friend auto operator & (const T& lhf, const Value& rhf) -> decltype(lhf & rhf.value) {
        return lhf & rhf.value;
    }

    template <typename T>
    Value &operator &= (const T &value) {
        return *this = this->value & value;
    }

    template <typename T>
    auto operator | (const T &value) const -> decltype(this->value | value) {
        return this->value | value;
    }

    template <typename T>
    friend auto operator | (const T& lhf, const Value& rhf) -> decltype(lhf | rhf.value) {
        return lhf | rhf.value;
    }

    template <typename T>
    Value &operator |= (const T &value) {
        return *this = this->value | value;
    }

    template <typename T>
    auto operator ^ (const T &value) const -> decltype(this->value ^ value) {
        return this->value ^ value;
    }

    template <typename T>
    friend auto operator ^ (const T& lhf, const Value& rhf) -> decltype(lhf ^ rhf.value) {
        return lhf ^ rhf.value;
    }

    template <typename T>
    Value &operator ^= (const T &value) {
        return *this = this->value ^ value;
    }

    template <typename T>
    auto operator ~ () const -> decltype(~(this->value)) {
        return ~(this->value);
    }

    template <typename T>
    auto operator >> (const T &value) const -> decltype(this->value >> value) {
        return this->value >> value;
    }

    template <typename T>
    friend auto operator >> (const T& lhf, const Value& rhf) -> decltype(lhf >> rhf.value) {
        return lhf >> rhf.value;
    }

    template <typename T>
    Value &operator >>= (const T &value) {
        return *this = this->value >> value;
    }

    template <typename T>
    auto operator << (const T &value) const -> decltype(this->value << value) {
        return this->value << value;
    }

    template <typename T>
    friend auto operator << (const T& lhf, const Value& rhf) -> decltype(lhf << rhf.value) {
        return lhf << rhf.value;
    }

    template <typename T>
    Value &operator <<= (const T &value) {
        return *this = this->value << value;
    }

    template <typename T>
    auto operator && (const T &value) const -> decltype(this->value && value) {
        return this->value && value;
    }

    template <typename T>
    friend auto operator && (const T& lhf, const Value& rhf) -> decltype(lhf && rhf.value) {
        return lhf && rhf.value;
    }

    template <typename T>
    auto operator || (const T &value) const -> decltype(this->value || value) {
        return this->value || value;
    }

    template <typename T>
    friend auto operator || (const T& lhf, const Value& rhf) -> decltype(lhf || rhf.value) {
        return lhf || rhf.value;
    }

    template <typename T>
    auto operator ! () const -> decltype(!this->value) {
        return !this->value;
    }
};


}}} // namespace bitstream::cached::field


#endif // __BITSTREAM_CACHED_FIELD_H__


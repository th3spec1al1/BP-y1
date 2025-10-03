#include "number.h"

void MakeShift(uint239_t& value, int shift){
    uint64_t new_shift = GetShift(value);
    new_shift += shift;
    
    shift %= 245;
    if (shift < 0) shift += 245;

    int bytes_shift = shift / 7;
    int bits_shift = shift % 7;

    uint239_t support239;

    for (int i = 0; i < 35; ++i) {
        support239.data[i] = 0b00000000;
    }

    if (bytes_shift > 0) {
        for (int i = 0; i < 35; ++i) {
            support239.data[i] = (value.data[(i + bytes_shift) % 35]) & 0b01111111;
        }
        for(int i = 34; i >= 0; --i){
            value.data[i] = support239.data[i];
        }
        for (int i = 34; i >= 0; --i) {
            support239.data[i] = 0b00000000;
        }
    }

    uint8_t carry = 0;
    uint8_t current = 0;

    if (bits_shift > 0){
        for(int i = 34; i >= 0; --i){
            carry = (value.data[i] & 0b01111111) >> (7 - bits_shift);
            current = (value.data[(i - 1 + 35) % 35] & 0b01111111) << bits_shift;
            support239.data[(i - 1 + 35) % 35] = carry | current;
        }
        for(int i = 34; i >= 0; --i){
            value.data[i] = support239.data[i];
        }
    }

    for (int i = 34; i >= 0; --i){
        value.data[i] = value.data[i] & 0b01111111;  
        value.data[i] |= ((new_shift & 1) << 7);
        new_shift = new_shift >> 1;
    }
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t number;

    for (int i=0; i<35; ++i){
        number.data[i] = 0b00000000;
    }

    for(int j = 34; j > 29; j--){
        number.data[j] = value % 128;
        value /= 128;
    }

    MakeShift(number, shift);
    return number;
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint239_t result;

    for (int i=0; i<35; ++i){
        result.data[i] = 0b00000000;
    }

    for (const char* figure = str; *figure; ++figure) {
        uint8_t carry = 0;
        for (int i = 34; i >= 0; --i) {
            uint16_t product = (result.data[i] & 0b01111111) * 10 + carry;
            result.data[i] = product & 0b01111111;
            carry = (product >> 7) & 0b01111111;
        }

        uint8_t value = *figure - '0';
        carry = value;
        for (int i = 34; i >= 0; --i) {
            uint16_t sum = (result.data[i] & 0b01111111) + carry;
            result.data[i] = sum & 0b01111111;
            carry = (sum >> 7) & 0b01111111;
        }
    }

    MakeShift(result, shift);
    return result;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;

    for (int i = 0; i < 35; ++i) {
        result.data[i] = 0b00000000;
    }

    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t new_shift = lhs_shift + rhs_shift;

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    uint8_t carry = 0;
    for (int i = 34; i >= 0; --i){
        uint8_t sum = lhs_copy.data[i] + rhs_copy.data[i] + carry;
        result.data[i] = sum & 0b01111111;
        carry = sum >> 7;
    }

    MakeShift(result, new_shift);
    return result;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;

    for (int i = 0; i < 35; ++i) {
        result.data[i] = 0b00000000;
    }

    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t new_shift = lhs_shift - rhs_shift;

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    uint8_t carry = 0;
    for (int i = 34; i >= 0; --i){
        uint8_t diff;
        int16_t razn = lhs_copy.data[i] - rhs_copy.data[i] - carry;
        diff = razn < 0 ? 256 + razn : razn;

        result.data[i] = diff & 0b01111111;
        carry = (diff >> 7) & 1;
    }

    MakeShift(result, new_shift);
    return result;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;

    for (int i = 0; i < 35; ++i) {
        result.data[i] = 0b00000000;
    }
    
    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t new_shift = lhs_shift + rhs_shift;

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    uint239_t zero = FromInt(0, 0);

    if (lhs_copy == zero || rhs_copy == zero){
        MakeShift(zero, new_shift);
        return zero;
    }

    for (int i = 0; i < 239; ++i) {
        if (rhs_copy.data[34 - i / 7] & (1 << (i % 7))) {
            uint239_t temp = lhs_copy;
            MakeShift(temp, i);
            uint8_t carry = 0;

            for (int j = 34; j >= 0; --j) {
                uint16_t sum = (result.data[j] & 0b01111111) + (temp.data[j] & 0b01111111) + carry;
                result.data[j] = sum & 0b01111111;
                carry = (sum > 128) ? 1 : 0;
            }
        }
    }

    MakeShift(result, new_shift);
    return result;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;

    for (int i = 0; i < 35; ++i) {
        result.data[i] = 0b00000000;
    }
    
    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t new_shift = lhs_shift - rhs_shift;

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    uint239_t zero = FromInt(0, 0);
    uint239_t unit = FromInt(1, 0);

    if (lhs_copy == zero){
        MakeShift(zero, new_shift);
        return zero;
    }

    if (rhs_copy == zero){
        std::cout << "Error: detorminator can't be 0" << std::endl;
        exit(0);
    }

    if (lhs_copy == rhs_copy){
        MakeShift(unit, new_shift);
        return unit;
    }

    if (rhs_copy == unit){
        MakeShift(lhs_copy, new_shift);
        return lhs_copy;
    }

    while (lhs_copy >= rhs_copy){
        result = result + unit;
        lhs_copy = lhs_copy - rhs_copy;
    }

    MakeShift(result, new_shift);
    return result;
}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    for (int i = 0; i < 35; ++i){
        if (lhs_copy.data[i] != rhs_copy.data[i]){
            return false;
        }
    }

    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    return !(lhs == rhs);
}

bool operator>=(const uint239_t &lhs, const uint239_t &rhs){
    uint239_t lhs_copy = lhs;
    uint239_t rhs_copy = rhs;

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);

    MakeShift(lhs_copy, -lhs_shift);
    MakeShift(rhs_copy, -rhs_shift);

    for (int i = 0; i < 35; ++i){
        if (lhs_copy.data[i] < rhs_copy.data[i]){
            return false;
        }
        if (lhs_copy.data[i] > rhs_copy.data[i]){
            return true;
        }
    }

    return true;
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    for (int i=0; i<35; ++i){
        for (int j=7; j>=0; --j){
            uint8_t figure = (value.data[i] << (7-j));
            stream << (figure >> 7);
        }
    }

    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint64_t shift = 0;
    uint64_t shift_bit = 0;

    for (int i = 0; i < 35; ++i) {
        shift_bit = (value.data[i] >> 7); 
        shift |= shift_bit;
        if (i < 34) {
            shift = (shift << 1);
        }
    }

    return shift;
}

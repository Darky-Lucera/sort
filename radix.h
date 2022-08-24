#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <type_traits>


// Already implemented in C++17
//--------------------------------------
template<typename T, typename... Rest>
struct is_any : std::false_type {};

template<typename T, typename First>
struct is_any<T, First> : std::is_same<T, First> {};

template<typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...> : std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value> {};


// Tricks from Michael Herf (Flip / Flop)
//--------------------------------------
static inline void
Flip(uint32_t &refValue) {
    uint32_t mask = -int32_t(refValue >> 31) | 0x8000'0000;
    refValue ^= mask;
}

//--------------------------------------
static inline void
Flip(uint64_t &refValue) {
    uint64_t mask = -int64_t(refValue >> 63) | 0x8000'0000'0000'0000ULL;
    refValue ^= mask;
}

//--------------------------------------
static inline void
Flop(uint32_t &refValue) {
    uint32_t mask = ((refValue >> 31) - 1) | 0x8000'0000;
    refValue ^= mask;
}

//--------------------------------------
static inline void
Flop(uint64_t &refValue) {
    uint64_t mask = ((refValue >> 63) - 1) | 0x8000'0000'0000'0000ULL;
    refValue ^= mask;
}

// Count sort
//--------------------------------------
template <typename T>
void
countSort(T *buffer, size_t size, size_t bits, T *tmp) {
    ptrdiff_t	i, digit;
    ptrdiff_t   count[256] = { 0 };

    // Store count of occurrences for each digit
    for (i = 0; i < ptrdiff_t(size); ++i) {
        digit = (buffer[i] >> bits) & 0xff;
        ++count[digit];
    }

    // Change count so that count now contains actual position of this digit in buffer
    --count[0];
    for (i = 1; i < 256; ++i) {
        count[i] += count[i - 1];
    }

    // Sort values in tmp according to the current digit
    for (i = size - 1; i >= 0; --i) {
        digit = (buffer[i] >> bits) & 0xff;
        tmp[count[digit]] = buffer[i];
        --count[digit];
    }

    // restore values to the original buffer
    //memcpy(buffer, tmp, sizeof(T) * size);
}

// Count sort with unrolling loops (faster)
//--------------------------------------
template <typename T>
void
countSort2(T *buffer, size_t size, size_t bitsA, T *tmp) {
    ptrdiff_t	i, digitA, digitB;
    ptrdiff_t   countA[256] = { 0 };
    ptrdiff_t   countB[256] = { 0 };
    size_t      bitsB = bitsA + 8;

    // Store count of occurrences for each digit
    for (i = 0; i < ptrdiff_t(size); ++i) {
        digitA = (buffer[i] >> bitsA) & 0xff;
        digitB = (buffer[i] >> bitsB) & 0xff;
        ++countA[digitA];
        ++countB[digitB];
    }

    // Change count so that count now contains actual position of this digit in buffer
    --countA[0];
    --countB[0];
    for (i = 1; i < 256; ++i) {
        countA[i] += countA[i - 1];
        countB[i] += countB[i - 1];
    }

    // Sort values in tmp according to the current digit
    for (i = size - 1; i >= 0; --i) {
        digitA = (buffer[i] >> bitsA) & 0xff;
        tmp[countA[digitA]] = buffer[i];
        --countA[digitA];
    }

    // Sort values in buffer according to the next digit
    for (i = size - 1; i >= 0; --i) {
        digitB = (tmp[i] >> bitsB) & 0xff;
        buffer[countB[digitB]] = tmp[i];
        --countB[digitB];
    }
}

//--------------------------------------
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
radixSort(T *buffer, size_t size) {
    using unsign = typename std::make_unsigned<T>::type;
    std::vector<T>	tmp(size);
    size_t			maxBits = sizeof(T) << 3;
    unsign          lastBit = (unsign(1) << (maxBits - 1));

    // Flip the sign bit so that negative integers will appear in front of positive integers
    if (std::is_signed<T>::value) {
        for (size_t i = 0; i < size; ++i) {
            buffer[i] ^= lastBit;
        }
    }

    for (size_t bits = 0; bits < maxBits; bits += 16) {
        countSort(buffer, size, bits, tmp.data());
        countSort(tmp.data(), size, bits + 8, buffer);      // Avoids the last memcpy
    }

    // Restore original numbers
    if (std::is_signed<T>::value) {
        for (size_t i = 0; i < size; ++i) {
            buffer[i] ^= lastBit;
        }
    }
}

//--------------------------------------
template <typename T>
typename std::enable_if<is_any<T, float, double>::value, void>::type
radixSort(T *buffer, size_t size) {
    using intType = typename std::conditional<std::is_same<T, float>::value, uint32_t, uint64_t>::type;

    auto *aux = reinterpret_cast<intType *>(buffer);

    // Trick from Michael Herf to be able to sort floating point numbers
    for (size_t i = 0; i < size; ++i) {
        Flip(aux[i]);
    }

    // Here I want to use countSort2 to compare it with the other version (2 countSort)
    //radixSort(aux, size);

    std::vector<intType>    tmp(size);
    size_t			        maxBits = sizeof(T) << 3;

    for (size_t bits = 0; bits < maxBits; bits += 16) {
        countSort2(aux, size, bits, tmp.data());
    }

    // Restore original numbers
    for (size_t i = 0; i < size; ++i) {
        Flop(aux[i]);
    }
}

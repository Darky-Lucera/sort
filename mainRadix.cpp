#include "radix.h"
//--------------------------------------
#include <vector>
#include <chrono>
#include <random>
#include <iostream>

using namespace std::chrono;

//--------------------------------------
template <typename T>
uint32_t
GetTime(T time) {
    return uint32_t(duration_cast<nanoseconds>(time).count());
}

//--------------------------------------
static inline high_resolution_clock::time_point
Now() {
    return high_resolution_clock::now();
}

void
Test(size_t size) {
    std::vector<int32_t>	int32s;
    std::vector<int64_t>	int64s;
    std::vector<float>	    floats;
    std::vector<double>	    doubles;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int32_t> randomInt(-123456, 123456);

    int32s.reserve(size);
    int64s.reserve(size);
    floats.reserve(size);
    doubles.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        auto value = randomInt(mt);
        int32s.push_back(int32_t(value));
        int64s.push_back(int64_t(value));
        floats.push_back(float(value));
        doubles.push_back(double(value));
    }

    auto    int32s_copy = int32s;
    auto    int64s_copy = int64s;
    auto    floats_copy = floats;
    auto    doubles_copy = doubles;

    using namespace std::chrono;

    auto timeStampA1 = Now();
    std::sort(int32s_copy.begin(), int32s_copy.end());
    auto timeA1 = Now() - timeStampA1;

    auto timeStampA2 = Now();
    std::sort(int64s_copy.begin(), int64s_copy.end());
    auto timeA2 = Now() - timeStampA2;

    auto timeStampA3 = Now();
    std::sort(floats_copy.begin(), floats_copy.end());
    auto timeA3 = Now() - timeStampA3;

    auto timeStampA4 = Now();
    std::sort(doubles_copy.begin(), doubles_copy.end());
    auto timeA4 = Now() - timeStampA4;

    //--

    auto timeStampB1 = Now();
    radixSort(int32s.data(), int32s.size());
    auto timeB1 = Now() - timeStampB1;

    auto timeStampB2 = Now();
    radixSort(int64s.data(), int64s.size());
    auto timeB2 = Now() - timeStampB2;

    auto timeStampB3 = Now();
    radixSort(floats.data(), floats.size());
    auto timeB3 = Now() - timeStampB3;

    auto timeStampB4 = Now();
    radixSort(doubles.data(), doubles.size());
    auto timeB4 = Now() - timeStampB4;

    std::cout << std::boolalpha;
    std::cout << "int32:  " << (int32s == int32s_copy) << " - std::sort: " << GetTime(timeA1) << " - radix: " << GetTime(timeB1) << std::endl;
    std::cout << "int64:  " << (int64s == int64s_copy) << " - std::sort: " << GetTime(timeA2) << " - radix: " << GetTime(timeB2) << std::endl;
    std::cout << "float:  " << (floats == floats_copy) << " - std::sort: " << GetTime(timeA3) << " - radix: " << GetTime(timeB3) << std::endl;
    std::cout << "double: " << (doubles == doubles_copy) << " - std::sort: " << GetTime(timeA4) << " - radix: " << GetTime(timeB4) << std::endl;
}

//--------------------------------------
int
main() {
    for(size_t s=256; s<=65536; s*=2) {
        std::cout << s << std::endl;
        Test(s);
        std::cout << std::endl;
    }

    return 0;
}
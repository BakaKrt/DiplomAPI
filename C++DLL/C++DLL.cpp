#include <iostream>
#include <cstddef> // For std::byte
#include <cstdint>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <thread>



#ifdef _DEBUG
    #include <stdexcept> // Для std::out_of_range
    
#endif

//#include "HeightMap.h"
#include "ExternDLL.h"

using std::vector;
using std::array;
using std::thread;

using byte = uint8_t;



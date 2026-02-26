#include <Windows.h>
#include <string>

import std;
import SharedMemoryHelper;


using namespace SharedMemory;


int main()
{
    auto memory = SharedMemoryObject(100, sizeof(char), L"test");


#ifdef TEST

    const wstring sharedMemoryName = L"test";

    wstring createdMemoryName{};

    vector<thread> THREADS; THREADS.reserve(2);

    THREADS.emplace_back([sharedMemoryName, &createdMemoryName]() {
        SharedMemoryObject object{ 10, sizeof(int), sharedMemoryName };

        auto settings = object.create();
        createdMemoryName = object.getName();

        wprintf(L"%7u: writed data in thread\n", std::this_thread::get_id());
        int* array = (int*)(settings.array);
        for (size_t x = 0; x < settings.size; x++) {
            array[x] = x;
        }
        wprintf(L"%7u: going to sleep in thread\n", std::this_thread::get_id());
        std::this_thread::sleep_for(2000ms);
    });

    THREADS.emplace_back([&createdMemoryName]() {
        std::this_thread::sleep_for(500ms);
        wprintf(L"%7u: sleeped\n", std::this_thread::get_id());

        SharedMemoryObject reader {};
        auto readerSettings = reader.connect(createdMemoryName);


        int* array = (int*)(readerSettings.array);

        
        wprintf(L"%7u: read data in thread\n", std::this_thread::get_id());

        for (size_t x = 0; x < readerSettings.size; x++) {
            wprintf(L"%d, ", array[x]);
        } wcout << L"\n";

        wprintf(L"%7u: readed data in thread\n", std::this_thread::get_id());
    });

    for (auto& a : THREADS) {
        if (a.joinable()) a.join();
    }
#endif // TEST
}

#include <Windows.h>
#include <string>

export module SharedMemoryHelper;

import std;


export namespace SharedMemory {
    using std::wcout, std::wstring, std::to_wstring;
    using std::vector;
    using std::thread;

    struct SharedMemoryMapSetting {
        size_t size;
        size_t objectSize;
        void* array;
    };

    class SharedMemoryObject {
    private:
        SharedMemoryMapSetting _settings;
        HANDLE _mapFileHandler = nullptr;
        static inline size_t _counter = 0;
        static const inline wstring _SHARED_MEMORY_NAME = L"SHARED_MEMORY_";
        wstring _name;
        void* _sharedMemoryPtr = nullptr;

        bool _is_opened = false;
        bool _is_closed = false;
        bool _is_owner = false;
    public:
        SharedMemoryObject(size_t size, size_t oneObjectSize, wstring postfix = L"") {
            _settings = { size, oneObjectSize, nullptr };
            _name = _SHARED_MEMORY_NAME + postfix + L"_" + to_wstring(_counter);
            _counter++;
        }
        SharedMemoryObject() {}

        SharedMemoryMapSetting connect(const wstring& existMemory) {
            _name = existMemory;

            this->_mapFileHandler = OpenFileMappingW(
                FILE_MAP_ALL_ACCESS,
                FALSE,
                _name.c_str()
            );

            if (_mapFileHandler == NULL || _mapFileHandler == INVALID_HANDLE_VALUE) {
                wcout << L"OPEN ERROR: " << GetLastError() << L"\n";
                return { 0, 0, nullptr };
            }

            // Получаем указатель на shared memory
            this->_sharedMemoryPtr = MapViewOfFile(
                _mapFileHandler,
                FILE_MAP_ALL_ACCESS,
                0,
                0,
                0
            );

            if (_sharedMemoryPtr == nullptr) {
                wcout << L"SOME ERR\n";
                return { 0, 0, nullptr };
            }

            // Читаем метаданные (первые два size_t)
            size_t* meta = static_cast<size_t*>(_sharedMemoryPtr);
            size_t size = meta[0];
            size_t objectSize = meta[1];

            // Вычисляем указатель на данные (после метаданных)
            void* dataPtr = static_cast<char*>(_sharedMemoryPtr) + sizeof(size_t) * 2;

            _settings = { size, objectSize, dataPtr };

            return _settings;
        }

        SharedMemoryMapSetting create() {
            const size_t metaSize = sizeof(size_t) * 2; // Для хранения метаданных
            const size_t dataSize = _settings.size * _settings.objectSize;
            const size_t totalSize = metaSize + dataSize;

            // Создаем/открываем shared memory
            this->_mapFileHandler = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                NULL,
                PAGE_READWRITE,
                0,
                static_cast<DWORD>(totalSize),
                this->_name.c_str()
            );

            auto err = GetLastError();

            if (err == ERROR_ALREADY_EXISTS) {
                CloseHandle(_mapFileHandler);
                _mapFileHandler = nullptr;
                wcout << L"ERROR: Shared memory with this name already exists\n";
                return { 0, 0, nullptr };
            }

            if (_mapFileHandler == NULL || _mapFileHandler == INVALID_HANDLE_VALUE) {
                wcout << L"OPEN ERROR: " << err << L"\n";
                return { 0, 0, nullptr };
            }

            // Получаем указатель на shared memory
            this->_sharedMemoryPtr = MapViewOfFile(
                _mapFileHandler,
                FILE_MAP_ALL_ACCESS,
                0,
                0,
                totalSize
            );

            if (_sharedMemoryPtr == nullptr) {
                CloseHandle(_mapFileHandler);
                wcout << L"MAP VIEW ERROR: " << GetLastError() << L"\n";
                return { 0, 0, nullptr };
            }

            _is_owner = true;
            _is_opened = true;

            // Записываем метаданные (первые два size_t)
            size_t* meta = static_cast<size_t*>(_sharedMemoryPtr);
            meta[0] = _settings.size;
            meta[1] = _settings.objectSize;

            // Вычисляем указатель на данные (после метаданных)
            void* dataPtr = static_cast<char*>(_sharedMemoryPtr) + metaSize;

            // Возвращаем настройки с правильным указателем на данные
            return {
                _settings.size,
                _settings.objectSize,
                dataPtr
            };
        }

        wstring getName() { return _name; }

        ~SharedMemoryObject() {
            if (_is_owner == false) return;

            // Освобождаем ресурсы
            UnmapViewOfFile(this->_sharedMemoryPtr);
            CloseHandle(_mapFileHandler);
            //CloseHandle(hDataReady);
            //CloseHandle(hReadConfirmed);
        }
    };
}
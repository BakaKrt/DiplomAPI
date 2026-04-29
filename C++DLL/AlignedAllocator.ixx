#include <malloc.h>
#include <string.h>

export module AlignedAllocator;

import std;

using std::size_t;
using std::align_val_t;

export namespace AlignedAllocator {
    class AlignedAllocator {
    public:
        inline static void* allocate(size_t size, size_t alignment = 16) noexcept {
            if (size == 0) return nullptr;

            void* ptr = _aligned_malloc(size, alignment);
            memset(ptr, 0, size);
            return ptr;
        }

        inline static void deallocate(void* ptr) noexcept {
            if (ptr) _aligned_free(ptr);
        }
    };

    template <typename T>
    class AlignedPtr {
    private:
        T* _ptr = nullptr;
        size_t _size {};
        size_t _align {};
    public:
        AlignedPtr() = delete;

        AlignedPtr(size_t size, size_t alignment) noexcept : _size(size), _align(alignment)
        {
            _ptr = reinterpret_cast<T*>(AlignedAllocator::allocate(size, alignment));
            memset(_ptr, 0, size);
        }

        /// <summary>
        /// Конструктор копирования
        /// </summary>
        /// <param name=""></param>
        AlignedPtr(const AlignedPtr&) = delete;
        AlignedPtr& operator=(const AlignedPtr&) = delete;

        /// <summary>
        /// Конструктор перемещения
        /// </summary>
        /// <param name="other"></param>
        AlignedPtr(AlignedPtr&& other) noexcept
            : _ptr(other._ptr), _size(other._size), _align(other._align)
        {
            other._ptr = nullptr;
            other._size = 0;
        }

        AlignedPtr& operator=(AlignedPtr&& other) noexcept {
            if (this != &other) {
                reset();
                _ptr = other._ptr;
                _size = other._size;
                _align = other._align;
                other._ptr = nullptr;
                other._size = 0;
            }
            return *this;
        }

        ~AlignedPtr() {
            reset();
        }

        // Метод сброса
        inline void reset() noexcept {
            AlignedAllocator::deallocate(_ptr);
            _ptr = nullptr;
            _size = 0;
        }
    };

    template <typename T>
    class SharedAlignedBuffer {
    public:
        static std::shared_ptr<T[]> create(size_t count, size_t alignment = 16) noexcept {
            if (count == 0) {
                return std::shared_ptr<T[]>(nullptr, [] (T*) {});
            }

            void* raw = AlignedAllocator::allocate(sizeof(T) * count, alignment);
            if (!raw) return std::shared_ptr<T[]>(nullptr, [] (T*) {});

            T* ptr = static_cast<T*>(raw);

            // shared_ptr с кастомным удалятором
            auto deleter = [] (T* p) {
                if (p) AlignedAllocator::deallocate(p);
                };

            return std::shared_ptr<T[]>(ptr, deleter);
        }
    };
}
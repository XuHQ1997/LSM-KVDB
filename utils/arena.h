#ifndef UTILS_ARENA_H
#define UTILS_ARENA_H

#include <vector>
#include <atomic>

namespace lsm {

class Arena {
public:
    Arena();
    ~Arena();

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    char* Allocate(size_t bytes);
    
    size_t MemoryUsage(void) const;
private:
    char* AllocateFallback(size_t bytes);
    char* AllocateNewBlock(size_t bytes);

    char* block_ptr_;
    size_t block_remaining_;
    std::vector<char*> blocks_;
    std::atomic<size_t> memory_usage_;
};

inline size_t Arena::MemoryUsage(void) const {
    return memory_usage_.load(std::memory_order_relaxed);
}

inline char* Arena::Allocate(size_t bytes) {
    if (bytes <= block_remaining_) {
        char* result = block_ptr_;
        block_ptr_ += bytes;
        block_remaining_ -= bytes;
        return result;
    }
    return AllocateFallback(bytes);
}

// wrapper for stl containers
template<typename T>
class ArenaAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    ArenaAllocator(Arena* arena) : arena_(arena) {}
    ~ArenaAllocator() = default;
    ArenaAllocator(const ArenaAllocator&) = default;

    // for compatibility with weird implementation of std::map 
    template<typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) 
        : arena_(other.arena_) {}

    pointer allocate(size_t n) {
        return reinterpret_cast<pointer>(
            arena_->Allocate(sizeof(T) * n));
    }

    void deallocate(T* ptr, size_t n) {}

    template<typename U>
    friend class ArenaAllocator;
private:
    Arena* arena_;
};

}  // namespace lsm
#endif
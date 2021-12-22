#include "arena.h"

namespace lsm {

static const size_t kBlockSize = 4096;

Arena::Arena() 
    : block_ptr_(nullptr)
    , block_remaining_(0)
    , memory_usage_(0) {}

Arena::~Arena() {
    for (size_t i = 0; i < blocks_.size(); ++i) {
        delete[] blocks_[i];
    }
}

char* Arena::AllocateFallback(size_t bytes) {
    if (bytes > kBlockSize / 4) {
        return AllocateNewBlock(bytes);
    }

    char* result = AllocateNewBlock(kBlockSize);
    block_ptr_ = result + bytes;
    block_remaining_ = kBlockSize - bytes;
    return result;
}

char* Arena::AllocateNewBlock(size_t bytes) {
    char* result = new char[bytes];
    blocks_.push_back(result);
    memory_usage_.fetch_add(bytes + sizeof(char*),
                            std::memory_order_relaxed);
    return result;
}

}  // namespace lsm

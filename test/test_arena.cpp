#include <random>
#include <gtest/gtest.h>

#include "utils/arena.h"

using lsm::Arena;

static size_t alloc_bytes = 0;

TEST(ArenaTest, Basic) {
    Arena arena;
    
    std::default_random_engine e;
    std::uniform_int_distribution<size_t> u(0, 2048);
    const int num_test = 1000;

    size_t exact_memory_usage = 0;
    for (int i = 0; i < num_test; ++i) {
        size_t bytes = u(e);
        arena.Allocate(bytes);
        exact_memory_usage += bytes;
    }
    ASSERT_GE(arena.MemoryUsage(), exact_memory_usage);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

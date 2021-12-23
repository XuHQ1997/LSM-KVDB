#include <string>
#include <gtest/gtest.h>
#include "lsm/memtable.h"

using lsm::MemTable;
using lsm::Status;

TEST(MemTableTest, Basic) {
    MemTable mem;
    Status s = mem.Add("key1", "value1");
    ASSERT_TRUE(s.ok());
    
    std::string value;
    s = mem.Get("key1", value);
    ASSERT_TRUE(s.ok());
    ASSERT_EQ(value, "value1");

    s = mem.Get("key2", value);
    ASSERT_EQ(s, Status::kNotFound);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

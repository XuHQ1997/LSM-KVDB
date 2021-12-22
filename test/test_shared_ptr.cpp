#include <gtest/gtest.h>

#include "utils/shared_ptr.h"


struct Obj : public lsm::Countable {
    static int num_alive;
    Obj() { ++num_alive; }
    ~Obj() { --num_alive; }
};

int Obj::num_alive = 0;
using ObjPtr = lsm::SharedPtr<Obj>;

TEST(SharedPtrTest, Basic) {
    {
        ObjPtr ptr(new Obj);
        ASSERT_EQ(Obj::num_alive, 1);
    }
    ASSERT_EQ(Obj::num_alive, 0);
}

TEST(SharedPtrTest, CopyAndAssign) {
    {
        ObjPtr ptr;
        {
            ObjPtr s_ptr(new Obj);
            ptr = s_ptr;
        }
        ASSERT_EQ(Obj::num_alive, 1);

        {
            ObjPtr s_ptr;
            ptr.swap(s_ptr);
        }
        ASSERT_EQ(Obj::num_alive, 0);

        {
            ObjPtr s_ptr(new Obj);
            ptr = ObjPtr(s_ptr);
        }
        ASSERT_EQ(Obj::num_alive, 1);
    }
    ASSERT_EQ(Obj::num_alive, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

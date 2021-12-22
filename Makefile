all: bin/test/test_shared_ptr bin/test/test_arena
	@echo make success

bin/test/test_shared_ptr: test/test_shared_ptr.cpp
	g++ test/test_shared_ptr.cpp -I . -I third_party/googletest/googletest/include -L third_party/googletest/lib -lgtest -lpthread  -o bin/test/test_shared_ptr

bin/test/test_arena: test/test_arena.cpp
	g++ test/test_arena.cpp utils/arena.cpp -I . -I third_party/googletest/googletest/include -L third_party/googletest/lib -lgtest -lpthread -o bin/test/test_arena

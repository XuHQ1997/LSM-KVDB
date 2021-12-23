bin/test/test_memtable.o: utils/arena.cpp test/test_memtable.cpp lsm/memtable.cpp lsm/memtable.h utils/shared_ptr.h utils/arena.h lsm/status.h
	g++ --std=c++11 utils/arena.cpp test/test_memtable.cpp lsm/memtable.cpp -I. -Ithird_party/googletest/googletest/include -Lthird_party/googletest/lib -lgtest -lpthread -o bin/test/test_memtable.o

bin/test/test_arena.o: utils/arena.cpp test/test_arena.cpp utils/arena.h
	g++ --std=c++11 utils/arena.cpp test/test_arena.cpp -I. -Ithird_party/googletest/googletest/include -Lthird_party/googletest/lib -lgtest -lpthread -o bin/test/test_arena.o

bin/test/test_shared_ptr.o: test/test_shared_ptr.cpp utils/shared_ptr.h
	g++ --std=c++11 test/test_shared_ptr.cpp -I. -Ithird_party/googletest/googletest/include -Lthird_party/googletest/lib -lgtest -lpthread -o bin/test/test_shared_ptr.o

.PHONEY: all
all: bin/test/test_memtable.o bin/test/test_arena.o bin/test/test_shared_ptr.o
	@echo make success

.PHONEY: clean
clean:
	rm bin/test/*


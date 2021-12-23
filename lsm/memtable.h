#ifndef LSM_MEMTABLE_H
#define LSM_MEMTABLE_H

#include <map>
#include <string>

#include "status.h"
#include "utils/shared_ptr.h"
#include "utils/arena.h"

namespace lsm {

class MemTable : public Countable {
public:
    MemTable() 
        : arena_()
        , kvdata_(ArenaAllocator<ValueType>(&arena_)) {}
    ~MemTable() = default;

    Status Get(const std::string& key, std::string& value);
    Status Add(const std::string& key, const std::string& value);
    size_t MemoryUsage(void) const { return arena_.MemoryUsage(); }
private:
    using ValueType = std::map<std::string, std::string>::allocator_type::value_type;

    // TODO: use skiplist as kvdata
    Arena arena_;
    std::map<std::string, std::string, 
             std::less<std::string>, 
             ArenaAllocator<ValueType>> kvdata_;
};

using MemTablePtr = SharedPtr<MemTable>;
using CMemTablePtr = SharedPtr<const MemTable>;

}  // namespace lsm
#endif

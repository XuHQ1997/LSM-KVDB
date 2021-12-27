#ifndef LSM_LSM_TREE_H
#define LSM_LSM_TREE_H

#include <string>
#include <vector>
#include "utils/slice.h"
#include "status.h"
#include "memtable.h"
#include "sstable.h"
#include "level.h"

namespace lsm {

const size_t kMemTableMemoryLimit = 1000000;

class LSMTree {
public:
    LSMTree(std::string data_dir);
    ~LSMTree() = default;

    Status Get(const Slice& key, std::string& value);
    Status Put(const Slice& key, const Slice& value);
private:
    Status MakeRoomForWrite();

    std::string data_dir_;

    MemTablePtr mem_;
    MemTablePtr imm_;

    std::vector<LevelPtr> levels_;
};
}  // namespace lsm
#endif
#ifndef LSM_LSM_TREE_H
#define LSM_LSM_TREE_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "utils/slice.h"
#include "utils/shared_ptr.h"
#include "status.h"
#include "memtable.h"
#include "sstable.h"
#include "level.h"
#include "compaction.h"

namespace lsm {

const size_t kMemTableMemoryLimit = 1000000;
const size_t kDelayWriteL0 = 7;
const size_t kMaxNumFilesL0 = 10;
const int64_t kDelayMicroSeconds = 1000;


class LSMTree: public Countable {
public:
    LSMTree(std::string data_dir);
    ~LSMTree();

    Status Get(const Slice& key, std::string& value);
    Status Put(const Slice& key, const Slice& value);

    friend class Compaction;
private:
    Status MakeRoomForWrite();

    std::string data_dir_;
    bool stop_;

    MemTablePtr mem_;
    MemTablePtr imm_;
    std::vector<LevelPtr> levels_;

    Compaction compaction_;
};

using LSMTreePtr = SharedPtr<LSMTree>;
using CLSMTreePtr = SharedPtr<const LSMTree>;
}  // namespace lsm
#endif
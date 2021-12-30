#ifndef LSM_LSM_TREE_H
#define LSM_LSM_TREE_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
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

using version_t = uint64_t;

class LSMTree: public Countable {
public:
    LSMTree(std::string data_dir);
    ~LSMTree();

    Status Get(const Slice& key, std::string& value);
    Status Put(const Slice& key, const Slice& value);

    class Snapshot;
    Snapshot MakeSnapshot(void);

    friend class Compaction;
private:
    Status MakeRoomForWrite(void);

    std::string data_dir_;
    std::atomic_bool stop_;
    version_t ver_;

    MemTablePtr mem_;
    MemTablePtr imm_;
    std::vector<LevelPtr> levels_;

    Compaction compaction_;
};

using LSMTreePtr = SharedPtr<LSMTree>;
using CLSMTreePtr = SharedPtr<const LSMTree>;

class LSMTree::Snapshot {
private:
   Snapshot(version_t ver, 
            const MemTablePtr& mem, 
            const MemTablePtr& imm, 
            std::vector<std::vector<SSTablePtr>>& levels);
   friend class LSMTree;

    version_t ver;
    MemTablePtr mem;
    MemTablePtr imm;
    std::vector<std::vector<SSTablePtr>> levels;
};
}  // namespace lsm
#endif
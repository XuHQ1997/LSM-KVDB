#include <cassert>
#include "lsm_tree.h"

namespace lsm {

LSMTree::LSMTree(std::string data_dir)
        : data_dir_(std::move(data_dir))
        , stop_(false)
        , compaction_(this) {
    compaction_.Start();
}

LSMTree::~LSMTree() {
    stop_.store(true, std::memory_order_relaxed);
    compaction_.Notify();
    compaction_.BlockDie();
}

Status LSMTree::Get(const Slice& key, std::string& value) {
    Status status;
    Snapshot snapshot = MakeSnapshot();

    // memtable should prove thread-safe by itself.
    status = snapshot.mem->Get(key, value);
    if (status.ok() || status.isKeyDeleted()) {
        return status;
    }
    assert(status.isKeyNotFound());

    if (snapshot.imm) {
        status = snapshot.imm->Get(key, value);
        if (status.ok() || status.isKeyDeleted()) {
            return status;
        }
        assert(status.isKeyNotFound());
    }

    for (std::vector<SSTablePtr> level : snapshot.levels) {
        // status = level->Get(key, value);
        // if (status.ok() || status.isKeyDeleted()) {
        //     return status;
        // }
    }
    status = Status::kKeyNotFound;
    return status;
}

Status LSMTree::Put(const Slice& key, const Slice& value) {
    Status status = MakeRoomForWrite();
    if (!status.ok()) {
        return status;
    }

    status = mem_->Put(key, value);
    return status;
}

// TODO: need protected by compaction_mu_
Status LSMTree::MakeRoomForWrite() {
    Status status = Status::OK();
    
    size_t size_l0;
    {
        // TODO: Better way to get the numbers of L0 files? 
        auto lck = compaction_.GetLock();
        size_l0 = levels_[0]->size();
    }

    if (size_l0 >= kDelayWriteL0) {
        // nearly hit the hard limit
        compaction_.DelayWrite();
    } else if (mem_->MemoryUsage() < kMemTableMemoryLimit) {
        // everything is fine
    } else if (imm_) {
        compaction_.BlockWrite();
    } else {
        auto lck = compaction_.GetLock();
        imm_.swap(mem_);
        compaction_.need_major_compaction_.store(
            true, std::memory_order_release);
        mem_ = MemTablePtr(new MemTable);
        compaction_.Notify();
    }
    return status;
}

LSMTree::Snapshot LSMTree::MakeSnapshot(void) {
    std::vector<std::vector<SSTablePtr>> tables;
    tables.reserve(levels_.size());

    auto lck = compaction_.GetLock();
    for (LevelPtr& ptr : levels_) {
        tables.emplace_back(ptr->tables());
    }

    return Snapshot(ver_, mem_, imm_, tables);
}

}  // namespace lsm

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
    stop_ = true;
    compaction_.Notify();
    compaction_.BlockDie();
}

Status LSMTree::Get(const Slice& key, std::string& value) {
    Status status;

    status = mem_->Get(key, value);
    if (status.ok() || status.isKeyDeleted()) {
        return status;
    }
    assert(status.isKeyNotFound());

    if (imm_) {
        status = imm_->Get(key, value);
        if (status.ok() || status.isKeyDeleted()) {
            return status;
        }
        assert(status.isKeyNotFound());
    }

    for (LevelPtr& level : levels_) {
        status = level->Get(key, value);
        if (status.ok() || status.isKeyDeleted()) {
            return status;
        }
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
    if (levels_[0]->size() >= kDelayWriteL0) {
        // nearly hit the hard limit
        compaction_.DelayWrite();
    } else if (mem_->MemoryUsage() < kMemTableMemoryLimit) {
        // everything is fine
    } else if (imm_) {
        compaction_.BlockWrite();
    } else {
        imm_.swap(mem_);
        mem_ = MemTablePtr(new MemTable);
        compaction_.Notify();
    }
    return status;
}
}  // namespace lsm

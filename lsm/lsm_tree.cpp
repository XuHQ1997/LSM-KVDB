#include <cassert>
#include "lsm_tree.h"

namespace lsm {

LSMTree::LSMTree(std::string data_dir)
    : data_dir_(std::move(data_dir)) {}

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

Status LSMTree::MakeRoomForWrite() {
    Status status = Status::OK();
    while (true) {
        if (mem_->MemoryUsage() < kMemTableMemoryLimit) {
            break;
        } else if (!imm_) {
            imm_.swap(mem_);
            mem_ = MemTablePtr(new MemTable());
            // TODO: Need notify compaction thread
            break;
        } else {
            // TODO: delay write if necessary
        }
    }
    return status;
}
}  // namespace lsm

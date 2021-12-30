#include <thread>
#include "compaction.h"
#include "lsm_tree.h"

namespace lsm {

void Compaction::BackgroundCompaction(void) {
    while (true) {
        if (lsmt_->stop_.load(std::memory_order_relaxed)) {
            // main thread stop
            break;
        } else if (!worker_status_.ok()) {
            // backgroud err
            break;
        } else if (lsmt_->levels_[0]->size() >= kMaxNumFilesL0) {
            // hit the hard limit of L0 files
            MinorCompactionL0();
        }else if (need_major_compaction_.load(std::memory_order_acquire)) {
            // compaction imm
            MajorCompaction();
            Notify();
        }  else if (need_minor_compaction_) {
            MinorCompaction();
        } else {
            PickCompactionLevel();

            auto is_busy = [this]() {
                return lsmt_->stop_  || need_minor_compaction_
                    || need_major_compaction_.load(std::memory_order_release);
            };
            if (!is_busy()) {
                std::unique_lock<std::mutex> lck(mu_);
                cv_.wait(lck, is_busy);
            }
        }
    }
    dead_ = true;
    Notify();
}

void Compaction::MajorCompaction(void) {

}

void Compaction::MinorCompactionL0(void) {

}

void Compaction::MinorCompaction(void) {

}
}  // namespace lsm
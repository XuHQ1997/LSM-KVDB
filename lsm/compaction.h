#ifndef LSM_COMPACTION_H
#define LSM_COMPACTION_H

#include <mutex>
#include <thread>
#include "status.h"

namespace lsm {

class LSMTree;

class Compaction {
public:
    Compaction(LSMTree* lsmt)
        : lsmt_(lsmt)
        , dead_(false)
        , need_major_compaction_(false)
        , need_minor_compaction_(false) {}
    ~Compaction() = default;

    // Start backgroud compaction.
    void Start(void) {
        std::thread worker(BackgroundCompaction, this);
        worker.detach();
    }
    
    // Awake the other thread.
    void Notify(void) { 
        cv_.notify_one(); 
    }

    // Block main thread to wait compaction of immtable.
    void BlockWrite(void) {
        std::unique_lock<std::mutex> lck(mu_);
        cv_.wait(lck, [this](){ return !lsmt_->imm_; });
    }

    // Block main thread to wait worker thread dead.
    void BlockDie(void) {
        std::unique_lock<std::mutex> lck(mu_);
        cv_.wait(lck, [this](){ return dead_; });
    }

    // delay main thread.
    void DelayWrite(void) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(kDelayMicroSeconds));
    }
private:
    // Worker entry function.
    void BackgroundCompaction(void);

    // Compaction of imm.
    void MajorCompaction(void);

    // Compaction of level0.
    void MinorCompactionL0(void);

    // Compaction of levels
    void MinorCompaction(void);

    // Choose a level to compact.
    void PickCompactionLevel(void);

private:
    LSMTree* lsmt_;
    
    std::mutex mu_;
    std::condition_variable cv_;

    Status worker_status_;
    bool dead_;
    bool need_major_compaction_;
    bool need_minor_compaction_;
};
}  // namespace lsm
#endif
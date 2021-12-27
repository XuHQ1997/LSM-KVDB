#ifndef LSM_SSTABLE_H
#define LSM_SSTABLE_H

#include "status.h"
#include "memtable.h"
#include "utils/shared_ptr.h"

namespace lsm {
// forward declaration
class SSTable;
using SSTablePtr = SharedPtr<SSTable>;
using CSSTablePtr = SharedPtr<const SSTable>;

class SSTable : public Countable {
public:
    static SSTablePtr FromMem(CMemTablePtr imm);
    static SSTablePtr FromMerge(const std::vector<CSSTablePtr>&);
private:
    SSTable() = default;
    ~SSTable() = default;
};
}  // namespace lsm
#endif

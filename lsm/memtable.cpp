#include "memtable.h"

namespace lsm {

Status MemTable::Get(const std::string& key, std::string& value) {
    auto iter = kvdata_.find(key);
    if (iter == kvdata_.end()) {
        return Status::KeyNotFound();
    }

    value = iter->second;
    return Status::OK();
}

Status MemTable::Add(const std::string& key, const std::string& value) {
    kvdata_[key] = value;
    return Status::OK();
}

}  // namespace lsm

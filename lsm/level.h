#ifndef LSM_LEVEL_H
#define LSM_LEVEL_H

#include <string>
#include <vector>
#include "utils/shared_ptr.h"
#include "status.h"

namespace lsm {

class Level : Countable {
public:
    Level(int level);
    ~Level();

    const std::vector<SSTablePtr>& tables(void);
    size_t size(void);

    Status Get(const Slice& key, std::string& value);
private:
    std::vector<SSTablePtr> tables_;
};

using LevelPtr = SharedPtr<Level>;
using CLevelPtr = SharedPtr<const Level>;
}  // namespace lsm 
#endif
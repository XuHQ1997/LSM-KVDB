#ifndef LSM_LEVEL_H
#define LSM_LEVEL_H

#include <string>
#include "utils/shared_ptr.h"
#include "status.h"

namespace lsm {

class Level : Countable {
public:
    Level(int level);
    ~Level();

    Status Get(const Slice& key, std::string& value);
    Status Put(const Slice& key, const Slice& value);
};

using LevelPtr = SharedPtr<Level>;
using CLevelPtr = SharedPtr<const Level>;
}  // namespace lsm 
#endif
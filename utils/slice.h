#ifndef UTILS_SLICE_H
#define UTILS_SLICE_H

#include <cstring>
#include <string>

namespace lsm {

// When you don't want to own the string, use Slice.
// Otherwise, use std::string.
class Slice {
public:
    Slice(const std::string& str)
        : data_(str.c_str()), size_(str.length()) {}
    Slice(const char* data)
        : data_(data), size_(strlen(data)) {}
    
    const char* data(void) const { return data_; }
    size_t length(void) const { return size_; }
private:
    const char* data_;
    size_t size_;
};
}  // namespace lsm
#endif
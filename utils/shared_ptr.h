#ifndef UTILS_REFCOUNT_H
#define UTILS_REFCOUNT_H

namespace lsm {

class Countable {
private:
    template<typename T> friend class SharedPtr;
    mutable int refcount_;
};

template<typename T>
class SharedPtr {
public:
    SharedPtr(void) : obj_(nullptr) {}
    SharedPtr(T* obj) : obj_(obj) {
        if (obj_ != nullptr) {
            ref();
        }
    }
    
    SharedPtr(const SharedPtr& other) 
        : SharedPtr(other.obj_) {}
    SharedPtr(SharedPtr&& other)
        : obj_(other.release()) {}
    ~SharedPtr() { 
        if (obj_ != nullptr) { 
            unref();
        }
    }

    const SharedPtr& operator=(const SharedPtr& rhs) {
        SharedPtr ptr(rhs);
        this->swap(ptr);
        return *this;
    }

    T* get(void) { return obj_; }
    T* release(void) {
        T* obj = obj_;
        obj_ = nullptr;
        return obj;
    }

    T* operator->(void) { return obj_; }
    operator bool() const { return obj_ != nullptr; }
    void swap(SharedPtr& rhs) { std::swap(this->obj_, rhs.obj_); }

private:
    void ref() {
        ++obj_->refcount_;
    }
    void unref() {
        --obj_->refcount_;
        if (obj_->refcount_ == 0) {
            delete get();
        }
    }

private:
    T* obj_;
};

}  // namespace lsm
#endif

#ifndef LSM_STATUS_H
#define LSM_STATUS_H

#include <iostream>

namespace lsm {

class Status {
public:
    enum Code {
        kOK = 0,
        kKeyNotFound = 1,
        kKeyDeleted = 2,
    };

    Status() = default;
    Status(Code code, char* msg=nullptr) 
        : code_(code), msg_(msg) {}

    static Status OK(char* msg=nullptr) { return Status(kOK, msg); }
    static Status KeyNotFound(char* msg=nullptr) { return Status(kKeyNotFound, msg); }
    static Status KeyDeleted(char* msg=nullptr) { return Status(kKeyDeleted, msg); }
    
    bool ok(void) const { return code_ == kOK; }
    bool isKeyNotFound(void) const { return code_ == kKeyNotFound; }
    bool isKeyDeleted(void) const { return code_ == kKeyDeleted; }

    Status& operator=(const Status& rhs) = default;
    Status& operator=(Code rhs);

    friend std::ostream& operator<<(std::ostream& os, const Status& s);
    friend bool operator==(const Status& lhs, Code rhs);
    friend bool operator==(Code lhs, const Status& rhs);
    friend bool operator==(const Status& lhs, const Status& rhs);
private:
    Code code_;
    char* msg_;
};

inline Status& Status::operator=(Code rhs) {
    code_ = rhs;
    return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Status& s) {
    const static char* CODE2MSG[] = {
        "OK",
        "KeyNotFound",
    };

    os << '(' << CODE2MSG[s.code_];
    if (s.msg_) {
        os << ", " << s.msg_;
    }
    os << ')';
    return os;
}

inline bool operator==(const Status& lhs, Status::Code rhs) { 
    return lhs.code_ == rhs; 
}
inline bool operator==(Status::Code lhs, const Status& rhs) { 
    return lhs == rhs.code_; 
}
inline bool operator==(const Status& lhs, const Status& rhs) { 
    return lhs.code_ == rhs.code_; 
}

}  // namespace lsm
#endif
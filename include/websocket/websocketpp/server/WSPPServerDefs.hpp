#pragma once
#include "IConnectionProvider.hpp"
#include <memory>

class Class {
public:
    virtual ~Class() = default;
};

using remote_created_callback = std::function<void(std::unique_ptr<IConnectionProvider>)>;

enum EOpCode {
    OP_CODE_TEXT,
    OP_CODE_BINARY,
    UNKNOWN
};

#pragma once

#include <iostream>
#include <cstdint>
#include "../../display/include/Display.h"
#include "../../system/include/System.h"

class Page {
private:
    

public:
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void tick() = 0;
    virtual bool wantsToExit() = 0;
    virtual Page* nextPage() = 0;
    virtual ~Page() {}
};

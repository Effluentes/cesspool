#pragma once

namespace communication::radio::transceivers {
class CC1101
{
public:
    CC1101();
    ~CC1101();
    bool initialize();
};
}
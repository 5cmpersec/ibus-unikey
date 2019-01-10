#pragma once

#include <string>
#include "base/port.h"


class Version {
public:

    static std::string GetUnikeyVersion();

    static int GetUnikeyVersionMajor();
    static int GetUnikeyVersionMinor();
    static int GetUnikeyVersionBuildNumber();
    static int GetUnikeyVersionRevision();
    static const char *GetUnikeyEngineVersion();

    // Returns true if lhs is less than rhs in the lexical order.
    // CompareVersion("1.2.3.4", "1.2.3.4") => false
    // CompareVersion("1.2.3.4", "5.2.3.4") => true
    // CompareVersion("1.25.3.4", "1.2.3.4") => false
    static bool CompareVersion(const std::string &lhs, const std::string &rhs);


private:
    DISALLOW_COPY_AND_ASSIGN(Version);
};


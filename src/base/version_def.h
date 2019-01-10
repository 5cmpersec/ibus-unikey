#pragma once

namespace version {

// Let a linker deduplicate the multiple definitions of kUnikeyVersion if any.
const char * const kUnikeyVersion = "0.6.3";

const int kUnikeyVersionMajor = 0;
const int kUnikeyVersionMinor = 6;
const int kUnikeyVersionBuildNumber = 3;
const int kUnikeyVersionRevision = 4;

const char * const kUnikeyEngineVersion = "10";


}  // namespace version
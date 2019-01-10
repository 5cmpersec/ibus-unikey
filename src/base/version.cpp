#include "base/version.h"

#include <sstream>
#include <vector>

#include "base/logging.h"
#include "base/version_def.h"


namespace {
bool StringAsIntegerComparator(const std::string &lhs, const std::string &rhs) {
  return std::stoi(lhs) < std::stoi(rhs);
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

}  // namespace

std::string Version::GetUnikeyVersion() {
  return version::kUnikeyVersion;
}

int Version::GetUnikeyVersionMajor() {
  return version::kUnikeyVersionMajor;
}

int Version::GetUnikeyVersionMinor() {
  return version::kUnikeyVersionMinor;
}

int Version::GetUnikeyVersionBuildNumber() {
  return version::kUnikeyVersionBuildNumber;
}

int Version::GetUnikeyVersionRevision() {
  return version::kUnikeyVersionRevision;
}

const char *Version::GetUnikeyEngineVersion() {
  return version::kUnikeyEngineVersion;
}

bool Version::CompareVersion(const std::string &lhs, const std::string &rhs) {
  if (lhs == rhs) {
    return false;
  }
  if (lhs.find("Unknown") != std::string::npos ||
      rhs.find("Unknown") != std::string::npos) {
    BLOG_WARNING("Unknown is given as version");
    return false;
  }
  std::vector<std::string> vlhs = split(lhs, '.');
  std::vector<std::string> vrhs = split(rhs, '.');

  return std::lexicographical_compare(vlhs.begin(), vlhs.end(), vrhs.begin(),
                                      vrhs.end(), StringAsIntegerComparator);
}


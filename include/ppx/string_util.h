#ifndef PPX_STRING_UTIL_H
#define PPX_STRING_UTIL_H

#include <string>

namespace ppx {
namespace string_util {

void TrimLeft(std::string& s);
void TrimRight(std::string& s);

std::string TrimCopy(const std::string& s);

} // namespace string_util
} // namespace ppx

#endif // PPX_STRING_UTIL_H
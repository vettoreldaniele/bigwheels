#include "ppx/string_util.h"

#include <algorithm>
#include <cctype>

namespace ppx {
namespace string_util {

void TrimLeft(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void TrimRight(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string TrimCopy(const std::string& s)
{
  std::string sc = s;
  TrimLeft(sc);
  TrimRight(sc);
  return sc;
}

} // namespace string_util
} // namespace ppx
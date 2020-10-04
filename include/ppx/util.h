#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <cstdint>
#include <vector>

namespace ppx {

template <typename T>
bool IsNull(const T* ptr)
{
    bool res = (ptr == nullptr);
    return res;
}

template <typename T>
T InvalidValue(const T value = static_cast<T>(~0))
{
    return value;
}

template <typename T>
T RoundUp(T value, T multiple)
{
    static_assert(
        std::is_integral<T>::value,
        "T must be an integral type");

    assert(multiple && ((multiple & (multiple - 1)) == 0));
    return (value + multiple - 1) & ~(multiple - 1);
}

template <typename T>
uint32_t CountU32(const std::vector<T>& container)
{
    uint32_t n = static_cast<uint32_t>(container.size());
    return n;
}

template <typename T>
const T* DataPtr(const std::vector<T>& container)
{
    const T* ptr = container.empty() ? nullptr : container.data();
    return ptr;
}

template <typename T>
uint32_t SizeInBytesU32(const std::vector<T>& container)
{
    uint32_t size = static_cast<uint32_t>(container.size() * sizeof(T));
    return size;
}

template <typename T>
uint64_t SizeInBytesU64(const std::vector<T>& container)
{
    uint64_t size = static_cast<uint64_t>(container.size() * sizeof(T));
    return size;
}

template <typename T>
bool IsIndexInRange(uint32_t index, const std::vector<T>& container)
{
    uint32_t n   = CountU32(container);
    bool     res = (index < n);
    return res;
}

template <typename T>
bool ElementExists(const T& elem, const std::vector<T>& container)
{
    auto it = std::find(std::begin(container), std::end(container), elem);
    bool exists = (it != std::end(container));
    return exists;
}

template <typename T>
bool GetElement(uint32_t index, const std::vector<T>& container, T* pElem)
{
    if (!IsIndexInRange(index, container)) {
        return false;
    }
    *pElem = container[index];
    return true;
}

template <typename T>
void AppendElements(const std::vector<T>& elements, std::vector<T>& container)
{
    if (!elements.empty()) {
        std::copy(
            std::begin(elements),
            std::end(elements),
            std::back_inserter(container));
    }
}

template <typename T>
void RemoveElement(const T& elem, std::vector<T>& container)
{
    container.erase(
        std::remove(std::begin(container), std::end(container), elem),
        container.end());
}

template <typename T>
void Unique(std::vector<T>& container)
{
    auto it = std::unique(std::begin(container), std::end(container));
    container.resize(std::distance(std::begin(container), it));
}

inline std::vector<const char*> GetCStrings(const std::vector<std::string>& container)
{
    std::vector<const char*> cstrings;
    for (auto& str : container) {
        cstrings.push_back(str.c_str());
    }
    return cstrings;
}

inline std::vector<std::string> GetNotFound(const std::vector<std::string>& search, const std::vector<std::string>& container)
{
    std::vector<std::string> result;
    for (auto& elem : search) {
        auto it = std::find(
            std::begin(container),
            std::end(container),
            elem);
        if (it == std::end(container)) {
            result.push_back(elem);
        }
    }
    return result;
}

} // namespace ppx

#endif // UTIL_H

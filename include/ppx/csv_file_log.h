#ifndef PPX_CSV_FILE_LOG_H
#define PPX_CSV_FILE_LOG_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <sstream>

const std::string PPX_DEFAULT_CSV_FILE{"stats.csv"};

namespace ppx {

//! @class CSVFileLog
//!
//!
class CSVFileLog
{
public:
    CSVFileLog();
    CSVFileLog(const std::string& filepath);
    ~CSVFileLog();

    template <typename T>
    CSVFileLog& operator<<(const T& value)
    {
        mBuffer << value;
        return *this;
    }

    template <typename T>
    void LogField(const T& value)
    {
        (*this) << value << ",";
    }
    template <typename T>
    void LastField(const T& value)
    {
        (*this) << value << "\n";
    }
    void NewLine()
    {
        (*this) << "\n";
    }

private:
    void Write(const char* msg);
    void Lock();
    void Unlock();
    void Flush();

private:
    std::string       mFilePath;
    std::ofstream     mFileStream;
    std::stringstream mBuffer;
    std::mutex        mWriteMutex;
};

} // namespace ppx

#endif // PPX_CSV_FILE_LOG_H

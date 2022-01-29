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

    void Restart(const std::string& filepath);

    template <typename T>
    CSVFileLog& operator<<(const T& value)
    {
        mBuffer << value;
        return *this;
    }

    template <typename T>
    void logField(const T& value)
    {
        Lock();
        (*this) << value << ",";
        Flush();
        Unlock();
    }
    template <typename T>
    void lastField(const T& value)
    {
        Lock();
        (*this) << value << "\n";
        Flush();
        Unlock();
    }
    void newLine()
    {
        Lock();
        (*this) << "\n";
        Flush();
        Unlock();
    }

private:
    void Lock();
    void Unlock();
    void Flush();
    void Write(const char* msg);

private:
    std::string       mFilePath;
    std::ofstream     mFileStream;
    std::stringstream mBuffer;
    std::mutex        mWriteMutex;
};

} // namespace ppx



#endif // PPX_CSV_FILE_LOG_H

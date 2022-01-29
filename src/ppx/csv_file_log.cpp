#include "ppx/csv_file_log.h"

namespace ppx {

CSVFileLog::CSVFileLog()
    : CSVFileLog(PPX_DEFAULT_CSV_FILE)
{

}

void CSVFileLog::Restart(const std::string& filepath)
{
    // Close previoust stream
    Flush();
    if (mFileStream.is_open()) {
        mFileStream.close();
    }
    // Open a new one
    mFilePath = filepath;
    if (!mFilePath.empty()) {
        mFileStream.open(mFilePath.c_str());
    }
}


CSVFileLog::CSVFileLog(const std::string& filepath)
{
    Restart(filepath);
}

CSVFileLog::~CSVFileLog()
{
    if (mFileStream.is_open()) {
        mFileStream.close();
    }
}

void CSVFileLog::Lock()
{
    mWriteMutex.lock();
}

void CSVFileLog::Unlock()
{
    mWriteMutex.unlock();
}

void CSVFileLog::Write(const char* msg)
{
    if (mFileStream.is_open()) {
        mFileStream << msg;
    }
}
void CSVFileLog::Flush()
{
    // Write anything that's in the buffer
    Write(mBuffer.str().c_str());

    // Signal flush for file
    if (mFileStream.is_open()) {
        mFileStream.flush();
    }

    // Clear buffer
    mBuffer.str(std::string());
    mBuffer.clear();
}

} // namespace ppx

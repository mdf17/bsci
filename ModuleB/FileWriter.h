#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <fstream>

#include "Common.h"


class FileWriter : public QObject
{
    Q_OBJECT

  public:
    FileWriter(const std::string id, const SharedQueue<QByteArray>& buffer, QObject *parent = 0);
    ~FileWriter();

    // "bootstrap" function
    // reads config params
    // sets up output stream
    // launches writeBlocks()
    void init();

    // Core function of the class
    // 
    void writeBlocks();

    SharedQueue<QByteArray> frameBuffer;

  private:
    std::string m_id;

    unsigned int m_bytesWritten;
    unsigned int m_maxFileSize;

    // Use ostream to make it possible
    // to switch between writing to a file
    // and writing to stdout
    bool m_useStdOut;
    std::streambuf * m_buf;
    std::ofstream m_ofstream;
    std::ostream *m_writer;

};

#endif

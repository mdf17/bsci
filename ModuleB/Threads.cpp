#include "Threads.h"


FileWriterThread::FileWriterThread(const std::string consumerId, std::shared_ptr<ThreadSafeQueue<QByteArray>> buffer, QObject *parent)
    : QThread(parent), m_id(consumerId)
{
    frameBuffer = buffer;

    std::string fileName = "./data/com_mod_output_" + m_id + ".bin";
    m_outputFile.open(fileName, std::ios::out | std::ios::binary);
}

FileWriterThread::~FileWriterThread()
{
    m_outputFile.close();
}

void FileWriterThread::processNextBlock()
{
    std::cout << "FileWriter::processNextBlock()" << std::endl;
    if(frameBuffer->size() == 0)
        return;


    QByteArray block = frameBuffer->pop_front();

    // calc block size
    // calc number of frames in block
    //
    // for each frame
    //   parse header
    //   for each channel
    //     parse checksum
    //   write to file
    
}

void FileWriterThread::run()
{
    std::cout << "FileWriterThread::run()" << std::endl;
    while (true) {
        processNextBlock();
    }
}

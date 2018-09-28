#include "Threads.h"


FileWriterThread::FileWriterThread(const std::string consumerId, SharedQueue<QByteArray> buffer, QObject *parent)
    : QThread(parent), m_id(consumerId)
{
    std::cout << "FileWriterThread(" << m_id << ")" << std::endl;
    frameBuffer = buffer;

    std::string fileName = "./data/com_mod_output_" + m_id + ".bin";
    m_outputFile = fopen(fileName.c_str(), "wb");
}

FileWriterThread::~FileWriterThread()
{
    fclose(m_outputFile);
}

void FileWriterThread::processNextBlock()
{
    if(frameBuffer->size() == 0)
        return;


    QByteArray block = frameBuffer->pop_front();
    QDataStream stream(block);
    stream.setVersion(QDataStreamVersion);

    ChecksumT checksum;
    while (!stream.atEnd()) {
        for (unsigned int i = 0; i < NUM_CHANNELS; ++i) {
            stream >> checksum.sum[i];
        }
        stream >> checksum.timestamp;
        // bail out
        if (stream.status() != QDataStream::Ok) {
            std::cout << "FileWriter::processNextBlock(): Error reading data stream" << std::endl;
            break;
        }

        //std::cout << "t=" << checksum.timestamp << ": " << checksum.sum[0] << ", " << checksum.sum[7] << std::endl;

        fwrite(&checksum, sizeof(ChecksumT), 1, m_outputFile);
    }
}

void FileWriterThread::run()
{
    while (true) {
        processNextBlock();
    }
}

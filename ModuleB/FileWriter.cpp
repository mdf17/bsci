#include "FileWriter.h"



std::ostream & operator << (std::ostream &out, const ChecksumT &c)
{
    out << c.timestamp << std::endl;
    for (size_t i = 0; i < NUM_CHANNELS; ++i)
        out << c.sum[i] << std::endl;
    out << std::endl;
    return out;
}


FileWriter::FileWriter(const std::string consumerId, const SharedQueue<QByteArray>& buffer, QObject *parent)
    : QObject(parent),
      m_id(consumerId),
      m_bytesWritten(0),
      m_maxFileSize(MAX_FILE_SIZE),
      m_useStdOut(false)
{
    std::cout << "FileWriter(" << m_id << ")" << std::endl;
    frameBuffer = buffer;
}

void FileWriter::close()
{
    emit finished();
}

FileWriter::~FileWriter()
{
}

void FileWriter::init()
{
    // lookup max file size
    std::string sMaxFileSize;
    if (Config::instance()->lookup("maxFileSize", sMaxFileSize)) {
        m_maxFileSize = (unsigned int) atoi(sMaxFileSize.c_str());
        std::cout << "FileWriter::init(): Max File Size = " << m_maxFileSize << " bytes" << std::endl;
    }

    std::string sOutputPath;
    if (!Config::instance()->lookup("outputPath", sOutputPath)) {
         sOutputPath = "./data/";
    } else {
        if (sOutputPath.back() != '/') {
            sOutputPath.append("/");
        }
    }

    std::string sUseStdOut;
    // use stdout
    if (!Config::instance()->lookup("useStdOut", sUseStdOut)) {
        m_useStdOut = false;
    } else {
        m_useStdOut = true;
    }

    std::string fileName = sOutputPath + "com_mod_output_" + m_id + ".bin";
    if (m_useStdOut) {
        std::cout << "Using stdout!" << std::endl;
    } else {
        m_outFile = fopen(fileName.c_str(), "wb");
    }
    writeBlocks();
}

void FileWriter::writeBlocks()
{
    QByteArray block;
    while (m_bytesWritten <= m_maxFileSize) {
        if(!frameBuffer->getElementNonblocking(block))
            continue;

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

            if (m_useStdOut) {
                std::cout << checksum;
            }

            else {
                fwrite(&checksum.timestamp, sizeof(double), 1, m_outFile);
                fwrite(&checksum.sum, sizeof(unsigned int), 8, m_outFile);
            }

        }
        if (stream.status() == QDataStream::Ok) {
            m_bytesWritten += block.size();
        }
    }
    std::cout << "Done writing file!" << std::endl;
    fclose(m_outFile);
    m_outFile = NULL;
}

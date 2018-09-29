#include "FileWriter.h"


// separator string (if piping to stdout)
std::string separator = "";

std::ostream & operator << (std::ostream &out, const ChecksumT &c)
{
    out << c.timestamp << separator;
    for (size_t i = 0; i < NUM_CHANNELS; ++i)
        out << c.sum[i] << separator;
    out << separator;
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
    m_ofstream.close();
    delete m_buf;
    delete m_writer;
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
        m_buf = std::cout.rdbuf();
    } else {
        m_ofstream.open(fileName, std::ios::out | std::ios::binary);
        m_buf = m_ofstream.rdbuf();
    }
    if (m_useStdOut) {
        separator = "\n";
    }
    m_writer = new std::ostream(m_buf);
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

            *m_writer << checksum;
        }
        if (stream.status() == QDataStream::Ok) {
            m_bytesWritten += block.size();
        }
    }
}

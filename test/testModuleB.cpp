#include <vector>
#include <iostream>
#include <fstream>

#include "Common.h"

int main(int argc, char *argv[])
{
    std::string testFile(argv[1]);

    std::string serverFile = "com_mod_output_truth.bin";
    std::vector<std::string> files;
    files.push_back(serverFile);
    files.push_back(testFile);

    std::vector< std::vector<ChecksumT> > data(2);

    std::vector<unsigned int> numElements(2);

    //for (size_t i = 0; i < 2; i++) {
    //    std::ifstream ifs(files[i], std::ios::in | std::ios::binary);
    //    std::cout << "opening file " << files[i] << std::endl;
    //    ifs.seekg(0, std::ios::end);
    //    numElements[i] = ifs.tellg() / sizeof(ChecksumT);
    //    std::cout << "num elements = " << numElements[i] << std::endl;
    //    ifs.seekg(0, std::ios::beg);
    //    for (size_t j = 0; j < numElements[i]; ++j) {
    //        double timestamp;
    //        ChecksumDataT sum;
    //        ChecksumT checksum;

    //        ifs >> timestamp;
    //        std::cout << timestamp << ": " << std::endl;
    //        for (int k = 0; k < 8; k++) {
    //            ifs >> sum[k];
    //            std::cout << sum[k] << " ";
    //        }
    //        std::cout << std::endl;
    //        checksum.timestamp = timestamp;
    //        checksum.sum = sum;
    //        data[i].push_back(checksum);
    //        ifs.close();
    //    }
    //}
    
    //ifs.seekg(0, std::ios::end);
    //unsigned int N = ifs.tellg() / sizeof(ChecksumT);
    //std::cout << "num elements = " << N << std::endl;
    //ifs.seekg(0, std::ios::beg);
    for (int i = 0; i < 2; i++) {
        FILE * file = fopen(files[i].c_str(), "rb");
        std::cout << "opening file " << files[i] << std::endl;
        size_t retval = 0;
        while (true) {
            double timestamp;
            ChecksumDataT sum;
            ChecksumT checksum;

            retval = fread(&timestamp, sizeof(double), 1, file);
            
            if (retval > 0) {
                checksum.timestamp = timestamp;
            } else {
                fclose(file);
                file = NULL;
                break;
            }

            retval = fread(&sum, sizeof(unsigned int), 8, file);

            if (retval > 0) {
                checksum.sum = sum;
            } else {
                fclose(file);
                file = NULL;
                break;
            }

            //std::cout << timestamp << ": " << std::endl;
            for (int k = 0; k < 8; k++) {
                //std::cout << sum[k] << " ";
            }
            //std::cout << std::endl;
            data[i].push_back(checksum);
        }
    }

    for (int i = 0; i < data[0].size(); i++) {

        double serverTime = data[0][i].timestamp;
        ChecksumDataT serverResult = data[0][i].sum;

        //std::cout << "\tt = " << serverTime << std::endl;

        bool found = false;
        for (int k = 0; k < data[1].size(); k++) {
            if (data[1][k].timestamp == serverTime) {
                found = true;

                for (int l = 0; l < 8; l++) {
                    if (data[1][k].sum[l] != serverResult[l]) {
                        std::cout << "Error: Results didn't match at time " << data[1][k].timestamp << " for output file " << testFile << std::endl;
                        return 1;
                    }
                }
                continue;

            }
        }
        if (!found) {
            std::cout << "Error: Never found timestamp " << serverTime << std::endl;
            return 1;
        }
    }
    std::cout << "\t\tall " << data[1].size() << " checksums in test file match truth file" << std::endl;


    return 0;
}

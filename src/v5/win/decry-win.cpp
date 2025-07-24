#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>

const std::string key = "fastcryptor_for_the_win_haha";
const int BUFFER_SIZE = 4096;

bool decryptFile(const std::string& fileName)
{
    std::ifstream inputFile(fileName, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return false;
    }

    if (fileName.length() < 3 || fileName.substr(fileName.length() - 3) != ".fc") {
        return false;
    }

    char extBytes[4];
    inputFile.read(extBytes, 4);

    // فك تشفير البايتات الأربعة الأولى
    for (int i = 0; i < 4; ++i) {
        extBytes[i] ^= key[i % key.length()];
    }

    std::string extension(extBytes, 4);


    // إزالة ".fc" من اسم الملف الأصلي
    std::string decryptedFileName = fileName;
    if (decryptedFileName.size() >= 3 && decryptedFileName.substr(decryptedFileName.size() - 3) == ".fc") {
        decryptedFileName.erase(decryptedFileName.size() - 3, 3);
    }

    // إرفاق الامتداد الأصلي الحقيقي
    decryptedFileName += extension;

    std::ofstream outputFile(decryptedFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error creating output file: " << decryptedFileName << std::endl;
        return false;
    }

    inputFile.seekg(0, std::ios::end);
    size_t totalSize = inputFile.tellg();
    inputFile.seekg(4, std::ios::beg);

    std::vector<char> buffer(BUFFER_SIZE);
    size_t processed = 4;

    size_t global_offset = 4;

    while (inputFile) {
        inputFile.read(buffer.data(), BUFFER_SIZE);
        std::streamsize bytesRead = inputFile.gcount();

        for (std::streamsize i = 0; i < bytesRead; i++) {
            buffer[i] ^= key[(global_offset + i) % key.length()];
        }

        outputFile.write(buffer.data(), bytesRead);
        global_offset += bytesRead;

        processed += bytesRead;
        int percentage = (processed * 100) / totalSize;
        std::cout << "\rDecrypting " << fileName << " - " << percentage << "% complete";
        std::cout.flush();
    }

    std::cout << "\nDecryption complete. Saved as: " << decryptedFileName << std::endl;
    inputFile.close();
    outputFile.close();

    if (std::remove(fileName.c_str()) != 0) {
        std::cerr << "Error deleting file: " << fileName << std::endl;
    }

    return true;
}

void decryptDirectory(const std::string& dirName)
{

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirName.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = dirName + "/" + ent->d_name;
            struct stat st;
            if (stat(fileName.c_str(), &st) == 0) {
                if (S_ISREG(st.st_mode)) {
                    std::cout << "Encoding: " << fileName << std::endl;
                    decryptFile(fileName);
                } else if (S_ISDIR(st.st_mode) && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    decryptDirectory(fileName);
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << dirName << std::endl;
    }
}

int main()
{
    decryptDirectory(".");

    return 0;
}

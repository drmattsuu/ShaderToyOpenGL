#include "FileUtils.h"

#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

std::string LoadFileText(const std::string& filename)
{
    fs::path resourcesPath(fs::current_path());

    if (!fs::is_directory(resourcesPath))
    {
        std::cout << "Unable to open resources directory" << std::endl;
        return std::string();
    }

    fs::path file(resourcesPath / filename);
    if (!fs::exists(file))
    {
        std::cout << "Unable to read resource file: " + filename << std::endl;
        return std::string();
    }

    std::string raw;
    fs::load_string_file(file, raw);
    return raw;
}

std::vector<char> LoadFileBin(const std::string& filename)
{
    using FileBinary = std::vector<char>;
    fs::path resourcesPath(fs::current_path());
    if (!fs::is_directory(resourcesPath))
    {
        std::cout << "Unable to open resources directory" << std::endl;
        return FileBinary();
    }

    fs::path file(resourcesPath / filename);
    if (!fs::exists(file))
    {
        std::cout << "Unable to read resource file: " + filename << std::endl;
        return FileBinary();
    }

    FileBinary result;
    std::fstream binFile(file.string(), std::ios::in | std::ios::binary);
    binFile.seekg(0, binFile.end);
    int length = binFile.tellg();
    binFile.seekg(0, binFile.beg);
    result.resize(length);
    binFile.read(result.data(), length);
    binFile.close();
    return result;
}

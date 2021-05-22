#include "FileUtils.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

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

#pragma once

#include <string>
#include <vector>

std::string LoadFileText(const std::string& filename);

std::vector<char> LoadFileBin(const std::string& filename);

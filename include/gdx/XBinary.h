#pragma once

#include "gdx/XTypes.h"
#include <string>

namespace gdx {

bool parseBinary(const void* data, size_t size, Document& out, std::string& error,
                 ConvertOptions opt = ConvertOptions::none());

} // namespace gdx

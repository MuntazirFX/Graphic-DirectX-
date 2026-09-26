#pragma once

#include "gdx/XTypes.h"
#include <string>

namespace gdx {

bool parseText(const std::string& text, Document& out, std::string& error,
               ConvertOptions opt = ConvertOptions::none());

bool parseBytes(const void* data, size_t size, Document& out, std::string& error,
                ConvertOptions opt = ConvertOptions::none());

bool parseFile(const std::string& path, Document& out, std::string& error,
               ConvertOptions opt = ConvertOptions::none());

} // namespace gdx

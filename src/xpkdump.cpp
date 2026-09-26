#include "gdx/Xpk.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: xpkdump archive.xpk [file-to-extract]\n";
        return 1;
    }
    gdx::XpkArchive a;
    if (!gdx::parseXpkFile(argv[1], a)) {
        std::cerr << "error: " << a.error << "\n";
        return 2;
    }
    std::cout << "files: " << a.count << " dataStart=" << a.dataStart << "\n";
    if (argc >= 3) {
        auto bytes = a.extract(argv[2]);
        if (bytes.empty()) {
            std::cerr << "missing " << argv[2] << "\n";
            return 3;
        }
        std::cout << "extracted " << argv[2] << " bytes=" << bytes.size() << "\n";
        return 0;
    }
    for (const auto& f : a.files)
        std::cout << "  " << f.name << " size=" << f.size << " off=" << f.offset << "\n";
    return 0;
}

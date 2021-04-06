/**
 Patches Cyberpunk2077.exe to disable vehicle FPP camera auto-centering
 Required game version: 1.2

 To build executable by yourself just run:
 g++ -o CyberPatcher.exe CyberPatcher.cpp -static -static-libgcc -static-libstdc++
*/

#include <iostream>
#include <fstream>
#include <cstring>

namespace config
{
const std::string filename = "Cyberpunk2077.exe";

const long offsets[] =
{
    53236145    // Patch 1.2 Steam & GOG
};

const long sequence_size = 16;
const char old_sequence[sequence_size] = { 0x68, 0x65, 0x61, 0x64, 0x69, 0x6E, 0x67, 0x52, 0x65, 0x73, 0x65, 0x74, 0x53, 0x70, 0x65, 0x65 };
const char new_sequence[sequence_size] = { 0x00, 0x65, 0x61, 0x64, 0x69, 0x6E, 0x67, 0x52, 0x65, 0x73, 0x65, 0x74, 0x53, 0x70, 0x65, 0x65 };
};

void create_backup(std::fstream &src)
{
    std::ofstream backup;
    backup.open(config::filename + ".backup", std::ios::out | std::ios::binary);
    src.seekg(0);
    backup << src.rdbuf();
    std::cout << config::filename + " - backup created" << std::endl;
    backup.close();
}

bool create_patch_if_matched(std::fstream &src, long offset)
{
    char* buffer = new char[config::sequence_size];
    src.seekg(offset);
    src.read(buffer, config::sequence_size);

    if (memcmp(config::old_sequence, buffer, config::sequence_size) == 0)
    {
        std::cout << "binary sequence found!" << std::endl;
        create_backup(src);
        std::cout << "Applying patch... ";
        src.seekp(offset);
        src.write(config::new_sequence, config::sequence_size);
        std::cout << "done!" << std::endl;
        return true;
    }
    else if (memcmp(config::new_sequence, buffer, config::sequence_size) == 0)
    {
        std::cout << "already patched!" << std::endl;
        return true;
    } else
    {
        std::cout << "binary sequence not found!" << std::endl;
        return false;
    }
}

int main()
{
    std::fstream cp77exe;
    cp77exe.open(config::filename, std::ios::in | std::ios::out | std::ios::binary);

    if (cp77exe.is_open())
    {
        std::cout << config::filename + " found" << std::endl;

        for (int i = 0; i < sizeof(config::offsets); i++) {
            long offset = config::offsets[i];
            std::cout << "Checking offset " << offset << ": ";
            if (create_patch_if_matched(cp77exe, offset)) {
                break;
            }
        }

        cp77exe.close();
    }
    else
    {
        std::cout << config::filename + " not found" << std::endl;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}

#include "filecomparator.h"

#include <algorithm>
#include <fstream>
#include <iostream>

FileComparator::FileComparator()
    : length1(0), length2(0), compareLength(0), file1(nullptr), file2(nullptr)
{
    std::cout << "    [default constructor]" << std::endl;
}

FileComparator::FileComparator(const std::string &firstName, const std::string &secondName)
    : name1(firstName), name2(secondName), length1(0), length2(0), compareLength(0),
      file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [constructor with 2 args]" << std::endl;
}

FileComparator::FileComparator(const std::string &firstName, const std::string &secondName, long length)
    : name1(firstName), name2(secondName), length1(0), length2(0), compareLength(length),
      file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [constructor with 3 args]" << std::endl;
}

FileComparator::FileComparator(const FileComparator &other)
    : name1(other.name1), name2(other.name2), length1(0), length2(0),
      compareLength(other.compareLength), file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [copy constructor]" << std::endl;
}

FileComparator::~FileComparator()
{
    CloseFiles();
    std::cout << "    [destructor: files " << (name1.empty() ? "-" : name1) << ", "
              << (name2.empty() ? "-" : name2) << " closed]" << std::endl;
}

void FileComparator::OpenFiles()
{
    file1 = std::fopen(name1.c_str(), "rb");
    file2 = std::fopen(name2.c_str(), "rb");
    length1 = file1 ? FileLength(file1) : 0;
    length2 = file2 ? FileLength(file2) : 0;
}

void FileComparator::CloseFiles()
{
    if (file1) std::fclose(file1);
    if (file2) std::fclose(file2);
    file1 = nullptr;
    file2 = nullptr;
}

long FileComparator::FileLength(FILE *file)
{
    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    return size;
}

long FileComparator::GetCompareLength() const
{
    long common = std::min(length1, length2);
    if (compareLength > 0)
        return std::min(compareLength, common);
    return common;
}

long FileComparator::CountDifferences()
{
    if (!IsOpen()) return -1;

    std::fseek(file1, 0, SEEK_SET);
    std::fseek(file2, 0, SEEK_SET);

    long count = 0;
    long limit = GetCompareLength();
    for (long i = 0; i < limit; i++) {
        if (std::fgetc(file1) != std::fgetc(file2))
            count++;
    }
    return count;
}

long FileComparator::SaveDifferences(const std::string &outName)
{
    if (!IsOpen()) return -1;

    std::ofstream out(outName);
    if (!out) return -1;

    std::fseek(file1, 0, SEEK_SET);
    std::fseek(file2, 0, SEEK_SET);

    long count = 0;
    long limit = GetCompareLength();
    for (long i = 0; i < limit; i++) {
        if (std::fgetc(file1) != std::fgetc(file2)) {
            out << i + 1 << "\n";
            count++;
        }
    }
    return count;
}

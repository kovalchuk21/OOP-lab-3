#ifndef FILECOMPARATOR_H
#define FILECOMPARATOR_H

#include <cstdio>
#include <string>

class FileComparator
{
public:
    FileComparator();
    FileComparator(const std::string &firstName, const std::string &secondName);
    FileComparator(const std::string &firstName, const std::string &secondName, long length);
    FileComparator(const FileComparator &other);
    ~FileComparator();

    bool IsOpen() const { return file1 != nullptr && file2 != nullptr; }
    long CountDifferences();
    long SaveDifferences(const std::string &outName);

    std::string GetName1() const { return name1; }
    std::string GetName2() const { return name2; }
    long GetLength1() const { return length1; }
    long GetLength2() const { return length2; }
    long GetCompareLength() const;

private:
    std::string name1;
    std::string name2;
    long length1;
    long length2;
    long compareLength;
    FILE *file1;
    FILE *file2;

    void OpenFiles();
    void CloseFiles();
    static long FileLength(FILE *file);
};

#endif

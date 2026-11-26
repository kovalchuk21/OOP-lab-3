#include "filecomparator.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

void ShowComparator(const std::string &name, FileComparator &cmp)
{
    if (!cmp.IsOpen()) {
        PrintTable({"Object", "File 1", "File 2", "Opened"}, {{name, "-", "-", "NO"}});
        return;
    }
    PrintTable({"Object", "File 1", "File 2", "Opened", "Compare len", "Differences"},
               {{name, cmp.GetName1() + " (" + std::to_string(cmp.GetLength1()) + " B)",
                 cmp.GetName2() + " (" + std::to_string(cmp.GetLength2()) + " B)", "YES",
                 std::to_string(cmp.GetCompareLength()) + " B", std::to_string(cmp.CountDifferences())}});
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    SaveOutputTo("output.txt");

    std::ifstream fin("input.txt");
    if (!fin) {
        Warn("Cannot open input.txt");
        return 1;
    }

    std::string first, second, resultName;
    long length;
    fin >> first >> second >> length >> resultName;

    PrintBox("<------- FILE COMPARATOR (VARIANT 10) ------->");

    PrintBox("Testing default constructor");
    FileComparator empty;
    ShowComparator("empty", empty);

    PrintBox("Testing constructor with 2 arguments (whole files)");
    FileComparator full(first, second);
    ShowComparator("full", full);
    long saved = full.SaveDifferences(resultName);
    Done("Positions of different bytes saved to " + resultName + ": " + std::to_string(saved));

    PrintBox("Testing constructor with 3 arguments (first " + std::to_string(length) + " bytes)");
    FileComparator part(first, second, length);
    ShowComparator("part", part);

    PrintBox("Testing copy constructor: FileComparator copy(part)");
    FileComparator copy(part);
    ShowComparator("copy", copy);

    PrintBox("Checking what is in " + resultName);
    std::ifstream result(resultName);
    std::string text, all;
    while (std::getline(result, text))
        all += text + " ";
    std::cout << "positions = " << all << "\n";

    PrintBox("End of main: destructors are called");
    return 0;
}

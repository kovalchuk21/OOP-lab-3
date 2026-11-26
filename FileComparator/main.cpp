#include "filecomparator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class TeeBuf : public std::streambuf
{
public:
    TeeBuf(std::streambuf *first, std::streambuf *second) : a(first), b(second) {}

protected:
    int overflow(int c) override
    {
        if (c == EOF) return 0;
        if (a->sputc(c) == EOF || b->sputc(c) == EOF) return EOF;
        return c;
    }
    int sync() override
    {
        return (a->pubsync() == 0 && b->pubsync() == 0) ? 0 : -1;
    }

private:
    std::streambuf *a;
    std::streambuf *b;
};

std::ofstream fout("output.txt");
TeeBuf tee(std::cout.rdbuf(), fout.rdbuf());
std::streambuf *consoleBuf = std::cout.rdbuf(&tee);

struct RestoreCout
{
    ~RestoreCout()
    {
        std::cout.flush();
        std::cout.rdbuf(consoleBuf);
    }
} restoreCout;

void print(const std::string &text)
{
    std::cout << text << std::endl;
}

void title(const std::string &text)
{
    const int width = 78;
    int left = (width - 2 - static_cast<int>(text.size())) / 2;
    int right = width - 2 - static_cast<int>(text.size()) - left;
    print(std::string(width, '-'));
    print("|" + std::string(left, ' ') + text + std::string(right, ' ') + "|");
    print(std::string(width, '-'));
}

void section(const std::string &text)
{
    print("");
    print("[ " + text + " ]");
}

void blockBegin(const std::string &name)
{
    const int width = 40;
    int left = (width - 2 - static_cast<int>(name.size())) / 2;
    int right = width - 2 - static_cast<int>(name.size()) - left;
    print(std::string(left, '-') + " " + name + " " + std::string(right, '-'));
}

void blockEnd()
{
    print(std::string(40, '-'));
}

void field(const std::string &name, const std::string &value)
{
    std::ostringstream s;
    s << std::left << std::setw(14) << name << ": " << value;
    print(s.str());
}

std::string line(const std::vector<int> &widths)
{
    std::string s;
    for (int w : widths)
        s += "+" + std::string(w + 2, '-');
    return s + "+";
}

std::string row(const std::vector<std::string> &cells, const std::vector<int> &widths)
{
    std::ostringstream s;
    for (size_t i = 0; i < cells.size(); i++)
        s << "| " << std::left << std::setw(widths[i]) << cells[i] << " ";
    s << "|";
    return s.str();
}

std::string num(double value, int digits = 2)
{
    std::ostringstream s;
    s << std::fixed << std::setprecision(digits) << value;
    return s.str();
}

void showObject(const std::string &name, FileComparator &cmp)
{
    blockBegin(name);
    field("File 1", cmp.GetName1().empty() ? "-" : cmp.GetName1() + " (" + std::to_string(cmp.GetLength1()) + " bytes)");
    field("File 2", cmp.GetName2().empty() ? "-" : cmp.GetName2() + " (" + std::to_string(cmp.GetLength2()) + " bytes)");
    field("Opened", cmp.IsOpen() ? "YES" : "NO");
    if (cmp.IsOpen()) {
        field("Compare len", std::to_string(cmp.GetCompareLength()) + " bytes");
        field("Differences", std::to_string(cmp.CountDifferences()));
    }
    blockEnd();
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::ifstream fin("input.txt");
    if (!fin) {
        print("ERROR: cannot open input.txt");
        return 1;
    }

    std::string first, second, resultName;
    long length;
    fin >> first >> second >> length >> resultName;

    title("FILE COMPARATOR (VARIANT 10)");

    section("1. DEFAULT CONSTRUCTOR: FileComparator empty;");
    FileComparator empty;
    showObject("empty", empty);

    section("2. TWO ARGS: FileComparator full(\"" + first + "\", \"" + second + "\");");
    FileComparator full(first, second);
    showObject("full", full);
    long saved = full.SaveDifferences(resultName);
    print(">> Positions saved to " + resultName + ": " + std::to_string(saved));

    section("3. THREE ARGS: FileComparator part(..., " + std::to_string(length) + ");");
    FileComparator part(first, second, length);
    showObject("part", part);

    section("4. COPY CONSTRUCTOR: FileComparator copy(part);");
    FileComparator copy(part);
    showObject("copy", copy);

    section("CONTENT OF " + resultName);
    std::ifstream result(resultName);
    std::string text, all;
    while (std::getline(result, text))
        all += text + " ";
    print(all);

    section("END OF main - DESTRUCTORS");
    return 0;
}

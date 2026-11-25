#include "filecomparator.h"

#include <fstream>
#include <iostream>
#include <string>

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

void show(const std::string &title, FileComparator &cmp)
{
    std::cout << title << std::endl;
    std::cout << "    файли: " << (cmp.GetName1().empty() ? "-" : cmp.GetName1()) << " ("
              << cmp.GetLength1() << " байт), " << (cmp.GetName2().empty() ? "-" : cmp.GetName2())
              << " (" << cmp.GetLength2() << " байт)" << std::endl;
    if (!cmp.IsOpen()) {
        std::cout << "    файли не відкрито, порівняння неможливе" << std::endl;
        return;
    }
    std::cout << "    довжина порівняння: " << cmp.GetCompareLength() << " байт" << std::endl;
    std::cout << "    кількість різних байтів: " << cmp.CountDifferences() << std::endl;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::ifstream fin("input.txt");
    if (!fin) {
        std::cout << "Не вдалося відкрити input.txt" << std::endl;
        return 1;
    }

    std::string first, second, resultName;
    long length;
    fin >> first >> second >> length >> resultName;

    std::cout << "1) Конструктор за замовчуванням:" << std::endl;
    FileComparator empty;
    show("Об'єкт empty:", empty);

    std::cout << std::endl << "2) Конструктор з двома аргументами:" << std::endl;
    FileComparator full(first, second);
    show("Об'єкт full (порівняння по всій довжині):", full);

    long saved = full.SaveDifferences(resultName);
    std::cout << "    позиції різних байтів записано у " << resultName
              << " (" << saved << " шт.)" << std::endl;

    std::cout << std::endl << "3) Конструктор з трьома аргументами (довжина " << length << "):" << std::endl;
    FileComparator part(first, second, length);
    show("Об'єкт part:", part);

    std::cout << std::endl << "4) Конструктор копіювання (копія part):" << std::endl;
    FileComparator copy(part);
    show("Об'єкт copy:", copy);

    std::cout << std::endl << "Вміст файлу " << resultName << ":" << std::endl;
    std::ifstream result(resultName);
    std::string line, all;
    while (std::getline(result, line)) all += line + " ";
    std::cout << "    " << all << std::endl;

    std::cout << std::endl << "Кінець main - далі викликаються деструктори:" << std::endl;
    return 0;
}

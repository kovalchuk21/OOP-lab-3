#include "atm.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
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

void print(const std::string &text)
{
    std::cout << text << std::endl;
}

std::string status(const std::string &name, const ATM &atm)
{
    std::ostringstream s;
    s << name << ": стан: " << atm.GetStateName()
      << " | гроші: " << atm.GetMoney() << " грн"
      << " | стрічка: " << std::fixed << std::setprecision(2) << atm.GetTapeLength() << " м"
      << " | права: " << atm.GetLevelName();
    return s.str();
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::ifstream fin("input.txt");

    if (!fin) {
        print("Не вдалося відкрити input.txt");
        return 1;
    }

    int startMoney, withdrawSum;
    double startTape;
    fin >> startMoney >> startTape >> withdrawSum;

    print("1) Створення об'єкта конструктором за замовчуванням:");
    ATM atm1;
    print(status("atm1", atm1));

    print("");
    print("2) Створення об'єкта конструктором ініціалізації (" +
          std::to_string(startMoney) + " грн, стрічка з input.txt):");
    ATM atm2(startMoney, startTape);
    print(status("atm2", atm2));

    print("");
    print("Зняття " + std::to_string(withdrawSum) + " грн з atm2:");
    atm2.Withdraw(withdrawSum);
    print(status("atm2", atm2));

    print("");
    print("3) Створення об'єкта конструктором копіювання (копія atm2):");
    ATM atm3(atm2);
    print(status("atm3", atm3));

    print("");
    print("Зняття " + std::to_string(withdrawSum) + " грн з atm3 (atm2 не змінюється):");
    atm3.Withdraw(withdrawSum);
    print(status("atm2", atm2));
    print(status("atm3", atm3));

    print("");
    print("Кінець main - далі викликаються деструктори:");
    return 0;
}

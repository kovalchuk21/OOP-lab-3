#include "atm.h"

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

void showState(const std::string &name, const ATM &atm)
{
    blockBegin(name);
    field("State", atm.GetStateName());
    field("Money", std::to_string(atm.GetMoney()) + " UAH");
    field("Tape", num(atm.GetTapeLength()) + " m");
    field("No tape", atm.GetNoTape() ? "YES" : "NO");
    field("User", atm.GetLevelName());
    blockEnd();
}

std::vector<std::string> stateCells(const ATM &atm)
{
    return {atm.GetStateName(), std::to_string(atm.GetMoney()), num(atm.GetTapeLength()),
            atm.GetNoTape() ? "YES" : "NO", atm.GetLevelName()};
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

    int startMoney, withdrawSum;
    double startTape;
    fin >> startMoney >> startTape >> withdrawSum;

    title("ATM CONSTRUCTORS AND DESTRUCTOR TEST");

    section("1. DEFAULT CONSTRUCTOR: ATM atm1;");
    ATM atm1;
    showState("atm1", atm1);

    section("2. INIT CONSTRUCTOR: ATM atm2(" + std::to_string(startMoney) + ", " + num(startTape) + ");");
    ATM atm2(startMoney, startTape);
    showState("atm2", atm2);

    section("atm2.Withdraw(" + std::to_string(withdrawSum) + ")");
    print(atm2.Withdraw(withdrawSum) ? ">> Done" : ">> Rejected");
    showState("atm2", atm2);

    section("3. COPY CONSTRUCTOR: ATM atm3(atm2);");
    ATM atm3(atm2);
    showState("atm3", atm3);

    section("atm3.Withdraw(" + std::to_string(withdrawSum) + ") - atm2 must not change");
    print(atm3.Withdraw(withdrawSum) ? ">> Done" : ">> Rejected");
    std::vector<int> w = {6, 11, 5, 4, 5};
    print(line(w));
    print(row({"Object", "State", "Money", "Tape", "User"}, w));
    print(line(w));
    print(row({"atm2", atm2.GetStateName(), std::to_string(atm2.GetMoney()), num(atm2.GetTapeLength()), atm2.GetLevelName()}, w));
    print(row({"atm3", atm3.GetStateName(), std::to_string(atm3.GetMoney()), num(atm3.GetTapeLength()), atm3.GetLevelName()}, w));
    print(line(w));

    section("END OF main - DESTRUCTORS");
    return 0;
}

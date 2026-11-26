#include "atm.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

void ShowAtm(const std::string &name, const ATM &atm)
{
    PrintTable({"Object", "State", "Money", "Tape", "No tape", "User"},
               {{name, atm.GetStateName(), std::to_string(atm.GetMoney()) + " UAH",
                 Num(atm.GetTapeLength()) + " m", atm.GetNoTape() ? "YES" : "NO", atm.GetLevelName()}});
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

    int startMoney, withdrawSum;
    double startTape;
    fin >> startMoney >> startTape >> withdrawSum;

    PrintBox("<------- ATM: CONSTRUCTORS AND DESTRUCTOR ------->");

    PrintBox("Testing default constructor: ATM atm1;");
    ATM atm1;
    ShowAtm("atm1", atm1);

    PrintBox("Testing init constructor: ATM atm2(" + std::to_string(startMoney) + ", " + Num(startTape) + ");");
    ATM atm2(startMoney, startTape);
    ShowAtm("atm2", atm2);

    PrintBox("Withdraw " + std::to_string(withdrawSum) + " UAH from atm2");
    if (atm2.Withdraw(withdrawSum))
        Done();
    else
        Warn("Withdraw failed");
    ShowAtm("atm2", atm2);

    PrintBox("Testing copy constructor: ATM atm3(atm2);");
    ATM atm3(atm2);
    ShowAtm("atm3", atm3);

    PrintBox("Withdraw " + std::to_string(withdrawSum) + " UAH from atm3 (atm2 must not change)");
    if (atm3.Withdraw(withdrawSum))
        Done();
    else
        Warn("Withdraw failed");
    PrintTable({"Object", "State", "Money", "Tape", "User"},
               {{"atm2", atm2.GetStateName(), std::to_string(atm2.GetMoney()) + " UAH", Num(atm2.GetTapeLength()) + " m", atm2.GetLevelName()},
                {"atm3", atm3.GetStateName(), std::to_string(atm3.GetMoney()) + " UAH", Num(atm3.GetTapeLength()) + " m", atm3.GetLevelName()}});

    PrintBox("End of main: destructors are called");
    return 0;
}

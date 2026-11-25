#include "atm.h"

#include <cmath>
#include <iostream>

const std::string ATM::ADMIN_PASSWORD = "1234";

ATM::ATM()
{
    Reset();
    std::cout << "    [конструктор за замовчуванням]" << std::endl;
}

ATM::ATM(int startMoney, double startTape)
{
    Reset();
    level = UserLevel::Admin;
    SetMoney(startMoney);
    SetTapeLength(startTape);
    level = UserLevel::Guest;
    std::cout << "    [конструктор ініціалізації]" << std::endl;
}

ATM::ATM(const ATM &other)
    : state(other.state)
    , money(other.money)
    , tapeCm(other.tapeCm)
    , noTape(other.noTape)
    , level(UserLevel::Guest)
{
    std::cout << "    [конструктор копіювання]" << std::endl;
}

ATM::~ATM()
{
    std::cout << "    [деструктор банкомата]" << std::endl;
}

void ATM::Reset()
{
    state = AtmState::NotWorking;
    money = 0;
    tapeCm = 0;
    noTape = true;
    level = UserLevel::Guest;
}

bool ATM::Authorize(UserLevel newLevel, const std::string &password)
{
    if (newLevel == UserLevel::Admin && password != ADMIN_PASSWORD)
        return false;

    level = newLevel;
    return true;
}

void ATM::Logout()
{
    level = UserLevel::Guest;
    if (state == AtmState::Error)
        state = AtmState::Working;
    UpdateState();
}

bool ATM::SetMoney(int sum)
{
    if (!IsAdmin()) return false;
    if (sum < 0 || sum > MAX_MONEY || sum % 5 != 0) return false;

    money = sum;
    UpdateState();
    return true;
}

bool ATM::SetTapeLength(double meters)
{
    if (!IsAdmin()) return false;
    if (meters < 0 || meters > MAX_TAPE_CM / 100.0) return false;

    tapeCm = static_cast<int>(std::lround(meters * 100));
    noTape = tapeCm < CHECK_CM;
    return true;
}

bool ATM::Withdraw(int sum)
{
    if (state != AtmState::Working) return false;
    if (sum <= 0 || sum % 5 != 0) return false;

    if (money < MIN_MONEY) {
        state = AtmState::Error;
        return false;
    }
    if (sum > money) return false;

    money -= sum;

    if (tapeCm >= CHECK_CM)
        tapeCm -= CHECK_CM;
    noTape = tapeCm < CHECK_CM;

    UpdateState();
    return true;
}

void ATM::UpdateState()
{
    if (state == AtmState::Error) return;
    state = (money > 0) ? AtmState::Working : AtmState::NotWorking;
}

std::string ATM::GetStateName() const
{
    switch (state) {
    case AtmState::Working: return "працює";
    case AtmState::NotWorking: return "не працює";
    default: return "помилка";
    }
}

std::string ATM::GetLevelName() const
{
    return level == UserLevel::Admin ? "адміністратор" : "гість";
}

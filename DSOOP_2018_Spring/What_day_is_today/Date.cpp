#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "Date.h"
using namespace std;

Date::Date(int dd, int mn, int yr)
{
    setDay(dd);
    setMonth(mn);
    setYear(yr);
}

Date::~Date()
{
}

void Date::setDay(int dd)
{
    if (dd >= 1 && dd <= 31)
        day = dd;
    else
        throw invalid_argument("Day must be 1-31");
}

void Date::setMonth(int mn)
{
    if (mn >= 1 && mn <= 12)
        month = mn;
    else
        throw invalid_argument("Month must be 1-12");
}

void Date::setYear(int yr)
{
    year = yr;
}

int Date::getDay()
{
    return day;
}

int Date::getMonth()
{
    return month;
}

int Date::getYear()
{
    return year;
}

string Date::dayOfWeek()
{
    string week[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    int c = 0, y = 0, m = 0, d = 0, w;

    c = year / 100;
    d = day;
    m = (month == 1 || month == 2) ? month + 12 : month;
    y = (month == 1 || month == 2) ? (year - 1) % 100 : year % 100;

    w = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1; // using Zeller's congruence
    if (w < 0)
        w = (w % 7 + 7) % 7;
    else
        w %= 7;
    if (w == 0)
        w = 7;
    return week[w - 1];
}

Date Date::DateAfter(int days)
{
    int buffer[3] = {day, month, year};
    for (int i = 0; i < days; i++)
        helpIncrement();
    Date after(day, month, year);
    day = buffer[0];
    month = buffer[1];
    year = buffer[2];
    return after;
}

Date Date::DateBefore(int days)
{
    int buffer[3] = {day, month, year};
    for (int i = 0; i < days; i++)
        helpDecrement();
    Date before(day, month, year);
    day = buffer[0];
    month = buffer[1];
    year = buffer[2];
    return before;
}

int Date::numberOfDays()
{
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
        return 31;
    else if (month == 2 && isLeap())
        return 29;
    else if (month == 2 && !isLeap())
        return 28;
    else
        return 30;
}

void Date::printMonthCalendar()
{
    // determine what day is the 1st day of the month
    int dayBuffer = day;
    int count, i;
    int startday;
    setDay(1);
    if (dayOfWeek() == "Monday")
        startday = 2;
    if (dayOfWeek() == "Tuesday")
        startday = 3;
    if (dayOfWeek() == "Wednesday")
        startday = 4;
    if (dayOfWeek() == "Thursday")
        startday = 5;
    if (dayOfWeek() == "Friday")
        startday = 6;
    if (dayOfWeek() == "Saturday")
        startday = 7;
    if (dayOfWeek() == "Sunday")
        startday = 1; // calendar starts from Sunday

    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeap())
        days[1] += 1;
    cout << "  ";
    switch (month)
    {
    case 1:
        cout << "-------------January-------------\n";
        break;
    case 2:
        cout << "-------------February------------\n";
        break;
    case 3:
        cout << "--------------March--------------\n";
        break;
    case 4:
        cout << "--------------April--------------\n";
        break;
    case 5:
        cout << "---------------May---------------\n";
        break;
    case 6:
        cout << "---------------June--------------\n";
        break;
    case 7:
        cout << "---------------July--------------\n";
        break;
    case 8:
        cout << "--------------August-------------\n";
        break;
    case 9:
        cout << "------------September------------\n";
        break;
    case 10:
        cout << "-------------October-------------\n";
        break;
    case 11:
        cout << "-------------November------------\n";
        break;
    case 12:
        cout << "-------------December------------\n";
        break;
    }
    cout << "  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n";
    for (i = 1, count = 1; count <= numberOfDays(); i++)
    {
        if (i < startday && startday != 1)
            cout << setw(5) << "";
        else
        {
            cout << setw(5) << count;
            count++;
        }
        if (i % 7 == 0)
            cout << "\n";
    }
    setDay(numberOfDays());
    if (dayOfWeek() != "Saturday")
        cout << "\n";
    setDay(dayBuffer);
}

bool Date::isLeap()
{
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        return true;
    else
        return false;
}

bool Date::endOfMonth(int testDay)
{
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeap())
        return testDay == 29;
    else
        return testDay == days[month];
}

void Date::helpIncrement()
{
    if (!endOfMonth(day))
        day++;
    else if (month < 12)
    {
        month++;
        day = 1;
    }
    else
    {
        year++;
        month = 1;
        day = 1;
    }
}

void Date::helpDecrement()
{
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (day != 1)
        day--;
    else if (month > 1)
    {
        month--;
        if (isLeap())
            days[2] = 29;
        day = days[month];
    }
    else
    {
        year--;
        month = 12;
        day = 31;
    }
}

#include <iostream>
#include "Date.h"

// Driver Program to check your implementation
int main()
{
    Date date1(3, 8, 2018);
    Date date2 = date1.DateAfter(13);
    Date date3 = date1.DateBefore(25);

    date1.setDay(25);
    date2.setMonth(12);
    date3.setYear(2015);

    cout << date1.numberOfDays() << endl;
    cout << date2.day() << endl;
    date3.printMonthCalendar();
    
    return 0;
}
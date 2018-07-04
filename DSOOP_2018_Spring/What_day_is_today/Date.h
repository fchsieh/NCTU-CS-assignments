#include <iostream>
using namespace std;

class Date
{
  public:
    Date(int day, int month, int year);
    ~Date();

    /*Get function for accessing private member*/
    int getDay();
    int getMonth();
    int getYear();

    /*Set function for updating private member*/
    void setDay(int day);
    void setMonth(int month);
    void setYear(int year);

    /*A Function that returns the date after a given date
        For e.g-
        
        today is 8/3/2018 (Day/Month/Year)
        DateAfter(3) should return a date of 11/3/2018 */
    Date DateAfter(int days);

    /*A Function that returns the date after a given date
        For e.g-
        
        today is 8/3/2018 (Day/Month/Year)
        DateBefore(3) should return a date of 5/3/2018 */
    Date DateBefore(int days);

    /*A Function that returns the day of a date
        For e.g-
        
        if the date is 8/3/2018, then the dayOfWeek() function
        should return "Thursday"
    */
    string dayOfWeek();

    /* A Function to return the number of days in
        Current Month.
        
        Month Number     Name        Number of Days
        0                January     31
        1                February    28 (non-leap) / 29 (leap)
        2                March       31
        3                April       30
        4                May         31
        5                June        30
        6                July        31
        7                August      31
        8                September   30
        9                October     31
        10               November    30
        11               December    31*/
    int numberOfDays();

    /* Function to print a calendar of the current month.
    Just like what you see on a calendar. An example is like

   --------------March--------------
   Sun  Mon  Tue  Wed  Thu  Fri  Sat
                        1    2    3
    4    5    6    7    8    9   10
   11   12   13   14   15   16   17
   18   19   20   21   22   23   24
   25   26   27   28   29   30   31
    
   */
    void printMonthCalendar();

    bool isLeap();
    bool endOfMonth(int);
    void helpIncrement();
    void helpDecrement();

  private:
    /*Current Date*/
    int day;
    int month;
    int year;
};
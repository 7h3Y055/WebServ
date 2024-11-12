#include <iostream>
#include <ctime>

std::string get_date() {
    time_t t;
    time(&t);

    std::string d = ctime(&t);

    std::string date;

    date += d.substr(0,3);
    date += ", ";
    date += d.substr(8, 2);
    date += " ";
    date += d.substr(4, 3);
    date += " ";
    date += d.substr(20, 4);
    date += " ";
    date += d.substr(11, 8);
    date += " GMT";


    return date;
}
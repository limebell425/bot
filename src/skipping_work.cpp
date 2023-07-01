#include "skipping_work.h"
#include "database.h"
#include <algorithm>
#include <iostream>

bool is_string_can_transform_to_int(std::string s)
{
    return (s.length() < 9) && (std::all_of(s.begin(), s.end(), [](char c) {return std::isdigit(c);}));
}

bool Skipping_work::parse(std::string &&date)
{
    date.erase(remove_if(date.begin(), date.end(), isspace), date.end());
    int hyphen_pos = date.find('-');
    if (hyphen_pos == -1) return false;
    std::string first_date = date.substr(0, hyphen_pos);
    std::string second_date = date.substr(hyphen_pos+1);
    bool without_error = true;
    first_date_ =  parse_one_date(move(first_date), without_error);
    if (!without_error)
    {
        return false;
    }
    last_date_ = parse_one_date(move(second_date), without_error);
    return without_error && first_date_ < last_date_;
}


Date Skipping_work::parse_one_date(std::string &&one_date_s, bool &without_error)
{
    Date date;
    int day_separator_pos = one_date_s.find('.');
    std::string day = one_date_s.substr(0, day_separator_pos);
    int month_separator_pos = one_date_s.find('.', day_separator_pos+1);
    std::string month = one_date_s.substr(day_separator_pos + 1, month_separator_pos - day_separator_pos -1);
    std::string year = one_date_s.substr(one_date_s.rfind('.')+1, one_date_s.length() - month_separator_pos - 1);
    std::cout << day << " " << month << std::endl;
    without_error = parse_month(month, date);
    if (!without_error)
    {
        return date;
    }
    without_error = parse_day(day, date);
    if (!without_error)
    {
        return date;
    }
    without_error = parse_year(year, date);
    return date;
}

bool Skipping_work::parse_month(const std::string &month, Date &date)
{
    if (!is_string_can_transform_to_int(month))
    {
        return false;
    }
    date.month = std::stoi(month);
    return date.month >= 1 && date.month <= 12;
}

bool Skipping_work::parse_day(const std::string &day, Date &date)
{
    if (!is_string_can_transform_to_int(day))
    {
        return false;
    }
    date.day = std::stoi(day);
    if (date.day < 1)
    {
        return false;
    }
    switch (date.month)
    {
    case 2:
        if (date.day > 29)
        {
            return false;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (date.day > 30)
        {
            return false;
        }
        break;
    default:
        if (date.day > 31)
        {
            return false;
        }
        break;
    }
    return true;
}

bool Skipping_work::parse_year(const std::string &year, Date &date)
{
    if (!is_string_can_transform_to_int(year))
    {
        return false;
    }
    date.year = std::stoi(year);
    return true;
}

std::string Skipping_work::to_string()
{
    return std::to_string(first_date_.day) + "." + std::to_string(first_date_.month) + "." + std::to_string(first_date_.year) + " - "
            + std::to_string(last_date_.day) + "." + std::to_string(last_date_.month) + "." + std::to_string(last_date_.year);
}

bool Sick_leave::save_to_data_base(int64_t chat_id, Database &db) const
{
    return db.save_sick_leave(chat_id, *this);
}


bool Vacation::save_to_data_base(int64_t chat_id, Database &db) const 
{ 
    return db.save_vacation(chat_id, *this); 
}
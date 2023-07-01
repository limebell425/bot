#pragma once

#include <string>
#include <tuple>

class Database;

struct Date 
{
    Date() = default;
    Date(int day_, int month_, int year_)
        : day(day_)
        , month(month_)
        , year(year_)
    {}
    auto get_tuple() const
    {
        return std::tie(year, month, day);
    }
    int day;
    int month;
    int year;
   
};

inline bool operator<(const Date &lhs, const Date &rhs)
{
    return lhs.get_tuple() < rhs.get_tuple();
}

inline bool operator==(const Date &lhs, const Date &rhs)
{
    return lhs.get_tuple() == rhs.get_tuple();
}


class Skipping_work
{
    public:
        Skipping_work() = default;
        bool parse(std::string &&date);
        Date parse_one_date(std::string &&one_date_s, bool &without_error);
        bool parse_day(const std::string &day, Date &date);
        bool parse_month(const std::string &month, Date &date);
        bool parse_year(const std::string &year, Date &date);
        const Date& first_date() const { return first_date_; }
        const Date& last_date() const { return last_date_; }
        std::string to_string();
        
        virtual bool save_to_data_base(int64_t chat_id, Database &db) const = 0;
        virtual std::string save_message() const = 0;
        virtual ~Skipping_work() = default;

    protected:
        Skipping_work(int day1, int month1, int year1, int day2, int month2, int year2)
            : first_date_{day1, month1, year1}
            , last_date_{day2, month2, year2}
        {}

    private:
        Date first_date_;
        Date last_date_;
};

class Sick_leave : public Skipping_work
{
    public:
        Sick_leave() = default;
        Sick_leave(bool is_closed_)
            : is_sick_leave_closed(is_closed_)
        {}
        Sick_leave(int day1, int month1, int year1, int day2, int month2, int year2)
            : Skipping_work(day1, month1, year1, day2, month2, year2)     
        {}
        Sick_leave(int day1, int month1, int year1, int day2, int month2, int year2, bool is_closed_)
            : Skipping_work(day1, month1, year1, day2, month2, year2) 
            , is_sick_leave_closed(is_closed_)   
        {}
        bool save_to_data_base(int64_t chat_id, Database &db) const override;
        std::string save_message() const override { return "Больничный лист сохранен"; }
        bool is_closed() const { return is_sick_leave_closed; }
    private:
        bool is_sick_leave_closed;
};

class Vacation : public Skipping_work
{
    public:
        Vacation() = default;
        Vacation(int day1, int month1, int year1, int day2, int month2, int year2)
            : Skipping_work(day1, month1, year1, day2, month2, year2)     
        {}
        bool save_to_data_base(int64_t chat_id, Database &db) const override;
        std::string save_message() const override { return "Отпуск сохранен"; }
};
#include <gtest/gtest.h>
#include "skipping_work.h"
#include <array>

TEST(Parse, NormalDate)
{
    Date ref{5, 8, 23};
    Sick_leave sick_leave;
    bool without_error;
    Date date = sick_leave.parse_one_date("05.08.23", without_error);
    EXPECT_TRUE(without_error);
    EXPECT_EQ(date, ref);

    date = sick_leave.parse_one_date("5.08.23", without_error);
    EXPECT_TRUE(without_error);
    EXPECT_EQ(date, ref);

    date = sick_leave.parse_one_date("5.8.23", without_error);
    EXPECT_TRUE(without_error);
    EXPECT_EQ(date, ref);

    date = sick_leave.parse_one_date("05.8.23", without_error);
    EXPECT_TRUE(without_error);
    EXPECT_EQ(date, ref);

    Date ref2{5, 8, 2023};
    date = sick_leave.parse_one_date("5.8.2023", without_error);
    EXPECT_TRUE(without_error);
    EXPECT_EQ(date, ref2);
}

TEST(Parse, WrongDate)
{
    Sick_leave sick_leave;
    bool without_error;
    Date date = sick_leave.parse_one_date("050823", without_error);
    EXPECT_FALSE(without_error);

    date = sick_leave.parse_one_date("start", without_error);
    EXPECT_FALSE(without_error);
}

TEST(Parse, WrongDay)
{
    Sick_leave sick_leave;
    Date date;
    date.month = 2;
    EXPECT_FALSE(sick_leave.parse_day("start", date));
    EXPECT_FALSE(sick_leave.parse_day("11111", date));
    EXPECT_FALSE(sick_leave.parse_day("30", date));

    std::array<int, 4> month1 = {4, 6, 9, 11};
    std::array<int, 7> month2 = {1, 3, 5, 7, 8, 10, 12};
    for (auto i : month1)
    {
        date.month = i;
        EXPECT_FALSE(sick_leave.parse_day("31", date));
    }

    for (auto i : month2)
    {
        date.month = i;
        EXPECT_FALSE(sick_leave.parse_day("32", date));
    }

    for (int i = 1; i < 12; ++i)
    {
        date.month = i;
        EXPECT_FALSE(sick_leave.parse_day("0", date));
    }
}

TEST(Parse, NormalDay)
{
    Sick_leave sick_leave;
    Date date;

        for (int i = 1; i < 12; ++i)
        {
            date.month = i;
            EXPECT_TRUE(sick_leave.parse_day("15", date));
            EXPECT_EQ(date.day, 15);
            EXPECT_TRUE(sick_leave.parse_day("1", date));
            EXPECT_EQ(date.day, 1);
            EXPECT_TRUE(sick_leave.parse_day("01", date));
            EXPECT_EQ(date.day, 1);
        }
}

TEST(Parse, WrongMonth)
{
    Sick_leave sick_leave;
    Date date;
    EXPECT_FALSE(sick_leave.parse_month("start", date));
    EXPECT_FALSE(sick_leave.parse_month("1111111", date));
    EXPECT_FALSE(sick_leave.parse_month("0", date));
    EXPECT_FALSE(sick_leave.parse_month("13", date));
}

TEST(Parse, NormalMonth)
{
    Sick_leave sick_leave;
    Date date;
    EXPECT_TRUE(sick_leave.parse_month("1", date));
    EXPECT_EQ(date.month, 1);
    EXPECT_TRUE(sick_leave.parse_month("12", date));
    EXPECT_EQ(date.month, 12);
    EXPECT_TRUE(sick_leave.parse_month("06", date));
    EXPECT_EQ(date.month, 6);
}


TEST(Parse, WrongYear)
{
    Sick_leave sick_leave;
    Date date;
    EXPECT_FALSE(sick_leave.parse_year("start", date));
    EXPECT_FALSE(sick_leave.parse_year("111111111", date));
}


TEST(Parse, NormalYear)
{
    Sick_leave sick_leave;
    Date date;
    EXPECT_TRUE(sick_leave.parse_year("2023", date));
    EXPECT_EQ(date.year, 2023);
    EXPECT_TRUE(sick_leave.parse_year("23", date));
    EXPECT_EQ(date.year, 23);
}

TEST (Parse, WrongPairOfDate)
{
    Sick_leave sick_leave;
    EXPECT_FALSE(sick_leave.parse("02.03.23--07.03.23"));
    EXPECT_FALSE(sick_leave.parse("02.03.23 07.03.23"));
    EXPECT_FALSE(sick_leave.parse("07.03.23-02.03.23"));
    EXPECT_FALSE(sick_leave.parse("02.04.23-07.03.23"));
    EXPECT_FALSE(sick_leave.parse("02.03.23-07.03.22"));
}

TEST (Parse, NormalPairOfDate)
{
    Sick_leave sick_leave;
    EXPECT_TRUE(sick_leave.parse("02.03.23-07.03.23"));
    EXPECT_TRUE(sick_leave.parse("02.03.23 - 07.03.23 "));
}
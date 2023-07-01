#include "database.h"
#include <iostream>

#include <QSqlQuery>
#include <QVariant>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <string>

bool Database::open_db()
{
    db_ = QSqlDatabase::addDatabase("QPSQL", "connection");
    db_.setHostName("");
    db_.setDatabaseName("");
    db_.setUserName("");
    db_.setPassword("");
    return db_.open();
}

bool Database::is_user_new(int64_t chat_id)
{
    QSqlQuery db_query(db_);
    std::string query_text = "SELECT * FROM users WHERE chat_id = " + std::to_string(chat_id);
    db_query.exec(query_text.c_str());
    return !db_query.next();
}

bool Database::save_user(int64_t chat_id)
{
    QSqlQuery db_query(db_); 
    std::string query_text = "INSERT INTO users (chat_id) VALUES (" + std::to_string(chat_id) + ")";
    return db_query.exec(query_text.c_str());
}

int Database::user_primary_key(int64_t chat_id) const
{
    QSqlQuery get_user_primary_key(db_);
    std::string query_text = "SELECT id FROM users WHERE chat_id = " + std::to_string(chat_id);
    get_user_primary_key.exec(query_text.c_str());
    get_user_primary_key.next();
    return get_user_primary_key.value(0).toInt();
}

bool Database::save_sick_leave(int64_t chat_id, const Sick_leave &sick_leave)
{
    QSqlQuery db_query(db_);
    std::string first_part = "INSERT INTO sick_leaves (user_id, start_day, start_month, start_year, end_day, end_month, end_year, is_closed) ";
    std::string second_part = "VALUES (:user_id, :start_day, :start_month, :start_year, :end_day, :end_month, :end_year, :is_closed)";
    std::string query_text = first_part + second_part;
    db_query.prepare(query_text.c_str());
    db_query.bindValue(":user_id", user_primary_key(chat_id));
    db_query.bindValue(":start_day", sick_leave.first_date().day);
    db_query.bindValue(":start_month", sick_leave.first_date().month);
    db_query.bindValue(":start_year", sick_leave.first_date().year);
    db_query.bindValue(":end_day", sick_leave.last_date().day);
    db_query.bindValue(":end_month", sick_leave.last_date().month);
    db_query.bindValue(":end_year", sick_leave.last_date().year);
    db_query.bindValue(":is_closed", sick_leave.is_closed());
    return db_query.exec();
}

bool Database::save_vacation(int64_t chat_id, const Vacation &vacation)
{
    QSqlQuery db_query(db_);
    std::string first_part = "INSERT INTO vacations (user_id, start_day, start_month, start_year, end_day, end_month, end_year) ";
    std::string second_part = "VALUES (:user_id, :start_day, :start_month, :start_year, :end_day, :end_month, :end_year)";
    std::string query_text = first_part + second_part;
    db_query.prepare(query_text.c_str());
    db_query.bindValue(":user_id", user_primary_key(chat_id));
    db_query.bindValue(":start_day", vacation.first_date().day);
    db_query.bindValue(":start_month", vacation.first_date().month);
    db_query.bindValue(":start_year", vacation.first_date().year);
    db_query.bindValue(":end_day", vacation.last_date().day);
    db_query.bindValue(":end_month", vacation.last_date().month);
    db_query.bindValue(":end_year", vacation.last_date().year);
    return db_query.exec();
}

std::string Database::view_sick_leaves(int64_t chat_id)
{
    std::string result;
    int count = 1;
    QSqlQueryModel sick_leave_model;
    std::string query_text = "SELECT start_day, start_month, start_year, end_day, end_month, end_year, is_closed FROM sick_leaves WHERE user_id = " 
                                + std::to_string(user_primary_key(chat_id));
    sick_leave_model.setQuery(query_text.c_str(), db_);
    if (sick_leave_model.rowCount() > 0)
    {
        result = "Список больничных: \n";
    }
    else
    {
        result = "Больничных нет. \n";
    }
    for (int i = 0; i < sick_leave_model.rowCount(); ++i) {
        Sick_leave sick_leave
        (
            sick_leave_model.record(i).value("start_day").toInt(),
            sick_leave_model.record(i).value("start_month").toInt(),
            sick_leave_model.record(i).value("start_year").toInt(),
            sick_leave_model.record(i).value("end_day").toInt(),
            sick_leave_model.record(i).value("end_month").toInt(),
            sick_leave_model.record(i).value("end_year").toInt()
        );
        std::string status;
        if (sick_leave_model.record(i).value("is_closed").toBool())
        {
            status = "Статус: закрыт";
        }
        else
        {
           status = "Статус: открыт";
        }
        result += (std::to_string(count)+ ". " + sick_leave.to_string() + " " + status +  "\n");
        ++count;
    }
    return result; 
}

std::string Database::view_vacations(int64_t chat_id)
{
    std::string result;
    int count = 1;
    QSqlQueryModel vacation_model;
    std::string query_text = "SELECT start_day, start_month, start_year, end_day, end_month, end_year FROM vacations WHERE user_id = " 
                                + std::to_string(user_primary_key(chat_id));
    vacation_model.setQuery(query_text.c_str(), db_);
    if (vacation_model.rowCount() > 0)
    {
        result = "Список отпусков: \n";
    }
    else
    {
        result = "Отпусков нет. \n";
    }
    for (int i = 0; i < vacation_model.rowCount(); ++i) {
        Vacation vacation
        (
            vacation_model.record(i).value("start_day").toInt(),
            vacation_model.record(i).value("start_month").toInt(),
            vacation_model.record(i).value("start_year").toInt(),
            vacation_model.record(i).value("end_day").toInt(),
            vacation_model.record(i).value("end_month").toInt(),
            vacation_model.record(i).value("end_year").toInt()
        );
        result += (std::to_string(count)+ ". " + vacation.to_string() + "\n");
        ++count;
    }
    return result; 
}
#pragma once
#include <QSqlDatabase>
#include "skipping_work.h"

class Database{
    public:
        bool open_db();
        bool is_user_new(int64_t chat_id);
        bool save_user(int64_t chat_id);
        bool save_sick_leave(int64_t chat_id, const Sick_leave &sick_leave);
        bool save_vacation(int64_t chat_id, const Vacation &sick_leave);
        std::string view_sick_leaves(int64_t chat_id);
        std::string view_vacations(int64_t chat_id);
        const QSqlDatabase& get_connection(){ return db_; };
        ~Database(){ db_.close(); }
    private: 
        int user_primary_key(int64_t chat_id) const;
        QSqlDatabase db_;
};
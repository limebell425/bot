#include <gtest/gtest.h>
#include "database.h"

#include <QSqlQuery>

TEST (DatabaseTest, IsUserNew)
{
    Database db;
    db.open_db();
    QSqlQuery db_query(db.get_connection());
    EXPECT_TRUE(db.is_user_new(-1));
    db_query.exec("INSERT INTO users (chat_id) VALUES (-1)");
    EXPECT_FALSE(db.is_user_new(-1));
    db_query.exec("DELETE FROM users WHERE chat_id = -1");
}

TEST (DatabaseTest, SaveUser)
{
    Database db;
    db.open_db();
    QSqlQuery db_query(db.get_connection());
    EXPECT_TRUE(db.save_user(-1));
    db_query.exec("SELECT * FROM users WHERE chat_id = -1");
    EXPECT_TRUE(db_query.next());
    db_query.exec("DELETE FROM users WHERE chat_id = -1");
}

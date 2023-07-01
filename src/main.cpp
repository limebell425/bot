#include <iostream>
#include <string>
#include <tgbot/tgbot.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include "database.h"
#include "skipping_work.h"

enum class State {NORMAL, WAIT};

int main()
{
    Database db;
    bool ok = db.open_db();
    std::cout << ok << std::endl;
    std::unordered_map<int16_t, std::unique_ptr<Skipping_work>> leaves;
    std::unordered_map<int16_t, State> bot_state;

    TgBot::Bot bot("");
    auto add_sick_leave_button = std::make_shared<TgBot::InlineKeyboardButton>();
    add_sick_leave_button->text = "Добавить больничный";
    add_sick_leave_button->callbackData = "add_sick_leave";

    auto add_vacation_button = std::make_shared<TgBot::InlineKeyboardButton>();
    add_vacation_button->text = "Добавить отпуск";
    add_vacation_button->callbackData = "add_vacation";

    auto view_button = std::make_shared<TgBot::InlineKeyboardButton>();
    view_button->text = "Просмотр сохраненых данных";
    view_button->callbackData = "view";

    auto main_keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    main_keyboard->inlineKeyboard.push_back({add_sick_leave_button});
    main_keyboard->inlineKeyboard.push_back({add_vacation_button});
    main_keyboard->inlineKeyboard.push_back({view_button});

    auto closed_sick_leave_button = std::make_shared<TgBot::InlineKeyboardButton>();
    closed_sick_leave_button->text = "Закрытый больничный";
    closed_sick_leave_button->callbackData = "closed_sick_leave";

    auto opened_sick_leave_button = std::make_shared<TgBot::InlineKeyboardButton>();
    opened_sick_leave_button->text = "Открытый больничный";
    opened_sick_leave_button->callbackData = "opened_sick_leave";

    auto return_button = std::make_shared<TgBot::InlineKeyboardButton>();
    return_button->text = "Вернуться в меню";
    return_button->callbackData = "return";

    auto sick_leave_keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    sick_leave_keyboard->inlineKeyboard.push_back({closed_sick_leave_button});
    sick_leave_keyboard->inlineKeyboard.push_back({opened_sick_leave_button});
    sick_leave_keyboard->inlineKeyboard.push_back({return_button});

    auto cancel_button = std::make_shared<TgBot::InlineKeyboardButton>();
    cancel_button->text = "Отмена";
    cancel_button->callbackData = "cancel";

    auto cancel_keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    cancel_keyboard->inlineKeyboard.push_back({cancel_button});

    auto return_keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    return_keyboard->inlineKeyboard.push_back({return_button});

    bot.getEvents().onCommand("start", [&bot, main_keyboard, &bot_state, &db](TgBot::Message::Ptr message) {
        if (db.is_user_new(message->chat->id))
        {
            bot.getApi().sendMessage(message->chat->id, "Здравствуйте, я бот для учета ваших больничных и отпусков. Выберите действие ниже.",
                                    false, 0, main_keyboard);
            db.save_user(message->chat->id);
            bot_state[message->chat->id] = State::NORMAL;
        }
    });

    bot.getEvents().onCallbackQuery([&bot, sick_leave_keyboard, main_keyboard, cancel_keyboard, return_keyboard, &bot_state, &leaves, &db](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "add_sick_leave") {
            if (bot_state[query->message->chat->id] == State::NORMAL)
            {
                bot.getApi().sendMessage(query->message->chat->id, "На данный момент больничный открыт или закрыт?", false, 0, sick_leave_keyboard);
            }
        } 
        else if (query->data == "add_vacation")
        {
            if (bot_state[query->message->chat->id] == State::NORMAL)
            {
                bot.getApi().sendMessage(query->message->chat->id, "Введите даты начала-конца в формате DD.MM.YY-DD.MM.YY", false, 0, cancel_keyboard);
                bot_state[query->message->chat->id] = State::WAIT;
                leaves[query->message->chat->id] = std::make_unique<Vacation>();
            }
        }
        else if (query->data == "closed_sick_leave")
        {
            if (bot_state[query->message->chat->id] == State::NORMAL)
            {
                bot.getApi().sendMessage(query->message->chat->id, "Введите даты начала-конца в формате DD.MM.YY-DD.MM.YY", false, 0, cancel_keyboard);
                bot_state[query->message->chat->id] = State::WAIT;
                leaves[query->message->chat->id] = std::make_unique<Sick_leave>(true);
            }
        }
        else if (query->data == "opened_sick_leave")
        {
            if (bot_state[query->message->chat->id] == State::NORMAL)
            {
                bot.getApi().sendMessage(query->message->chat->id, "Введите даты начала - предполагаемого конца в формате DD.MM.YY-DD.MM.YY", false, 0, cancel_keyboard);
                bot_state[query->message->chat->id] = State::WAIT;
                leaves[query->message->chat->id] = std::make_unique<Sick_leave>(false);
            }           
        }
        else if (query->data == "view")
        {
            if (bot_state[query->message->chat->id] == State::NORMAL)
            {
                std::string result = db.view_sick_leaves(query->message->chat->id) + "\n" + db.view_vacations(query->message->chat->id);
                bot.getApi().sendMessage(query->message->chat->id, result, false, 0, return_keyboard);
            }            
        }
        else if (query->data == "return" || query->data == "cancel")
        {
            bot.getApi().sendMessage(query->message->chat->id, "Выберите действие", false, 0, main_keyboard);
            bot_state[query->message->chat->id] = State::NORMAL;
        }
    });

    bot.getEvents().onAnyMessage([&bot, main_keyboard, cancel_keyboard, return_keyboard, &bot_state, &leaves, &db](TgBot::Message::Ptr message)
    {
        if (bot_state[message->chat->id] == State::WAIT)
        {
            if((!leaves[message->chat->id]->parse(move(message->text))))
            {
                bot.getApi().sendMessage(message->chat->id, "Неправильный ввод даты. Пожалуйста, введите даты начала-конца в формате DD.MM.YY-DD.MM.YY", false, 0, cancel_keyboard);
            }
            else
            {
                bot_state[message->chat->id] = State::NORMAL;
                leaves[message->chat->id]->save_to_data_base(message->chat->id, db);
                bot.getApi().sendMessage(message->chat->id, leaves[message->chat->id]->save_message(), false, 0, return_keyboard);
            }    
        }
    });

     try {
         printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
         printf("error: %s\n", e.what());
    }

    return 0;
}

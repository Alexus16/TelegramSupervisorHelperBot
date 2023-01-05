#pragma once
#include <string>
#include <vector>

#define TELEGRAM_TYPES_H

namespace teletypes
{
    struct User
    {
    public:
        std::string id;
        bool is_bot;
        std::string first_name;
        std::string last_name;
        std::string username;
    };

    struct Chat
    {
    public:
        std::string id;
        std::string type;
        std::string title;
        std::string username;
        std::string first_name;
        std::string last_name;

    };

    struct PollAnswer
    {
    public:
        std::string poll_id;
        User user;
        std::vector<int> option_ids;
    };

    struct PollOption
    {
    public:
        std::string text;
        int voter_count;
    };

    struct Poll
    {
    public:
        std::string id;
        std::string question;
        std::vector<PollOption> options;
        int total_voter_count;
        bool is_closed;
        bool is_anonymous;
        std::string type;
        bool allow_multiple_answers;
    };

    struct ChatMemberAdministrator
    {
    public:
        User user;
        std::string status;
        std::string custom_title;
    };

    struct Message
    {
    public:
        int message_id;
        User from;
        Chat sender_chat;
        int date;
        Chat chat;
        std::string text;
        Poll poll;
    };

    struct Update
    {
    public:
        int update_id;
        Message message;
        Message edited_message;
        Poll poll;
        PollAnswer poll_answer;
    };
}
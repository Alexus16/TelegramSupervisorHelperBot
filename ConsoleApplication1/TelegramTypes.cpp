#include <string>
#include <vector>
#include "TelegramTypes.h"

using namespace std;

namespace teletypes
{
    struct User;
    /*{
    public:
        int id;
        bool is_bot;
        string first_name;
        string last_name;
        string username;
    };*/

    struct Chat;
    /*{
    public:
        int id;
        string type;
        string title;
        string username;
        string first_name;
        string last_name;
    };*/

    struct PollAnswer;
    /*{
    public:
        string poll_id;
        User user;
        vector<int> option_ids;
    };*/

    struct PollOption;
    /*{
    public:
        string text;
        int voter_count;
    };*/

    struct Poll;
    /*{
    public:
        string id;
        string question;
        vector<PollOption> options;
        int total_voter_count;
        bool is_closed;
        bool is_anonymous;
        string type;
        bool allow_multiple_answers;
    };*/

    struct ChatMemberAdministrator;
    /*{
    public:
        User user;
        string status;
        string custom_title;
    };*/

    struct Message;
   /* {
    public:
        int message_id;
        User from;
        Chat sender_chat;
        int date;
        Chat chat;
        string text;
        Poll poll;
    };*/

    struct Update;
    /*{
    public:
        int update_id;
        Message message;
        Message edited_message;
        Poll poll;
        PollAnswer poll_answer;
    };*/
}
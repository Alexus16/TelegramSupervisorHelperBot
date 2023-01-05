#pragma once
#include <iostream>
#include <vector>
#include <cpr/cpr.h>
#include "TelegramMethods.h"

#ifndef TELEGRAM_TYPE_DESERIALIZER_H
#include "TelegramTypeDeserializer.h"
#endif

#ifndef TELEGRAM_TYPES_H
#include "TelegramTypes.h"
#endif

using namespace std;

namespace telemethods
{
    string apiUrlWithToken;
    
    string cp1251_to_utf8(const char* str) {
        string res;
        int result_u, result_c;
        result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
        if (!result_u) { return 0; }
        wchar_t* ures = new wchar_t[result_u];
        if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
            delete[] ures;
            return 0;
        }
        result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
        if (!result_c) {
            delete[] ures;
            return 0;
        }
        char* cres = new char[result_c];
        if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
            delete[] cres;
            return 0;
        }
        delete[] ures;
        res.append(cres);
        delete[] cres;
        return res;
    }

    void setApiUrlWithToken(string strUrl)
    {
        apiUrlWithToken = strUrl;
    }

	bool sendMessageTo(std::string chatId, std::string text, teletypes::Message* message)
	{
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "sendMessage" },
            cpr::Parameters{ {"text",  cp1251_to_utf8(text.c_str())}, {"chat_id", chatId}});
        
        if (r.status_code == cpr::status::HTTP_OK)
        {
            if (message != nullptr)
            {
                *message = teletypes::deserializeMessage(r.text);
            }
            return true;
        }
        cerr << "Sending message failed. Status-code: " << r.text << " " << chatId << endl;
        return false;
	}

    bool sendPollTo(std::string chatId, std::string question, std::vector<std::string> options, teletypes::Message* message,
        bool allowMultipleAnswers, bool isAnonymous)
	{
        string stringOfArrayOptions = "[";
        for (string option : options)
        {
            stringOfArrayOptions += "\"" + option + "\",";
        }
        stringOfArrayOptions = stringOfArrayOptions.substr(0, stringOfArrayOptions.length() - 1) + "]";
        cout << stringOfArrayOptions << endl;
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "sendPoll" },
            cpr::Parameters{ {"chat_id", chatId},
                            {"question", cp1251_to_utf8(question.c_str())},
                            {"options", cp1251_to_utf8(stringOfArrayOptions.c_str())},
                            {"allows_multiple_answers", to_string(allowMultipleAnswers)},
                            {"is_anonymous", to_string(isAnonymous)}});
        if (r.status_code == cpr::status::HTTP_OK)
        {
            if (message != nullptr)
            {
                *message = teletypes::deserializeMessage(r.text);
            }
            return true;
        }
        cerr << "Sending poll failed. Status-code: " << r.status_code << endl;
        return false;
	}

    bool getChatAdministrators(string chatId, vector<teletypes::ChatMemberAdministrator>* admins)
    {
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "getChatAdministrators"},
            cpr::Parameters{ {"chat_id", chatId} });
        if (r.status_code == cpr::status::HTTP_OK)
        {
            if (admins != nullptr)
            {
                *admins = teletypes::deserializeChatMemberAdministrators(r.text);
            }
            return true;
        }
        return false;
    }

    bool pinChatMessage(string chatId, string messageId)
    {
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "pinChatMessage" },
            cpr::Parameters{ {"chat_id", chatId},
            {"message_id", messageId} });
        if (r.status_code == cpr::status::HTTP_OK)
        {
            return true;
        }
        return false;
    }

    bool deleteMessage(string chatId, string messageId)
    {
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "deleteMessage" },
            cpr::Parameters{ {"chat_id", chatId},
            {"message_id", messageId} });
        if (r.status_code == cpr::status::HTTP_OK)
        {
            return true;
        }
        return false;
    }

    int offsetId = 0;
    vector<teletypes::Update> getUpdates()
    {
        auto r = cpr::Post(cpr::Url{ apiUrlWithToken + "getUpdates" },
            cpr::Timeout{ INT32_MAX },
            cpr::Parameters{{"offset", to_string(offsetId)}});
        if (r.status_code == cpr::status::HTTP_OK)
        {
            vector<teletypes::Update> res = teletypes::deserializeUpdates(r.text);
            for (auto update : res) if (update.update_id >= offsetId) offsetId = update.update_id + 1;
            return res;
        }
        cerr << "Failed to receive updates " << r.status_code << endl;
        return {};
    }
}
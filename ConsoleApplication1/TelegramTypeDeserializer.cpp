#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include "TelegramTypeDeserializer.h"

#ifndef TELEGRAM_TYPES_H
#include "TelegramTypes.h"
#endif

using namespace std;
using json = nlohmann::json;

namespace teletypes
{
	string utf8_to_cp1251(const char* str) {
		string res;
		int result_u, result_c;
		result_u = MultiByteToWideChar(65001, 0, str, -1, 0, 0);
		if (!result_u) { return 0; }
		wchar_t* ures = new wchar_t[result_u];
		if (!MultiByteToWideChar(65001, 0, str, -1, ures, result_u)) {
			delete[] ures;
			return 0;
		}
		result_c = WideCharToMultiByte(1251, 0, ures, -1, 0, 0, 0, 0);
		if (!result_c) {
			delete[] ures;
			return 0;
		}
		char* cres = new char[result_c];
		if (!WideCharToMultiByte(1251, 0, ures, -1, cres, result_c, 0, 0)) {
			delete[] cres;
			return 0;
		}
		delete[] ures;
		res.append(cres);
		delete[] cres;
		return res;
	}

	Message deserializeMessage(string data)
	{
		Message res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.message_id = processedData["message_id"];
		res.from = deserializeUser(to_string(processedData["from"]));
		res.chat = deserializeChat(to_string(processedData["chat"]));
		res.date = processedData["date"];
		if (processedData.contains("text")) res.text = utf8_to_cp1251(((string)processedData["text"]).c_str());
		if (processedData.contains("poll")) res.poll = deserializePoll(to_string(processedData["poll"]));
		if (processedData.contains("sender_chat")) res.sender_chat = deserializeChat(to_string(processedData["sender_chat"]));
		return res;
	}

	User deserializeUser(string data)
	{
		User res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.id = to_string(processedData["id"]);
		if (processedData.contains("username")) res.username = processedData["username"];
		if (processedData.contains("last_name")) res.last_name = utf8_to_cp1251(((string)processedData["last_name"]).c_str());
		if (processedData.contains("first_name")) res.first_name = utf8_to_cp1251(((string)processedData["first_name"]).c_str());
		res.is_bot = processedData["is_bot"];
		return res;
	}

	Poll deserializePoll(string data)
	{
		Poll res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.id = processedData["id"];
		res.allow_multiple_answers = processedData["allows_multiple_answers"];
		res.is_anonymous = processedData["is_anonymous"];
		res.question = processedData["question"];
		res.type = processedData["type"];
		res.total_voter_count = processedData["total_voter_count"];
		res.is_closed = processedData["is_closed"];
		for (auto option : processedData["options"])
		{
			res.options.push_back(deserializePollOption(to_string(option)));
		}
		return res;
	}

	Chat deserializeChat(string data)
	{
		Chat res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.id = to_string(processedData["id"]);
		res.type = processedData["type"];
		if (processedData.contains("username")) res.username = processedData["username"];
		if (processedData.contains("first_name")) res.first_name = processedData["first_name"];
		if (processedData.contains("last_name")) res.last_name = processedData["last_name"];
		if (processedData.contains("title")) res.title = processedData["title"];
		return res;
	}

	PollOption deserializePollOption(string data)
	{
		PollOption res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.text = processedData["text"];
		res.voter_count = processedData["voter_count"];
		return res;
	}

	PollAnswer deserializePollAnswer(string data)
	{
		PollAnswer res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.user = deserializeUser(to_string(processedData["user"]));
		res.poll_id = processedData["poll_id"];
		for (auto option : processedData["option_ids"])
		{
			res.option_ids.push_back(option);
		}
		return res;
	}

	ChatMemberAdministrator deserializeChatMemberAdministrator(string data)
	{
		ChatMemberAdministrator res;
		try
		{
			auto processedData = json::parse(data);
			if (processedData.contains("result"))
				processedData = processedData["result"];
			res.user = deserializeUser(to_string(processedData["user"]));
			if (processedData.contains("custom_title")) res.custom_title = utf8_to_cp1251(((string)processedData["custom_title"]).c_str());
		}
		catch (exception& e)
		{
			cerr << "Decoding data about student failed: " <<  e.what() << endl;
		}
		return res;

	}

	vector<ChatMemberAdministrator> deserializeChatMemberAdministrators(string data)
	{
		vector<ChatMemberAdministrator> res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		for (auto admin : processedData)
		{
			res.push_back(deserializeChatMemberAdministrator(to_string(admin)));
		}
		return res;
	}

	Update deserializeUpdate(string data)
	{
		Update res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		res.update_id = processedData["update_id"];
		if (processedData.contains("message")) res.message = deserializeMessage(to_string(processedData["message"]));
		else res.message.message_id = -1;
		if (processedData.contains("poll")) res.poll = deserializePoll(to_string(processedData["poll"]));
		else res.poll.id = "";
		if (processedData.contains("poll_answer")) res.poll_answer = deserializePollAnswer(to_string(processedData["poll_answer"]));
		else res.poll_answer.poll_id = "";
		return res;
	}

	vector<Update> deserializeUpdates(string data)
	{
		vector<Update> res;
		auto processedData = json::parse(data);
		if (processedData.contains("result"))
			processedData = processedData["result"];
		for (auto update : processedData)
		{
			res.push_back(deserializeUpdate(to_string(update)));
		}
		return res;
	}
}
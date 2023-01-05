#pragma once
#include <string>
#include <vector>

#ifndef TELEGRAM_TYPES_H
#include "TelegramTypes.h"
#endif

#define TELEGRAM_METHODS_H

namespace telemethods
{
	void setApiUrlWithToken(std::string strUrl);
	bool sendMessageTo(std::string chatId, std::string text, teletypes::Message* message);
	bool sendPollTo(std::string chatId, std::string question, std::vector<std::string> options, teletypes::Message* message,
		bool allowMultipleAnswers = false,
		bool isAnonymous = false);
	std::vector<teletypes::Update> getUpdates();
	bool getChatAdministrators(std::string chatId, std::vector<teletypes::ChatMemberAdministrator>* admins);
	bool pinChatMessage(std::string chatId, std::string messageId);
	bool deleteMessage(std::string chatId, std::string messageId);
}

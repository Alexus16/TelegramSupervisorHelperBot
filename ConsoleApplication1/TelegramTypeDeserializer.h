#include <string>
#include <vector>

#ifndef TELEGRAM_TYPES_H
#include "TelegramTypes.h"
#endif

#define TELEGRAM_TYPE_DESERIALIZER_H

namespace teletypes
{
	Message deserializeMessage(std::string data);
	User deserializeUser(std::string data);
	ChatMemberAdministrator deserializeChatMemberAdministrator(std::string data);
	std::vector<ChatMemberAdministrator> deserializeChatMemberAdministrators(std::string data);
	Poll deserializePoll(std::string data);
	PollAnswer deserializePollAnswer(std::string data);
	PollOption deserializePollOption(std::string data);
	Chat deserializeChat(std::string data);
	Update deserializeUpdate(std::string data);
	std::vector<Update> deserializeUpdates(std::string data);
}
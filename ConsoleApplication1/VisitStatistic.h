#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

namespace statistic
{
	struct StudentData
	{
	public:
		std::string userId;
		std::string fullname;
	};

	struct DayRecord
	{
	public:
		StudentData student;
		std::vector<bool> lessonStatuses;
	};

	struct DayStatistic
	{
	public:
		std::string dateStr;
		int lessonAmount;
		std::vector<DayRecord> studentRecords;
	};
	
	DayStatistic createClearStatistic(std::vector<StudentData> data, int lessonAmount, std::string dateStr);
	StudentData createStudent(std::string userId, std::string fullname);
	nlohmann::json serializeStatistic(DayStatistic stat);
	DayStatistic deserializeStatistic(std::string statData);
}
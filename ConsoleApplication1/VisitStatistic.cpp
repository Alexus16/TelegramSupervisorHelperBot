#include <iostream>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>
#include <windows.h>
#include "VisitStatistic.h"

using namespace std;
using json = nlohmann::json;

namespace statistic
{
	struct StudentData;
	struct DayRecord;
	struct DayStatistic;

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

	DayStatistic createClearStatistic(vector<StudentData> data, int lessonAmount, string dateStr)
	{
		DayStatistic res;
		res.dateStr = dateStr;
		res.lessonAmount = lessonAmount;
		for (auto student : data)
		{
			DayRecord record;
			record.student = student;
			record.lessonStatuses = vector<bool>(lessonAmount, true);
			res.studentRecords.push_back(record);
		}
		return res;
	}

	StudentData createStudent(string userId, string fullname)
	{
		StudentData data;
		data.fullname = fullname;
		data.userId = userId;
		return data;
	}


	StudentData deserializeStudentData(string studentData)
	{
		StudentData res;
		auto data = json::parse(studentData);
		res.userId = data["userId"];
		res.fullname = utf8_to_cp1251(((string)data["fullname"]).c_str());
		return res;
	}

	vector<bool> deserializeStatuses(string statusesData)
	{
		vector<bool> res;
		auto data = json::parse(statusesData);
		for (auto status : data)
		{
			res.push_back(status);
		}
		return res;
	}
	
	DayRecord deserializeRecord(string recData)
	{
		DayRecord res;
		auto data = json::parse(recData);
		res.lessonStatuses = deserializeStatuses(to_string(data["lessonStatuses"]));
		res.student = deserializeStudentData(to_string(data["student"]));
		return res;
	}

	vector<DayRecord> deserializeRecords(string recordsData)
	{
		vector<DayRecord> res;
		auto data = json::parse(recordsData);
		for (auto recordData : data)
		{
			res.push_back(deserializeRecord(to_string(recordData)));
		}
		return res;
	}

	DayStatistic deserializeStatistic(string statData)
	{
		DayStatistic res;
		auto data = json::parse(statData);
		res.dateStr = data["dateStr"];
		res.lessonAmount = data["lessonAmount"];
		res.studentRecords = deserializeRecords(to_string(data["studentRecords"]));
		return res;
	}


	json serializeStudent(StudentData studentData)
	{
		json data = json();
		data["userId"] = studentData.userId;
		data["fullname"] = cp1251_to_utf8(studentData.fullname.c_str());
		return data;
	}

	json serializeRecord(DayRecord record)
	{
		json data = json();
		data["lessonStatuses"] = record.lessonStatuses;
		data["student"] = serializeStudent(record.student);
		return data;
	}

	json serializeRecords(vector<DayRecord> records)
	{
		vector<json> serializedRecs;
		for (auto rec : records)
		{
			serializedRecs.push_back(serializeRecord(rec));
		}
		json data;
		data = serializedRecs;
		return data;
	}

	json serializeStatistic(DayStatistic stat)
	{
		json data = json();
		data["dateStr"] = stat.dateStr;
		data["lessonAmount"] = stat.lessonAmount;
		data["studentRecords"] = serializeRecords(stat.studentRecords);
		return data;
	}
}
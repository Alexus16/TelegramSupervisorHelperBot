#include <iostream>
#include <cpr/cpr.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include "TelegramTypes.h"
#include "TelegramMethods.h"
#include "VisitStatistic.h"

#define GROUP_CHAT_KEY "groupChatId"
#define SUPERVISOR_CHAT_KEY "supervisorChatId"
#define ADMIN_CHAT_KEY "adminChatId"

using namespace std;
using namespace teletypes;
using json = nlohmann::json;

const string dataPath = "";
const string apiUrl = "https://api.telegram.org/bot";
string botToken{ "" };
vector<statistic::StudentData> group;
int amountOfLessons[2][7] = { {2, 2, 3, 3, 3, 2, 0}, {2, 2, 2, 3, 3, 2, 0} };
string adminPwd{ "" }, supervisorPwd{ "" }, chatPwd{""};

string _groupChatId{ "" }, _supervisorChatId{ "" }, _adminChatId{ "" };
string _pollMessageId{ "" }, _pollId{ "" };

statistic::DayStatistic todayStatistic;
vector<statistic::StudentData> studentData;
bool isDayReopened = false;

//Definition of additional functions
string tolower(string str)
{
    string res = str;
    for (int i = 0; i < 26; i++)
    {
        int ind;
        while ((ind = res.find('A' + i)) != -1)
        {
            res[ind] = 'a' + i;
        }
    }
    for (int i = 0; i < 33; i++)
    {
        int ind;
        while ((ind = res.find('А' + i)) != -1)
        {
            res[ind] = 'а' + i;
        }
    }
    return res;
}

bool exists(const string path)
{
    ifstream f(path);
    bool isExists = f.good();
    if (!isExists)
    {
        remove(path.c_str());
    }
    return f.good();
}

int getWeekDay()
{
    time_t t = time(0);
    tm ltm;
    localtime_s(&ltm, &t);
    return (ltm.tm_wday + 6) % 7;
}

int getDayAmount(int month)
{
    switch (month)
    {
    case 0:
    case 2:
    case 4:
    case 6:
    case 7:
    case 9:
    case 11:
        return 31;
    case 3:
    case 5:
    case 8:
    case 10:
        return 30;
    case 1:
        return 28;
    }
}

int getWeekNumber()
{
    time_t t = time(0);
    tm ltm;
    localtime_s(&ltm, &t);
    int numberOfWeek = 0;
    while (true)
    {
        if (--ltm.tm_mday == 0)
        {
            if (--ltm.tm_mon < 0)
            {
                ltm.tm_mon = 11;
                ltm.tm_year--;
            }
            ltm.tm_mday = getDayAmount(ltm.tm_mon);
        }
        if (--ltm.tm_wday < 0)
        {
            ltm.tm_wday = 6;
            numberOfWeek++;
        }
        if (ltm.tm_mon == 8 && ltm.tm_wday > ltm.tm_mday) break;
    }
    return numberOfWeek;
}

string getDate(bool makeOffset = false)
{
    time_t t = time(0);
    tm ltm;
    localtime_s(&ltm, &t);
    int day{ ltm.tm_mday }, month{ ltm.tm_mon + 1 }, year{ ltm.tm_year };
    string m31 = "1 3 5 7 8 10 12 ";
    string m30 = "4 6 9 11 ";
    string m28 = "2 ";
    if (makeOffset)
    {
        if (m28.find(to_string(month) + " ") != -1)
        {
            if (day + 1 > 28)
            {
                day = 1;
                month += 1;
            }
            else
            {
                day++;
            }
        }
        else if (m30.find(to_string(month) + " ") != -1)
        {
            if (day + 1 > 30)
            {
                day = 1;
                month += 1;
            }
            else
            {
                day++;
            }
        }
        else
        {
            if (day + 1 > 31)
            {
                day = 1;
                if (month + 1 > 12)
                {
                    month = 1;
                    year++;
                }
                else
                {
                    month++;
                }
            }
            else
            {
                day++;
            }
        }
    }
    string date = (day < 10 ? "0" : "") + to_string(day) + "." + (month < 10 ? "0" : "") + to_string(month) + "." + to_string(year + 1900);
    return date;
}
//

//Critical data
void loadBotCredentials()
{
    string credentialPath = dataPath + "bot.credentials";
    if (!exists(credentialPath)) exit(1);
    ifstream sData(credentialPath);
    string token;
    if (sData.eof() || sData.peek() == '\n') exit(2);
    getline(sData, token);
    botToken = token;
}
//

void collectStudentData()
{
    cout << "Collecting data about students..." << endl;
    studentData = {};
    if (_groupChatId == "") return;
    vector<teletypes::ChatMemberAdministrator> admins;
    telemethods::getChatAdministrators(_groupChatId, &admins);
    for (auto admin : admins)
    {
        if (admin.custom_title.find("куратор") == -1)
        {
            cout << "Added student " << admin.custom_title << endl;
            studentData.push_back(statistic::createStudent(admin.user.id, admin.custom_title));
        }
    }
    cout << "Collected data about " << to_string(studentData.size()) << " students" << endl;
}

bool tryLoadChatData()
{
    string chatDataPath = dataPath + "chat_data.json";
    if (!exists(chatDataPath)) return false;
    ifstream sData(chatDataPath);
    auto data = json::parse(sData);
    bool allOk = true;
    if ((allOk = allOk && data.contains(GROUP_CHAT_KEY)))
    {
        _groupChatId = data[GROUP_CHAT_KEY];
        collectStudentData();
    }
    if ((allOk = allOk && data.contains(SUPERVISOR_CHAT_KEY))) _supervisorChatId = data[SUPERVISOR_CHAT_KEY];
    if ((allOk = allOk && data.contains(ADMIN_CHAT_KEY))) _adminChatId = data[ADMIN_CHAT_KEY];
    if (!allOk) cerr << "Unsuccessful chat data loading" << endl;
    return allOk;
}

bool saveChatData()
{
    try
    {
        string chatDataPath = dataPath + "chat_data.json";
        json data;
        if (_groupChatId != "") data[GROUP_CHAT_KEY] = _groupChatId;
        if (_supervisorChatId != "") data[SUPERVISOR_CHAT_KEY] = _supervisorChatId;
        if (_adminChatId != "") data[ADMIN_CHAT_KEY] = _adminChatId;
        ofstream sData(chatDataPath);
        sData << to_string(data);
        sData.close();
        cout << "Chat data saved" << endl;
        return true;
    }
    catch (exception e)
    {
        cerr << "Save chat data failed " << e.what() << endl;
        return false;
    }
}

bool tryLoadDayData()
{
    try
    {
        string dayDataPath = dataPath + "day_data.json";
        if (!exists(dayDataPath)) return false;
        ifstream sData(dayDataPath);
        auto data = json::parse(sData);
        todayStatistic = statistic::deserializeStatistic(to_string(data["todayStatistic"]));
        _pollId = data["pollId"];
        _pollMessageId = data["pollMessageId"];
        return true;
    }
    catch (exception e)
    {
        cerr << "Unsuccessful day data loading " << e.what() << endl;
        return false;
    }
    
}

bool saveDayData()
{
    try
    {
        string dayDataPath = dataPath + "day_data.json";
        json data;
        data["todayStatistic"] = statistic::serializeStatistic(todayStatistic);
        string s = to_string(data);
        data["pollId"] = _pollId;
        s = to_string(data);
        data["pollMessageId"] = _pollMessageId;
        ofstream sData(dayDataPath);
        s = to_string(data);
        sData << s;
        sData.close();
        cout << "Day data saved" << endl;
        return true;
    }
    catch (exception e)
    {
        cerr << "Save day data failed " << e.what() << endl;
        return false;
    }
}
//

string generateStatisticReport()
{
    string resultReport = "Статистика отсутствующих\n\n";
    resultReport += "Дата: " + todayStatistic.dateStr + "\n\n";
    for (int i = 0; i < todayStatistic.lessonAmount; i++)
    {
        string reportPart = "Пара " + to_string(i + 1) + ":\n";
        int counter = 1;
        bool isNeedToInclude = false;
        for (auto rec : todayStatistic.studentRecords)
        {
            if (!rec.lessonStatuses[i])
            {
                reportPart += to_string(counter++) + ". " + rec.student.fullname + "\n";
                isNeedToInclude = true;
            }
        }
        reportPart += "\n";
        if (isNeedToInclude) resultReport += reportPart;
    }
    return resultReport;
}

void sendStatisticTo(string chatId)
{
    telemethods::sendMessageTo(chatId, generateStatisticReport(), nullptr);
}

void closeDayAndDeletePoll()
{
    if (_pollMessageId == "") return;
    sendStatisticTo(_supervisorChatId);
    if (!telemethods::deleteMessage(_groupChatId, _pollMessageId))
        cerr << "Unsuccessful poll delete" << endl;
}

void openDayAndSendNewPoll()
{
    string dateStr = getDate(true);
    cout << "Preparing to send poll for " << getWeekNumber() << " week " << getWeekDay() << " day" << endl;
    int lessonAmount = amountOfLessons[getWeekNumber() % 2][(getWeekDay() + 1) % 7];
    todayStatistic = statistic::createClearStatistic(studentData, lessonAmount, dateStr);
    vector<string> lessonNames;
    for (int i = 0; i < lessonAmount; i++)
    {
        lessonNames.push_back("Пара " + to_string(i + 1));
    }
    Message msg;
    telemethods::sendPollTo(_groupChatId, "Какие пары прогуливаешь " + dateStr +"?", lessonNames, &msg, true, false);
    _pollMessageId = to_string(msg.message_id);
    _pollId = msg.poll.id;
    telemethods::pinChatMessage(_groupChatId, _pollMessageId);
    saveDayData();
}

void processPrivateMessage(teletypes::Message message)
{
    string cmd = tolower(message.text);
    if (message.text == supervisorPwd)
    {
        _supervisorChatId = message.from.id;
        telemethods::sendMessageTo(_supervisorChatId, "Теперь ты староста группы", nullptr);
        saveChatData();
    }
    else if (message.text == adminPwd)
    {
        _adminChatId = message.chat.id;
        telemethods::sendMessageTo(_adminChatId, "Теперь ты администратор группы", nullptr);
        saveChatData();
    }
    else if (message.chat.id == _supervisorChatId || message.chat.id == _adminChatId)
    {
        if (tolower(cmd) == "стат")
        {
            sendStatisticTo(message.chat.id);
        }
        else if(tolower(cmd) == "закрыть")
        {
            isDayReopened = true;
            telemethods::sendMessageTo(message.from.id, "Выполняю...", nullptr);
            closeDayAndDeletePoll();
            openDayAndSendNewPoll();
        }
    }
}

void processGroupMessage(teletypes::Message message)
{
    if (message.text == chatPwd)
    {
        _groupChatId = message.chat.id;
        telemethods::sendMessageTo(_groupChatId, "Чат зарегистрирован", nullptr);
        collectStudentData();
        telemethods::deleteMessage(_groupChatId, to_string(message.message_id));
        saveChatData();
    }
}

void processMessage(teletypes::Message message)
{
    if (message.chat.type == "private")
        processPrivateMessage(message);
    else
        processGroupMessage(message);
}

void processPollAnswer(teletypes::PollAnswer answer)
{
    if (answer.poll_id != _pollId) return;
    for (int j = 0; j < todayStatistic.studentRecords.size(); j++)
    {
        if (todayStatistic.studentRecords[j].student.userId == answer.user.id)
        {
            for (int i = 0; i < todayStatistic.studentRecords[j].lessonStatuses.size(); todayStatistic.studentRecords[j].lessonStatuses[i++] = true);
            for (auto option_id : answer.option_ids)
            {
                todayStatistic.studentRecords[j].lessonStatuses[option_id] = false;
            }
            saveDayData();
            break;
        }
    }
}

void threadTick()
{
    while (true)
    {
        time_t t = time(0);
        tm ltm;
        localtime_s(&ltm, &t);
        if (!isDayReopened && ltm.tm_hour == 19)
        {
            isDayReopened = true;
            closeDayAndDeletePoll();
            openDayAndSendNewPoll();
        }
        else if (isDayReopened && ltm.tm_hour == 20)
        {
            isDayReopened = false;
        }
        Sleep(1000);
    }
}

int main()
{
    setlocale(0, "");
    loadBotCredentials();
    ifstream f(dataPath + "adminPwd.txt");
    f >> adminPwd;
    cout << "SET ADMIN PASSWORD: " << adminPwd << endl;
    f.close();
    f.open(dataPath + "supervisorPwd.txt");
    f >> supervisorPwd;
    cout << "SET SUPERVISOR PASSWORD: " << supervisorPwd << endl;
    f.close();
    f.open(dataPath + "chatPwd.txt");
    f >> chatPwd;
    cout << "SET CHAT PASSWORD: " << chatPwd << endl;
    f.close();
    telemethods::setApiUrlWithToken(apiUrl + botToken + "/");
    if (!tryLoadChatData()) cerr << "No chat data loaded" << endl;
    else if (!tryLoadDayData()) cerr << "No day data loaded" << endl;
    thread th(threadTick);
    setlocale(0, "");
    while (true)
    {   
        auto updates = telemethods::getUpdates();

        for (auto update : updates)
        {
            if (update.message.message_id != -1)
            {
                processMessage(update.message);
            }
            if (update.poll_answer.poll_id != "")
            {
                processPollAnswer(update.poll_answer);
            }
        }
        Sleep(100);
    }
}
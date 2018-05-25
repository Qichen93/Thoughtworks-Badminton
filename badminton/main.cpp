//主程序
//默认为输入'!'表示输入结束
#define _CRT_SECURE_NO_DEPRECATE
//或 #pragma warning(disable:4996)

#include"tou.h"

//定义一些常量
const int cActivityCancle = -1;
const int cPayEach = 30;
const int cEveryPlace = 6;

//定义枚举常量
enum DAY_OF_WEEK
{
	EM_DAY_MON = 1,
	EM_DAY_TUE,
	EM_DAY_WED,
	EM_DAY_THU,
	EM_DAY_FRI,
	EM_DAY_SAT,
	EM_DAY_SUN
};

//建立一个哈希Map来存储不同时间段租金的关系
unordered_map<int, int> g_setWeekPay = {
	{ 9, 30 }, { 10, 30 }, { 11, 30 }, { 12, 50 },
	{ 13, 50 }, { 14, 50 }, { 15, 50 }, { 16, 50 }, { 17, 50 }, { 18, 80 }, { 19, 80 },
	{ 20, 60 }, { 21, 60 }
};
unordered_map<int, int> g_setWeekendPay = {
	{ 9, 40 }, { 10, 40 }, { 11, 40 }, { 12, 50 },
	{ 13, 50 }, { 14, 50 }, { 15, 50 }, { 16, 50 }, { 17, 50 }, { 18, 60 },
	{ 19, 60 }, { 20, 60 }, { 21, 60 }
};

//定义结构体类型
typedef struct _ACTIVITY_INFO
{
	int nYear;
	int nMonth;
	int nDay;
	int nStartTime;
	int nEndTime;
	int nPeoeleNum;
	string strRawTime;
}ACTIVITY_INFO;

typedef struct _TOTAL_MONEY
{
	int nTotalIncome;
	int nTotalPayment;
	int nTotalProfit;
}TOTAL_MONEY;

bool ParseInputTime(const string & strTime, ACTIVITY_INFO & structInfo)
{
	int nRet = sscanf(strTime.c_str(), "%d-%d-%d %d:00~%d:00 %d",&structInfo.nYear,&structInfo.nMonth,&structInfo.nDay,&structInfo.nStartTime,&structInfo.nEndTime,&structInfo.nPeoeleNum);

	if (6 != nRet)
		return false;
	int nIntervalTime = structInfo.nEndTime - structInfo.nStartTime;
	if (nIntervalTime != 2 && nIntervalTime != 3)
		return false;

	bool bRet = true;
	auto iterBlank = strTime.rfind(" ");
	if (iterBlank != string::npos)
		structInfo.strRawTime = strTime.substr(0, iterBlank);
	else
		bRet = false;

	return bRet;

}

int GetDayOfWeek(int nYear, int nMonth, int nDay)
{
	//2016-06-02 20:00~22:00
	int iWeek = 0;
	unsigned int y = 0, c = 0, m = 0, d = 0;

	if (nMonth == 1 || nMonth == 2)
	{
		c = (nYear - 1) / 100;
		y = (nYear - 1) % 100;
		m = nMonth + 12;
		d = nDay;
	}
	else
	{
		c = nYear / 100;
		y = nYear % 100;
		m = nMonth;
		d = nDay;
	}
	//蔡勒公式  
	iWeek = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1;

	iWeek = iWeek >= 0 ? (iWeek % 7) : (iWeek % 7 + 7);
	if (iWeek == 0)
	{
		iWeek = 7;
	}

	return iWeek;
}

int  GetOrderStrategy(const int nPlaceNum, const int nRedundant)
{
	int nTotalPlace = 0;
	if (0 == nPlaceNum)
	{
		if (nRedundant < 4)
			nTotalPlace = cActivityCancle;
		else
			nTotalPlace = 1;
	}
	else if (1 == nPlaceNum)
	{
		nTotalPlace = 2;
	}
	else if (2 == nPlaceNum || 3 == nPlaceNum)
	{
		if (nRedundant >= 4)
			nTotalPlace = nPlaceNum + 1;
		else
			nTotalPlace = nPlaceNum;
	}
	else
	{
		nTotalPlace = nPlaceNum;
	}
	return nTotalPlace;
}

int GetTotalPay(int nDayOfWeek, int nStartTime, int nEndTime)
{
	//时间必然为整小时
	int nTotalPay = 0;
	int nCurrentHour = nStartTime;
	switch (nDayOfWeek)
	{
	case EM_DAY_MON:
	case EM_DAY_TUE:
	case EM_DAY_WED:
	case EM_DAY_THU:
	case EM_DAY_FRI:
		while (nCurrentHour < nEndTime)
		{
			auto iter = g_setWeekPay.find(nCurrentHour);
			if (iter != g_setWeekPay.end())
				nTotalPay += iter->second;
			nCurrentHour++;
		}
		break;
	case EM_DAY_SAT:
	case EM_DAY_SUN:
		while (nCurrentHour < nEndTime)
		{
			auto iter = g_setWeekendPay.find(nCurrentHour);
			if (iter != g_setWeekendPay.end())
				nTotalPay += iter->second;
			nCurrentHour++;
		}
		break;
	default:
		break;
	}
	return nTotalPay;
}

string generateEachLineSummary(const string &strEachLine)
{
	string eachLineOutPut("");
	ACTIVITY_INFO tmActivity;
	if (!ParseInputTime(strEachLine, tmActivity))
	{
		printf("Input Format Error\n");
		exit(0);
	}

	int nPlaceNum = tmActivity.nPeoeleNum / cEveryPlace;
	int nRedudant = tmActivity.nPeoeleNum%cEveryPlace;
	int nTotalPlace = GetOrderStrategy(nPlaceNum, nRedudant);
	if (nTotalPlace == cActivityCancle)
	{
		eachLineOutPut += tmActivity.strRawTime + "+0 -0 0";
	}
	else
	{
		int nDayOfWeek = GetDayOfWeek(tmActivity.nYear,tmActivity.nMonth,tmActivity.nDay);
		int nPay = GetTotalPay(nDayOfWeek,tmActivity.nStartTime,tmActivity.nEndTime);
		nPay *= nTotalPlace;
		int nIncome = tmActivity.nPeoeleNum*cPayEach;
		int nProfit = nIncome - nPay;
		if (nProfit > 0)
		{
			eachLineOutPut = tmActivity.strRawTime + " +" + to_string(nIncome);
			eachLineOutPut += " -" + to_string(nPay) + " +"+to_string(nProfit);
		}
		else
		{
			eachLineOutPut = tmActivity.strRawTime + " +" + std::to_string(nIncome);
			eachLineOutPut += " -" + std::to_string(nPay) + " " + std::to_string(nProfit);
		}
	}
	
	return eachLineOutPut;
}

string generateSummary(const string &input)
{
	string strSummary("");
	vector<string> strEachLine;
	auto iter = input.find("\n");
	int nStartPos = 0;
	while (iter != string::npos)
	{
		strEachLine.push_back(input.substr(nStartPos, iter - nStartPos));
		nStartPos = iter + 1;
		iter = input.find("\n", iter + 1);
	}
	if (!input.substr(nStartPos).empty())
		strEachLine.push_back(input.substr(nStartPos));

	for (size_t i = 0; i < strEachLine.size(); i++)
	{
		strSummary += generateEachLineSummary(strEachLine[i]);
		strSummary += "\n";
	}

	return strSummary;
}

TOTAL_MONEY GetTotalMoney(const string & strSummary)
{
	TOTAL_MONEY totalMoney{ 0, 0, 0 };  //列表初始化
	auto iter = strSummary.find("\n");
	int nStartPos = 0;
	while (iter != string::npos)
	{
		string strEachLine = strSummary.substr(nStartPos, iter - nStartPos);
		auto iterProfit = strEachLine.rfind(" ");
		if (iterProfit != string::npos)
			totalMoney.nTotalProfit += stoi(strEachLine.substr(iterProfit));
		auto iterPay = strEachLine.rfind(" ", iterProfit - 1);
		if (iterPay != std::string::npos)
			totalMoney.nTotalPayment += std::stoi(strEachLine.substr(iterPay, iterProfit - iterPay));

		auto iterIncome = strEachLine.rfind(" ", iterPay - 1);
		if (iterIncome != std::string::npos)
			totalMoney.nTotalIncome += std::stoi(strEachLine.substr(iterIncome, iterPay - iterIncome));

		nStartPos = iter + 1;
		iter = strSummary.find("\n", iter + 1);
	}
	return totalMoney;
}

int main()
{
	string strInput("");
	string consoleInput("");
	while (getline(cin, consoleInput))
	{
		if (consoleInput == "!")
			break;
		strInput += consoleInput + "\n";
	}
	string strSummary = generateSummary(strInput);
	printf("[Summary]\n\n");
	printf("%s", strSummary.c_str());

	TOTAL_MONEY totalMoney = GetTotalMoney(strSummary);
	printf("Total Income: %d\n",totalMoney.nTotalIncome);
	printf("Total Payment: %d\n", -totalMoney.nTotalPayment);
	printf("Total Profit: %d\n", totalMoney.nTotalProfit);

	system("pause");
	return 0;
	return 0;
}
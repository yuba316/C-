#include <iostream>
#include <stdio.h>
#include <string>
#include "CustomerMdSpi.h"

using namespace std;

#pragma comment (lib, "thostmduserapi_se.lib")
#pragma comment (lib, "thosttraderapi_se.lib")

TThostFtdcBrokerIDType gBrokerID = "9999"; // 期货公司代码，或者用simnow的模拟代码
TThostFtdcInvestorIDType gInvesterID = "162548"; // 在simnow注册的模拟账号及密码
TThostFtdcPasswordType gInvesterPassword = "iamaman369";
CThostFtdcMdApi* g_pMdUserApi = nullptr; // Api接口对象
char gMdFrontAddr[] = "tcp://180.168.146.187:10131"; // 前置地址可在simnow官网找到7*24的测试地址
char* g_pInstrumentID[] = { "AP007","TA011" }; // 行情合约代码列表，中、上、大、郑交易所各选一种
int instrumentNum = 4; // 行情合约订阅数量

int main()
{
	// 初始化行情线程
	cout << "初始化行情" << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(); // 创建行情实例
	CThostFtdcMdSpi* pMdUserSpi = new CustomerMdSpi;   // 创建行情回调实例
	g_pMdUserApi->RegisterSpi(pMdUserSpi);             // 注册事件类
	g_pMdUserApi->RegisterFront(gMdFrontAddr);         // 设置行情前置地址
	g_pMdUserApi->Init();                              // 连接运行

	g_pMdUserApi->Join();
	delete pMdUserSpi;
	g_pMdUserApi->Release();

	getchar();
	return 0;
}
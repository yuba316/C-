#include "CustomerMdSpi.h"
#include <iostream>

extern CThostFtdcMdApi* g_pMdUserApi; // 订阅行情用的Api接口对象
extern char gMdFrontAddr[]; // 前置地址，需要与期货公司的BrokerID一一对应
extern TThostFtdcBrokerIDType gBrokerID; // 期货公司代码
extern TThostFtdcInvestorIDType gInvesterID; // 用户登录用账号，同UserID
extern TThostFtdcPasswordType gInvesterPassword; // 密码
extern char* g_pInstrumentID[]; // 需要订阅的合约代码
extern int instrumentNum; // 行情订阅数

void CustomerMdSpi::OnFrontConnected() {
	std::cout << "连接接口成功" << std::endl;
	// 响应的意思就是，在接收到接口成功连接后返回的应答信息以后，Api接口对象需要做出的下一步指令
	// 此处在确保连接成功以后，就应该进行登录操作
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq)); // 清空登录信息
	strcpy_s(loginReq.BrokerID, gBrokerID); // 重新写入账号密码
	strcpy_s(loginReq.UserID, gInvesterID);
	strcpy_s(loginReq.Password, gInvesterPassword);
	static int requestID = 0;
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << "登录请求发送成功" << std::endl;
	else
		std::cerr << "登录请求发送失败" << std::endl;
}

void CustomerMdSpi::OnFrontDisconnected(int nReason)
{	
	// nReason 错误原因
	// 0x1001 网络读失败
	// 0x1002 网络写失败
	// 0x2001 接收心跳超时
	// 0x2002 发送心跳失败
	// 0x2003 收到错误报文
	std::cerr << "接口连接已断开(error: " << nReason << ")" << std::endl;
}

void CustomerMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "连接心跳超时，距离上次连接时间：" << nTimeLapse << std::endl;
}

void CustomerMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField* pRspUserLogin,
	CThostFtdcRspInfoField* pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "用户登录成功" << std::endl;
		std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "用户名： " << pRspUserLogin->UserID << std::endl;
		// 开始订阅行情
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt) {
			std::cout << "请求订阅行情成功" << std::endl;
			std::cout << "合约代码" << ","
				<< "更新时间" << ","
				<< "最新价" << ","
				<< "成交量" << ","
				<< "买价一" << ","
				<< "买量一" << ","
				<< "卖价一" << ","
				<< "卖量一" << ","
				<< "持仓量" << ","
				<< "换手率" << std::endl;
		}
		else
			std::cerr << "请求订阅行情失败" << std::endl;
	}
	else
		std::cerr << "用户登录失败(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
}

void CustomerMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField* pUserLogout,
	CThostFtdcRspInfoField* pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "用户退出成功" << std::endl;
		std::cout << "经纪商： " << pUserLogout->BrokerID << std::endl;
		std::cout << "用户名： " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "用户退出失败(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
}

void CustomerMdSpi::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "错误报告(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
}

void CustomerMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	// 逐行输出行情，与上文的位置相对应
	std::cout << pDepthMarketData->InstrumentID << "," 
		<< pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << "," 
		<< pDepthMarketData->LastPrice << "," 
		<< pDepthMarketData->Volume << "," 
		<< pDepthMarketData->BidPrice1 << "," 
		<< pDepthMarketData->BidVolume1 << "," 
		<< pDepthMarketData->AskPrice1 << "," 
		<< pDepthMarketData->AskVolume1 << "," 
		<< pDepthMarketData->OpenInterest << "," 
		<< pDepthMarketData->Turnover << std::endl;
}
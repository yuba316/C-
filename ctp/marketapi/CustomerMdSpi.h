#pragma once
#include <vector>
#include "ThostFtdcMdApi.h"

class CustomerMdSpi: public CThostFtdcMdSpi
{
	// 继承父类Spi类，用于定义应答Api发出请求后响应回复的函数
	// 一般来讲，行情接口的Spi类就只需要响应连接成功后的登录以及登录成功后的行情订阅
public:
	void OnFrontConnected(); // 响应连接成功
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	// 响应登录成功
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	// 响应行情订阅
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
};
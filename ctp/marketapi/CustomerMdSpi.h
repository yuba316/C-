#pragma once
#include <vector>
#include "ThostFtdcMdApi.h"

class CustomerMdSpi: public CThostFtdcMdSpi
{
	// �̳и���Spi�࣬���ڶ���Ӧ��Api�����������Ӧ�ظ��ĺ���
	// һ������������ӿڵ�Spi���ֻ��Ҫ��Ӧ���ӳɹ���ĵ�¼�Լ���¼�ɹ�������鶩��
public:
	void OnFrontConnected(); // ��Ӧ���ӳɹ�
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	// ��Ӧ��¼�ɹ�
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	// ��Ӧ���鶩��
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
};
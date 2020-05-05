#include "CustomerMdSpi.h"
#include <iostream>

extern CThostFtdcMdApi* g_pMdUserApi; // ���������õ�Api�ӿڶ���
extern char gMdFrontAddr[]; // ǰ�õ�ַ����Ҫ���ڻ���˾��BrokerIDһһ��Ӧ
extern TThostFtdcBrokerIDType gBrokerID; // �ڻ���˾����
extern TThostFtdcInvestorIDType gInvesterID; // �û���¼���˺ţ�ͬUserID
extern TThostFtdcPasswordType gInvesterPassword; // ����
extern char* g_pInstrumentID[]; // ��Ҫ���ĵĺ�Լ����
extern int instrumentNum; // ���鶩����

void CustomerMdSpi::OnFrontConnected() {
	std::cout << "���ӽӿڳɹ�" << std::endl;
	// ��Ӧ����˼���ǣ��ڽ��յ��ӿڳɹ����Ӻ󷵻ص�Ӧ����Ϣ�Ժ�Api�ӿڶ�����Ҫ��������һ��ָ��
	// �˴���ȷ�����ӳɹ��Ժ󣬾�Ӧ�ý��е�¼����
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq)); // ��յ�¼��Ϣ
	strcpy_s(loginReq.BrokerID, gBrokerID); // ����д���˺�����
	strcpy_s(loginReq.UserID, gInvesterID);
	strcpy_s(loginReq.Password, gInvesterPassword);
	static int requestID = 0;
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << "��¼�����ͳɹ�" << std::endl;
	else
		std::cerr << "��¼������ʧ��" << std::endl;
}

void CustomerMdSpi::OnFrontDisconnected(int nReason)
{	
	// nReason ����ԭ��
	// 0x1001 �����ʧ��
	// 0x1002 ����дʧ��
	// 0x2001 ����������ʱ
	// 0x2002 ��������ʧ��
	// 0x2003 �յ�������
	std::cerr << "�ӿ������ѶϿ�(error: " << nReason << ")" << std::endl;
}

void CustomerMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "����������ʱ�������ϴ�����ʱ�䣺" << nTimeLapse << std::endl;
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
		std::cout << "�û���¼�ɹ�" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�����̣� " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "�û����� " << pRspUserLogin->UserID << std::endl;
		// ��ʼ��������
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt) {
			std::cout << "����������ɹ�" << std::endl;
			std::cout << "��Լ����" << ","
				<< "����ʱ��" << ","
				<< "���¼�" << ","
				<< "�ɽ���" << ","
				<< "���һ" << ","
				<< "����һ" << ","
				<< "����һ" << ","
				<< "����һ" << ","
				<< "�ֲ���" << ","
				<< "������" << std::endl;
		}
		else
			std::cerr << "����������ʧ��" << std::endl;
	}
	else
		std::cerr << "�û���¼ʧ��(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
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
		std::cout << "�û��˳��ɹ�" << std::endl;
		std::cout << "�����̣� " << pUserLogout->BrokerID << std::endl;
		std::cout << "�û����� " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "�û��˳�ʧ��(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
}

void CustomerMdSpi::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "���󱨸�(errorID: " << pRspInfo->ErrorID << ", errorMsg: " << pRspInfo->ErrorMsg << ")" << std::endl;
}

void CustomerMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	// ����������飬�����ĵ�λ�����Ӧ
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
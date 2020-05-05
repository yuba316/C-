#include <iostream>
#include <stdio.h>
#include <string>
#include "CustomerMdSpi.h"

using namespace std;

#pragma comment (lib, "thostmduserapi_se.lib")
#pragma comment (lib, "thosttraderapi_se.lib")

TThostFtdcBrokerIDType gBrokerID = "9999"; // �ڻ���˾���룬������simnow��ģ�����
TThostFtdcInvestorIDType gInvesterID = "162548"; // ��simnowע���ģ���˺ż�����
TThostFtdcPasswordType gInvesterPassword = "iamaman369";
CThostFtdcMdApi* g_pMdUserApi = nullptr; // Api�ӿڶ���
char gMdFrontAddr[] = "tcp://180.168.146.187:10131"; // ǰ�õ�ַ����simnow�����ҵ�7*24�Ĳ��Ե�ַ
char* g_pInstrumentID[] = { "AP007","TA011" }; // �����Լ�����б��С��ϡ���֣��������ѡһ��
int instrumentNum = 4; // �����Լ��������

int main()
{
	// ��ʼ�������߳�
	cout << "��ʼ������" << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(); // ��������ʵ��
	CThostFtdcMdSpi* pMdUserSpi = new CustomerMdSpi;   // ��������ص�ʵ��
	g_pMdUserApi->RegisterSpi(pMdUserSpi);             // ע���¼���
	g_pMdUserApi->RegisterFront(gMdFrontAddr);         // ��������ǰ�õ�ַ
	g_pMdUserApi->Init();                              // ��������

	g_pMdUserApi->Join();
	delete pMdUserSpi;
	g_pMdUserApi->Release();

	getchar();
	return 0;
}
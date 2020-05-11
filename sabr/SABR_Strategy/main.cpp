#include "calculator.h"
#include "CustomerMdSpi.h"

using namespace std;

TThostFtdcBrokerIDType gBrokerID = "16333"; // �ڻ���˾���룬������simnow��ģ����룬�˴��õ��ǻ����ڻ�
TThostFtdcInvestorIDType gInvesterID = "162548"; // ��simnowע���ģ���˺ż����룬����ӿڵ�ʹ��ʵ���Ͽ��Բ����˺ź������½
TThostFtdcPasswordType gInvesterPassword = "iamaman369";
CThostFtdcMdApi* g_pMdUserApi = nullptr; // Api�ӿڶ���
char gMdFrontAddr[] = "tcp://182.131.17.103:41168"; // ǰ�õ�ַ����simnow�����ҵ�7*24�Ĳ��Ե�ַ
map<string, vector<double>> quoteMap; // �洢��Լʵʱ����ı����CustomerMdSpi.cpp

// �������ݶ��ǿ��Դ������ط���ȡ�ģ�����˵pythonÿ��������Ҫʵʱ���е���Ȩ��Լ�������굱������ʺ͵������ٴ���C++���и��ӵ��Ż�����
char* g_pInstrumentID[] = { "m2009","m2009-C-2600","m2009-C-2650","m2009-C-2700","m2009-C-2750","m2009-C-2800","m2009-C-2850","m2009-C-2900" };
int instrumentNum = 8; // �����Լ��������
string underlying_code = "m2009"; // ��ĺ�Լ����
double T = 0.369863, rf = 0.030275, beta = 0; // �����պ��޷������ʣ����õ���SHBOR����beta���԰���������޸ģ��䶯���Ǳ����۸�ķֲ����
double exercise_price[7] = { 2600,2650,2700,2750,2800,2850,2900 };

vector<string> calTdSabr(char** code, int n, double T, double rf, double beta, double* exercise_price) {
	calculator c;
	double underlying_price = quoteMap[(code[0])][0];
	vector<double> bid_price, ask_price, bid_IV, ask_IV; // ��Ϊ��ۺ����۷ֿ�������Ҫ���պ�����ĺ�Լ���Լ�С�����۲���������
	double temp_price, temp_IV;
	for (int i = 1; i < n; i++) {
		temp_price = quoteMap[(code[i])][2];
		temp_IV = c.calIV(exercise_price[i-1], true, T, rf, underlying_price, temp_price);
		bid_price.push_back(temp_price);
		bid_IV.push_back(temp_IV);
		temp_price = quoteMap[(code[i])][4];
		temp_IV = c.calIV(exercise_price[i-1], true, T, rf, underlying_price, temp_price);
		ask_price.push_back(temp_price);
		ask_IV.push_back(temp_IV);
	}
	int m = n - 1, len = sizeof(double);
	double* new_bid_IV = new double[m];
	double* new_ask_IV = new double[m];
	memcpy(new_bid_IV, &bid_IV[0], m * len);
	memcpy(new_ask_IV, &ask_IV[0], m * len);
	double bid_alpha = c.calAlpha(exercise_price, new_bid_IV, m, underlying_price);
	double ask_alpha = c.calAlpha(exercise_price, new_ask_IV, m, underlying_price);
	vector<double> bid_vr = c.calVegaRho(exercise_price, new_bid_IV, m, T, rf, underlying_price, bid_alpha, beta);
	vector<double> ask_vr = c.calVegaRho(exercise_price, new_ask_IV, m, T, rf, underlying_price, ask_alpha, beta);
	double new_bid_vr[2] = { bid_vr[0],bid_vr[1] }, new_ask_vr[2] = { ask_vr[0],ask_vr[1] };
	vector<double> bid_dis, ask_dis;
	double temp_sabr;
	for (int i = 0; i < m; i++) {
		temp_sabr = c.calSabr(exercise_price[i], T, rf, underlying_price, bid_alpha, beta, new_bid_vr);
		bid_dis.push_back(bid_IV[i] - temp_sabr);
		temp_sabr = c.calSabr(exercise_price[i], T, rf, underlying_price, ask_alpha, beta, new_ask_vr);
		ask_dis.push_back(temp_sabr - ask_IV[i]);
	}
	int sell_iterator = distance(begin(bid_dis), max_element(begin(bid_dis), end(bid_dis)));
	int buy_iterator = distance(begin(ask_dis), max_element(begin(ask_dis), end(ask_dis)));
	vector<string> res;
	res.push_back((code[sell_iterator + 1]));
	res.push_back(to_string(bid_IV[sell_iterator]));
	res.push_back(to_string(bid_IV[sell_iterator] - bid_dis[sell_iterator]));
	res.push_back(to_string(bid_vr[0]));
	res.push_back(to_string(bid_vr[1]));
	res.push_back((code[buy_iterator + 1]));
	res.push_back(to_string(ask_IV[buy_iterator]));
	res.push_back(to_string(ask_IV[buy_iterator] + ask_dis[buy_iterator]));
	res.push_back(to_string(ask_vr[0]));
	res.push_back(to_string(ask_vr[1]));
	return res;
}

int main() {
	// ��ʼ�������߳�
	cout << "��ʼ������" << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(); // ��������ʵ��
	CThostFtdcMdSpi* pMdUserSpi = new CustomerMdSpi;   // ��������ص�ʵ��
	g_pMdUserApi->RegisterSpi(pMdUserSpi);             // ע���¼���
	g_pMdUserApi->RegisterFront(gMdFrontAddr);         // ��������ǰ�õ�ַ
	g_pMdUserApi->Init();                              // ��������

	while (true) {
		if (quoteMap.size() == instrumentNum) {
			break;
		}
	}

	vector<string> combination = calTdSabr(g_pInstrumentID, instrumentNum, T, rf, beta, exercise_price);
	cout << "==================================================" << endl;
	cout << endl;
	cout << "��ǰsabr�������Ϊ��" << endl;
	cout << "�� " << combination[0] << " VIX: " << combination[1] << " sabr: " << combination[2] << " vega: " << combination[3] << " rho: " << combination[4] << endl;
	cout << "�� " << combination[5] << " VIX: " << combination[6] << " sabr: " << combination[7] << " vega: " << combination[8] << " rho: " << combination[9] << endl;
	cout << endl;
	cout << "==================================================" << endl;

	while (true) { // CTP�ӿڷ��ص�����ʵ������һ���첽�Ĺ��̣���������ķ���������while trueѭ��ʵ��ʵʱ��sabr������ϼ���
		vector<string> temp = calTdSabr(g_pInstrumentID, instrumentNum, T, rf, beta, exercise_price);
		if ((temp[0] != combination[0]) || (temp[5] != combination[5])) {
			combination = temp;
			cout << "==================================================" << endl;
			cout << endl;
			cout << "��ǰsabr������ϱ��Ϊ��" << endl;
			cout << "�� " << combination[0] << " VIX: " << combination[1] << " sabr: " << combination[2] << " vega: " << combination[3] << " rho: " << combination[4] << endl;
			cout << "�� " << combination[5] << " VIX: " << combination[6] << " sabr: " << combination[7] << " vega: " << combination[8] << " rho: " << combination[9] << endl;
			cout << endl;
			cout << "==================================================" << endl;
		}
	}

	g_pMdUserApi->Join();
	delete pMdUserSpi;
	g_pMdUserApi->Release();

	getchar();
	return 0;
}
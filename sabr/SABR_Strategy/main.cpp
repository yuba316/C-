#include "calculator.h"
#include "CustomerMdSpi.h"

using namespace std;

TThostFtdcBrokerIDType gBrokerID = "16333"; // 期货公司代码，或者用simnow的模拟代码，此处用的是华西期货
TThostFtdcInvestorIDType gInvesterID = "162548"; // 在simnow注册的模拟账号及密码，行情接口的使用实际上可以不用账号和密码登陆
TThostFtdcPasswordType gInvesterPassword = "iamaman369";
CThostFtdcMdApi* g_pMdUserApi = nullptr; // Api接口对象
char gMdFrontAddr[] = "tcp://182.131.17.103:41168"; // 前置地址可在simnow官网找到7*24的测试地址
map<string, vector<double>> quoteMap; // 存储合约实时行情的表，详见CustomerMdSpi.cpp

// 以下内容都是可以从其他地方获取的，比如说python每天设置需要实时盯市的期权合约，计算完当天的利率和到期日再传给C++进行复杂的优化计算
char* g_pInstrumentID[] = { "m2009","m2009-C-2600","m2009-C-2650","m2009-C-2700","m2009-C-2750","m2009-C-2800","m2009-C-2850","m2009-C-2900" };
int instrumentNum = 8; // 行情合约订阅数量
string underlying_code = "m2009"; // 标的合约代码
double T = 0.369863, rf = 0.030275, beta = 0; // 到期日和无风险利率（采用的是SHBOR），beta可以按需求进行修改，变动的是标的物价格的分布情况
double exercise_price[7] = { 2600,2650,2700,2750,2800,2850,2900 };

vector<string> calTdSabr(char** code, int n, double T, double rf, double beta, double* exercise_price) {
	calculator c;
	double underlying_price = quoteMap[(code[0])][0];
	vector<double> bid_price, ask_price, bid_IV, ask_IV; // 分为买价和卖价分开计算需要做空和做多的合约，以减小买卖价差带来的误差
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
	// 初始化行情线程
	cout << "初始化行情" << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(); // 创建行情实例
	CThostFtdcMdSpi* pMdUserSpi = new CustomerMdSpi;   // 创建行情回调实例
	g_pMdUserApi->RegisterSpi(pMdUserSpi);             // 注册事件类
	g_pMdUserApi->RegisterFront(gMdFrontAddr);         // 设置行情前置地址
	g_pMdUserApi->Init();                              // 连接运行

	while (true) {
		if (quoteMap.size() == instrumentNum) {
			break;
		}
	}

	vector<string> combination = calTdSabr(g_pInstrumentID, instrumentNum, T, rf, beta, exercise_price);
	cout << "==================================================" << endl;
	cout << endl;
	cout << "当前sabr套利组合为：" << endl;
	cout << "空 " << combination[0] << " VIX: " << combination[1] << " sabr: " << combination[2] << " vega: " << combination[3] << " rho: " << combination[4] << endl;
	cout << "多 " << combination[5] << " VIX: " << combination[6] << " sabr: " << combination[7] << " vega: " << combination[8] << " rho: " << combination[9] << endl;
	cout << endl;
	cout << "==================================================" << endl;

	while (true) { // CTP接口返回的行情实际上是一个异步的过程，所以最暴力的方法就是用while true循环实现实时的sabr套利组合计算
		vector<string> temp = calTdSabr(g_pInstrumentID, instrumentNum, T, rf, beta, exercise_price);
		if ((temp[0] != combination[0]) || (temp[5] != combination[5])) {
			combination = temp;
			cout << "==================================================" << endl;
			cout << endl;
			cout << "当前sabr套利组合变更为：" << endl;
			cout << "空 " << combination[0] << " VIX: " << combination[1] << " sabr: " << combination[2] << " vega: " << combination[3] << " rho: " << combination[4] << endl;
			cout << "多 " << combination[5] << " VIX: " << combination[6] << " sabr: " << combination[7] << " vega: " << combination[8] << " rho: " << combination[9] << endl;
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
#include "calculator.h"

int main() {
	calculator c;
	double exercise_price[5] = { 2.2,2.25,2.3,2.35,2.4 }, last_price[5] = { 0.0418,0.0142,0.0053,0.0027,0.0016 }, IV[5];
	double T = 0.027397, rf = 0.030275, underlying_price = 2.229, beta = 0, alpha;
	vector<double> vr;
	for (int i = 0; i < 5; i++) {
		IV[i] = c.calIV(exercise_price[i],true,T,rf,underlying_price,last_price[i]);
		cout << to_string(IV[i]) << endl;
	}
	alpha = c.calAlpha(exercise_price, IV, 5, underlying_price);
	vr = c.calVegaRho(exercise_price, IV, 5, T, rf, underlying_price, alpha, beta);
	cout << "alpha: " << to_string(alpha) << " vega: " << to_string(vr[0]) << " rho: " << to_string(vr[1]) << endl;
	double x[2] = { vr[0],vr[1] };
	double sabr = c.calSabr(exercise_price[2], T, rf, underlying_price, alpha, beta, x);
	cout << "sabr: " << to_string(sabr) << endl;
	return 0;
}
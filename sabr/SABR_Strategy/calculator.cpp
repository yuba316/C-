#include "calculator.h"

typedef struct{
	double* exercise_price;
	double* IV;
	double T, rf, underlying_price, alpha, beta;
	int n;
}f_data_type;

double calculator::calIV(double exercise_price, bool CorP, double T, double rf, double underlying_price, double last_price) {
	double BSM_price = 0, sigma = 0.5, up = 1, dw = 0, d1, d2;
	boost::math::normal_distribution<> norm(0, 1);
	if (CorP) {
		while (abs(BSM_price - last_price) > 1e-6) {
			d1 = (log(underlying_price / exercise_price) + (rf + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
			d2 = d1 - sigma * sqrt(T);
			BSM_price = underlying_price * (1-boost::math::cdf(boost::math::complement(norm, d1))) -
				exercise_price * exp(-1 * rf * T) * (1-boost::math::cdf(boost::math::complement(norm, d2)));
			if (BSM_price > last_price) {
				up = sigma;
				sigma = (sigma + dw) / 2;
			}
			else if (BSM_price < last_price) {
				dw = sigma;
				sigma = (sigma + up) / 2;
			}
			else {
				break;
			}
		}
	}
	else {
		while (abs(BSM_price - last_price) > 1e-6) {
			d1 = (log(underlying_price / exercise_price) + (rf + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
			d2 = d1 - sigma * sqrt(T);
			BSM_price = exercise_price * exp(-1 * rf * T) * (1-boost::math::cdf(boost::math::complement(norm, -1 * d2))) -
				underlying_price * (1-boost::math::cdf(boost::math::complement(norm, -1 * d1)));
			if (BSM_price > last_price) {
				up = sigma;
				sigma = (sigma + dw) / 2;
			}
			else if (BSM_price < last_price) {
				dw = sigma;
				sigma = (sigma + up) / 2;
			}
			else {
				break;
			}
		}
	}
	return sigma;
}

double calculator::calAlpha(double* price, double* IV, int n, double ATM_price) {
	Spline sp(price, IV, n, GivenSecondOrder, 0, 0);
	double pred_x[1] = { ATM_price };
	double pred_y[1];
	sp.MultiPointInterp(pred_x, 1, pred_y);
	return pred_y[0];
}

double calculator::calSabr(double exercise_price, double T, double rf, double underlying_price, double alpha, double beta, const double* opt_args) {
	double vega = opt_args[0], rho = opt_args[1];
	double z = (vega / alpha) * pow(underlying_price * exercise_price, (1 - beta) / 2) * log(underlying_price / exercise_price);
	double X = log(sqrt(1 - 2 * rho * z + pow(z, 2)) + z - rho) / (1 - rho);
	double a = ((pow((1 - beta) * alpha, 2) / (24 * pow(underlying_price * exercise_price, 1 - beta)) +
		rho * beta * vega * alpha / (4 * pow(underlying_price * exercise_price, (1 - beta) / 2)) +
		(2 - 3 * pow(rho, 2)) * pow(vega, 2) / 24) * T + 1) * alpha;
	double b = pow(underlying_price * exercise_price, (1 - beta) / 2) * (1 + pow((1 - beta) * log(underlying_price / exercise_price), 2) / 24 + pow((1 - beta) * log(underlying_price / exercise_price), 4) / 1920);
	double sabr = a / b * z / X;
	return sabr;
}

double optMSE(unsigned n, const double* x, double* grad, void* f_data_p) {
	calculator c;
	f_data_type* args = (f_data_type*) f_data_p;
	double* exercise_price = args->exercise_price;
	double* IV = args->IV;
	double T = args->T, rf = args->rf, underlying_price = args->underlying_price, alpha = args->alpha, beta = args->beta;
	int m = args->n;
	double MSE = 0, SABR;
	for (int i = 0; i < m; i++) {
		SABR = c.calSabr(exercise_price[i], T, rf, underlying_price, alpha, beta, x);
		MSE = MSE + pow(SABR - IV[i], 2);
	}
	cout << "vega: " << x[0] << " rho: " << x[1] << " MSE: " << MSE << endl;
	return MSE / m;
}

double SABRconstraint(unsigned n, const double* x, double* grad, void* f_data_p) {
	calculator c;
	f_data_type* args = (f_data_type*)f_data_p;
	double* exercise_price = args->exercise_price;
	double* IV = args->IV;
	double T = args->T, rf = args->rf, underlying_price = args->underlying_price, alpha = args->alpha, beta = args->beta;
	int m = args->n;
	double SABR;
	for (int i = 0; i < m; i++) {
		SABR = c.calSabr(exercise_price[i], T, rf, underlying_price, alpha, beta, x);
		if (SABR < 0) {
			break;
		}
	};
	return -1 * SABR;
}

vector<double> calculator::calVegaRho(double* exercise_price, double* IV, int n, double T, double rf, double underlying_price, double alpha, double beta) {
	f_data_type f_data;
	f_data.exercise_price = exercise_price;
	f_data.IV = IV;
	f_data.n = n;
	f_data.T = T;
	f_data.rf = rf;
	f_data.underlying_price = underlying_price;
	f_data.alpha = alpha;
	f_data.beta = beta;
	f_data_type* f_data_p;
	f_data_p = &f_data;

	nlopt_opt opt;
	opt = nlopt_create(NLOPT_LN_NELDERMEAD, 2);
	double lb[2] = { 0,-1 }, rb[2] = { 1,1 }; // left & right bound condition
	nlopt_set_lower_bounds(opt, lb);
	nlopt_set_upper_bounds(opt, rb);
	nlopt_add_inequality_constraint(opt, SABRconstraint, f_data_p, 0);
	nlopt_set_min_objective(opt, optMSE, f_data_p);
	nlopt_set_xtol_rel(opt, 1e-6);
	double x[2] = { 0.5,0.5 };
	double optimal_dis = 1;
	nlopt_optimize(opt, x, &optimal_dis);
	cout << "dis: " << to_string(optimal_dis) << " vega: " << to_string(x[0]) << " rho: " << to_string(x[1]) << endl;
	vector<double> vr{ x[0],x[1] };
	return vr;
}
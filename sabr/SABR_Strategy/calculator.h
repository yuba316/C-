#pragma once
#include <iostream>
#include <cmath>
#include <boost\math\distributions\normal.hpp>
#include <vector>
#include "Spline.h"
#include <nlopt.h>
#include <nlopt.hpp>

using namespace std;
using namespace SplineSpace;
using namespace nlopt;

class calculator
{
public:
	double calIV(double exercise_price, bool CorP, double T, double rf, double underlying_price, double last_price);
	double calAlpha(double* price, double* IV, int n, double ATM_price);
	double calSabr(double exercise_price, double T, double rf, double underlying_price, double alpha, double beta, const double* opt_args);
	vector<double> calVegaRho(double* exercise_price, double* IV, int n, double T, double rf, double underlying_price, double alpha, double beta);
};


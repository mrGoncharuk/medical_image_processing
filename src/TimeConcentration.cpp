#include "TimeConcentration.hpp"



TimeConcentration::TimeConcentration(/* args */): TE(30),
												repetitionTime(1500),
												t0(0),
												TTA(0),
												baseline(0),
												PE(0),
												TTP(0),
												rTTP(0),
												FWHM(0),
												WiR(0),
												WoR(0),
												timeBetweenShots(1500)
{
}

TimeConcentration::~TimeConcentration()
{
}

void TimeConcentration::init(int p_TE, int p_repetitionTime)
{
	TE = p_TE;
	repetitionTime = p_repetitionTime;
}


void TimeConcentration::calcBaseline()
{
	// int PE_index = data.end() - std::min_element(data.begin(), data.end());
	// this->PE = *std::min_element(data.begin(), data.end());

	this->t0 = 0;
	std::vector<double> before_t0;
	double S = 0;

	for (size_t i = 0; i < curve.size(); i++)
	{
		before_t0.push_back(curve[i]);
		S = std::accumulate(before_t0.begin(), before_t0.end(), 0.0);
		S = S / before_t0.size();
		if (i == curve.size() - 1)
		{
			this->t0 = 10;
			break;
		}
		if (abs((S - curve[i + 1]) / S) > 0.15)
		{
			this->t0 = i - 1;
			break;
		}
	}

	this->baseline = std::accumulate(data.begin(), data.begin() + t0, 0.0) / t0;
}

void TimeConcentration::flipCurve()
{
    curve.clear();
	for (int i = 0; i < data.size(); ++i)
		curve.push_back( (-1.0 / this->TE) * log(data[i]/this->baseline));
}

void TimeConcentration::calcTTP()
{
	this->TTP = std::max_element(curve.begin(), curve.end()) - curve.begin();
	this->TTP = this->TTP * this->repetitionTime / 1000.0;
}

void TimeConcentration::calcrTTP()
{
	this->rTTP = std::max_element(curve.begin(), curve.end()) - curve.begin() - this->t0;
	this->rTTP = this->TTP * this->repetitionTime / 1000.0;
}

static double get_slope(double x1, double y1, double x2, double y2)
{
	double denominator = (x2 - x1);
	if (denominator == 0)
		return 0;

	return (y2 - y1) / denominator;
}


void TimeConcentration::calcWashRate()
{
	double tPEAK = std::max_element(curve.begin(), curve.end()) - curve.begin();
	double trec = tPEAK + 3;
	double x1 = t0;
	double y1 = curve[t0];
	double x_max = tPEAK;
	double y_max = curve[tPEAK];
	double x2 = trec;
	double y2 = curve[trec];

	this->WiR = get_slope(x1, y1, x_max, y_max);
	this->WoR = get_slope(x_max, y_max, x2, y2);
}

void TimeConcentration::calcFWHM()
{
	double max_y = *std::max_element(curve.begin(), curve.end());
	std::vector<int> xs;

	for (int i = 0; i < curve.size(); ++i)
	{
		if (curve[i] > max_y / 2.0)
			xs.push_back(i);
	}
    if (xs.size() < 2)
        this->FWHM = 0;
    else
	    this->FWHM = abs(*std::max_element(xs.begin(), xs.end()) - *std::min_element(xs.begin(), xs.end())) * this->repetitionTime / 1000;
}

void TimeConcentration::setData(std::vector<double> p_data)
{
	this->data = p_data;
	this->calcBaseline();

	// Build time-concentration curve
	this->flipCurve();

	// TTA(time to arrival) calculation
	this->TTA = this->t0 * this->repetitionTime / 1000.0;

	// PE(peak enhancement) calcuation
	this->PE = *std::max_element(curve.begin(), curve.end());

	// TTP(time to peak) calculation
	this->calcTTP();

	// rTTP(relative time to peak) calculation
	this->calcrTTP();

	// WiR and WoR calculation
	this->calcWashRate();

	// FWHM(Full width at half maximum) calculation
	this->calcFWHM();
}

double TimeConcentration::getTTA() const { return TTA;}
double TimeConcentration::getbaseline() const { return baseline;}
double TimeConcentration::getPE() const { return PE;}
double TimeConcentration::getTTP() const { return TTP;}
double TimeConcentration::getrTTP() const { return rTTP;}
double TimeConcentration::getFWHM() const { return FWHM;}
double TimeConcentration::getWiR() const { return WiR;}
double TimeConcentration::getWoR() const { return WoR;}
std::vector<double> TimeConcentration::getCurve() const { return curve; };


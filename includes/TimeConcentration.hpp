#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

class TimeConcentration
{
private:
	int TE;
	int repetitionTime;
	int t0;
	double TTA;
	double baseline;
	double PE;
	double TTP;
	double rTTP;
	double FWHM;
	double WiR;
	double WoR;
	double timeBetweenShots;
	std::vector<double> data;
	std::vector<double> curve;
	void calcBaseline();
	void flipCurve();
	void calcTTP();
	void calcrTTP();
	void calcWashRate();
	void calcFWHM();
public:
	TimeConcentration(/* args */);
	void init(int p_TE, int p_repetitionTime);
	void setData(std::vector<double> raw_data);
	double getTTA() const;
	double getbaseline() const;
	double getPE() const;
	double getTTP() const;
	double getrTTP() const;
	double getFWHM() const;
	double getWiR() const;
	double getWoR() const;
	std::vector<double> getCurve() const;
	~TimeConcentration();
};


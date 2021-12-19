#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>

class CTStatist
{
private:
	double   mathExpectation;
	double   entropy2D;
	double   energy2D;
	double   asymmetryCoef;
	double   meanAbsDiff;
	cv::Mat histogram;
	cv::Mat roi;
	cv::Mat histogram2;
	cv::Mat roi2;
	void   calcMathExpectation();
	void   calcEntropy2D();
	void   calcEnergy2D();
	void   calcAsymmetryCoef();
	void   calcMeanAbsDiff();
public:
	CTStatist(/* args */);
	~CTStatist();
	void setROI(const cv::Mat &p_roi);
	void setHistogram(const cv::Mat &p_histogram);
	void setData2D(const cv::Mat &p_roi, const cv::Mat &p_histogram);

	void recalculate();
	double	getMathExpectation() const;
	double	getEntropy2D() const;
	double	getEnergy2D() const;
	double	getAsymmetryCoef() const;
	double	getMeanAbsDiff() const;
};

#include "CTStatist.hpp"
using namespace std;


CTStatist::CTStatist(/* args */):mathExpectation(0),
								entropy2D(0),
								energy2D(0),
								asymmetryCoef(0),
								meanAbsDiff(0)
{
}

CTStatist::~CTStatist()
{
}


void CTStatist::calcMathExpectation()
{
	double	*hist_ptr = histogram.ptr<double>(0);
	double sum = 0;

	for (int i = 0; i <  histogram.size[0]; i++)
		sum += i * (hist_ptr[i] / histogram.size[0]);

	this->mathExpectation = sum;
}

static cv::Mat calcFreq2D(cv::Mat roi, cv::Mat roi2, cv::Mat histogram, cv::Mat histogram2)
{
	size_t max_size = MAX(histogram.size[0], histogram2.size[0]) + 1;
	cv::Mat hist2d = cv::Mat(max_size, max_size, CV_64F);

	double * roi_flat = (double*)roi.data;
	double * roi2_flat = (double*)roi2.data;

	for (int i = 0; i < roi.size[0] * roi.size[1]; i++)
		hist2d.at<double>(roi_flat[i], roi2_flat[i]) += 1;
	hist2d = hist2d / (roi.size[0] * roi.size[1]);

	return hist2d;
}

void	CTStatist::calcEntropy2D()
{

	cv::Mat hist2d = calcFreq2D(roi, roi2, histogram, histogram2);

	double entropy = 0;
	for (int i = 0; i < hist2d.size[0]; i++)
		for (int j = 0; j < hist2d.size[1]; j++)
			if (hist2d.at<double>(i,j))
				entropy += hist2d.at<double>(i,j) * log2(hist2d.at<double>(i,j));

	this->entropy2D = entropy;
}


void	CTStatist::calcEnergy2D()
{
	cv::Mat hist2d = calcFreq2D(roi, roi2, histogram, histogram2);

	double energy = 0;
	for (int i = 0; i < hist2d.size[0]; i++)
		for (int j = 0; j < hist2d.size[1]; j++)
			energy += pow(hist2d.at<double>(i,j), 2);

	this->energy2D = energy;
}


void	CTStatist::calcAsymmetryCoef()
{
    cv::Scalar skewness,mean,stddev;
    skewness.val[0]=0;

    cv::meanStdDev(roi,mean,stddev,cv::Mat());
    int sum0;
    float den0=0;
    int N=roi.rows*roi.cols;

    for (int i=0;i<roi.rows;i++)
    {
        for (int j=0;j<roi.cols;j++)
        {
            sum0=roi.ptr<double>(i)[j]-mean.val[0];
            skewness.val[0]+=pow(sum0, 3);
            den0+=pow(sum0, 2);
        }
    }
    skewness.val[0]=skewness.val[0]*sqrt(N)/(den0*sqrt(den0));
	this->asymmetryCoef = skewness.val[0];
}


void	CTStatist::calcMeanAbsDiff()
{
	cv::Mat hist2d = calcFreq2D(roi, roi2, histogram, histogram2);

	double ad = 0;
	for (int i = 0; i < hist2d.size[0]; i++)
		for (int j = 0; j < hist2d.size[1]; j++)
			ad += abs(i - j) * hist2d.at<double>(i,j);
	
	this->meanAbsDiff = ad;
}


void CTStatist::recalculate()
{
	if (!roi.empty())
	{
		calcMathExpectation();
		calcAsymmetryCoef();
	}
	if (!roi.empty() && !roi2.empty() && roi.size == roi2.size)
	{
		calcEntropy2D();
		calcEnergy2D();
		calcMeanAbsDiff();
	}
	
}


void CTStatist::setROI(const cv::Mat &p_roi)
{
	p_roi.convertTo(this->roi, CV_64F);
	// std::cout << this->roi << std::endl;
}
void CTStatist::setHistogram(const cv::Mat &p_histogram)
{
	p_histogram.convertTo(this->histogram, CV_64F);
	// std::cout << this->histogram << std::endl;
}

void CTStatist::setData2D(const cv::Mat &p_roi, const cv::Mat &p_histogram)
{
	p_roi.convertTo(this->roi2, CV_64F);
	p_histogram.convertTo(this->histogram2, CV_64F);
}

double	CTStatist::getMathExpectation() const { return this->mathExpectation; }
double	CTStatist::getEntropy2D() const { return this->entropy2D; }
double	CTStatist::getEnergy2D() const { return this->energy2D; }
double	CTStatist::getAsymmetryCoef() const { return this->asymmetryCoef; }
double	CTStatist::getMeanAbsDiff() const { return this->meanAbsDiff; }


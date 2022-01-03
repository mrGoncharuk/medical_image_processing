#pragma once
#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


enum class Plane
{
	TRANSVERSE,
	SAGITTAL,
	FRONTAL
};



template<class T> 
class SetSlicer
{
private:
	int w, h, l, t, thickness;
	std::vector<std::vector<cv::Mat_<T>>> transverse;
	std::vector<std::vector<cv::Mat_<T>>> sagittal;
	std::vector<std::vector<cv::Mat_<T>>> frontal;
	void histogramNormalization(T* data, int width, int height, T max_val);
	void rotate(T *img, int width, int height);

public:
	SetSlicer();
	SetSlicer(T **data, const int w, const int h, const int l);
	void init(T **data, const int w, const int h, const int l);
	void init(T **data, std::map<std::string, int> &params);
	T *getTransverseSlice(size_t index, size_t time);
	T *getSagittalSlice(size_t index, size_t time);
	T *getFrontalSlice(size_t index, size_t time);
	T *getSlice(Plane planeType, size_t index, size_t time);
	std::vector<double> getPointInTime(Plane planeType, size_t slice, size_t x, size_t y);
	~SetSlicer();
};


template<class T> 
SetSlicer<T>::SetSlicer():w(0), h(0), l(0)
{
}

template<class T> 
void SetSlicer<T>::init(T **data, std::map<std::string, int> &params)
{
	using namespace std;
	using namespace cv;
	this->w = params["width"];
	this->h = params["heigth"];
	this->l = params["length"];
	this->t = params["time"];
	this->thickness = params["thickness"];

	for (size_t frame = 0; frame < t; ++frame)
	{
		vector<Mat_<T>> slice_set;
		for (size_t slice = 0; slice < l; ++slice)
		{
			Mat_<T> img = Mat_<T>(h, w, data[slice*t + frame]);
			slice_set.push_back(img);
		}
		transverse.push_back(slice_set);
	}

	for (size_t frame = 0; frame < t; ++frame)
	{
		vector<Mat_<T>> slice_set;

		for (size_t col = 0; col < w; ++col)
		{
			Mat_<T> result;
			for (size_t i = 0; i < l; ++i)
				for (size_t s = 0; s < thickness; s++)
					result.push_back(transverse[frame][i].col(col));
			result = result.reshape(0, l*thickness);
			rotate((T*)result.data, h, l*thickness);
			histogramNormalization((T*)result.data, h, l*thickness, std::numeric_limits<T>::max());
			slice_set.push_back(result);
		}
		sagittal.push_back(slice_set);
	}

	for (size_t frame = 0; frame < t; ++frame)
	{
		vector<Mat_<T>> slice_set;

		for (size_t row = 0; row < h; ++row)
		{
			Mat_<T> result;
			for (size_t i = 0; i < l; ++i)
				for (size_t s = 0; s < thickness; s++)
					result.push_back(transverse[frame][i].row(row));
			result = result.reshape(0, l*thickness);
			rotate((T*)result.data, w, l*thickness);
			histogramNormalization((T*)result.data, w, l*thickness, std::numeric_limits<T>::max());
			slice_set.push_back(result);
		}
		frontal.push_back(slice_set);
	}

	for (size_t frame = 0; frame < t; ++frame)
		for (size_t slice = 0; slice < l; ++slice)
			histogramNormalization((T*)transverse[frame][slice].data, w, h, std::numeric_limits<T>::max());
}


template<class T>
T *SetSlicer<T>::getTransverseSlice(size_t index, size_t time)
{
	if (index > l)
		index = l - 1;
	if (time > t)
		time = t - 1;
	return (T*)this->transverse[time][index].data;
}

template<class T>
T *SetSlicer<T>::getSagittalSlice(size_t index, size_t time)
{
	if (index > w)
		index = w - 1;
	if (time > t)
		time = t - 1;
	return (T*)this->sagittal[time][index].data;
}

template<class T>
T *SetSlicer<T>::getFrontalSlice(size_t index, size_t time)
{
	if (index > h)
		index = h - 1;
	if (time > t)
		time = t - 1;
	return (T*)this->frontal[time][index].data;
}

template<class T>
SetSlicer<T>::~SetSlicer()
{

}

template<class T> 
void SetSlicer<T>::rotate(T *img, int width, int height)
{
	T *pixels = new T[width * height];

	double radians = M_PI;
	int sinf = (int) sin(radians);
	int cosf = (int) cos(radians);

	double x0 = 0.5 * (width - 1);	 // point to rotate about
	double y0 = 0.5 * (height - 1);	 // center of image

	// rotation
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			long double a = x - x0;
			long double b = y - y0;
			int xx = (int) (+a * cosf - b * sinf + x0);
			int yy = (int) (+a * sinf + b * cosf + y0);

			if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
				pixels[(y * width + x)] = img[(yy * width + xx)];
			}
		}
	}
	for (int i = 0; i < width*height; i++)
		img[i] = pixels[i];
	delete []pixels;
}


template<class T>
void SetSlicer<T>::histogramNormalization(T* pdata, int width, int height, T max_val)
{
	int size = width * height;
	T max_el = *std::max_element(pdata, pdata + size);

	for (int i = 0; i < size; ++i)
	{
		pdata[i] = pdata[i] / float(max_el) * max_val;
	}

	// int total = width*height;
	// int n_bins = max_val + 1;

	// // Compute histogram
	// std::vector<int> hist(n_bins, 0);
	// for (int i = 0; i < total; ++i) {
	//	 hist[pdata[i]]++;
	// }

	// // Build LUT from cumulative histrogram

	// // Find first non-zero bin
	// int i = 0;
	// while (!hist[i]) ++i;

	// if (hist[i] == total) {
	//	 for (int j = 0; j < total; ++j) { 
	//		 pdata[j] = i; 
	//	 }
	//	 return;
	// }

	// // Compute scale
	// float scale = (n_bins - 1.f) / (total - hist[i]);

	// // Initialize lut
	// std::vector<int> lut(n_bins, 0);
	// i++;

	// int sum = 0;
	// for (; i < hist.size(); ++i) {
	//	 sum += hist[i];
	//	 // the value is saturated in range [0, max_val]
	//	 lut[i] = std::max(T(0), std::min(T(round(sum * scale)), max_val));
	// }

	// // Apply equalization
	// for (int i = 0; i < total; ++i) {
	//	 pdata[i] = lut[pdata[i]];
	// }
}



template<class T>
T *SetSlicer<T>::getSlice(Plane planeType, size_t index, size_t time)
{
	switch (planeType)
	{
	case Plane::TRANSVERSE:
		if (index > l)
			index = l - 1;
		if (time > t)
			time = t - 1;
		return (T*)this->transverse[time][index].data;
	case Plane::SAGITTAL:
		if (index > w)
			index = w - 1;
		if (time > t)
			time = t - 1;
		return (T*)this->sagittal[time][index].data;

	case Plane::FRONTAL:
		if (index > h)
			index = h - 1;
		if (time > t)
			time = t - 1;
		return (T*)this->frontal[time][index].data;
	default:
		if (index > l)
			index = l - 1;
		if (time > t)
			time = t - 1;
		return (T*)this->transverse[time][index].data;
	}
}

template<class T>
std::vector<double> SetSlicer<T>::getPointInTime(Plane planeType, size_t slice, size_t x, size_t y)
{
	std::vector<double> data;

	switch (planeType)
	{
	case Plane::TRANSVERSE:
		if (slice > l - 1)
			slice = l - 1;
		if (x > w - 1)
			x = w - 1;
		if (y > h)
			y = h - 1;

		for (int time = 0; time < t; ++time)
			data.push_back(static_cast<double>(transverse[time][slice](x, y)));
		return data;

	case Plane::SAGITTAL:
		if (slice > w - 1)
			slice = w - 1;
		if (x > h - 1)
			x = h - 1;
		if (y > l*thickness)
			y = l*thickness - 1;

		for (int time = 0; time < t; ++time)
			data.push_back(static_cast<double>(sagittal[time][slice](x, y)));
		return data;

	case Plane::FRONTAL:
		if (slice > h - 1)
			slice = h - 1;
		if (x > w - 1)
			x = w - 1;
		if (y > l*thickness)
			y = l*thickness - 1;

		for (int time = 0; time < t; ++time)
			data.push_back(static_cast<double>(frontal[time][slice](x, y)));
		return data;
	default:
		if (slice > l - 1)
			slice = l - 1;
		if (x > w - 1)
			x = w - 1;
		if (y > h)
			y = h - 1;

		for (int time = 0; time < t; ++time)
			data.push_back(static_cast<double>(transverse[time][slice](x, y)));
		return data;
	}
}


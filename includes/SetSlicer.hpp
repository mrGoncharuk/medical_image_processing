#pragma once
#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
template<class T> 
class SetSlicer
{
private:
    int w, h, l;
    T **transverse;
    T **sagital;
    T **frontal;
    void histogramEqualization(T* data, int width, int height, T max_val);
public:
    SetSlicer();
    SetSlicer(T **data, const int w, const int h, const int l);
    void init(T **data, const int w, const int h, const int l);
    T *getTransverseSlice(size_t index);
    T *getSagitalSlice(size_t index);
    T *getFrontalSlice(size_t index);
    ~SetSlicer();
};

template<class T> 
void rotate(T *img, int width, int height)
{
    T *pixels = new T[width * height];

    double radians = M_PI;
    int sinf = (int) sin(radians);
    int cosf = (int) cos(radians);

    double x0 = 0.5 * (width - 1);     // point to rotate about
    double y0 = 0.5 * (height - 1);     // center of image

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
SetSlicer<T>::SetSlicer():w(0), h(0), l(0), transverse(NULL), sagital(NULL), frontal(NULL)
{
}

template<class T>
void SetSlicer<T>::init(T **data, const int w, const int h, const int l)
{
    this->w = w;
    this->h = h;
    this->l = l;
    transverse = new T *[l];
	for (int i = 0; i < l; ++i)
    {
		transverse[i] = new T[w * h];
        for (int j = 0; j < w * h; ++j)
            transverse[i][j] = data[i][j];
        histogramEqualization(transverse[i], w, h, std::numeric_limits<T>::max());
    }

    sagital = new T *[h];
    for (int i = 0; i < w; ++i)
    {
        sagital[i] = new T[l * w];
        for (int j = 0; j < l; ++j)
            for (int k = 0; k < w; ++k)
                sagital[i][j * w + k] = transverse[j][k * w + i];
        histogramEqualization(sagital[i], l, w, std::numeric_limits<T>::max());
        rotate<T>(sagital[i], l, w);
    }

    frontal = new T *[w];
    for (int i = 0; i < h; ++i)
    {
        frontal[i] = new T[l * h];
        for (int j = 0; j < l; ++j)
            for (int k = 0; k < h; ++k)
                frontal[i][j * h + k] = transverse[j][i * h + k];
        histogramEqualization(frontal[i], l, h, std::numeric_limits<T>::max());
    }
}


template<class T> 
SetSlicer<T>::SetSlicer(T **data, const int width, const int heigth, const int length):w(width), h(heigth), l(length)
{
    transverse = new T *[l];
	for (int i = 0; i < l; ++i)
    {
		transverse[i] = new T[w * h];
        for (int j = 0; j < w * h; ++j)
            transverse[i][j] = data[i][j];
        histogramEqualization(transverse[i], w, h, std::numeric_limits<T>::max());
    }

    frontal = new T *[h];
    for (int i = 0; i < w; ++i)
    {
        frontal[i] = new T[l * w];
        for (int j = 0; j < l; ++j)
            for (int k = 0; k < w; ++k)
                frontal[i][j * w + k] = transverse[j][k * w + i];
        histogramEqualization(frontal[i], l, w, std::numeric_limits<T>::max());
        rotate<T>(frontal[i], l, w);
    }

    sagital = new T *[w];
    for (int i = 0; i < h; ++i)
    {
        sagital[i] = new T[l * h];
        for (int j = 0; j < l; ++j)
            for (int k = 0; k < h; ++k)
                sagital[i][j * h + k] = transverse[j][i * h + k];
        histogramEqualization(sagital[i], l, h, std::numeric_limits<T>::max());
    }
}
template<class T>
T *SetSlicer<T>::getTransverseSlice(size_t index)
{
    if (index > l)
        return this->transverse[l - 1];
    return this->transverse[index];
}
template<class T>
T *SetSlicer<T>::getSagitalSlice(size_t index)
{
    if (index > w)
        return this->sagital[w - 1];
    return this->sagital[index];

}
template<class T>
T *SetSlicer<T>::getFrontalSlice(size_t index)
{
    if (index > h)
        return this->frontal[h - 1];
    return this->frontal[index];
}


template<class T>
SetSlicer<T>::~SetSlicer()
{
	for (int i = 0; i < l; ++i)
        delete []transverse[i];
    delete []transverse;

    for (int i = 0; i < w; ++i)
        delete []frontal[i];
    delete []frontal;

    for (int i = 0; i < h; ++i)
        delete []sagital[i];
    delete []sagital;
}


template<class T>
void SetSlicer<T>::histogramEqualization(T* pdata, int width, int height, T max_val)
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
    //     hist[pdata[i]]++;
    // }

    // // Build LUT from cumulative histrogram

    // // Find first non-zero bin
    // int i = 0;
    // while (!hist[i]) ++i;

    // if (hist[i] == total) {
    //     for (int j = 0; j < total; ++j) { 
    //         pdata[j] = i; 
    //     }
    //     return;
    // }

    // // Compute scale
    // float scale = (n_bins - 1.f) / (total - hist[i]);

    // // Initialize lut
    // std::vector<int> lut(n_bins, 0);
    // i++;

    // int sum = 0;
    // for (; i < hist.size(); ++i) {
    //     sum += hist[i];
    //     // the value is saturated in range [0, max_val]
    //     lut[i] = std::max(T(0), std::min(T(round(sum * scale)), max_val));
    // }

    // // Apply equalization
    // for (int i = 0; i < total; ++i) {
    //     pdata[i] = lut[pdata[i]];
    // }
}

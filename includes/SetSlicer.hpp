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
    T **transverseOriginal;
    void histogramNormalization(T* data, int width, int height, T max_val);
public:
    SetSlicer();
    SetSlicer(T **data, const int w, const int h, const int l);
    void init(T **data, const int w, const int h, const int l);
    T *getTransverseSlice(size_t index);
    T *getTransverseSliceOriginal(size_t index);

    ~SetSlicer();
};


template<class T> 
SetSlicer<T>::SetSlicer():w(0), h(0), l(0), transverse(NULL), transverseOriginal(NULL)
{
}

template<class T>
void SetSlicer<T>::init(T **data, const int w, const int h, const int l)
{
    this->w = w;
    this->h = h;
    this->l = l;
    
    transverseOriginal = new T *[l];
	for (int i = 0; i < l; ++i)
    {
		transverseOriginal[i] = new T[w * h];
        for (int j = 0; j < w * h; ++j)
            transverseOriginal[i][j] = data[i][j];
    }

    transverse = new T *[l];
	for (int i = 0; i < l; ++i)
    {
		transverse[i] = new T[w * h];
        for (int j = 0; j < w * h; ++j)
            transverse[i][j] = data[i][j];
        histogramNormalization(transverse[i], w, h, std::numeric_limits<T>::max());
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
T *SetSlicer<T>::getTransverseSliceOriginal(size_t index)
{
    if (index > l)
        return this->transverse[l - 1];
    return this->transverseOriginal[index];

}

template<class T>
SetSlicer<T>::~SetSlicer()
{
	for (int i = 0; i < l; ++i)
        delete []transverse[i];
    delete []transverse;

	for (int i = 0; i < l; ++i)
        delete []transverseOriginal[i];
    delete []transverseOriginal;
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

#include <iostream>
#include <cmath>
#include <chrono>
#include <limits.h>

#include <algorithm>
#include <iterator>
//!
//! \fn void std_to_box(int boxes[], float sigma, int n)  
//!
//! \brief this function converts the standard deviation of 
//! Gaussian blur into dimensions of boxes for box blur. For 
//! further details please refer to :
//!
//! \param[out] boxes   boxes dimensions
//! \param[in] sigma    Gaussian standard deviation
//! \param[in] n        number of boxes
//!
void std_to_box(int boxes[], float sigma, int n)  
{
    // ideal filter width
    float wi = std::sqrt((12*sigma*sigma/n)+1); 
    int wl = std::floor(wi);  
    if(wl%2==0) wl--;
    int wu = wl+2;
                
    float mi = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    int m = std::round(mi);
                
    for(int i=0; i<n; i++) 
        boxes[i] = ((i < m ? wl : wu) - 1) / 2;
}

//!
//! \fn void horizontal_blur(int * in, int * out, int w,int h, int r)   
//!
//! \brief this function performs the horizontal blur pass for box blur. 
//!
//! \param[in,out] in       source channel
//! \param[in,out] out      target channel
//! \param[in] w            image width
//! \param[in] h            image height
//! \param[in] r            box dimension
//!
void horizontal_blur(int * in, int * out, int w, int h, int r) 
{
    float iarr = 1.f / (r+r+1);
    #pragma omp parallel for
    for(int i=0; i<h; i++) 
    {
        int ti = i*w, li = ti, ri = ti+r, fv = in[ti], lv = in[ti+w-1], val = (r+1)*fv;
        for(int j=0;   j<r;   j++) { val += in[ti+j]; }
        for(int j=0  ; j<=r ; j++) { val += in[ri++] - fv      ; out[ti++] = std::round(val*iarr); }
        for(int j=r+1; j<w-r; j++) { val += in[ri++] - in[li++]; out[ti++] = std::round(val*iarr); }
        for(int j=w-r; j<w  ; j++) { val += lv       - in[li++]; out[ti++] = std::round(val*iarr); }
    }
}

//!
//! \fn void total_blur(int * in, int * out, int w, int h, int r)   
//!
//! \brief this function performs the total blur pass for box blur. 
//!
//! \param[in,out] in       source channel
//! \param[in,out] out      target channel
//! \param[in] w            image width
//! \param[in] h            image height
//! \param[in] r            box dimension
//!
void total_blur(int * in, int * out, int w, int h, int r) 
{
    float iarr = 1.f / (r+r+1);
    #pragma omp parallel for
    for(int i=0; i<w; i++) 
    {
        int ti = i, li = ti, ri = ti+r*w, fv = in[ti], lv = in[ti+w*(h-1)], val = (r+1)*fv;
        for(int j=0;   j<r;   j++) { val += in[ti+j*w]; }
        for(int j=0  ; j<=r ; j++) { val += in[ri] - fv    ; out[ti] = std::round(val*iarr); ri+=w; ti+=w; }
        for(int j=r+1; j<h-r; j++) { val += in[ri] - in[li]; out[ti] = std::round(val*iarr); li+=w; ri+=w; ti+=w; }
        for(int j=h-r; j<h  ; j++) { val += lv     - in[li]; out[ti] = std::round(val*iarr); li+=w; ti+=w; }
    }
}

//!
//! \fn void box_blur(int * in, int * out, int w, int h, int r)   
//!
//! \brief this function performs a box blur pass. 
//!
//! \param[in,out] in       source channel
//! \param[in,out] out      target channel
//! \param[in] w            image width
//! \param[in] h            image height
//! \param[in] r            box dimension
//!
void box_blur(int *& in, int *& out, int w, int h, int r) 
{
    std::swap(in, out);
    horizontal_blur(out, in, w, h, r);
    total_blur(in, out, w, h, r);
    // Note to myself : 
    // here we could go anisotropic with different radiis rx,ry in HBlur and TBlur
}

//!
//! \fn void fast_gaussian_blur(int * in, int * out, int w, int h, float sigma)   
//!
//! \brief this function performs a fast Gaussian blur. Applying several
//! times box blur tends towards a true Gaussian blur. Three passes are sufficient
//! for good results. For further details please refer to :  
//!
//! \param[in,out] in       source channel
//! \param[in,out] out      target channel
//! \param[in] w            image width
//! \param[in] h            image height
//! \param[in] sigma        gaussian std dev
//!
void fast_gaussian_blur(int *& in, int *& out, int w, int h, float sigma) 
{
    // sigma conversion to box dimensions
    int boxes[3];
    std_to_box(boxes, sigma, 3);
    box_blur(in, out, w, h, boxes[0]);
    box_blur(out, in, w, h, boxes[1]);
    box_blur(in, out, w, h, boxes[2]);
}


unsigned short *gaus_blur(unsigned short *image_data, int width, int height, int channels, float sigma)
{   

    std::cout << "Source image: " << width<<"x" << height << " ("<<channels<<")" << std::endl;
    if(channels < 3)
    {
        std::cout<< "Input images must be RGB images."<<std::endl;
        exit(1);
    }

    // copy data
    int size = width * height;
    
    // output channels r,g,b
    int * newb = new int[size];
    int * newg = new int[size];
    int * newr = new int[size];
    
    // input channels r,g,b
    int * oldb = new int[size];
    int * oldg = new int[size];
    int * oldr = new int[size];

    // channels copy r,g,b
    for(int i = 0; i < size; ++i)
    {
        oldb[i] = image_data[channels * i + 0];
        oldg[i] = image_data[channels * i + 1];
        oldr[i] = image_data[channels * i + 2];
    }

    // per channel filter
    auto start = std::chrono::system_clock::now();
    fast_gaussian_blur(oldb, newb, width, height, sigma);
    fast_gaussian_blur(oldg, newg, width, height, sigma);
    fast_gaussian_blur(oldr, newr, width, height, sigma);
    auto end = std::chrono::system_clock::now();

    // stats
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time " << elapsed << "ms" << std::endl;

    // channels copy r,g,b
    for(int i = 0; i < size; ++i)
    {
        image_data[channels * i + 0] = (unsigned short) std::min(SHRT_MAX, std::max(0, newb[i]));
        image_data[channels * i + 1] = (unsigned short) std::min(SHRT_MAX, std::max(0, newg[i]));
        image_data[channels * i + 2] = (unsigned short) std::min(SHRT_MAX, std::max(0, newr[i]));
    }


    // clean memory
    delete[] newr;
    delete[] newb;
    delete[] newg;
    delete[] oldr;
    delete[] oldb;
    delete[] oldg;

    return image_data;
}


/* get sign of number */
bool getSign(int data)
{
	if(data > 0)      /* positif data */
		return (true);
	else            /* negatif data */
		return (false);
}


void zeroCrossing(int *data, unsigned short *zerCross, int nx)
{
	int i;
	bool sign1, sign2;

	for(i=0; i<nx-1; i++)     /* loop over data  */
	{
		sign1 = getSign(data[i]);
		sign2 = getSign(data[i+1]);
		if(sign1!=sign2)  /* set zero crossing location */
			zerCross[i+1] = USHRT_MAX;
	}
}



unsigned short *DoG(unsigned short *image_data, int width, int height, int channels, float sigma, float sigma2)
{
    int length = width*height*channels;
    unsigned short *low_sigma = new unsigned short[length];
    unsigned short *high_sigma = new unsigned short[length];
    unsigned short *zero_crossing = new unsigned short[length] ();
    int *result = new int[width * height];

    std::copy(&image_data[0], &image_data[length - 1], &low_sigma[0]);
    std::copy(&image_data[0], &image_data[length - 1], &high_sigma[0]);

    gaus_blur(low_sigma, width, height, channels, sigma);
    gaus_blur(high_sigma, width, height, channels, sigma2);

    for (size_t i = 0; i < width * height; i++)
    {
        int value = (int)low_sigma[i*channels] - (int)high_sigma[i*channels];
        result[i] = value;
    }
    zeroCrossing(result, zero_crossing, width * height);

    // std::copy(&zero_crossing[0], &zero_crossing[length - 1], &image_data[0]);
    for (size_t i = 0; i < width * height; i++)
        for (size_t j = 0; j < channels; j++)
            image_data[i * channels + j] = (unsigned short)zero_crossing[i];
    
    delete [] zero_crossing;
    delete [] result;
    delete [] low_sigma;
    delete [] high_sigma;
}

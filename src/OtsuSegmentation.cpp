#include <iostream>
#include <climits>

#define MAX_INTENSITY USHRT_MAX


void computeHistogram(unsigned short *input, unsigned *hist, int width, int height)
{
	// Compute number of pixels
	long int N = width * height;
	int i = 0;

	// Initialize array
	for (i = 0; i <= MAX_INTENSITY; i++)
		hist[i] = 0;
	for (i = 0; i < N; i++)
	{
		int value = (int)input[i];
		hist[value]++;
	}
}

/*! Segments the image using the computed threshold
    \param input Input image
    \param output Output segmented image
    \param threshold Threshold used for segmentation
*/
void segmentImage(unsigned short *input, unsigned short *output, int width, int height, int threshold)
{
	// Compute number of pixels
	long int N = width * height;
	// Modify output image
	// Use first channel
	// Iterate image
	for (int i = 0; i < N; i++)
	{
		int value = (int)input[i];
		// Build the segmented image
		if (value > threshold)
		{
			output[i] = MAX_INTENSITY;
		}
		else
		{
			output[i] = 0;
		}
	}
}

/*! Computes Otsus segmentation
    \param input Input image
    \param hist Image histogram
    \param output Output segmented image
    \param overrided_threshold Input param that overrides threshold
*/
void computeOtsusSegmentation(unsigned short *input, unsigned short *output, int width, int height, int overrided_threshold)
{
	unsigned hist[MAX_INTENSITY + 1];
	computeHistogram(input, hist, width, height);
	// Compute number of pixels
	long int N = width * height;
	int threshold = 0;

	if (overrided_threshold != 0)
	{
		// If threshold was manually entered
		threshold = overrided_threshold;
	}
	else
	{
		// Compute threshold
		// Init variables
		float sum = 0;
		float sumB = 0;
		int q1 = 0;
		int q2 = 0;
		float varMax = 0;

		// Auxiliary value for computing m2
		for (int i = 0; i <= MAX_INTENSITY; i++)
		{
			sum += i * ((int)hist[i]);
		}

		for (int i = 0; i <= MAX_INTENSITY; i++)
		{
			// Update q1
			q1 += hist[i];
			if (q1 == 0)
				continue;
			// Update q2
			q2 = N - q1;

			if (q2 == 0)
				break;
			// Update m1 and m2
			sumB += (float)(i * ((int)hist[i]));
			float m1 = sumB / q1;
			float m2 = (sum - sumB) / q2;

			// Update the between class variance
			float varBetween = (float)q1 * (float)q2 * (m1 - m2) * (m1 - m2);

			// Update the threshold if necessary
			if (varBetween > varMax)
			{
				varMax = varBetween;
				threshold = i;
			}
			// std::cout << "Threshold: " << threshold << std::endl;
		}
	}

	// Perform the segmentation
	segmentImage(input, output, width, height, threshold);
}
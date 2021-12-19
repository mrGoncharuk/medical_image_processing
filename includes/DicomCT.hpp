#pragma once

#include <climits>
#include <imebra/imebra.h>
#include <map>
#include <string>
#include <iostream>
#include <experimental/filesystem>
#include "Graphics.hpp"


class DicomCT
{
private:
	int w, h, l;	// width, heigth, length of Computer Tomography images
	int pixelSize;
	double pixelSpacing[2];
	double sliceThickness, spaceBetweenSlices;
	bool isBytesSigned;
	char **imageSet;
	imebra::bitDepth_t depth;

public:
	DicomCT();
	DicomCT(const std::string &DICOM_set_path);
	~DicomCT();

	void	readImageSet(const std::string &DICOM_set_path);
	void	readDicomSet(const std::string &DICOM_set_path);
	int		normalizeImage(char *image, int w, int h, int bytesStored, bool isBytesSigned);
	void	setTextureParameters();
	const std::map<std::string, int> &getParams() const;
	char **getImageSet();
	std::map<std::string, int> params;
	double getPixelSpacingX() const;
	double getPixelSpacingY() const;
};

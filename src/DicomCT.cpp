#include "DicomCT.hpp"
#include <vector>
#include <algorithm>

DicomCT::DicomCT::DicomCT(/* args */): w(0), h(0), l(0), imageSet(NULL)
{
}

DicomCT::DicomCT::~DicomCT()
{
	if (this->imageSet)
	{
		for (int i = 0; i < params["time"] * params["length"]; ++i)
			delete[] this->imageSet[i];
		delete[] this->imageSet;
	}
}

void		DicomCT::readDicomSet(const std::string &set_path)
{
	using namespace imebra;
	using namespace std;

	DataSet 	loadedDataSet(CodecFactory::load(std::experimental::filesystem::directory_iterator(set_path)->path()));
	Image		image(loadedDataSet.getImage(0));
	this->w = image.getWidth();
	this->h = image.getHeight();
	if (loadedDataSet.getString(TagId(0x0028, 0x1052), 0) != "0"
		&& loadedDataSet.getString(TagId(0x0028, 0x1053), 0) != "1")
	{
		std::cout << "Processing image with pixels in float is not yet implemented. Exiting." << std::endl;
		exit(1);
	}

	ReadingDataHandlerNumeric dataHandler = image.getReadingDataHandler();
	this->sliceThickness = loadedDataSet.getDouble(TagId(0x0018, 0x050), 0);
	this->spaceBetweenSlices = loadedDataSet.getDouble(TagId(0x0018, 0x088), 0);
	double intercept = loadedDataSet.getDouble(TagId(0x028, 0x1052), 0);
	double slope = loadedDataSet.getDouble(TagId(0x028, 0x1053), 0);
	// int channelLength = loadedDataSet.getDouble(TagId(0x300A, 0x0284), 0);
	this->pixelSpacing[0] = loadedDataSet.getDouble(TagId(0x028, 0x030), 0) ;
	this->pixelSpacing[1] = loadedDataSet.getDouble(TagId(0x028, 0x030), 1);
	this->pixelSize = dataHandler.getUnitSize();
	this->isBytesSigned = dataHandler.isSigned();
	this->depth = image.getDepth();
	params["time"] = loadedDataSet.getDouble(TagId(0x0020, 0x0105), 0);
	params["thickness"] = ceil(this->sliceThickness / this->pixelSpacing[0]);
	params["TE"] = loadedDataSet.getDouble(TagId(0x0018, 0x0081), 0);
	params["repetitionTime"] = loadedDataSet.getDouble(TagId(0x0018, 0x0080), 0);
	cout << "sliceThickness: " << this->sliceThickness << endl; 
	cout << "spaceBetweenSlices: " << this->spaceBetweenSlices << endl; 
	cout << "Thickness: " << params["thickness"] << endl;
	cout << "width: " << this->w << endl; 
	cout << "height: " << this->h << endl; 
	cout << "isBytesSigned: " << this->isBytesSigned << endl; 
	cout << "Pixel size: " << this->pixelSize << endl; 
	cout << "Number of color channels:  " << image.getChannelsNumber() << endl;
	cout << "Depth: " << (int)this->depth << endl;
	cout << "Number of Temporal Positions Attribute: " << params["time"] << endl;
	cout << "Echo time: " << params["TE"] << endl;
	cout << "Trigger time: " << loadedDataSet.getDouble(TagId(0x0018, 0x1060), 0) << '\t';
	cout << "Repetition Time: " << params["repetitionTime"] << '\t';
	cout << "Instance number: " << loadedDataSet.getDouble(TagId(0x0020, 0x0013), 0) << endl;
	params["width"] = this->w;
	params["heigth"] = this->h;
	this->readImageSet(set_path);
	this->setTextureParameters();
}

void		DicomCT::readImageSet(const std::string &set_path)
{
	using namespace std;
	using namespace imebra;
	vector<std::string> files;
	
    for (const auto & entry : std::experimental::filesystem::directory_iterator(set_path))
		files.push_back(entry.path());
	sort(files.begin(), files.end());
	// for (auto i = files.begin(); i < files.end(); ++i)
    //     std::cout << *i << std::endl;
	this->l = files.size() / params["time"];
	this->params["length"] = this->l;
	this->imageSet = new char *[params["time"] * params["length"]];
	for (size_t i = 0; i < params["time"] * params["length"]; ++i)
	{
		imebra::DataSet 	loadedDataSet(imebra::CodecFactory::load(files[i]));
		imebra::Image		image(loadedDataSet.getImage(0));
		uint32_t 			data_size = image.getWidth() * image.getHeight() * image.getReadingDataHandler().getUnitSize();	// *channels/*pixelsize
		imageSet[i] = new char[data_size];
		image.getReadingDataHandler().data(imageSet[i], data_size);
	}
}


// TODO: adjust to all propper formats
void DicomCT::setTextureParameters()
{
	switch (this->depth)
	{
		case imebra::bitDepth_t::depthU8 :
			this->params["internalFormat"] = GL_R8;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_UNSIGNED_BYTE;
			break;
		case imebra::bitDepth_t::depthS8 :
			this->params["internalFormat"] = GL_R8;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_BYTE;
			break;
		case imebra::bitDepth_t::depthU16 :
			this->params["internalFormat"] = GL_R16;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_UNSIGNED_SHORT;
			break;
		case imebra::bitDepth_t::depthS16 :
			this->params["internalFormat"] = GL_RED;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_SHORT;
			break;
		case imebra::bitDepth_t::depthU32 :
			this->params["internalFormat"] = GL_R32F;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_FLOAT;
			break;
		case imebra::bitDepth_t::depthS32 :
			this->params["internalFormat"] = GL_R32F;
			this->params["format"] = GL_RED;
			this->params["type"] = GL_FLOAT;
			break;
		default:
			break;
	}
}

const std::map<std::string, int> &DicomCT::getParams() const
{
	return this->params;
}


char **DicomCT::getImageSet()
{
	return this->imageSet;
}

double DicomCT::getPixelSpacingX() const
{
	return this->pixelSpacing[0];
}

double DicomCT::getPixelSpacingY() const
{
	return this->pixelSpacing[1];
}


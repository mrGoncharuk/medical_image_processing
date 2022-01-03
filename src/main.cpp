#include "MIA.hpp"

int main()
{

	MIA game("Medical Image Analysis",
		800, 800,
		false);

	//MAIN LOOP
	while (!game.getWindowShouldClose())
	{
		//UPDATE INPUT ---
		game.update();
		game.render();	
	}

	return 0;
}



// int mainly() 
// {
// 	const int w = 6;
// 	const int h = 5;
// 	const int l = 3;
// 	const int t = 2;
// 	const int thickness = 3;
// 	int array[l*t][w * h] =  {
// 		{	1, 1, 1, 1, 1, 1,
// 			2, 2, 2, 2, 2, 2,
// 			3, 3, 3, 3, 3, 3,
// 			4, 4, 4, 4, 4, 4,
// 			5, 5, 5, 5, 5, 5},

// 		{	6, 6, 6, 6, 6, 6,
// 			7, 7, 7, 7, 7, 7,
// 			8, 8, 8, 8, 8, 8,
// 			9, 9, 9, 9, 9, 9,
// 			10, 10, 10, 10, 10, 10},

// 		{	11, 11, 11, 11, 11, 11,
// 			12, 12, 12, 12, 12, 12,
// 			13, 13, 13, 13, 13, 13,
// 			14, 14, 14, 14, 14, 14,
// 			15, 15, 15, 15, 15, 15},

// 		{	1, 1, 1, 1, 1, 1,
// 			2, 2, 2, 2, 2, 2,
// 			3, 3, 3, 3, 3, 3,
// 			4, 4, 4, 4, 4, 4,
// 			5, 5, 5, 5, 5, 5},

// 		{	6, 6, 6, 6, 6, 6,
// 			7, 7, 7, 7, 7, 7,
// 			8, 8, 8, 8, 8, 8,
// 			9, 9, 9, 9, 9, 9,
// 			10, 10, 10, 10, 10, 10},

// 		{	11, 11, 11, 11, 11, 11,
// 			12, 12, 12, 12, 12, 12,
// 			13, 13, 13, 13, 13, 13,
// 			14, 14, 14, 14, 14, 14,
// 			15, 15, 15, 15, 15, 15},
// 							};


// 	vector<vector<Mat>> transvers;
// 	for (size_t frame = 0; frame < t; ++frame)
// 	{
// 		vector<Mat> slice_set;
// 		for (size_t slice = 0; slice < l; ++slice)
// 			slice_set.push_back(Mat(h, w, CV_32S, array[slice*t + frame]));
// 		transvers.push_back(slice_set);
// 	}
// 	cout << "############ TRANSVERSE #############" << endl;
// 	for (size_t frame = 0; frame<t; ++frame)
// 	{
// 		for (size_t slice = 0; slice < l; ++slice)
// 			cout << transvers[frame][slice] << endl;
// 		cout << endl;
// 	}
// 	cout << endl;


// 	vector<vector<Mat>> sagittal;
// 	for (size_t frame = 0; frame < t; ++frame)
// 	{
// 		vector<Mat> slice_set;

// 		for (size_t col = 0; col < w; ++col)
// 		{
// 			Mat result;
// 			for (size_t i = 0; i < l; ++i)
// 				for (size_t s = 0; s < thickness; s++)
// 					result.push_back(transvers[frame][i].col(col));
// 			result = result.reshape(0, l*thickness);
// 			slice_set.push_back(result);
// 		}
// 		sagittal.push_back(slice_set);
// 	}
// 	cout << "############ Sagittal #############" << endl;
// 	for (size_t frame = 0; frame<t; ++frame)
// 	{
// 		for (size_t slice = 0; slice < w; ++slice)
// 			cout << sagittal[frame][slice] << endl;
// 		cout << endl;
// 	}
// 	cout << endl;


// 	vector<vector<Mat>> frontal;
// 	for (size_t frame = 0; frame < t; ++frame)
// 	{
// 		vector<Mat> slice_set;

// 		for (size_t row = 0; row < h; ++row)
// 		{
// 			Mat result;
// 			for (size_t i = 0; i < l; ++i)
// 				for (size_t s = 0; s < thickness; s++)
// 					result.push_back(transvers[frame][i].row(row));
// 			result = result.reshape(0, l*thickness);
// 			slice_set.push_back(result);
// 		}
// 		frontal.push_back(slice_set);
// 	}
// 	cout << "############ Frontal #############" << endl;
// 	for (size_t frame = 0; frame<t; ++frame)
// 	{
// 		for (size_t slice = 0; slice < h; ++slice)
// 			cout << frontal[frame][slice] << endl;
// 		cout << endl;
// 	}
// 	cout << endl;

// }



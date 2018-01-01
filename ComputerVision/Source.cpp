#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;


// Methods
char getSelectionFromUser();
void createAndShowHybridImage();
void handleHybrid();
void handleZoom();


// Global variables
const int PYR_LEVEL = 8;
char selection;
String windowHybridImg = "Hybrid image (result)";

Mat sourceImg1, sourceImg2, hybridImg, zoomImg;
Mat img1Gaussian[PYR_LEVEL];
Mat img1Laplacian[PYR_LEVEL - 1];
Mat img2Gaussian[PYR_LEVEL];
Mat img2Laplacian[PYR_LEVEL - 1];

int hybridTrackbarValue = 0;
int zoomTrackbarValue;
const int MAX_ZOOM = min(PYR_LEVEL, 4);


int main(int argc, char** argv)
{
	// Variables
	String windowImg1 = "Image 1";
	String windowImg2 = "Image 2";

	// Load images and check if they exist
	sourceImg1 = imread("data/fish.bmp");
	sourceImg2 = imread("data/submarine.bmp");

	if (sourceImg1.empty() || sourceImg2.empty())
	{
		printf("Error while loading image/s \n");
		return -1;
	}

	// Makes the images on the same size if needed
	if (sourceImg1.size < sourceImg2.size)
		resize(sourceImg2, sourceImg2, sourceImg1.size());
	else if (sourceImg2.size < sourceImg1.size)
		resize(sourceImg1, sourceImg1, sourceImg2.size());

	// Clone source images into gaussian arrays
	img1Gaussian[0] = sourceImg1.clone();
	img2Gaussian[0] = sourceImg2.clone();

	// Create the Gaussian and Laplacian pyramids (arrays)
	for (int i = 0; i < PYR_LEVEL - 1; i++)
	{
		// Image 1 arrays.
		pyrDown(img1Gaussian[i], img1Gaussian[i + 1]);
		pyrUp(img1Gaussian[i + 1], img1Gaussian[i + 1], img1Gaussian[i].size());
		img1Laplacian[i] = img1Gaussian[i] - img1Gaussian[i + 1];

		// Image 2 arrays.
		pyrDown(img2Gaussian[i], img2Gaussian[i + 1]);
		pyrUp(img2Gaussian[i + 1], img2Gaussian[i + 1], img2Gaussian[i].size());
		img2Laplacian[i] = img2Gaussian[i] - img2Gaussian[i + 1];
	}

	// Get selection from user
	selection = getSelectionFromUser();

	// Create windows for source images and show them
	// imshow auto create default window with WINDOW_AUTOSIZE parameter if does not exist.  
	imshow(windowImg1, sourceImg1);
	imshow(windowImg2, sourceImg2);

	// Prepare the hybrid (result) window for user interaction.
	createAndShowHybridImage();

	waitKey();
	return 0;

} // End main


char getSelectionFromUser() {

	cout << "######################################################\n";
	cout << "#     Hybrid Image Program   -   Tal & Gal Zemah     #\n";
	cout << "######################################################\n\n";

	selection = ' ';

	do
	{
		cout << "Please select one option from the menu:\n\n";
		cout << "1) The first image is closer\n";
		cout << "2) The second image is closer\n";
		cin >> selection;
	} while (selection != '1' && selection != '2');

	return selection;
}

void createAndShowHybridImage() {

	// Show the selected image (original without hybrid)
	hybridImg = selection == '1' ? sourceImg1.clone() : sourceImg2.clone();
	imshow(windowHybridImg, hybridImg);

	// Create Hybrid and zoom trackbar to hybrid window
	createTrackbar("Hybrid:", windowHybridImg, &hybridTrackbarValue, PYR_LEVEL - 1, TrackbarCallback(handleHybrid));
	createTrackbar("Zoom:", windowHybridImg, &zoomTrackbarValue, MAX_ZOOM, TrackbarCallback(handleZoom));
}

// Invoke each time when user changes the hybrid value 
void handleHybrid() {

	// Choose the correct image (Gaussian) according to user selection
	hybridImg = selection == '1' ? img1Gaussian[hybridTrackbarValue].clone() : img2Gaussian[hybridTrackbarValue].clone();

	// Add the Laplasians Image/s according to user selection
	for (int i = 0; i < hybridTrackbarValue; i++) {
		hybridImg += selection == '1' ? img2Laplacian[i] : img1Laplacian[i];
	}

	// In order to save the zoom level before changes the hybrid level
	handleZoom();
}

// Invoke each time when user changes the zoom value
void handleZoom() {

	zoomImg = hybridImg.clone();

	for (int i = 0; i < zoomTrackbarValue; i++)
	{
		pyrDown(zoomImg, zoomImg);
	}

	imshow(windowHybridImg, zoomImg);
	resizeWindow(windowHybridImg, hybridImg.size());
}
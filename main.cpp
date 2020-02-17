#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <typeinfo>




using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    VideoCapture cap(0); // open the video camera no. 0
    //cap.set(CV_CAP_PROP_FPS,10);

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    //double fps = cap.get(CV_CAP_PROP_FPS);
    cout << "Frame size : " << dWidth << " x " << dHeight <<endl;

    int iLowH = 0;
    int iHighH = 255;
    int iLowS = 0;
    int iHighS = 255;
    int iLowV = 0;
    int iHighV = 255;
    int edgeThresh = 1;


    //threshhold control for color detection
    namedWindow("color threshold Control",CV_WINDOW_AUTOSIZE); //create a window called "Control"
    cvCreateTrackbar("LowH", "color threshold Control", &iLowH, 255);
    cvCreateTrackbar("HighH", "color threshold Control", &iHighH, 255);

    cvCreateTrackbar("LowS", "color threshold Control", &iLowS, 255);
    cvCreateTrackbar("HighS", "color threshold Control", &iHighS, 255);

    cvCreateTrackbar("LowV", "color threshold Control", &iLowV, 255);
    cvCreateTrackbar("HighV", "color threshold Control", &iHighV, 255);

    //edge detection control
    namedWindow("edge threshold control", CV_WINDOW_AUTOSIZE);
    createTrackbar("threshold", "edge threshold control", &edgeThresh, 100);

    Mat video;
    Mat video_HSV;
    Mat video_grey;
    Mat video_edge;
    Mat Vthreshold;


   while(waitKey(10) != 27)
   {
       bool bSuccess = cap.read(video); // read from camera

         if (!bSuccess) //if not success, break loop
          {
             cout << "Cannot read from camera" << endl;
             break;
          }

          //capture and covert to gray
          cap.read(video);
          GaussianBlur( video, video, Size(9, 9), 2, 2 );//blur the image
          cvtColor(video, video_HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
          cvtColor(video, video_grey, CV_RGB2GRAY );// to gray
          //threshold color to isolate desired ball color
          inRange(video_HSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), Vthreshold); //Threshold the image
          dilate( Vthreshold, Vthreshold, getStructuringElement(MORPH_RECT, Size(3, 3)) ); //dilate the image to get rid of holes
          //get the edge of the ball
          Canny(Vthreshold, video_edge, edgeThresh, edgeThresh*3, 3);
          //detect circle and track the ball
          vector<Vec3f> circles;

          // Apply the Hough Transform to find the circles
          HoughCircles( video_edge, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0 );

          // Draw the circles detected
          for( size_t i = 0; i < circles.size(); i++ )
          {
              Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
              int radius = cvRound(circles[i][2]);
              // circle center
              circle( video, center, 3, Scalar(0,255,0), -1, 8, 0 );
              // circle outline
              circle( video, center, radius, Scalar(0,0,255), 3, 8, 0 );
          }

          imshow("Thresholded Image", Vthreshold); //show the thresholded image
          imshow("Edge",video_edge);
          imshow("ball tracking", video); //show the original image

    }

    return 0;

}

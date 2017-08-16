#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "GameLogic.cpp" 

#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

void drawRectangles(Mat img);
void getSamples(Mat img);
Mat backProject(Mat img);
Mat getHistogram(Mat image);
void DetectContour(Mat img);

//Histogram Calculation Parameters
static const int h_bins = 30;
static const int s_bins = 32;

//Skin Detection constants
static const int channels[] = {0, 1};    // HUE & SATURATION
static const float h_range[] = {0, 179};
static const float s_range[] = {50, 255};
static const float *ranges[] = {h_range, s_range};

//Boolean variable: is there any skin color data  
bool sampled;
// Coordinates of the palm
float x, y;
Mat gameWindow, frame, frameHSV, ROI;
//Sub images for each of the sample region
Mat sub0, sub1, sub2, sub3, sub4; 
GameLogic gl;

int main( int argc, const char** argv ){
    
    sampled = false;

    //Start capture from the webcam
    CvCapture* capture;
    capture = cvCaptureFromCAM( -1 );

    if( capture ){
            
        while( true ){
            //Get current frame
            frame = cvQueryFrame( capture );

            if( !frame.empty() ){
                //Resize the webcam capture to be the same size as the game window 
                resize(frame,frame, Size(gl.WIN_WIDTH,gl.WIN_HEIGHT));

                //Wipe the gameWindows with the color black
                gameWindow = Mat::zeros( Size(gl.WIN_WIDTH, gl.WIN_HEIGHT) , CV_8UC3 );

                //Flip the frame and get HSV frame to be used in segmentation
                flip(frame,frame,1);
                cvtColor(frame, frameHSV, COLOR_BGR2HSV);

                drawRectangles(frame);

                //If there is color data track the hand and advance the game
                if(sampled == true){
                    ROI = backProject(frameHSV);		
                    DetectContour(ROI);
        
                    //Render the Game
                    gl.update(x,y);
                    rectangle(gameWindow, Point(gl.ballX,gl.ballY),Point(gl.ballX+gl.BALL_RADIUS
                        ,gl.ballY+gl.BALL_RADIUS),Scalar(255,255,255),CV_FILLED);

                    Scalar color;

                    for(int i=0; i < 40;i++){
                        //Diferent Color for each line of bricks
                        if(i/10==0){color =  Scalar(72,72,200);}
                        else if(i/10==1){color =  Scalar(58,108,198);}
                        else if(i/10==2){color =  Scalar(42,160,162);}
                        else if(i/10==3){color =  Scalar(72,160,72);}

                        //If brick is not destroyed draw it
                        if(gl.bricks[i]){
                            rectangle(gameWindow, Point( (i%10)*gl.BRICK_WIDTH, (i/10)*gl.BRICK_HEIGHT ),
                                Point( ((i%10)*gl.BRICK_WIDTH)+gl.BRICK_WIDTH, ((i/10)*gl.BRICK_HEIGHT)+gl.BRICK_HEIGHT), color,CV_FILLED);  
                        }
                          
                    }

                    //Draw the paddle
                    rectangle(gameWindow,Point(gl.paddleX-gl.PADDLE_WIDTH,gl.paddleY-gl.PADDLE_HEIGHT),
                        Point(gl.paddleX+gl.PADDLE_WIDTH, gl.paddleY+gl.PADDLE_HEIGHT),Scalar(255,255,255),CV_FILLED);

                    //Print the score on the upper right corner of the game screen
                    std::ostringstream ss;
                    ss << gl.score;

                    putText(gameWindow,"Score: "+ss.str(), cvPoint(10,20), 
                        FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);

                    //Print the remaining number of lives on the upper left corner of the game screen
                    std::ostringstream ll;
                    ll << gl.lives;

                    putText(gameWindow,"Lives: "+ll.str(), cvPoint(560,20), 
                        FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);       

                    //Draw Game Over Message
                    if(gl.GameState==gl.GAMEOVER){
                        putText(gameWindow,"GAMEOVER!", cvPoint(220,250), 
                            FONT_HERSHEY_COMPLEX_SMALL, 1.8, cvScalar(255,255,255), 1, CV_AA);
                    }
                    //Draw Victory Message
                    else if(gl.GameState==gl.WIN){
                        putText(gameWindow,"YOU WIN!!", cvPoint(220,250), 
                            FONT_HERSHEY_COMPLEX_SMALL, 1.8, cvScalar(255,255,255), 1, CV_AA);
                    }
                    //Draw Pause Message
                    else if(gl.GameState==gl.PAUSE){
                        putText(gameWindow,"PAUSED", cvPoint(235,250), 
                            FONT_HERSHEY_COMPLEX_SMALL, 1.8, cvScalar(255,255,255), 1, CV_AA);
                    }

                }	

                namedWindow("Capture",CV_WINDOW_AUTOSIZE);
                namedWindow("GameWindow",CV_WINDOW_AUTOSIZE);

                imshow("Capture", frame);
                imshow("GameWindow",gameWindow);  			 
            }
            else{ 
                printf("ERROR: No captured frame"); 
                break; 
            }
            int c = waitKey(15);
            //If "SPACE" is pressed sample colors
            if( c == 32){
                getSamples(frameHSV);
                sampled = true;
            }
            //If "ENTER" is pressed exit pause or restart
            else if( c == 13 ){
                if(gl.GameState == gl.PAUSE){
                    gl.GameState=gl.RUNNING;
                }
                else{gl.restart();}
            }
            //If "ESC" is pressed pause the game or quit
            else if( c == 27) {
                if(gl.GameState != gl.PAUSE){
                gl.GameState=gl.PAUSE;
                }
                else{break;}
            }
        }
    }
    return 0;
}

//Draw Rectangles on the captured frame corresponding to the sample areas
void drawRectangles(Mat img){
    rectangle(img,Point(385,240),Point(395,250),Scalar(255,0,0));
    rectangle(img,Point(320,280),Point(330,290),Scalar(255,0,0));
    rectangle(img,Point(350,310),Point(360,320),Scalar(255,0,0));	
    rectangle(img,Point(320,220),Point(330,230),Scalar(255,0,0));
    rectangle(img,Point(350,180),Point(360,190),Scalar(255,0,0));		    	
}

//Obtain an save a normalized histogram for each of the 4 regions
void getSamples(Mat img){
    sub0 = getHistogram(img(Rect(385,240,10,10)));
    normalize(sub0, sub0, 0, 255, NORM_MINMAX, -1, Mat());

    sub1 = getHistogram(img(Rect(320,280,10,10)));
    normalize(sub1, sub1, 0, 255, NORM_MINMAX, -1, Mat());

    sub2 = getHistogram(img(Rect(350,310,10,10)));
    normalize(sub2, sub2, 0, 255, NORM_MINMAX, -1, Mat());

    sub3 = getHistogram(img(Rect(320,220,10,10)));
    normalize(sub3, sub3, 0, 255, NORM_MINMAX, -1, Mat());

    sub4 = getHistogram(img(Rect(350,180,10,10)));
    normalize(sub4, sub4, 0, 255, NORM_MINMAX, -1, Mat());  
}

Mat backProject(Mat img){
    Mat p0,p1,p2,p3,p4,result;

    //Calculate teh Back Projection for the image with each of the 4 histograms
    calcBackProject(&img, 1, channels, sub0, p0, ranges, 1, true);
    calcBackProject(&img, 1, channels, sub1, p1, ranges, 1, true);
    calcBackProject(&img, 1, channels, sub2, p2, ranges, 1, true);
    calcBackProject(&img, 1, channels, sub3, p3, ranges, 1, true);
    calcBackProject(&img, 1, channels, sub4, p4, ranges, 1, true);

    //Add the obtained images for each sample region 
    result = p0+p1+p2+p3+p4;

    //Blur and Threshold the result
    GaussianBlur(result, result, Size(5, 5),0);
    threshold(result, result, 0,255,THRESH_BINARY+THRESH_OTSU);	   
    medianBlur(result,result,5);	

    return result;
}

//Calcute Histogram of an image 
Mat getHistogram(Mat image){
    Mat hist;

    int histSize[] = {h_bins, s_bins};
    cv::calcHist(&image, 1, channels, Mat(), hist, 2, histSize, ranges);

    return hist;
}

void DetectContour(Mat img){
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //Find the countors of the image
    findContours(img,contours, hierarchy, RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, Point());

    if(contours.size()>0){
        vector<vector<Point> > hullsP(contours.size());
        
        //Find big countors draw them and save the coordinates of its center
        for( int i = 0; i < contours.size(); i++ ){
            if(contourArea(contours[i])>5000){
                
                convexHull(contours[i], hullsP[i], false, true);
                
                Moments mon = moments(contours[i]);
                x = mon.m10/mon.m00;
                y = mon.m01/mon.m00;
                circle(frame,Point(x,y),25,Scalar(0,255,255),-1);    
            }
        }

        //Draw the contors on the captured frame
        for( int i = 0; i< contours.size(); i++ ){
            drawContours( frame, hullsP, i, Scalar( 0, 255, 0 ), 1, 8, vector<Vec4i>(), 0, Point() );
        }
    }            
}

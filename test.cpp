#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SerialPort.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;


// Global Variables :/


// Serial Port stuff
const char* portName = "COM3";
#define MAX_DATA_LENGTH 255
char incomingData[MAX_DATA_LENGTH];
SerialPort* arduino;

// character buffer
char dataBuffer[2*MAX_DATA_LENGTH];
int charIndex = 0;
char charToDetectGlobal = 'X';
char dataToOutput[MAX_DATA_LENGTH];

// led on/off for serialWrite
const char ledON[] = "ON\n";
const char ledOFF[] = "OFF\n";
const unsigned int BLINKING_DELAY = 1000;
// If you want to send data then define "SEND" else comment it out
//#define SEND

int inputX = 127;
int inputY = 127;
char inputColor = 'Y';


void inputToInt() {
    string inputString(dataToOutput);

    string xCoor, yCoor, remover;

    istringstream iss(inputString);
    getline(iss, remover, '=');
    getline(iss, xCoor, ',');
    getline(iss, remover, '=');
    getline(iss, yCoor, ',');

    inputX = stoi(xCoor);
    inputY = stoi(yCoor);

   /* cout << "int_1: " << inputX << endl;
    cout << "int_2: " << inputY << endl;*/
}


int countNumChar() {
    int count = 0;
    char charToDetect = charToDetectGlobal;

    for (int i = 0; i < sizeof(dataBuffer); i++) {
        if (dataBuffer[i] == NULL) break;
        if (dataBuffer[i] == charToDetect) count++;
    }
    return count;
}

void findLastWholeData(int countOfChar) {
    char charToDetect = charToDetectGlobal;
    int count = 0;
    int indexOfSecondLastChar = 0;
    int indexOfLastChar = 0;

    for (int i = 0; i < sizeof(dataBuffer); i++) {
        if (dataBuffer[i] == charToDetect) {
            indexOfLastChar = i;
            count++;

            if (count == countOfChar) {
                /*printf("indexLast: %d", indexOfLastChar);
                printf("indexSecLast: %d", indexOfSecondLastChar);*/
                break;
            }
            else {
                indexOfSecondLastChar = i;
            }
        }
    }

    int arrIndex = 0;
    for (int i = indexOfSecondLastChar; i < indexOfLastChar; i++) {
        dataToOutput[arrIndex] = dataBuffer[i];
        arrIndex++;
    }

    //dump every other data because its just faster I think and easier code
    for (int i = 0; i < sizeof(dataBuffer); i++) {
        dataBuffer[i] = NULL;
    }
    charIndex = 0;
}


// serial connection functions
void exampleReceiveData() {
    int readResult = arduino->readSerialPort(incomingData, MAX_DATA_LENGTH);
    //cout << incomingData[1] << endl;

    for (int i = 0; i < MAX_DATA_LENGTH; i++) {
        //cout << i;
        if (incomingData[i] == NULL) { break; }
        else {
            dataBuffer[charIndex] = incomingData[i];
            charIndex++;
        }
    }
    if (countNumChar() > 1) {
        findLastWholeData(countNumChar());
    }


    if (dataToOutput[0] != NULL) { 
        printf("%s", dataToOutput); // print data here
        if (dataToOutput[1] == '=') {
            // parse the numbers into the variables
            inputToInt();
        }
        else {
            // change the filter variable. We have Red, Blue, Green, Cyan, Yellow
            inputColor = dataToOutput[1];
        }
        for (int i = 0; i < sizeof(dataToOutput); i++) {
            dataToOutput[i] = NULL;
        }
    }

    Sleep(10);
}

void exampleWriteData(unsigned int delayTime) {
    arduino->writeSerialPort(ledON, MAX_DATA_LENGTH);
    Sleep(delayTime);
    arduino->writeSerialPort(ledOFF, MAX_DATA_LENGTH);
    Sleep(delayTime);
}

void autoConnect() {
    // better than recursion
    // avoid stack overflows
    //while (1) {
    // 
    // ui - searching
    //cout << "Searching in progress";
    while (!arduino->isConnected()) {
        Sleep(100);
        cout << ".";
        arduino = new SerialPort(portName);
    }

   /* if (arduino->isConnected()) {
        cout << endl << "Connection established at port " << portName << endl;
    }*/

    #ifdef SEND
        if (arduino->isConnected()) exampleWriteData(BLINKING_DELAY);
    #else
        if (arduino->isConnected()) exampleReceiveData();
    #endif

    //}
}

/////////////////  Images  //////////////////////

//Mat brightness(Mat img) {     
//
//    Mat hsv;
//    cvtColor(img, hsv, COLOR_BGR2HSV);         
//    float val1 = inputX/255.0*1.5;
//    float val2 = inputY/255.0;
//    
//    Mat channels[3];
//
//    split(hsv,channels);         
//    Mat H = channels[0];
//
//    H.convertTo(H,CV_32F);         
//    Mat S = channels[1];
//
//    S.convertTo(S,CV_32F);         
//    Mat V = channels[2];
//
//    V.convertTo(V,CV_32F);         
//    for (int i=0; i < H.size().height; i++){             
//        for (int j=0; j < H.size().width; j++){     // scale pixel values up or down for channel 1(Saturation)                 
//            S.at<float>(i,j) *= val1;                 
//            if (S.at<float>(i,j) > 255)                     
//                S.at<float>(i,j) = 255;     // scale pixel values up or down for channel 2(Value)                 
//            V.at<float>(i,j) *= val2;                 
//            if (V.at<float>(i,j) > 255)                     
//                V.at<float>(i,j) = 255;             
//        }         
//    }         
//    H.convertTo(H,CV_8U);         
//    S.convertTo(S,CV_8U);         
//    V.convertTo(V,CV_8U);         
//    vector<Mat> hsvChannels{H,S,V};         
//    Mat hsvNew;         
//    merge(hsvChannels,hsvNew);         
//    Mat res;         
//    cvtColor(hsvNew,res,COLOR_HSV2BGR);         
//               
//    return res;
//
//}


Mat brightness2(Mat img) {

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    float val1 = (float)inputX / 255.0 * 2.0;
    float val2 = (float)inputY / 255.0;
    
    for (int j = 0; j < img.rows; j++) {
        for (int i = 0; i < img.cols; i++) {
            unsigned char s = hsv.at<Vec3b>(j, i)[1];

            s = (s * val1);
            if (s > 255) s = 255;

            hsv.at<Vec3b>(j, i)[1] = s;


            unsigned char v = hsv.at<Vec3b>(j, i)[2];

            v = (v * val2);
            if (s > 255) s = 255;

            hsv.at<Vec3b>(j, i)[2] = v;
        }
    }

    Mat res;
    cvtColor(hsv, res, COLOR_HSV2BGR);
    return res;
}


Mat kaleidoscope(Mat img) {

    Mat img3(480, 480, CV_8UC3, Scalar(100, 100, 100));
    Mat imgCrop;

    int windowWidth = 480;

    Rect roi(80, 0, windowWidth, windowWidth);
    imgCrop = img(roi);


    int iMax = windowWidth / 2;

    for (int i = 0; i <= iMax; i++) {
        for (int j = 0; j < i; j++) {
            for (int k = 0; k < 3; k++) {
                uchar pointInfo = imgCrop.at<uchar>(Point(i * 3 + k, j));
                img3.at<uchar>(Point(i * 3 + k, j)) = pointInfo;
                img3.at<uchar>(Point(windowWidth * 3 - i*3 +k, j)) = pointInfo;
                img3.at<uchar>(Point(i*3+k, windowWidth-1 - j)) = pointInfo;
                img3.at<uchar>(Point(windowWidth * 3 - i*3 + k, windowWidth - 1 - j)) = pointInfo;
    
                img3.at<uchar>(Point(j*3 + k, i-1)) = pointInfo;
                img3.at<uchar>(Point(windowWidth * 3 - (j+1) * 3 + k, i-1)) = pointInfo;
                img3.at<uchar>(Point(j * 3 + k, windowWidth - i)) = pointInfo;
                img3.at<uchar>(Point(windowWidth * 3 - (j+1) * 3 + k, windowWidth - i)) = pointInfo;
            }
        }
    }

    int hueShift = static_cast<int>(inputX / 2 + inputY / 2);

    Mat hsv;
    cvtColor(img3, hsv, COLOR_BGR2HSV);

    for (int j = 0; j < img3.rows; j++) {
        for (int i = 0; i < img3.cols; i++) {
            unsigned char h = hsv.at<Vec3b>(j, i)[0];

            h = (h + hueShift) % 180;

            hsv.at<Vec3b>(j, i)[0] = h;
        }
    }

    cvtColor(hsv, img3, COLOR_HSV2BGR);
    
    return img3;
}


Mat cannyFilter(Mat img) {
    Mat imgBlur, imgCanny, imgDil, imgErode;

    int val1 = inputX / 3;
    int val2 = inputY / 2 + val1;

    GaussianBlur(img, imgBlur, Size(3, 3), 3, 0);
    Canny(imgBlur, imgCanny, val1, val2);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);
    erode(imgDil, imgErode, kernel);

    return imgErode;
}


Mat hueRainbow(Mat img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    float val1 = inputX;
    float val2 = inputY;

    for (int j = 0; j < img.rows; j++) {
        for (int i = 0; i < img.cols; i++) {
            unsigned char b = img.at<Vec3b>(j, i)[0];

            b = b + val1;
            b = b % 180;

            hsv.at<Vec3b>(j, i)[0] = b;
            

            if (hsv.at<Vec3b>(j, i)[1] > val2) {

            }
            else {
                hsv.at<Vec3b>(j, i)[1] = val2;
            }

            if (hsv.at<Vec3b>(j, i)[2] > val2) {

            }
            else {
                hsv.at<Vec3b>(j, i)[2] = val2;
            }

        }
    }

    Mat res;
    cvtColor(hsv, res, COLOR_HSV2BGR);

    return res;
}


Mat buchiContrast2(Mat img) {
    Mat new_image, output_image;
    int val1 = (float)inputX / 255 * 200 - 100; // [-100, 100]
    float val2 = 0; // [0, 5], but centered at 0
    if (inputY > 127) val2 = (float)inputY / 255.0 * 10.0 - 4.0;
    else val2 = (float)inputY / 255.0 * 2.0;

    img.convertTo(new_image, -1, 1, val1);
    new_image.convertTo(output_image, -1, val2, 0);

    return output_image;
}





// for sliders, I don't know why we need this
void nothing(int x, void* data) {}


int main() {

    Mat img, img2;
    VideoCapture cap(0); 
    Mat imgCrop;

    arduino = new SerialPort(portName);

    namedWindow("img");

    /*int slider1 = 0;
    int slider2 = 127;
    int slider3 = 127;
    createTrackbar("filter", "img", &slider1, 4, nothing);
    createTrackbar("X input", "img", &slider2, 255, nothing);
    createTrackbar("Y input", "img", &slider2, 255, nothing);*/


    while (true) {
        autoConnect(); // collect arduino messages

        cap >> img; // collect video feed frame
        cap >> img2;

        // used to change variables without arduino
        /*int colorValue = getTrackbarPos("filter", "img");
        if (colorValue == 0) inputColor = 'R';
        else if (colorValue == 1) inputColor = 'B';
        else if (colorValue == 2) inputColor = 'G';
        else if (colorValue == 3) inputColor = 'C';
        else if (colorValue == 4) inputColor = 'Y';
        inputX = getTrackbarPos("X input", "img");
        inputY = getTrackbarPos("Y input", "img");*/

        // change the filter. We have Red, Blue, Green, Cyan, Yellow
        if (inputColor == 'R') {
            // alpha beta contrast filter
            img2 = buchiContrast2(img);
        }
        else if (inputColor == 'B') {
            // brightness and saturation
            img2 = brightness2(img);
        }
        else if (inputColor == 'G') {
            // kaleidoscope + hue change
            img2 = kaleidoscope(img);
        }
        else if (inputColor == 'C') {
            // B of BGR to hue, then saturation and hue change
            img2 = hueRainbow(img);
        }
        else if (inputColor == 'Y') {
            // Canny filter with adjustable settings
            img2 = cannyFilter(img);
        }

        imshow("img", img);
        imshow("img2", img2);
        waitKey(1);
    }

}
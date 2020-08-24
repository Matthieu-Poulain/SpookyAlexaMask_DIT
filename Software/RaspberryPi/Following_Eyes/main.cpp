#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <cstdio>
#include <iostream>
#include "opencv2/opencv.hpp"
//Include the C Libraries. //
extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include "rs232.h"
#include <pthread.h>

}

using namespace std;
using namespace cv;


#define BUF_SIZE 128
cv::VideoCapture * webCam_;
cv::Mat frame;

/*
void * refresh_frame(void *)       //Function refreshing the frame as soon as possible
{
    while(1)                      // while(1) : we want to grab frames continuously
    {
        webCam_->grab();
    }
}
*/

void * ImageProcessingFunction(void *){



    // 0 = PC, 1 = Raspberry Pi, 2 = Beaglebone AI;
    //Can be set to avoid using raspberry pi's components when compiling on and for a PC.
    int computerType=2;


    int cport_nr=18; /* /dev/ttyUSB2 replaced with an alias */  //<------- Here, the default value may have to be changed. (Look at the appropriate website)
    int bdrate=115200; /* 115200 baud */
    char mode[]={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
    char str_send[3][BUF_SIZE]; // send data buffer, containing the different usable commands.
    unsigned char str_recv[BUF_SIZE]; // recv data buffer
    //Adding to the str_send Buffer the different commands we can send. More can be added by increasing the size of the buffer, and then adding the corresponding command.
    strcpy(str_send[0], "[NoFaceDetected]");
    strcpy(str_send[1], "[srvCMD");
    strcpy(str_send[2], "[othCMD");


    //Open communication port. Needs to be set for Beaglebone AI too (Add option computerTyper==2)
    if (computerType==1){
        if(RS232_OpenComport(cport_nr, bdrate, mode,0))
        {
            printf("Can not open comport\n");
            return(NULL);
        }
        usleep(2000000);  /* waits 2000ms for stable condition */
    }



    // //////////////// //
    // Init OpenCV Part //
    // //////////////// //

    // (You can set here the Frame width and height you need to use) | Warning : changing the default values (640/480) requires to change the range of values used on the arduino to move the eyes
    //int frameWidth=1280;
    //int frameHeight=720;

    Point pointBuffer1(0,0); // Point buffers used to determine if the detected face is still the same.
    Point pointBuffer2(0,0);

    cout<<"width :"<<webCam_->get(CAP_PROP_FRAME_WIDTH)<<endl;
    cout<<"height :"<<webCam_->get(CAP_PROP_FRAME_HEIGHT)<<endl;

    // To change camera resolution, uncomment the following lines:
    //webCam_->set(CAP_PROP_FRAME_WIDTH,frameWidth);
    //webCam_->set(CAP_PROP_FRAME_HEIGHT,frameHeight);
    //cout<<"width :"<<webCam_->get(CAP_PROP_FRAME_WIDTH)<<endl;
    //cout<<"height :"<<webCam_->get(CAP_PROP_FRAME_HEIGHT)<<endl;


    if(!webCam_->isOpened())  // check if the camera has been opened correctly.
    {
        cerr<<"Error openning the default camera"<<endl;
        return (100);
    }
    //Init the cascadeClassifier
    cv::CascadeClassifier face_cascade;


    // The file location needs to be set properly.
    if( !face_cascade.load( "/home/pi/Desktop/software/Following_Eyes/Following_Eyes/haarcascade_frontalface_alt2.xml" ) )
    {
        cerr<<"Error loading haarcascade"<<endl;
        return (101);
    }


    // Start the main loop of the code
    while(1){


        Mat frame_gray;
        std::vector<Rect> faces; // Vector containing the coordinates of all detected faces (Several can be detected).
        // Empty the frame buffer
        webCam_->grab();
        // Get the last frame
        if (webCam_->read(frame)){

            // Mirror effect - usefull when viewing the result, but resources consuming. If not required,
            // it is better to simply return an inverted result.

            //cv::flip(frame,frame,1);
            // Convert to gray
            cv::cvtColor(frame,frame_gray,COLOR_BGR2GRAY);
            // Equalize graylevels
            //        equalizeHist( frame_gray, frame_gray );

            //-- Face(s) Detection
            face_cascade.detectMultiScale( frame_gray, faces, 1.1, 4, 0, Size(20, 20) );// Increasing the size decreases the time needed to process the image. It needs to be set according to the smallest size of head you want to detect
            char charBuf[256];
            char charPoint[256];
            switch ( faces.size() ){
            case 0:{
                strcpy(charBuf,str_send[0]);
                cout<<"case 0 : no face detected"<<endl;
                break;
            }
            case 1:{
                Point workingCenter(faces[0].x+faces[0].height/2,faces[0].y+faces[0].width/2);
                sprintf(charPoint, "%d", int(webCam_->get(CAP_PROP_FRAME_WIDTH)-workingCenter.x)); // X coordinate buffer (inverted to compensate the mirror effect)
                strcpy(charBuf,str_send[1]); // Command to use
                strcat(charBuf,charPoint); // x coordinate value to be send
                strcat(charBuf,"]"); // End of data.
                pointBuffer2 = pointBuffer1;
                pointBuffer1 = workingCenter;
                cout<<"case 1 : 1 face detected"<<endl;
                break;
            }
            default:{
                int minDist=sqrt(pow(webCam_->get(CAP_PROP_FRAME_WIDTH),2)+pow(webCam_->get(CAP_PROP_FRAME_HEIGHT),2));
                int indexI=0;
                // Face drawings
                for (int i=0;i<(int)faces.size();i++){
                    Point workingCenter(faces[i].x+faces[i].height/2,faces[i].y+faces[i].width/2);
                    if (sqrt(pow(workingCenter.x-pointBuffer1.x,2)+pow(workingCenter.y-pointBuffer1.y,2))<=minDist){
                        minDist = sqrt(pow(workingCenter.x-pointBuffer1.x,2)+pow(workingCenter.y-pointBuffer1.y,2));
                        indexI=i;
                    }
                }

                Point workingCenter(faces[indexI].x+faces[indexI].height/2,faces[indexI].y+faces[indexI].width/2);
                sprintf(charPoint, "%d", int(webCam_->get(CAP_PROP_FRAME_WIDTH)-workingCenter.x));
                strcpy(charBuf,str_send[1]);
                strcat(charBuf,charPoint);
                strcat(charBuf,"]");
                pointBuffer2 = pointBuffer1;
                pointBuffer1 = workingCenter;
                cout<<"Case 2 : Several faces detected"<<endl;
                break;
                }
            }
            //cout<<charBuf<<endl;
            if (computerType==1){
                RS232_cputs(cport_nr, charBuf); // sends string on serial
            }
            printf("Sent to Arduino: '%s'\n", charBuf); // Print on the console the string sent to the arduino

            if (computerType==1){
                usleep(100000);  // waits for reply 100ms - may be decreased if the data are send fast enough
                int n = RS232_PollComport(cport_nr, str_recv, (int)BUF_SIZE); //Get the received data
                if(n > 0){
                    str_recv[n] = 0;   // always put a "null" at the end of a string!
                    printf("Received %i bytes: --]%s[--\n", n, (char *)str_recv);
                }
                else{
                    cout<<"error in int n rs232"<<endl;
                    cout<<"Reseting the communication port"<<endl;
                    RS232_CloseComport(cport_nr);
                    usleep(500000);
                    if(RS232_OpenComport(cport_nr, bdrate, mode,0))
                    {
                        printf("Can not open comport\n");
                        //printf("Scanning for available comports\n");
                        //Add an option to scan for available comports and check if the good one is available. Mostly done with the alias based on ID outside of the software. Requires manual setup on linux.
                        //return(0); // commented in order to try again.
                    }
                    usleep(1000000);
                }
            }






            // Output window to show the image on our screen - Debug purpose.
            //imshow("WebCam", frame);
            // cv::waitKey(1);
            cout<<"Done!"<<endl;
        }

        else{
            cout<<"Not Done, error in acquiring image!"<<endl;

            cout<<"A new Camera Object may require to be set"<<endl;
            // Code has to be added here to reload the camera object//
        }




    }
}






int main()
{


    // Init output window to show the image on our screen - Debug purpose.
    //cv::namedWindow("WebCam",1);

    // ////////////////////////////////////////////////////////////////////////////////// //
    // //////////////////////////////////Thread part///////////////////////////////////// //
    // ////////////////////////////////////////////////////////////////////////////////// //

    webCam_ =new cv::VideoCapture(-1); // -1 = autosetup, but BBAI already has a video0 device, which isn't the webcam. It then needs to be set to 1 (or the number corresponding to the webcam in /dev/videoX)
    webCam_->set(cv::CAP_PROP_BUFFERSIZE, 1);



    pthread_t thread_1; // Create a Thread object
    //pthread_t thread_2; // Create a second Thread object
    //pthread_create(&thread_2,NULL,&refresh_frame,NULL);

    pthread_create(&thread_1,NULL,&ImageProcessingFunction,NULL); // Init the thread

    //s is used to have a clean way to quit the software
    std::string s;
    while(s!="q"){cin>>s;}


    // Close the threads.
    pthread_cancel(thread_1);
    pthread_join(thread_1,NULL);
    //pthread_cancel(thread_2);
    //pthread_join(thread_2,NULL);

    return(0);
}












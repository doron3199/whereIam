#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv))
        ROS_ERROR("Failed to call service DriveToTarget");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;
    int i,j;    

    // for loop for each pixel in the image
    for (i = 0; i < img.height; i++){
       for (j = 0; j < img.step; j++){
            // checking where is the pixel and sending the coresponding command
            if (img.data[i*img.step + j] == white_pixel){
                ROS_INFO("Saw the ball ");
                if ( j < img.width){
                    ROS_INFO("in the left");
                    drive_robot(0.5,1);
                } else if (j > img.width*2){
                    ROS_INFO("in the right");
                    drive_robot(0.5,-1);
                } else {
                    ROS_INFO("in the middle");
                    drive_robot(0.5,0.0);
                }
                return;  
            }                     
        }
    }
    ROS_INFO("didn't saw the ball ");

    // if the loop didn't found a white pixel the code will reach here
    drive_robot(0.0,0.0);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

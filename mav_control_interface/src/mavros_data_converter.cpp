#include "ros/ros.h"
#include <mav_msgs/RollPitchYawrateThrust.h>
#include <mavros_msgs/AttitudeTarget.h>
#include <tf/tf.h>
#include <sstream>

ros::Publisher pub;
ros::Subscriber sub;

void attitudeTargetCallback(const mavros_msgs::AttitudeTarget::ConstPtr msg){
    
    tf::Quaternion q(
        msg->orientation.x,
        msg->orientation.y,
        msg->orientation.z,
        msg->orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    
    mav_msgs::RollPitchYawrateThrust rpyt;
    rpyt.header.stamp = msg->header.stamp;
    rpyt.header.seq = msg->header.seq;
    rpyt.pitch = pitch;
    rpyt.roll = roll;
    rpyt.yaw_rate = msg->body_rate.z;
    rpyt.thrust.z = msg->thrust;
    pub.publish(rpyt);
}

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "mavros_data_converter");

  ros::NodeHandle n;

  pub = n.advertise<mav_msgs::RollPitchYawrateThrust>("rpythrust", 1000);
  sub = n.subscribe("/scout/mavros/setpoint_raw/target_attitude", 1000, attitudeTargetCallback);

  ros::spin();
  return 0;
}
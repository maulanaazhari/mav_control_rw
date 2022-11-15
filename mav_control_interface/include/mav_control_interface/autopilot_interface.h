#ifndef AUTOPILOT_INTERFACE_H_
#define AUTOPILOT_INTERFACE_H_

#include <mavros_msgs/AttitudeTarget.h>
#include <sensor_msgs/Imu.h>
#include <mav_msgs/default_topics.h>
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include "mav_control_interface/mav_control_interface.h"

struct AutopilotInterface {
  static const std::string kDefaultAutopilotInterface;

  enum class Types { ASCTEC, MAVROS, INVALID };

  static Types getInterfaceType(const ros::NodeHandle& nh_in);

  static void setupRCInterface(
      const ros::NodeHandle& nh, const ros::NodeHandle& private_nh,
      std::shared_ptr<mav_control_interface::RcInterfaceBase>*
          rc_interface_ptr);
};

class BaseCommandPublisher {
 public:
  BaseCommandPublisher(const ros::NodeHandle& nh,
                       const ros::NodeHandle& private_nh);

  virtual void publishCommand(
      const mav_msgs::EigenRollPitchYawrateThrust& command, double thrust_min,
      double thrust_max) = 0;

 protected:
  ros::NodeHandle nh_;
  ros::NodeHandle private_nh_;
};

class AscTecCommandPublisher : public BaseCommandPublisher {
 public:
  AscTecCommandPublisher(const ros::NodeHandle& nh,
                         const ros::NodeHandle& private_nh);

  void publishCommand(const mav_msgs::EigenRollPitchYawrateThrust& command,
                      double thrust_min, double thrust_max);

 private:
  ros::Publisher attitude_and_thrust_command_publisher_;
};

class MavRosCommandPublisher : public BaseCommandPublisher {
 public:
  static const double kDefaultYawGain;

  MavRosCommandPublisher(const ros::NodeHandle& nh,
                         const ros::NodeHandle& private_nh);

  void publishCommand(const mav_msgs::EigenRollPitchYawrateThrust& command,
                      double thrust_min, double thrust_max);

  void orientationCallback(const sensor_msgs::ImuConstPtr& msg);

 private:
  ros::Publisher command_publisher_;
  ros::Subscriber orientation_subscriber_;

  double internal_yaw_;
  double yaw_gain_;
};

class CommandInterface {
 public:
  CommandInterface(const ros::NodeHandle& nh,
                   const ros::NodeHandle& private_nh);

  void publishCommand(const mav_msgs::EigenRollPitchYawrateThrust& command,
                      double thrust_min, double thrust_max);

 private:
  std::shared_ptr<BaseCommandPublisher> command_pub_ptr_;
};

#endif  // AUTOPILOT_INTERFACE_H_
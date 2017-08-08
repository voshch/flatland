/*
 *  ______                   __  __              __
 * /\  _  \           __    /\ \/\ \            /\ \__
 * \ \ \L\ \  __  __ /\_\   \_\ \ \ \____    ___\ \ ,_\   ____
 *  \ \  __ \/\ \/\ \\/\ \  /'_` \ \ '__`\  / __`\ \ \/  /',__\
 *   \ \ \/\ \ \ \_/ |\ \ \/\ \L\ \ \ \L\ \/\ \L\ \ \ \_/\__, `\
 *    \ \_\ \_\ \___/  \ \_\ \___,_\ \_,__/\ \____/\ \__\/\____/
 *     \/_/\/_/\/__/    \/_/\/__,_ /\/___/  \/___/  \/__/\/___/
 * @copyright Copyright 2017 Avidbots Corp.
 * @name	DiffDrive.cpp
 * @brief   DiffDrive plugin
 * @author  Mike Brousseau
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2017, Avidbots Corp.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Avidbots Corp. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <Box2D/Box2D.h>
#include <flatland_plugins/diff_drive.h>
#include <flatland_server/debug_visualization.h>
#include <flatland_server/model_plugin.h>
#include <pluginlib/class_list_macros.h>
#include <ros/ros.h>
#include "geometry_msgs/Twist.h"

namespace flatland_plugins {

void DiffDrive::OnInitialize(const YAML::Node& config) {
  ROS_INFO_NAMED("DiffDrivePlugin", "DiffDrive Initialized");
  robot_angle = 0.0;
  robot_position = b2Vec2(0.0, 0.0);

  // get the robot pointer
  robot = model_->GetBody("base")->physics_body_;

  // subscribe to the cmd_vel topic
  sub = nh_.subscribe("/cmd_vel", 0, &DiffDrive::TwistCallback, this);
}

void DiffDrive::BeforePhysicsStep(
    const flatland_server::Timekeeper& timekeeper) {
  time_step = timekeeper.GetStepSize() * speed_factor;

  robot = model_->GetBody("base")->physics_body_;

  ApplyVelocity();

  flatland_server::DebugVisualization::Get().Reset("diffbody");
  flatland_server::DebugVisualization::Get().Visualize("diffbody", robot, 1.0,
                                                       1.0, 1.0, 0.5);
}

void DiffDrive::TwistCallback(const geometry_msgs::Twist& msg) {
  velocity = msg.linear.x;
  omega = msg.angular.z;
}

void DiffDrive::ApplyVelocity() {
  // Integrate the twist
  robot_position.x += velocity * -sin(robot_angle) * time_step;
  robot_position.y += velocity * cos(robot_angle) * time_step;
  robot_angle += omega * time_step;

  // set the robot transform
  robot->SetTransform(robot_position, robot_angle);

  // ROS_INFO_STREAM("Subscriber velocities:"<<" velocity="<<velocity<<"
  // omega="<< omega);
  // ROS_INFO_STREAM(" robot_angle="<<robot_angle);
}
};

PLUGINLIB_EXPORT_CLASS(flatland_plugins::DiffDrive,
                       flatland_server::ModelPlugin)
#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <explore_lite/SubmapSizeConfig.h>
#include <visualization_msgs/MarkerArray.h>

double submap_size;

void dynamicReconfigureCallback(explore_lite::SubmapSizeConfig &config, uint32_t level) {
    submap_size = config.submap_size;
}

void frontierCallback(const visualization_msgs::MarkerArray::ConstPtr& msg) {
    int frontier_count = 0;
    for (const auto& marker : msg->markers) {
        if (marker.ns == "frontiers" && marker.type == visualization_msgs::Marker::SPHERE) {
            frontier_count++;
        }
    }
    ROS_INFO("Number of frontiers: %d", frontier_count);

    if (frontier_count <= 2) {
        ROS_INFO("No frontiers found, expanding search area to 16m");
        submap_size = 16;

        dynamic_reconfigure::ReconfigureRequest srv_req;
        dynamic_reconfigure::ReconfigureResponse srv_resp;
        dynamic_reconfigure::IntParameter param;
        dynamic_reconfigure::Config config;

        param.name = "submap_size";
        param.value = 16;
        config.ints.push_back(param);

        srv_req.config = config;
        ros::service::call("/submap_reconfig_node/set_parameters", srv_req, srv_resp);
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "submap_reconfig_node");
    ros::NodeHandle nh;

    // Dynamic reconfigure server
    dynamic_reconfigure::Server<explore_lite::SubmapSizeConfig> server;
    dynamic_reconfigure::Server<explore_lite::SubmapSizeConfig>::CallbackType f;

    f = boost::bind(&dynamicReconfigureCallback, _1, _2);
    server.setCallback(f);

    ros::Subscriber frontier_sub = nh.subscribe("/explore/frontiers", 10, frontierCallback);

    ros::spin();

    return 0;
}

// #include <ros/ros.h>
// #include <std_msgs/Int32.h>
// #include <dynamic_reconfigure/server.h>
// #include <explore_lite/SubmapSizeConfig.h>

// double submap_size;

// void dynamicReconfigureCallback(explore_lite::SubmapSizeConfig &config, uint32_t level) {
//     submap_size = config.submap_size;
// }

// void frontierCallback(const std_msgs::Int32::ConstPtr& msg) {
//     if (msg->data == 0) {
//         ROS_INFO("No frontiers found, expanding search area to 16m");
//         submap_size = 16;
//         // Code to dynamically set the parameter submap_size to 16
//         dynamic_reconfigure::Config config;
//         dynamic_reconfigure::IntParameter param;
//         param.name = "submap_size";
//         param.value = 16;
//         config.ints.push_back(param);
//         dynamic_reconfigure::ReconfigureRequest srv_req;
//         dynamic_reconfigure::ReconfigureResponse srv_resp;
//         srv_req.config = config;
//         ros::service::call("/submap_reconfig_node/set_parameters", srv_req, srv_resp);
//     }
// }

// int main(int argc, char** argv) {
//     ros::init(argc, argv, "submap_reconfig_node");
//     ros::NodeHandle nh;

//     // Dynamic reconfigure server
//     dynamic_reconfigure::Server<explore_lite::SubmapSizeConfig> server;
//     dynamic_reconfigure::Server<explore_lite::SubmapSizeConfig>::CallbackType f;

//     f = boost::bind(&dynamicReconfigureCallback, _1, _2);
//     server.setCallback(f);

//     ros::Subscriber frontier_sub = nh.subscribe("frontier_count", 10, frontierCallback);

//     ros::spin();

//     return 0;
// }

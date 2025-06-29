﻿#include "Triangle.hpp"
#include "rasterizer.hpp"
#include<Eigen/Core>
#include<Eigen/Dense>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float angle_rad = rotation_angle * MY_PI / 180.0f;
    Eigen::Matrix4f rotation_matrix;

    rotation_matrix << std::cos(angle_rad), -std::sin(angle_rad), 0, 0,
                       std::sin(angle_rad),  std::cos(angle_rad), 0, 0,
                                         0,                    0, 1, 0,
		                                 0,                    0, 0, 1;

    model = model * rotation_matrix;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    // https://math.stackexchange.com/questions/4522384/how-is-aspect-ratio-used-in-perspective-projection
    // The projection matrix is for a general frustum, assume the viewing volume is symmetric.
    // Then we have r + 1 = 0, t + b = 0 => r = -l, t = -b

    // Finding the values of l, r, b, t, n, f
    auto eye_fov_rad = eye_fov / 180.0f * MY_PI;
    auto t = std::tan(eye_fov_rad / 2) * std::abs(zNear); // FOV = tan(x/2) = t / |n|, where x is the vertical FOV angle
    auto r = t * aspect_ratio; // aspect_ratio = (r - l) / (t - b)
    auto b = -t;
    auto l = -r;

    // https://stackoverflow.com/questions/28286057/trying-to-understand-the-math-behind-the-perspective-matrix-in-webgl
    // In here, take the - sign convention for z-axis, so that the camera looks towards the negative z-axis.
    // from the formula we use is assuming the camera is looking towards the negative z-axis
    // and the near and far planes are at -zNear and -zFar respectively.

    auto n = -zNear;
    auto f = -zFar;

    Eigen::Matrix4f persp_to_ortho_matrix;
    persp_to_ortho_matrix << n, 0, 0, 0,
					         0, n, 0, 0,
					         0, 0, n + f, -n * f,
					         0, 0, 1, 0;

    Eigen::Matrix4f ortho_matrix;
    ortho_matrix << 2 / (r - l), 0, 0, -(r + l) / (r - l),
					0, 2 / (t - b), 0, -(t + b) / (t - b),
					0, 0, 2 / (n - f), -(n + f) / (n - f),
					0, 0, 0, 1;

    projection = ortho_matrix * persp_to_ortho_matrix;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        // std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
            std::cout << "Clicked a";
        }
        else if (key == 'd') {
            angle -= 10;
            std::cout << "Clicked d";
        }
    }

    return 0;
}

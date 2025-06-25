#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
// #define NORMAL_TASK

#ifdef NORMAL_TASK
#define FILENAME "my_bezier_curve_normal.png"
#else
#define FILENAME "my_bezier_curve_anti_aliasing.png"
#endif // NORMAL_TASK

std::vector<cv::Point2f> control_points;

int steps = 1000;

void mouse_handler(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4)
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
                  << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window)
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                     3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

/**
 *  Lerp:
 *  |---- t ----|--------------- 1-t ------------|
 *  a           pt                               b
 */
cv::Point2f lerp(cv::Point2f a, cv::Point2f b, float t)
{
    cv::Point2f pt = a * (1 - t) + b * t;
    return pt;
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t)
{
    // TODO: Implement de Casteljau's algorithm
    if (control_points.size() == 1)
    {
        return control_points[0];
    }

    std::vector<cv::Point2f> sub_control_points;

    for (int i = 0; (i + 1) < control_points.size(); i++)
    {
        sub_control_points.push_back(lerp(control_points[i], control_points[i + 1], t));
    }

    return recursive_bezier(sub_control_points, t);
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window)
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's
    // recursive Bezier algorithm.
    for (int i = 0; i < steps; i++)
    {
        float t = (float)i * (1.0f / steps);
        auto point = recursive_bezier(control_points, t);

        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;

#ifdef NORMAL_TASK
        continue;
#endif

        // Bilinear interpolation (closest four points)
        int x_upper = floor(point.x) + 1;
        int y_upper = floor(point.y) + 1;
        cv::Point2i p00 = cv::Point2i(floor(point.x), floor(point.y));
        cv::Point2i p01 = cv::Point2i(x_upper, floor(point.y));
        cv::Point2i p10 = cv::Point2i(floor(point.x), y_upper);
        cv::Point2i p11 = cv::Point2i(x_upper, y_upper);

        // Their weighted mean
        // ref: https://en.wikipedia.org/wiki/Bilinear_interpolation
        auto w3 = (point.x - floor(point.x)) * (point.y - floor(point.y));
        auto w2 = (x_upper - point.x) * (point.y - floor(point.y));
        auto w1 = (point.x - floor(point.x)) * (y_upper - point.y);
        auto w0 = (x_upper - point.x) * (y_upper - point.y);

        // Final color
        window.at<cv::Vec3b>(p00.y, p00.x)[1] = MIN(255, window.at<cv::Vec3b>(p00.y, p00.x)[1] + 255.0f * w0);
        window.at<cv::Vec3b>(p01.y, p01.x)[1] = MIN(255, window.at<cv::Vec3b>(p01.y, p01.x)[1] + 255.0f * w1);
        window.at<cv::Vec3b>(p10.y, p10.x)[1] = MIN(255, window.at<cv::Vec3b>(p10.y, p10.x)[1] + 255.0f * w2);
        window.at<cv::Vec3b>(p11.y, p11.x)[1] = MIN(255, window.at<cv::Vec3b>(p11.y, p11.x)[1] + 255.0f * w3);
    }
}

int main(int argc, char *argv[])
{
    // Check arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-steps" && i + 1 < argc)
        {
            try
            {
                auto _steps = std::stoi(argv[i + 1]);
                steps = _steps;
                ++i;
                std::cout << "[Args] Steps: " << _steps << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cout << "Error: Invalid value for -steps" << std::endl;
            }
        }
    }

    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    control_points.push_back(cv::Point2f(100, 100));
    control_points.push_back(cv::Point2f(200, 500));
    control_points.push_back(cv::Point2f(250, 400));
    control_points.push_back(cv::Point2f(400, 100));

    int key = -1;
    while (key != 27)
    {
        for (auto &point : control_points)
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4)
        {
#ifdef NORMAL_TASK
            naive_bezier(control_points, window);
#endif
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite(FILENAME, window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}

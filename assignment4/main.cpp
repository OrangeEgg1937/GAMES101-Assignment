#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

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
        sub_control_points.push_back(lerp(control_points[i], control_points[i+1], t));
    }
    
    
    return recursive_bezier(sub_control_points, t);

}

float weighted_area(cv::Point2i point, cv::Point2f center)
{
    auto w = abs(center.x - point.x);
    auto h = abs(center.y - point.y);
    return w * h;
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for (int i = 0; i < steps; i++)
    {
        float t = (float) i * (1.0f / steps);
        auto point = recursive_bezier(control_points, t);

        // Bilinear interpolation (closest four points)
        cv::Point2i p00 = cv::Point2i(floor(point.x), floor(point.y));
        cv::Point2i p01 = cv::Point2i(floor(point.x), ceil(point.y));
        cv::Point2i p10 = cv::Point2i(ceil(point.x), floor(point.y));
        cv::Point2i p11 = cv::Point2i(ceil(point.x), ceil(point.y));

        // Their weighted mean 
        // ref: https://en.wikipedia.org/wiki/Bilinear_interpolation
        auto w0 = weighted_area(p00, point);
        auto w1 = weighted_area(p01, point);
        auto w2 = weighted_area(p10, point);
        auto w3 = weighted_area(p11, point);
        auto sum = w0 + w1 + w2 + w3;
        if (sum == 0) sum = 1;

        // Final color
        window.at<cv::Vec3b>(p00.y, p00.x)[1] = MIN(255, window.at<cv::Vec3b>(p00.y, p00.x)[1] + 255.0f * w0 / sum);
        window.at<cv::Vec3b>(p01.y, p01.x)[1] = MIN(255, window.at<cv::Vec3b>(p01.y, p01.x)[1] + 255.0f * w1 / sum);
        window.at<cv::Vec3b>(p10.y, p10.x)[1] = MIN(255, window.at<cv::Vec3b>(p10.y, p10.x)[1] + 255.0f * w2 / sum);
        window.at<cv::Vec3b>(p11.y, p11.x)[1] = MIN(255, window.at<cv::Vec3b>(p11.y, p11.x)[1] + 255.0f * w3 / sum);
    }
}

void bezier_gen(const std::vector<cv::Point2f> &control_points, cv::Mat &window)
{
    // Make sure 'steps' is defined, e.g., const int steps = 1000;
    for (int i = 0; i < steps; i++)
    {
        float t = (float) i * (1.0f / steps);
        auto point = recursive_bezier(control_points, t);

        // Get integer and fractional parts of the floating point coordinate
        // These are the coordinates of the bottom-left pixel of the 2x2 block
        int x_i = static_cast<int>(floor(point.x)); // or just static_cast<int>(point.x) for positive values
        int y_j = static_cast<int>(floor(point.y)); // or just static_cast<int>(point.y) for positive values

        float x_f = point.x - x_i; // Fractional part for x
        float y_f = point.y - y_j; // Fractional part for y

        // Define the four surrounding pixel coordinates (using x_i, y_j for clarity)
        cv::Point2i p00(x_i,     y_j);     // bottom-left
        cv::Point2i p10(x_i + 1, y_j);     // bottom-right
        cv::Point2i p01(x_i,     y_j + 1); // top-left
        cv::Point2i p11(x_i + 1, y_j + 1); // top-right

        // Calculate the correct bilinear interpolation weights
        float w_p00 = (1.0f - x_f) * (1.0f - y_f);
        float w_p10 = x_f * (1.0f - y_f);
        float w_p01 = (1.0f - x_f) * y_f;
        float w_p11 = x_f * y_f;

        // The sum of these weights (w_p00 + w_p10 + w_p01 + w_p11) will be 1.0f.
        // So, no need for the `sum` and `if (sum == 0) sum = 1;` logic.

        // Function to safely apply intensity to a pixel
        auto apply_intensity = [&](const cv::Point2i& p, float weight) {
            // Check bounds to prevent crashes
            if (p.x >= 0 && p.x < window.cols && p.y >= 0 && p.y < window.rows) {
                // Add the weighted intensity to the green channel
                // Assuming window is initialized to black (0,0,0)
                window.at<cv::Vec3b>(p.y, p.x)[1] = MIN(255, static_cast<int>(window.at<cv::Vec3b>(p.y, p.x)[1] + 255.0f * weight));
            }
        };

        apply_intensity(p00, w_p00);
        apply_intensity(p10, w_p10);
        apply_intensity(p01, w_p01);
        apply_intensity(p11, w_p11);
    }
}

int main(int argc, char* argv[]) 
{
    // Check arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-steps" && i + 1 < argc) {
            try {
                auto _steps = std::stoi(argv[i + 1]);
                steps = _steps;
                ++i;
                std::cout << "[Args] Steps: " << _steps << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error: Invalid value for -steps" << std::endl;
            }
        }
    }

    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            // naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}

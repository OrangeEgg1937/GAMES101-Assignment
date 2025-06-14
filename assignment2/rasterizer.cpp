// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vector3f P(x, y, _v[0].z());
    
    Vector3f AB = _v[1] - _v[0];
    Vector3f BC = _v[2] - _v[1];
    Vector3f CA = _v[0] - _v[2];

    Vector3f AP = P - _v[0];
    Vector3f BP = P - _v[1];
    Vector3f CP = P - _v[2];

    // Edge Check
    Vector3f AB_cross_AP = AB.cross(AP);
    Vector3f BC_cross_BP = BC.cross(BP);
    Vector3f CA_cross_CP = CA.cross(CP);

    // Dot product with the reference edge
    return (AB_cross_AP.dot(BC_cross_BP) > 0) && (BC_cross_BP.dot(CA_cross_CP) > 0) && (CA_cross_CP.dot(AB_cross_AP) > 0);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t, bool isSuperSampling) {
    auto v = t.toVector4();

    // TODO : Find out the bounding box of current triangle.
    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    
    auto minX = MIN(v[0][0], MIN(v[1][0], v[2][0]));
    auto maxX = MAX(v[0][0], MAX(v[1][0], v[2][0]));
    auto minY = MIN(v[0][1], MIN(v[1][1], v[2][1]));
    auto maxY = MAX(v[0][1], MAX(v[1][1], v[2][1]));

    minX = std::floor(minX);
    maxX = std::ceil(maxX);
    minY = std::floor(minY);
    maxY = std::ceil(maxY);

    // iterate through the pixel and find if the current pixel is inside the triangle
    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            if (true) {
                // Sampling the pixel with 4 sub-pixels (2x2)
                // We assume the center of the pixel is the index, i.e point[x][y]
                // The sub-pixels are at (x+0.25, y+0.25), (x+0.75, y+0.25), (x+0.25, y+0.75), (x+0.75, y+0.75)
                // We define as follow:
                // ---------------
                // | 0.25 | 0.25 |
                // |----(x,y)----|
                // | 0.25 | 0.25 |
                // ---------------
                std::pair <float, float> sub_pixels[4] = {
					{x - 0.25f, y + 0.25f},
					{x + 0.25f, y + 0.25f},
					{x - 0.25f, y - 0.25f},
					{x + 0.25f, y - 0.25f}
				};

                static const float SSAA_RATIO[5] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};

                // Check each sub-pixel is inside the triangle
                int sub_count = 0;
                for (int sub_pixel_i = 0; sub_pixel_i < 4; sub_pixel_i++) {
                    auto sub_pixel = sub_pixels[sub_pixel_i];
                    if (insideTriangle(sub_pixel.first, sub_pixel.second, t.v)) {
						sub_count++;

                        // If so, use the following code to get the interpolated z value.
                        auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                        float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;

                        // Check the sample depth buffer
                        auto curr = get_index(x, y) + sub_pixel_i;

                        if (-z_interpolated < depth_sample[curr]) {
                            depth_sample[curr] = -z_interpolated;
                            frame_sample[curr] = t.getColor() / 4.0f;
                        }
					}
				}

                // Current pixel
                auto curr = get_index(x, y);
                auto pixel_color = frame_sample[curr] + frame_sample[curr + 1] + frame_sample[curr + 2] + frame_sample[curr + 3];
                set_pixel(Vector3f(x, y, 1.0f), pixel_color);
            }
            else {
                if (insideTriangle(x, y, t.v)) {
                    // If so, use the following code to get the interpolated z value.
                    auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;

                    // Check the depth buffer
                    auto curr = get_index(x, y);

                    // Count:58081 | z=0.714285
                    // Count:34756 | z=0.500002
                    // By default, the depth_buf = +inf, larger value = more far away from the viewpoint
                    // Since it using the positive to compare, we flip the sign of the z-value 
                    if (-z_interpolated < depth_buf[curr]) {
                        set_pixel(Vector3f(x, y, 1.0f), t.getColor());
                        depth_buf[curr] = -z_interpolated;
                    }
                }
            }
        }
    }
    
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    // Added to init the frame_sample and depth_sample buffers
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(frame_sample.begin(), frame_sample.end(), Eigen::Vector3f{ 0, 0, 0 });
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(depth_sample.begin(), depth_sample.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);

    // Added to init the frame_sample and depth_sample buffers
    frame_sample.resize(w * h * 4);
    depth_sample.resize(w * h * 4);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on
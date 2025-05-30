#include<cmath>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<iostream>

#define PI 3.14159265

// Define the rotation matrix R(a)
Eigen::Matrix3f rotation_matrix(float degress) {
    
    Eigen::Matrix3f _m;
    float _radians = degress * M_PI / 180;

    _m << std::cos(_radians), -std::sin(_radians), 0,
          std::sin(_radians),  std::cos(_radians), 0,
                0           ,          0         , 1;
    
    return _m;
}

// Define the translation matrix T(tx, ty)
Eigen::Matrix3f translation_matrix(float tx, float ty) {
    
    Eigen::Matrix3f _m;

    _m << 1, 0, tx,
          0, 1, ty,
          0, 0,  1;
    
    return _m;
}

int main(){

    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a/b << std::endl;
    std::cout << std::sqrt(b) << std::endl;
    std::cout << std::acos(-1) << std::endl;
    std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl;

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f,2.0f,3.0f);
    Eigen::Vector3f w(1.0f,0.0f,0.0f);
    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    std::cout << v + w << std::endl;
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl;
    std::cout << 2.0f * v << std::endl;

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix3f i,j;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl;
    
    // matrix add i + j
    std::cout << "Example of matrix add (i+j) \n";
    std::cout << i + j << std::endl;
    
    // matrix scalar multiply i * 2.0
    std::cout << "Example of matrix scalar multiply (i*2.0) \n";
    std::cout << i * 2.0f << std::endl;
    
    // matrix multiply i * j
    std::cout << "Example of matrix multiply (i*j) \n";
    std::cout << i * j << std::endl;

    // matrix multiply vector i * v
    std::cout << "Example of matrix multiply vector (i*v) \n";
    std::cout << i * v << std::endl;

    std::cout << "== START OF AS0 ==\n";
    /* 
    * PA 0
    * 1. 给定一个点 P=(2,1), 将该点绕原点先逆时针旋转45◦
    * 2. 再平移(1,2), 计算出变换后点的坐标（要求用齐次坐标进行计算）。
    * 
    * 1. Given a point P=(2,1), rotate the point counterclockwise by 45 degrees around the origin,
    * 2. then translate it by (1,2). Calculate the coordinates of the transformed point (using homogeneous coordinates).
    */

    // Define the point P in homogeneous coordinates
    Eigen::Vector3f P(2.0f, 1.0f, 1.0f); // (x, y, 1)
    
    /*
    *  ALWAYS APPLY IT FROM RIGHT TO LEFT 
    */
    P = translation_matrix(1, 2) * rotation_matrix(45) * P;

    std::cout << "Result:\n" << P << "\n\n" << "Final point is P'=(" << P[0] << "," << P[1] << ")\n";

    std::cout << "== END OF AS0 ==";
    return 0;
}
#include "vectors.hpp"
#include <stdexcept>
#include <iostream>


/*************************************************************************************************/
/*                                                                                               */
/*                                           Vec3                                                */
/*                                                                                               */
/*                                                                                               */
/*************************************************************************************************/

Vec3 Vec3::operator+(const Vec3 &other) {
    
    Vec3 result(this->x,this->y,this->z);
    
    result.x += other.x;
    result.y += other.y;
    result.z += other.z;

    return result;

};

Vec3 Vec3::operator-(const Vec3 &other) {
    
    Vec3 result(this->x,this->y,this->z);
    
    result.x -= other.x;
    result.y -= other.y;
    result.z -= other.z;

    return result;

};

Vec3 Vec3::operator*(const double &scalar){
    Vec3 result(this->x,this->y,this->z);
    
    result.x *= scalar;
    result.y *= scalar;
    result.z *= scalar;

    return result;
}

double Vec3::operator*(const Vec3 &other) {

    return this->x * other.x + this->y * other.y + this->z * other.z ;

}

void Vec3::operator+=(const Vec3 &other) {
    
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

};

void Vec3::operator-=(const Vec3 &other) {
        
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;



};

void Vec3::operator*=(const double &scalar){
    
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;

}

Vec3 Vec3::normalize() {
    double n = norm();
    if (n > 1e-15) {
        return Vec3(x / n, y / n, z / n);
    }
    return Vec3(0, 0, 0); // Évite la division par zéro
}

Vec3 Vec3::cross(const Vec3 &other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

void Vec3::to_string() {
    std::cout << "( " << this->x << " , " << this->y << " , " << this->z << " )" << std::endl;
}





/*************************************************************************************************/
/*                                                                                               */
/*                                           Vec4                                                */
/*                                                                                               */
/*                                                                                               */
/*************************************************************************************************/




Vec4 Vec4::operator+(const Vec4 &other) {
    
    Vec4 result(this->x,this->y,this->z, this->t);
    
    result.x += other.x;
    result.y += other.y;
    result.z += other.z;
    result.t += other.t;


    return result;

};

Vec4 Vec4::operator-(const Vec4 &other) {
    
    Vec4 result(this->x,this->y,this->z, this->t);
    
    result.x -= other.x;
    result.y -= other.y;
    result.z -= other.z;
    result.t -= other.t;


    return result;

};

Vec4 Vec4::operator*(const double &scalar){
    Vec4 result(this->x,this->y,this->z, this->t);
    
    result.x *= scalar;
    result.y *= scalar;
    result.z *= scalar;
    result.t *= scalar;

    return result;
}

double Vec4::operator*(const Vec4 &other) {

    return this->x * other.x + this->y * other.y + this->z * other.z + this->t * other.t;

}

void Vec4::operator+=(const Vec4 &other) {
    
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    this->t += other.t;

};

void Vec4::operator-=(const Vec4 &other) {
        
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    this->t -= other.t;

};

void Vec4::operator*=(const double &scalar){
    
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    this->t *= scalar;

}

double Vec4::operator[](const int indx) {
    switch (indx)
    {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    case 3: return t;
    
    default:
        throw std::invalid_argument( "received shitty value" );
    }
}

const double& Vec4::operator[](int indx) const {
    switch (indx) {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    case 3: return t;
    default:
        throw std::invalid_argument( "received shitty value" );
    }
}



void Vec4::to_string() {
    std::cout << "( " << this->x << " , " << this->y << " , " << this->z << " , " << this->t << " )" << std::endl;
}
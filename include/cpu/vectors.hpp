#pragma once

struct Vec3 {
    double x,y,z;
    double &r, &theta, &phi; //aliases

    Vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z), r(x), theta(y), phi(z) {}
    Vec3() : x(0), y(0), z(0), r(x), theta(y), phi(z) {}
    // copy ctor and assignment to keep references valid
    Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z), r(x), theta(y), phi(z) {}
    Vec3& operator=(const Vec3 &other) { x = other.x; y = other.y; z = other.z; return *this; }

    Vec3 operator+(const Vec3 &other);
    Vec3 operator-(const Vec3 &other);
    Vec3 operator*(const double &scalar);
    double operator*(const Vec3 &other); //dot product

    void operator+=(const Vec3 &other);
    void operator-=(const Vec3 &other);
    void operator*=(const double &scalar);

    void to_string();
};


struct Vec4 {
    double x,y,z,t;
    double &r, &theta, &phi;

    Vec4(double _x, double _y, double _z, double _t) : x(_x), y(_y), z(_z), t(_t), r(x), theta(y), phi(z) {}
    Vec4() : x(0), y(0), z(0), t(0), r(x), theta(y), phi(z) {}
    // copy ctor and assignment to keep reference aliases valid
    Vec4(const Vec4 &other) : x(other.x), y(other.y), z(other.z), t(other.t), r(x), theta(y), phi(z) {}
    Vec4& operator=(const Vec4 &other) { x = other.x; y = other.y; z = other.z; t = other.t; return *this; }

    Vec4 operator+(const Vec4 &other);
    Vec4 operator-(const Vec4 &other);
    Vec4 operator*(const double &scalar);
    double operator*(const Vec4 &other); //dot product

    void operator+=(const Vec4 &other);
    void operator-=(const Vec4 &other);
    void operator*=(const double &scalar);
    

    double operator[](const int indx);
    const double& operator[](int indx) const;


    void to_string();
};
#pragma once

#include "vectors.hpp"
#include "blackhole.hpp"

struct PhotonState {
    Vec4 x;
    Vec4 k;

    PhotonState(Vec4 _x, Vec4 _k): x(_x), k(_k) {};

    PhotonState F(const BlackHole& blackHole);
    const double sum(int mu, const BlackHole& blackHole) const;

    PhotonState operator+(const PhotonState &other);
    PhotonState operator-(const PhotonState &other);
    PhotonState operator*(const double &scalar);

    void operator+=(const PhotonState &other);
    void operator-=(const PhotonState &other);
    void operator*=(const double &scalar);

    void to_string();
};

struct Photon {
    PhotonState state;

    Photon(Vec4 _x, Vec4 _k) : state(_x, _k) {};
    Photon() : state(Vec4(), Vec4()) {};

    void RK4step( const BlackHole& blackHole, double h, bool debug = false);

};


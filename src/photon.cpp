#include "photon.hpp"
#include <iostream>
#include <cmath>

void Photon::RK4step( const BlackHole& blackHole, double h) {

    // calcul des ki
    PhotonState k1 = this->state.F(blackHole);
    PhotonState k2 = (this->state + k1*(h/2)).F(blackHole);
    PhotonState k3 = (this->state + k2*(h/2)).F(blackHole);
    PhotonState k4 = (this->state + k3*h).F(blackHole);

    PhotonState delta = ((k1 + k2*2 + k3*2 + k4) * (h/6));

    //compute deltas on eache composant
    double nx = this->state.x.x + delta.x.x;
    double ny = this->state.x.y + delta.x.y;
    double nz = this->state.x.z + delta.x.z;
    double nt = this->state.x.t + delta.x.t;

    double nkx = this->state.k.x + delta.k.x;
    double nky = this->state.k.y + delta.k.y;
    double nkz = this->state.k.z + delta.k.z;
    double nkt = this->state.k.t + delta.k.t;

    //check if any is nan
    bool anyNan = std::isnan(nx) || std::isnan(ny) || std::isnan(nz) || std::isnan(nt)
               || std::isnan(nkx) || std::isnan(nky) || std::isnan(nkz) || std::isnan(nkt);

    // apply delta
    this->state += delta;

    (void)anyNan; // caller can detect NaNs in state afterwards

}


/*************************************************************************************************/
/*                                                                                               */
/*                                    PhotonState                                                */
/*                                                                                               */
/*                                                                                               */
/*************************************************************************************************/



// deprecated
// const double PhotonState::sum(int mu, const BlackHole &blackHole) const {
//     double ret = 0;
//     for(int a=0; a<4; ++a){
//         for(int b=0; b<4; ++b){
//             // if(mu==2){
//             //     std::cout << "alpha : " << a << " beta " << b << " cristofel " << blackHole.christoffel(mu,a,b,this->x) * this->k[a] * this->k[b] << std::endl;
//             // }
//             ret += blackHole.christoffel(mu,a,b,this->x) * this->k[a] * this->k[b];
//         }
//     }
//     return ret;

// }


//fonction principale F pour calculer les ki en fonction de l'etat précédent
PhotonState PhotonState::F(const BlackHole& blackHole) {

    double r = this->x.r; // x.x
    double th = this->x.theta; // x.y
    double M = blackHole.mass;
    
    // pré-calculs 
    double r2 = r * r;
    double sinTh = sin(th);
    double cosTh = cos(th);
    double inv_r_minus_2M = 1.0 / (r - 2.0 * M);
    double f = 1.0 - (2.0 * M / r); // Facteur de Schwarzschild

    // les composantes de k^mu
    double kr = this->k.r;
    double kth = this->k.theta;
    double kph = this->k.phi;
    double kt = this->k.t;

    // ensuite calcul des acceleration (cristoffel * k^ak ^b)

    // r
    double Gamma_r = - ( 
                    blackHole.christoffel(0,0,0,x) * k.r * k.r +
                    blackHole.christoffel(0,1,1,x) * k.theta * k.theta +
                    blackHole.christoffel(0,2,2,x) * k.phi * k.phi +
                    blackHole.christoffel(0,3,3,x) * k.t * k.t
                );
    // theta
    double Gamma_th = - ( 2.0 * blackHole.christoffel(1,0,1,x) * k.r * k.theta + blackHole.christoffel(1,2,2,x) * k.phi * k.phi );

    //  phi
    
    double Gamma_ph = - ( 2.0 * blackHole.christoffel(2,0,2,x) * k.r * k.phi +  2.0 * blackHole.christoffel(2,1,2,x) * k.theta * k.phi );

    // t
    double Gamma_t = - ( 2.0 * blackHole.christoffel(3,0,3,x) * k.r * k.t );

    // nouvel etat
    return PhotonState(
        Vec4(kr, kth, kph, kt),        // dx/dlambda = k
        Vec4(Gamma_r, Gamma_th, Gamma_ph, Gamma_t) // dk/dlambda = -Gamma*k*k
    );

}




PhotonState PhotonState::operator+(const PhotonState &other) {
    
    PhotonState result(this->x, this->k);
    
    result.x += other.x;
    result.k += other.k;


    return result;

};

PhotonState PhotonState::operator-(const PhotonState &other) {
    
    PhotonState result(this->x,this->k);
    
    result.x -= other.x;
    result.k -= other.k;

    return result;

};

PhotonState PhotonState::operator*(const double &scalar){
    PhotonState result(this->x,this->k);
    
    result.x *= scalar;
    result.k *= scalar;

    return result;
}

void PhotonState::operator+=(const PhotonState &other) {
    
    this->x += other.x;
    this->k += other.k;

};

void PhotonState::operator-=(const PhotonState &other) {
    
    this->x -= other.x;
    this->k -= other.k;

};

void PhotonState::operator*=(const double &scalar){
    
    this->x *= scalar;
    this->k *= scalar;

}


void PhotonState::to_string() {
    std::cout << "x : "; this->x.to_string(); std::cout << "k : "; this->k.to_string();
}
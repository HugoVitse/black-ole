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

    double x_c = this->x.x; 
    double y_c = this->x.y;
    double z_c = this->x.z;
    
    double kx = this->k.x;
    double ky = this->k.y;
    double kz = this->k.z;
    double kt = this->k.t;

    double M = blackHole.mass;
    
    // 2. Calcul du rayon et du facteur de Schwarzschild
    double r2 = x_c*x_c + y_c*y_c + z_c*z_c;
    double r  = sqrt(r2);
    double f  = 1.0 - (2.0 * M / r); 
    
    // r_dot est la vitesse de variation du rayon (dr/dlambda)
    // C'est le produit scalaire (Position . Vitesse) / r
    double r_dot = (x_c * kx + y_c * ky + z_c * kz) / r;

    // 3. Calcul des accélérations (dk/dlambda)
    // Ces formules remplacent tous tes anciens calculs de Christoffel
    
    // Facteur commun lié à la courbure
    double A = (M / (r2 * r)) * ( f * kt * kt - (r_dot * r_dot / f) );
    // Facteur lié à la direction du mouvement
    double B = (2.0 * M * r_dot) / (r * r * f);

    // Accélération spatiale (dkx, dky, dkz)
    // Remplace Gamma_r, Gamma_th, Gamma_ph
    double dkx = -x_c * A + kx * B;
    double dky = -y_c * A + ky * B;
    double dkz = -z_c * A + kz * B;

    // Accélération temporelle (dkt)
    // Remplace Gamma_t
    double dkt = -kt * B;

    // 4. Retourne l'état dérivé pour le RK4
    return PhotonState(
        Vec4(kx, ky, kz, kt),          // dx/dlambda = k
        Vec4(dkx, dky, dkz, dkt)       // dk/dlambda = accélération
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
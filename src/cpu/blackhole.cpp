// #include "blackhole.hpp"
// #include <math.h>

// const inline double BlackHole::christoffel(int mu, int alpha, int beta, const Vec4& x) const{

//     switch(mu) {
//         case 3: //t
//         switch(alpha) {
//             case 3: //t
//             switch(beta){
//                 case 0: //r -> ttr
//                 return this->mass / (x.r * (x.r - 2*this->mass));
//                 default:
//                 return 0;
//             }
//             case 0: //r
//             switch(beta){
//                 case 3: //t -> trt
//                 return this->mass / (x.r * (x.r - 2*this->mass));
//                 default:
//                 return 0;
//             }
//             default:
//             return 0;

//         }
        
//         case 0: //r
//         switch(alpha) {
//             case 3: //t
//             switch(beta){
//                 case 3: //t -> rtt
//                 return (this->mass *(x.r - 2* this->mass)) / (pow(x.r,3));
//                 default:
//                 return 0;
//             }
//             case 1: // theta
//             switch(beta){
//                 case 1: //theta -> r theta theta
//                 return - (x.r - (2*this->mass));
//                 default:
//                 return 0;
//             }
//             case 2: // phi
//             switch(beta){
//                 case 2: //phi -> r phi phi
//                 return -1*(x.r - (2*this->mass))*pow(sin(x.theta),2);
//                 default:
//                 return 0;
//             }
//             case 0: //r
//             switch(beta){
//                 case 0: //r -> rrr
//                 return - (this->mass / (x.r * (x.r - 2*this->mass)));
//                 default:
//                 return 0;
//             }
//             default:
//             return 0;

//         }

//         case 1: //theta
//         switch(alpha) {
//             case 0: //r
//             switch(beta){
//                 case 1: //theta -> theta r theta
//                 return 1 / (x.r);
//                 default:
//                 return 0;
//             }
//             case 1: //theta
//             switch(beta){
//                 case 0: //r -> theta theta r
//                 return 1 / (x.r);
//                 default:
//                 return 0;   
//             }
//             case 2: //phi
//             switch(beta){
//                 case 2: //phi -> theta phi phi
//                 return -1 * sin(x.theta)*cos(x.theta);
//                 default:
//                 return 0;
//             }
//             default:
//             return 0;

//         }

//         case 2: //phi
//         switch(alpha) {
//             case 0: //r
//             switch(beta){
//                 case 2: //phi -> phi r phi
//                 return 1 / (x.r);
//                 default:
//                 return 0;
//             }
            
//             case 1: //theta
//             switch(beta){
//                 case 2: //phi -> phi theta phi
//                 return cos(x.theta)/sin(x.theta);
//                 default:
//                 return 0;
//             }

//             case 2: //phi
//             switch(beta){
//                 case 0: //r -> phi phi r
//                 return 1 / (x.r);
//                 case 1: //theta -> phi phi theta
//                 return cos(x.theta)/sin(x.theta);
//                 default:
//                 return 0;
//             }
//             default:
//             return 0;

//         }


//         default:
//         return 0;
//     }
// }

/**
 *@file Pose.cc
 *@brief 
 */
#include "Pose.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

namespace BVL {
    using namespace std;

    ostream &operator<<(ostream &os, const RotationRep &r)
    {
        os << "format cache:"<<r.format_cache<<"\n";
        os << "Euler angles:"<<r.euler * 180./M_PI<<"\n";
        os << "Quaternion: " <<r.quaternion << "\n";
        os << "Matrix:\n" <<r.matrix ;
        return os;
    }

    ostream &operator<<(ostream &os, const Pose &p) 
    {
        os<<"Translation: "<<p.translation<<"\n";
        os <<*(p.rotation);
        return os;
    }
}



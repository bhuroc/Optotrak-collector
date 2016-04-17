#ifndef _POSE_H_
#define _POSE_H_

/**
 *@file Pose.h
 *@brief Different representations of the 6D pose.
 */
#include "src/math/linalg/linalg.h"
#include "src/motion/rigid_body/rigid_body.h"
#include <assert.h>
#include <iostream>
#include <stdexcept>

namespace BVL {

    struct RotationRep {
        enum {
            EULER_SET=0x1,
            QUATERNION_SET=0x2,
            MATRIX_SET=0x4
        };

        RotationRep():format_cache(0),euler(3,0.),quaternion(4,0.),matrix(3,3,0.) {
            quaternion[0]=1.;
            matrix(1,1) = 1.;
            matrix(2,2) = 1.;
            matrix(3,3) = 1.;
        }

        // When A_to_B is called, A is guarantted to be valid
        // E.g., euler_to_matrix implies EULER_SET is true.
        void euler_to_matrix() {
            matrix = euler2matrix(euler);
            format_cache |= MATRIX_SET;
        }

        void euler_to_quaternion() {
            if(format_cache & MATRIX_SET) {
                quaternion = matrix2quaternion(matrix);
                format_cache |= QUATERNION_SET;
            }else  {
                matrix = euler2matrix(euler);
                format_cache |= MATRIX_SET;
                quaternion = matrix2quaternion(matrix);
                format_cache |= QUATERNION_SET;
            }
        }

        void matrix_to_euler() {
            euler = matrix2euler(matrix);
            format_cache |= EULER_SET;
        }

        void matrix_to_quaternion() {
            quaternion = matrix2quaternion(matrix);
            format_cache |= QUATERNION_SET;
        }

        void quaternion_to_euler() {
            matrix = quaternion2matrix(quaternion);
            format_cache |= MATRIX_SET;
            euler = matrix2euler(matrix);
            format_cache |= EULER_SET;
        }

        void quaternion_to_matrix() {
            //std::cerr <<"quater2matrix "<<typeid(*this).name() <<"\n";
            matrix = quaternion2matrix(quaternion);
            format_cache |= MATRIX_SET;
        }

        bool euler_set() const {
            return (format_cache & EULER_SET) != 0;
        }

        bool quaternion_set() const {
            return (format_cache & QUATERNION_SET) != 0;
        }

        bool matrix_set() const {
            return (format_cache & MATRIX_SET) != 0;
        }
        
        void get_euler() {
            if(!euler_set()) {
                if(matrix_set()) {
                    matrix_to_euler();
                }else
                    quaternion_to_euler();
            }
        }

        void get_quaternion() {
            if(!quaternion_set()) {
                if(matrix_set()) {
                    matrix_to_quaternion();
                }else {
                    euler_to_quaternion();
                }
            }
        }

        void get_matrix() {
            if(!matrix_set()) {
                if(euler_set()) {
                    euler_to_matrix();
                }else {
                    quaternion_to_matrix();
                }
            }
        }

        void set_euler(double rx, double ry, double rz) {
            euler(1) = rx;
            euler(2) = ry;
            euler(3) = rz;
            // not |=, because we need to invalidate the cache
            format_cache = EULER_SET;
        }

        void set_quaternion(double q0, double q1, double q2, double q3) {
            quaternion(1) = q0;
            quaternion(2) = q1;
            quaternion(3) = q2;
            quaternion(4) = q3;
            format_cache = QUATERNION_SET;
        }

        void set_matrix(const Matrix<double> &r) {
            assert(r.dim1() == 3 && r.dim2() == 3);
            matrix = r;
            format_cache = MATRIX_SET;
        }

        unsigned int format_cache;
        Vector<double> euler;
        Vector<double> quaternion;
        Matrix<double> matrix;

    };

    std::ostream &operator<<(std::ostream &os, const RotationRep &);

    class Pose {
        friend std::ostream &operator<<(std::ostream &os, const Pose &);
        public:

            Pose() : translation(3,0.) {
                rotation = new RotationRep;
            }

            Pose(const Vector<double> &r, const Vector<double> &t) {
                rotation = new RotationRep;
                if(r.dim() == 3)
                    rotation->set_euler(r(1),r(2),r(3));
                else if(r.dim() == 4) 
                    rotation->set_quaternion(r(1),r(2),r(3), r(4));
                else 
                    throw std::logic_error("Using wrong-sized vector to initialize pose.");
                translation = t;
            }

            Pose(const Matrix<double> &r, const Vector<double> &t) {
                rotation = new RotationRep;
                if(r.dim1() == 3 && r.dim2() == 3) {
                    rotation->set_matrix(r);
                }else 
                    throw std::logic_error("Using wrong-sized matrix to initialize pose.");
                translation = t;

            }

            Pose(const Pose &p) {
                rotation = new RotationRep;
                translation = p.translation;
                // NOTE rotation's generated = works fine.
                *rotation = *(p.rotation);
            }

            virtual ~Pose() {
                delete rotation;
            }

            Pose &operator=(const Pose &p) {
                if(this == &p) 
                    return *this;
                translation = p.translation;
                *rotation = *(p.rotation);
                return *this;
            }

            void set_translation(double x, double y, double z) {
                translation[0] = x;
                translation[1] = y;
                translation[2] = z;
            }

            void set_translation(const Vector<double> &t) {
                translation = t;
            }

            Vector<double> get_translation() const {
                return translation;
            }

            void get_translation(double &x, double &y, double &z) const {
                x = translation[0];
                y = translation[1];
                z = translation[2];
            }

            Matrix<double> get_rotation_matrix() const {
                rotation->get_matrix();
                return rotation->matrix;
            }

            void get_rotation_matrix(Matrix<double> &r) const {
                rotation->get_matrix();
                r = rotation->matrix;
            }

            Matrix<double> get_homogeneous_matrix() const {
                Matrix<double> r=this->get_rotation_matrix();
                Matrix<double> h(4,4,0.);
                for(int i=1;i<=3;++i)
                    for(int j=1; j<=3; ++j) 
                        h(i,j) = r(i,j);
                h(1,4) = translation[0];
                h(2,4) = translation[1];
                h(3,4) = translation[2];
                h(4,4) = 1.;

                return h;
            }

            Vector<double> get_euler_angles() const {
                rotation->get_euler();
                return rotation->euler;
            }

            void get_euler_angles(double &rx, double &ry, double &rz) const {
                rotation->get_euler();
                rx = rotation->euler(1);
                ry = rotation->euler(2);
                rz = rotation->euler(3);
            }

            Vector<double> get_quaternion() const {
                rotation->get_quaternion();
                return rotation->quaternion;
            }

            void get_quaternion(double &q0, double &q1, double &q2, double &q3) const {
                rotation->get_quaternion();
                q0 = rotation->quaternion(1);
                q1 = rotation->quaternion(2);
                q2 = rotation->quaternion(3);
                q3 = rotation->quaternion(4);
            }

            void set_rotation(double rx, double ry, double rz) {
                rotation->set_euler(rx,ry,rz);
            }

            void set_rotation(double q0, double q1, double q2, double q3) {
                rotation->set_quaternion(q0,q1,q2,q3);
            }

            void set_rotation(const Matrix<double> &r) {
                rotation->set_matrix(r);
            };


        protected:
            Vector<double> translation;
            RotationRep *rotation;
    };

    std::ostream &operator<<(std::ostream &os, const Pose &);

}

#endif/*_POSE_H_*/


#ifndef QUAT_H
#define QUAT_H

#include "tVector.h"
#include "tMatrix.h"

#define PIOVER180 0.0174532925199

class tQuat
{
friend tQuat operator*(const tQuat&, const tQuat&);
friend tQuat operator/(tQuat&, double);
friend tVecf operator*(const tQuat&, const tVecf&);
friend void Print(tQuat);
protected:
	double m_real;
	tVecf m_vec;
	tQuat conjugate() const;
	double magnitude() const;
public:
    tQuat()
    {
        m_vec = tVecf(0.0f, 0.0f, 0.0f);
        m_real = 1.0f;
    }
	tQuat(double, tVecf); // create one out of an angle and a vector
	tQuat inverse() const;
	void normalize();
	tMat4 convertToMatrix();
};

#endif

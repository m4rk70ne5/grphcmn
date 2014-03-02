#include "tQuat.h"

tQuat::tQuat(double angle, tVecf axis)
{
	m_real = cos(angle * 0.5f);
	m_vec = axis * sin(angle * 0.5f);
}

double tQuat::magnitude() const
{
	double sum = m_real * m_real + m_vec.x * m_vec.x + m_vec.y * m_vec.y + m_vec.z * m_vec.z;
	return sqrt(sum);
}

tQuat tQuat::conjugate() const
{
    tQuat t;
    t.m_real = m_real;
    t.m_vec = m_vec * -1.0f;
	return t;
}

tQuat tQuat::inverse() const
{   tQuat t = conjugate();
	return t / magnitude();
}

void tQuat::normalize()
{
	double val = magnitude();
	m_real /= val;
	m_vec.x /= val;
	m_vec.y /= val;
	m_vec.z /= val;
}

tQuat operator*(const tQuat& op1, const tQuat& op2)
{
    // concatenate two "rotations" (rotate vec (or other quat) by op2 first then op1 after)
    tQuat result;
	// double realPart = (op1.m_real * op2.m_real) - tVecf::dot(const_cast<tQuat*>(&op1)->m_vec, const_cast<tQuat*>(&op2)->m_vec);
	// tVecf vecPart = op2.m_vec * op1.m_real + op1.m_vec * op2.m_real + tVecf::cross(op1.m_vec, op2.m_vec);
	// result.m_real = realPart;
	// result.m_vec = vecPart;
	result.m_real = op1.m_real * op2.m_real - op1.m_vec.x * op2.m_vec.x - op1.m_vec.y * op2.m_vec.y - op1.m_vec.z * op2.m_vec.z; 
	result.m_vec.x = op1.m_real * op2.m_vec.x + op1.m_vec.x * op2.m_real + op1.m_vec.y * op2.m_vec.z - op1.m_vec.z * op2.m_vec.y;
	result.m_vec.y = op1.m_real * op2.m_vec.y + op1.m_vec.y * op2.m_real + op1.m_vec.z * op2.m_vec.x - op1.m_vec.x * op2.m_vec.z;
	result.m_vec.z = op1.m_real * op2.m_vec.z + op1.m_vec.z * op2.m_real + op1.m_vec.x * op2.m_vec.y - op1.m_vec.y * op2.m_vec.x;
	return result;
}

tQuat operator/(tQuat& op1, double dividend)
{
    tQuat t;
    t.m_real = op1.m_real / dividend;
    t.m_vec = op1.m_vec / dividend;
	return t;
}

tVecf operator*(const tQuat& op1, const tVecf& vec)
{
	// q p q-1
	tQuat p;
	p.m_vec = vec;
	tQuat product = op1 * (p * op1.inverse());
	return product.m_vec;
}

tMat4 tQuat::convertToMatrix()
{
    tMat4 t;
    double& w = m_real;
    float& x = m_vec.x;
    float& y = m_vec.y;
    float& z = m_vec.z;

    t[0][0] = 1 - 2*(y*y + z*z);
    t[0][1] = 2*(x*y + w*z);
    t[0][2] = 2*(x*z - w*y);
    t[1][0] = 2*(x*y - w*z);
    t[1][1] = 1 - 2*(x*x + z*z);
    t[1][2] = 2*(y*z + w*x);
    t[2][0] = 2*(x*z + w*y);
    t[2][1] = 2*(y*z - w*x);
    t[2][2] = 1 - 2*(x*x + y*y);

    return t;
}

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

template<typename T>
class tVec
{
    protected:
        T* m_values[4];
        void SetValues()
        {
           m_values[0] = &x;
           m_values[1] = &y;
           m_values[2] = &z;
           m_values[3] = &w;
        };
    public:
        T x, y, z, w;
        // all operators go here
        tVec()
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
            SetValues();
        };
        tVec(T xx, T yy) : x(xx), y(yy)
        {
            z = 0;
            w = 0;
            SetValues();
        };
        tVec(T xx, T yy, T zz) : x(xx), y(yy), z(zz)
        {
            w = 0;
            SetValues();
        };
        tVec(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww)
        {
            SetValues();
        };
        tVec(const tVec<T>& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            SetValues();
        };
        tVec<T>& operator=(const tVec<T>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;
            SetValues();
            return *this;
        };
        T& operator[] (int i)
        {
            return *(m_values[i]);
        };
        tVec<T>& operator+=(const tVec<T>& op)
        {
            // in the future, make addition able to take place between vectors of different dimensions
            x += op.x;
            y += op.y;
            z += op.z;
            w += op.w;
            return *this;
        };
        tVec<T>& operator-=(const tVec<T>& op)
        {
            // in the future, make addition able to take place between vectors of different dimensions
            x -= op.x;
            y -= op.y;
            z -= op.z;
            w -= op.w;
            return *this;
        };
        template <typename U>
        tVec<T>& operator*=(U scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        };
        template <typename U>
        tVec<T>& operator/=(U scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        };
        void normalize()
        {
            T mag = GetMagnitude(this);
            *this /= mag;
        };
        static double dot(tVec<T>& op1, tVec<T>& op2)
        {
            double sum = 0.0f;
            for (int i = 0; i < 4; i++)
            {
                sum += op1[i] * op2[i];
            }
            return sum;
        };
        static tVec<T> cross(const tVec<T>& op1, const tVec<T>& op2)
        {
            tVec<T> t;
            t.x = op1.y * op2.z - op1.z * op2.y;
            t.y = op1.z * op2.x - op1.x * op2.z;
            t.z = op1.x * op2.y - op1.y * op2.x;
            return t;
        }
        static T GetMagnitude(tVec<T>* vec)
        {
            T sum = 0;
            for (int i = 0; i < 4; i++)
            {
                sum = sum + ((*vec)[i] * (*vec)[i]);
            }
            return sqrt(sum);
        };
		void GetData(T(&convertData)[4])
		{
			for (int i = 0; i < 4; i++)
			{
				convertData[i] = *m_values[i];
			}
		};
};

template<typename T>
tVec<T> operator+(const tVec<T>& op1, const tVec<T>& op2)
{
    tVec<T> t(op1);
    t += op2;
    return t;
}

template<typename T>
tVec<T> operator-(const tVec<T>& op1, const tVec<T>& op2)
{
    tVec<T> t(op1);
    t -= op2;
    return t;
}

template <typename T, typename U>
tVec<T> operator*(const tVec<T>& op1, U scalar)
{
    tVec<T> t(op1);
    t *= scalar;
    return t;
}

template <typename T, typename U>
tVec<T> operator/(const tVec<T>& op1, U scalar)
{
    tVec<T> t(op1);
    t /= scalar;
    return t;
}

typedef tVec<float> tVecf;

#endif

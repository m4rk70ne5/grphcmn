#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include "tVector.h"

template<typename T, int R, int C>
class tMat
{
protected:
	T m_data[R][C];
public:
	// Proxy class is used for [][]
	class Proxy
	{
	protected:
		T* m_data;
	public:
		Proxy(T* dat) : m_data(dat) { }
		T& operator[](int index)
		{
			return m_data[index];
		}
	};
	Proxy operator[](int i)
	{
		return Proxy(&m_data[i][0]);
	};
	tMat<T, R, C>()
	{
	    // create an identity matrix
	    for (int i = 0; i < R; i++)
	    {
	        for (int j = 0; j < C; j++)
	        {
	            m_data[i][j] = (i == j);
	        }
	    }
	}
	T* GetData() { return &m_data[0][0]; };
};

template<typename T, int R, int C, int N>
tMat<T, R, C> operator*(tMat<T, R, N>& op1, tMat<T, N, C>& op2)
{
    tMat<T, R, C> product;

    // row by column multiplication
    for (int i = 0; i < R; i++)
    {
        // C = number of multiplier columns
        for (int j = 0; j < C; j++)
        {
            T sum = 0.0f;
            // N = number of multiplicand rows
            for (int k = 0; k < N; k++)
            {
                sum += op1[i][k] * op2[k][j];
            }
            product[i][j] = sum;
        }

    }
    return product;
}

template<typename T, int R, int C>
tVec<T> operator*(tMat<T, R, C>& op1, tVec<T>& op2)
{
    // convert
    tMat<T, 4, 1> prod;
    prod[0][0] = op2.x;
    prod[1][0] = op2.y;
    prod[2][0] = op2.z;
    prod[3][0] = op2.w;

    // multiply
    prod = op1 * prod;

    //re-convert
    tVec<T> vecprod;
    vecprod.x = prod[0][0];
    vecprod.y = prod[1][0];
    vecprod.z = prod[2][0];
    vecprod.w = prod[3][0];

    return vecprod;
}

typedef tMat<float, 4, 4> tMat4;

#endif

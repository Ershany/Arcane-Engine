#pragma once
#ifndef ARRAY2D_H
#define ARRAY2D_H

#include "arcpch.h"

namespace Arcane
{
	template<typename Type>
	class Array2D
	{
	public:
		Array2D(int cols, int rows) : m_Cols(cols), m_Rows(rows)
		{
			Init();
		}

		~Array2D()
		{
			if (m_Data)
			{
				free(m_Data);
				m_Data = nullptr;
			}
		}

		Array2D(const Array2D &other)
		{
			m_Cols = other.m_Cols;
			m_Rows = other.m_Rows;
			Init();

			memcpy(m_Data, other.m_Data, m_Rows * m_Cols * sizeof(Type))
		}

		Array2D(Array2D &&other)
		{
			m_Cols = other.m_Cols;
			m_Rows = other.m_Rows;
			m_Data = other.m_Data;

			other.m_Data = nullptr;
			other.m_Cols = 0;
			other.m_Rows = 0;
		}

		void Init()
		{
			if (m_Data)
			{
				free (m_Data);
			}
			m_Data = (Type*)malloc(m_Rows * m_Cols * sizeof(Type));
		}

		void Init(Type initValue)
		{
			Init();
			memset(m_Data, initValue, m_Rows * m_Cols);
		}

		inline Type& At(int col, int row)
		{
			return m_Data[CalculateIndex(col, row)];
		}

		inline Type& At(int index)
		{
#ifdef ARC_DEV_BUILD
			if (index >= m_Rows * m_Cols)
			{
				ARC_LOG_ERROR("Array2D At index {0} exceeds the size of the array: {1}", index, m_Rows * m_Cols);
			}
#endif
			return m_Data[index];
		}

		inline void Set(int col, int row, const Type &val)
		{
			m_Data[CalculateIndex(col, row)] = val;
		}

		inline void Set(int index, const Type &val)
		{
#ifdef ARC_DEV_BUILD
			if (index >= m_Rows * m_Cols)
			{
				ARC_LOG_ERROR("Array2D Set at index {0} exceeds the size of the array: {1}", index, m_Rows * m_Cols);
			}
#endif
			m_Data[index] = val;
		}

		inline Type* GetDataAddress(int col = 0, int row = 0)
		{
			return &m_Data[CalculateIndex(col, row)];
		}

		inline GetSize() const
		{
			return m_Rows * m_Cols;
		}

		inline GetSizeInBytes() const
		{
			return GetSize() * sizeof(Type);
		}

		void GetMinMax(Type &minOut, Type &maxOut)
		{
			minOut = maxOut = this->At(0);
			for (int i = 1; i < this->GetSize(); i++)
			{
				if (m_Data[i] < minOut)
				{
					minOut = m_Data[i];
				}
				if (m_Data[i] > maxOut)
				{
					maxOut = m_Data[i];
				}
			}
		}

		void Normalize(Type minRange, Type maxRange)
		{
			Type min, max;
			GetMinMax(min, max);

			if (Max <= Min)
			{
				return;
			}

			Type minMaxDelta = max - min;
			Type minMaxRange = maxRange - minRange;

			for (int i = 0; i < m_Rows * m_Cols; i++)
			{
				m_Data[i] = ((m_Data[i] - min) / minMaxDelta) * minMaxRange + minRange;
			}
		}


#ifdef ARC_DEV_BUILD
		void PrintFloat()
		{
			for (int y = 0; y < m_Rows; y++)
			{
				printf("%d: ", y);
				for (int x = 0; x < m_Cols; x++)
				{
					float f = (float)m_Data[y * m_Cols + x];
					printf("%.8f ", f);
				}
				printf("\n");
			}
		}
#endif
	private:
		inline size_t CalculateIndex(int col, int row)
		{
#ifdef ARC_DEV_BUILD
			if (col < 0)
			{
				ARC_LOG_ERROR("Can't calculate 2D index with negative column index: {0}", col);
				return 0;
			}
			else if (col >= m_Cols)
			{
				ARC_LOG_ERROR("Can't calculate 2D index with column overflow value: {0} - when column size is {1}", col, m_Cols);
				return 0;
			}

			if (row < 0)
			{
				ARC_LOG_ERROR("Can't calculate 2D index with negative row index: {0}", row);
				return 0;
			}
			else if (row >= m_Rows)
			{
				ARC_LOG_ERROR("Can't calculate 2D index with row overflow value: {0} - when row size is {1}", row, m_Rows);
				return 0;
			}
#endif
			return (size_t)(row * m_Cols + col);
		}
	private:
		int m_Rows = 0, m_Cols = 0;
		Type *m_Data = nullptr;
	};
}

#endif

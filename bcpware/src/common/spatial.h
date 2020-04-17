#pragma once
#ifndef SPATIAL_H
#define SPATIAL_H
#include <cstdlib> 
#include <ctime>   
#include <functional>
#include <chrono>
#include <random>
#include <QMap>
#include "meshmodel.h"
#include "Result.h"

//#include "binPacking.h"

//class ResultP;

namespace BINPACK{
	template <class Scalartype>
	bool LessThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b);

	template <class Scalartype>
	bool GreaterThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b);



	struct SpatialKey
	{
		int x, y, z;
		SpatialKey()
		{
			x = 0;
			y = 0;
			z = 0;
		}
		SpatialKey(int _x, int _y, int _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		bool const operator==(const SpatialKey &o)
		{
			return x == o.x && y == o.y && z == o.z;
		}
		
		bool const operator<=(const SpatialKey &o)
		{
			return x <= o.x && y <= o.y && z <= o.z;
		}
		/*bool const operator <(const SpatialKey &o)
		{
			return x < o.x && y < o.y && z < o.z;
		}*/

		friend bool operator< (SpatialKey const& a, SpatialKey const& o)
		{
			return a.x < o.x && a.y < o.y && a.z < o.z;
		}

		
		


	};

	template<class placement>
	struct SpatialValue
	{
		Point3f minn, maxx;
		Result<placement> result;
		SpatialValue()
		{
			minn.Zero();
			maxx.Zero();
			result = *new Result<placement>();
		}
		SpatialValue(Point3f _minn, Point3f _maxx, Result<placement> _result)
		{
			minn = _minn;
			maxx = _maxx;
			result = _result;
		}
	};

	template<class placement>
	class SpatialHash
	{
	public:
		int cellSize;
		SpatialHash(){}
		QMap<SpatialKey, QVector<SpatialValue<placement>>> cells;

		SpatialHash(int _cellSize, QMap<SpatialKey, QVector<SpatialValue<placement>>> _cells)
		{
			cellSize = _cellSize;
			cells = _cells;
		}

		SpatialKey keyForVector(Point3f v)
		{
			double x = v.X() / cellSize;
			double y = v.Y() / cellSize;
			double z = v.Z() / cellSize;
			return *new SpatialKey(x, y, z);
		}
		void add(Point3f minn, Point3f maxx, Result<placement> result)
		{
			SpatialValue<placement> value(minn, maxx, result);// , value2(Point3f(8700, 7800, 8700), Point3f(54000, 45000, 600005), &result);


			QVector<SpatialValue<placement>> tempcells, tempcells2;

			SpatialKey k1 = keyForVector(minn);
			SpatialKey k2 = keyForVector(maxx);


		/*	tempcells.push_back(value);
			tempcells2.push_back(value2);*/

		/*	SpatialKey ktt(3, 4, 5);
			SpatialKey ktt2(12, 1, 5);
			cells.insert(ktt,tempcells);
			cells.insert(ktt2, tempcells2);*/

			//QVector<SpatialValue<placement>> vvall = cells.value(ktt);

			for (int x = k1.x; x <= k2.x; x++)
			for (int y = k1.y; y <= k2.y; y++)
			for (int z = k1.z; z <= k2.z; z++)
			{
				SpatialKey k(x, y, z);
				//tempcells.push_back(value);
				//cells.insert(k, tempcells);
				//cells.insert(k, value);
				cells[k].push_back(value);


				//cells.insert(,value);
			}
		}
		bool get(Point3f v, Result<placement> &re)
		{
			SpatialKey k = keyForVector(v);
			for (int i = 0; i < cells[k].size(); i++)
			{
				SpatialValue<placement> value = cells[k].at(i);
				if (GreaterThanOrEqual(v, value.minn) && LessThanOrEqual(v, value.maxx))
				{
					re = value.result;
					return true;
				}
			}
			//re = new Result<placement>();
			return false;
		}
	};

	template<class placement>
	static SpatialHash<placement>* NewSpatialHash(int cellSize)
	{
		QMap<SpatialKey, QVector<SpatialValue<placement>>> cells;
		return (new SpatialHash<placement>(cellSize, cells));
	}




}





//static 
#endif
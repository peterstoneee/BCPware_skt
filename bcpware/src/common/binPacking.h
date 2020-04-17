#pragma once
#ifndef BINPACKING_H
#define BINPACKING_H

#include <cstdlib> 
#include <ctime>   
#include <functional>
#include <chrono>
#include <random>
#include "meshmodel.h"
#include "spatial.h"
#include "Result.h"
#include "interfaces.h"
//#include "NodeBVH.h"


namespace BINPACK{


	class BinBox;
	class Item;
	struct Placement;

	/*template <class Scalartype>
	Point3<Scalartype> p3Sort(Point3<Scalartype> &in);*/

	template <class Scalartype>
	bool p3Fits(Point3<Scalartype> in_a, Point3<Scalartype> in_b);

	template <class Scalartype>
	Point3<Scalartype> SortP3(Point3<Scalartype> &in);

	template <class Scalartype>
	bool LessThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b);

	template <class Scalartype>
	bool GreaterThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b);

	/*template <class Scalartype>
	bool Fits(Point3<Scalartype> in_a, Point3<Scalartype> in_b);*/



	template<class placement>
	static Result<placement>* bin_pack2(vector<Item> items, BinBox box, SpatialHash<placement> *hash, Point3f minVolumn);

	static QVector<Matrix44m> BPRotations;
	static void initBPRotations()
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Matrix44m m;
				m.SetIdentity();
				m.SetRotateRad(float(i)*M_PI / 2., Point3f(0, 0, 1));
				switch (j)
				{
				case 1:
				{
						  Matrix44m m2;
						  m2.SetRotateRad(M_PI / 2., Point3f(1, 0, 0));
						  m *= m2;
				}
					break;
				case 2:
				{

						  Matrix44m m2;
						  m2.SetRotateRad(M_PI / 2., Point3f(0, 1, 0));
						  m *= m2;
				}
					break;
				}
				BPRotations.push_back(m);
			}
		}
	}



	enum BinAxis{ AxisX, AxisY, AxisZ };
	class BinItem
	{
	public:
		int ID;
		int Score;
		Point3f Size;
		BinItem()
		{
			ID = 0;
			Score = 0;
			Size.Zero();
		}
		BinItem(int _ID, int _Score, Point3f _Size)
		{
			ID = _ID;
			Score = _Score;
			Size = _Size;
		}
		BinItem & operator =(const BinItem &a)
		{
			ID = a.ID;
			Score = a.Score;
			Size = a.Size;
			return *this;
		}

	};

	class Placement
	{
	public:
		BinItem *item;
		vcg::Point3f position;
		Placement()
		{
			item = new BinItem();
			position.Zero();
		}		
		Placement(BinItem *_item, Point3f &_position)
		{
			item = new BinItem();
			*item = *_item;
			position = _position;
		}
		~Placement(){}

		Placement & operator=(const Placement &a)
		{
			*item = *(a.item);
			position = a.position;
			return *this;
		}
	};



	class BinBox
	{
	public:
		Point3f Origin;
		Point3f Size;
		BinBox()
		{
			Origin = Point3f::Zero();
			Size = Point3f::Zero();
		}
		BinBox(Point3f _Origin, Point3f _Size)
		{
			Origin = _Origin;
			Size = _Size;
		}
		BinBox *Copy()
		{

			return (new BinBox(Origin, Size));
		}
		void Cut(BinAxis axis, int offset, BinBox &boxFirst, BinBox &boxOut)
		{
			Point3f o1 = this->Origin;
			Point3f s1 = this->Size;
			Point3f o2 = this->Origin;
			Point3f s2 = this->Size;

			switch (axis)
			{
			case BinAxis::AxisX:
			{
								   //Point3f xoffset(offset, 0, 0);
								   s1.X() = offset;
								   s2.X() -= offset;
								   o2.X() += offset;

			}
				break;
			case BinAxis::AxisY:
			{
								   //Point3f yoffset(0, offset, 0);
								   s1.Y() = offset;
								   s2.Y() -= offset;
								   o2.Y() += offset;
			}
				break;
			case BinAxis::AxisZ:
			{
								  // Point3f zoffset(0, 0, offset);
								   s1.Z() = offset;
								   s2.Z() -= offset;
								   o2.Z() += offset;
			}
				break;
			}
			boxFirst.Origin = o1;
			boxFirst.Size = s1;
			boxOut.Origin = o2;
			boxOut.Size = s2;

		}


		void Cuts(BinAxis a1, BinAxis a2, BinAxis a3, int s1, int s2, int s3, BinBox &box1, BinBox &box2, BinBox &box3)
		{
			BinBox *tempb = this->Copy();
			BinBox tempx;
			tempb->Cut(a1, s1, *tempb, box1);
			tempb->Cut(a2, s2, *tempb, box2);
			tempb->Cut(a3, s3, tempx, box3);
		}

	};

	static BinBox BinBoxFactory()
	{
		return *new BinBox();
	}


	template<class xlacement>
	static Result<xlacement> * MakeReult(Result<xlacement> r0, Result<xlacement> r1, Result<xlacement> r2, BinItem item, Point3f position)
	{
		vector<Placement> tempPlacements;
		//Placement test = *new Placement( );
		tempPlacements.push_back(*new Placement(&item, position));		
		Result<xlacement> r3 = *new Result<xlacement>(item.Score, tempPlacements);

		int scoreTemp = r0.Score + r1.Score + r2.Score + r3.Score;
		int n = r0.Placements.size() + r1.Placements.size() + r2.Placements.size() + r3.Placements.size();

		vector<Placement> placememtsTemp;
		for (int i = 0; i < r0.Placements.size(); i++)
			placememtsTemp.push_back(r0.Placements[i]);
		for (int i = 0; i < r1.Placements.size(); i++)
			placememtsTemp.push_back(r1.Placements[i]);
		for (int i = 0; i < r2.Placements.size(); i++)
			placememtsTemp.push_back(r2.Placements[i]);
		for (int i = 0; i < r3.Placements.size(); i++)
			placememtsTemp.push_back(r3.Placements[i]);

		if (placememtsTemp.size()>100)
			int test = 0;

		return new Result<xlacement>(scoreTemp, placememtsTemp);


	}


	//template<class xlacement>
	//static Result<xlacement>  bin_pack(vector<BinItem> items, BinBox box)
	//{
	//	Point3f bs = box.Size;
	//	SpatialHash<xlacement> *hash = NewSpatialHash<xlacement>(1000);
	//	Point3f minVolumn = SortP3(items[0].Size);
	//	for (int i = 0; i < items.size(); i++)
	//	{
	//		BinItem ii = items[i];
	//		minVolumn = minVolumn.MinP3(SortP3(ii.Size));
	//	}
	//	Result<xlacement> getresult = (bin_pack2<xlacement>(items, box, hash, minVolumn));
	//	//*rrr = getresult;

	//	return getresult;
	//	//if ()
	//	/*hash: = NewSpatialHash(1000)
	//	minVolume : = items[0].Size.Sort()
	//	for _, item : = range items{
	//	minVolume = minVolume.Min(item.Size.Sort())
	//	}
	//	return pack(items, box, hash, minVolume)*/

	//}

	template<class xlacement>
	static Result<xlacement>  bin_pack(vector<BinItem> items, BinBox box)
	{
		Point3f bs = box.Size;
		SpatialHash<xlacement> *hash = NewSpatialHash<xlacement>(1000);
		Point3f minVolumn = SortP3(items[0].Size);
		for (int i = 0; i < items.size(); i++)
		{
			BinItem ii = items[i];
			minVolumn = minVolumn.MinP3(SortP3(ii.Size));
		}
		Result<xlacement> getresult = (bin_pack2<xlacement>(items, box, hash, minVolumn));
		//r2 = new Result<xlacement>(getresult.Score, getresult.Placements);

		//*rrr = getresult;

		return getresult;		

	}
	template<class placement>
	static Result<placement> bin_pack2(vector<BinItem> items, BinBox box, SpatialHash<placement> *hash, Point3f minVolumn)
	{
		/*if (box.Origin.X() == 0 && box.Origin.Y() == 0 )
			int testets = 0;*/
		Point3f bs = box.Size;
		Point3f ddd = SortP3<float>(bs);
		if (!p3Fits<float>(SortP3<float>(bs), minVolumn))
			return *new Result<placement>();//return *new ResultP();
		Result<placement> result;
		bool ok = hash->get(bs, result);
		if (ok)
			return result;
		Result<placement> *best = new Result<placement>();;
		
		for (int o = 0; o < items.size(); o++)
		{
			BinItem item = items[o];
			Point3f s = item.Size;
			if (s.X()>bs.X() || s.Y() > bs.Y() || s.Z() > bs.Z())
				continue;

			BinBox b[6][3];
			for (int i = 0; i < 6; i++)
			for (int j = 0; j < 3; j++)
			{
				b[i][j] = BinBoxFactory();
			}

			box.Cuts(AxisX, AxisY, AxisZ, s.X(), s.Y(), s.Z(), b[0][0], b[0][1], b[0][2]);
			box.Cuts(AxisX, AxisZ, AxisY, s.X(), s.Z(), s.Y(), b[1][0], b[1][1], b[1][2]);
			box.Cuts(AxisY, AxisX, AxisZ, s.Y(), s.X(), s.Z(), b[2][0], b[2][1], b[2][2]);
			box.Cuts(AxisY, AxisZ, AxisX, s.Y(), s.Z(), s.X(), b[3][0], b[3][1], b[3][2]);
			box.Cuts(AxisZ, AxisX, AxisY, s.Z(), s.X(), s.Y(), b[4][0], b[4][1], b[4][2]);
			box.Cuts(AxisZ, AxisY, AxisX, s.Z(), s.Y(), s.X(), b[5][0], b[5][1], b[5][2]);

			for (int i = 0; i < 6; i++)
			{
				std::vector<Result<placement> > r(3);
				int score = item.Score;
				for (int j = 0; j < 3; j++)
				{
					//Result<placement> temptest = *bin_pack2<placement>(items, b[i][j], hash, minVolumn);
					r[j] = bin_pack2<placement>(items, b[i][j], hash, minVolumn);
					
					//r[j] = *new Result<placement>();
					score += r[j].Score;
 				}			
				if (score>best->Score)
				{
					for (int j = 0; j < 3; j++)
					{
						qDebug()<<"j"<<j<<" "<<r[j].Placements.size();
						qDebug() << " best->placement" << best->Placements.size();						

						Result<placement> temptest2 = r[j].Translate(b[i][j].Origin);

						r[j] = temptest2;// &(r[j]->Translate(b[i][j].Origin));
					}
					best = MakeReult<placement>(r[0], r[1], r[2], item, box.Origin);

				}
			}

		}
		Result<placement> temptest = best->Translate(box.Origin.Negate());
		*best = temptest;//&(best->Translate(box.Origin.Negate()));
		Point3f size(0, 0, 0);
		for (int i = 0; i < best->Placements.size(); i++)
		{
			Placement p= best->Placements[i];
			size = size.MaxP3(p.position + p.item->Size);
		}
 		hash->add(size, bs, *best);
		
		return *best;


		/*
		bs := box.Size
		if !bs.Sort().Fits(minVolume) {
		return Result{}
		}
		if result, ok := hash.Get(bs); ok {
		return result
		}
		best := Result{}
		for _, item := range items {
		s := item.Size
		if s.X > bs.X || s.Y > bs.Y || s.Z > bs.Z {
		continue
		}
		var b [6][3]Box
		b[0][0], b[0][1], b[0][2] = box.Cuts(AxisX, AxisY, AxisZ, s.X, s.Y, s.Z)
		b[1][0], b[1][1], b[1][2] = box.Cuts(AxisX, AxisZ, AxisY, s.X, s.Z, s.Y)
		b[2][0], b[2][1], b[2][2] = box.Cuts(AxisY, AxisX, AxisZ, s.Y, s.X, s.Z)
		b[3][0], b[3][1], b[3][2] = box.Cuts(AxisY, AxisZ, AxisX, s.Y, s.Z, s.X)
		b[4][0], b[4][1], b[4][2] = box.Cuts(AxisZ, AxisX, AxisY, s.Z, s.X, s.Y)
		b[5][0], b[5][1], b[5][2] = box.Cuts(AxisZ, AxisY, AxisX, s.Z, s.Y, s.X)
		for i := 0; i < 6; i++ {
		var r [3]Result
		score := item.Score
		for j := 0; j < 3; j++ {
		r[j] = pack(items, b[i][j], hash, minVolume)
		score += r[j].Score
		}
		if score > best.Score {
		for j := 0; j < 3; j++ {
		r[j] = r[j].Translate(b[i][j].Origin)
		}
		best = MakeResult(r[0], r[1], r[2], item, box.Origin)
		}
		}
		}
		best = best.Translate(box.Origin.Negate())
		var size Vector
		for _, p := range best.Placements {
		size = size.Max(p.Position.Add(p.Item.Size))
		}
		hash.Add(size, bs, best)
		return best
		*/

	}

	template <class Scalartype>
	Point3<Scalartype> SortP3(Point3<Scalartype> &in)
	{
		Point3<Scalartype> result(in);
		/*if (in.X() > in.Z())
		{
			Scalartype xc = in.X();
			in.X() = in.Z();
			in.Z() = xc;
		}
		if (in.X() > in.Y())
		{
			Scalartype xc = in.X();
			in.X() = in.Y();
			in.Y() = xc;
		}
		if (in.Y() > in.Z())
		{
			Scalartype yc = in.Y();
			in.Y() = in.Z();
			in.Z() = yc;
		}*/
		if (result.X() > result.Z())
		{
			Scalartype xc = result.X();
			result.X() = result.Z();
			result.Z() = xc;
		}
		if (result.X() > result.Y())
		{
			Scalartype xc = result.X();
			result.X() = result.Y();
			result.Y() = xc;
		}
		if (result.Y() > result.Z())
		{
			Scalartype yc = result.Y();
			result.Y() = result.Z();
			result.Z() = yc;
		}


		return result;
	}

	template <class Scalartype>
	bool p3Fits(Point3<Scalartype> in_a, Point3<Scalartype> in_b)
	{
		bool p3fits = in_a.X() >= in_b.X() && in_a.Y() >= in_b.Y() && in_a.Z() >= in_b.Z();
		return in_a.X() >= in_b.X() && in_a.Y() >= in_b.Y() && in_a.Z() >= in_b.Z();
	}


	template <class Scalartype>
	bool GreaterThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b)
	{
		bool greaterThanOrEqual = in_a.X() >= in_b.X() && in_a.Y() >= in_b.Y() && in_a.Z() >= in_b.Z();
		return in_a.X() >= in_b.X() && in_a.Y() >= in_b.Y() && in_a.Z() >= in_b.Z();
	}


	template <class Scalartype>
	bool LessThanOrEqual(Point3<Scalartype> in_a, Point3<Scalartype> in_b)
	{
		return in_a.X() <= in_b.X() && in_a.Y() <= in_b.Y() && in_a.Z() <= in_b.Z();
	}
}

#endif
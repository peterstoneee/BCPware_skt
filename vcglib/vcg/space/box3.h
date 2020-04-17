/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
#ifndef __VCGLIB_BOX3
#define __VCGLIB_BOX3

#include <vcg/space/point3.h>
#include <vcg/math/matrix44.h>
#include <vcg/space/line3.h>
#include <vcg/space/plane3.h>

namespace vcg {

	/** \addtogroup space */
	/*@{*/
	/**
	Templated class for 3D boxes.
	This is the class for definition of a axis aligned bounding box in 3D space. It is stored just as two Point3
	@param BoxScalarType (template parameter) Specifies the type of scalar used to represent coords.
	*/
	template <class BoxScalarType>
	class Box3
	{
	public:

		/// The scalar type
		typedef BoxScalarType ScalarType;

		/// min coordinate point
		Point3<BoxScalarType> min;
		/// max coordinate point
		Point3<BoxScalarType> max;
		/// The bounding box constructor
		inline  Box3() { min.X() = 1; max.X() = -1; min.Y() = 1; max.Y() = -1; min.Z() = 1; max.Z() = -1; }
		/// Copy constructor
		inline  Box3(const Box3 & b) { min = b.min; max = b.max; }
		/// Min Max constructor
		inline  Box3(const Point3<BoxScalarType> & mi, const Point3<BoxScalarType> & ma) { min = mi; max = ma; }
		/// Point Radius Constructor
		inline Box3(const Point3<BoxScalarType> & center, const BoxScalarType & radius) {
			min = center - Point3<BoxScalarType>(radius, radius, radius);
			max = center + Point3<BoxScalarType>(radius, radius, radius);
		}
		/// The bounding box distructor
		inline ~Box3() { }
		/// Operator to compare two bounding box
		inline bool operator == (Box3<BoxScalarType> const & p) const
		{
			return min == p.min && max == p.max;
		}
		/// Operator to dispare two bounding box
		inline bool operator != (Box3<BoxScalarType> const & p) const
		{
			return min != p.min || max != p.max;
		}
		/** Varia le dimensioni del bounding box scalandole rispetto al parametro scalare.
			@param s Valore scalare che indica di quanto deve variare il bounding box
			*/
		void Offset(const BoxScalarType s)
		{
			Offset(Point3<BoxScalarType>(s, s, s));
		}
		/** Varia le dimensioni del bounding box del valore fornito attraverso il parametro.
			@param delta Point in 3D space
			*/
		void Offset(const Point3<BoxScalarType> & delta)
		{
			min -= delta;
			max += delta;
		}
		/// Initializing the bounding box
		void Set(const Point3<BoxScalarType> & p)
		{
			min = max = p;
		}
		/// Set the bounding box to a null value
		void SetNull()
		{
			min.X() = 1; max.X() = -1;
			min.Y() = 1; max.Y() = -1;
			min.Z() = 1; max.Z() = -1;
		}
		/** Function to add two bounding box
			@param b Il bounding box che si vuole aggiungere
			*/
		void Add(Box3<BoxScalarType> const & b)
		{
			if (b.IsNull()) return; // Adding a null bbox should do nothing
			if (IsNull()) *this = b;
			else
			{
				if (min.X() > b.min.X()) min.X() = b.min.X();
				if (min.Y() > b.min.Y()) min.Y() = b.min.Y();
				if (min.Z() > b.min.Z()) min.Z() = b.min.Z();

				if (max.X() < b.max.X()) max.X() = b.max.X();
				if (max.Y() < b.max.Y()) max.Y() = b.max.Y();
				if (max.Z() < b.max.Z()) max.Z() = b.max.Z();
			}
		}
		/** Funzione per aggiungere un punto al bounding box. Il bounding box viene modificato se il punto
			cade fuori da esso.
			@param p The point 3D
			*/
		void Add(const Point3<BoxScalarType> & p)
		{
			if (IsNull()) Set(p);
			else
			{
				if (min.X() > p.X()) min.X() = p.X();
				if (min.Y() > p.Y()) min.Y() = p.Y();
				if (min.Z() > p.Z()) min.Z() = p.Z();

				if (max.X() < p.X()) max.X() = p.X();
				if (max.Y() < p.Y()) max.Y() = p.Y();
				if (max.Z() < p.Z()) max.Z() = p.Z();
			}
		}

		/** Function to add a sphere (a point + radius) to a bbox
			@param p The point 3D
			@param radius the radius of the sphere centered on p
			*/
		void Add(const Point3<BoxScalarType> & p, const BoxScalarType radius)
		{
			if (IsNull()) Set(p);
			else
			{
				min.X() = std::min(min.X(), p.X() - radius);
				min.Y() = std::min(min.Y(), p.Y() - radius);
				min.Z() = std::min(min.Z(), p.Z() - radius);

				max.X() = std::max(max.X(), p.X() + radius);
				max.Y() = std::max(max.Y(), p.Y() + radius);
				max.Z() = std::max(max.Z(), p.Z() + radius);
			}
		}
		// Aggiunge ad un box un altro box trasformato secondo la matrice m
		void Add(const Matrix44<BoxScalarType> &m, const Box3<BoxScalarType> & b)
		{
			const Point3<BoxScalarType> &mn = b.min;
			const Point3<BoxScalarType> &mx = b.max;
			Add(m*(Point3<BoxScalarType>(mn[0], mn[1], mn[2])));
			Add(m*(Point3<BoxScalarType>(mx[0], mn[1], mn[2])));
			Add(m*(Point3<BoxScalarType>(mn[0], mx[1], mn[2])));
			Add(m*(Point3<BoxScalarType>(mx[0], mx[1], mn[2])));
			Add(m*(Point3<BoxScalarType>(mn[0], mn[1], mx[2])));
			Add(m*(Point3<BoxScalarType>(mx[0], mn[1], mx[2])));
			Add(m*(Point3<BoxScalarType>(mn[0], mx[1], mx[2])));
			Add(m*(Point3<BoxScalarType>(mx[0], mx[1], mx[2])));
		}
		/** Calcola l'intersezione tra due bounding box. Al bounding box viene assegnato il valore risultante.
			@param b Il bounding box con il quale si vuole effettuare l'intersezione
			*/
		void Intersect(const Box3<BoxScalarType> & b)
		{
			if (min.X() < b.min.X()) min.X() = b.min.X();
			if (min.Y() < b.min.Y()) min.Y() = b.min.Y();
			if (min.Z() < b.min.Z()) min.Z() = b.min.Z();

			if (max.X() > b.max.X()) max.X() = b.max.X();
			if (max.Y() > b.max.Y()) max.Y() = b.max.Y();
			if (max.Z() > b.max.Z()) max.Z() = b.max.Z();

			if (min.X() > max.X() || min.Y() > max.Y() || min.Z() > max.Z()) SetNull();
		}


		/** Trasla il bounding box di un valore definito dal parametro.
			@param p Il bounding box trasla sulla x e sulla y in base alle coordinate del parametro
			*/
		void Translate(const Point3<BoxScalarType> & p)
		{
			min += p;
			max += p;
		}

		/** Verifica se un punto appartiene ad un bounding box.
			@param p The point 3D
			@return True se p appartiene al bounding box, false altrimenti
			*/
		bool IsIn(Point3<BoxScalarType> const & p) const
		{
			return (
				min.X() <= p.X() && p.X() <= max.X() &&
				min.Y() <= p.Y() && p.Y() <= max.Y() &&
				min.Z() <= p.Z() && p.Z() <= max.Z()
				);
		}
		/** Verifica se un punto appartiene ad un bounding box aperto sul max.
			@param p The point 3D
			@return True se p appartiene al bounding box, false altrimenti
			*/
		bool IsInEx(Point3<BoxScalarType> const & p) const
		{
			return (
				min.X() <= p.X() && p.X() < max.X() &&
				min.Y() <= p.Y() && p.Y() < max.Y() &&
				min.Z() <= p.Z() && p.Z() < max.Z()
				);
		}
		/** Verifica se due bounding box collidono cioe' se hanno una intersezione non vuota. Per esempio
			due bounding box adiacenti non collidono.
			@param b A bounding box
			@return True se collidoo, false altrimenti
			*/
		/* old version
		bool Collide(Box3<BoxScalarType> const &b)
		{
		Box3<BoxScalarType> bb=*this;
		bb.Intersect(b);
		return bb.IsValid();
		}
		*/
		bool Collide(Box3<BoxScalarType> const &b) const
		{
			return b.min.X()<max.X() && b.max.X()>min.X() &&
				b.min.Y()<max.Y() && b.max.Y()>min.Y() &&
				b.min.Z()<max.Z() && b.max.Z()>min.Z();
		}
		Box3<BoxScalarType> IntersectionBox(Box3<BoxScalarType> const &b) const
		{
			Box3<BoxScalarType> temp;
			temp.SetNull();
			if (!Collide(b))
				return temp;
			if (b.IsNull())
				return temp;
			Point3f minn(std::max(min.X(), b.min.X()), std::max(min.Y(), b.min.Y()), std::max(min.Z(), b.min.Z()));
			Point3f maxx(std::min(max.X(), b.max.X()), std::min(max.Y(), b.max.Y()), std::min(max.Z(), b.max.Z()));

			Point3f minnn (std::min(minn.X(), maxx.X()), std::min(minn.Y(), maxx.Y()), std::min(minn.Z(), maxx.Z()));
			Point3f maxxx (std::max(minn.X(), maxx.X()), std::max(minn.Y(), maxx.Y()), std::max(minn.Z(), maxx.Z()));
			temp.Import(Box3<BoxScalarType>(minnn, maxxx));
			return 	temp;
			
		}
		bool isInside(Box3<BoxScalarType> const&b)const
		{
			return b.min.X() >= (min.X()-0.01) && b.max.X()<=(max.X()+0.01) &&
				b.min.Y()>= (min.Y()-0.01) && b.max.Y() <= (max.Y()+0.01) &&
				b.min.Z() >= (min.Z()-0.01) && b.max.Z() <= (max.Z() + 0.01);

		}

		bool Intersects(Box3<BoxScalarType> const&b)
		{
			return !(min.X()>b.max.X() || max.X() < b.min.X() || min.Y() > b.max.Y() ||
				min.Y() < b.min.Y() || min.Z() > b.max.Z() || min.Z() < b.min.Z());
		}
		 
		Box3<BoxScalarType> Transformbox(Matrix44<BoxScalarType> const &m) const
		{
			Box3<BoxScalarType> tempBox;
			Point3f tempMin = m * min;
			Point3f tempMax = m * max;
			tempBox.Import(Box3<BoxScalarType>(
				Point3f(std::min(tempMin.X(), tempMax.X()), std::min(tempMin.Y(), tempMax.Y()), std::min(tempMin.Z(), tempMax.Z())),
				Point3f(std::max(tempMin.X(), tempMax.X()), std::max(tempMin.Y(), tempMax.Y()), std::max(tempMin.Z(), tempMax.Z()))));
			return tempBox;
		}

		Box3<BoxScalarType> Transformbox_test(Matrix44<BoxScalarType> const &m) const
		{
			Box3<BoxScalarType> tempBox;
			Point3<BoxScalarType> resultMin(m *P(0) - Center());
			Point3<BoxScalarType> resultMax(m *P(0) - Center());
			for (int i = 0; i < 8; i++)
			{
				Point3<BoxScalarType> tempI = m *P(i) - Center();
				resultMin.Import(Point3<BoxScalarType>(std::min(tempI.X(), resultMin.X()), std::min(tempI.Y(), resultMin.Y()), std::min(tempI.Z(), resultMin.Z())));
				resultMax.Import(Point3<BoxScalarType>(std::max(tempI.X(), resultMax.X()), std::max(tempI.Y(), resultMax.Y()), std::max(tempI.Z(), resultMax.Z())));



			}	
			
			tempBox.Import(Box3<BoxScalarType>(resultMin+Center(), resultMax+Center()));
			return tempBox;
		}

		/*bool isInside(Box3<BoxScalarType> const&b)const
		{
			return b.min.X() >= (min.X()) && b.max.X() <= (max.X()) &&
				b.min.Y() >= (min.Y()) && b.max.Y() <= (max.Y() ) &&
				b.min.Z() >= (min.Z()) && b.max.Z() <= (max.Z() );

		}*/
		/** Controlla se il bounding box e' nullo.
			@return True se il bounding box e' nullo, false altrimenti
			*/
		bool IsNull() const { return min.X()>max.X() || min.Y() > max.Y() || min.Z() > max.Z(); }
		/** Controlla se il bounding box e' vuoto.
			@return True se il bounding box e' vuoto, false altrimenti
			*/
		bool IsEmpty() const { return min == max; }
		/// Restituisce la lunghezza della diagonale del bounding box.
		BoxScalarType Diag() const
		{
			return Distance(min, max);
		}
		/// Calcola il quadrato della diagonale del bounding box.
		BoxScalarType SquaredDiag() const
		{
			return SquaredDistance(min, max);
		}
		/// Calcola il centro del bounding box.
		Point3<BoxScalarType> Center() const
		{
			return (min + max) / 2;
		}
		/// Compute bounding box size.
		Point3<BoxScalarType> Dim() const
		{
			return (max - min);
		}
		/// Returns global coords of a local point expressed in [0..1]^3
		Point3<BoxScalarType> LocalToGlobal(Point3<BoxScalarType> const & p) const{
			return Point3<BoxScalarType>(
				min[0] + p[0] * (max[0] - min[0]),
				min[1] + p[1] * (max[1] - min[1]),
				min[2] + p[2] * (max[2] - min[2]));
		}
		/// Returns local coords expressed in [0..1]^3 of a point in 3D
		Point3<BoxScalarType> GlobalToLocal(Point3<BoxScalarType> const & p) const{
			return Point3<BoxScalarType>(
				(p[0] - min[0]) / (max[0] - min[0]),
				(p[1] - min[1]) / (max[1] - min[1]),
				(p[2] - min[2]) / (max[2] - min[2])
				);
		}
		/// Calcola il volume del bounding box.
		BoxScalarType Volume() const
		{
			if (!IsNull())
				return (max.X() - min.X())*(max.Y() - min.Y())*(max.Z() - min.Z());
			else return 0;
		}
		/// Calcola la dimensione del bounding box sulla x.
		inline BoxScalarType DimX() const { return max.X() - min.X(); }
		/// Calcola la dimensione del bounding box sulla y.
		inline BoxScalarType DimY() const { return max.Y() - min.Y(); }
		/// Calcola la dimensione del bounding box sulla z.
		inline BoxScalarType DimZ() const { return max.Z() - min.Z(); }
		/// Calcola il lato di lunghezza maggiore
		inline unsigned char MaxDim() const {
			int i;
			Point3<BoxScalarType> diag = max - min;
			if (diag[0] > diag[1]) i = 0; else i = 1;
			return (diag[i] > diag[2]) ? i : 2;
		}
		/// Calcola il lato di lunghezza minore
		inline unsigned char MinDim() const {
			int i;
			Point3<BoxScalarType> diag = max - min;
			if (diag[0] < diag[1]) i = 0; else i = 1;
			return (diag[i] < diag[2]) ? i : 2;
		}
		inline BoxScalarType DimValue(int num) const
		{
			switch (num)
			{
			case 0:
				return DimX();
				break;
			case 1:
				return DimY();
				break;
			case 2:
				return DimZ();
				break;
			default:
				return false;
				break;
			}
		}
		//***20150421_plus_midDim
		inline unsigned char MidDim() const {
			if (MaxDim() == MinDim())return MaxDim();
			else return (3 - MaxDim() - MinDim());
		}

		inline Point3<BoxScalarType> Size()
		{
			return Point3<BoxScalarType>(this->max - this->min);
		}

		//***20150420_max area
		inline BoxScalarType MaxArea() const {
			BoxScalarType i;
			Point3<BoxScalarType> area(DimX()*DimY(), DimY()*DimZ(), DimZ()*DimX());
			if (area[0] > area[1]) i = area[0]; else i = area[1];
			return (i > area[2]) ? i : area[2];
		}
		//***20150420_max area
		inline BoxScalarType Total_SurfaceArea() const {
			return 2.0*(DimX()*DimY() + DimY()*DimZ() + DimZ()*DimX());
		}

		template <class Q>
		inline void Import(const Box3<Q> & b)
		{
			min.Import(b.min);
			max.Import(b.max);
		}

		template <class Q>
		static inline Box3 Construct(const Box3<Q> & b)
		{
			return Box3(Point3<BoxScalarType>::Construct(b.min), Point3<BoxScalarType>::Construct(b.max));
		}

		/// gives the ith box vertex in order: (x,y,z),(X,y,z),(x,Y,z),(X,Y,z),(x,y,Z),(X,y,Z),(x,Y,Z),(X,Y,Z)
		Point3<BoxScalarType> P(const int & i) const {
			return Point3<BoxScalarType>(
				min[0] + (i % 2) * DimX(),
				min[1] + ((i / 2) % 2) * DimY(),
				min[2] + (i > 3)* DimZ());
		}
		//***20150818***�s�W����box
		template <class Q>
		static inline Box3 Seperate(Box3<Q> &b, Box3<Q> &c, ScalarType x_cutLength, ScalarType y_cutLength, ScalarType z_cutLength)
		{
			//qDebug() << "b.max.Y()"<< b.max.Y();
			Point3<ScalarType> cut_1_min(b.min.X(), b.max.Y() - y_cutLength, b.min.Z());
			Point3<ScalarType> cut_1_max(b.max.X() - x_cutLength, b.max.Y(), b.max.Z() - z_cutLength);
			Box3<ScalarType> cut_1_box(cut_1_min, cut_1_max);

			Point3<ScalarType> cut_2_min(b.min.X(), b.min.Y(), b.min.Z());
			Point3<ScalarType> cut_2_max(b.max.X(), b.max.Y() - y_cutLength, b.max.Z());
			Box3<ScalarType> cut_2_box(cut_2_min, cut_2_max);
			c = cut_2_box;
			//Box3(Point3<BoxScalarType>::Construct(b.min), Point3<BoxScalarType>::Construct(b.max));

			return cut_1_box;
		}
		//***20150930***
		/*static bool Intersect_(Box3<BoxScalarType> a,const Box3<BoxScalarType> & b)
		{
		if (a.min.X() < b.min.X()) a.min.X() = b.min.X();
		if (a.min.Y() < b.min.Y()) a.min.Y() = b.min.Y();
		if (a.min.Z() < b.min.Z()) a.min.Z() = b.min.Z();

		if (a.max.X() > b.max.X()) a.max.X() = b.max.X();
		if (a.max.Y() > b.max.Y()) a.max.Y() = b.max.Y();
		if (a.max.Z() > b.max.Z()) a.max.Z() = b.max.Z();

		if (a.min.X()>max.X() || a.min.Y()>max.Y() || a.min.Z()>max.Z()) return false;
		else return true;
		}*/
	}; // end class definition

	template <class T> Box3<T> Point3<T>::GetBBox(Box3<T> &bb) const {
		bb.Set(*this);
	}


	typedef Box3<short>  Box3s;
	typedef Box3<int>	 Box3i;
	typedef Box3<float>  Box3f;
	typedef Box3<double> Box3d;


	/*@}*/

} // end namespace
#endif


/****************************************************************************
* BCPware
*
*  Copyright 2018 by
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/
#ifndef CONSTRUCTION_GROOVE_H
#define CONSTRUCTION_GROOVE_H


#include <vcg/complex/complex.h>
#include <vcg/space/box3.h>

using namespace vcg;

template<typename Scalarp>
class Construction_Groove :public Box3<Scalarp>
{
private:
	bool grooveHit;
public:

	Scalarp groovelength;
	Scalarp groovewidth;
	Scalarp grooveheight;
	

    Construction_Groove()
    {
        groovelength = 20.0;
        groovewidth = 22.0;
        grooveheight = 22.0;
		min = vcg::Point3<Scalarp>(-groovelength / 2., -grooveheight / 2., -groovewidth / 2.);
		max = vcg::Point3<Scalarp>(groovelength / 2., grooveheight / 2., groovewidth / 2.);
    }
    //inline Construction_Groove(const ScalarType &groovelength, const ScalarType &groovewidth, const ScalarType &grooveheight) :Box3(vcg::Point3<ScalarType>(-groovelength / 2., -groovewidth / 2., -grooveheight / 2.), vcg::Point3<PALETTE_SCALAR>(groovelength / 2., groovewidth / 2., grooveheight / 2.))
    //{
    //    this->groovelength = groovelength;
    //    this->groovewidth = groovewidth;
    //    this->grooveheight = grooveheight;
    //    //max = vcg::Point3<PALETTE_SCALAR>(-groovelength / 2., -groovewidth / 2., -grooveheight / 2.);
    //    //min = vcg::Point3<PALETTE_SCALAR>(groovelength / 2., groovewidth / 2., grooveheight / 2.);
    //}

	Construction_Groove(Scalarp groovelength, Scalarp groovewidth, Scalarp grooveheight)
	{
		this->groovelength = groovelength;
		this->groovewidth = groovewidth;
		this->grooveheight = grooveheight;
		min = vcg::Point3<Scalarp>(-groovelength / 2., -grooveheight / 2., -groovewidth / 2.);
		max = vcg::Point3<Scalarp>(groovelength / 2., grooveheight / 2., groovewidth / 2.);
	}
	//opengl coordinate****x*********************z*********************y
	void setLWH(Scalarp groovelength, Scalarp groovewidth, Scalarp grooveheight)
	{
		this->groovelength = groovelength;//x
		this->groovewidth = groovewidth;//z
		this->grooveheight = grooveheight;//y
		min = vcg::Point3<Scalarp>(-groovelength / 2., -grooveheight / 2.,  - groovewidth / 2.);
		max = vcg::Point3<Scalarp>(groovelength / 2., grooveheight / 2., groovewidth / 2.);
		/*min = vcg::Point3<Scalarp>(0, 0, 0);
		max = vcg::Point3<Scalarp>(groovelength , grooveheight, groovewidth);*/
	}

	void setGrooveHit(bool x)
	{
		grooveHit = x;
	}
	bool getGrooveHit()
	{
		return grooveHit;
	}


	//Construction_Groove(Scalarp a){}
};

#endif // CONSTRUCTION_GROOVE_H

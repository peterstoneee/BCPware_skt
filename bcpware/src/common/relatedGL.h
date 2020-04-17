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
#ifndef RELATEDGL_H
#define RELATEDGL_H

namespace SKT
{	
	
	template<typename MeshD>
	class BufferRelated
	{
	public :
		BufferRelated(MeshD meshSS) :_meshSS(meshSS)
		{

		}


		~BufferRelated(){}
	protected:
		bool updateMeshBuffer()
		{
			size_t tn = _meshSS.fn;
			size_t facechunk = std::min(size_t(tn), _perbatchprim);
			typename 
			for
			{

			}
		}

		size_t _perbatchprim;
		MeshD &_meshSS;




	};




};





#endif
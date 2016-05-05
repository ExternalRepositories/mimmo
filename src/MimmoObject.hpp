/*---------------------------------------------------------------------------*\
 *
 *  MiMMO
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of MiMMO.
 *
 *  MiMMO is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  MiMMO is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MiMMO. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/
#ifndef __MIMMOOBJECT_HPP__
#define __MIMMOOBJECT_HPP__

#include "bitpit.hpp"
#include "MiMMO_TypeDef.hpp"

namespace mimmo{

/*!
 *	\date			09/feb/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief MimmoObject is the base container of geometry for MiMMo library
 *
 *	MiMMO container has the information about the geometry mesh:
 *	- the type of the linked mesh of class bitpit::Patch (generic patch, surface patch or volume patch);
 *	- the pointer to the mesh of class bitpit::Patch;
 *	- the boolean indicator of the nature of the patch (true if the patch is created within the MiMMo container).
 *
 */
class MimmoObject{
public:

	//members
	int						m_type;				/**<Type of geometry (0 = generic patch, 1 = surface mesh, 2 = volume mesh). */
	bitpit::PatchKernel*	m_geometry;			/**<Reference geometry. */
	bool					m_internalPatch;	/**<If the geometry is internally created. */
	livector1D				m_mapData;			/**<Map of vertex ids actually set, for aligning external vertex data to bitpit::PatchKernel ordering */
	livector1D				m_mapCell;			/**<Map of cell ids actually set, for aligning external cell data to bitpit::PatchKernel ordering*/ 
	liimap					m_mapDataInv;		/**<Inverse of Map of vertex ids actually set, for aligning external vertex data to bitpit::Patch ordering */
	liimap					m_mapCellInv;		/**<Inverse of Map of cell ids actually set, for aligning external vertex data to bitpit::Patch ordering */
	shivector1D				m_pids;				/**<pid data associated to each tessellation cell, in local compact indexing */
	shivector1D				m_pidsType;			/**<pid type available for your geometry */
	
public:
	MimmoObject(int type = 1);
	//TODO connectivity must be a vector<vector<long int> >
	MimmoObject(int type, dvecarr3E & vertex, ivector2D * connectivity = NULL);
	MimmoObject(int type, bitpit::PatchKernel* geometry);
	~MimmoObject();

	MimmoObject(const MimmoObject & other);
	MimmoObject & operator=(const MimmoObject & other);

	void 		clear();
	bool		isEmpty();
	int			getType();
	long		getNVertex();
	long		getNCells();
	dvecarr3E	getVertex();
	darray3E	getVertex(long i);
	ivector1D	getConnectivity(long i);
	ivector2D	getConnectivity();
	bitpit::PatchKernel*		getGeometry();
	
	livector1D&	getMapData();
	long		getMapData(int i);
	liimap&		getMapDataInv();
	int			getMapDataInv(long id);
	
	livector1D&	getMapCell();
	long		getMapCell(int i);
	liimap&		getMapCellInv();
	int			getMapCellInv(long id);
	
	const shivector1D 	&	getPidTypeList() const ;
	const shivector1D	&	getPid() const;
	
	bool		setVertex(dvecarr3E & vertex);
	bool		resetVertex(dvecarr3E & vertex);
	bool		setVertex(darray3E & vertex);
	bool		setVertex(darray3E & vertex, long idtag);
	bool		modifyVertex(darray3E & vertex, long id);

	//TODO connectivity must be a vector<vector<long int> >
	bool		setConnectivity(ivector2D * connectivity);
	bool		setGeometry(int type, bitpit::PatchKernel* geometry);
	bool		setMapData();
	bool		setMapCell();
	bool		cleanGeometry();

	livector1D 	getVertexFromCellList(livector1D cellList);
	ivector1D	convertVertexIDtoLocal(livector1D vertList);
	livector1D	convertLocaltoVertexID(ivector1D vList);
	ivector1D	convertCellIDtoLocal(livector1D cellList);
	livector1D	convertLocaltoCellID(ivector1D cList);
	
	livector1D  extractBoundaryVertexID();
	livector1D	extractPIDCells(short);
	void		write(std::string filename);
	//TODO implement safe access to m_mapData/Cell members in getMap... methods and convertVertexIDToLocal, convertLocalToVertexID methods.
};

}

#endif /* __MIMMOOBJECT_HPP__ */

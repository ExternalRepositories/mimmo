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
#include "BvTree.hpp"
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
	
protected:
	//members
	int						m_type;				/**<Type of geometry (0 = pointCloud, 1 = surface mesh, 2 = volume mesh). */
	bitpit::PatchKernel*	m_patch;			/**<Reference to bitpit patch handling geometry. */
	bool					m_internalPatch;	/**<If the geometry is internally created. */
	livector1D				m_mapData;			/**<Map of vertex ids actually set, for aligning external vertex data to bitpit::PatchKernel ordering */
	livector1D				m_mapCell;			/**<Map of cell ids actually set, for aligning external cell data to bitpit::PatchKernel ordering*/ 
	liimap					m_mapDataInv;		/**<Inverse of Map of vertex ids actually set, for aligning external vertex data to bitpit::Patch ordering */
	liimap					m_mapCellInv;		/**<Inverse of Map of cell ids actually set, for aligning external vertex data to bitpit::Patch ordering */
	
	//TODO to be updated w/ new PID feature of PatchKernel
	shivector1D				m_pids;				/**<pid data associated to each tessellation cell, in local compact indexing */
	shivector1D				m_pidsType;			/**<pid type available for your geometry */

	BvTree							m_bvTree;			/**< ordered tree of geometry simplicies for fast searching purposes */
	bool							m_bvTreeBuilt; 		/**< track correct building of bvtree along with geometry modifications */
	bitpit::KdTree<3,darray3E,long>	m_kdTree;			/**< ordered tree of geometry vertices for fast searching purposes */
	bool							m_kdTreeBuilt;		/**< track correct building of kdtree along eith geometry modifications */
	bool							m_BvTreeSupported; /**< Flag for geometries not supporting bvTree building*/
	//TODO temporary flag for modifying vertices -> ambigous effect on bvTree, kdTree
	bool							m_retrackTrees;    /**< set true if trees are still usable but non sync'd with geometry modifications */
	
	
public:
	MimmoObject(int type = 1); 
	MimmoObject(int type, dvecarr3E & vertex, ivector2D * connectivity = NULL);
	MimmoObject(int type, bitpit::PatchKernel* geometry);
	~MimmoObject();

	MimmoObject(const MimmoObject & other);
	MimmoObject & operator=(const MimmoObject & other);

	void 				clear();
	bool				isEmpty();
	bool				isBvTreeSupported();
	int					getType();
	long				getNVertex()const ;
	long				getNCells()const;
	
	dvecarr3E									getVertexCoords();
	darray3E									getVertexCoords(long i);
	bitpit::PiercedVector<bitpit::Vertex> &				getVertices();
	const bitpit::PiercedVector<bitpit::Vertex> &		getVertices() const ;
	
	ivector2D									getCompactConnectivity();
	livector2D									getConnectivity();
	livector1D									getCellConnectivity(long id);
	bitpit::PiercedVector<bitpit::Cell> &		getCells();
	const bitpit::PiercedVector<bitpit::Cell> &		getCells() const;
	
	bitpit::PatchKernel*		getPatch();
	
	livector1D&		getMapData();
	long			getMapData(int i);
	
	liimap&			getMapDataInv();
	const liimap&	getMapDataInv()const;
	
	int				getMapDataInv(long id);
	
	livector1D&		getMapCell();
	long			getMapCell(int i);
	liimap&			getMapCellInv();
	int				getMapCellInv(long id);
	
	//TODO to be updated w/ new PID feature of PatchKernel
	shivector1D &	getPidTypeList();
	shivector1D	&	getPid();
	
	bool								isBvTreeBuilt();
	BvTree*								getBvTree();
	bool								isKdTreeBuilt();
	bitpit::KdTree<3, darray3E, long> *	getKdTree();
	bool								retrackTrees();
	
	const MimmoObject * getCopy();
	
	bool	setVertices(const bitpit::PiercedVector<bitpit::Vertex> & vertices);
	bool	addVertex(const darray3E & vertex, const long idtag = bitpit::Vertex::NULL_ID);
	bool	modifyVertex(const darray3E & vertex, long id);

?	bool	setCells(const bitpit::PiercedVector<bitpit::Cell> & cells);
?	bool	addConnectedCell(const livector1D & locConn, bitpit::ElementInfo::Type type, long idtag = bitpit::Cell::NULL_ID);
	
?	bool	setPatch(int type, bitpit::PatchKernel* geometry);
	bool	setMapData();
?	bool	setMapCell();

	void 	setPID(shivector1D ); //TODO to be updated w/ new PID feature of PatchKernel
?	void	setSOFTCopy(const MimmoObject * other);
?	void	setHARDCopy(const MimmoObject * other);
	
?	bool	cleanGeometry();

	livector1D 	getVertexFromCellList(livector1D cellList);

	livector1D 	convertLocalToVertexID(ivector1D);
	ivector1D 	convertVertexIDToLocal(livector1D);
	livector1D 	convertLocalToCellID(ivector1D);
	ivector1D 	convertCellIDToLocal(livector1D);
	
	livector1D  extractBoundaryVertexID();
	livector1D	extractPIDCells(short);
	livector1D	extractPIDCells(shivector1D);
	
?	void		buildBvTree();
?	void		buildKdTree();	


private:
?	int checkCellType(bitpit::ElementInfo::Type type);
?	void 	cleanKdTree();
};

}

#endif /* __MIMMOOBJECT_HPP__ */

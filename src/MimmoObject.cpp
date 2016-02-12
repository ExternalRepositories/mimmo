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
#include "MimmoObject.hpp"
#include "Operators.hpp"

using namespace std;

/*!Default constructor of MimmoObject.
 * It sets to zero/null each member/pointer.
 * \param[in] type Type of linked Patch (0 = generic (default value), 1 = surface, 2 = volume).
 */
MimmoObject::MimmoObject(int type){
	m_type = type;
	m_geometry = NULL;
	m_internalPatch = false;
}

/*!Custom constructor of MimmoObject. This constructor builds a generic patch from given vertex and connectivity.
 * \param[in] type Type of linked Patch (0 = generic (default value), 1 = surface, 2 = volume).
 * \param[in] vertex Coordinates of vertices of the geometry.
 * \param[in] connectivity Pointer to connectivity strucutre of the surface/volume mesh.
 */
MimmoObject::MimmoObject(int type, dvecarr3E & vertex, ivector2D * connectivity){
	m_type = type;
	m_internalPatch = true;
	//TODO surftri e voltri con costruttore con vertici e connettività
//	if (m_type){
//		m_geometry = new Voltri();
//	}else{
//		m_geometry = new Surftri();
//	}
//	setVertex(vertex);
//	if (connectivity != NULL)
//		setConnectivity((*connectivity));
};

/*!Custom constructor of MimmoObject.
 * This constructor links a given patch of given type.
 * \param[in] type Type of linked Patch (0 = generic (default value), 1 = surface, 2 = volume).
 * \param[in] geometry Pointer to a geometry of class Patch to be linked.
 */
MimmoObject::MimmoObject(int type, Patch* geometry){
	m_type 			= type;
	m_geometry 		= geometry;
	m_internalPatch = false;
}

/*!Default destructor of MimmoObject.
 * It calls clear() method of MimmoObject.
 */
MimmoObject::~MimmoObject(){
	clear();
};

/*!Copy constructor of MimmoObject.
 */
MimmoObject::MimmoObject(const MimmoObject & other){
	m_type 			= other.m_type;
	m_geometry 		= other.m_geometry;
	m_internalPatch = other.m_internalPatch;
};

/*!Assignement operator of MimmoObject.
 */
MimmoObject & MimmoObject::operator=(const MimmoObject & other){
	m_type 			= other.m_type;
	m_geometry 		= other.m_geometry;
	m_internalPatch = other.m_internalPatch;
};

/*!It Clears the object. The pointer to the geometry is set to NULL and the
 *  mesh is deleted only if internally created.
 */
void
MimmoObject::clear(){
	if (m_internalPatch){
		delete m_geometry;
	}
	m_geometry = NULL;
};
/*!Is the object empty?
 * \return True/false if the pointer to the geometry is NULL.
 */
bool
MimmoObject::isEmpty(){
	return (m_geometry == NULL);
};

/*!It gets the type of the geometry Patch.
 * \return Type of geometry mesh (0 = generic (deprecated), 1 = surface, 2 = volume).
 */
int
MimmoObject::getType(){
	return m_type;
};

/*!It gets the number of vertices of the geometry Patch.
 * \return Number of vertices of geometry mesh.
 */
long
MimmoObject::getNVertex(){
	return m_geometry->getVertexCount();
};

/*!It gets the number of cells of the geometry Patch.
 * \return Number of cells of geometry mesh.
 */
long
MimmoObject::getNCells(){
	return m_geometry->getCellCount();
};

/*!It gets the coordinates of the vertices of the geometry Patch.
 * \return Coordinates of vertices of geometry mesh.
 */
dvecarr3E
MimmoObject::getVertex(){
	long nv = getNVertex();
	dvecarr3E result(nv);
	for (int i=0; i<nv; i++){
		result[i] = m_geometry->getVertexCoords(i);
	}
	return result;
};

/*!It gets the coordinates of the vertices of the geometry Patch.
 * \param[in] i Index of the vertex of geometry mesh.
 * \return Coordinates of the i-th vertex of geometry mesh.
 */
darray3E
MimmoObject::getVertex(long i){
	return 	m_geometry->getVertexCoords(i);
};

//ivector2D*
//MimmoObject::getConnectivity(){
//};

/*!It gets the geometry Patch linked by Mimmo Object.
 * \return Pointer to geometry mesh.
 */
Patch*
MimmoObject::getGeometry(){
	return m_geometry;
};

/*!It sets the coordinates of the vertices of the geometry Patch.
 * \param[in] vertex Coordinates of vertices of geometry mesh.
 * \return False if no geometry is linked.
 */
bool
MimmoObject::setVertex(dvecarr3E & vertex){
	if (m_geometry == NULL) return false;
	long nv = vertex.size();
	long index;
	for (int i=0; i<nv; i++){
		index = m_geometry->addVertex();
		m_geometry->getvertex(index).setCoords(vertex);
	}
	return true;

};

/*!It adds and it sets the coordinates of one vertex of the geometry Patch.
 * \param[in] index Index of vertex to be added to the geometry mesh.
 * \param[in] vertex Coordinates of vertex to be added to geometry mesh.
 * \return False if no geometry is linked.
 */
bool
MimmoObject::setVertex(int index, darray3E & vertex){
	if (m_geometry == NULL) return false;
	m_geometry->addVertex(index);
	m_geometry->getvertex(index).setCoords(vertex);
	return true;
};

/*!It sets the connectivity of the cells of the geometry Patch.
 * \param[in] connectivity Connectivity of cells of geometry mesh.
 * \return False if no geometry is linked.
 */
bool
MimmoObject::setConnectivity(ivector2D & connectivity){
	if (m_geometry == NULL) return false;
};

/*!It sets the geometry Patch.
 * \param[in] type Type of linked Patch (0 = generic (default value), 1 = surface, 2 = volume).
 * \param[in] geometry Pointer to a geometry of class Patch to be linked.
 * \return False if the argument pointer is NULL.
 */
bool
MimmoObject::setGeometry(int type, Patch* geometry){
	if (geometry == NULL) return false;
	m_geometry = geometry;
	m_type = type;
};

/*!It writes the mesh geometry on an output file.
 * \param[in] filename Name of the output file.
 */
void
MimmoObject::write(string filename){
	m_geometry->writeMesh(filename);
};





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
#include "Apply.hpp"

using namespace mimmo;

/*!Default constructor of Apply
 */
Apply::Apply():BaseManipulation(){
	m_name = "MiMMO.Apply";
	m_force = false;
	buildPorts();
};

/*!Default destructor of Apply
 */
Apply::~Apply(){};

/*!Copy constructor of Apply.
 */
Apply::Apply(const Apply & other){
	*this = other;
};

/*!Assignement operator of Apply.
 */
Apply & Apply::operator=(const Apply & other){
	*(static_cast<BaseManipulation*> (this)) = *(static_cast<const BaseManipulation*> (&other));
	m_force = other.m_force;
	return(*this);
};

/*! It builds the input/output ports of the object
 */
void
Apply::buildPorts(){
	bool built = true;
	built = (built && createPortIn<dvecarr3E, Apply>(this, &Apply::setInput, PortType::M_GDISPLS, mimmo::pin::containerTAG::VECARR3, mimmo::pin::dataTAG::FLOAT));
	built = (built && createPortIn<MimmoObject*, Apply>(this, &BaseManipulation::setGeometry, PortType::M_GEOM, mimmo::pin::containerTAG::SCALAR, mimmo::pin::dataTAG::MIMMO_));
	m_arePortsBuilt = built;
};

/*!
 * Return true, if rebuilding of search trees of your target geometry of class MimmoObject is forced by the User
 */
bool	Apply::getRefreshGeometryTrees(){
	return m_force;
}



/*!
 * If set true, forces rebuilding of search trees of your target geometry of class MimmoObject
 */
void	Apply::setRefreshGeometryTrees(bool force){
	if(getGeometry() == NULL) return;
}

/*!It sets the displacements input.
 * \param[in] input Input displacements of the geometry vertices.
 */
void
Apply::setInput(dvecarr3E input){
	m_input = input;
}

/*!Execution command.
 * It applies the deformation stored in the input of base class (casting the input
 * for apply object to dvecarr3E) to the linked geometry.
 * After exec() the original geometry will be permanently modified.
 */
void
Apply::execute(){
	if (getGeometry() == NULL) return;

	if(m_force){
		getGeometry()->buildBvTree();
		getGeometry()->buildKdTree();
	}
	
	dvecarr3E vertex = getGeometry()->getVertexCoords();
	long nv = getGeometry()->getNVertex();
	nv = long(std::min(int(nv), int(m_input.size())));
	livector1D & idmap = getGeometry()->getMapData();
	for (long i=0; i<nv; i++){
		vertex[i] += m_input[i];
		getGeometry()->modifyVertex(vertex[i], idmap[i]);
	}
	return;
};


/*!
 * Get settings of the class from bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::absorbSectionXML.The class read only RefreshGeometryTrees parameter, 
 * while Input and Geometry parameters are meant to be passed only through Port linking.
 * 
 * \param[in]	slotXML bitpit::Config::Section which reads from
 */
 void Apply::absorbSectionXML(bitpit::Config::Section & slotXML){
	 
	std::string input; 
	if(slotXML.hasOption("RefreshGeometryTrees")){
		std::string input = slotXML.get("RefreshGeometryTrees");
	}; 
	
	bool value = false;
	if(!input.empty()){
		std::stringstream ss(bitpit::utils::trim(input));
		ss >> value;
	}
	
	setRefreshGeometryTrees(value);
};

/*!
 * Write settings of the class to bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::flushSectionXML;
 * The class write only RefreshGeometryTrees parameter, if it is different from its default value, 
 * while Input and Geometry parameters are meant to be passed only through Port linking.
 * 
 * \param[in]	slotXML bitpit::Config::Section which writes to
 */
void Apply::flushSectionXML(bitpit::Config::Section & slotXML){
	
	bool value = getRefreshGeometryTrees();
	
	std::string towrite = std::to_string(value);
	
	slotXML.set("RefreshGeometryTrees", towrite);
};	






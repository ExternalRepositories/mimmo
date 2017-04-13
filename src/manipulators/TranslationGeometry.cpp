/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of mimmo.
 *
 *  mimmo is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  mimmo is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mimmo. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/
#include "TranslationGeometry.hpp"

namespace mimmo{


/*!
 * Default constructor of TranslationGeometry
 */
TranslationGeometry::TranslationGeometry(darray3E direction){
	m_direction = direction;
	m_name = "mimmo.TranslationGeometry";
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
TranslationGeometry::TranslationGeometry(const bitpit::Config::Section & rootXML){
	
	m_direction.fill(0.0);
	m_name = "mimmo.TranslationGeometry";
	
	std::string fallback_name = "ClassNONE";	
	std::string input = rootXML.get("ClassName", fallback_name);
	input = bitpit::utils::trim(input);
	if(input == "mimmo.TranslationGeometry"){
		absorbSectionXML(rootXML);
	}else{	
		std::cout<<"Warning in custom xml mimmo::TranslationGeometry constructor. No valid xml data found"<<std::endl;
	};
}

/*!Default destructor of TranslationGeometry
 */
TranslationGeometry::~TranslationGeometry(){};

/*!Copy constructor of TranslationGeometry.
 */
TranslationGeometry::TranslationGeometry(const TranslationGeometry & other):BaseManipulation(other){
	m_direction = other.m_direction;
};

/*!Assignement operator of TranslationGeometry.
 */
TranslationGeometry & TranslationGeometry::operator=(const TranslationGeometry & other){
	*(static_cast<BaseManipulation*> (this)) = *(static_cast<const BaseManipulation*> (&other));
	m_direction = other.m_direction;
	return(*this);
};


/*! It builds the input/output ports of the object
 */
void TranslationGeometry::buildPorts(){
	bool built = true;
	built = (built && createPortIn<darray3E, TranslationGeometry>(&m_direction, PortType::M_AXIS, mimmo::pin::containerTAG::ARRAY3, mimmo::pin::dataTAG::FLOAT));
	built = (built && createPortIn<double, TranslationGeometry>(&m_alpha, PortType::M_VALUED, mimmo::pin::containerTAG::SCALAR, mimmo::pin::dataTAG::FLOAT));
    built = (built && createPortIn<dvector1D, TranslationGeometry>(this, &mimmo::TranslationGeometry::setFilter, PortType::M_FILTER, mimmo::pin::containerTAG::VECTOR, mimmo::pin::dataTAG::FLOAT));
    built = (built && createPortIn<MimmoObject*, TranslationGeometry>(&m_geometry, PortType::M_GEOM, mimmo::pin::containerTAG::SCALAR, mimmo::pin::dataTAG::MIMMO_));
    built = (built && createPortOut<dvecarr3E, TranslationGeometry>(this, &mimmo::TranslationGeometry::getDisplacements, PortType::M_GDISPLS, mimmo::pin::containerTAG::VECARR3, mimmo::pin::dataTAG::FLOAT));
	m_arePortsBuilt = built;
};

/*!It sets the origin and direction of the translation axis.
 * \param[in] direction Direction of translation axis.
 */
void
TranslationGeometry::setAxis(darray3E direction){
	m_direction = direction;
}

/*!It sets the direction of the translation axis.
 * \param[in] direction Direction of translation axis.
 */
void
TranslationGeometry::setDirection(darray3E direction){
	m_direction = direction;
	double L = norm2(m_direction);
	for (int i=0; i<3; i++)
		m_direction[i] /= L;
}

/*!It sets the value of the translation.
 * \param[in] alpha Value of translation in length unity.
 */
void
TranslationGeometry::setTranslation(double alpha){
	m_alpha = alpha;
}

/*!It sets the filter field to modulate the displacements of the vertices
 * of the target geometry.
 * \param[in] filter Filter field defined on geometry vertices.
 */
void
TranslationGeometry::setFilter(dvector1D filter){
	m_filter = filter;
}

/*!
 * Return actual computed displacements field (if any) for the geometry linked.
 * @return  The computed deformation field on the vertices of the linked geometry
 */
dvecarr3E
TranslationGeometry::getDisplacements(){
    return m_displ;
};

/*!Execution command. It perform the translation by computing the displacements
 * of the points of the geometry. It applies a filter field eventually set as input.
 */
void
TranslationGeometry::execute(){

    int nV = m_geometry->getNVertex();
    m_displ.resize(nV);
    m_filter.resize(nV, 1.0);

    long ID;
    int idx;
    liimap mapID = m_geometry->getMapDataInv();

    for (auto vertex : m_geometry->getVertices()){
        ID = vertex.getId();
        idx = mapID[ID];


        m_displ[idx] = m_alpha*m_direction*m_filter[idx];

    }

    return;
};

/*!
 * Get settings of the class from bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::absorbSectionXML.The class read essential parameters to perform translation
 * a geometry. Filter field, geometry and resulting displacements are passed mandatorily through ports
 * 
 * --> Absorbing data:
 * - <B>Priority</B>: uint marking priority in multi-chain execution;	
 * - <B>Direction</B>: axis direction coordinates
 * - <B>Translation</B>: translation value in length unity.
 * 
 * \param[in] slotXML bitpit::Config::Section which reads from
 * \param[in] name   name associated to the slot
 */
void TranslationGeometry::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){
	
	BITPIT_UNUSED(name);
	
	if(slotXML.hasOption("Priority")){
		std::string input = slotXML.get("Priority");
		int value =0;
		if(!input.empty()){
			std::stringstream ss(bitpit::utils::trim(input));
			ss>>value;
		}
		setPriority(value);
	};
	
	if(slotXML.hasOption("Direction")){
		std::string input = slotXML.get("Direction");
		input = bitpit::utils::trim(input);
		darray3E temp = {{0.0,0.0,0.0}};
		if(!input.empty()){
			std::stringstream ss(input);
			for(auto &val : temp) ss>>val;
		}
		setDirection(temp);
	} 
	
	if(slotXML.hasOption("Translation")){
		std::string input = slotXML.get("Translation");
		input = bitpit::utils::trim(input);
		double temp = 0.0;
		if(!input.empty()){
			std::stringstream ss(input);
			ss>>temp;
		}
		setTranslation(temp);
	} 
	
};	
/*!
 * Write settings of the class from bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::absorbSectionXML.The class read essential parameters to perform translation of a
 * geometry.Filter field, geometry and resulting displacements are passed mandatorily through ports
 * 
 * --> Flushing data// how to write it on XML:
 * - <B>ClassName</B>: name of the class as "mimmo.TranslationGeometry"
 * - <B>Priority</B>: uint marking priority in multi-chain execution;	
 * - <B>Direction</B>: axis direction coordinates
 * - <B>Translation</B>: translation value in length unity.
 *
 * \param[in] slotXML bitpit::Config::Section which writes to
 * \param[in] name   name associated to the slot
 */
void TranslationGeometry::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

	BITPIT_UNUSED(name);
	
	slotXML.set("ClassName", m_name);
	slotXML.set("Priority", std::to_string(getPriority()));
	
	{
		std::stringstream ss;
		ss<<std::scientific<<m_direction[0]<<'\t'<<m_direction[1]<<'\t'<<m_direction[2];
		slotXML.set("Direction", ss.str());
	}
	
	slotXML.set("Translation", std::to_string(m_alpha));
	
};	

}

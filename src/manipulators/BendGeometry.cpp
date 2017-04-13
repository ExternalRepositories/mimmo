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
#include "BendGeometry.hpp"
#include "customOperators.hpp"

using namespace bitpit;

namespace mimmo{


/*!Default constructor of BendGeometry
 */
BendGeometry::BendGeometry(){
    m_name = "mimmo.BendGeometry";
    m_degree.fill({{0,0,0}});
    m_origin.fill(0.0);
    m_system = { 1.0 , 0.0 , 0.0 , 0.0, 1.0, 0.0 , 0.0 , 0.0 , 1.0 };
    m_local = false;
};


/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
BendGeometry::BendGeometry(const bitpit::Config::Section & rootXML){

    m_name = "mimmo.BendGeometry";
    m_degree.fill({{0,0,0}});
    m_origin.fill(0.0);
    m_system = { 1.0 , 0.0 , 0.0 , 0.0, 1.0, 0.0 , 0.0 , 0.0 , 1.0 };
    m_local = false;

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::trim(input);
    if(input == "mimmo.BendGeometry"){
        absorbSectionXML(rootXML);
    }else{
        std::cout<<"Warning in custom xml mimmo::BendGeometry constructor. No valid xml data found"<<std::endl;
    };
}

/*!Default destructor of BendGeometry
 */
BendGeometry::~BendGeometry(){};

/*!Copy constructor of BendGeometry.
 */
BendGeometry::BendGeometry(const BendGeometry & other):BaseManipulation(other){
    m_origin = other.m_origin;
    m_system = other.m_system;
    m_local  = other.m_local;
    m_filter = other.m_filter;
    m_degree = other.m_degree;
    m_coeffs = other.m_coeffs;
};

/*!Assignement operator of BendGeometry.
 */
BendGeometry & BendGeometry::operator=(const BendGeometry & other){
    *(static_cast<BaseManipulation*> (this)) = *(static_cast<const BaseManipulation*> (&other));
    m_origin = other.m_origin;
    m_system = other.m_system;
    m_local  = other.m_local;
    m_filter = other.m_filter;
    m_degree = other.m_degree;
    m_coeffs = other.m_coeffs;
    return	*this;
};


/*! It builds the input/output ports of the object
 */
void
BendGeometry::buildPorts(){
    bool built = true;
    built = (built && createPortIn<MimmoObject*, BendGeometry>(&m_geometry, PortType::M_GEOM, mimmo::pin::containerTAG::SCALAR, mimmo::pin::dataTAG::MIMMO_));
    built = (built && createPortIn<umatrix33E, BendGeometry>(&m_degree, PortType::M_BMATRIX, mimmo::pin::containerTAG::ARR3ARR3, mimmo::pin::dataTAG::INT));
    built = (built && createPortIn<dmat33Evec, BendGeometry>(&m_coeffs, PortType::M_BCOEFFS, mimmo::pin::containerTAG::ARR3ARR3VEC, mimmo::pin::dataTAG::FLOAT));
    built = (built && createPortIn<dmatrix33E, BendGeometry>(&m_system, PortType::M_AXES, mimmo::pin::containerTAG::ARR3ARR3, mimmo::pin::dataTAG::FLOAT));
    built = (built && createPortIn<darray3E, BendGeometry>(&m_origin, PortType::M_POINT, mimmo::pin::containerTAG::ARRAY3, mimmo::pin::dataTAG::FLOAT));
    built = (built && createPortOut<dvecarr3E, BendGeometry>(this, &mimmo::BendGeometry::getDisplacements, PortType::M_GDISPLS, mimmo::pin::containerTAG::VECARR3, mimmo::pin::dataTAG::FLOAT));
    m_arePortsBuilt = built;
};

/*!It gets the degrees of polynomial law for each component of displacements of degrees of freedom.
 * \return Degrees of polynomial laws (degree[i][j] = degree of displacement function si = f(xj)).
 */
umatrix33E
BendGeometry::getDegree(){
    return(m_degree);
};

/*!It gets the coefficients of the polynomial laws.
 * \return Coefficients of the polynomial laws. (coeffs[i][j][k] = coefficients aijk of term si = aij * xj^k).
 */
dmat33Evec
BendGeometry::getCoeffs(){
    return(m_coeffs);
};

/*!It gets the displacements of the geometry computed during the execution.
 * \return Displacements of the points of the input MimmoObject.
 */
dvecarr3E
BendGeometry::getDisplacements(){
    return(m_displ);
};

/*!It sets the degrees of polynomial law for each component of displacements of degrees of freedom.
 * \param[in] degree Degrees of polynomial laws (degree[i][j] = degree of displacement function si = f(xj)).
 */
void
BendGeometry::setDegree(umatrix33E degree){
    for(int i=0; i<3; ++i){
        for(int j=0; j<3; ++j){
            setDegree(i,j, degree[i][j]);
        }
    }
};

/*!It sets the degrees of a term of a polynomial law for a component of displacements of degrees of freedom.
 * \param[in] i Components of displacement.
 * \param[in] j Coordinate of the function related to input degree.
 * \param[in] degree Degrees of polynomial laws (degree[i][j] = degree of displacement function si = f(xj)).
 */
void
BendGeometry::setDegree(int i, int j, uint32_t degree){
    m_degree[i][j] = degree;
    m_coeffs[i][j].resize(degree+1, 0.0);
};

/*!It sets the coefficients of the polynomial laws.
 * \param[in] coeffs Coefficients of the polynomial laws. (coeffs[i][j][k] = coefficients aijk of term si = aij * xj^k).
 */
void
BendGeometry::setCoeffs(dmat33Evec coeffs){
    m_coeffs = coeffs;
};

/*!It sets the coefficients of the polynomial laws.
 * \param[in] i Components of displacement.
 * \param[in] j Coordinate of the function related to input degree.
 * \param[in] coeffs Coefficients of the polynomial laws. (coeffs[i][j][k] = coefficients aijk of term si = aij * xj^k).
 */
void
BendGeometry::setCoeffs(int i, int j, dvector1D coeffs){
    m_coeffs[i][j] = coeffs;
};

/*!It sets the origin of the local reference system.
 * \param[in] origin Origin of local reference system.
 */
void
BendGeometry::setOrigin(darray3E origin){
    m_origin = origin;
}

/*! Set new axis orientation of the local reference system.
 * \param[in] axes new direction of all local axes.
 */
void BendGeometry::setRefSystem(dmatrix33E axes){
    m_system[0] = axes[0];
    m_system[1] = axes[1];
    m_system[2] = axes[2];
    m_local        = true;
}

/*!Execution command. It computes the nodes displacements with the polynomial law by
 * using the local reference system (by default the absolute one is used).
 * After exec() the displacements are stored in local m_displ variable.
 */
void
BendGeometry::execute(){

    if (getGeometry() == NULL) return;

    //check coherence of degrees and coeffs;
    for(int i=0; i<3; ++i){
        for(int j=0; j<3; ++j){
            m_coeffs[i][j].resize(m_degree[i][j]+1, 0.0);
        }
    }

    int nV = m_geometry->getNVertex();
    m_displ.resize(nV);
    m_filter.resize(nV, 1.0);
    long ID;
    int idx;
    liimap mapID = m_geometry->getMapDataInv();

    darray3E point;
    for (auto vertex : m_geometry->getVertices()){
        point = vertex.getCoords();
        if (m_local) point = toLocalCoord(point);
        ID = vertex.getId();
        idx = mapID[ID];
        for (int j=0; j<3; j++){
            for (int z=0; z<3; z++){
                if (m_degree[j][z] > 0){
                    for (int k=0; k<(int)m_degree[j][z]+1; k++){
                        m_displ[idx][j] += pow(point[z],(double)k)*m_coeffs[j][z][k];
                    }
                }
            }
        }
    }
    return;
};


/*! It gets the local coordinates of a point wrt the local reference system
 * \param[in] point Input point global coordinates
 * \return Local point coordinates
 */
darray3E
BendGeometry::toLocalCoord(darray3E  point){
    darray3E work, work2;
    //unapply origin translation
    work = point - m_origin;
    //apply change to local sdr transformation
    dmatrix33E transp = linearalgebra::transpose(m_system);
    linearalgebra::matmul(work, transp, work2);
    return(work);
};


/*!
 * Get settings of the class from bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::absorbSectionXML.The class read only RefreshGeometryTrees parameter, 
 * while Input and Geometry parameters are meant to be passed only through Port linking.
 * 
 * Assuming that x,y,z are the reference directions in space, as well as the preference directions
 * for nodal displacements, we can assume that:
 * 
 * --> Absorbing data:
 * - <B>Priority</B>  : uint marking priority of class execution in multichain frame	
 * - <B>DegreesMatrix(3x3)</B> : degrees of each polynomial function referred to a displacement 
 *      in direction i (x,y,z) and modulating displacement in direction j (x,y,z). Degree 0
 *      marks a constant function 
 * - <B>PolyCoefficients</B>: coefficients of each 9 bending polynomial functions.
 * 
 * \param[in] slotXML bitpit::Config::Section which reads from
 * \param[in] name   name associated to the slot
 */
void BendGeometry::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    std::string input;

    if(slotXML.hasOption("Priority")){
        input = slotXML.get("Priority");
        int value =0;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::trim(input));
            ss>>value;
        }
        setPriority(value);
    };

    if(slotXML.hasSection("DegreesMatrix")){
        auto & subslot = slotXML.getSection("DegreesMatrix");
        umatrix33E temp;
        temp.fill({{0,0,0}});

        if(subslot.hasOption("xDispl")){
            input = subslot.get("xDispl");
            std::stringstream ss(bitpit::utils::trim(input));
            for(int i=0; i<3; ++i)	ss>>temp[0][i];
        }

        if(subslot.hasOption("yDispl")){
            input = subslot.get("yDispl");
            std::stringstream ss(bitpit::utils::trim(input));
            for(int i=0; i<3; ++i)	ss>>temp[1][i];
        }

        if(subslot.hasOption("yDispl")){
            input = subslot.get("yDispl");
            std::stringstream ss(bitpit::utils::trim(input));
            for(int i=0; i<3; ++i)	ss>>temp[2][i];
        }

        setDegree(temp);
    };

    if(slotXML.hasSection("PolyCoefficients")){
        auto & subslot = slotXML.getSection("PolyCoefficients");
        dmat33Evec temp = getCoeffs();
        std::string rootPoly = "Poly";
        std::string locPoly;
        int ik,jk;
        for (int k=0; k<9; ++k){
            locPoly = rootPoly + std::to_string(k);
            if(subslot.hasOption(locPoly)){
                input = subslot.get(locPoly);
                std::stringstream ss(bitpit::utils::trim(input));
                ik = (int)(k/3);
                jk = k%3;
                for(auto & val: temp[ik][jk])	ss>>val;
            }
        }
        setCoeffs(temp);
    };
};

/*!
 * Write settings of the class to bitpit::Config::Section slot. Reimplemented from
 * BaseManipulation::flushSectionXML;
 * The class write only RefreshGeometryTrees parameter, if it is different from its default value, 
 * while Input and Geometry parameters are meant to be passed only through Port linking.
 * 
 *    --> Flushing data// how to write it on XML:
 * - <B>ClassName</B> : name of the class as "mimmo.BendGeometry"
 * - <B>Priority</B>  : uint marking priority of class execution in multichain frame	
 *
 * - <B>DegreesMatrix(3x3)</B>: degrees of each polynomial function referred to a displacement 
 * 					        in direction i (x,y,z) and modulating displacement in direction j (x,y,z). Degree 0
 * 						    marks a constant function.
 *							Written in XML as:
 * 							<DegreesMatrix>
 *								<xDispl> 1 0 0 </xDispl> (linear x-displacement distribution in x bending direction)
 *								<yDispl> 2 0 0 </yDispl> (quadratic y-displacement distribution in x bending direction)
 *								<zDispl> 0 3 0  </zDispl> (cubic z-displacement in y bending direction)
 *							</DegreesMatrix>
 *
 * - <B>PolyCoefficients</B>: coefficients of each 9 bending polynomial functions. Writing following 
 * 						  the enumeration n = i*3 + j, where i is the displacement direction and j the bending direction.
 * 						  For example n=7 corresponds to i=2, j=1, reflecting in a z displacement distribution in y-bending direction. 
 * 						  Please note the number of coefficients for a ij-bending function is equal to the degree
 * 						  of freedom DegreesMatrix(i,j), ordered as c0 + c1*x +c2*x^2+...
 * 						  Written in xml as :
 * 						  <PolyCoefficients>
 * 							<Poly0> 1.0 1.5 </Poly0>
 * 							<Poly3> -0.1 0.2 -0.01 </Poly3>
 * 							<Poly7> 1.5 0.0 0.1 0.2 </Poly7>
 * 						  </PolyCoefficients>
 * 
 * \param[in]	slotXML bitpit::Config::Section which writes to
 * \param[in] name   name associated to the slot
 */
void BendGeometry::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    slotXML.set("ClassName", m_name);
    slotXML.set("Priority", std::to_string(getPriority()));

    bitpit::Config::Section & degXML = slotXML.addSection("DegreesMatrix");

    svector1D d_str(3);
    int counter = 0;
    for(auto & val: m_degree){
        std::stringstream ss;
        for(auto & loc: val)	ss<<loc<<'\t';
        d_str[counter] = ss.str();
        ++counter;
    }

    degXML.set("xDispl", d_str[0]);
    degXML.set("yDispl", d_str[1]);
    degXML.set("zDispl", d_str[2]);

    bitpit::Config::Section & polyXML = slotXML.addSection("PolyCoefficients");
    std::string rootPoly = "Poly";
    std::string locPoly;
    for(int i=0; i<3; i++){
        for(int j =0; j<3; j++){
            if(m_coeffs[i][j].empty()) continue;
            locPoly = rootPoly + std::to_string(int(i*3+j));
            std::stringstream ss;
            for(auto & loc: m_coeffs[i][j])	ss<<loc<<'\t';
            polyXML.set(locPoly, ss.str());
        }
    }

};	

}


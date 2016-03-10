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
 \ *---------------------------------------------------------------------------*/
#include "BasicShapes.hpp"
#include "customOperators.hpp"

using namespace bitpit;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BasicShape IMPLEMENTATION 
/*
 *	\date			03/01/2015
 *  \authors		Edoardo Lombardi
 *	\authors		Arpa Rocco
 *	\copyright		Copyright 2015 Optimad engineering srl. All rights reserved.
 *	\par			License:\n
 *
 *	\brief Abstract Interface class for Elementary Shape Representation
 *
 *	Interface class for Volumetric Core Element, suitable for interaction with Data Structure stored in a MimmoObject class.
 *  Object orientation in 3D space can be externally manipulated with dedicated transformation blocks. Class 
 *  internally implement transformation to/from local sdr to/from world sdr, that can be used in derived objects from it   
 */
 
/*! Basic Constructor */
BasicShape::BasicShape(){

	m_shape = ShapeType::CUBE;
	m_origin.fill(0.0);
	m_span.fill(0.0);
	m_infLimits.fill(0.0);
	for(int i=0; i<3; ++i){
		m_sdr[i].fill(0.0);
	}
	m_sdr[0][0] = m_sdr[1][1] = m_sdr[2][2] = 1.0;
	m_closedLoops.resize(3, false);
	m_scaling.fill(1.0);
};

/*! Basic Destructor */
BasicShape::~BasicShape(){};

/*! Set origin of your shape. The origin is meant as the baricenter of your shape in absolute r.s.
 * \param[in] origin new origin point
 */
void BasicShape::setOrigin(darray3E origin){
	m_origin = origin;
}

/*! Set span of your shape, according to its local reference system 
 * \param[in] s0 first coordinate span
 * \param[in] s1 second coordinate span
 * \param[in] s2 third coordinate span
 */
void BasicShape::setSpan(double s0, double s1, double s2){
	checkSpan(s0,s1,s2);
	setScaling(s0,s1,s2);
}

/*! Set coordinate's origin of your shape, according to its local reference system  
 * \param[in] orig first coordinate origin
 * \param[in] dir 0,1,2 int flag identifying coordinate
 */
void BasicShape::setInfLimits(double orig, int dir){
	if(dir<0 || dir>2) return;
	
	darray3E span = getSpan();
	bool check = checkInfLimits(orig,dir);
	if(check){
		m_infLimits[dir] = orig; 
		setSpan(span[0],span[1],span[2]);
	}
}

/*! Set new axis orientation of the local reference system
 * \param[in] axis0 first axis
 * \param[in] axis1 second axis
 * \param[in] axis2 third axis
 * 
 * if chosen axes are not orthogonal, doing nothing
 */
void BasicShape::setRefSystem(darray3E axis0, darray3E axis1, darray3E axis2){
	
	axis0 = axis0/norm2(axis0);
	axis1 = axis1/norm2(axis1);
	axis2 = axis2/norm2(axis2);
	
	double tol = 1.0e-12;
	double check = dotProduct(axis0,axis1) + dotProduct(axis1,axis2) + dotProduct(axis0,axis2);
	if(check > tol) return;
	m_sdr[0] = axis0;
	m_sdr[1] = axis1;
	m_sdr[2] = axis2;
}

/*! Set new axis orientation of the local reference system
 * \param[in] int 0,1,2 identify local x,y,z axis of the primitive shape
 * \param[in] axis new direction of selected local axis.
 */
void BasicShape::setRefSystem(int label, darray3E axis){
	
	if(label <0 || label >2 ) return;
	
	m_sdr[label] = axis/norm2(axis);
	dvecarr3E point_mat(3,darray3E{0,0,0});
	point_mat[0][0] = point_mat[1][1]= point_mat[2][2]=1.0;
	
	int next_label = (label + 1)%3;
	int fin_label = (label + 2)%3;
	
	double pj = dotProduct(point_mat[next_label], axis);
	m_sdr[next_label] = point_mat[next_label] - pj*axis;
	m_sdr[next_label] = m_sdr[next_label]/norm2(m_sdr[next_label]);
	
	m_sdr[fin_label] = crossProduct(m_sdr[label],m_sdr[next_label]);
	m_sdr[fin_label] = m_sdr[fin_label]/norm2(m_sdr[fin_label]);
}


/*! Set booleans to treat your shape coordinates as periodic (true) or regular (false)
 * \param[in] flag true/false to mark coordinate as periodic/regular 
 * \param[in] dir  0,1,2 int flag identifying coordinate
 */
void BasicShape::setClosedLoops(bool flag, int dir){
	m_closedLoops[dir] = flag;
}

/*! Return current origin of your shape
 * \param[out] result origin
 */
darray3E BasicShape::getOrigin(){
	return(m_origin);
}

/*! Return current span of your shape
 * \param[out] result span
 */
darray3E BasicShape::getSpan(){
	darray3E result = getLocalSpan();
	darray3E scale = getScaling();
	for(int i; i<3; ++i){
		result[i] = scale[i]*result[i];
	}
	return(result);
}

/*! Return current coordinates' origin of your shape, in local coord system
 * \param[out] result coords origin
 */
darray3E BasicShape::getInfLimits(){
	return(m_infLimits);
}

/*! Return actual axis of global relative sdr
 * \param[out] result relative sdr
 */
dmatrix33E BasicShape::getRefSystem(){
	return(m_sdr);
}

/*! Return if your current shape coordinate "dir" is set as periodic or not
 * \param[in] result boolean flag , true if dir is periodic
 * \param[in] dir   0,1,2 int flag identifying coordinate
 */
bool BasicShape::areClosedLoops(int dir){
	return(m_closedLoops[dir]);
}
/*! Get current type of shape instantiated
 * \param[out] result BasicShape::ShapeType enum
 */
BasicShape::ShapeType BasicShape::getShapeType(){
	return(m_shape);
};

/*! Get current type of shape instantiated. Const method version
 * \param[out] result const BasicShape::ShapeType enum
 */
const BasicShape::ShapeType BasicShape::getShapeType() const {
	return(m_shape);
};

/*! Get current scaling w.r.t the primitive unitary shape*/
darray3E BasicShape::getScaling(){
	return(m_scaling);
}

/*! Return span of your elementary shape, in local coord system
 * \param[out] result span
 */
darray3E BasicShape::getLocalSpan(){
	return(m_span);
}

/*! Given a bitpit class bitpit::Patch tessellation, return cell identifiers of those simplex inside the volume of
 * the BasicShape object
 * \param[in] tri target tessellation
 * \param[out] result list-by-ids of simplicies included in the volumetric patch
 */
ivector1D BasicShape::includeGeometry(bitpit::PatchKernel * tri ){
  
  int nCells = tri->getCellCount();	
  ivector1D result(nCells); 
  int counter=0;
  
  for(auto &cell : tri->cells()){
   
    if(isSimplexIncluded(tri, cell.get_id())){
      result[counter] = cell.get_id();
      ++counter;
    }
  }
  result.resize(counter);
  return(result);
};

/*! Given a bitpit class bitpit::Patch tessellation, return cell identifiers of those simplex outside the volume of
 * the BasicShape object 
 * \param[in] tri target tesselation
 * \param[out] result list-by-ids of simplicies outside the volumetric patch
 */
ivector1D BasicShape::excludeGeometry(bitpit::PatchKernel * tri){
  
	int nCells = tri->getCellCount();	
	ivector1D result(nCells); 
	int counter=0;
	
	for(auto &cell : tri->cells()){
		
		if(!isSimplexIncluded(tri, cell.get_id())){
			result[counter] = cell.get_id();
			++counter;
		}
	}
	result.resize(counter);
	return(result);
};

/*! Given a list of vertices of a point cloud, return indices of those vertices included into 
 * the volume of BaseSelPatch object 
 * \param[in] list list of cloud points
 * \param[out] result list-by-indices of vertices included in the volumetric patch
 */
ivector1D BasicShape::includeCloudPoints(dvecarr3E & list){

  int size = list.size();
  ivector1D result(size); 
  int counter=0;
  
  for(int i=0; i<size; ++i){
   
    if(isPointIncluded(list[i])){
      result[counter] = i;
      ++counter;
    }
  }
  result.resize(counter);
  return(result);
};

/*! Given a list of vertices of a point cloud, return indices of those vertices outside 
 * the volume of BasicShape object 
 * \param[in] list list of cloud points
 * \param[out] result list-by-indices of vertices outside the volumetric patch
 */
ivector1D BasicShape::excludeCloudPoints(dvecarr3E & list){
  
  int size = list.size();
  ivector1D result(size); 
  int counter=0;
  
  for(int i=0; i<size; ++i){
   
    if(!isPointIncluded(list[i])){
      result[counter] = i;
      ++counter;
    }
  }
  result.resize(counter);
  return(result);
  
};

/*! Given a bitpit class bitpit::Patch point cloud, return identifiers of those points inside the volume of
 * the BasicShape object  
 * \param[in] list list of cloud points
 * \param[out] result list-by-indices of vertices included in the volumetric patch
 */
ivector1D BasicShape::includeCloudPoints(bitpit::PatchKernel * tri){
	
	int nVert = tri->getVertexCount();	
	ivector1D result(nVert); 
	int counter=0;
	
	for(auto &vertex : tri->vertices()){
		
		if(isPointIncluded(tri, vertex.get_id())){
			result[counter] = vertex.get_id();
			++counter;
		}
	}
	result.resize(counter);
	return(result);
};

/*! Given a bitpit class bitpit::Patch point cloud, return identifiers of those points outside the volume of
 * the BasicShape object  
 * \param[in] list list of cloud points
 * \param[out] result list-by-indices of vertices outside the volumetric patch
 */
ivector1D BasicShape::excludeCloudPoints(bitpit::PatchKernel * tri){
	
	int nVert = tri->getVertexCount();	
	ivector1D result(nVert); 
	int counter=0;
	
	for(auto &vertex : tri->vertices()){
		
		if(!isPointIncluded(tri, vertex.get_id())){
			result[counter] = vertex.get_id();
			++counter;
		}
	}
	result.resize(counter);
	return(result);	
};

/*! Return True if at least one vertex of a given triangle is included in the volume of the shape
 * \param[in] simplexVert 3 vertices of the given Triangle
 * \param[out] result boolean
 */   
bool BasicShape::isSimplexIncluded(dvecarr3E & simplexVert){
  
  bool check = false;
  for(int i=0; i<simplexVert.size(); ++i){
   check = check || isPointIncluded(simplexVert[i]); 
  }
  return(check);
};

/*! Return True if at least one vertex of a given triangle is included in the volume of the shape
 * \param[in] tri pointer to a Class_SurfTri tesselation 
 * \param[in] indexT triangle index of tri.
 * \param[out] result boolean
 */ 
bool BasicShape::isSimplexIncluded(bitpit::PatchKernel * tri, int indexT){

  Cell cell = tri->getCell(indexT);
  long * conn = cell.getConnect();
  int nVertices = cell.getVertexCount();
  bool check = false;
  for(int i=0; i<nVertices; ++i){ 
	//recover vertex index
	check = check || isPointIncluded(tri, conn[i]); 
  }
  return(check);
};

/*! Return True if the given point is included in the volume of the patch
 * \param[in] point given vertex
 * \param[out] result boolean
 */
bool BasicShape::isPointIncluded(darray3E point){
	
	bool check = true;
	darray3E temp = toLocalCoord(point);
	darray3E temp2 = localToBasic(temp);
	
	for(int i=0; i<3; ++i){  
		check = check && ((temp2[i] >= 0.0) && (temp2[i]<=1.0));
	}
	
	return(check);  
};

/*! Return True if the given point is included in the volume of the patch
 * \param[in] tri pointer to a bitpit::Patch tesselation / point cloud
 * \param[in] indexV id of a vertex belonging to tri;
 * \param[out] result boolean
 */
bool BasicShape::isPointIncluded(bitpit::PatchKernel * tri, int indexV){
	
	bool check = true;
	darray3E coords = tri->getVertex(indexV).getCoords();
	return(isPointIncluded(coords));  
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Cube IMPLEMENTATION 
/*
 *	\date			03/01/2015
 *	\authors		Edoardo Lombardi
 *	\authors		Arpa Rocco
 *	\copyright		Copyright 2015 Optimad engineering srl. All rights reserved.
 *	\par			License:\n
 *
 *	\brief Elementary Shape Representation of a Cube
 *
 *	Volumetric Core Element, shaped as a cube, directly derived from BasicShape class.   
 */
   
/*! Basic Constructor */
Cube::Cube(){
	m_shape=ShapeType::CUBE;
};

 /*! Custom Constructor. Set shape origin, span of the cube,
  * ordered as width, height, span. 
   * \param[in] origin_ point origin in global reference system
   * \param[in] span span in each shape local coordinate x,y,z;
   */
 Cube::Cube(darray3E &origin, darray3E & span): Cube(){
	      
	setOrigin(origin);
	setSpan(span[0], span[1], span[2]);
}; 

/*! Basic Destructor */
Cube::~Cube(){};


/*! Copy Constructor 
 * \param[in] other Cube object where copy from
 */
Cube::Cube(const Cube & other){
	*this = other;
};

/*! Copy Operator 
 * \param[in] other Cube object where copy from
 */
Cube & Cube::operator=(const Cube & other){
	
	m_shape = other.m_shape;
	m_origin = other.m_origin;
	m_span = other.m_span;
	m_infLimits = other.m_infLimits;
	m_sdr = other.m_sdr;
	m_closedLoops = other.m_closedLoops;
	m_scaling = other.m_scaling;
	return(*this);
};


/*! Transform point from local reference system of the shape,
 * to world reference system. 
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cube::toWorldCoord(darray3E & point){
	
	darray3E work, work2;
	//unscale your local point
	for(int i =0; i<3; ++i){
		work[i] = point[i]*m_scaling[i];
	}
	
	//return to local xyz system
	// -> cube, doing nothing
	
	//unapply change to local sdr transformation
	linearalgebra::matmul(work, m_sdr, work2);
	
	//unapply origin translation
	work = work2 + m_origin;
	return(work);
};
/*! Transform point from world coordinate system, to local reference system 
 * of the shape.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cube::toLocalCoord(darray3E & point){

	darray3E work, work2;

	//unapply origin translation
	work = point - m_origin;

	//apply change to local sdr transformation
	dmatrix33E transp = linearalgebra::transpose(m_sdr);
	linearalgebra::matmul(work, transp, work2);
	
	//get to proper local system
	// -> cube, doing nothing
	
	//scale your local point
	for(int i =0; i<3; ++i){
		work[i] = work2[i]/m_scaling[i];
	}
	
	return(work);
	
};

/*! Return local origin of your primitive shape*/
darray3E	Cube::getLocalOrigin(){
	return(darray3E{-0.5,-0.5,-0.5});
};

/*! Transform point from unitary cube reference system, to local reference system 
 * of the shape. Practically, doing nothing!
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cube::basicToLocal(darray3E & point){
	return(point + getLocalOrigin());
};

/*! Transform point from local reference system of the shape,
 * to unitary cube reference system. Practically, doing nothing!
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cube::localToBasic(darray3E & point){
	return(point - getLocalOrigin());
};

/*! Check if your new span values fit your current shape set up
 * and eventually return correct values.
 */
void 		Cube::checkSpan(double &s0, double &s1, double &s2){
			s0 = std::abs(s0);
			s1 = std::abs(s1);
			s2 = std::abs(s2);
};

/*! Check if your coords origin values fit your current shape set up
 * and eventually return correct values. Return true, if valid new value is set.
 */
bool 		Cube::checkInfLimits(double &o0, int &dir){
			//really doing nothing here.
			//whatever origin for whatever coordinate must return always 0 for cubic/cuboidal shape
			return(false);
};

/*! set local span & scaling vectors of your object */
void 		Cube::setScaling( double &s0, double &s1, double &s2){
			m_span.fill(1.0);
			m_scaling[0] = s0;
			m_scaling[1] = s1;
			m_scaling[2] = s2;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Cylinder IMPLEMENTATION 
/*
 *	\date			03/01/2015
 *	\authors		Edoardo Lombardi
 *	\authors		Arpa Rocco
 *	\copyright		Copyright 2015 Optimad engineering srl. All rights reserved.
 *	\par			License:\n
 *
 *	\brief Elementary Shape Representation of a Cylinder or portion of it
 *
 *	Volumetric Core Element, shaped as a cylinder, directly derived from BasicShape class.   
 */

/*! Basic Constructor */
Cylinder::Cylinder(){
	m_shape=ShapeType::CYLINDER;
};

/*! Custom Constructor. Set shape origin, inferior/superior limits of the cylinder
 * ordered as basis radius, azimuthal/tangential coordinate, height. 
 * \param[in] origin_ point origin in global reference system
 * \param[in] limits inf/sup limit for each shape coordinate;
 */
Cylinder::Cylinder(darray3E &origin, darray3E & span): Cylinder(){
	
	setOrigin(origin);
	setSpan(span[0], span[1], span[2]);
}; 

/*! Basic Destructor */
Cylinder::~Cylinder(){};


/*! Copy Constructor 
 * \param[in] other Cylinder object where copy from
 */
Cylinder::Cylinder(const Cylinder & other){
	*this = other;
};

/*! Copy Operator 
 * \param[in] other Cylinder object where copy from
 */
Cylinder & Cylinder::operator=(const Cylinder & other){
	
	m_shape = other.m_shape;
	m_origin = other.m_origin;
	m_span = other.m_span;
	m_infLimits = other.m_infLimits;
	m_sdr = other.m_sdr;
	m_closedLoops = other.m_closedLoops;
	m_scaling = other.m_scaling;
	return(*this);
};


/*! Transform point from local reference system of the shape,
 * to world reference system. 
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cylinder::toWorldCoord(darray3E & point){
	
	darray3E work, work2;
	//unscale your local point
	for(int i =0; i<3; ++i){
		work[i] = point[i]*m_scaling[i];
	}
	
	//return to local xyz system
	work2[0] = work[0]*std::cos(work[1] + m_infLimits[1]); 
	work2[1] = work[0]*std::sin(work[1] + m_infLimits[1]); 
	work2[2] = work[2];
	
	//unapply change to local sdr transformation
	work.fill(0.0);
	linearalgebra::matmul(work2, m_sdr, work);
	
	//unapply origin translation
	work2 = work + m_origin;
	
	return(work2);
};
/*! Transform point from world coordinate system, to local reference system 
 * of the shape.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cylinder::toLocalCoord(darray3E & point){
	darray3E work, work2;
	
	//unapply origin translation
	work = point - m_origin;
	
	//apply change to local sdr transformation
	dmatrix33E transp = linearalgebra::transpose(m_sdr);
	linearalgebra::matmul(work, transp, work2);
	
	//get to proper local system
	if(work2[0] ==0.0 && work2[1] ==0.0){work[0] = 0.0; work[1] = 0.0;}
	else{
		work[0] = pow(work2[0]*work2[0] + work2[1]*work2[1],0.5);
		double pdum = std::atan2(work2[1],work2[0]);
		work[1] = pdum - 4.0*(getSign(pdum)-1.0)*std::atan(1.0); 
	}
	//get to the correct m_thetaOrigin mark
	double param = 8*std::atan(1.0);
	work[1] = work[1] - m_infLimits[1];
	if(work[1] < 0) 		work[1] = param + work[1];
	if(work[1] > param) 	work[1] = work[1] - param;
	
	work[2] = work2[2];
	
	//scale your local point
	for(int i =0; i<3; ++i){
		work2[i] = work[i]/m_scaling[i];
	}
	return(work2);
};

/*! Return local origin of your primitive shape*/
darray3E	Cylinder::getLocalOrigin(){
	return(darray3E{0.0,0.0,-0.5});
};

/*! Transform point from unitary cube reference system, to local reference system 
 * of the shape.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cylinder::basicToLocal(darray3E & point){
	point[1] = point[1]*m_span[1];
	point[2] = point[2]-0.5;
	return(point);
};

/*! Transform point from local reference system of the shape,
 * to unitary cube reference system.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Cylinder::localToBasic(darray3E & point){
	point[1] = point[1]/m_span[1];
	point[2] = point[2]+0.5;
	return(point);
};

/*! Check if your new span values fit your current shape set up
 * and eventually return correct values.
 */
void 		Cylinder::checkSpan(double &s0, double &s1, double &s2){
	s0 = std::abs(s0);
	s1 = std::abs(s1);
	s2 = std::abs(s2);
	
	double thetalim = 8.0* std::atan(1.0);
	s1 = std::min(s1, thetalim);
	//check closedLoops;
	setClosedLoops(!(s1 < thetalim),1);
};

/*! Check if your coords origin values fit your current shape set up
 * and eventually return correct values. Return true, if valid new value is set.
 */
bool 	Cylinder::checkInfLimits(double &orig, int &dir){
	double thetalim = 8.0* std::atan(1.0);
	bool check = false;
	switch(dir){
		case 1: 
			orig = std::min(thetalim, std::max(0.0, orig));
			check = true;
			break;
		default:	// doing nothing
			break;
	}
	return(check);
};

/*! set span and scaling vectors of your object */
void 		Cylinder::setScaling(double &s0, double &s1, double &s2){
		m_span.fill(1.0);
		m_scaling.fill(1.0);
		m_span[1] = s1;
		m_scaling[0] = s0;
		m_scaling[2] = s2;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere IMPLEMENTATION 
/*
 *	\date			03/01/2015
 *	\authors		Edoardo Lombardi
 *	\authors		Arpa Rocco
 *	\copyright		Copyright 2015 Optimad engineering srl. All rights reserved.
 *	\par			License:\n
 *
 *	\brief Elementary Shape Representation of a sphere or portion of it
 *
 *	Volumetric Core Element, shaped as a sphere, directly derived from BasicShape class.   
 */

/*! Basic Constructor */
Sphere::Sphere(){
	m_shape=ShapeType::SPHERE;
};

/*! Custom Constructor. Set shape origin, inferior/superior limits of the sphere
 * ordered as overall radius, azimuthal/tangential coordinate, polar coordinate. 
 * \param[in] origin_ point origin in global reference system
 * \param[in] limits inf/sup limit for each shape coordinate;
 */
Sphere::Sphere(darray3E &origin, darray3E & span): Sphere(){
	
	setOrigin(origin);
	setSpan(span[0], span[1], span[2]);
}; 

/*! Basic Destructor */
Sphere::~Sphere(){};

/*! Copy Constructor 
 * \param[in] other Sphere object where copy from
 */
Sphere::Sphere(const Sphere & other){
	*this = other;
};

/*! Copy Operator 
 * \param[in] other Sphere object where copy from
 */
Sphere & Sphere::operator=(const Sphere & other){
	
	m_shape = other.m_shape;
	m_origin = other.m_origin;
	m_span = other.m_span;
	m_infLimits = other.m_infLimits;
	m_sdr = other.m_sdr;
	m_closedLoops = other.m_closedLoops;
	m_scaling = other.m_scaling;
	return(*this);
};

/*! Transform point from local reference system of the shape,
 * to world reference system. 
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Sphere::toWorldCoord(darray3E & point){
	
	darray3E work, work2;
	//unscale your local point
	for(int i =0; i<3; ++i){
		work[i] = point[i]*m_scaling[i];
	}
	
	//return to local xyz system
	work2[0] = work[0]*std::cos(work[1] + m_infLimits[1])*std::sin(work[2] + m_infLimits[2]); 
	work2[1] = work[0]*std::sin(work[1] + m_infLimits[1])*std::sin(work[2] + m_infLimits[2]); 
	work2[2] = work[0]*std::cos(work[2] + m_infLimits[2]);
	
	//unapply change to local sdr transformation
	work.fill(0.0);
	linearalgebra::matmul(work2, m_sdr, work);
	
	//unapply origin translation
	work2 = work + m_origin;
	return(work2);
};
/*! Transform point from world coordinate system, to local reference system 
 * of the shape.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Sphere::toLocalCoord(darray3E & point){
	
	darray3E work, work2;
	//unapply origin translation
	work = point - m_origin;
	
	//apply change to local sdr transformation
	dmatrix33E transp = linearalgebra::transpose(m_sdr);
	linearalgebra::matmul(work, transp, work2);
	
	//get to proper local system
	work[0] = norm2(work2);
	
	if(work[0]>0.0){
		if(work2[0] ==0.0 && work2[1] ==0.0){
			work[1] = 0.0;
		}else{
			double pdum = std::atan2(work2[1],work2[0]);
			work[1] = pdum - 4.0*(getSign(pdum)-1.0)*std::atan(1.0); 
		}
		//get to the correct m_thetaOrigin mark
		double param = 8*std::atan(1.0);
		work[1] = work[1] - m_infLimits[1];
		if(work[1] < 0) 		work[1] = param + work[1];
		if(work[1] > param) 	work[1] = work[1] - param;
	
		work[2] = std::acos(work2[2]/work[0]);
		work[2] = work[2] - m_infLimits[2];
	}
	
	//scale your local point
	for(int i =0; i<3; ++i){
		work2[i] = work[i]/m_scaling[i];
	}
	return(work2);
};

/*! Return local origin of your primitive shape*/
darray3E	Sphere::getLocalOrigin(){
	return(darray3E{0.0,0.0,0.0});
};

/*! Transform point from unitary cube reference system, to local reference system 
 * of the shape.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Sphere::basicToLocal(darray3E & point){
	point[1] = point[1]*m_span[1];
	point[2] = point[2]*m_span[2];
	return(point);
};

/*! Transform point from local reference system of the shape,
 * to unitary cube reference system.
 * \param[in] point target
 * \param[out] result transformed point
 */
darray3E	Sphere::localToBasic(darray3E & point){
	point[1] = point[1]/m_span[1];
	point[2] = point[2]/m_span[2];
	return(point);
};

/*! Check if your new span values fit your current shape set up
 * and eventually return correct values.
 */
void 		Sphere::checkSpan(double &s0, double &s1, double &s2){
	s0 = std::abs(s0);
	s1 = std::abs(s1);
	s2 = std::abs(s2);
	
	double thetalim = 8.0* std::atan(1.0);
	s1 = std::min(s1, thetalim);
	
	double maxS2 = 0.5*thetalim - m_infLimits[2];
	s2 = std::min(s2, maxS2);
	
	//check closedLoops;
	setClosedLoops(!(s1 < thetalim),1);
	
};

/*! Check if your coords origin values fit your current shape set up
 * and eventually return correct values.
 */
bool 		Sphere::checkInfLimits(double &orig, int &dir){
	
	double thetalim = 8.0* std::atan(1.0);
	double tol = 1.e-12;
	bool check = false;
	switch(dir){
		case 1: 
			orig = std::min(thetalim, std::max(0.0, orig));
			check = true;
			break;
		case 2: 
			orig = std::min((0.5-tol)*thetalim, std::max(0.0, orig));
			check = true;
			break;
		default:	// doing nothing
			break;
	}
	return(check);
};

/*! set scaling vector of your object */
void 		Sphere::setScaling(double &s0, double &s1, double &s2){
	m_span.fill(1.0);
	m_scaling.fill(1.0);
	
	m_scaling[0] = s0;
	m_span[1] = s1;
	m_span[2] = s2;
};

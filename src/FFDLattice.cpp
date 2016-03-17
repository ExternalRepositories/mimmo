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

#include "FFDLattice.hpp"

using namespace std;

// IMPLEMENTATION OF FFDLATTICE ***********************************************//
/*
 *	\date			09/feb/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief Free Form Deformation of a 3D surface and point clouds, with structured lattice.
 *
 *	Free Form deformation tool for 3D geometries (surface and point clouds). Basically, it builds an elemental 3D shape 
 *  (box, sphere, cylinder or part of them) around the geometry and set a structured cartesian mesh of control 
 *  points on it (lattice). Displacements of each control point is linked to the geometry inside 
 *  the shape by means of a NURBS volumetric parameterization. Deformation will be applied only to 
 *  those portion of geometry encased into the 3D shape.
 *
 */

/*! Basic Constructor. Doing nothing.*/
FFDLattice::FFDLattice(){
	m_knots.resize(3);
	m_mapEff.resize(3);
	m_deg.resize(3,1);
	m_mapNodes.resize(3);
	m_globalDispl = false;
};
/*! Custom constructor.Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *   
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type BasicShape::ShapeType enum identifies the shape
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
FFDLattice::FFDLattice(darray3E &origin, darray3E & span, BasicShape::ShapeType type, ivector1D &dimensions, 
						ivector1D & degrees):FFDLattice(){
	setMesh(origin, span, type, dimensions, degrees);
};

/*! Custom Constructor.Set lattice mesh, dimensions and curve degree for Rational Bezier trivariate parameterization.
 *  Knots structure is built with curve degrees as in case of a Pure Bezier Volumetric 
 *  Parameterization, that is degX = nx-1, degY = ny-1, degZ=nz-1.
 *   
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type BasicShape::ShapeType enum identifies the shape
 * \param[in] dimensions number of control nodes for each direction
 */
FFDLattice::FFDLattice(darray3E &origin, darray3E & span, BasicShape::ShapeType type, ivector1D &dimensions 
					   ):FFDLattice(){
	   setMesh(origin, span, type, dimensions);
};

/*! Custom Constructor.Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *   
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
FFDLattice::FFDLattice(BasicShape * shape, ivector1D &dimensions, ivector1D & degrees):FFDLattice(){
	setMesh(shape, dimensions, degrees);
};

/*! Set lattice mesh, dimensions and curve degree for Rational Bezier trivariate parameterization.
 *  Knots structure is built with curve degrees as in case of a Pure Bezier Volumetric 
 *  Parameterization, that is degX = nx-1, degY = ny-1, degZ=nz-1.
 *   
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] dimensions number of control nodes for each direction
 *
 */
FFDLattice::FFDLattice(BasicShape * shape, ivector1D &dimensions):FFDLattice(){
	setMesh(shape, dimensions);
};

/*! Destructor */
FFDLattice::~FFDLattice(){};

/*! Copy Constructor
 *\param[in] other FFDLattice where copy from
 */ 
FFDLattice::FFDLattice(const FFDLattice & other){
	*this = other;
};

/*! Copy Operator
 * \param[in] other FFDLattice where copy from
 */ 
FFDLattice & FFDLattice::operator=(const FFDLattice & other){
	
	*(static_cast<UStructMesh *>(this))  = *(static_cast<const UStructMesh *>(&other));

	m_deg = other.m_deg;
	m_knots = other.m_knots;
	m_mapEff = other.m_mapEff;
	m_weights = other.m_weights;
	m_globalDispl = other.m_globalDispl;
	m_mapdeg = other.m_mapdeg;
	m_globalDispl = other.m_globalDispl;
	m_intMapDOF = other.m_intMapDOF;
	return(*this);
};

/*!Clean all stuffs in your lattice */
void FFDLattice::clearLattice(){
		clear(); //base manipulation stuff clear
		clearMesh(); // structured mesh cleaned
		clearKnots(); //clear all knots stuff;
};

/*! Return a vector of six elements reporting the real number of knots effectively stored in the current class (first 3 elements)
 * and the theoretical number of knots (last 3 elements) for Nurbs representation (see Nurbs Books of Peigl)
 * \param[out] result six element vector
 */
ivector1D 	FFDLattice::getKnotsDimension(){
	ivector1D res(6,0);
	
		//effectively stored
		res[0] = m_knots[0].size();
		res[1] = m_knots[1].size();
		res[2] = m_knots[2].size();
		
		//theoretical stored
		res[3] = m_mapEff[0].size();
		res[4] = m_mapEff[1].size();
		res[5] = m_mapEff[2].size();
		return(res);
};

/*! Return weight actually set for each control node 
 * \param[out] result list of weights
 */
dvector1D	FFDLattice::getWeights(){
	return(recoverFullNodeWeights());
};

/*! Return knots structure and theoretical map of knots distributions for the current Lattice
 * \param[out] knots knots list 
 * \param[out] mapTheo  map of knots theoretical distribution
 */ 
void 		FFDLattice::returnKnotsStructure(dvector2D & knots, ivector2D &mapTheo){
	knots.resize(3);
	mapTheo.resize(3);
	
	returnKnotsStructure(0, knots[0], mapTheo[0]);
	returnKnotsStructure(1, knots[1], mapTheo[1]);
	returnKnotsStructure(2, knots[2], mapTheo[2]);
};

/*! Return knots structure and knot multiplicity vector for a specified Nurbs curve "dir"
 * \param[in] dir integer (0,1,2) identifies nurbs curve in x,y,and z direction respectively
 * \param[out] knots knots list
 * \param[out] mapT theoretical knot map distribution
 */ 
void 		FFDLattice::returnKnotsStructure( int dir, dvector1D & knots, ivector1D & mapT){
	if(dir<0 || dir>2){return;}
	
	knots.resize(m_knots[dir].size());
	mapT.resize(m_mapEff[dir].size());
	
	knots = m_knots[dir];   
	mapT = m_mapEff[dir];
};

/*! Set number of control nodes in each space direction.Nurbs curves are treated as
 * Bezier curves, their degree is automatically set. Weights are reset to unitary value
 * \param[in] dimension vector of control nodes numbers in each direction
 */
void		FFDLattice::setDimension(ivector1D &dimensions){
		
		if(getShape() ==NULL) return;
		if(dimensions.size() < 3 || getShape() ==NULL) return;
		ivector1D dimLimit(3,2);
		switch(getShapeType()){
			case BasicShape::ShapeType::CYLINDER :
				dimLimit[1] = 5;
				break;
			case BasicShape::ShapeType::SPHERE :
				dimLimit[1] = 5; dimLimit[2] = 3;
				break;
			default://CUBE
				break;
		}
		
		//check on dimensions and eventual closed loops on coordinates.
		m_nx = std::max(dimensions[0], dimLimit[0])-1;
		m_ny = std::max(dimensions[1], dimLimit[1])-1;
		m_nz = std::max(dimensions[2], dimLimit[2])-1;
		
		rebaseMesh();
		
		m_deg[0] = m_nx;
		m_deg[1] = m_ny;
		m_deg[2] = m_nz;
	
		//setting knots and eventually weights to non-rational B-Spline
		setKnotsStructure();
		resizeDisplacements(m_nx+1, m_ny+1, m_nz+1);
		m_weights.resize(m_ndeg, 1.0);
		
		//reorder dimensions
		orderDimension();

};

/*! Set number of control nodes in each space direction and degrees of Nurbs curves. 
 *  Weights are reset to unitary value
 * \param[in] dimension vector of control nodes numbers in each direction
 * \param[in] degrees vector of degree of nurbs curve in each direction
 */
void		FFDLattice::setDimension(ivector1D &dimensions, ivector1D &degrees){
	
	if(dimensions.size() < 3 || degrees.size() <3 || getShape() ==NULL) return;
	
	ivector1D dimLimit(3,2);
	switch(getShapeType()){
		case BasicShape::ShapeType::CYLINDER :
			dimLimit[1] = 5;
			break;
		case BasicShape::ShapeType::SPHERE :
			dimLimit[1] = 5; dimLimit[2] = 3;
			break;
		default://CUBE
			break;
	}
	
	//check on dimensions and eventual closed loops on coordinates.
	m_nx = std::max(dimensions[0], dimLimit[0])-1;
	m_ny = std::max(dimensions[1], dimLimit[1])-1;
	m_nz = std::max(dimensions[2], dimLimit[2])-1;
	
	rebaseMesh();
	
	m_deg[0] = std::min(m_nx,std::max(1,degrees[0]));
	m_deg[1] = std::min(m_ny,std::max(1,degrees[1]));
	m_deg[2] = std::min(m_nz,std::max(1,degrees[2]));
	
	//setting knots and eventually weights to non-rational B-Spline
	setKnotsStructure();
	
	resizeDisplacements(m_nx+1, m_ny+1, m_nz+1);
	m_weights.resize(m_ndeg, 1.0);
	
	//reorder dimensions
	orderDimension();

};

/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *   
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type BasicShape::ShapeType enum identifies the shape
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
void FFDLattice::setMesh(darray3E &origin,darray3E & span, BasicShape::ShapeType type, ivector1D & dimensions, ivector1D & degrees){
	
	clearMesh();
	UStructMesh::setMesh(origin,span,type,dimensions);
	
	m_deg[0] = std::min(m_nx,std::max(1,degrees[0]));
	m_deg[1] = std::min(m_ny,std::max(1,degrees[1]));
	m_deg[2] = std::min(m_nz,std::max(1,degrees[2]));
	
	//setting knots and eventually weights to non-rational B-Spline
	setKnotsStructure();
	
	//reallocate your displacement node
	ivector1D dd = getDimension();
	resizeDisplacements(dd[0],dd[1],dd[2]);
	//reset your weights
	m_weights.resize(m_ndeg, 1.0);
	
	//reorder dimensions
	orderDimension();
};

/*! Set lattice mesh, dimensions and curve degree for Rational Bezier trivariate parameterization.
 *  Knots structure is built with curve degrees as in case of a Pure Bezier Volumetric 
 *  Parameterization, that is degX = nx-1, degY = ny-1, degZ=nz-1.
 *   
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type BasicShape::ShapeType enum identifies the shape
 * \param[in] dimensions number of control nodes for each direction
 */
void FFDLattice::setMesh(darray3E &origin,darray3E & span, BasicShape::ShapeType type, ivector1D & dimensions){
	
	clearMesh();
	UStructMesh::setMesh(origin,span,type,dimensions);
	
	m_deg[0] = m_nx;
	m_deg[1] = m_ny;
	m_deg[2] = m_nz;
	
	//setting knots and eventually weights to non-rational B-Spline
	setKnotsStructure();
	
	//reallocate your displacement node
	ivector1D dd = getDimension();
	resizeDisplacements(dd[0],dd[1],dd[2]);
	//reset your weights
	m_weights.resize(m_ndeg, 1.0);

	//reorder dimensions
	orderDimension();

};

/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *   
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
void FFDLattice::setMesh(BasicShape * shape, ivector1D & dimensions, ivector1D & degrees){
	
	clearMesh();
	UStructMesh::setMesh(shape,dimensions);
	
	m_deg[0] = std::min(m_nx,std::max(1,degrees[0]));
	m_deg[1] = std::min(m_ny,std::max(1,degrees[1]));
	m_deg[2] = std::min(m_nz,std::max(1,degrees[2]));
	
	//setting knots and eventually weights to non-rational B-Spline
	setKnotsStructure();
	
	//reallocate your displacement node
	ivector1D dd = getDimension();
	resizeDisplacements(dd[0],dd[1],dd[2]);
	//reset your weights
	m_weights.resize(m_ndeg, 1.0);
	
	//reorder dimensions
	orderDimension();

};

/*! Set lattice mesh, dimensions and curve degree for Rational Bezier trivariate parameterization.
 *  Knots structure is built with curve degrees as in case of a Pure Bezier Volumetric 
 *  Parameterization, that is degX = nx-1, degY = ny-1, degZ=nz-1.
 *   
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] dimensions number of control nodes for each direction
 *
 */
void FFDLattice::setMesh(BasicShape * shape, ivector1D & dimensions){
	
	clearMesh();
	UStructMesh::setMesh(shape,dimensions);
	
	m_deg[0] = m_nx;
	m_deg[1] = m_ny;
	m_deg[2] = m_nz;
	
	//setting knots and eventually weights to non-rational B-Spline
	setKnotsStructure();
	
	//reallocate your displacement node
	ivector1D dd = getDimension();
	resizeDisplacements(dd[0],dd[1],dd[2]);
	//reset your weights
	m_weights.resize(m_ndeg, 1.0);
	
	//reorder dimensions
	orderDimension();

};

/*! Modify a weight of a control node. Access to a node in global GRID indexing
 * \param[in] val weight value
 * \param[in] index index of the control node -> gloab indexing
 */
void 		FFDLattice::setNodalWeight(double val, int index){
			int ind = accessDOFFromGrid(index);
			m_weights[ind] =  val;
};

/*! Modify a weight of a control node. Access to a node in GRID cartesian indexing
 * \param[in] val weight value
 * \param[in] i index of x coordinate
 * \param[in] j index of y coordinates 
 * \param[in] k index of z coordinate
 */
void 		FFDLattice::setNodalWeight(double val, int i, int j, int k){
		int index = accessPointIndex(i,j,k);
		setNodalWeight(val, index);
};

/*! set current DOF displacements of your lattice. If Input list does not fit current DOF size
 * of lattice, return doing nothing */
void
FFDLattice::setDisplacements(dvecarr3E & displacements){
	if (m_ndeg != displacements.size() || getShape() == NULL)	return;
	
	BaseManipulation::setDisplacements(displacements);
};

/*! Set if displacements are meant as global-true or local-false*/
void
FFDLattice::setDisplGlobal(bool flag){m_globalDispl = flag;}

/*! Check if displacements are meant as global-true or local-false*/
bool
FFDLattice::isDisplGlobal(){return(m_globalDispl);}

/*! Set span of your shape, according to its local reference system 
 * \param[in] s0 first coordinate span
 * \param[in] s1 second coordinate span
 * \param[in] s2 third coordinate span
 * \param[in] flag if true, lattice is rebuilt according to the new input.TRUE is default 
 */
void FFDLattice::changeSpan(double s0, double s1, double s2, bool flag){
	getShape()->setSpan( s0,  s1,s2);
	
	if(flag){
		rebaseMesh();
		setKnotsStructure();
	}
}

/*! Set coordinate's origin of your shape, according to its local reference system  
 * \param[in] orig first coordinate origin
 * \param[in] dir 0,1,2 int flag identifying coordinate
 * \param[in] flag if true, lattice is rebuilt according to the new input.TRUE is default
 */
void FFDLattice::setInfLimits(double orig, int dir, bool flag){
	getShape()->setInfLimits( orig, dir);
	
	if(flag){
		rebaseMesh();
		setKnotsStructure();
	}
}

/*! Set coordinate type of Lattice core shape. See BasicShape::CoordType enum
 * \param[in] type coordinate type
 * \param[in] dir  0,1,2 flag for coordinate
 * \param[in] flag if true, force lattice nodal structure to be updated.TRUE is default
 */
void FFDLattice::setCoordType(BasicShape::CoordType type, int dir, bool flag){
	getShape()->setCoordinateType(type,dir);
	if(flag){
		setKnotsStructure(dir, type);
		ivector1D dim = getDimension();
		resizeDisplacements(dim[0],dim[1],dim[2]);
	}
}


/*! Find a corrispondent degree of freedom index of a lattice grid node
 * \param[in] index lattice grid global index
 * \param[out] result corrispondent DOF global index
 */ 
int FFDLattice::accessDOFFromGrid(int index){
	return(m_intMapDOF[index]);
}

/*! Find a corrispondent lattice grid index of a degree of freedom node
 * \param[in] index DOF global index
 * \param[out] result corrispondent lattice grid global index
 */  
int FFDLattice::accessGridFromDOF(int index){
	return(posVectorFind(m_intMapDOF, index));
}

/*! Set if displacements are meant as global-true or local-false*/
void
FFDLattice::setDisplGlobal(bool flag){m_globalDispl = flag;}

dvecarr3E*
FFDLattice::releaseResult(){
	return(&m_result);
};


/*! Plot your current lattice as a structured grid to *vtu file. Wrapped method of plotGrid of father class UCubicMesh.
 * \param[in] directory output directory
 * \param[in] filename  output filename w/out tag
 * \param[in] counter   integer identifier of the file
 * \param[in] binary     boolean flag for 0-"ascii" or 1-"appended" writing 
 * \param[in] deformed  boolean flag for plotting 0-"original lattice", 1-"deformed lattice"
 */
void		FFDLattice::plotGrid(std::string directory, std::string filename,int counter, bool binary, bool deformed){
		
		if(deformed){
				ivector1D n =getDimension();
				dvecarr3E dispXYZ;
				if(isDisplGlobal()){
					dispXYZ = recoverFullGridDispl();
				}else{
					dispXYZ = convertDisplToXYZ(); 
				}
				int size = n[0]*n[1]*n[2];
				dvecarr3E data(size);
				for(int i=0; i<size; ++i){
					data[i] = getGlobalPoint(i) + dispXYZ[i];
				}
			UStructMesh::plotGrid(directory, filename, counter, binary, &data);
		}else{
			dvecarr3E* pnull = NULL;
			UStructMesh::plotGrid(directory, filename, counter, binary,  pnull);
			
		}
			
	
};

/*! Plot your current lattice as a point cloud to *vtu file.Wrapped method of plotCloud of father class UCubicMesh.
 * \param[in] directory output directory
 * \param[in] filename  output filename w/out tag
 * \param[in] counter   integer identifier of the file
 * \param[in] binary     boolean flag for 0-"ascii" or 1-"appended" writing 
 * \param[in] deformed  boolean flag for plotting 0-"original lattice", 1-"deformed lattice"
 */
void		FFDLattice::plotCloud(std::string directory, std::string filename, int counter, bool binary, bool deformed){
	
	if(deformed){
		ivector1D n = getDimension();
		
		dvecarr3E dispXYZ;
		if(isDisplGlobal()){
			dispXYZ = recoverFullGridDispl();
		}else{
			dispXYZ = convertDisplToXYZ(); 
		}
		
		int size = n[0]*n[1]*n[2];
		dvecarr3E data(size);
		for(int i=0; i<size; ++i){
			data[i] = getGlobalPoint(i) + dispXYZ[i];
		}
		UStructMesh::plotCloud(directory, filename, counter, binary, &data);
	}else{
		dvecarr3E* pnull = NULL;
		UStructMesh::plotCloud(directory, filename, counter, binary, pnull);
		
	}
	
};

/*! TODO per Edo: ma che fa sto metodo me lo potevi pure lasciar detto no? :-D */
void
FFDLattice::setInfo(){
	m_info->m_naxes = 3;

	darray3E origin = getShape()->getOrigin();
	m_info->m_origin.resize(3);
	for (int i=0; i<3; i++){
		m_info->m_origin[i] = origin[i];
	}

	dmatrix33E sdr = getShape()->getRefSystem();
	m_info->m_axes.resize(3, dvector1D(3));
	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++){
		m_info->m_axes[i][j] = sdr[i][j];
		}
	}

	ivector1D n = getDimension();
	int size = n[0]*n[1]*n[2];
	m_info->m_coords.resize(size, dvector1D(3));
	for(int i=0; i<size; ++i){
		darray3E data = getGlobalPoint(i);
		for (int j=0; j<3; j++){
			m_info->m_coords[i][j] = data[j];
		}
	}

}

/*! Given pointer to a reference geometry and, execute deformation w/ the current setup */
void 		FFDLattice::execute(){
			
			MimmoObject * container = getGeometry();
			if(container == NULL ) return;
			
			livector1D map;
			dvecarr3E localdef = apply(map);
			
			//reset displacement in a unique vector
			int size = container->getNVertex();
			
			dvecarr3E result(size, darray3E{0,0,0});
			
			for(int i=0; i<map.size(); ++i){
				result[container->getMapDataInv(map[i])] = localdef[i];
			}
	//debug
// 			for(int i=0; i<size; ++i){
// 				darray3E pV = container->getVertex(i);
// 				pV = pV + result[i];
// 				container->modifyVertex(pV, i);
// 			}	
			
			
			for (int i=0; i<getNChild(); i++){
//				setDisplacementsOut(i, result);
			}


			//temporary result in a member m_result
			//TODO FIND ALTERNATIVE TO MEMORY SAVE (??)
			m_result = result;

};

/*! Apply current deformation setup to a single 3D point. If point is not included in lattice return zero
 * \param[in] point coordinate of the points 
 * \param[out] result point displacement 
 */
darray3E 	FFDLattice::apply(darray3E & point){
	darray3E result;
	result.fill(0.0);
	if(!getShape()->isPointIncluded(point)) return result;
	return(nurbsEvaluator(point));
};

/*! Apply current deformation setup to geometry linked as a MimmoObject container, member of the class 
 * (see method getGeometry).If MimmoObject member m_geometry is NULL,return void results. 
 * \param[out] result list of non-zero displacement of m_geometry vertices 
 * \param[out] map list of ids of non-zero displaced vertex belonging to geometry
 */
dvecarr3E 	FFDLattice::apply(livector1D & list){
	
	MimmoObject * container = getGeometry();
	if(container == NULL ) return dvecarr3E(0);
	
	bitpit::PatchKernel * tri = container->getGeometry();
	list.clear();

	//check simplex included and extract their vertex in global IDs;
	list= container->getVertexFromCellList(getShape()->includeGeometry(tri));
	//return deformation
	return(nurbsEvaluator(list));

};

/*! Apply current deformation setup to a custom list of 3D points. Only points contained into the lattice will be deformed, 
 *  diplacement of the others will be set to zero. If point list is NULL,return void results. 
 * \param[in] point pointer to a list of 3D points. 
 * \param[out] result displacements of points 
 */
dvecarr3E 	FFDLattice::apply(dvecarr3E * point){
	
	dvecarr3E result;
	if(point ==NULL ) return result;
	
	result.resize(point->size(), darray3E{0,0,0});
	livector1D list = getShape()->includeCloudPoints(*point);
	
	for(int i=0; i<list.size(); ++i){
		darray3E target = (*point)[list[i]];
		result[list[i]] = nurbsEvaluator(target);
	}
	
	return(result);
};

/*! Convert a target displacement (expressed in local shape ref frame) in XYZ frame
 *	\param[in] target  target displacement
 * 	\param[out] result displacement in xyz ref frame
 */
darray3E FFDLattice::convertDisplToXYZ(darray3E & target, int i){
	
	darray3E scaling = getShape()->getScaling();
	darray3E work;
	for(int i=0; i<3; ++i){
		work[i] = target[i]/scaling[i];
	}
	work += getLocalPoint(i);
	darray3E result = transfToGlobal(work) -  getGlobalPoint(i);
	return(result);
};

/*! Convert and return all target displacements (expressed in local shape ref frame) in XYZ frame
 *	\param[in] target  target displacement
 * 	\param[out] result displacement in xyz ref frame
 */
dvecarr3E FFDLattice::convertDisplToXYZ(){
	
	dvecarr3E displ = recoverFullGridDispl();
	int sizeD = displ.size();
	
	dvecarr3E result(sizeD);
	for(int i=0; i<sizeD; ++i){
		result[i] = convertDisplToXYZ(displ[i],i);
	}
	return(result);
};

/*! Return displacement of a given point, under the deformation effect of the whole Lattice. 
 * \param[in] coord 3D point
 * \param[out] result displacement  
 */
darray3E 	FFDLattice::nurbsEvaluator(darray3E & pointOr){
	
	darray3E point = transfToLocal(pointOr);
	darray3E scaling = getShape()->getScaling();
	
	ivector1D knotInterval(3,0);
	dvector2D BSbasis(3);
	dvector1D valH(4,0), temp1(4,0),temp2(4,0), zeros(4,0);
	int uind, vind, wind, index;
	
	dvecarr3E *displ = getDisplacements();

	int i0 = m_mapdeg[0];
	int i1 = m_mapdeg[1];
	int i2 = m_mapdeg[2];

	iarray3E mappedIndex;
	
	for(int i=0; i<3; i++){
		knotInterval[i] = getKnotInterval(point[i],i);
		BSbasis[i] = basisITS0(knotInterval[i], i, point[i]);
	}
	
	uind = knotInterval[i0] - m_deg[i0];
	vind = knotInterval[i1] - m_deg[i1];
	wind = knotInterval[i2] - m_deg[i2];

	for(int i=0; i<=m_deg[i0]; ++i){
		
		mappedIndex[0] = uind+i;
		temp1 = zeros;
		
		for(int j=0; j<=m_deg[i1]; ++j){
			
			mappedIndex[1] = vind+j;
			temp2 = zeros;
			
			for(int k=0; k<=m_deg[i2]; ++k){
				
				mappedIndex[2] = wind+k;
				index = accessMapNodes(mappedIndex[0],mappedIndex[1],mappedIndex[2]);

				for(int intv=0; intv<3; ++intv){
					temp2[intv] += BSbasis[i2][k]*m_weights[m_intMapDOF[index]]*(*displ)[m_intMapDOF[index]][intv];
				}	
				temp2[3] += BSbasis[i2][k]*m_weights[index];
			}
			for(int intv=0; intv<4; ++intv){
				temp1[intv] += BSbasis[i1][j]*temp2[intv];
			}	
			
		}
		for(int intv=0; intv<4; ++intv){
			valH[intv] += BSbasis[i0][i]*temp1[intv];
		}	
	}

	darray3E outres;
	if(isDisplGlobal()){
		for(int i=0; i<3; ++i){
			outres[i] =  valH[i]/valH[3];
		}
		
	}else{
		//summing scaled displ in local ref frame; 
		for(int i=0; i<3; ++i){
			point[i] +=  valH[i]/(valH[3]*scaling[i]);
		}
	
		//get final displ in global ref frame:
		outres = transfToGlobal(point) - pointOr;
	}
	return(outres);
	
}; 

/*! Return displacement of a given point, under the deformation effect of the whole Lattice.
 * \param[in] coord 3D point
 * \param[out] result displacement
 */
dvecarr3E 	FFDLattice::nurbsEvaluator(livector1D & list){

	bitpit::PatchKernel * tri = getGeometry()->getGeometry();
	long int id;
	int lsize = list.size();
	livector1D::iterator it, itend = list.end();
	darray3E target, point;
	ivector1D knotInterval(3,0);
	dvector1D BSbasisi0, BSbasisi1, BSbasisi2;
	dvector1D valH(4,0), temp1(4,0),temp2(4,0);

	dvecarr3E displ = recoverFullGridDispl();
	dvector1D weig = recoverFullNodeWeights();
	
	int uind, vind, wind, index;

	int intv, i, j, k;

	int i0 = m_mapdeg[0];
	int i1 = m_mapdeg[1];
	int i2 = m_mapdeg[2];
	iarray3E mappedIndex;

	int md0 = m_deg[i0];
	int md1 = m_deg[i1];
	int md2 = m_deg[i2];

	dvecarr3E outres(lsize);
	dvecarr3E::iterator itout = outres.begin();
	darray3E scaling = getShape()->getScaling();
	
	double bbasisw2;
	double bbasis1, bbasis0;

	for(it = list.begin(); it != itend; ++it){

		for(intv=0; intv<4; ++intv){
			valH[intv] = 0.0;
		}
		id  = *it;
		target = tri->getVertex(id).getCoords();
		point = transfToLocal(target);

		// get reference Interval int the knot matrix
		for(i=0; i<3; i++){
			knotInterval[i] = getKnotInterval(point[i],i);
		}
		BSbasisi0 = basisITS0(knotInterval[i0], i0, point[i0]);
		BSbasisi1 = basisITS0(knotInterval[i1], i1, point[i1]);
		BSbasisi2 = basisITS0(knotInterval[i2], i2, point[i2]);

		uind = knotInterval[i0] - md0;
		vind = knotInterval[i1] - md1;
		wind = knotInterval[i2] - md2;


		for(i=0; i<=md0; ++i){

			mappedIndex[i0] = uind + i;

			for(int intv=0; intv<4; ++intv){
				temp1[intv] = 0.0;
			}

			for(j=0; j<=md1; ++j){

				mappedIndex[i1] = vind + j;

				for(intv=0; intv<4; ++intv){
					temp2[intv] = 0.0;
				}

				for(k=0; k<=md2; ++k){

					mappedIndex[i2] = wind + k;

					index = accessMapNodes(mappedIndex[0], mappedIndex[1], mappedIndex[2]);

					bbasisw2 = BSbasisi2[k]* weig[index];

					for(intv=0; intv<3; ++intv){
						temp2[intv] += bbasisw2 * displ[index][intv];
					}
					temp2[3] += bbasisw2;

				}
				bbasis1 = BSbasisi1[j];
				for(intv=0; intv<4; ++intv){
					temp1[intv] += bbasis1*temp2[intv];
				}

			}
			bbasis0 = BSbasisi0[i];
			for(intv=0; intv<4; ++intv){
				valH[intv] += bbasis0*temp1[intv];
			}
		}


		if(isDisplGlobal()){

			//adding to local point displ rescaled
			for(i=0; i<3; ++i){
				(*itout)[i] = valH[i]/valH[3];
			}

		}else{

			//adding to local point displ rescaled
			for(i=0; i<3; ++i){
				 point[i]+= valH[i]/(valH[3]*scaling[i]);
			}

			//get absolute displ as difference of
			for(i=0; i<3; ++i){
				(*itout)[i] = transfToGlobal(point)[i] - target[i];
			}

		}

		itout++;

	}//next list id

	itout = outres.end();


	return(outres);

};

/*! Return a specified component of a displacement of a given point, under the deformation effect of the whole Lattice. 
 * \param[in] coord 3D point
 * \param[in] intV component of displacement vector (0,1,2)
 * \param[out] result return displacement disp[intV]
 */
double 		FFDLattice::nurbsEvaluatorScalar(darray3E & coordOr, int targ){
	
	darray3E point = transfToLocal(coordOr);
	double scaling = getScaling()[targ];
	
	ivector1D knotInterval(3,0);
	dvector2D BSbasis(3);
	dvector1D valH(2,0), temp1(2,0),temp2(2,0), zeros(2,0);
	int uind, vind, wind, index;
	
	dvecarr3E *displ = getDisplacements();
	
	int i0 = m_mapdeg[0];
	int i1 = m_mapdeg[1];
	int i2 = m_mapdeg[2];
	
	int md0 = m_deg[i0];
	int md1 = m_deg[i1];
	int md2 = m_deg[i2];

	iarray3E mappedIndex;
	
	for(int i=0; i<3; i++){
		knotInterval[i] = getKnotInterval(point[i],i);
		BSbasis[i] = basisITS0(knotInterval[i], i, point[i]);
	}
	
	uind = knotInterval[i0] - md0;
	vind = knotInterval[i1] - md1;
	wind = knotInterval[i2] - md2;
	
	for(int i=0; i<=md0; ++i){
		
		mappedIndex[0] = uind+i;
		temp1 = zeros;
		
		for(int j=0; j<=md1; ++j){
			
			mappedIndex[1] = vind+j;
			temp2 = zeros;
			
			for(int k=0; k<=md2; ++k){
				
				mappedIndex[2] = wind+k;
				index = accessMapNodes(mappedIndex[0],mappedIndex[1],mappedIndex[2]);
				
				temp2[0] += BSbasis[i2][k]*m_weights[m_intMapDOF[index]]*(*displ)[m_intMapDOF[index]][targ];
				temp2[1] += BSbasis[i2][k]*m_weights[index];
			}
			
			for(int intv=0; intv<2; ++intv){
				temp1[intv] += BSbasis[i1][j]*temp2[intv];
			}	
			
		}
		
		for(int intv=0; intv<2; ++intv){
			valH[intv] += BSbasis[i0][i]*temp1[intv];
		}	
	}

	darray3E res;
	if(isDisplGlobal()){
		res[targ] = valH[0]/valH[1];
	}else{
		point[targ] += valH[0]/(valH[1]*scaling);
		res = transfToGlobal(point)- coordOr;
	}
	return(res[targ]);
};

/*!Return the local basis function of a Nurbs Curve.Please refer to NURBS book of PEIGL 
 * for this Inverted Triangular Scheme Algorithm (pag 74);* 
 *\param[in] k  local knot interval in which coord resides -> theoretical knot indexing, 
 *\param[in] pos identifies which nurbs curve of lattice (3 curve for 3 box direction) you are pointing
 *\param[in] coord the evaluation point on the curve
 *\param[out] result local basis of ITS algorithm-> coefficient of interpolation of control nodes for position u of curve pos
 */
dvector1D 	FFDLattice::basisITS0(int k, int pos, double coord){
	
	//return local basis function given the local interval in theoretical knot index,
	//local degree of the curve -> Please refer to NURBS book of PEIGL for this Inverted Triangular Scheme Algorithm (pag 74);
	int dd1 = m_deg[pos]+1;
	dvector1D basis(dd1,1);
	dvector1D left(dd1,0), right(dd1,0);
	double saved, tmp;
	
	for(int j = 1; j < dd1; ++j){
		saved = 0.0;
		left[j] = coord - getKnotValue(k+1-j, pos);
		right[j]= getKnotValue(k+j, pos) - coord;
		
		for(int r = 0; r < j; ++r){
			tmp = basis[r]/(right[r+1] + left[j-r]);
			basis[r] = saved + right[r+1] * tmp;
			saved = left[j-r] * tmp;
		}//next r	
		
		basis[j] = saved;  
	}//next j
	
	return(basis);
};	

/*!Return list of equispaced knots for the Nurbs curve in a specific lattice direction
 * \param[in] dir 0,1,2 int identifier of Lattice's Nurbs Curve.
 */
dvector1D	FFDLattice::getNodeSpacing(int dir){
	
	dvector1D result;
	int dim = getDimension()[dir];
	double span = getLocalSpan()[dir];
	double locOr= getShape()->getLocalOrigin()[dir];
	
	int nn, retroOrigin;
	double dKn, origin;
	
	switch(getCoordType(dir))
	{
		case BasicShape::CoordType::PERIODIC :
			nn = dim+m_deg[dir]-1;
			result.resize(nn);
			dKn = span/(dim-1);
		
			retroOrigin = (m_deg[dir]-1)/2 + (m_deg[dir]-1)%2;
			origin = locOr-1.0 * retroOrigin * dKn;
		
			for(int i=0; i<nn; ++i){
				result[i] = origin + i*dKn;
			}
			break;

		case BasicShape::CoordType::SYMMETRIC :
			nn = dim+m_deg[dir]-1;
			result.resize(nn);
			dKn = span/(dim-1);
			
			retroOrigin = (m_deg[dir]-1)/2 + (m_deg[dir]-1)%2;
			origin = locOr-1.0 * retroOrigin * dKn;
			
			for(int i=0; i<nn; ++i){
				result[i] = origin + i*dKn;
			}
			break;
			
		case BasicShape::CoordType::CLAMPED :
			nn = dim;
			result.resize(nn);
			dKn = span/(dim-1);
			for(int i=0; i<nn; ++i){
				result[i] =locOr+ i*dKn;
			}
			break;

		case BasicShape::CoordType::UNCLAMPED :
			nn = dim;
			result.resize(nn);
			dKn = span/(dim-1);
			for(int i=0; i<nn; ++i){
				result[i] =locOr+ i*dKn;
			}
			break;
			
		default: //never been reached
			break;
	}
	
	return(result);
};

/*!Clean all knots stuff in your lattice */
void FFDLattice::clearKnots(){
	
	m_knots.clear();
	m_mapEff.clear();
	m_deg.clear();
	m_weights.clear();
	m_mapNodes.clear();
	m_intMapDOF.clear();
	
	m_knots.resize(3);
	m_mapEff.resize(3);
	m_deg.resize(3,1);
	m_mapNodes.resize(3);
	m_mapdeg[0]=0; m_mapdeg[1]=1; m_mapdeg[2]=2;
	
	
};

/*! Apply knots structure after modifications to Nurbs curve degrees member */
void 		FFDLattice::setKnotsStructure(){
		for(int i=0; i<3; i++){
			setKnotsStructure(i,getCoordType(i));
		}
}

/*! Apply knots structure after modifications to Nurbs curve degrees member, to a specific curve knots structure.
 * Curve is identified by a int 0,1,2 for Nurbs curves in x,y,z, directions respectively.
 * \param[in] dir int identifier
 * \param[in] flag identifies a closed periodic curve (true) or a clamped one (false)
 */
void 		FFDLattice::setKnotsStructure(int dir,BasicShape::CoordType type){
	
	//recover number of node for direction dir;
	ivector1D dim = getDimension();
	int nn = dim[dir];
	
	// free necessary knot structures
	m_knots[dir].clear();
	m_mapEff[dir].clear();
	
	dvector1D equinode = getNodeSpacing(dir);
	int nEff, kEff,kTheo, kend;
	double tol = 1.0E-12;
	
	switch(type){
		//clamped curve structure 
		case BasicShape::CoordType::CLAMPED :
			
			m_deg[dir] = min(m_deg[dir], nn-1);
			kEff = nn - m_deg[dir] + 1;
			kTheo = nn + m_deg[dir] + 1;
			m_knots[dir].resize(kEff);
			m_mapEff[dir].resize(kTheo,0);
		
			//set knots grid in the dir space considered 1 for this example
			m_knots[dir][0] = equinode[0];
			m_knots[dir][kEff-1] = equinode[equinode.size()-1];
		
			for(int i=1; i<kEff-1; ++i){
				m_knots[dir][i] = 0.0;
				for(int j=i; j<=i+m_deg[dir]-1; ++j){
					m_knots[dir][i] += equinode[j]/((double) m_deg[dir]);
				}
			}
		
			for(int i=m_deg[dir]; i<(m_deg[dir]+kEff); i++){
				m_mapEff[dir][i]=i-m_deg[dir];
			}
		
			for(int i=(m_deg[dir]+kEff); i<kTheo; i++){
				m_mapEff[dir][i]=kEff-1;
			}
			break;
			
		case BasicShape::CoordType::PERIODIC :
			
			m_deg[dir] = min(m_deg[dir], nn-1);
			nEff = nn + m_deg[dir] - 1;
			kEff = nEff -m_deg[dir] + 1;
			kTheo = nEff +m_deg[dir] + 1;
			m_knots[dir].resize(kTheo);
			m_mapEff[dir].resize(kTheo,0);
		
			//set knots grid in the dir space considered 1 for this example
		
			m_knots[dir][m_deg[dir]] = 0.0;
			for(int j=0; j<=m_deg[dir]-1; ++j){
				m_knots[dir][m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
			}
			if(abs(m_knots[dir][m_deg[dir]])<1.e-12) m_knots[dir][m_deg[dir]] = 0.0;
		
			for(int i=1; i<kEff; ++i){
				m_knots[dir][i+m_deg[dir]] = 0.0;
				for(int j=i; j<=i+m_deg[dir]-1; ++j){
					m_knots[dir][i+m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
				}
			}
		
			kend = m_deg[dir] + kEff-1;
			//unclamp the other nodes
			for(int i=0; i<m_deg[dir]; ++i){
				m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
				m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
			}
	
			for(int i=0; i<kTheo; i++){
				m_mapEff[dir][i]=i;
			}
			break;

		case BasicShape::CoordType::SYMMETRIC :
			
			m_deg[dir] = min(m_deg[dir], nn-1);
			nEff = nn + m_deg[dir]-1;
			kEff = nEff -m_deg[dir] + 1;
			kTheo = nEff +m_deg[dir] + 1;
			m_knots[dir].resize(kTheo);
			m_mapEff[dir].resize(kTheo,0);
			
			//set knots grid in the dir space considered 1 for this example
			
			m_knots[dir][m_deg[dir]] = 0.0;
			for(int j=0; j<=m_deg[dir]-1; ++j){
				m_knots[dir][m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
			}
			if(abs(m_knots[dir][m_deg[dir]])<1.e-12) m_knots[dir][m_deg[dir]] = 0.0;
			
			for(int i=1; i<kEff; ++i){
				m_knots[dir][i+m_deg[dir]] = 0.0;
				for(int j=i; j<=i+m_deg[dir]-1; ++j){
					m_knots[dir][i+m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
				}
			}
			
			kend = m_deg[dir] + kEff-1;
			//unclamp the other nodes
			for(int i=0; i<m_deg[dir]; ++i){
				m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
				m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
			}
			
			for(int i=0; i<kTheo; i++){
				m_mapEff[dir][i]=i;
			}
			break;			
			
			
		case BasicShape::CoordType::UNCLAMPED :
			
			m_deg[dir] = min(m_deg[dir], nn-1);
			kEff = nn - m_deg[dir] + 1;
			kTheo = nn + m_deg[dir] + 1;
			m_knots[dir].resize(kTheo);
			m_mapEff[dir].resize(kTheo,0);
			
			kend = m_deg[dir] + kEff-1;
			//set knots grid in the dir space considered 1 for this example
			m_knots[dir][m_deg[dir]] = equinode[0];
			m_knots[dir][kend] = equinode[equinode.size()-1] + tol;
			
			for(int i=1; i<kEff-1; ++i){
				m_knots[dir][m_deg[dir]+i] = 0.0;
				for(int j=i; j<=i+m_deg[dir]-1; ++j){
					m_knots[dir][m_deg[dir]+i] += equinode[j]/((double) m_deg[dir]);
				}
			}
			
			//unclamp the other nodes
			for(int i=0; i<m_deg[dir]; ++i){
				m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
				m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
			}
			
			for(int i=0; i<kTheo; i++){
				m_mapEff[dir][i]=i;
			}
			break;
		
		default: //never been reached
				break;
	}
	
	setMapNodes(dir);
	
};

/*! Given a knots distribution for one curve in direction "dir", return the index of
 * interval which a coord belongs to
 * \param[in] coord target position
 * \param[in] dir   0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 * \param[out] result return the interval index.
 */
int  		FFDLattice::getKnotInterval(double coord, int dir){

	int size = m_knots[dir].size();
	if(coord< m_knots[dir][0] ){ return(getTheoreticalKnotIndex(0, dir));}
	if(coord >= m_knots[dir][size-1]){ return(getTheoreticalKnotIndex(size-2, dir));}

	int low = 0;
	int high = size-1;
	int mid = (low + high)/2;
	while( coord < m_knots[dir][mid] || coord>= m_knots[dir][mid+1]){
		if(coord < m_knots[dir][mid])	{high=mid;}
		else				{low=mid;}
		mid = (low+high)/2;
	}
	return(getTheoreticalKnotIndex(mid, dir));
};

/*! Return value of a knot for a given its theoretical index and a direction in space
 * \param[in] index theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 */
double 		FFDLattice::getKnotValue(int index, int dir){
	int target = getKnotIndex(index, dir);
	if(target ==-1){return(-1.0);}
	return(m_knots[dir][target]);
};

/*! Return a knot real index given its theoretical index and a direction in space
 * \param[in] index theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 */
int 		FFDLattice::getKnotIndex(int index ,int dir){
	if(index < 0 || index>=m_mapEff[dir].size()) return -1;
	return(m_mapEff[dir][index]);
};

/*! Return a knot theoretical index given its real index and a direction in space
 * \param[in] index theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 */
int 		FFDLattice::getTheoreticalKnotIndex(int locIndex,int dir){
	if(locIndex <0 || locIndex >= m_knots[dir].size()){return(-1);}

	// search from the end your m_mapEff vector
	ivector1D::reverse_iterator it = find(m_mapEff[dir].rbegin(), m_mapEff[dir].rend(), locIndex);
	int result = std::distance(m_mapEff[dir].begin(), (it.base()-1));
	return(result);
};

/*! Resize BaseManipulation class member m_displ to fit a total number od degree of freedom nx*ny*nz.
 * Old structure is deleted and reset to zero.
 *  \param[in] nx number of control nodes in x direction
 *  \param[in] ny number of control nodes in y direction 
 *  \param[in] nz number of control nodes in z direction
 */
void 		FFDLattice::resizeDisplacements(int nx, int ny,int nz){
	//reallocate your displacement node
	m_displ.clear();
	m_intMapDOF.clear();
	m_intMapDOF.resize(nx*ny*nz, -1);
	ivector1D::iterator itMapBegin = m_intMapDOF.begin();
	ivector1D::iterator itMap = itMapBegin;
	ivector1D::iterator itMapEnd = m_intMapDOF.end();
	bvector1D info;
	m_ndeg = reduceDimToDOF(nx,ny,nz, info);
	m_displ.resize(m_ndeg, darray3E{0,0,0});
	
	//set m_intMapDOF
	
	int target;
	int index;
	ivector1D dummy;
	
	int i0,i1,i2;
	switch(getShapeType()){
		
		case BasicShape::ShapeType::CYLINDER :
			target=0;
			while(itMap != itMapEnd){
				
				*itMap = target;
				index = std::distance(itMapBegin, itMap);
				accessPointIndex(index,i0,i1,i2);
				
				if(info[0] && i0 == 0){
					for(int k=0; k<ny;++k){
						m_intMapDOF[accessPointIndex(i0,k,i2)] = target;
					}
				}
				if(info[1] && i1 == 0){
					m_intMapDOF[accessPointIndex(i0,ny-1,i2)] = target;
				}
			
				itMap = find(m_intMapDOF.begin(), itMapEnd,-1);
				target++;
			}
			break;
			
		case BasicShape::ShapeType::SPHERE :
			
			target = 0;
			while(itMap != itMapEnd){
				
				*itMap = target;
				index = std::distance(itMapBegin, itMap);
				accessPointIndex(index,i0,i1,i2);
				
				if(info[0] && i0 == 0){
					for(int k1=0; k1<ny;++k1){
						for(int k2=0; k2<nz; ++k2){
							m_intMapDOF[accessPointIndex(i0,k1,k2)] = target;
						}
					}
				}
				
				if(info[1] && i1 == 0){
					m_intMapDOF[accessPointIndex(i0,ny-1,i2)] = target;
				}
				
				if(info[2] && i2 == 0){
					for(int k1=0; k1<ny; ++k1){
							m_intMapDOF[accessPointIndex(i0,k1,i2)] = target;
						}
				}
				
				if(info[3] && i2 == (nz-1)){
					for(int k1=0; k1<ny;++k1){
						m_intMapDOF[accessPointIndex(i0,k1,i2)] = target;
					}
				}
				
				itMap = find(m_intMapDOF.begin(), itMapEnd,-1);
				target++;
			}
			break;
			
			
		case BasicShape::ShapeType::CUBE :
			target = 0;
			while(itMap != itMapEnd){
				
				*itMap = target;
				itMap = find(m_intMapDOF.begin(), itMapEnd,-1);
				target++;
			}
			break;
		
		default: //doing nothing
			break;
	}//end switch
	
}

/*! Recover full displacements vector from DOF */
dvecarr3E FFDLattice::recoverFullGridDispl(){
	
	ivector1D dim = getDimension();
	int size = dim[0]*dim[1]*dim[2];
	dvecarr3E result(size);
	for(int i=0; i<size; ++i){
		result[i] = m_displ[m_intMapDOF[i]];
	}
	return(result);
};

/*! Recover full displacements vector from DOF */
dvector1D FFDLattice::recoverFullNodeWeights(){
	
	ivector1D dim = getDimension();
	int size = dim[0]*dim[1]*dim[2];
	dvector1D result(size);
	for(int i=0; i<size; ++i){
		result[i] = m_weights[m_intMapDOF[i]];
	}
	return(result);
};


/*! Fill m_mapnodes, to access correct displacement w knots structure 
 * theoretical knot indexing*/
void FFDLattice::setMapNodes( int ind){

		int dimdir = getDimension()[ind];
		int nn,preNNumb,postNNumb, pInd;
		m_mapNodes[ind].clear();

		switch(getCoordType(ind)){
			case BasicShape::CoordType::PERIODIC :
				
				nn = dimdir+m_deg[ind]-1;
				m_mapNodes[ind].resize(nn+1);
				
				preNNumb = (m_deg[ind]-1)/2 + (m_deg[ind]-1)%2;
				postNNumb = (m_deg[ind]-1) - preNNumb;
				
				// set the other internal loads
				for(int i=0; i<dimdir; ++i){
					m_mapNodes[ind][i+preNNumb] = i;
				}
				
				//postpend the first preNNumb loads
				pInd = dimdir - preNNumb -1;
				for(int i=0; i<preNNumb; ++i){
					m_mapNodes[ind][i] = pInd + i;
				}
				//prepend the last postNNumb loads.
				pInd = 1;
				for(int i=0; i<=postNNumb; ++i){
					m_mapNodes[ind][i+preNNumb+dimdir] = pInd+i;
				}
			break;
				
			case BasicShape::CoordType::SYMMETRIC :
				
				nn = dimdir+m_deg[ind]-1;
				m_mapNodes[ind].resize(nn+1);
				
				preNNumb = (m_deg[ind]-1)/2 + (m_deg[ind]-1)%2;
				postNNumb = (m_deg[ind]-1) - preNNumb;
				
				// set the other internal loads
				for(int i=0; i<dimdir; ++i){
					m_mapNodes[ind][i+preNNumb] = i;
				}
				
				//prepend symmetrically loads
				for(int i=0; i<preNNumb; ++i){
					m_mapNodes[ind][preNNumb -1 - i] = (i+1)%(dimdir-1);
				}
				//postpend symmetrically loads.
				for(int i=0; i<=postNNumb; ++i){
					m_mapNodes[ind][i+preNNumb+dimdir] = (dimdir-2-i)%(dimdir-1);
				}
				break;
				
			default:
				m_mapNodes[ind].resize(dimdir);
				for (int i=0; i<dimdir; ++i){
					m_mapNodes[ind][i] = i;
				}
			break;	
		}
};

/*!Get the effective dof size of the lattice according to its shape. Return info
 * to build successfully m_intMapDOF
 */ 
int 
FFDLattice::reduceDimToDOF(int nx, int ny, int nz, bvector1D & info){
	
	int delta = 0;
	int dum = 0;
	double dval;
	switch(getShapeType()){
		
		case BasicShape::ShapeType::CYLINDER :
			delta += nz;
			nx--;
			if(getCoordType(1) == BasicShape::CoordType::PERIODIC)	ny--;

			info.push_back(true);
			info.push_back(getCoordType(1) == BasicShape::CoordType::PERIODIC);
			break;

		case BasicShape::ShapeType::SPHERE :
			delta ++;
			nx--;
			if(getCoordType(1) == BasicShape::CoordType::PERIODIC)	ny--;
			dval = getInfLimits()[2];
			if(dval == 0.0)	{
				nz--;
				delta += nx;
			}
			if((dval + getLocalSpan()[2]) == M_PI){
				nz--;
				delta += nx;
			}

			info.push_back(true);
			info.push_back(getCoordType(1) == BasicShape::CoordType::PERIODIC);
			info.push_back(dval==0.0);
			info.push_back((dval + getLocalSpan()[2]) == M_PI);
			break;
			
		default:
			//doing nothing
			break;
	}

	int result = nx*ny*nz + delta;
	return(result);
};


/*! Fill m_mapdeg with the ordered indices of dimensions.
*/
void FFDLattice::orderDimension(){

	map<pair<int,int>, int > mapsort;
	mapsort[make_pair(m_nx,0)] = 0;
	mapsort[make_pair(m_ny,1)] = 1;
	mapsort[make_pair(m_nz,2)] = 2;

	int i=0;
	for (map<pair<int,int>, int >::iterator it = mapsort.begin(); it != mapsort.end(); ++it){
		m_mapdeg[i] = it->second;
		i++;
	}

};

/*---------------------------------------------------------------------------*\
 *
 *  MiMMO
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of mimmo.
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

#include "MiMMO.hpp"
#include <functional>
#include "customOperators.hpp"
using namespace std;
using namespace bitpit;
using namespace mimmo;


#include <chrono>

using namespace std::chrono;
using namespace std::placeholders;

// =================================================================================== //

void test0005() {




	//Instantiation of mimmo geometry Object.
	MimmoGeometry* geometry = new MimmoGeometry();
	geometry->setRead(true);
	geometry->setReadFileType(0);
	geometry->setReadDir("./geo_data");
	geometry->setReadFilename("sphere2");
	geometry->setWrite(true);
	geometry->setWriteFileType(0);
	geometry->setWriteDir(".");
	string filename = "mimmo_0005.0000";
	geometry->setWriteFilename(filename);

	//Instantiation of a FFDobject (and Input object).
	FFDLattice* lattice = new FFDLattice();

	//Set Inputs with Shape and Mesh Info
	darray3E origin = {0.0, 0.0, 0.0};
	darray3E span;
	span[0]= 1.6;
	span[1]= 1.6;
	span[2]= 1.6;

	iarray3E dim, deg;
	dim[0] = 20;
	dim[1] = 20;
	dim[2] = 20;
	deg[0] = 2;
	deg[1] = 2;
	deg[2] = 2;

	int t = 0;
	GenericInput* inputshapet = new GenericInput();
	inputshapet->setInput(t);
	inputshapet->setName("MiMMO.InputShape");

	GenericInput* inputorig = new GenericInput();
	inputorig->setInput(origin);
	inputorig->setName("MiMMO.InputOrigin");

	GenericInput* inputspan = new GenericInput();
	inputspan->setInput(span);
	inputspan->setName("MiMMO.InputSpan");

	GenericInput* inputdim = new GenericInput();
	inputdim->setInput(dim);
	inputdim->setName("MiMMO.InputDim");

	GenericInput* inputdeg = new GenericInput();
	inputdeg->setInput(deg);
	inputdeg->setName("MiMMO.InputDeg");

	GenericInput* inputname = new GenericInput();
	string name = "test_MIMMO_0005.out";
	inputname->setInput(name);
	inputname->setName("MiMMO.InputName");

	GenericOutput* output = new GenericOutput();

	//Set Input with Init Displacements
	int ndeg = (dim[0])*(dim[1])*(dim[2]);
	dvecarr3E displ(ndeg);
	time_t Time = time(NULL);
	srand(Time);
	for (int i=0; i<ndeg; i++){
		for (int j=0; j<3; j++){
			displ[i][j] = 0.25*( (double) (rand()) / RAND_MAX - 0.5);
		}
	}
	GenericInput* input = new GenericInput();
	input->setInput(displ);
	input->setName("MiMMO.InputDispl");

	//create aux lattice for mesh and nodes coordinates
	Lattice* mesh = new Lattice();
	mesh->setShape(t);
	mesh->setSpan(span);
	mesh->setOrigin(origin);
	mesh->setDimension(dim);

	lattice->setShape(t);
	lattice->setSpan(span);
	lattice->setOrigin(origin);
	lattice->setDimension(dim);
	lattice->setDegrees(deg);

	//create Mask
	Mask* mask = new Mask();
	mask->setThresholds({{-10.0,0}}, 0);
	mask->setThresholds({{-10.0,10.0}}, 1);
	mask->setThresholds({{-10.0,10.0}}, 2);
	mask->setInside(0, true);
	mask->setInside(1, true);
	mask->setInside(2, true);

	//create Bend
	Bend* bend = new Bend();
	uint32_t	degree = 2;
	dvector1D	coeffs(degree+1);
	coeffs[0] = 0;
	coeffs[1] = 0;
	coeffs[2] = 1;
	bend->setDegree(2,0,degree);
	bend->setCoeffs(2,0,coeffs);

	//create TranslationBox
	TranslationBox* translation = new TranslationBox();
	translation->setDirection({{1.0, 1.0, 0.2}});
	translation->setTranslation(-0.1);

	//create RotationBox
	RotationBox* rotation = new RotationBox();
	rotation->setOrigin({{-0.25, -0.25, 0.0}});
	rotation->setDirection({{0.5, 0.5, 2.0}});
	rotation->setRotation(M_PI/4.0);

	//create applier
	Apply* applier = new Apply();

	//Set PINS
	cout << "set pins" << endl;

	cout << "add pin info 1 : " << boolalpha << addPin(geometry, lattice, PortType::PortType::M_GEOM, PortType::PortType::M_GEOM) << endl;
	cout << "add pin info 2 : " << boolalpha << addPin(geometry, applier, PortType::PortType::M_GEOM, PortType::PortType::M_GEOM) << endl;

	cout << "add pin info 3 : " << boolalpha << addPin(inputshapet, mesh, PortType::PortType::M_VALUEI, PortType::PortType::M_SHAPEI) << endl;
	cout << "add pin info 4 : " << boolalpha << addPin(inputorig, mesh, PortType::PortType::M_POINT, PortType::PortType::M_POINT) << endl;
	cout << "add pin info 5 : " << boolalpha << addPin(inputspan, mesh, PortType::PortType::M_SPAN, PortType::PortType::M_SPAN) << endl;
	cout << "add pin info 6 : " << boolalpha << addPin(inputdim, mesh, PortType::PortType::M_DIMENSION, PortType::PortType::M_DIMENSION) << endl;

	cout << "add pin info 7 : " << boolalpha << addPin(mesh, mask, PortType::PortType::M_GLOBAL, PortType::PortType::M_COORDS) << endl;
	cout << "add pin info 8 : " << boolalpha << addPin(input, mask, PortType::PortType::M_DISPLS, PortType::PortType::M_DISPLS) << endl;

	cout << "add pin info 9 : " << boolalpha << addPin(mask, bend, PortType::PortType::M_COORDS, PortType::PortType::M_COORDS) << endl;
	cout << "add pin info 10 : " << boolalpha << addPin(mask, bend, PortType::PortType::M_DISPLS, PortType::PortType::M_DISPLS) << endl;

	cout << "add pin info 11 : " << boolalpha << addPin(mesh, translation, PortType::PortType::M_POINT, PortType::PortType::M_POINT) << endl;
	cout << "add pin info 12 : " << boolalpha << addPin(translation, rotation, PortType::PortType::M_POINT, PortType::PortType::M_POINT) << endl;
	cout << "add pin info 13 : " << boolalpha << addPin(mesh, rotation, PortType::PortType::M_AXES, PortType::PortType::M_AXES) << endl;

	cout << "add pin info 14 : " << boolalpha << addPin(inputshapet, lattice, PortType::M_VALUEI, PortType::M_SHAPEI) << endl;
	cout << "add pin info 15 : " << boolalpha << addPin(rotation, lattice, PortType::M_POINT, PortType::M_POINT) << endl;
	cout << "add pin info 16 : " << boolalpha << addPin(rotation, lattice, PortType::M_AXES, PortType::M_AXES) << endl;
	cout << "add pin info 17 : " << boolalpha << addPin(inputspan, lattice, PortType::M_SPAN, PortType::M_SPAN) << endl;
	cout << "add pin info 18 : " << boolalpha << addPin(inputdim, lattice, PortType::M_DIMENSION, PortType::M_DIMENSION) << endl;
	cout << "add pin info 19 : " << boolalpha << addPin(inputdeg, lattice, PortType::M_DEG, PortType::M_DEG) << endl;

	cout << "add pin info 20 : " << boolalpha << addPin(inputname, output, PortType::M_FILENAME, PortType::M_FILENAME) << endl;
	cout << "add pin info 21 : " << boolalpha << addPin(bend, output, PortType::M_DISPLS, PortType::M_DISPLS) << endl;

	cout << "add pin info 22 : " << boolalpha << addPin(bend, lattice, PortType::M_DISPLS, PortType::M_DISPLS) << endl;
	cout << "add pin info 23 : " << boolalpha << addPin(lattice, applier, PortType::M_GDISPLS, PortType::M_GDISPLS) << endl;

	cout << "set pins done" << endl;

	//Create chain
	Chain ch0;
	cout << "add inputs and objects to the chain" << endl;
//	ch0.addObject(inputname);
//	ch0.addObject(inputorig);
	ch0.addObject(geometry);
	ch0.addObject(lattice);
//	ch0.addObject(inputshapet);
	ch0.addObject(bend);
//	ch0.addObject(inputspan);
	ch0.addObject(applier);
//	ch0.addObject(inputdim);
	ch0.addObject(mesh);
//	ch0.addObject(inputdeg);
	ch0.addObject(rotation);
	ch0.addObject(input);
	ch0.addObject(mask);
	ch0.addObject(translation);
//	ch0.addObject(output);

	//Execution of chain
	cout << "execution start" << endl;
	steady_clock::time_point t1 = steady_clock::now();
	ch0.exec();
	steady_clock::time_point t2 = steady_clock::now();
	cout << "execution done" << endl;

	//Plot results
	lattice->plotGrid("./", "lattice_0005", 0, false, false);
	lattice->plotGrid("./", "lattice_0005", 1, false, true);

	filename = "mimmo_0005.0001";
	geometry->setWriteFilename(filename);
	geometry->write();

	//Delete and nullify pointer
	cout << "delete geom" << endl;
	delete geometry;
	cout << "delete lattice" << endl;
	delete lattice;
	cout << "delete applier" << endl;
	delete applier;
	cout << "delete input" << endl;
	delete input;
	delete inputorig;
	delete inputspan;
	delete inputshapet;
	delete inputdim;
	delete inputdeg;

	geometry 	= NULL;
	lattice 	= NULL;
	applier 	= NULL;
	inputorig 	= NULL;
	inputspan 	= NULL;
	inputshapet	= NULL;
	inputdim 	= NULL;
	inputdeg 	= NULL;
	input 		= NULL;

	//Print execution time
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "MiMMO execution took me " << time_span.count() << " seconds.";
	std::cout << std::endl;

	return;

}

// =================================================================================== //

int main( int argc, char *argv[] ) {

#if ENABLE_MPI==1
	MPI::Init(argc, argv);

	{
#endif
		/**<Calling MiMMO Test routines*/

		test0005() ;

#if ENABLE_MPI==1
	}

	MPI::Finalize();
#endif
}

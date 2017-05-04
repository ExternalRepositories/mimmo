/*---------------------------------------------------------------------------
#
#  mimmo
#
#  Copyright (C) 2015-2016 OPTIMAD engineering Srl
#
#  -------------------------------------------------------------------------
#  License
#  This file is part of mimmo.
#
#  mimmo is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License v3 (LGPL)
#  as published by the Free Software Foundation.
#
#  mimmo is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
#  License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with mimmo. If not, see <http://www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------*/


#include "mimmo_manipulators.hpp"
#include "mimmo_iogeneric.hpp"
#include "bitpit.hpp"
using namespace std;
using namespace bitpit;
using namespace mimmo;
using namespace mimmo::pin;

// =================================================================================== //
/*!
	\example manipulators_example_00002.cpp

	\brief Example of usage of free form deformation block to manipulate an input geometry.

	Geometry deformation block used: FFD (shape cube).

	<b>To run</b>: ./manipulators_example_00002 \n

	<b> visit</b>: <a href="http://optimad.github.io/mimmo/">mimmo website</a> \n
 */

void test00002() {

    /* Creation of mimmo containers.
     * Input and output MimmoGeometry are instantiated
     * as two different objects (no loop in chain are permitted).
     */
    MimmoGeometry * mimmo0 = new MimmoGeometry();

    mimmo0->setRead(true);
    mimmo0->setReadDir("geodata");
    mimmo0->setReadFileType(FileType::STL);
    mimmo0->setReadFilename("sphere2");
    mimmo0->setBuildBvTree(true);

    mimmo0->setWrite(true);
    mimmo0->setWriteDir("./");
    mimmo0->setWriteFileType(FileType::STL);
    mimmo0->setWriteFilename("manipulators_output_00002.0000");

    MimmoGeometry * mimmo1 = new MimmoGeometry();
    mimmo1->setRead(false);
    mimmo1->setWrite(true);
    mimmo1->setWriteDir(".");
    mimmo1->setWriteFileType(FileType::STL);
    mimmo1->setWriteFilename("manipulators_output_00002.0001");

    /* Instantiation of a FFDobject with default shape cube.
     * Setup of span and origin of cube.
     * Plot Optional results during execution active for FFD block.
     */
    FFDLattice* lattice = new FFDLattice();
    darray3E origin = {0.0, 0.0, 0.0};
    darray3E span;
    span[0]= 1.2;
    span[1]= 1.2;
    span[2]= 1.2;

    /* Set number of nodes of the mesh (dim) and degree of nurbs functions (deg).
     */
    iarray3E dim, deg;
    dim[0] = 20;
    dim[1] = 20;
    dim[2] = 20;
    deg[0] = 2;
    deg[1] = 2;
    deg[2] = 2;

    lattice->setLattice(origin, span, ShapeType::CUBE, dim, deg);
    lattice->setPlotInExecution(true);

    /* Creation of Generic input block to read the
     * displacements of the control nodes of the lattice.
     */
    GenericInput* input = new GenericInput();
    input->setReadFromFile(true);
    input->setFilename("input/input_mimmo_00002.txt");

    /* Create applier block.
     * It applies the deformation displacements to the original input geometry.
     */
    Apply* applier = new Apply();

    /* Setup pin connections.
     */
    cout << " --- create pin ---" << endl;
    cout << " " << endl;
    /* Add pin with port ID
     */
    cout << " add pin info : " << boolalpha << addPin(mimmo0, lattice, 99, 99) << endl;
    cout << " add pin info : " << boolalpha << addPin(input, lattice, 10, 10) << endl;
    /* Add pin with port TAG
     */
    cout << " add pin info : " << boolalpha << addPin(lattice, applier, PortType::M_GDISPLS, PortType::M_GDISPLS) << endl;
    cout << " add pin info : " << boolalpha << addPin(mimmo0, applier, PortType::M_GEOM, PortType::M_GEOM) << endl;
    cout << " add pin info : " << boolalpha << addPin(applier, mimmo1, PortType::M_GEOM, PortType::M_GEOM) << endl;
    cout << " " << endl;

    /* Setup execution chain.
     */
    Chain ch0;
    ch0.addObject(mimmo0);
    ch0.addObject(input);
    ch0.addObject(lattice);
    ch0.addObject(applier);
    ch0.addObject(mimmo1);

    /* Execution of chain.
     * Use debug flag false (default) to avoid to print out the execution steps.
     */
    cout << " " << endl;
    cout << " --- execution start ---" << endl;
    ch0.exec(false);
    cout << " --- execution done --- " << endl;
    cout << " " << endl;

    /* Clean up & exit;
     */
    delete lattice;
    delete applier;
    delete input;
    delete mimmo0;
    delete mimmo1;

    lattice = NULL;
    applier = NULL;
    input 	= NULL;
    mimmo0  = NULL;
    mimmo1  = NULL;

}

int main( int argc, char *argv[] ) {

    BITPIT_UNUSED(argc);
    BITPIT_UNUSED(argv);

#if ENABLE_MPI==1
    MPI::Init(argc, argv);

    {
#endif
        /**<Calling mimmo Test routine*/

        test00002() ;

#if ENABLE_MPI==1
    }

    MPI::Finalize();
#endif

    return(1);
}

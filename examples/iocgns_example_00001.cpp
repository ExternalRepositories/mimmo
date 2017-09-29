/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-2017 OPTIMAD engineering Srl
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
 \ *---------------------------------------------------------------------------*/
#include "mimmo_iocgns.hpp"
#include "mimmo_geohandlers.hpp"
#include "mimmo_manipulators.hpp"

using namespace mimmo;

// =================================================================================== //

void example00001() {

    /* Create IO_CGNS object to import input file. */
    IOCGNS * cgnsI = new IOCGNS();
    cgnsI->setRead(true);
    cgnsI->setReadDir("geodata");
    cgnsI->setReadFilename("grid");

    /* Create IO_CGNS object to export output file. */
    IOCGNS * cgnsO = new IOCGNS();
    cgnsO->setRead(false);
    cgnsO->setWriteDir(".");
    cgnsO->setWriteFilename("iocgns_output_00001");

    /* Create CGNS PID extractor object to test input file.
     * Extraction of PID = 1 (Wing wall boundary in input file).
     */
    CGNSPidExtractor * cgnsExtr = new CGNSPidExtractor();
    cgnsExtr->setPID({1});
    cgnsExtr->setForcedToTriangulate(false);
    cgnsExtr->setPlotInExecution(true);

    /* Instantiation of a Selection By Box block.
     * Setup of span and origin of cube.
     */
    SelectionByBox      * boxSel = new SelectionByBox();
    boxSel->setOrigin({{700., 1500., 0.}});
    boxSel->setSpan(1500.,700.,100.);
    boxSel->setPlotInExecution(true);

    /* Creation of rotation block.
     */
    RotationGeometry* rotation = new RotationGeometry();
    rotation->setDirection(darray3E{0.,1.,0.});
    rotation->setRotation((M_PI/4));
    rotation->setPlotInExecution(true);

    /* Create reconstruct vector block and set to recontruct over the whole
     * input geometry the displacements field.
     */
    ReconstructVector* recon = new ReconstructVector();
    recon->setPlotInExecution(true);

    /* Create PINs. */
    addPin(cgnsI, cgnsO, M_GEOM, M_GEOM);
    addPin(cgnsI, cgnsExtr, M_GEOM2, M_GEOM);
    addPin(cgnsExtr, boxSel, M_GEOM, M_GEOM);
    addPin(boxSel, rotation, M_GEOM, M_GEOM);
    addPin(rotation, recon, M_GDISPLS, M_VECTORFIELD);
    addPin(cgnsI, recon, M_GEOM2, M_GEOM);

    addPin(cgnsI, cgnsO, M_GEOM2, M_GEOM2);
    addPin(cgnsI, cgnsO, M_BCCGNS, M_BCCGNS);

    /* Create and execute chain. */
    Chain ch0;
    ch0.addObject(cgnsI);
    ch0.addObject(cgnsExtr);
    ch0.addObject(boxSel);
    ch0.addObject(rotation);
    ch0.addObject(recon);
    ch0.addObject(cgnsO);
    ch0.exec(true);

    cgnsI->getSurfaceBoundary()->getPatch()->write();

    /* Destroy objects. */
    delete cgnsI;
    delete cgnsExtr;
    delete cgnsO;
    cgnsI       = NULL;
    cgnsExtr    = NULL;
    cgnsO       = NULL;

    return;
}

// =================================================================================== //

int main( int argc, char *argv[] ) {
    
    BITPIT_UNUSED(argc);
    BITPIT_UNUSED(argv);
    
#if ENABLE_MPI==1
    MPI::Init(argc, argv);

    {
#endif
        /**< Call mimmo example routine. */
        example00001();

#if ENABLE_MPI==1
    }

    MPI::Finalize();
#endif

    return 0;
}


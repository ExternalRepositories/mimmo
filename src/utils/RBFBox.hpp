/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-201/ OPTIMAD engineering Srl
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
#ifndef __RBFBox_HPP__
#define __RBFBox_HPP__

#include "BaseManipulation.hpp"

namespace mimmo{

/*!
 *    \class RBFBox
 *    \ingroup utils
 *    \brief Radial Basis Functions Bounding Box calculator.
 *
 *    Builds the axes aligned bounding box of a set of RBF points.
 *
 * \n
 * Ports available in RBFBox Class :
 *
 *    =========================================================
 * ~~~
     |-------------------------------------------------------------------------------------|
     |                    Port Input                                                       |
     |-------|-------------|---------------------------------------|-----------------------|
     |PortID | PortType    | variable/function                     | DataType              |
     |-------|-------------|---------------------------------------|-----------------------|
     | 0     | M_COORDS    | setNode                               | (VECARR3, FLOAT)      |
     | 30    | M_VALUED    | setSupportRadius                      | (SCALAR, FLOAT)       |
     |-------|-------------|---------------------------------------|-----------------------|


     |---------------------------------------------------------------|
     |               Port Output                                     |
     |-------|-------------|-------------------|---------------------|
     |PortID | PortType    | variable/function | DataType            |
     |-------|-------------|-------------------|---------------------|
     | 20    | M_POINT     | getOrigin         | (ARRAY3, FLOAT)     |
     | 22    | M_AXES      | getAxes           | (ARR3ARR3, FLOAT)   |
     | 23    | M_SPAN      | getSpan           | (ARRAY3, FLOAT)     |
     |-------|-------------|-------------------|---------------------|
  ~~~
 *    =========================================================
 * \n
 * The xml available parameters, sections and subsections are the following :
 *
 * - <B>ClassName</B>: name of the class as <tt>mimmo.RBFBox</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>SupportRadius</B>: Influence Radius value for RBF cloud in input;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Nodes list has to be mandatorily passed through port.
 *
 */
class RBFBox: public BaseManipulation {

protected:
    darray3E    m_origin;        /**< Origin of the BB.*/
    darray3E    m_span;         /**< Span of the BB. */
    dmatrix33E    m_axes;            /**<reference system of the bbox (axes aligned AABB) */
    dvecarr3E   m_nodes;        /**<Radial Basis Functions control points.*/
    double      m_suppR;        /**<Support radius value of RBFs.*/


public:
    RBFBox();
    RBFBox(const bitpit::Config::Section & rootXML);
    virtual ~RBFBox();

    //copy operators/constructors
    RBFBox(const RBFBox & other);
    RBFBox & operator=(const RBFBox & other);

    void buildPorts();

    //clean structure;
    void         clearRBFBox();

    //internal methods
    darray3E    getOrigin();
    darray3E    getSpan();
    dmatrix33E    getAxes();
    void        setNode(dvecarr3E);
    void        setSupportRadius(double suppR_);

    //plotting wrappers
    void        plot(std::string directory, std::string filename, int counter, bool binary);

    //building method
    void execute();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name = "");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name= "");


protected:
    virtual void plotOptionalResults();

};

REGISTER(BaseManipulation,RBFBox, "mimmo.RBFBox")
};

#endif /* __RBFBOX_HPP__ */

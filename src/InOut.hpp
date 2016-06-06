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
#ifndef __INOUT_HPP__
#define __INOUT_HPP__

#include "MiMMO_TypeDef.hpp"
#include "binary_stream.hpp"
#include "MimmoNamespace.hpp"
#include <functional>

bitpit::IBinaryStream& operator>>(bitpit::IBinaryStream &buf,dvecarr3E& element);
bitpit::OBinaryStream& operator<<(bitpit::OBinaryStream &buf, const dvecarr3E& element);

namespace mimmo{

class BaseManipulation;

/**< @namespace mimmo
 * mimmo namespace
 */

/**
 * Port type specification.
 *
 * A type of data is related to each label. Same type of data can be related to
 * multiple type of ports but with different meaning.
 *
 * mimmo::pin::PortType::COORDS -
 * Port dedicated to communicates coordinates of points.
 * A port COORDS communicates a std::vector<std::array<double, 3> >.
 *
 * mimmo::pin::PortType::DISPLS -
 * Port dedicated to communicates displacements of points.
 * A port DISPLS communicates a std::vector<std::array<double, 3> >.
 *
 *  mimmo::pin::PortType::FILTER -
 *  Port dedicated to communicates a scalar field used as filter function.
 *  A port FILTER communicates a std::vector<double>.
 */
typedef mimmo::pin::PortType	PortType;
typedef short int 				PortID;


/*!
 *	\date			14/mar/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief PortOut is the input-output PIN base class.
 *
 *	A pin is an object member of BaseManipulation object.
 *	Through a pin two base manipulation objects are linked together. One of these two
 *	objects is the parent object that gives an output to the other one that takes
 *	this value as input. Therefore a pin can be OR input OR output pin for an object.
 *
 */
class PortOut{
public:



	//members
	bitpit::OBinaryStream				m_obuffer;	/**<Output buffer to communicate data.*/
	std::vector<BaseManipulation*>		m_objLink;	/**<Outputs object to which communicate the data.*/
	std::vector<PortID>					m_portLink;	/**<ID of the input ports of the linked objects.*/

	PortType							m_label;	/**<Label (tag) of the type of the port.*/

public:
	PortOut();
	virtual ~PortOut();

	PortOut(const PortOut & other);
	PortOut & operator=(const PortOut & other);
	bool operator==(const PortOut & other);

	PortType						getLabel();
	std::vector<BaseManipulation*>	getLink();
	std::vector<PortID>				getPortLink();

	virtual void	writeBuffer() = 0;
	void 			cleanBuffer();

	void clear();
	void clear(int j);

	/*!Execution method.
	 */
	void exec();

};


//==============================================================//
// TEMPLATE DERIVED INOUT CLASS									//
//==============================================================//

/*!
 *	\date			14/mar/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief PortOutT is the input-output templated PIN class derived from base PortOut pin class.
 *
 *	A pin is an object member of BaseManipulation object.
 *	Through a pin two base manipulation objects are linked together. One of these two
 *	objects is the parent object that gives an output to the other one that takes
 *	this value as input. Therefore a pin can be OR input OR output pin for an object.
 *
 *	The parent object gives the output value by a function set by the user
 *	during the creation of the pin (getVal functions), while the child object
 *	use this value by a set function (setVal function).
 *	The functions are given to the pin by get/set function objects with any
 *	type of value (the same for the two functions of the linked object) by using
 *	the template formulation. The functions have to be function objects of the standard library
 *	(functional include) created by the bind method.
 *	The input/output value can be returned by copy, reference or pointer by the get function
 *	of the parent and it can be passed by copy or pointer to the set value of the child object.
 *	The output pins of an object are executed after its own execution.
 *
 */
template<typename T, typename O>
class PortOutT: public PortOut {

public:

	O*		m_obj_;				/**<Object owner of the port.*/
	T		*m_var_;			/**<Linked variable to communicate.*/
	T		(O::*m_getVar_)();	/**<Pointer to function that recovers the data to communicate (alternative to linked variable).*/

public:
	PortOutT();
	PortOutT(O* obj_, T *var_);
	PortOutT(O* obj_, T (O::*getVar_)());
	virtual ~PortOutT();

	PortOutT(const PortOutT & other);
	PortOutT & operator=(const PortOutT & other);
	bool operator==(const PortOutT & other);

	void writeBuffer();
	void readBuffer();

};



/*!
 *	\date			14/mar/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief PortOut is the input-output PIN base class.
 *
 *	A pin is an object member of BaseManipulation object.
 *	Through a pin two base manipulation objects are linked together. One of these two
 *	objects is the parent object that gives an output to the other one that takes
 *	this value as input. Therefore a pin can be OR input OR output pin for an object.
 *
 */
class PortIn{
public:
	//members
	bitpit::IBinaryStream				m_ibuffer;	/**<input buffer to recover data.*/
	BaseManipulation*					m_objLink;	/**<Input object from which	recover the data. */
	std::vector<PortType>				m_labelOK;	/**<Compatibility output port labels. */

public:
	PortIn();
	virtual ~PortIn();

	PortIn(const PortIn & other);
	PortIn & operator=(const PortIn & other);
	bool operator==(const PortIn & other);

	void addCompatibility(PortType label);

	const std::vector<PortType>&	getCompatibility();

	BaseManipulation*	getLink();

	void clear();

	virtual void	readBuffer() = 0;
	void 			cleanBuffer();

};


//==============================================================//
// TEMPLATE DERIVED INOUT CLASS									//
//==============================================================//

/*!
 *	\date			14/mar/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief PortInT is the input-output templated PIN class derived from base PortIn pin class.
 *
 *	A pin is an object member of BaseManipulation object.
 *	Through a pin two base manipulation objects are linked together. One of these two
 *	objects is the parent object that gives an output to the other one that takes
 *	this value as input. Therefore a pin can be OR input OR output pin for an object.
 *
 *	The parent object gives the output value by a function set by the user
 *	during the creation of the pin (getVal functions), while the child object
 *	use this value by a set function (setVal function).
 *	The functions are given to the pin by get/set function objects with any
 *	type of value (the same for the two functions of the linked object) by using
 *	the template formulation. The functions have to be function objects of the standard library
 *	(functional include) created by the bind method.
 *	The input/output value can be returned by copy, reference or pointer by the get function
 *	of the parent and it can be passed by copy or pointer to the set value of the child object.
 *	The output pins of an object are executed after its own execution.
 *
 */
template<typename T>
class PortInT: public PortIn {

public:

	T		*m_var_;	/**<Linked variable to fill with communicated data.*/

public:
	PortInT();
	PortInT(T *var_);
	virtual ~PortInT();

	PortInT(const PortInT & other);
	PortInT & operator=(const PortInT & other);
	bool operator==(const PortInT & other);

	void writeBuffer();
	void readBuffer();

};


}

#include "InOut.tpp"

#endif /* __INOUT_HPP__ */

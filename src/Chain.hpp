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
#ifndef __CHAIN_HPP__
#define __CHAIN_HPP__

#include "BaseManipulation.hpp"

/*!
 *	\date			22/feb/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief Chain is the class used to manager the execution chain of MiMMO objects.
 *
 *
 */
class Chain{
public:
	//members
	uint8_t							m_id;			/**<ID of the chain.*/
	std::vector<BaseManipulation*>	m_objects;		/**<Pointers to manipulation objects placed in the current execution chain. */
	std::vector<int>				m_idObjects;	/**<ID (order of insertion) of the mimmo objects in the chain. */
	uint32_t						m_objcounter;	/**<Counter of objects inserted the chain.*/

	//static members
	static	uint8_t					sm_chaincounter;/**<Current global number of chain in the instance. */

public:
	Chain();
	~Chain();

	Chain(const Chain & other);
	Chain & operator=(const Chain & other);

	//internal methods
	uint32_t	getNObjects();
	uint8_t		getID();
	uint8_t		getNChains();

	int			addObject(BaseManipulation* obj, int id_ = 0);

	bool		deleteObject(int idobj);
	void		clear();

	//relationship methods
	void 		exec();
	void 		exec(int idobj);

};

#endif /* __CHAIN_HPP__ */

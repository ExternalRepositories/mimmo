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
#ifndef __TRACKINGPOINTER_HPP__
#define __TRACKINGPOINTER_HPP__

namespace mimmo{
/*!
 *	\date			07/jul/2016
 *	\authors		Rocco Arpa
 *
 *	\brief Basic virtual class to create a generic object whose pointer can return an identifying name.
 */
class TrackingPointer{
public:
	TrackingPointer();
	virtual ~TrackingPointer();

	virtual std::string whichClass() = 0;
};

}

#endif /* __APPLYDEFORMATION_HPP__ */

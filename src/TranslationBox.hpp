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
#ifndef __TRANSLATIONBOX_HPP__
#define __TRANSLATIONBOX_HPP__

#include "BaseManipulation.hpp"
#include "FFDLattice.hpp"

/*!
 *	\date			09/feb/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief TranslationBox is the class that applies the a translation to the box of a latticeBox object.
 *
 *	Result saved in result of base class and in modified member m_origin.
 */
class TranslationBox: public BaseManipulation{
private:
	//members
	darray3E	m_direction;	/**<Components of the translation axis.*/
	darray3E	m_origin;		/**<Origin of box to be deformed (recovered in recoverInfo and used in useInfo).*/
	double		m_alpha;		/**<Value of translation.*/

public:
	TranslationBox(darray3E direction = { {0, 0, 0} });
	~TranslationBox();

	TranslationBox(const TranslationBox & other);
	TranslationBox & operator=(const TranslationBox & other);

	darray3E 	getDirection();
	darray3E 	getOrigin();
	double 		getTranslation();
	void 		setDirection(darray3E direction);
	void 		setOrigin(darray3E origin);
	void 		setTranslation(double alpha);

	void 	execute();

};

#endif /* __TRANSLATIONBOX_HPP__ */

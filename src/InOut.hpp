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
#include <functional>

class BaseManipulation;
/*!
 *	\date			14/mar/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief InOut is the input-output PIN class.
 *
 *	OR input OR output.
 *
 */
class InOut{
public:
	//members
	bool				m_type;		/**<Type of pin 0/1 = input/output pin.*/
	BaseManipulation*	m_obj;		/**<Owner object of this input/output PIN. */
	BaseManipulation*	m_objLink;	/**<Input/Output object from/to which recover/give the target variable. */


public:
	InOut();
	~InOut();

	InOut(const InOut & other);
	InOut & operator=(const InOut & other);

	bool				getType();
	BaseManipulation*	getLink();

	virtual void exec() = 0;

};

//==============================================================//
// TEMPLATE DERIVED INOUT CLASS									//
//==============================================================//

template<typename T>
class InOutT: public InOut {

public:
	std::function<T(void)>	m_getVal;	/**<Pointer to get function with copy return.*/
	std::function<T&(void)>	m_getValR;	/**<Pointer to get function with reference return.*/
	std::function<T*(void)>	m_getValP;	/**<Pointer to get function with pointer return.*/
	std::function<void(T)>	m_setVal;	/**<Pointer to set function with copy argument.*/
	std::function<void(T*)>	m_setValP;	/**<Pointer to set function with pointer argument.*/

public:
	InOutT();
	~InOutT();

	InOutT(const InOutT & other);
	InOutT & operator=(const InOutT & other);

	void setInput(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T)> setVal);
	void setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T(void)> getVal);
	void setInput(BaseManipulation* objIn, std::function<T&(void)> getValR, std::function<void(T)> setVal);
	void setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T&(void)> getValR);
	void setInput(BaseManipulation* objIn, std::function<T*(void)> getValP, std::function<void(T)> setVal);
	void setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T*(void)> getValP);

	void setInput(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T*)> setValP);
	void setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T(void)> getVal);
	void setInput(BaseManipulation* objIn, std::function<T&(void)> getValR, std::function<void(T*)> setValP);
	void setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T&(void)> getValR);
	void setInput(BaseManipulation* objIn, std::function<T*(void)> getValP, std::function<void(T*)> setValP);
	void setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T*(void)> getValP);

	void exec();

};

//==============================================================//
// TEMPLATE DERIVED INOUT CLASS	TEMPLATE METHODS				//
//==============================================================//

/*!Default constructor of InOutT
 */
template<typename T>
InOutT<T>::InOutT(){
	m_getVal 	= NULL;
	m_getValR 	= NULL;
	m_getValP 	= NULL;
	m_setVal 	= NULL;
	m_setValP 	= NULL;
};


/*!Default destructor of InOutT
 */
template<typename T>
InOutT<T>::~InOutT(){
	m_getVal 	= NULL;
	m_getValR 	= NULL;
	m_getValP 	= NULL;
	m_setVal 	= NULL;
	m_setValP 	= NULL;
};

/*!Copy constructor of InOutT.
 */
template<typename T>
InOutT<T>::InOutT(const InOutT<T> & other){
	m_getVal 	= other.m_getVal;
	m_getValR 	= other.m_getValR;
	m_getValP 	= other.m_getValP;
	m_setVal 	= other.m_setVal;
	m_setValP 	= other.m_setValP;
};

/*!Assignement operator of InOutT.
 */
template<typename T>
InOutT<T> & InOutT<T>::operator=(const InOutT<T> & other){
	m_getVal 	= other.m_getVal;
	m_getValR 	= other.m_getValR;
	m_getValP 	= other.m_getValP;
	m_setVal 	= other.m_setVal;
	m_setValP 	= other.m_setValP;
	return (*this);
};

template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T)> setVal){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getVal	= getVal;
	m_setVal	= setVal;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T(void)> getVal){
	m_type 		= true;
	m_objLink	= objOut;
	m_getVal	= getVal;
	m_setVal	= setVal;
};

template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T&(void)> getValR, std::function<void(T)> setVal){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getValR	= getValR;
	m_setVal	= setVal;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T&(void)> getValR){
	m_type 		= true;
	m_objLink	= objOut;
	m_getValR	= getValR;
	m_setVal	= setVal;
};

template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T*(void)> getValP, std::function<void(T)> setVal){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getValP	= getValP;
	m_setVal	= setVal;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T*(void)> getValP){
	m_type 		= true;
	m_objLink	= objOut;
	m_getValP	= getValP;
	m_setVal	= setVal;
};


template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T*)> setValP){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getVal	= getVal;
	m_setValP	= setValP;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T(void)> getVal){
	m_type 		= true;
	m_objLink	= objOut;
	m_getVal	= getVal;
	m_setValP	= setValP;
};

template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T&(void)> getValR, std::function<void(T*)> setValP){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getValR	= getValR;
	m_setValP	= setValP;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T&(void)> getValR){
	m_type 		= true;
	m_objLink	= objOut;
	m_getValR	= getValR;
	m_setValP	= setValP;
};

template<typename T>
void
InOutT<T>::setInput(BaseManipulation* objIn, std::function<T*(void)> getValP, std::function<void(T*)> setValP){
	m_type 		= false;
	m_objLink 	= objIn;
	m_getValP	= getValP;
	m_setValP	= setValP;
};

template<typename T>
void
InOutT<T>::setOutput(BaseManipulation* objOut, std::function<void(T*)> setValP, std::function<T*(void)> getValP){
	m_type 		= true;
	m_objLink	= objOut;
	m_getValP	= getValP;
	m_setValP	= setValP;
};


template<typename T>
void
InOutT<T>::exec(){
	if (m_getVal != NULL){
		T val = m_getVal();
		if (m_setVal != NULL){
			m_setVal(val);
		}else if (m_setValP != NULL){
			m_setValP(&val);
		}
	}else if (m_getValR != NULL){
		T& val = m_getValR();
		if (m_setVal != NULL){
			m_setVal(val);
		}else if (m_setValP != NULL){
			m_setValP(&val);
		}
	}else if (m_getValP != NULL){
		T* val = m_getValP();
		if (m_setVal != NULL){
			m_setVal(*val);
		}else if (m_setValP != NULL){
			m_setValP(val);
		}
	}
};


#endif /* __INOUT_HPP__ */

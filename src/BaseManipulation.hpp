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
#ifndef __BASEMANIPULATION_HPP__
#define __BASEMANIPULATION_HPP__

#include "MimmoObject.hpp"
#include "Info.hpp"
#include "InOut.hpp"

#include <string>
#include <functional>
#include <unordered_map>


class IOData;

/*!
 *	\date			09/feb/2016
 *	\authors		Rocco Arpa
 *	\authors		Edoardo Lombardi
 *
 *	\brief BaseManipulation is the base class of any object (derived class) for manipulation of the geometry.
 *
 *	BaseManipulation is the base class used to build each generic or particular manipulation object.
 *	This base class has some common interface methods, as the base get/set methods.
 *	The only method to be called to execute the manipulation object is the method exec() that calls the pure virtual execute().
 *	Each manipulation base object has a linked geometry (a target MiMMO object) and one or more linked manipulation
 *	objects from wich recovering/distributing relevant data (as number of degrees of freedom, initial displacements or other). 
 *  The exchange of such data is realized through Pins (input/output link to same class objects). See mimmo::pin namespace 
 *  for further information about the linking procedure of BaseManipulation objects.
 *
 */
class BaseManipulation{

	//friendship declaration of mimmo::pin methods
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL (G::*fget) (), void (S::*fset) (VAL)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL* (G::*fget) (), void (S::*fset) (VAL)) ;

	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL& (G::*fget) (), void (S::*fset) (VAL)) ;

	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL (G::*fget) (), void (S::*fset) (VAL*)) ;

	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL* (G::*fget) (), void (S::*fset) (VAL*)) ;

	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void addPin(OO* objSend, OI* objRec, VAL& (G::*fget) (), void (S::*fset) (VAL*)) ;

	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL (G::*fget) (), void (S::*fset) (VAL)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL* (G::*fget) (), void (S::*fset) (VAL)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL& (G::*fget) (), void (S::*fset) (VAL)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL (G::*fget) (), void (S::*fset) (VAL*)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL* (G::*fget) (), void (S::*fset) (VAL*)) ;
	
	template<typename OO, typename G, typename OI, typename S, typename VAL>
	friend void removePin(OO* objSend, OI* objRec, VAL& (G::*fget) (), void (S::*fset) (VAL*)) ;
	
	
	
protected:
	MimmoObject*								m_geometry;		/**<Pointer to manipulated geometry. */
	std::unordered_map<BaseManipulation*, int>	m_parent;		/**<Pointers list to manipulation objects FATHER of the current class. List retains for each 
																	pointer a counter. When this counter is 0, pointer is released*/
	std::unordered_map<BaseManipulation*, int>	m_child;		/**<Pointers list to manipulation objects CHILD of the current class.List retains for each 
																	pointer a counter. When this counter is 0, pointer is released*/
																	
	std::vector<InOut*>				m_pinIn;		/**<Input pins vector. */
	std::vector<InOut*>				m_pinOut;		/**<Output pins vector. */

	std::unique_ptr<IOData>			m_input;		/**<Pointer to a base class object Input, meant for input temporary data, cleanable in execution (derived class is template).*/
	std::unique_ptr<IOData>			m_result;		/**<Pointer to a base class object Result (derived class is template).*/

public:
	BaseManipulation();
	~BaseManipulation();

	BaseManipulation(const BaseManipulation & other);
	BaseManipulation & operator=(const BaseManipulation & other);

	//get methods
	MimmoObject*		getGeometry();
	int					getNParent();
	BaseManipulation*	getParent(int i = 0);
	bool				isParent(BaseManipulation *, int);
	int					getNChild();
	BaseManipulation*	getChild(int i = 0);
	bool				isChild(BaseManipulation *, int);
	int 				getNPinIn();
	int 				getNPinOut();

	template<typename T>	
	T*					getInput();
	template<typename T>
	T* 					getResult();
	
	//set methods
	void 				setGeometry(MimmoObject* geometry);
	
	template<typename T>
	void 				setInput(T* data);
	template<typename T>
	void 				setInput(T& data);
	template<typename T>
	void 				setResult(T* data);
	template<typename T>
	void 				setResult(T& data);

	//cleaning/unset
	void 	unsetGeometry();

	//TODO 6) is useful to cancel all connection of this object and parent together with the presence of parent itself? see TODO 5) 
	void 	unsetParent(BaseManipulation * parent);
	void 	unsetChild(BaseManipulation * child);
	void 	unsetAllParent();
	void 	unsetAllChild();
	
	
	void	clearInput();
	void	clearResult();
	void	clear();
	
	//execution utils
	void 	exec();
	
protected:
	void				addParent(BaseManipulation* parent); 
	void				addChild(BaseManipulation* child);
	
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T(void)> getVal);
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T&(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T&(void)> getVal);
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T*(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T*(void)> getVal);
	
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T(void)> getVal);
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T&(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T&(void)> getVal);
	template<typename T>
	void				addPinIn(BaseManipulation* objIn, std::function<T*(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				addPinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T*(void)> getVal);

	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T(void)> getVal);
	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T&(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T&(void)> getVal);
	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T*(void)> getVal, std::function<void(T)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T)> setVal, std::function<T*(void)> getVal);
	
	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T(void)> getVal);
	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T&(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T&(void)> getVal);
	template<typename T>
	void				removePinIn(BaseManipulation* objIn, std::function<T*(void)> getVal, std::function<void(T*)> setVal);
	template<typename T>
	void				removePinOut(BaseManipulation* objOut, std::function<void(T*)> setVal, std::function<T*(void)> getVal);
	
	
	virtual void 	execute() = 0;				//called in exec
	
private:
	//TODO 5) Not completely meaningful list of internal pin cleaning methods. To be reviewed and reorganized. When launch them 
	// pins of other linked objects survive. You need to manage them also! Another case is when a class is destroyed? Destructor 
	//must implement also this pin removal.
	void 	removePins();
	void 	removePinsIn();
	void 	removePinsOut();
	void 	removePinIn(int i);
	void 	removePinOut(int i);
	
};


//==============================//
//DATA CLASS
//==============================//

//==============================//
//DATA BASE CLASS
//==============================//
class IOData{
	template<typename T>
	T* getData();

	template<typename T>
	void setData(T data);
};

//==============================//
//DATA DERIVED TEMPLATE CLASS
//==============================//
template<typename T>
class IODataT: public IOData{
	T m_data;

public:
	IODataT();
	IODataT(T data){
		m_data = data;
	};
	~IODataT();

	IODataT(const IODataT & other){
		m_data 	= other.m_data;
	}

	IODataT & operator=(const IODataT & other){
		m_data 	= other.m_data;
		return (*this);
	}

	void setData(T data){
		m_data = data;
	}

	T* getData(){
		return(&m_data);
	}

};

#include "BaseManipulation.tpp"

#endif /* __BASEMANIPULATION_HPP__ */

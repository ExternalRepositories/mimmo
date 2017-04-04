/*---------------------------------------------------------------------------*\
 * 
 *  mimmo
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
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

#include "mimmo_common.hpp"
using namespace std;
using namespace bitpit;
using namespace mimmo;


class BaseClass {
protected:
	std::string m_member;
public:
	
	BaseClass(){m_member="None";};
	virtual ~BaseClass(){};
	virtual std::string whoAmI() = 0;
	
};

class DerivedClass:public BaseClass {
public:
	DerivedClass(const bitpit::Config::Section & xmlroot){
		m_member= "DerivedClass";
	};
	virtual ~DerivedClass(){};
	std::string whoAmI(){return m_member;};
};

REGISTER(BaseClass, DerivedClass, "DerivedObject");

// =================================================================================== //

int test1() {
	
	
	auto & factory = Factory<BaseClass>::instance();
	std::string name = "DerivedObject";
	
	if(factory.containsCreator(name)){
		std::cout<<"Found instantiable object "<<name<<std::endl;
		std::unique_ptr<BaseClass> obj(factory.create(name, config::root));
		if(obj->whoAmI() == "DerivedClass"){
			std::cout<<"Correctly instantiated object of type "<<obj->whoAmI()<<std::endl;
			return 0;
		}else{
			std::cout<<"Instantition of object "<<name<<" failed."<<std::endl;
		}	
	}else{
		std::cout<<"NOT Found instantiable object "<<name<<std::endl;
		
	}
    return 1;
}

// =================================================================================== //

int main( int argc, char *argv[] ) {

	BITPIT_UNUSED(argc);
	BITPIT_UNUSED(argv);
	
#if ENABLE_MPI==1
	MPI::Init(argc, argv);

	{
#endif
		/**<Calling mimmo Test routines*/

        int val = test1() ;

#if ENABLE_MPI==1
	}

	MPI::Finalize();
#endif
	
	return val;
}

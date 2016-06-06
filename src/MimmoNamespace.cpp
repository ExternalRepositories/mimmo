#include "MimmoNamespace.hpp"
#include "BaseManipulation.hpp"

namespace mimmo{

namespace pin{

/**< @namespace mimmo::pin
 * pin namespace
 */

/*!It adds a pin between two objects.
 * \param[in] objSend Pointer to BaseManipulation sender object.
 * \param[in] objRec Pointer to BaseManipulation receiver object.
 * \param[in] fget Get function of the sender object (copy return).
 * \param[in] fset Set function of the receiver object (copy argument).
 * \return True if the pin is added.
 */
bool
addPin(BaseManipulation* objSend, BaseManipulation* objRec, PortID portS, PortID portR, bool forced){
	bool done = false;
	if (!objSend->arePortsBuilt()){
		objSend->buildPorts();
		if (!objSend->arePortsBuilt()){
			std::cout << "MiMMO : error " << objSend->m_name << " cannot build ports -> exit! " << std::endl;
			exit(11);
		}
	}
	if (!objRec->arePortsBuilt()){
		objRec->buildPorts();
			if (!objRec->arePortsBuilt()){
			std::cout << "MiMMO : error " << objRec->m_name << " cannot build ports -> exit! " << std::endl;
			exit(11);
		}
	}
	if (!(objSend->getPortsType() == PortsType::BACKWARD) && !(objRec->getPortsType() == PortsType::FORWARD) ){
		if (forced || checkCompatibility(objSend, objRec, portS, portR)){
			objSend->addPinOut(objRec, portS, portR);
			objRec->addPinIn(objSend, portR);
			objSend->addChild(objRec);
			objRec->addParent(objSend);
			done = true;
		}
	}
	return done;
}

/*!It adds a pin between two objects.
 * \param[in] objSend Pointer to BaseManipulation sender object.
 * \param[in] objRec Pointer to BaseManipulation receiver object.
 * \param[in] fget Get function of the sender object (copy return).
 * \param[in] fset Set function of the receiver object (copy argument).
 * \return True if the pin is added.
 */
bool
addPin(BaseManipulation* objSend, BaseManipulation* objRec, PortType portS, PortType portR, bool forced){
	bool done = false;
	if (!objSend->arePortsBuilt()){
		objSend->buildPorts();
		if (!objSend->arePortsBuilt()){
			std::cout << "MiMMO : error " << objSend->m_name << " cannot build ports -> exit! " << std::endl;
			exit(11);
		}
	}
	if (!objRec->arePortsBuilt()){
		objRec->buildPorts();
			if (!objRec->arePortsBuilt()){
			std::cout << "MiMMO : error " << objRec->m_name << " cannot build ports -> exit! " << std::endl;
			exit(11);
		}
	}
	if (!(objSend->getPortsType() == PortsType::BACKWARD) && !(objRec->getPortsType() == PortsType::FORWARD) ){
		if (forced || checkCompatibility(objSend, objRec, portS, portR)){
			objSend->addPinOut(objRec, portS, portR);
			objRec->addPinIn(objSend, portR);
			objSend->addChild(objRec);
			objRec->addParent(objSend);
			done = true;
		}
	}
	return done;
}

/*!It remove all pins between two objects.
 * \param[in] objSend Pointer to BaseManipulation sender object.
 * \param[in] objRec Pointer to BaseManipulation receiver object.
 */
void
removeAllPins(BaseManipulation* objSend, BaseManipulation* objRec){

	std::vector<PortOut*> pinsOut = objSend->getPortsOut();
	for (int i=0; i<objSend->getNPortsOut(); i++){
		if (pinsOut[i] != NULL){
			std::vector<BaseManipulation*> linked = pinsOut[i]->getLink();
			for (int j=0; j<linked.size(); j++){
				if (linked[j] == objRec){
					objSend->removePinOut(i,j);
					objSend->unsetChild(objRec);
				}
			}
		}
	}

	std::vector<PortIn*> pinsIn = objRec->getPortsIn();
	for (int i=0; i<objRec->getNPortsIn(); i++){
		if (pinsIn[i] != NULL){
			if (pinsIn[i]->getLink() == objSend){
				objRec->removePinIn(i);
				objRec->unsetParent(objSend);
			}
		}
	}

}

/*!It remove a pin between two objects. If the pin is found it is removed, otherwise
 * nothing is done.
 * \param[in] objSend Pointer to BaseManipulation sender object.
 * \param[in] objRec Pointer to BaseManipulation receiver object.
 * \param[in] fget Get function of the sender object (copy return).
 * \param[in] fset Set function of the receiver object (copy argument).
 */
void
removePin(BaseManipulation* objSend, BaseManipulation* objRec, PortID portS, PortID portR){

	objSend->removePinOut(objRec, portS);
	objRec->removePinIn(objSend, portR);
	objSend->unsetChild(objRec);
	objRec->unsetParent(objSend);

}


/*!It remove a pin between two objects. If the pin is found it is removed, otherwise
 * nothing is done.
 * \param[in] objSend Pointer to BaseManipulation sender object.
 * \param[in] objRec Pointer to BaseManipulation receiver object.
 * \param[in] fget Get function of the sender object (copy return).
 * \param[in] fset Set function of the receiver object (copy argument).
 */
void
removePin(BaseManipulation* objSend, BaseManipulation* objRec, PortType portS, PortType portR){

	objSend->removePinOut(objRec, portS);
	objRec->removePinIn(objSend, portR);
	objSend->unsetChild(objRec);
	objRec->unsetParent(objSend);

}


bool
checkCompatibility(BaseManipulation* objSend, BaseManipulation* objRec, PortID portS, PortID portR){
	bool check = false;
	PortIn*	pinin = objRec->getPortsIn()[portR];
	std::vector<PortType> comp = pinin->getCompatibility();
	check = (find(comp.begin(), comp.end(), objSend->getPortType(portS)) != comp.end());
	return(check);
}


bool
checkCompatibility(BaseManipulation* objSend, BaseManipulation* objRec, PortType portS, PortType portR){
	bool check = false;
	if (objSend->m_mapPortOut[portS] > 0 && objRec->m_mapPortIn[portR] > 0){
		PortIn*	pinin = objRec->getPortsIn()[objRec->m_mapPortIn[portR]-1];
		std::vector<PortType> comp = pinin->getCompatibility();
		check = (find(comp.begin(), comp.end(), portS ) != comp.end());
	}
	return(check);
}


}//end pin namespace


}//end mimmo namespace

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
	if (!(objSend->getConnectionType() == ConnectionType::BACKWARD) && !(objRec->getConnectionType() == ConnectionType::FORWARD) ){
		if (objSend->m_portOut.count(portS) != 0 && objRec->m_portIn.count(portR) != 0){
			if (forced || checkCompatibility(objSend, objRec, portS, portR)){
				objSend->addPinOut(objRec, portS, portR);
				objRec->addPinIn(objSend, portR);
				objSend->addChild(objRec);
				objRec->addParent(objSend);
				done = true;
			}
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

	std::cout << "remove all pins  - pins out " << std::endl;

	std::map<PortID, PortOut*> pinsOut = objSend->getPortsOut();
	for (std::map<PortID, PortOut*>::iterator i = pinsOut.begin(); i != pinsOut.end(); i++){
		if (i->second != NULL){
			std::vector<BaseManipulation*> linked = i->second->getLink();
			for (int j=0; j<linked.size(); j++){
				if (linked[j] == objRec){
					objSend->removePinOut(i->first,j);
					objSend->unsetChild(objRec);
				}
			}
		}
	}

	std::cout << "remove all pins  - pins in " << std::endl;

	std::map<PortID, PortIn*> pinsIn = objRec->getPortsIn();
	for (std::map<PortID, PortIn*>::iterator i = pinsIn.begin(); i != pinsIn.end(); i++){
		std::vector<BaseManipulation*> linked = i->second->getLink();
		if (i->second != NULL){
			for (int j=0; j<linked.size(); j++){
				if (linked[j] == objSend){
					objRec->removePinIn(i->first,j);
					objRec->unsetParent(objSend);
				}
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

bool
checkCompatibility(BaseManipulation* objSend, BaseManipulation* objRec, PortID portS, PortID portR){
	bool check = false;
	PortIn*	pinin = objRec->getPortsIn()[portR];
	std::vector<PortID> comp = pinin->getCompatibility();
	check = (find(comp.begin(), comp.end(), portS) != comp.end());
	return(check);
}

}//end pin namespace

}//end mimmo namespace

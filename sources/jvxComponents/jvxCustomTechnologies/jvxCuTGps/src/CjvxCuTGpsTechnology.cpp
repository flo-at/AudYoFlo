#include "CjvxCuTGpsTechnology.h"
#include "CjvxCuTGpsDevice.h"

CjvxCuTGpsTechnology::CjvxCuTGpsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericRs232Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CUSTOM_TECHNOLOGY);
	// numPortsPolled = 20;
	// connectionIdenitificationToken = "jvxTRs232*";

}

CjvxCuTGpsTechnology::~CjvxCuTGpsTechnology()
{
}

CjvxGenericConnectionDevice*
CjvxCuTGpsTechnology::allocate_device(const std::string& pName, jvxSize id)
{
	std::string devName = "Gps device at port <" + pName + ">";
	CjvxCuTGpsDevice* nDevice = new CjvxCuTGpsDevice(devName.c_str(), false, _common_set.theDescriptor.c_str(),
		_common_set.theFeatureClass, _common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT,
		JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);
	nDevice->setPortId(id, subcomponents.theConnectionRef, subcomponents.theConnectionObj);

	return nDevice;
}

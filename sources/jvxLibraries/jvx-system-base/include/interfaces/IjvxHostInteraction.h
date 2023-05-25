#ifndef __IJVXHOSTINTERACTION_H__
#define __IJVXHOSTINTERACTION_H__

JVX_INTERFACE IjvxHostInteraction
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHostInteraction(){};
	
	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
	// Module configuration	
	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 

	virtual jvxErrorType JVX_CALLINGCONVENTION set_external_report_target(IjvxReport* hdl) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_external_report_on_config(IjvxReportOnConfig* callbackStruct) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_report_state_switch(IjvxReportStateSwitch* callbackStruct, const char* tag) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_report_state_switch(IjvxReportStateSwitch* callbackStruct) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_component(
		IjvxObject* theObj, IjvxGlobalInstance* theGlob,
		const char* locationDescription, 
		jvxBool allowMultipleInstance = false, 
		jvxInitObject_tp funcInit = NULL, 
		jvxTerminateObject_tp funcTerm = NULL) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_component(IjvxObject* theObj) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_interface(jvxHandle* theHdl, jvxInterfaceType theIFacetype) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_interface(jvxHandle* theHdl, jvxInterfaceType theIFacetype) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_component_load_blacklist(jvxComponentType theTp, jvxBool targetBlackList = true) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_component_load_blacklist(jvxComponentType theTp, jvxBool targetBlackList = true) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_component_load_filter_function(jvxLoadModuleFilterCallback, jvxHandle* priv) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION store_config(const char* token, const char* cfgToken, jvxBool overwrite_old = true) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_config(const char* token, jvxApiString* strReturn) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_config(const char* token) = 0;

};

#endif
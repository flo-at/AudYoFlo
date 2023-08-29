#ifndef __AYF_EMBEDDING_PROXY_H__
#define __AYF_EMBEDDING_PROXY_H__

#include "jvx.h"

enum class ayfHostBindingType
{
	// There is no host at all!!
	AYF_HOST_BINDING_NONE,

	// There is a very minor host binding
	AYF_HOST_BINDING_MIN_HOST,

	// There is a full host binding but embedded somewhere
	AYF_HOST_BINDING_EMBEDDED_HOST,

	// There is a full host binding
	AYF_HOST_BINDING_FULL_HOST,
};

typedef jvxErrorType(*ayf_register_module_host)(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe,
	IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn);
typedef jvxErrorType(*ayf_unregister_module_host)(IjvxObject* regMe);
typedef jvxErrorType(*ayf_load_config_content)(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName);
typedef jvxErrorType(*ayf_release_config_content)(IjvxObject* priObj, jvxConfigData* datOnReturn);
typedef jvxErrorType(*ayf_attach_component_module)(const char* nm, IjvxObject* priObj, IjvxObject* attachMe);
typedef jvxErrorType(*ayf_detach_component_module)(const char* nm, IjvxObject* priObj);
typedef jvxErrorType(*ayf_forward_text_command)(const char* command, IjvxObject* priObj, jvxApiString& astr);

struct ayfHostBindingReferences
{
	ayfHostBindingType bindType = ayfHostBindingType::AYF_HOST_BINDING_NONE;

	// An external module is added to host if allowed
	ayf_register_module_host ayf_register_module_host_call = nullptr;

	// Unregister external module
	ayf_unregister_module_host ayf_unregister_module_host_call = nullptr;

	// Load config content from host - in most cases due to a postponed load of the module/component
	ayf_load_config_content ayf_load_config_content_call = nullptr;

	// Release the config content
	ayf_release_config_content ayf_release_config_content_call = nullptr;

	// Function to attach another component - the secondary component is associated to the first object
	ayf_attach_component_module ayf_attach_component_module_call = nullptr;

	// Detach the previously attached component
	ayf_detach_component_module ayf_detach_component_module_call = nullptr;

	// Forward a text token to the host from where the appropriate action is taken
	ayf_forward_text_command ayf_forward_text_command_call = nullptr;
};


#endif
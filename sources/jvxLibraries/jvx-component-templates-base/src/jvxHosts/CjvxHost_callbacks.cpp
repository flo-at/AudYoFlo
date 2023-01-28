#include "jvxHosts/CjvxHost.h"

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxHost, cb_command_post_set)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (ident.cat == JVX_PROPERTY_CATEGORY_UNSPECIFIC)
	{
		if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.trigger_save_config.globalIdx)
		{
			if (CjvxHost_genpcg::ext_interfaces.properties_active.trigger_save_config.value == c_true)
			{
				if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_save_config.value)
				{
					if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_save_config.value->cb)
					{
						res = CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_save_config.value->cb(CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_save_config.value->cb_priv, NULL);
					}
					else
					{
						res = JVX_ERROR_UNSUPPORTED;
					}
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}
			}
		}
		if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.globalIdx)
		{		
			std::string propDescription = "autostart";
			jvxFloatingPointer_propadmin adm;

			std::cout << "Set the autostart property" << std::endl;
			adm.offset = 0;
			adm.num = 1;
			adm.form = CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.format;
			adm.fld = &CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.value;

			if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value)
			{
				// Callbacks towards host from property system, look at the definition of 
				// jvx_exchange_property_callback_local_pack for details
				res = jvx_exchange_property_callback_local_pack(
					CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb,
					CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb_priv,
					&adm, propDescription, true);
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
		}

		if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.trigger_shutdown.globalIdx)
		{
			if (CjvxHost_genpcg::ext_interfaces.properties_active.trigger_shutdown.value == c_true)
			{
				if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value)
				{
					if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value->cb)
					{
						res = CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value->cb(
							CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value->cb_priv, NULL);
					}
					else
					{
						res = JVX_ERROR_UNSUPPORTED;
					}
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}
			}
		}

		if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.trigger_restart.globalIdx)
		{
			if (CjvxHost_genpcg::ext_interfaces.properties_active.trigger_restart.value == c_true)
			{
				if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value)
				{
					if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value->cb)
					{
						res = CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value->cb(
							CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value->cb_priv, NULL);
					}
					else
					{
						res = JVX_ERROR_UNSUPPORTED;
					}
				}
				else
				{
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxHost, cb_command_pre_get)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.globalIdx)
	{
		std::string propDescription = "autostart";
		jvxFloatingPointer_propadmin adm;

		// std::cout << "Get the autostart property" << std::endl;
		adm.offset = 0;
		adm.num = 1;
		adm.form = CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.format;
		adm.fld = &CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostart.value;

		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value)
		{
			// Callbacks towards host from property system, look at the definition of 
			// jvx_exchange_property_callback_local_pack for details
			res = jvx_exchange_property_callback_local_pack(
				CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb,
				CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb_priv,
				&adm, propDescription, false);
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
		}
	}

	else if (ident.id == CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostop.globalIdx)
	{
		std::string propDescription = "autostop";
		jvxFloatingPointer_propadmin adm;

		// std::cout << "Get the autostart property" << std::endl;
		adm.offset = 0;
		adm.num = 1;
		adm.form = CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostop.format;
		adm.fld = &CjvxHost_genpcg::ext_interfaces.properties_active.application_property_autostop.value;

		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value)
		{
			// Callbacks towards host from property system, look at the definition of 
			// jvx_exchange_property_callback_local_pack for details
			res = jvx_exchange_property_callback_local_pack(
				CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb,
				CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_xchg_property.value->cb_priv,
				&adm, propDescription, false);
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
		}
	}
	return res;
}

jvxErrorType 
CjvxHost::attach_property_submodule(const char* prefix, IjvxProperties* props)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (props)
	{
		jvxSize numSpan = JVX_SIZE_UNSELECTED;
		props->max_property_span(&numSpan);
		if (numSpan == JVX_SIZE_UNSELECTED)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		res = CjvxProperties::_register_sub_module(props, CjvxProperties::_common_set_properties.nextPropId, prefix, prefix);
		if (res == JVX_NO_ERROR)
		{
			CjvxProperties::_common_set_properties.nextPropId += numSpan;
		}
	}
	return res;
}

jvxErrorType 
CjvxHost::detach_property_submodule(IjvxProperties* props)
{
	if (props)
	{
		return CjvxProperties::_unregister_sub_module(props);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}
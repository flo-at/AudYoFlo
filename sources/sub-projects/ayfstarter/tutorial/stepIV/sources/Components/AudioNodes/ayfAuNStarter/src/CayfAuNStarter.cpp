#include "jvx.h"
#include "CayfAuNStarter.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif
	
CayfAuNStarter::CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	AYF_AUDIO_NODE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	

	memset(&processing_lib, 0, sizeof(processing_lib));
}

CayfAuNStarter::~CayfAuNStarter()
{
}

//! ================================ STEP II ============================================
jvxErrorType 
CayfAuNStarter::activate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::activate();
	if (res == JVX_NO_ERROR)
	{
		ayf_starter_init(&processing_lib, 128);

		// Added for step III
		genStarter_node::init_all();
		genStarter_node::allocate_all();
		genStarter_node::register_all(static_cast<CjvxProperties*>(this));

		genStarter_node::associate__properties(static_cast<CjvxProperties*>(this),
			&processing_lib.prmAsync.volume, 1);

		genStarter_node::register_callbacks(static_cast<CjvxProperties*>(this), cb_async_set, this);

#ifdef JVX_EXTERNAL_CALL_ENABLED
		genStarter_node::associate__develop__config(static_cast<CjvxProperties*>(this), 
			&config.skip_involve_ccode, 1,
			&config.matlab_profiling_enabled, 1,
			&processing_lib.prmInit.bsize, 1);
#endif

	}
	return res;
}

jvxErrorType 
CayfAuNStarter::deactivate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		genStarter_node::deassociate__develop__config(static_cast<CjvxProperties*>(this));
#endif
		// Added for step III
		genStarter_node::unregister_callbacks(static_cast<CjvxProperties*>(this));
		genStarter_node::deassociate__properties(static_cast<CjvxProperties*>(this));
		genStarter_node::unregister_all(static_cast<CjvxProperties*>(this));
		genStarter_node::deallocate_all();
			
		res = AYF_AUDIO_NODE_BASE_CLASS::deactivate();
	}
	return res;
}

/** 
 * Prepare processing, that is, allocate all buffers and required structures. We will use the allocated memory
 * during processing until postprocessing is run.
 */
jvxErrorType
CayfAuNStarter::local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// jvxErrorType res = AYF_AUDIO_NODE_NODE_CLASS::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	// if (res != JVX_NO_ERROR) return res; 
	// <- no longer in use as this call is in template class already
	processing_lib.prmInit.bsize = _common_set_icon.theData_in->con_params.buffersize;
	return ayf_starter_prepare(&processing_lib);
}

/**
 * Process frame-by-frame the inout signal into an output signal. We copy the input data to the output buffer.
 * Once, local processing is done, we forward processing towards the next component. Note that processing may be 
 * in-place - in that case we need to make sure that we do not destroy the data since input may be overriden while
 * producing the output data.
 */
jvxErrorType
CayfAuNStarter::local_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	// Involve the c-library for processing
	return ayf_starter_process(&processing_lib, buffers_in, buffers_out, 
		_common_set_icon.theData_in->con_params.number_channels, 
		_common_set_ocon.theData_out.con_params.number_channels,
		_common_set_icon.theData_in->con_params.buffersize);
	
	// return fwd_process_buffers_icon(mt_mask, idx_stage); <- no longer in use as this call is in template class already
}

/**
 * Run the post processing, that is, deallocate all buffers and required structures which we allocated on prepare.
 */
jvxErrorType
CayfAuNStarter::local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// jvxErrorType res = AYF_AUDIO_NODE_NODE_CLASS::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	// if (res != JVX_NO_ERROR) return res; 
	// <-no longer in use as this call is in template class already
	return ayf_starter_postprocess(&processing_lib);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CayfAuNStarter, cb_async_set)
{
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genStarter_node::properties.volume))
	{
		ayf_starter_update(&processing_lib, JVX_DSP_UPDATE_ASYNC, c_true);
	}
	return JVX_NO_ERROR;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

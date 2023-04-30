#include "CjvxAuNConvert.h"
#include "jvx-helpers-cpp.h"
#include "jvx_math/jvx_math_extensions.h"

static jvxSize resamplerQualities[] =
{
	32, 64, 128
};

CjvxAuNConvert::CjvxAuNConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
	// outputArgsFromOutputParams = true;

	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT);
	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT);
	jvx_bitSet(checkRequestUpdate, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT);
	jvx_bitInvert(checkRequestUpdate);
}

CjvxAuNConvert::~CjvxAuNConvert()
{
	
}

jvxErrorType
CjvxAuNConvert::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genConvert_node::init_all();
		genConvert_node::allocate_all();
		genConvert_node::register_all(this);
		genConvert_node::associate__resampling(this, &resampling.cc.oversamplingFactor, 1, &resampling.cc.downsamplingFactor, 1);
		genConvert_node::register_callbacks(this, set_config, this);

		// Initial read of functionality
		fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);

		// genChannelRearrange_node::register_callbacks(this, get_level_pre, get_level_post, set_passthru, this);
		resampling.cc.downsamplingFactor = 1;
		resampling.cc.oversamplingFactor = 1;
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genConvert_node::unregister_all(this);
		genConvert_node::deallocate_all();

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

jvxErrorType 
CjvxAuNConvert::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	
	whatChanged = CjvxNodeBase1io::requires_reconfig(_common_set_icon.theData_in, checkRequestUpdate);
	res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		reChannel.numIn = node_inout._common_set_node_params_a_1io.number_channels;
		reType.formIn = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;

		reChannel.numOut = node_output._common_set_node_params_a_1io.number_channels;
		reType.formOut = (jvxDataFormat)node_output._common_set_node_params_a_1io.format;

		// auto mode = genConvert_node::translate__config__fixed_rate_location_mode_from();
		switch (fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_output._common_set_node_params_a_1io.samplerate * (jvxData)resampling.cc.downsamplingFactor / (jvxData)resampling.cc.oversamplingFactor;
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			genConvert_node::resampling.real_rate.value = (jvxData)node_inout._common_set_node_params_a_1io.samplerate * (jvxData)resampling.cc.oversamplingFactor / (jvxData)resampling.cc.downsamplingFactor;
			break;
		default: 
			break;
		}		
	}
	return res;
}

void
CjvxAuNConvert::from_input_to_output()
{
	// If the input arguments have changed we need to reconfigure
	if (whatChanged)
	{
		CjvxBareNode1ioRearrange::from_input_to_output();

		// Check the relation input/output
		adapt_output_parameters_forward();
	}
}

void
CjvxAuNConvert::compute_buffer_relations(jvxBool fromInput)
{
	if (fromInput)
	{
		resampling.bSizeInMin = node_inout._common_set_node_params_a_1io.buffersize;
		resampling.bSizeInMax = node_inout._common_set_node_params_a_1io.buffersize;
		if (node_inout._common_set_node_params_a_1io.buffersize % resampling.cc.downsamplingFactor)
		{
			resampling.bSizeInMin = node_inout._common_set_node_params_a_1io.buffersize / resampling.cc.downsamplingFactor;
			resampling.bSizeInMin *= resampling.cc.downsamplingFactor;
			resampling.bSizeInMax += resampling.cc.downsamplingFactor;
		}
		resampling.bSizeOutMin = resampling.bSizeInMin * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
		resampling.bSizeOutMax = resampling.bSizeInMax * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	}
	else
	{
		resampling.bSizeOutMin = node_output._common_set_node_params_a_1io.buffersize;
		resampling.bSizeOutMax = node_output._common_set_node_params_a_1io.buffersize;
		if (node_output._common_set_node_params_a_1io.buffersize % resampling.cc.oversamplingFactor)
		{
			resampling.bSizeOutMin = node_output._common_set_node_params_a_1io.buffersize / resampling.cc.oversamplingFactor;
			resampling.bSizeOutMin *= resampling.cc.oversamplingFactor;
			resampling.bSizeOutMax += resampling.cc.oversamplingFactor;
		}
		resampling.bSizeInMin = resampling.bSizeOutMin * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
		resampling.bSizeInMax = resampling.bSizeOutMax * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
	}
}

void
CjvxAuNConvert::adapt_output_parameters_forward()
{
	compute_buffer_relations(true);

	// Check if there is a new value in rate or buffersize - if so, reset num/den
	// node_output._common_set_node_params_a_1io.buffersize = ceil((jvxData)node_inout._common_set_node_params_a_1io.buffersize * (jvxData) resampling.cc.oversamplingFactor / (jvxData)resampling.cc.downsamplingFactor);
	switch (fixedLocationMode)
	{
	case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:

		// If we forward output data with a variable framesize, we need to allocate the maximum buffersize towards the output
		node_output._common_set_node_params_a_1io.buffersize = resampling.bSizeOutMax;
		break;
	}

	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	
	neg_output._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		node_output._common_set_node_params_a_1io.buffersize,
		node_output._common_set_node_params_a_1io.samplerate);
}

void
CjvxAuNConvert::adapt_output_parameters_backward()
{
	// Recompute the input parameters
	update_simple_params_from_ldesc();

	// Only the output samplerate should have changed!
	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;

	neg_output._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		JVX_SIZE_UNSELECTED,
		node_output._common_set_node_params_a_1io.samplerate);
}

jvxErrorType 
CjvxAuNConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// First, try to activate the setting as is:
	jvxLinkDataDescriptor tryThis = *_common_set_icon.theData_in;

	// First, try to just activate the output desire - which typically does not really work
	tryThis = *preferredByOutput;
	res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	switch(res)
	{
		case JVX_NO_ERROR:

			// Reset the resampler to NOT resample
			resampling.cc.downsamplingFactor = 1;
			resampling.cc.oversamplingFactor = 1;
			resampling.cc.gcd = 1;

			// We need to run from the beginning..
			whatChanged = 0;
			jvx_bitInvert(whatChanged);

			update_simple_params_from_ldesc();
			update_output_params_from_input_params();
			adapt_output_parameters_forward();
			test_set_output_parameters();

			// This I need to find an example for
			assert(0);
			break;
		case JVX_ERROR_COMPROMISE:

			// Too much complexity, we better run the test again!
			res = JVX_ERROR_POSTPONE;
			break;
		default:
			// Simple forwarding did not work out, hence, let us find out step by step
			whatChanged = CjvxNodeBase1io::requires_reconfig(preferredByOutput, checkRequestUpdate);
			res = JVX_NO_ERROR;

			switch (fixedLocationMode)
			{
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			
				// ===========================================================================================
				// Try to align number channels
				// ===========================================================================================
				if (jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT))
				{
					tryThis.con_params = _common_set_icon.theData_in->con_params;
					tryThis.con_params.number_channels = preferredByOutput->con_params.number_channels;
					switch (res)
					{
					case JVX_NO_ERROR:
						node_inout._common_set_node_params_a_1io.number_channels = _common_set_icon.theData_in->con_params.number_channels;
						break;
					case JVX_ERROR_COMPROMISE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:

						// Override error message - can deal with it
						res = JVX_NO_ERROR;
					}

					// Modifify the number of output channels as requested
					node_output._common_set_node_params_a_1io.number_channels = preferredByOutput->con_params.number_channels;
					if (res != JVX_NO_ERROR)
					{
						return res;
					}
				}

				// ===========================================================================================
				// Try to align processing
				// ===========================================================================================
				if (jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT))
				{
					tryThis.con_params = _common_set_icon.theData_in->con_params;
					tryThis.con_params.format = preferredByOutput->con_params.format;
					res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

					switch (res)
					{
					case JVX_NO_ERROR:
						node_inout._common_set_node_params_a_1io.format = _common_set_icon.theData_in->con_params.format;
						break;
					case JVX_ERROR_COMPROMISE:
						res = JVX_ERROR_UNSUPPORTED;
						break;
					default:

						// Override error message - can deal with it
						res = JVX_NO_ERROR;
					}
					// Modifify the number of output format as requested
					node_output._common_set_node_params_a_1io.format = preferredByOutput->con_params.format;
					if (res != JVX_NO_ERROR)
					{
						return res;
					}
				}
				
				// ===========================================================================================
				// Next we deal with buffersize/samperates
				// ===========================================================================================
				if (
					(jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT)) ||
					(jvx_bitTest(whatChanged, (int)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT)))
				{

					// Final attempt without any other components involved: convert resampling options
					jvx_fixed_resampler_init_conversion(
						&resampling.cc,
						node_inout._common_set_node_params_a_1io.samplerate,
						preferredByOutput->con_params.rate);

					// We need to deviate from the proposed procedure since this segment returns a "COMPROMISE"
					// The test_set_output
					adapt_output_parameters_forward();
					// test_set_output_parameters(); <- will be called in calling function

					res = JVX_ERROR_COMPROMISE;
					break;
				}
				break;
			case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
				
				// Final attempt without any other components involved: convert resampling options
				tryThis = *_common_set_icon.theData_in;
				jvx_fixed_resampler_init_conversion(&resampling.cc, node_inout._common_set_node_params_a_1io.samplerate,
					preferredByOutput->con_params.rate);

				compute_buffer_relations(false);
				tryThis.con_params.buffersize = resampling.bSizeInMax;

				// Here, we must rely on a buffersize update in the previous component - this is the degree of freedom that we have
				res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryThis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					adapt_output_parameters_backward();					
				}
				break;			
			}		
	}
	return res;
		//CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, preferredByOutput JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

#if 0
jvxErrorType
CjvxAuNConvert::computeResamplerOperation(jvxSize sRateIn, jvxSize sRateOut,
	jvxSize bSizeIn, jvxSize bSizeOut, jvxSize runCnt, 
	jvxLinkDataDescriptor& startRequestSuccessorWiththis
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxData resamplingFacInOut = (jvxData)sRateOut / (jvxData)sRateIn;
	jvxData bSizeInFrac = (jvxData) bSizeOut / resamplingFacInOut;
	jvxSize bSizeInFwd = JVX_DATA2SIZE(bSizeInFrac);
	jvxData bsizeOutFrac = 0;
	jvxInt32 gcdVal = 1;	
	jvxSize bSizeOutFwd = 0;

	/*
	 * Optimize the resampler settings
	 */

	switch (runCnt)
	{
	case 0:
		// Try to force the successor to provide another buffersize
		gcdVal = jvx_gcd(bSizeInFwd, bSizeOut);
		startRequestSuccessorWiththis.con_params.buffersize = bSizeInFwd;
		startRequestSuccessorWiththis.con_params.segmentation.x = startRequestSuccessorWiththis.con_params.buffersize;
		res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &startRequestSuccessorWiththis JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{

		}
		break;
	case 1:

		/*
		resampling.loopNum = 1;
		bsizeOutFrac = (jvxData)bSizeIn * (jvxData)resampling.loopNum * resamplingFacInOut;
		while (bsizeOutFrac - floor(bsizeOutFrac) != 0.0)
		{
			resampling.loopNum++;
			bsizeOutFrac = (jvxData)bSizeIn * (jvxData)resampling.loopNum * resamplingFacInOut;
		}
		bSizeOutFwd = JVX_DATA2SIZE(bsizeOutFrac);		
		*/
		jvx_fixed_resampler_init_conversion(&resampling.cc, sRateIn, sRateOut);

		resampling.granularityIn = resampling.cc.downsamplingFactor;
		resampling.granularityOut = resampling.cc.oversamplingFactor;
		
		// Update the output side
		adapt_output_parameters();
		test_set_output_parameters();

		// Keep input as it is and adapt the output arguments
		//startRequestSuccessorWiththis.con_params.buffersize = bSizeOutFwd;
		//startRequestSuccessorWiththis.con_params.segmentation.x = startRequestSuccessorWiththis.con_params.buffersize;
		res = JVX_ERROR_COMPROMISE;
		break;
	}
	
	if (
		(res == JVX_NO_ERROR) ||
		(res == JVX_ERROR_COMPROMISE))
	{
	}
	return res;
}
#endif

jvxErrorType
CjvxAuNConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	// Configure this module to be zerocopy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;
	runtime.active_resampling = false;
	runtime.active_rechannel = false;
	runtime.requiresRebuffering = false;
	runtime.requiresRebufferHeadroom= false;
	runtime.lFieldRebuffer = 0;

	if ((resampling.cc.oversamplingFactor != 1) || (resampling.cc.downsamplingFactor != 1))
	{
		runtime.active_resampling = true;
	}
	if (reChannel.numIn != reChannel.numOut)
	{
		runtime.active_rechannel = true;
	}
	if (node_inout._common_set_node_params_a_1io.format != node_output._common_set_node_params_a_1io.format)
	{
		runtime.active_retype = true;
	}

	if(runtime.active_resampling || runtime.active_rechannel || runtime.active_retype)
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = false;

		switch (fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			runtime.requiresRebuffering = jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags,
				(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			/*runtime.requiresRebuffering = jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags,
				(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
				*/
			break;
		}
		
		if (runtime.requiresRebuffering)
		{
			// In case of variable input size, add headroom space on input side
			runtime.requiresRebufferHeadroom = true;
		}
		else
		{			
			// If we have a fixed input / output relation, check if we need a variable framesize
			if (node_output._common_set_node_params_a_1io.buffersize % resampling.cc.oversamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
			if (node_inout._common_set_node_params_a_1io.buffersize % resampling.cc.downsamplingFactor)
			{
				runtime.requiresRebufferHeadroom= true;
				runtime.requiresRebuffering = true;
			}
		}
		if (runtime.active_rechannel || runtime.active_retype)
		{
			runtime.requiresRebuffering = true;
		}

		switch (fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			if (runtime.requiresRebufferHeadroom)
			{
				// This module may produce 
				jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
					(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
			}
			break;
		}
	}

	res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (runtime.active_resampling)
		{
			jvxDataFormat formOut = (jvxDataFormat)node_inout._common_set_node_params_a_1io.format;
			runtime.numResampler = node_output._common_set_node_params_a_1io.number_channels;

			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.fldResampler, jvx_fixed_resampler, runtime.numResampler);
			for (i = 0; i < runtime.numResampler; i++)
			{
				jvx_fixed_resampler_initConfig(&runtime.fldResampler[i]);
				runtime.fldResampler[i].prmInit.buffersizeIn = resampling.bSizeInMax;
				runtime.fldResampler[i].prmInit.buffersizeOut = resampling.bSizeOutMax;
				runtime.fldResampler[i].prmInit.format = formOut;
				runtime.fldResampler[i].prmInit.lFilter = resamplerQualities[(jvxSize)resamplerQuality];
				runtime.fldResampler[i].prmInit.optPtrConversion = &resampling.cc; // Provide the resampling exact config
				jvx_fixed_resampler_prepare(&runtime.fldResampler[i]);
			}
		}

		if (runtime.requiresRebuffering)
		{
			if (runtime.requiresRebufferHeadroom)
			{
				switch (fixedLocationMode)
				{
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
					runtime.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize + resampling.cc.downsamplingFactor;
					break;
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
					runtime.lFieldRebuffer = node_output._common_set_node_params_a_1io.buffersize + resampling.cc.oversamplingFactor;
					break;
				}
			}
			else
			{
				runtime.lFieldRebuffer = node_inout._common_set_node_params_a_1io.buffersize;
			}

			runtime.fFieldRebuffer = 0;
			runtime.nCFieldRebuffer = node_output._common_set_node_params_a_1io.number_channels;
			runtime.ptrFieldBuffer = nullptr;
			runtime.lFieldRebufferChannel = runtime.lFieldRebuffer * jvxDataFormat_getsize(node_output._common_set_node_params_a_1io.format);
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.ptrFieldBuffer, jvxHandle*, runtime.nCFieldRebuffer);
			for (i = 0; i < runtime.nCFieldRebuffer; i++)
			{
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.ptrFieldBuffer[i], jvxByte, runtime.lFieldRebufferChannel);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNConvert::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (runtime.active_resampling)
	{
		for (i = 0; i < runtime.numResampler; i++)
		{
			jvx_fixed_resampler_postprocess(&runtime.fldResampler[i]);
		}
		JVX_SAFE_DELETE_FIELD(runtime.fldResampler);
		runtime.numResampler = 0;
	}

	if (runtime.requiresRebuffering)
	{
		for (i = 0; i < runtime.nCFieldRebuffer; i++)
		{
			JVX_SAFE_DELETE_FIELD(runtime.ptrFieldBuffer[i]);
		}
		JVX_SAFE_DELETE_FIELD(runtime.ptrFieldBuffer);

		runtime.fFieldRebuffer = 0;
		runtime.nCFieldRebuffer = 0;
		runtime.ptrFieldBuffer = nullptr;
		runtime.lFieldRebufferChannel = 0;
		runtime.lFieldRebuffer = 0;

	}

	runtime.requiresRebuffering = false;
	runtime.requiresRebufferHeadroom = false;
	
	runtime.active_resampling = false;
	runtime.active_retype = false;
	runtime.active_rechannel = false;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numInputSamples = _common_set_icon.theData_in->con_params.buffersize;
	jvxSize numOutputSamples = 0;

	if(zeroCopyBuffering_rt)
	{ 
		return CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}

	jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	jvxSize idx_stage_local = jvx_process_icon_extract_stage(_common_set_icon.theData_in, idx_stage);

	if (JVX_CHECK_SIZE_SELECTED(_common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x))
	{
		numInputSamples = _common_set_icon.theData_in->con_data.fHeights[idx_stage_local].x;
	}
	
	if (runtime.requiresRebuffering)
	{
		jvxSize numCopyNextFrame = 0;
		jvxSize numResamplerOut = 0;
		jvxSize numResamplerIn = 0;
		jvxSize cnt = 0;
		jvxSize splCnt = 0;

		jvxSize loopNum = JVX_MAX(runtime.nCFieldRebuffer,
			_common_set_icon.theData_in->con_params.number_channels);

		switch (fixedLocationMode)
		{
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:
			// Set the output to 0 first
			for (cnt = 0; cnt < runtime.nCFieldRebuffer; cnt++)
			{
				jvxData* targetIn = (jvxData*)runtime.ptrFieldBuffer[cnt];
				targetIn += runtime.fFieldRebuffer;

				jvxSize splCnt = 0;
				for (; splCnt < numInputSamples; splCnt++)
				{
					targetIn[splCnt] = 0;
				}
			}

			for (cnt = 0; cnt < loopNum; cnt++)
			{
				jvxSize inCnt = cnt % _common_set_icon.theData_in->con_params.number_channels;
				jvxSize outCnt = cnt % runtime.nCFieldRebuffer;

				jvxData* sourceIn = (jvxData*)bufsIn[inCnt];

				jvxData* targetIn = (jvxData*)runtime.ptrFieldBuffer[outCnt];
				targetIn += runtime.fFieldRebuffer;


				for (splCnt = 0; splCnt < numInputSamples; splCnt++)
				{
					jvxData sampleIn = sourceIn[splCnt];
					targetIn[splCnt] += sampleIn;
				}
			}
			runtime.fFieldRebuffer += numInputSamples;

			numResamplerIn = runtime.fFieldRebuffer / resampling.cc.downsamplingFactor * resampling.cc.downsamplingFactor; // Find multiples of input granularity
			numResamplerOut = numResamplerIn * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
			assert(numResamplerOut <= _common_set_ocon.theData_out.con_params.buffersize);

			for (i = 0; i < runtime.numResampler; i++)
			{
				fixed_resample_variable_out varOut;
				varOut.bsizeIn = numResamplerIn;
				varOut.numOut = numResamplerOut;

				// Important: use the "new" input buffer
				jvx_fixed_resampler_process(&runtime.fldResampler[i], runtime.ptrFieldBuffer[i], bufsOut[i], &varOut); // Full size resampling
			}
			idx_stage_local = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;

			_common_set_ocon.theData_out.con_data.fHeights[idx_stage_local].x = numResamplerOut;

			numCopyNextFrame = runtime.fFieldRebuffer - numResamplerIn;
			for (cnt = 0; cnt < runtime.nCFieldRebuffer; cnt++)
			{
				jvxData* cpyTo = (jvxData*)runtime.ptrFieldBuffer[cnt];
				jvxData* cpyFrom = cpyTo + numResamplerIn;

				for (splCnt = 0; splCnt < numCopyNextFrame; splCnt++)
				{
					*cpyTo = *cpyFrom;
					cpyTo++;
					cpyFrom++;
				}
			}
			runtime.fFieldRebuffer -= numResamplerIn;
			break;
		case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:
			assert(0);
			break;
		}
	}
	else
	{
		// Simple case: just resample straight away
		for (i = 0; i < runtime.numResampler; i++)
		{
			jvx_fixed_resampler_process(&runtime.fldResampler[i], bufsIn[i], bufsOut[i], nullptr); // Full size resampling
		}
	}
	return fwd_process_buffers_icon(mt_mask, idx_stage);
	
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNConvert, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.fixed_rate_location_mode))
	{
		fixedLocationMode = genConvert_node::translate__config__fixed_rate_location_mode_from(0);
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genConvert_node::config.resampler_quality))
	{
		resamplerQuality = genConvert_node::translate__config__resampler_quality_from(0);
	}

	return JVX_NO_ERROR;
}

/*


// ===================================================================
jvxErrorType
CjvxAuNChannelRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
}

// ===================================================================
// ===========================================================================

jvxErrorType
CjvxAuNChannelRearrange::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{

	if (!_common_set_ocon.setup_for_termination)
	{
		jvxData** fieldInput = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		jvxData** fieldOutput = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		jvxBool passThrough = (genChannelRearrange_node::passthrough.active.value == c_true);
		// Talkthrough
		jvxSize ii;
		jvxSize inChans = _common_set_icon.theData_in->con_params.number_channels;
		jvxSize outChans = _common_set_ocon.theData_out.con_params.number_channels;
		if (passThrough)
		{
			inChans = JVX_MAX((int)inChans - 1, 0);
			outChans = JVX_MAX((int)outChans - 1, 0);
		}
		jvxSize maxChans = JVX_MAX(inChans, outChans);
		jvxSize minChans = JVX_MIN(inChans, outChans);

		// This default function does not tolerate a lot of unexpected settings
		assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
		assert(_common_set_icon.theData_in->con_params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);

		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % inChans;
				jvxSize idxOut = ii % outChans;

				assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
				jvx_convertSamples_memcpy(
					_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][idxIn],
							_common_set_ocon.theData_out.con_data.buffers[
								*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][idxOut],
									jvxDataFormat_size[_common_set_icon.theData_in->con_params.format],
									_common_set_icon.theData_in->con_params.buffersize);
			}
		}
		if (passThrough)
		{
			if ((inChans > 0) && (outChans > 0))
			{
				memcpy(fieldOutput[outChans], fieldInput[inChans], jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format) * _common_set_icon.theData_in->con_params.buffersize);
			}
		}
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNChannelRearrange::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// Forward call to base class
		return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNChannelRearrange::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genChannelRearrange_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
		}
	}
	return res;
}

jvxErrorType
CjvxAuNChannelRearrange::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);;
	if (res == JVX_NO_ERROR)
	{
		genChannelRearrange_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_pre)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_post)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, set_passthru)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genChannelRearrange_node::passthrough.active))
	{
		// Actually, we need not do anything here, this macro is considered only in the procesing function
	}
	return JVX_NO_ERROR;
}
*/
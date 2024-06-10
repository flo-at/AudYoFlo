#include "CjvxSpNMixChainEnterLeave.h"

jvxErrorType
CjvxSpNMixChainEnterLeave::report_selected_connector(CjvxSingleInputConnector* iconn)
{
	oneConnectorPlusName<CjvxSingleInputConnector> addme;
	addme.ioconn = iconn;
	addme.nmUnique = _common_set_min.theDescription + "-" + 
		CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName + "_" + jvx_size2String(uIdGen++);

	CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors[iconn] = addme;
	if (_common_set_icon.theData_in)
	{
		// All parameters as desired
		jvxLinkDataDescriptor_con_params params = _common_set_icon.theData_in->con_params;

		// Number of channels differs
		params.number_channels = genMixChain::config.number_channels_side.value;
		params.data_flow = JVX_DATAFLOW_PUSH_ON_PULL;

		// Pass it to the connector but do not request a test of the chain - it is in build status anyway
		iconn->updateFixedProcessingArgs(params);
	}
	return JVX_NO_ERROR;
}

void
CjvxSpNMixChainEnterLeave::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId)
{
	auto elm = CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.find(iconn);
	if (elm != CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.end())
	{
		if (_common_set.theUniqueId)
		{
			_common_set.theUniqueId->obtain_unique_id(uId, elm->second.nmUnique.c_str());
		}
	}
}

void
CjvxSpNMixChainEnterLeave::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId)
{
	auto elm = CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.find(iconn);
	if (elm != CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.end())
	{
		if (_common_set.theUniqueId)
		{
			_common_set.theUniqueId->release_unique_id(uId);
		}
	}
}

jvxErrorType
CjvxSpNMixChainEnterLeave::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxComponentIdentification cpId;
	if (iconn->_common_set_icon.theData_in)
	{
		if (iconn->_common_set_icon.theData_in->con_link.connect_from)
		{
			if (iconn->_common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_REAL_MASTER, 
				&cpId  JVX_CONNECTION_FEEDBACK_CALL_A(fdb)) == JVX_NO_ERROR)
			{
				res = check_preset_channels(iconn->chanSetting, cpId);
			}
		}
	}
	return res;
}

// This function is called for each single connection. We may search for the entry in the map
void 
CjvxSpNMixChainEnterLeave::report_process_buffers(CjvxSingleInputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params)
{
	jvxSize i;
	
	assert(JVX_CHECK_SIZE_SELECTED(idxBufferProcess));
	assert(params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);
	// assert(params.number_channels == szExtraBuffersChannels);
	assert(params.format == _common_set_ocon.theData_out.con_params.format);
	
	jvxSize numChannels = JVX_MIN(params.number_channels, szExtraBuffersChannels);
	
	// The pointers are a shortcut!!
	jvxData** bufsOut = (jvxData**)bufsSideChannel[idxBufferProcess];
	jvxData** bufsFrom = (jvxData**)bufferPtrs;

	for (i = 0; i < numChannels; i++)
	{
		jvx_mixSamples_flp(bufsFrom[i], bufsOut[i], params.buffersize);
		//memcpy(bufsOut[i], bufferPtrs[i], jvxDataFormat_getsize(params.format) * params.buffersize);
	}
}
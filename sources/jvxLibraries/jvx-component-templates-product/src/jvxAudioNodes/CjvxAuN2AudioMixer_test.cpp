#include "jvx.h"
#include "jvxAudioNodes/CjvxAuN2AudioMixer.h"

#define JVX_STRING_COMPOSE_OUTPUT_CHANNEL(nmMaster, cnt) nmMaster + " -- Channel#" + jvx_size2String(cnt);

jvxErrorType
CjvxAuN2AudioMixer::test_connect_icon_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxNVTasks::test_connect_icon_ntask(
		theData_in,
		theData_out,
		idCtxt,
		refto
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		update_channels_on_test(theData_in, theData_out);
	}
	return res;
}

jvxErrorType
CjvxAuN2AudioMixer::test_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{

	jvxErrorType res = CjvxNVTasks::test_connect_icon_vtask(
		ctxtId,
		ctxtSubId
		JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
	if (res == JVX_NO_ERROR)
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

		res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
		if (res == JVX_NO_ERROR)
		{
			jvxLinkDataDescriptor* datIn = icon->con->_common_set_icon_nvtask.theData_in;

			// Always focus on FIRST ocon
			jvxLinkDataDescriptor* datOut = nullptr;
			if (ocon.size())
			{
				auto oconElm = ocon.begin();
				if ((*oconElm)->con)
				{
					datOut = &(*oconElm)->con->_common_set_ocon_nvtask.theData_out;
				}
			}
			res = update_channels_on_test(datIn, datOut);
		}
	}
	return res;
}

void 
CjvxAuN2AudioMixer::test_set_output_parameters(jvxSize ctxtId, jvxSize ctxtSubId, jvxLinkDataDescriptor* theDataOut)
{
	auto elm = mixBuffers.begin();
	theDataOut->con_params.number_channels = elm->second.numChannels_fixed;
}

jvxErrorType
CjvxAuN2AudioMixer::update_channels_on_test(const jvxLinkDataDescriptor* datIn, const jvxLinkDataDescriptor* datOut)
{
	jvxSize i = 0;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if (datIn)
	{
		if (datIn->con_link.connect_from)
		{
			jvxSize uId = datIn->con_link.uIdConn;
			auto itElm = registeredChannelListInput.find(uId);

			jvxApiString astr;
			std::string nmMaster = "Link#" + jvx_size2String(uId);

			IjvxConnectionMasterFactory* masFac = nullptr;
			datIn->con_link.master->parent_master_factory(&masFac);
			if (masFac)
			{
				IjvxObject* obj = nullptr;
				masFac->request_reference_object(&obj);
				if (obj)
				{
					obj->description(&astr);
					nmMaster = astr.std_str();
					masFac->return_reference_object(obj);
				}
			}

			// If a device has no input channels it will not be in this list
			if (itElm != registeredChannelListInput.end())
			{
				itElm->second.masName = nmMaster;
				if (itElm->second.channels.size() != datIn->con_params.number_channels)
				{
					updateChannelToStorage(inputChannelsInStorage, nmMaster, itElm->second.channels, true);

					// Create new channel list for this device
					for (auto& elmChan : itElm->second.channels)
					{
						/*
						if (extender) extender->deallocateAttachChannelSpecific(elmChan.attSpecificPtr, true);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
						elmChan.attSpecificPtr = nullptr;
#endif
						*/
					}
					JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
					itElm->second.channels.clear();
					mixer_input.associationValid = false;
					JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

					i = 0;

					// Check current input channel selection
					jvx::propertyCallCompactRefList cptTrans;

					jvxSelectionList selLst;
					jvx::propertyCallCompactElement<jPRG, jPAD, jPD> cptElmChans(
						jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxSelectionList>(selLst), 
						jPAD ("/system/sel_input_channels"), jPD());

					cptTrans.propReqs.push_back(cptElmChans.toRefElement());

					jvxApiString srcName;										
					jvx::propertyCallCompactElement< jPRG, jPAD, jPD> cptElmSrc(
						jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiString>(srcName), jPAD("/system/src_name"), jPD());
					cptTrans.propReqs.push_back(cptElmSrc.toRefElement());
					jvxBool channelsFound = false;

					jvxErrorType resL = datIn->con_link.connect_from->transfer_backward_ocon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES,
						reinterpret_cast<jvxHandle*>(&cptTrans) JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					if (resL == JVX_NO_ERROR)
					{
						if ((cptElmSrc.resCall == JVX_NO_ERROR) && (cptElmSrc.callMan.access_protocol == jvxAccessProtocol::JVX_ACCESS_PROTOCOL_OK))
						{
							nmMaster = srcName.std_str();
						}
						if ((cptElmSrc.resCall == JVX_NO_ERROR) && (cptElmChans.callMan.access_protocol == jvxAccessProtocol::JVX_ACCESS_PROTOCOL_OK))
						{
							for (i = 0; i < selLst.strList.ll(); i++)
							{
								if (jvx_bitTest(selLst.bitFieldSelected(), i))
								{
									oneEntryChannel newEntry;
									if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(true);
									newEntry.name = nmMaster + "--" + selLst.strList.std_str_at(i);

									// Use itElm->second.masName here: when first allocating the object we do not yet know the "friendly" name
									updateChannelFromStorage(inputChannelsInStorage, newEntry, itElm->second.masName, true);
									JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
									itElm->second.channels.push_back(newEntry);
									mixer_input.associationValid = false;
									JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
									newEntry.attSpecificPtr = nullptr;
#endif
									cnt++;
								}
							}

							if (datIn->con_params.number_channels != itElm->second.channels.size())
							{
								res = JVX_ERROR_INVALID_FORMAT;
								goto exit_fail;
							}
							channelsFound = true;
						}
					}
					if(!channelsFound)
					{
						for (; cnt < datIn->con_params.number_channels; cnt++)
						{
							oneEntryChannel newEntry;
							if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(true);
							newEntry.name = nmMaster + " -- Channel#" + jvx_size2String(cnt);// +"/" + jvx_size2String(uId);
							// Use itElm->second.masName here: when first allocating the object we do not yet know the "friendly" name
							updateChannelFromStorage(inputChannelsInStorage, newEntry, itElm->second.masName, true);
							JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
							itElm->second.channels.push_back(newEntry);
							mixer_input.associationValid = false;
							JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
							newEntry.attSpecificPtr = nullptr;
#endif
						}
					}
					new_setup_to_properties(true, false);


				}
			}
		} // if (datIn->con_link.connect_from)
	}

	if (datOut)
	{
		if (datOut->con_link.connect_to)
		{
			jvxSize uId = datOut->con_link.uIdConn;
			auto itElm = registeredChannelListOutput.find(uId);

			jvxApiString astr;
			std::string nmMaster = "Link#" + jvx_size2String(uId);

			IjvxConnectionMasterFactory* masFac = nullptr;
			datOut->con_link.master->parent_master_factory(&masFac);
			if (masFac)
			{
				IjvxObject* obj = nullptr;
				masFac->request_reference_object(&obj);
				if (obj)
				{
					obj->description(&astr);
					nmMaster = astr.std_str();
					masFac->return_reference_object(obj);
				}
			}

			// If a device has no input channels it will not be in this list
			if (itElm != registeredChannelListOutput.end())
			{
				itElm->second.masName = nmMaster;
				if (itElm->second.channels.size() != datOut->con_params.number_channels)
				{
					std::list<oneEntryChannel> channels_old = itElm->second.channels;
					updateChannelToStorage(outputChannelsInStorage, nmMaster, channels_old, false);

					// Create new channel list for this device
					for (auto& elmChan : itElm->second.channels)
					{
						/*
						if (extender) extender->deallocateAttachChannelSpecific(elmChan.attSpecificPtr, false);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
						elmChan.attSpecificPtr = nullptr;
#endif
						*/
					}
					JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
					itElm->second.channels.clear();
					mixer_output.associationValid = false;
					JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

					i = 0;

					// Check current input channel selection
					jvx::propertyCallCompactRefList cptTrans;

					jvxSelectionList selLst;
					jvx::propertyCallCompactElement< jPRG, jPAD, jPD> cptElmChans( 
						jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxSelectionList>(selLst), 
						jPAD("/system/sel_output_channels"), jPD());

					cptTrans.propReqs.push_back(cptElmChans.toRefElement());

					jvxApiString srcName;
					jvx::propertyCallCompactElement< jPRG, jPAD, jPD> cptElmSrc( 
						jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiString>(srcName), 
						jPAD("/system/snk_name"), jPD());
					cptTrans.propReqs.push_back(cptElmSrc.toRefElement());

					jvxErrorType resL = datOut->con_link.connect_to->transfer_forward_icon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES,
						reinterpret_cast<jvxHandle*>(&cptTrans) JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					if ((cptElmSrc.resCall == JVX_NO_ERROR) && (cptElmSrc.callMan.access_protocol == jvxAccessProtocol::JVX_ACCESS_PROTOCOL_OK))
					{
						nmMaster = srcName.std_str();
					}

					if ((cptElmSrc.resCall == JVX_NO_ERROR) && (cptElmChans.callMan.access_protocol == jvxAccessProtocol::JVX_ACCESS_PROTOCOL_OK))
					{
						for (i = 0; i < selLst.strList.ll(); i++)
						{
							if (jvx_bitTest(selLst.bitFieldSelected(), i))
							{
								oneEntryChannel newEntry;
								if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(false);
								newEntry.name = nmMaster + "--" + selLst.strList.std_str_at(i);
								// Use itElm->second.masName here: when first allocating the object we do not yet know the "friendly" name
								updateChannelFromStorage(outputChannelsInStorage, newEntry, itElm->second.masName, false);
								JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
								itElm->second.channels.push_back(newEntry);
								mixer_output.associationValid = false;
								JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
								newEntry.attSpecificPtr = nullptr;
#endif
								cnt++;
							}
						}
					}

					// Modified this assertion code to find useful assumptions if the device does not deliver the required names, 02. Aug. 2023 - HK
					// assert(datOut->con_params.number_channels == itElm->second.channels.size());
					if (datOut->con_params.number_channels < itElm->second.channels.size())
					{
						// If there is a difference, we might have a channel constraint acting before the actual device.
						// In that case, we are about to change the number of channels and should trigger another test!

						itElm->second.channels.resize(datOut->con_params.number_channels);
						// reRunTestThisChain = true;
					}
					if (datOut->con_params.number_channels > itElm->second.channels.size())
					{
						cnt = itElm->second.channels.size();
						for (; cnt < datOut->con_params.number_channels; cnt++)
						{
							oneEntryChannel newEntry;
							if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(false);
							newEntry.name = JVX_STRING_COMPOSE_OUTPUT_CHANNEL(nmMaster, cnt);
							// Use itElm->second.masName here: when first allocating the object we do not yet know the "friendly" name
							updateChannelFromStorage(outputChannelsInStorage, newEntry, itElm->second.masName, false);
							JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
							itElm->second.channels.push_back(newEntry);
							mixer_output.associationValid = false;
							JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
							newEntry.attSpecificPtr = nullptr;
#endif
						}						
						cnt++;
					}
					new_setup_to_properties(false, true);
				}
			}
		}
	}

exit_fail:
	return res;
}

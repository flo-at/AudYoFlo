
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

enum class jvxOperationMode
{
	JVX_FORWARDBUFFER_BUFFER_INPUT,
	JVX_FORWARDBUFFER_BUFFER_OUTPUT
};

#include "pcg_exports_node.h"

extern "C"
{
#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"
}

class CjvxAudioStackBuffer
{
protected:
	jvxAudioStackDispenserCont myAudioDispenser;

	JVX_MUTEX_HANDLE safeAccess_lock; // <- lock used in audio sample disposer
	jvxHandle** work_buffers_from_hw = nullptr;
	jvxHandle** work_buffers_to_hw = nullptr;

	jvxHandle* fHeightEstimator = nullptr;
	jvxSize numberEventsConsidered_perMinMaxSection = 0;
	jvxSize num_MinMaxSections = 0;
	jvxData recSmoothFactor = 0.;
	jvxSize numOperations = 0;

public:
	// ==================================================================================================
	CjvxAudioStackBuffer();
	~CjvxAudioStackBuffer();

	static jvxDspBaseErrorType lock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType lock_sync_threads_ic();
	static jvxDspBaseErrorType unlock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType unlock_sync_threads_ic();

	void start_audiostack(
		jvxSize num_samples_buffer,
		jvxSize number_input_channels,
		jvxDataFormat format,
		jvxBool withFHeightEstimator,
		jvxBool withStartThreshold = true);
	void stop_audiostack();
};

// =====================================================================

class CjvxAuNForwardBuffer: public CjvxBareNode1ioRearrange, 
	public IjvxThreads_report, public CjvxAudioStackBuffer,
	public genForwardBuffer_node
{
private:
	
	

protected:

	jvxBool bypass_buffer_mode = true;
	/*
	jvxOperationMode mode =
		jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT;
		*/
	class proc_params
	{
	public:
		jvxSize buffersize = JVX_SIZE_UNSELECTED;
		jvxSize number_channels = JVX_SIZE_UNSELECTED;
		jvxSize samplerate = JVX_SIZE_UNSELECTED;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
	};

	refComp<IjvxThreads> refThreads;

	// The forward buffer can be configured for input as well as for output functionality
	jvxOperationMode buffermode = jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT;

	// The following struct is used for a simple input channel mapping. The number of channels in the buffer always
	// follows the output connection. The input can then be routed as desired.
	struct
	{
		// Field to expose the selection towards UI. Buffer and number of entries.
		jvxBitField* selChannels = nullptr;
		jvxSize numChannels = 0;

		//! Container to hold the buffers while operating
		jvxData** bufReroute = nullptr;

		//! Zero input buffer for muting of a special channel
		jvxData* zbuf = nullptr;
	} reRouting;

	jvxBool runInitInThread = false;

	jvxDataflow dataFlowOperation_output = JVX_DATAFLOW_DONT_CARE;
	jvxDataflow dataFlowOperation_input = JVX_DATAFLOW_DONT_CARE;

public:

	JVX_CALLINGCONVENTION CjvxAuNForwardBuffer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNForwardBuffer();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===================================================================================
	
	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual void test_set_output_parameters() override;
	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
			jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
			jvxSize idx_stage,
			jvxBool operate_first_call,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock cb,
			jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_processing_monitor);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_bypass_buffer);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_buffer_mode);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ==================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION startup(jvxInt64 timestamp_us) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stopped(jvxInt64 timestamp_us) override;

	jvxErrorType push_on_pull_one_buffer(jvxHandle* data, jvxBool runStartStop, jvxBool awakeThreadInputSide);

	void read_samples_to_buffer();
	void write_samples_to_output(jvxBool runStartStopBuffer = true);
	jvxErrorType write_samples_to_output_one_buf(jvxBool runStartStopBuffer);
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	virtual void from_input_to_output() override;
	void update_output_params();
};

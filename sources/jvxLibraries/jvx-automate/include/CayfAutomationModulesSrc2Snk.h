#ifndef __CAYFAUTOMATIONMODULESSEC2SNK_H__
#define __CAYFAUTOMATIONMODULESSEC2SNK_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	/*
	* This class allows to costruct a chain composed of the following parts:
	* SRC ->  <MOD1> -> <MOD2> -> .. -> <MODN> -> SNK
	* The chain is created once a SRC has become visible/was born
	* The SRC provides at the same time the master.
	* The module requires info about:
	* 1) Name of the master of the SRC (in the master factory)
	* 2) Name of the output connector of the SRC (in the connector factory)
	* 3) Name of the input connector in the SNK (in the connector factory)
	 */
	class ayfConnectConfigSrc2Snk
	{
	public:
		std::list<ayfConnectConfigCpEntry> connectedNodes;

		// This defines the Src side: master and output connector
		std::string nmMaster;
		std::string oconNmSource;

		// This defines the sink side: input connector
		std::string iconNmSink;

		// Chain prefix name
		std::string chainNamePrefix = "myChainCfgName";
		
		// More general variable:
		// Typically, tpInvolved is going to be 
		// - the SNK in case of the use-case <Src2Snk> and
		// - the SRC in case of the use-case <Src2SnkPreChain>
		// -> deriveArguments
		jvxComponentIdentification tpAssign = JVX_COMPONENT_UNKNOWN;

		CayfAutomationModules::ayfConnectConfigConMiscArgs miscArgs;

		jvxSize oconIdTrigger = JVX_SIZE_UNSELECTED;
		jvxSize iconIdTrigger = JVX_SIZE_UNSELECTED;
		
		ayfConnectConfigSrc2Snk() {};
		ayfConnectConfigSrc2Snk(const std::string& chainName,
			const std::list<ayfConnectConfigCpEntry>& connectedNodesArg,
			const std::string& nmMasterArg = "default",
			const std::string& oconNmSourceArg = "default",
			const std::string& iconNmSinkArg = "",
			jvxComponentIdentification tpInvolvedArg = JVX_COMPONENT_UNKNOWN,
			const CayfAutomationModules::ayfConnectConfigConMiscArgs& miscArgsArg = CayfAutomationModules::ayfConnectConfigConMiscArgs(),
			jvxSize oconTriggerIdArg = JVX_SIZE_UNSELECTED,
			jvxSize iconTriggerIdArg = JVX_SIZE_UNSELECTED) :
			chainNamePrefix(chainName), connectedNodes(connectedNodesArg),
			nmMaster(nmMasterArg), oconNmSource(oconNmSourceArg), iconNmSink(iconNmSinkArg),
			miscArgs(miscArgsArg), tpAssign(tpInvolvedArg),
			oconIdTrigger(oconTriggerIdArg), iconIdTrigger(iconTriggerIdArg)
		{};
	};

	class ayfConnectConfigCpEntryRuntime : public ayfConnectConfigCpEntry
	{
	public:

		// Add a component identification to identify allocated component
		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		ayfConnectConfigCpEntryRuntime(const ayfConnectConfigCpEntry& cp) :
			ayfConnectConfigCpEntry(cp){};
	};		

	class IayfEstablishedProcessesSrc2Snk 	
	{
	public:
		std::list<ayfConnectConfigCpEntryRuntime> lstEntries;		
	};

	class CayfEstablishedProcessesSrc2Snk: public IayfEstablishedProcessesCommon, public IayfEstablishedProcessesSrc2Snk, public CayfEstablishedProcessesMixin
	{
		virtual jvxHandle* specificType(ayfEstablishedProcessType tp) override
		{
			switch (tp)
			{
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2Snk*>(this));
				break;
			}
			return nullptr;
		}

		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};
		

	class CayfAutomationModulesSrc2Snk: public CayfAutomationModulesCommon, public CayfAutomationModuleHandlerIf
	{
	
	public:

		class ayfConnectDerivedSrc2Snk
		{
		public:
			jvxComponentIdentification tpSrc = JVX_COMPONENT_UNKNOWN;
			jvxComponentIdentification tpSink = JVX_COMPONENT_UNKNOWN;
			jvxComponentIdentification tpMaster = JVX_COMPONENT_UNKNOWN;
		};

	protected:

		ayfAutoConnect_callbacks* cbPtr = nullptr;
		ayfConnectConfigSrc2Snk config;
		ayfConnectDerivedSrc2Snk derived;

	public:

		CayfAutomationModulesSrc2Snk()
		{
		};

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigSrc2Snk& cfg,
			CjvxObjectLog* ptrLog = nullptr);
		jvxErrorType deactivate();

		/* Build the chain as follows:
		 * derived.tpMaster
		 *     derived.tpSrc -> create_bridges -> derived.tpSink
		 * 
		 */
		virtual void try_connect(
			jvxComponentIdentification tp_ident,
			jvxBool& established);

		jvxErrorType associate_process(jvxSize uIdProcess,
			const std::string& nmChain) override;
		jvxErrorType deassociate_process(jvxSize uIdProcess) override;
		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated) override;
		jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated) override;
		jvxErrorType adapt_all_submodules(jvxSize uIdProc, const std::string& modName, const std::string& description,
			const jvxComponentIdentification& tp_activated, jvxReportCommandRequest req) override;
		void postponed_try_connect();

		virtual void create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl, 
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			IayfEstablishedProcessesCommon* sglElmPtr,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,			
			jvxSize& bridgeId,
			jvxSize segId,
			jvxSize oconIdTrigger,
			jvxSize iconIdTrigger);

		// Functions to be called in the connection rule creation to adapt to use case

		// Depending on the use-case, derive the sinks and sources
		virtual void deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated);

		virtual IayfEstablishedProcessesCommon* allocate_chain_realization() override;
		virtual void deallocate_chain_realization(IayfEstablishedProcessesCommon* deallocMe) override;
		virtual jvxErrorType pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain) override;
		virtual jvxErrorType post_run_chain_prepare(IayfEstablishedProcessesCommon* realizeChain) override;

		virtual jvxErrorType on_connection_not_established(jvxComponentIdentification tp_activated, IayfEstablishedProcessesCommon* realizeChainPtr);

		virtual void print(std::ostream& out);
		void print(std::ostream& out, jvxSize& segId);
		void printList(std::ostream& out, jvxSize& segId, std::list<ayfConnectConfigCpEntry>& lst);
	};
}

#endif

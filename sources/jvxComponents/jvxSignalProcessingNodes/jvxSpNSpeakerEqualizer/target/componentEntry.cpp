#include "CjvxSpNSpeakerEqualizer.h"

#define COMPONENT_DESCRIPTION "JVX Equalize Speaker"
#define COMPONENT_TYPE CjvxSpNSpeakerEqualizer
#define _JVX_ALLOW_MULTIPLE_INSTANCES
#define COMPONENT_TYPE_SPECIALIZATION JVX_COMPONENT_SIGNAL_PROCESSING_NODE
#define COMPONENT_DESCR_SPECIALIZATION "signalprocessing_node/jvx-equalize-speaker"

#define JVX_NODE_TYPE_SPECIFIER_TYPE 
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "signal_processing_node"

#include "templates/targets/factory_IjvxObject.cpp"

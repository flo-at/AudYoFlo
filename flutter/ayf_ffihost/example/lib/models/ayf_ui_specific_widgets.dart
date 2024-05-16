import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'ayf_ui_specific.dart';

import 'package:ayf_pack/main-widgets/ayf_audio_settings_widget.dart';

import 'package:flutter/foundation.dart' show kIsWeb;

// ============================================================================
// Callback to allocate an audio device sliver in the list of active/inactive
// devices lists
Widget? allocateAudioDeviceSliver(AudYoFloOneSelectionOptionWithId option,
        bool fromActiveList, AudYoFloSingleSelectionComponent? reportW) =>
    AudYoFloOneDeviceSliver(option, fromActiveList, reportW);

// ============================================================================
// Callback to allocate a headtracker device sliver in the list of active/
// inactibe devices list
Widget? allocateHeadtrackerDeviceSliver(AudYoFloOneSelectionOptionWithId option,
        bool fromActiveList, AudYoFloSingleSelectionComponent? reportW) =>
    AudYoFloOneHeadtrackerDeviceSliver(option, fromActiveList, reportW);

BottomAppBar? allocatorBottomAppBar() {
  if (kIsWeb) {
    return AudYoFloPlayConnectBottomAppBar();
  } else {
    return AudYoFloBottomAppBar();
  }
}

class AudYoFloPlayConnectBottomAppBar extends BottomAppBar {
  AudYoFloPlayConnectBottomAppBar()
      : super(
            child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          children: [AudYoFloPlaypannel(), AudYoFloPlaypannel()],
        ));
}

// ============================================================================
// ============================================================================
// This class extends the common UiModel part and adds application specific
// configuration entries.
// ============================================================================
class AudYoFloUiModelSpecificWithWidget extends AudYoFloUiModelSpecific {
  // ==========================================================================
  // Define the core widget composition here: The function configures all application
  // specific widgets here. Then, if all specific widgets were added, the base class
  // call may be used to attach the application unspecific widgets - which are typically
  // only the debug output and the component property viewer widget.
  // ==========================================================================

  AudYoFloUiModelSpecificWithWidget() : super() {
    allocateBottomAppBar = allocatorBottomAppBar;
  }

  @override
  void createTabsAndViews(Map<String, String> entriesCfgWidgets) {
    // Tabs are added to a list along multiple layers of the class hierarchie
    // Since we dont know if we are the first here, we can not clear the list
    // when entering the call. It is the task of the caller to run clear before

    // ======================================================================
    // Add the primary audio device widget
    // ======================================================================
    String imageNameAudioIoPri =
        'packages/ayf_pack/images/ayf_icons/ayf-audio-settings.png';
    String textAudioIoPri = 'ASIO I/O';
    JvxComponentIdentification cpTpPriTech = widgetConfigCpId(
        entriesCfgWidgets,
        JvxComponentIdentification(
            cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
            slotid: 0,
            slotsubid: 0),
        'priaudiot');

    JvxComponentIdentification cpTpPriDev = widgetConfigCpId(
        entriesCfgWidgets,
        JvxComponentIdentification(
            cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
            slotid: cpTpPriTech.slotid,
            slotsubid: 0),
        'priaudiod');

    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: '${cpTpPriTech.txt}--${cpTpPriDev.txt}',
            child: Column(children: [
              Image.asset(imageNameAudioIoPri,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textAudioIoPri)
            ]))));
    theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
        cpTpPriTech,
        'Audio Technologies',
        cpTpPriDev,
        'Audio Devices',
        'soundIo',
        (JvxComponentIdentification selCpId) =>
            AudYoFloAudioDeviceInputOutputChannelWidget(selCpId),
        allocateAudioDeviceSliver));

    // Here, the base class function is called. This call allows the base class
    // to attach the "standard widgets" debug and components. If you do not call these functions
    // the debug widgets will not be viisible.
    super.createTabsAndViews(entriesCfgWidgets);
  }
}

// ======================================================================
// \title  Stm32f4DeploymentTopologyDefs.hpp
// \brief required header file containing the required definitions for the topology autocoder
//
// ======================================================================
#ifndef STM32F4DEPLOYMENT_STM32F4DEPLOYMENTTOPOLOGYDEFS_HPP
#define STM32F4DEPLOYMENT_STM32F4DEPLOYMENTTOPOLOGYDEFS_HPP

#include <cstring>

// Subtopology PingEntries includes
#include "Svc/Subtopologies/CdhCore/PingEntries.hpp"
#include "Svc/Subtopologies/ComCcsds/PingEntries.hpp"

// SubtopologyTopologyDefs includes
#include "Svc/Subtopologies/CdhCore/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsds/SubtopologyTopologyDefs.hpp"

//ComCcsds Enum Includes
#include "Svc/Subtopologies/ComCcsds/Ports_ComPacketQueueEnumAc.hpp"
#include "Svc/Subtopologies/ComCcsds/Ports_ComBufferQueueEnumAc.hpp"

// Include autocoded FPP constants
#include "Stm32f4Reference/Deployments/Stm32f4Deployment/Top/FppConstantsAc.hpp"

/**
 * \brief required ping constants
 *
 * The topology autocoder requires a WARN and FATAL constant definition for each component that supports the health-ping
 * interface. These are expressed as enum constants placed in a namespace named for the component instance. These
 * are all placed in the PingEntries namespace.
 *
 * Each constant specifies how many missed pings are allowed before a WARNING_HI/FATAL event is triggered. In the
 * following example, the health component will emit a WARNING_HI event if the component instance cmdDisp does not
 * respond for 3 pings and will FATAL if responses are not received after a total of 5 pings.
 *
 * ```c++
 * namespace PingEntries {
 * namespace cmdDisp {
 *     enum { WARN = 3, FATAL = 5 };
 * }
 * }
 * ```
 */
namespace PingEntries {
    namespace Deployments_rateGroup_1Hz {enum { WARN = 3, FATAL = 5 };}
    namespace Deployments_rateGroup_0_25Hz {enum { WARN = 3, FATAL = 5 };}
}  // namespace PingEntries

// Definitions are placed within the same namespace as the FPP module that contains the topology.
namespace Deployments {

/**
 * \brief State required by the retained CdhCore and ComCcsds subtopologies.
 */
struct TopologyState {
    CdhCore::SubtopologyState cdhCore;
    ComCcsds::SubtopologyState comCcsds;
};

namespace PingEntries = ::PingEntries;
}  // namespace Deployments

#endif

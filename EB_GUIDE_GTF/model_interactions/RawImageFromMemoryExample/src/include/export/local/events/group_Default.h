#ifndef GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_DEFAULT_H
#define GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_DEFAULT_H

//! @brief GroupName: Default
#define LOCAL_EVENT_GROUP_DEFAULT ((uint32_t) 0x00000002)
#include <gtf/types/TypesC99.h>

//! @brief Name: feature
//! @brief GroupName: Default
//! @brief Namespace : 
//!
//! Parameters: 1
//! 1) featureID, Index: 0, Type: gtf::eventsystem::IntegerValue
#define LOCAL_EVENT_GRP_ID_FEATURE LOCAL_EVENT_GROUP_DEFAULT
#define LOCAL_EVENT_MSG_ID_FEATURE ((uint32_t) 0x00000002)

//! @brief Name: home
//! @brief GroupName: Default
//! @brief Namespace : 
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_HOME LOCAL_EVENT_GROUP_DEFAULT
#define LOCAL_EVENT_MSG_ID_HOME ((uint32_t) 0x00000001)

//! @brief Name: next
//! @brief GroupName: Default
//! @brief Namespace : 
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_NEXT LOCAL_EVENT_GROUP_DEFAULT
#define LOCAL_EVENT_MSG_ID_NEXT ((uint32_t) 0x00000000)

//! @brief Name: prev
//! @brief GroupName: Default
//! @brief Namespace : 
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_PREV LOCAL_EVENT_GROUP_DEFAULT
#define LOCAL_EVENT_MSG_ID_PREV ((uint32_t) 0x00000005)

#endif //GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_DEFAULT_H

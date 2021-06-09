#ifndef GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_APPLICATION_H
#define GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_APPLICATION_H

//! @brief GroupName: Application
#define LOCAL_EVENT_GROUP_APPLICATION ((uint32_t) 0x00010000)
#include <gtf/types/TypesC99.h>

//! @brief Name: evInterplayIn
//! @brief GroupName: Application
//! @brief Namespace : 
//!
//! Parameters: 1
//! 1) myParam, Index: 0, Type: gtf::eventsystem::IntegerValue
#define LOCAL_EVENT_GRP_ID_EVINTERPLAYIN LOCAL_EVENT_GROUP_APPLICATION
#define LOCAL_EVENT_MSG_ID_EVINTERPLAYIN ((uint32_t) 0x00000003)

//! @brief Name: evInterplayOut
//! @brief GroupName: Application
//! @brief Namespace : 
//!
//! Parameters: 1
//! 1) myParam, Index: 0, Type: gtf::eventsystem::IntegerValue
#define LOCAL_EVENT_GRP_ID_EVINTERPLAYOUT LOCAL_EVENT_GROUP_APPLICATION
#define LOCAL_EVENT_MSG_ID_EVINTERPLAYOUT ((uint32_t) 0x00000004)

#endif //GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_APPLICATION_H

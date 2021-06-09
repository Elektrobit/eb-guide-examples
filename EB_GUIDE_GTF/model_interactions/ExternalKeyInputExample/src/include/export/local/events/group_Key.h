#ifndef GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_KEY_H
#define GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_KEY_H

//! @brief GroupName: Key
#define LOCAL_EVENT_GROUP_KEY ((uint32_t) 0x0000000A)
#include <gtf/types/TypesC99.h>

//! @brief Name: keyEvent
//! @brief GroupName: Key
//! @brief Namespace : 
//!
//! Parameters: 2
//! 1) displayManagerId, Index: 0, Type: gtf::eventsystem::IntegerValue
//! 2) status, Index: 1, Type: gtf::eventsystem::IntegerValue
#define LOCAL_EVENT_GRP_ID_KEYEVENT LOCAL_EVENT_GROUP_KEY
#define LOCAL_EVENT_MSG_ID_KEYEVENT ((uint32_t) 0x00000020)

#endif //GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_KEY_H

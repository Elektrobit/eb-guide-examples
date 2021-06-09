#ifndef GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_TOUCH_H
#define GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_TOUCH_H

//! @brief GroupName: Touch
#define LOCAL_EVENT_GROUP_TOUCH ((uint32_t) 0x0000000B)
#include <gtf/types/TypesC99.h>

//! @brief Name: externalTouch
//! @brief GroupName: Touch
//! @brief Namespace : 
//!
//! Parameters: 5
//! 1) displayManagerId, Index: 0, Type: gtf::eventsystem::IntegerValue
//! 2) status, Index: 1, Type: gtf::eventsystem::IntegerValue
//! 3) x, Index: 2, Type: gtf::eventsystem::IntegerValue
//! 4) y, Index: 3, Type: gtf::eventsystem::IntegerValue
//! 5) fingerId, Index: 4, Type: gtf::eventsystem::IntegerValue
#define LOCAL_EVENT_GRP_ID_EXTERNALTOUCH LOCAL_EVENT_GROUP_TOUCH
#define LOCAL_EVENT_MSG_ID_EXTERNALTOUCH ((uint32_t) 0x00000000)

#endif //GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_TOUCH_H

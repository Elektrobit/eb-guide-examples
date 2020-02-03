#ifndef GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_GLOBAL_H
#define GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_GLOBAL_H

//! @brief GroupName: global
#define LOCAL_EVENT_GROUP_GLOBAL ((uint32_t) 0x00010000)
#include <gtf/types/TypesC99.h>

//! @brief Name: GoToA
//! @brief GroupName: global
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_GOTOA LOCAL_EVENT_GROUP_GLOBAL
#define LOCAL_EVENT_MSG_ID_GOTOA ((uint32_t) 0x00000001)

//! @brief Name: GoToB
//! @brief GroupName: global
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_GOTOB LOCAL_EVENT_GROUP_GLOBAL
#define LOCAL_EVENT_MSG_ID_GOTOB ((uint32_t) 0x00000002)

//! @brief Name: WhatToDo
//! @brief GroupName: global
//!
//! Parameters: 0
#define LOCAL_EVENT_GRP_ID_WHATTODO LOCAL_EVENT_GROUP_GLOBAL
#define LOCAL_EVENT_MSG_ID_WHATTODO ((uint32_t) 0x00000000)

#endif //GTF_INCLUDED_LOCAL_EVENTS_IDENTIFIERS_GROUP_GLOBAL_H

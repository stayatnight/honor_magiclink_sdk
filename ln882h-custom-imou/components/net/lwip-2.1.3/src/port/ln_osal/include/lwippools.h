/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

/* Memory pool definitions for the libcoap when used with lwIP (which has its
 * own mechanism for quickly allocating chunks of data with known sizes). Has
 * to be findable by lwIP (ie. an #include <lwippools.h> must either directly
 * include this or include something more generic which includes this), and
 * MEMP_USE_CUSTOM_POOLS has to be set in lwipopts.h. */

#include "coap_internal.h"
#include "coap_net.h"
#include "coap_resource.h"
#include "coap_subscribe.h"

#ifndef MEMP_NUM_COAPCONTEXT
#define MEMP_NUM_COAPCONTEXT 1
#endif

#ifndef MEMP_NUM_COAPENDPOINT
#define MEMP_NUM_COAPENDPOINT 2
#endif

/* 1 is sufficient as this is very short-lived */
#ifndef MEMP_NUM_COAPPACKET
#define MEMP_NUM_COAPPACKET 1
#endif

#ifndef MEMP_NUM_COAPNODE
#define MEMP_NUM_COAPNODE 4
#endif

#ifndef MEMP_NUM_COAPPDU
#define MEMP_NUM_COAPPDU MEMP_NUM_COAPNODE
#endif

#ifndef MEMP_NUM_COAPSESSION
#define MEMP_NUM_COAPSESSION 4
#endif

#ifndef MEMP_NUM_COAP_SUBSCRIPTION
#define MEMP_NUM_COAP_SUBSCRIPTION 0
#endif

#ifndef MEMP_NUM_COAPRESOURCE
#define MEMP_NUM_COAPRESOURCE 3
#endif

#ifndef MEMP_NUM_COAPRESOURCEATTR
#define MEMP_NUM_COAPRESOURCEATTR 0
#endif

#ifndef MEMP_NUM_COAPOPTLIST
#define MEMP_NUM_COAPOPTLIST 0
#endif

#ifndef MEMP_LEN_COAPOPTLIST
#define MEMP_LEN_COAPOPTLIST 12
#endif

#ifndef MEMP_NUM_COAPSTRING
#define MEMP_NUM_COAPSTRING 10
#endif

#ifndef MEMP_LEN_COAPSTRING
#define MEMP_LEN_COAPSTRING 32
#endif

#ifndef MEMP_NUM_COAPCACHE_KEYS
#define MEMP_NUM_COAPCACHE_KEYS        (0U)
#endif /* MEMP_NUM_COAPCACHE_KEYS */

#ifndef MEMP_NUM_COAPCACHE_ENTRIES
#define MEMP_NUM_COAPCACHE_ENTRIES        (0U)
#endif /* MEMP_NUM_COAPCACHE_ENTRIES */

#ifndef MEMP_NUM_COAPPDUBUF
#define MEMP_NUM_COAPPDUBUF 2
#endif

#ifndef MEMP_LEN_COAPPDUBUF
#define MEMP_LEN_COAPPDUBUF 32
#endif

#ifndef MEMP_NUM_COAPLGXMIT
#define MEMP_NUM_COAPLGXMIT 0
#endif

#ifndef MEMP_NUM_COAPLGCRCV
#define MEMP_NUM_COAPLGCRCV 0
#endif

#ifndef MEMP_NUM_COAPLGSRCV
#define MEMP_NUM_COAPLGSRCV 0
#endif

#ifndef MEMP_NUM_COAPDIGESTCTX
#define MEMP_NUM_COAPDIGESTCTX 4
#endif

LWIP_MEMPOOL(COAP_CONTEXT, MEMP_NUM_COAPCONTEXT, sizeof(coap_context_t), "COAP_CONTEXT")
#ifdef COAP_SERVER_SUPPORT
LWIP_MEMPOOL(COAP_ENDPOINT, MEMP_NUM_COAPENDPOINT, sizeof(coap_endpoint_t), "COAP_ENDPOINT")
#endif /* COAP_SERVER_SUPPORT */
LWIP_MEMPOOL(COAP_PACKET, MEMP_NUM_COAPPACKET, sizeof(coap_packet_t), "COAP_PACKET")
LWIP_MEMPOOL(COAP_NODE, MEMP_NUM_COAPNODE, sizeof(coap_queue_t), "COAP_NODE")
LWIP_MEMPOOL(COAP_PDU, MEMP_NUM_COAPPDU, sizeof(coap_pdu_t), "COAP_PDU")
LWIP_MEMPOOL(COAP_SESSION, MEMP_NUM_COAPSESSION, sizeof(coap_session_t), "COAP_SESSION")
#ifdef COAP_SERVER_SUPPORT
LWIP_MEMPOOL(COAP_SUBSCRIPTION, MEMP_NUM_COAP_SUBSCRIPTION, sizeof(coap_subscription_t), "COAP_SUBSCRIPTION")
LWIP_MEMPOOL(COAP_RESOURCE, MEMP_NUM_COAPRESOURCE, sizeof(coap_resource_t), "COAP_RESOURCE")
LWIP_MEMPOOL(COAP_RESOURCEATTR, MEMP_NUM_COAPRESOURCEATTR, sizeof(coap_attr_t), "COAP_RESOURCEATTR")
#endif /* COAP_SERVER_SUPPORT */
LWIP_MEMPOOL(COAP_OPTLIST, MEMP_NUM_COAPOPTLIST, sizeof(coap_optlist_t)+MEMP_LEN_COAPOPTLIST, "COAP_OPTLIST")
LWIP_MEMPOOL(COAP_STRING, MEMP_NUM_COAPSTRING, sizeof(coap_string_t)+MEMP_LEN_COAPSTRING, "COAP_STRING")
#ifdef COAP_SERVER_SUPPORT
LWIP_MEMPOOL(COAP_CACHE_KEY, MEMP_NUM_COAPCACHE_KEYS, sizeof(coap_cache_key_t), "COAP_CACHE_KEY")
LWIP_MEMPOOL(COAP_CACHE_ENTRY, MEMP_NUM_COAPCACHE_ENTRIES, sizeof(coap_cache_entry_t), "COAP_CACHE_ENTRY")
#endif /* COAP_SERVER_SUPPORT */
LWIP_MEMPOOL(COAP_PDU_BUF, MEMP_NUM_COAPPDUBUF, MEMP_LEN_COAPPDUBUF, "COAP_PDU_BUF")
LWIP_MEMPOOL(COAP_LG_XMIT, MEMP_NUM_COAPLGXMIT, sizeof(coap_lg_xmit_t), "COAP_LG_XMIT")
#ifdef COAP_CLIENT_SUPPORT
LWIP_MEMPOOL(COAP_LG_CRCV, MEMP_NUM_COAPLGCRCV, sizeof(coap_lg_crcv_t), "COAP_LG_CRCV")
#endif /* COAP_CLIENT_SUPPORT */
#ifdef COAP_SERVER_SUPPORT
LWIP_MEMPOOL(COAP_LG_SRCV, MEMP_NUM_COAPLGSRCV, sizeof(coap_lg_srcv_t), "COAP_LG_SRCV")
#endif /* COAP_SERVER_SUPPORT */
LWIP_MEMPOOL(COAP_DIGEST_CTX, MEMP_NUM_COAPDIGESTCTX, sizeof(coap_digest_t) + sizeof(size_t), "COAP_DIGEST_CTX")
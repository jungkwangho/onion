/*
	Onion HTTP server library
	Copyright (C) 2010 David Moreno Montero

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 3.0 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not see <http://www.gnu.org/licenses/>.
	*/

#ifndef __ONION_TYPES__
#define __ONION_TYPES__

/**
 * @struct onion_dict_t
 * @short A 'char *' to 'char *' dictionary.
 */
struct onion_dict_t;
typedef struct onion_dict_t onion_dict;
/**
 * @struct onion_handler_t
 * @short Information about a handler for onion. A tree structure of handlers is what really serves the data.
 */
struct onion_handler_t;
typedef struct onion_handler_t onion_handler;

/**
 * @struct onion_url_t
 * @short Url regexp pack. This is also a handler, and can be converted with onion_url_to_handle.
 */
struct onion_url_t;
typedef struct onion_url_t onion_url;

/**
 * @struct onion_request_t
 * @short Basic information about a request
 */
struct onion_request_t;
typedef struct onion_request_t onion_request;
/**
 * @struct onion_response_t
 * @short The response
 */
struct onion_response_t;
typedef struct onion_response_t onion_response;
/**
 * @struct onion_server_t
 * @short Onion server that do not depend on specific IO structure.
 * 
 * This is separated as you can build your own servers using this structure instead of onion_t. For example
 * using onion_server_t you can do a inet daemon that listens HTTP data.
 */
struct onion_server_t;
typedef struct onion_server_t onion_server;
/**
 * @struct onion_t
 * @short Webserver info.
 * 
 * This is information about onion implementation of the generic server. It contains the listening descriptors,
 * the SSL parameters if SSL is enabled... 
 * 
 * This is platform specific server IO. Normally POSIX, using TCP/IP.
 */
struct onion_t;
typedef struct onion_t onion;
/**
 * @struct onion_sessions_t
 * @short Storage for all sessions known
 * 
 * This is a simple storage for sessions.
 * 
 * Sessions are thread safe to use.
 * 
 * The sessions themselves are not created until some data is written to it by the program. This way we avoid
 * "session attack" where a malicious user sends many petitions asking for new sessions.
 * 
 * FIXME to add some LRU so that on some moment we can remove old sessions.
 */
struct onion_sessions_t;
typedef struct onion_sessions_t onion_sessions;


/**
 * @struct onion_block_t
 * @short Data type to store some raw data
 * 
 * Normally it will be used to store strings when the size is unknown beforehand,
 * but it can contain any type of data.
 * 
 * Use with care as in most situations it might not be needed and more efficient 
 * alternatives may exist.
 */
struct onion_block_t;
typedef struct onion_block_t onion_block;

/**
 * @struct onion_poller_t
 * @short Manages the polling on a set of file descriptors
 */
struct onion_poller_t;
typedef struct onion_poller_t onion_poller;

/**
 * @struct onion_poller_slot_t
 * @short Data about a poller element: timeout, function to call shutdown function
 * @memberof onion_poller_t
 */
struct onion_poller_slot_t;
typedef struct onion_poller_slot_t onion_poller_slot;


/**
 * @struct onion_listen_point_t
 * @short Stored common data for each listen point: address, port, protocol status data...
 * @memberof onion_protocol_t
 *
 * Stored information about the listen points; where they are listenting, and how to handle
 * a new connection. Each listen point can understand a protocol and associated data.
 * 
 * A protocol is HTTP, HTTPS, SPDY... each may do the request parsing in adiferent way, and the 
 * response write too.
 * 
 * A listen point A can be HTTPS with one certificate, and B with another, with C using SPDY.
 * 
 */
struct onion_listen_point_t;
typedef struct onion_listen_point_t onion_listen_point;

/// Flags for the mode of operation of the onion server.
enum onion_mode_e{
	O_ONE=1,							///< Perform just one petition
	O_ONE_LOOP=3,					///< Perform one petition at a time; lineal processing
	O_THREADED=4,					///< Threaded processing, process many petitions at a time. Needs pthread support.
	O_DETACH_LISTEN=8,		///< When calling onion_listen, it returns inmediatly and do the listening on another thread. Only if threading is available.
	O_SYSTEMD=0x010,			///< Allow to start as systemd service. It try to start as if from systemd, but if not, start normally, so its "transparent".
/**
 * @short Use polling for request read, then as other flags say
 * 
 * O_POLL must be used with other method, O_ONE_LOOP or O_THREAD, and it will poll for requests until the
 * request is ready. On that moment it will or just launch the request, which should not block, or
 * a new thread for this request.
 * 
 * If on O_ONE_LOOP mode the request themselves can hook to the onion_poller object, and be called 
 * when ready. (TODO).
 * 
 */
	O_POLL=0x020, ///< Use epoll for request read, then as other flags say.
  O_POOL=0x024, ///< Create some threads, and make them listen for ready file descriptors. It is O_POLL|O_THREADED
	/// @{  @name From here on, they are internal. User may check them, but not set.
	O_SSL_AVAILABLE=0x0100, ///< This is set by the library when creating the onion object, if SSL support is available.
	O_SSL_ENABLED=0x0200,   ///< This is set by the library when setting the certificates, if SSL is available.

	O_THREADS_AVALIABLE=0x0400, ///< Threads are available on this onion build
	O_THREADS_ENABLED=0x0800,   ///< Threads are enabled on this onion object. It difers from O_THREADED as this is set by the library, so it states a real status, not a desired one.
	
	O_DETACHED=0x01000,		///< Currently listening on another thread.
	/// @}
};

typedef enum onion_mode_e onion_mode;

/**
 * @short The desired connection state of the connection.
 * 
 * If <0 it means close connection. May mean also to show something to the client.
 */
enum onion_connection_status_e{
	OCS_NOT_PROCESSED=0,
	OCS_NEED_MORE_DATA=1,
	OCS_PROCESSED=2,
	OCS_CLOSE_CONNECTION=-2,
	OCS_KEEP_ALIVE=3,
	OCS_INTERNAL_ERROR=-500,
	OCS_NOT_IMPLEMENTED=-501,
  OCS_FORBIDDEN=-502,
};

typedef enum onion_connection_status_e onion_connection_status;


/// Signature of request handlers.
typedef onion_connection_status (*onion_handler_handler)(void *privdata, onion_request *req, onion_response *res);
/// Signature of free function of private data of request handlers
typedef void (*onion_handler_private_data_free)(void *privdata);


#endif


/* Copyright  (C) 2010-2022 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (net_natt.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _LIBRETRO_SDK_NET_NATT_H
#define _LIBRETRO_SDK_NET_NATT_H

#include <net/net_compat.h>
#include <net/net_socket.h>

#include <retro_common_api.h>

RETRO_BEGIN_DECLS

enum natt_forward_type
{
   NATT_FORWARD_TYPE_NONE,
   NATT_FORWARD_TYPE_ANY
};

/* Use this enum to implement a higher-level interface. */
enum nat_traversal_status
{
   NAT_TRAVERSAL_STATUS_DISCOVERY,
   NAT_TRAVERSAL_STATUS_SELECT_DEVICE,
   NAT_TRAVERSAL_STATUS_QUERY_DEVICE,
   NAT_TRAVERSAL_STATUS_EXTERNAL_ADDRESS,
   NAT_TRAVERSAL_STATUS_OPEN,
   NAT_TRAVERSAL_STATUS_OPENING,
   NAT_TRAVERSAL_STATUS_OPENED,
   NAT_TRAVERSAL_STATUS_CLOSE,
   NAT_TRAVERSAL_STATUS_CLOSING,
   NAT_TRAVERSAL_STATUS_CLOSED
};

struct natt_discovery
{
   retro_time_t timeout;
   int fd;
};

struct natt_device
{
   struct sockaddr_in addr;
   struct sockaddr_in ext_addr;
   char desc        [256];
   char control     [256];
   char service_type[256];
   bool busy;
};

struct natt_request
{
   struct sockaddr_in addr;
   struct natt_device *device;
   enum socket_protocol proto;
   bool success;
};

/* Use this struct to implement a higher-level interface. */
struct nat_traversal_data
{
   struct natt_request request;
   enum natt_forward_type forward_type;
   enum nat_traversal_status status;
};

/**
 * natt_init:
 *
 * @discovery : Pointer to a discovery object that will be written to.
 *
 * Starts a multicast discovery for UPnP devices.
 *
 * Returns: true if the discovery was started.
 */
bool natt_init(struct natt_discovery *discovery);

/**
 * natt_device_next:
 *
 * @discovery : Pointer to a discovery object.
 * @device    : Pointer to a device object that will be written to.
 *
 * Grabs the next device that has reported in to our discovery.
 *
 * Returns: true if we've retrieved a new device or
 * if timeout has not yet been reached. If device->desc is not an empty string,
 * a new valid device was retrieved.
 */
bool natt_device_next(struct natt_discovery *discovery,
   struct natt_device *device);

/**
 * natt_device_end:
 *
 * @discovery : Pointer to a discovery object.
 *
 * Stop checking for new devices and close the discovery socket.
 *
 */
void natt_device_end(struct natt_discovery *discovery);

/**
 * natt_query_device:
 *
 * @device : Pointer to a device to query into.
 * @block  : Blocks until the HTTP task is finished.
 *
 * Query an IGD for its service type and control URL.
 *
 * Returns: true if the task was successfully started.
 * If both device->service_type and device->control are not empty strings,
 * the task completed successfully.
 */
bool natt_query_device(struct natt_device *device, bool block);

/**
 * natt_external_address:
 *
 * @device : Pointer to a device to retrieve its external address.
 * @block  : Blocks until the HTTP task is finished.
 *
 * Retrieve the external IP address of an IGD.
 * natt_query_device must have been successfully called.
 *
 * Returns: true if the task was successfully started.
 * If device->ext_addr.sin_family is AF_INET,
 * the task completed successfully.
 */
bool natt_external_address(struct natt_device *device, bool block);

/**
 * natt_open_port:
 *
 * @device       : Pointer to a device to forward a port.
 * @request      : Port forwarding request information.
 * @forward_type : UPnP port forwarding command type.
 * @block        : Blocks until the HTTP task is finished.
 *
 * Forward a port.
 * natt_query_device must have been successfully called.
 *
 * Returns: true if the task was successfully started.
 * If request->success is true, the task completed successfully.
 */
bool natt_open_port(struct natt_device *device,
   struct natt_request *request, enum natt_forward_type forward_type,
   bool block);

/**
 * natt_close_port:
 *
 * @device       : Pointer to a device to unforward a port.
 * @request      : Port unforwarding request information.
 * @block        : Blocks until the HTTP task is finished.
 *
 * Unforward a port.
 * natt_query_device must have been successfully called.
 *
 * Returns: true if the task was successfully started.
 * If request->success is true, the task completed successfully.
 */
bool natt_close_port(struct natt_device *device,
   struct natt_request *request, bool block);

RETRO_END_DECLS

#endif

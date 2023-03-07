#pragma once

#ifndef _ZF_LOG_CONFIG_H_
#define _ZF_LOG_CONFIG_H_

#if !defined(_WIN32) && !defined(_WIN64)

#if defined(__linux__)
	#if !defined(__ANDROID__) && !defined(_GNU_SOURCE)
		#define _GNU_SOURCE
	#endif
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "zf_log.h"

#define ZF_LOG_EXTERN_GLOBAL_OUTPUT 1

#define DO(expr) while((expr) < 0 && errno == EINTR)
typedef struct {const char *ip; const int port;} target;

static void socket_output_callback(const zf_log_message *msg, void *arg)
{
	static struct sockaddr_in addr;
	static int fd = -2;
	if (fd == -2)
	{
		memset(&addr, 0, sizeof(addr));

		target *dst = (target *)arg;

		addr.sin_family = AF_INET;
		addr.sin_port   = htons(dst->port);

		if (dst->ip && inet_pton(AF_INET, dst->ip, &addr.sin_addr) == 1)
			fd = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, 0);
	}
	if (fd >= 0)
		DO(sendto(fd, msg->buf, (msg->p - msg->buf), 0, (struct sockaddr *)&addr, sizeof(addr)));
}

static target sink = {"239.192.0.1", 42099};
ZF_LOG_DEFINE_GLOBAL_OUTPUT = {ZF_LOG_PUT_STD, &sink, socket_output_callback};

#endif  /* !defined(_WIN32) && !defined(_WIN64) */
#endif  /* _ZF_LOG_CONFIG_H_ */

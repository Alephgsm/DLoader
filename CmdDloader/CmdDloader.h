#pragma once

#include "resource.h"
#define DO_PACKET_PORT (-1)
/* Errors */
enum
{
	DOWNLOAD_OK = 0,
	DOWNLOAD_ERRPARAM,
	DOWNLOAD_ERRLSTARTDLOADER,
	DOWNLOAD_ERRDOWNLOAD,
	DOWNLOAD_ERR_DETECT_DUT,
	/* insert here new errors */
	DOWNLOAD_ERRMAX,
    DOWNLOAD_ALREADY_RUNNING
};

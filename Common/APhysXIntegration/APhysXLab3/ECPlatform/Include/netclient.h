#ifndef __GNET__NETCLIENT
#define __GNET__NETCLIENT

#include "gnproto.h"

namespace GNET
{
	class NetClient: public Protocol::Manager
	{
	public:
		Octets nonce;
	};
};
#endif

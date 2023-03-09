#include "PCH.h"
#include "Networking.h"


namespace Spaceng
{
	void NET::test()
	{
		asio::io_context io_context;
		asio::ip::tcp::socket socket(io_context);
	}
}

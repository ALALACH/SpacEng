#pragma once
#include "Asio_/asio/include/asio.hpp"
#include "Spaceng/Core/Core.h"


namespace Spaceng
{

	class Server
	{
	public:
		Server(asio::io_context& io_context, uint32_t port);
		void Run();
		void SendData();

	private:
		asio::ip::tcp::acceptor Acceptor;
		asio::io_context io_context;
		std::optional<asio::ip::tcp::socket> Socket;
		uint32_t Port =0;
	};

	




}
#pragma once
#include "Asio_/asio/include/asio.hpp"
#include "Spaceng/Core/Core.h"


namespace Spaceng
{

	class Server
	{
	public:
		Server(uint32_t port);
		~Server();
		void do_accept();
		void EncapsulateIO();
		void _Run();
		void SendData();

	private:
		std::thread Server_thread;
		asio::io_context _io_context;
		asio::ip::tcp::acceptor Acceptor;
		std::shared_ptr <asio::ip::tcp::socket> _Socket;
		uint32_t Port =0;
		uint32_t MAX_PENDING = 0;
	};

	class Service
	{
	public:
		Service(std::shared_ptr<asio::ip::tcp::socket> sock);
		void Handle_Request();
	private:
		std::string ProcessRequest(asio::streambuf& request);
	private:
		std::shared_ptr<asio::ip::tcp::socket> Service_Socket;
		std::string m_response;
		asio::streambuf m_request;
	};




}
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
		void ReceiveData();

	private:
		std::thread Server_thread;
		asio::io_context _io_context;
		asio::ip::tcp::acceptor Acceptor;
		uint32_t Port =0;
		uint32_t MAX_PENDING = 0;
	};

	class Service : public std::enable_shared_from_this<Service>
	{
	public:
		Service(asio::ip::tcp::socket sock);
		void Handle_Request();
	private:
		std::string ProcessRequest(asio::streambuf& request);
	private:
		asio::ip::tcp::socket Service_Socket;
		std::string m_response;
		asio::streambuf m_request;
		std::size_t MAX_TCP_BLOCK = 65536;
		size_t Data_size = 4;
		uint8_t* buf = nullptr;
		asio::mutable_buffer receive_buffer;


	};




}
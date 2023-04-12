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
	public:
		std::queue<std::vector<uint8_t>> Queue;
		std::vector<uint8_t> Data;
		uint32_t Port =0;
	private:
		std::thread Server_thread;
		asio::io_context _io_context;
		asio::ip::tcp::acceptor Acceptor;
		uint32_t MAX_PENDING = 0;
	};

	class Service : public std::enable_shared_from_this<Service>
	{
	public:
		Service(asio::ip::tcp::socket sock);
		void Handle_Request(Server* Server);
	private:
		std::string ProcessRequest(asio::streambuf& request);
	private:
		asio::ip::tcp::socket Service_Socket;
		Server* Service_Server = nullptr;
		size_t Data_size = 65536;
		uint8_t* buf = nullptr;
		asio::mutable_buffer receive_buffer;
		std::size_t MAX_TCP_BLOCK = 65536;

		std::string m_response;
		asio::streambuf m_request;
		int index = 0;
		


	};




}
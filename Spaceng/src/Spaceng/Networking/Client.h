#pragma once
#include "Asio_/asio/include/asio.hpp"
#include "Spaceng/Core/Core.h"
#include <string>


namespace Spaceng
{

	class Client
	{
	public:
		Client(uint32_t Port , std::string& Adress);
		~Client();
		void connect();
		void SendData();
		void SendImgData();
		void ReceiveData();
	private:
	private:
		std::thread io_context_thread_;
		asio::io_context io_context;
		asio::ip::tcp::resolver::results_type endpoints;
		asio::ip::tcp::socket Client_Socket;
		std::size_t MAX_TCP_BLOCK = 65536;
	};





}
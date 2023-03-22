#include "PCH.h"
#include "Client.h"


namespace Spaceng
{
	Client::Client(uint32_t Port, std::string& Adress)
		:Client_Socket(io_context)
	{
		asio::ip::tcp::resolver resolver (io_context);
		endpoints = resolver.resolve(Adress, std::to_string(Port));
	}
	void Client::connect()
	{
		asio::async_connect(Client_Socket, endpoints, [this](asio::error_code er, asio::ip::tcp::endpoint ep) {
			if (!er)
			{
				std::srand(static_cast<unsigned int>(std::time(nullptr)));
				uint32_t random_num = std::rand() % 100;
				SE_LOG_INFO("Client {0} Connected to server!",random_num);
			}
			else
			{
				SE_LOG_ERROR ("failed to connect to Server")
			}
		});
		io_context.run();
	}
	void Client::SendData(std::string buffer)
	{
		asio::streambuf buf;
		std::ostream os(&buf);
		os.write(reinterpret_cast<const char*>(buffer.c_str()), sizeof(buffer));

		asio::async_write(Client_Socket,buf, [&](asio::error_code er, std::size_t bytesTransferred) {
			if (er)
			{
				Client_Socket.close(er);
				SE_LOG_ERROR("couldn't send data packet: {0}",er.message());
				return;
			}

			if (bytesTransferred < buf.size())
			{
				SendData(buffer);
				SE_LOG_CRITICAL("Debug");
			}
			else
			{
				SE_LOG_DEBUG("Client buffer Sent Successfully");
			}
			});
	}
	void Client::ReceiveData()
	{

	}
}
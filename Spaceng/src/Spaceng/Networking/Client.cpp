#include "PCH.h"
#include "Client.h"


namespace Spaceng
{
	Client::Client(uint32_t Port, std::string& Adress)
		:Socket(io_context)
	{
		asio::ip::tcp::resolver resolver (io_context);
		endpoints = resolver.resolve(Adress, std::to_string(Port));
	}
	void Client::connect()
	{
		asio::async_connect(Socket, endpoints, [this](asio::error_code ec, asio::ip::tcp::endpoint ep) {
			if (!ec)
			{
				SE_LOG_INFO("Connected to server!");
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

		asio::async_write(Socket,buf, [&](asio::error_code ec, size_t bytesTransferred) {
			if (ec)
			{
				Socket.close(ec);
				SE_LOG_ERROR("couldn't send data packet");
				std::cout << ec << std::endl;
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
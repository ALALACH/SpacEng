#include "PCH.h"
#include "Server.h"


namespace Spaceng
{
	Server::Server(asio::io_context& io_context , uint32_t port)
		:Acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		Port = static_cast<uint32_t>(Acceptor.local_endpoint().port());
	}

	void Server::Run()
	{
		try {
			uint32_t MAX_PENDING = 30;
			//Acceptor.listen(MAX_PENDING);
			//SE_LOG_INFO("Server Listening on Port : {0}", Port);
			Socket.emplace(io_context);
			Acceptor.async_accept(*Socket, [this](const asio::error_code& er) {
				if (er)
				{
					SE_LOG_INFO("Couldn't initiate the Server");
				}
				else
				{
					SE_LOG_INFO ("SERVER LISTENING...")
				}
				Run();
			});


			io_context.run();
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			__debugbreak();
		}
	}	
		
	void SendData()
	{
	
	}


}

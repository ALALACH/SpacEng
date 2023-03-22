#include "PCH.h"
#include "Server.h"


namespace Spaceng
{
	//Server

	Server::Server(uint32_t port)
		:Acceptor(_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		Port = static_cast<uint32_t>(Acceptor.local_endpoint().port());
		SE_LOG_DEBUG("Server Listening on Port : {0}", Port);
	}
	Server::~Server()
	{
		_io_context.stop();
		Acceptor.cancel();
		Acceptor.close();
		_Socket->close();
		Server_thread.join();
	}
	void Server::_Run()
	{
		do_accept();
		std::thread ServerThread(&Server::EncapsulateIO, this);
		Server_thread = std::move(ServerThread);
	}
	void Server::EncapsulateIO()
	{
		std::mutex threadMutex;
		threadMutex.lock();
		asio::io_context::work work(_io_context);
		_io_context.run();
		threadMutex.unlock();
	}
	void Server::do_accept()
	{
		try
		{ 
			MAX_PENDING = 30;
			Acceptor.listen(MAX_PENDING);
			_Socket = std::make_shared<asio::ip::tcp::socket>(_io_context);
			Acceptor.async_accept(*_Socket, [this](const asio::error_code& er) {
				if (er)
				{
					SE_LOG_ERROR("Couldn't initiate the Server : {0} ",er.message());
				}
				else
				{
					SE_LOG_INFO("SERVER accepted Client..."); //remove 
					(new Service(_Socket))->Handle_Request();
				}
				do_accept();
			});
			
		}
		catch (std::exception& er)
		{
			SE_LOG_ERROR("Server Error: {0} ",er.what());
		}
	}	
		
	void SendData()
	{
	
	}


	//Service

	Service::Service(std::shared_ptr<asio::ip::tcp::socket> Sock)
		:Service_Socket(Sock)
	{}

	void Service::Handle_Request()
	{
		asio::async_read_until(*Service_Socket.get(), m_request, '\n', [this](const asio::error_code& er, std::size_t bytesTransferred) 
			{
				if (er)
				{
					SE_LOG_ERROR("Service Error: {0}", er.message());
					delete this;
					return;
				}
				m_response = ProcessRequest(m_request); 
				
				asio::async_write(*Service_Socket.get(), asio::buffer(m_response), [this](asio::error_code er, std::size_t bytesTransfered)
					{
						if (er)
						{
							SE_LOG_ERROR("Service Error: {0}", er.message());
						}
						delete this; //free the allocated Service after response
					});

			});
	}

	std::string Service::ProcessRequest(asio::streambuf& request) //todo : do something with request : binding recources or setting commands
	{
		SE_LOG_INFO("Request received from client"); //todo : Add names or ID to each client that comes along with the request
		std::string response = "Server recieved the client";
		return response;
	}
}

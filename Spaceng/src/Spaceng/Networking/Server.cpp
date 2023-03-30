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
		MAX_PENDING = 3;
		Acceptor.listen(MAX_PENDING);
	}
	Server::~Server()
	{
		_io_context.stop();
		Acceptor.cancel();
		Acceptor.close();
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
		_io_context.run();
		threadMutex.unlock();
	}
	void Server::do_accept()
	{
		try
		{ 
			Acceptor.async_accept([this](const asio::error_code& er , asio::ip::tcp::socket Socket) {
				if (er)
				{
					SE_LOG_ERROR("Couldn't initiate the Server : {0} ",er.message());
				}
				else
				{
					SE_LOG_INFO("SERVER accepted Client..."); 
					std::make_shared<Service>(std::move(Socket))->Handle_Request();
				}
				do_accept();
			});
			
		}
		catch (std::exception& er)
		{
			SE_LOG_ERROR("Server Error: {0} ",er.what());
		}
	}	
		
	void Server::SendData()
	{
	
	}
	void Server::ReceiveData()
	{

	}

	//Service

	Service::Service(asio::ip::tcp::socket Sock)
		:Service_Socket(std::move(Sock))
	{}

	void Service::Handle_Request()
	{
		auto self(shared_from_this());
		if (Data_size == 4)
		{
			int test = 1;
			receive_buffer = asio::buffer(&test, 4);
		}
		else
		{
		buf = new uint8_t[Data_size];
		receive_buffer = asio::buffer(buf, Data_size);
		}
		
		asio::async_read(Service_Socket, receive_buffer, [this, self](const asio::error_code er, std::size_t bytesTransferred)
			{
				if (er)
				{
					SE_LOG_ERROR("couldn't receive Size packet: {0}", er.message());
				}
				else if (!er && bytesTransferred == 4)
				{
					int* data_ptr = asio::buffer_cast<int*>(receive_buffer);
					int size = static_cast<int>(*data_ptr);
					Data_size = size;
					SE_LOG_INFO("Size : {0}", Data_size);
					Handle_Request();
				}
				else if (!er && bytesTransferred > 4)
				{
					char* data_ptr = asio::buffer_cast<char*>(receive_buffer);
					std::size_t buffer_size = asio::buffer_size(receive_buffer);
					std::vector<uint8_t> data_vec(data_ptr, data_ptr + buffer_size); //transmit to Queue
					SE_LOG_INFO("Client Image received Successfully : {0} bytes", bytesTransferred);
					delete buf;
					//Data_size = 4;
					Handle_Request();
				}
			});
	}

	std::string Service::ProcessRequest(asio::streambuf& request) //todo : do something with request : binding recources or setting commands
	{
		SE_LOG_INFO("Data received from client"); //todo : Add names or ID to each client that comes along with the request
		std::string response = "";
		return response;
		
	}
}

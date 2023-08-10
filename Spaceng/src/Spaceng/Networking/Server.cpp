#include "PCH.h"
#include "Server.h"


namespace Spaceng
{
	//Server

	Server::Server(uint32_t port)
		:Acceptor(_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		Server_Instance = this;
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
				else if (!er)
				{
					std::make_shared<Service>(std::move(Socket))->Handle_Request(this);
					SE_LOG_INFO("Request Handle created for the client!");
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

	void Service::Handle_Request(Server* Server)
	{
		auto self(shared_from_this());
		ServerRef = Server;

		buf = new uint8_t[MAX_TCP_BLOCK];
		receive_buffer = asio::buffer(buf, MAX_TCP_BLOCK);

		asio::async_read(Service_Socket, receive_buffer, [this, self](const asio::error_code er, std::size_t bytesTransferred)
			{
				if (er)
				{
					SE_LOG_ERROR("couldn't receive packet: {0}", er.message());
					//TODO : handle all error cases and reset operations.
				}
				else if (!er)
				{
					SE_LOG_INFO("Client Packet received Successfully : {0} bytes", bytesTransferred);
					
					char* data_ptr = asio::buffer_cast<char*>(receive_buffer);
					std::size_t buffer_size = asio::buffer_size(receive_buffer);
					std::vector<uint8_t> data_vec(data_ptr, data_ptr + buffer_size);

					if (index == 0)
					{
						bool ImgTag = std::all_of(data_vec.begin(), data_vec.begin() + 6, [](int val) {return val == 0; }); //000000 - ImageTag
						if (ImgTag)
						{
							Chunks = data_vec[6];
						}
					}
					index++;

					ServerRef->Data.insert(ServerRef->Data.end(), std::make_move_iterator(data_vec.begin()), std::make_move_iterator(data_vec.end()));
					if (index == Chunks)
					{
						if (ServerRef->Data[7] == 1)
						{
							auto it = std::find(ServerRef->Data.rbegin(), ServerRef->Data.rend(), 255);
							if (it != ServerRef->Data.rend()) {
								ServerRef->Data.erase(it.base(), ServerRef->Data.end());
							}
							SE_LOG_WARN("END OF FILE FOUND - {0} Chunks", index);
						}
						std::mutex QueueMutex;
						QueueMutex.lock();
						ServerRef->Queue.push(std::move(ServerRef->Data));
						QueueMutex.unlock();
						index = 0;
					}

					delete buf;
					Handle_Request(ServerRef);
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

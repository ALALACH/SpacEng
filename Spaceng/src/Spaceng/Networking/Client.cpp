#include "PCH.h"
#include "Client.h"

#include "tinygltf/stb_image.h"
namespace Spaceng
{
	Client::Client(uint32_t Port, std::string& Adress)
		:Client_Socket(io_context)
	{
		asio::ip::tcp::resolver resolver (io_context);
		endpoints = resolver.resolve(Adress, std::to_string(Port));
	}
	Client::~Client()
	{
		//io_context_thread_.join();
	}
	void Client::connect()
	{
		asio::async_connect(Client_Socket, endpoints, [this](const asio::error_code er, asio::ip::tcp::endpoint ep) {
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
		//io_context_thread_ = std::thread([&] { io_context.run(); });
		io_context.run();
		
	}
	void Client::SendData() 
	{
		int stb_width, stb_height, stb_channels;
		uint8_t* ImgData = stbi_load("C:\\Users\\AcerNitro5\\Desktop\\Spaceng\\App\\assets\\Textures\\Screen.png", &stb_width, &stb_height, &stb_channels, 4);
		SE_ASSERT(ImgData, "Could not load Texture File.", stbi_failure_reason());
		int ImgSize = stb_width * stb_height * 4;

		asio::async_write(Client_Socket, asio::buffer(&ImgSize, sizeof(int)), [this](const asio::error_code er, std::size_t bytesTransferred)
			{
				if (er)
				{
					SE_LOG_ERROR("couldn't send Size packet: {0}", er.message());
				}
				else if (!er)
				{
					SE_LOG_INFO("Client Imagebuffer Size Sent Successfully");
				}
			});
		size_t refactor = (ImgSize / MAX_TCP_BLOCK) + 1;
		for (int i = 0; i < refactor; i++)
		{
			asio::async_write(Client_Socket, asio::buffer(ImgData, ImgSize), [this](const asio::error_code er, std::size_t bytesTransferred)
				{
					if (er)
					{
						SE_LOG_ERROR("couldn't send packet: {0}", er.message());
					}
					else if (!er)
					{
						SE_LOG_INFO("Client Imagebuffer Sent Successfully");
					}
				});
		}
	}
	void Client::ReceiveData()
	{

	}
	void Client::SendImgData()
	{
		int stb_width, stb_height, stb_channels;
		uint8_t* ImgData = stbi_load("C:\\Users\\AcerNitro5\\Desktop\\Spaceng\\App\\assets\\Textures\\Screen.png", &stb_width, &stb_height, &stb_channels, 4);
		SE_ASSERT(ImgData, "Could not load Texture File.", stbi_failure_reason());
		int ImgSize = stb_width * stb_height * 4;

		std::vector<uint8_t> data_vec(ImgData, ImgData + ImgSize);
		//std::vector<uint8_t> data_vec{ 1,2,3,4,254 };
		std::vector<uint8_t> StagingBuffer;
		for (int i = 0; i < data_vec.size(); i += (int)MAX_TCP_BLOCK)
		{
			auto start = data_vec.begin() + i;
			auto incremented = std::min(i + (int)MAX_TCP_BLOCK, static_cast<int>(data_vec.size()));
			auto end = data_vec.begin() + incremented;
			StagingBuffer.clear();
			std::copy(start, end, std::back_inserter(StagingBuffer));
			if (StagingBuffer.size() < MAX_TCP_BLOCK)
			{
				std::vector<uint8_t> FillVec(MAX_TCP_BLOCK - StagingBuffer.size() +1 ,0);
				StagingBuffer.insert(StagingBuffer.end(), FillVec.begin(), FillVec.end());
			}


			asio::async_write(Client_Socket, asio::buffer(StagingBuffer, MAX_TCP_BLOCK), [this](const asio::error_code er, std::size_t bytesTransferred)
				{
					if (er)
					{
						SE_LOG_ERROR("couldn't send packet: {0}", er.message());
					}
					else if (!er)
					{
						SE_LOG_INFO("Client Imagebuffer Sent Successfully");
					}
				});
		}
		
		stbi_image_free(ImgData);
		SE_LOG_CRITICAL("Sending Image Data..");
	}
}
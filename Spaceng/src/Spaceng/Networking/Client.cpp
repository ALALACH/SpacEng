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
		uint8_t* ImgData = stbi_load("C:\\Users\\AcerNitro5\\Desktop\\Spaceng\\Network\\assets\\Textures\\Screen.png", &stb_width, &stb_height, &stb_channels, 4);
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
	void Client::SendImgData(std::string Path)
	{
		std::string Filepath = Path + "\\assets\\Textures\\Screen3.jpg";
		int stb_width, stb_height, stb_channels;
		uint8_t* ImgData = stbi_load(Filepath.c_str(), &stb_width, &stb_height, &stb_channels, 4);
		SE_ASSERT(ImgData, "Could not load Texture File.", stbi_failure_reason());
		int ImgSize = stb_width * stb_height * 4;

		std::vector<uint8_t> data_vec(ImgData, ImgData + ImgSize);
	

		size_t Chunks = (ImgSize % MAX_TCP_BLOCK == 0 ) ? (ImgSize / MAX_TCP_BLOCK) : (ImgSize / MAX_TCP_BLOCK +1);
		SE_ASSERT(Chunks < 255, "Data too Big to Transmit : Reform")
		std::string Width;
		SE_ASSERT(stb_width>100,"relatively Small : Reform")
		(1000 <= stb_width && stb_width < 10000) ? Width = std::to_string(stb_width) : Width = "0" + std::to_string(stb_width);
		uint8_t w0 = static_cast<uint8_t>(Width[0] - '0');
		uint8_t w1 = static_cast<uint8_t>(Width[1] - '0');
		uint8_t w2 = static_cast<uint8_t>(Width[2] - '0');
		uint8_t w3 = static_cast<uint8_t>(Width[3] - '0');

		std::string Height;
		SE_ASSERT(stb_height > 100, "relatively small :Reform")
		(1000 <= stb_height && stb_height < 10000) ? Height = std::to_string(stb_height) : Height = "0"+ std::to_string(stb_height);
		uint8_t h0 = static_cast<uint8_t>(Height[0] - '0');
		uint8_t h1 = static_cast<uint8_t>(Height[1] - '0');
		uint8_t h2 = static_cast<uint8_t>(Height[2] - '0');
		uint8_t h3 = static_cast<uint8_t>(Height[3] - '0');

		bool Fill = false;
		if (ImgSize % MAX_TCP_BLOCK != 0)
		{
			size_t Remaining = ImgSize - (Chunks - 1) * MAX_TCP_BLOCK;
			size_t toFill = MAX_TCP_BLOCK - Remaining;
			if (toFill>4)
			Fill = true;
		}
		std::vector<uint8_t> Header = { 0,0,0,0,0,0,static_cast<uint8_t>(Chunks),static_cast<uint8_t>((Fill) ? 1 : 0),w0,w1,w2,w3,h0,h1,h2,h3 };
		data_vec.insert(data_vec.begin(), Header.begin(), Header.end());

		

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
				std::vector<uint8_t> FillVec(MAX_TCP_BLOCK - StagingBuffer.size() ,0);
				StagingBuffer.insert(StagingBuffer.end(), FillVec.begin(), FillVec.end());
			}


			asio::async_write(Client_Socket, asio::buffer(StagingBuffer, MAX_TCP_BLOCK), [this](const asio::error_code er, std::size_t bytesTransferred)
				{
					if (er)
					{
						SE_LOG_ERROR("couldn't send packet: {0}", er.message());
						//TODO : handle all error cases and reset operations.
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
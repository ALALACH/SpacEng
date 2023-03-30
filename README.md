# SPACENG
Capabilities:

-At the moment Spaceng affoards a way to communicate with graphic Hardware : mainly AMD and Nvidea using Vulkan SDK.

-Vulkan SDK is a Graphics API that provides a way to execute Shaders (SPIRV , GLSL , HLSL) on Graphics hardware.

-This allows the user of Spaceng to upload 2D-3D models and Textures (images) which are rendered on the GLFW API window Surface.

-An Event System is also implemented that allows for command-based rendering

-Spaceng has a feature function "RefreshTexture" which uses the Vulkan updatedescriptor method to refresh the texture rendered to the Surface
and therefore being able to create a video out of a sequence of textures by streaming the images to the  Graphics Hardware at a fixed framerate.

-A Client/Server Model has been implemented using asio networking sockets that allows for a system communication between multiple
Clients and a server , the Server/Client does succesfully send/receive data (image/Text Data at the moment).

prospects:

-Allows for Video Streaming between multiple users (Audio:to be implemented).

-3D Models Design or Gaming (scene Rendering : to be impelemented).

-Systems Intercommunication (Files transmition / chat-Server).

-IOT Applications (a work already in progress).

Next Steps:

-implementing Imgui "Graphics User Interface".
-Rendering / Networking internal Profiling.
-Creating a new Server Thread for each incoming Client.
-communicating with a microcontroller using Wifi , using the server/client handles on the MC.
-Sending Image Data from a camera Module.


How to Run the Project:
Supporting Visual Studio 17+ at the moment
- Win-GenProjects.bat in script folder.
- Run Spaceng.sln in ProjectDirectory
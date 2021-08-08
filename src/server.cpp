#include <glm/glm.hpp>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <functional>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <memory>
#include "server/netCommon.hpp"

using namespace std;
using asio::ip::tcp;

// Server
//
// accept client
// send id?
// receive glm::vec3 player position
// send chunks

// Client
//
// connect to server
// setup receiver callback
// receive id
// send position
// receive chunk

// void onMessage(Message msg) {
// 	printf(":P\n");
// }

class ClientHandler;

struct OwnedMessage {
	Message msg;
	std::shared_ptr<ClientHandler> owner = nullptr;
	OwnedMessage(Message msg, std::shared_ptr<ClientHandler> owner): msg{msg}, owner{owner} {}
};

class ClientHandler : public enable_shared_from_this<ClientHandler>, public Connection {
	// asio::ip::tcp::socket mclient;
	MsgHeader header {MsgType::Id, 0};
	uint32_t id;
	vector<uint16_t> t_chunk;
	TsQueue<OwnedMessage>* serverReceivedQueue = nullptr;
	// Connection conn;
public:
	ClientHandler(tcp::socket client, uint32_t id, TsQueue<OwnedMessage>* serverQueue): id{id}, Connection{std::move(client)}, serverReceivedQueue{serverQueue} {
		// printf("client connected: %s:%d\n", mclient.local_endpoint().address().to_string().c_str(), mclient.local_endpoint().port());
		printf("client made\n");
		genSampleChunk();
		connectToClient();
		sendId();
	}

	void genSampleChunk() {
		const int chunkSize = 16;
		vector<uint16_t> chunk(chunkSize * chunkSize * chunkSize);
		for (int z = 0; z < chunkSize; z++) {
			for (int y = 0; y < chunkSize; y++) {
				for (int x = 0; x < chunkSize; x++) {
					chunk[z * chunkSize * chunkSize + y * chunkSize + x] = 2 * (y < 8);
				}
			}
		}
		t_chunk = chunk;
	}

	void sendId() {
		Message msg;
		msg.setData<uint32_t>(id);
		msg.header.type = MsgType::Id;
		msg.header.size = msg.data.size();
		sendMessage(msg);
	}

	void sendChunk(glm::ivec3 chunkPosition) {
		Message msg;

		size_t chunkDataSize = t_chunk.size() * sizeof(unsigned short);
		size_t dataSize = chunkDataSize + sizeof(glm::ivec3);
		msg.data.resize(dataSize);
		printf("	Sending chunk size: %d %d\n", t_chunk.size(), msg.data.size());
		memcpy(msg.data.data(), &chunkPosition, sizeof(glm::ivec3));
		memcpy(msg.data.data() + sizeof(glm::ivec3), t_chunk.data(), chunkDataSize);

		msg.header.type = MsgType::Chunk;
		msg.header.size = dataSize; // should be region_dtype
		printf("header, data: %d %d\n", msg.header.size, msg.data.size());
		sendMessage(msg);
	}

	void addToReceivedQueue() {
		serverReceivedQueue->push(OwnedMessage(temp_message, shared_from_this()));
	}

	void onMessage(Message msg) {
		switch (msg.header.type) {
			case MsgType::ChunkRequest:
				glm::ivec3 position = msg.getData<glm::ivec3>();
				printf("Received chunk requested position: %2d, %2d, %2d\n", position.x, position.y, position.z);
				if (
					position.x >= -2 && position.x <= 2 &&
					position.y == 0 &&
					position.z >= -2 && position.z <= 2 
				) {
					sendChunk(position);
				}
			break;
		}
		// printf("message type: %d\n", msg.header.type);
	}

	~ClientHandler() {
		printf("seeya\n");
	}
};



class Server {
	tcp::acceptor acceptor;
	asio::io_context& context;

	vector<char> someText;
	vector<shared_ptr<ClientHandler>> clients;
	TsQueue<OwnedMessage> receivedMessages;
	bool running = true;
public:
	Server(asio::io_context& context): context{context}, acceptor{context, tcp::endpoint(tcp::v4(), 25013)} {
		someText = vector<char>(4);
		acceptClient();
	}

	void handleMessages() {
		while(running) {
			receivedMessages.wait();
			while (!receivedMessages.empty()) {
				OwnedMessage& omsg = receivedMessages.front();
				omsg.owner->onMessage(omsg.msg);
				receivedMessages.pop();
			}
		}
	}

	void acceptClient() {
		acceptor.async_accept([this] (error_code err, tcp::socket socket) {
			if(!err) {
				// Make sure this doesn't get removed after this function finishes.
				shared_ptr<ClientHandler> client = make_shared<ClientHandler>(std::move(socket), 12, &receivedMessages);
				clients.push_back(client);

				acceptClient(); // This is not infinite recursion.
								// Accepting is done in context.run(); (probably)
			} else {
				printf("Accepting client error: %s\n", err.message().c_str());
			}
			// thread t(ClientHandler(move(socket)));
		});
	}
};

int main() {
	// try {
		asio::io_context context;
		Server s(context);
		// acceptor = tcp::acceptor(context, tcp::endpoint(tcp::v4(), 16013));
		// acceptor.async_accept(accept_func);

		thread asioThread([&] {
			context.run();
		});
		
		s.handleMessages();


		// while(!context.stopped()) {
		// }

		printf("context stopped\n");

		// std::vector<char> buf(128);
		// for (;;) {
		// 	tcp::socket socket(context);
		// 	acceptor.accept(socket);
		// }
	// } catch (exception& e) {
	// 	printf("err: %s\n", e.what());
	// }

	return 0;
}
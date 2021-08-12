// Based on this
// https://github.com/OneLoneCoder/olcPixelGameEngine/tree/master/Videos/Networking/Parts1%262

#ifndef netCommon_hpp
#define netCommon_hpp

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <functional>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <queue>
#include <memory>

#include "netQueue.hpp"

using namespace std;
using asio::ip::tcp;

enum class MsgType : uint32_t {
	Id,
	Chunk,
	ChunkRequest,
	PlayerPosition,
	PlayerChunkPosition,
	BlockChange
};

struct MsgHeader {
	MsgType type;
	uint32_t size;
};

// uint32_t
// glm::vec3
// vector<unsinged short>
struct Message {
	MsgHeader header;
	vector<uint8_t> data;

	template<typename DataType>
	void setData(const DataType& d) {
		data.resize(sizeof(DataType));
		memcpy(data.data(), &d, sizeof(DataType));
	}

	template<typename DataType>
	DataType getData() {
		DataType t;
		memcpy(&t, data.data(), sizeof(DataType));
		return t;
	}
};

class Connection {
	tcp::socket socket;
	
	TsQueue<Message> sendQueue;
	bool server = false; // is this connection part of server
	// function<void(Message)> onMessage;
protected:
	Message temp_message;
	TsQueue<Message> receivedQueue;
public:
	Connection(tcp::socket socket): socket{move(socket)} {
		// printf("Connection made\n");
	}
	
	void connectToClient() {
		readHeader();
	}

	void connectToServer(tcp::resolver::results_type& endpoints) {
		asio::async_connect(socket, endpoints, 
			[&] (error_code err, asio::ip::tcp::endpoint endpoint) {
				if (!err) {
					// Get id from server.
					// receiveMessage();
					readHeader();
				} else {
					printf("Couldn't connect to server: %s\n", err.message().c_str());
				}
			}
		);
	}

	void sendMessage(const Message& msg) {
		bool was_empty = sendQueue.empty();
		sendQueue.push(msg);

		if (was_empty) {
			sendHeader();
		}
	}

	void sendHeader() {
		// header.type = MsgType::Chunk;
		// header.size = 0L;
		if (!sendQueue.empty()) {
			asio::async_write(socket, asio::buffer(&sendQueue.front().header, sizeof(MsgHeader)), 
				[this] (error_code err, size_t length) {
					if (!err) {
						// printf("sent header(type: %d, size: %d) %lu\n", header.type, header.size, length);
						sendBody();
					} else {
						printf("sending header error: %s\n", err.message().c_str());
						// printf("error header(type: %d, size: %d): %s\n", header.type, header.size, err.message().c_str());
					}
				}
			);
		}
	}

	void sendBody() {
		asio::async_write(socket, asio::buffer(sendQueue.front().data.data(), sendQueue.front().data.size()), 
			[this] (error_code err, size_t length) {
				if (!err) {
					// printf("sent header(type: %d, size: %d) %lu\n", header.type, header.size, length);
					// printf("Message sent(%d)[%d]\n", length, sendQueue.front().header.type);
					sendQueue.pop();
					if (!sendQueue.empty()) {
						sendHeader();
					}
				} else {
					printf("sending body error: %s\n", err.message().c_str());
					// printf("error header(type: %d, size: %d): %s\n", header.type, header.size, err.message().c_str());
				}
			}
		);
	}

	void readHeader() {
		asio::async_read(socket, asio::buffer(&temp_message.header, sizeof(MsgHeader)), 
			[this](error_code err, size_t length) {
				if (!err) {
					// printf("got header(type: %d, size: %d) %lu\n", temp_message.header.type, temp_message.header.size, length);
					temp_message.data.resize(temp_message.header.size);
					readBody();
				} else {
					printf("readHeader error: %s\n", err.message().c_str());
				}
			}
		);
	}

	void readBody() {
		// Resize buffer.

		asio::async_read(socket, asio::buffer(temp_message.data.data(), temp_message.data.size()),
			[&] (error_code err, size_t length) {
				if (!err) {
					addToReceivedQueue();
					// printf("Body received(%d)[%d]\n", length, temp_message.header.type);
					// onMessage(std::move(temp_message));
					readHeader();
				} else {
					printf("readBody error: %s\n", err.message().c_str());
				}
			}
		);
	}

	virtual void addToReceivedQueue() {
		if (server) {
		} else {
			receivedQueue.push(temp_message);
		}
	}

	// virtual void onMessage(Message msg) {}

	virtual ~Connection() {}
	// void addToReceivedQueue() {
	// 	// make it thread safe dude.
	// 	receivedQueue.push(temp_message);
	// 	readHeader();
	// }
};

#endif
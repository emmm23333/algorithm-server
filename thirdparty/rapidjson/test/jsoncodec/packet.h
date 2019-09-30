#ifndef PACKET_H_
#define PACKET_H_

#include <rapidjson/json_stream.h>

struct PacketHead
{
    uint32_t id;       // 包的ID
    uint32_t type;     // 包的类型
    uint32_t size;     // 包的长度, 含包头与包体
    uint32_t version;  // 协议版本号
    uint32_t ack;      // 返回值, 0表示成功
    uint32_t token;    // 令牌
};

template <typename T, int _type>
class Packet : public PacketHead, public rapidjson::JsonInterface
{
public:
	void print()
	{
		printf("id=%u\n", id);
		printf("type=%u\n", type);
		printf("size=%u\n", size);
		printf("version=%u\n", version);
		printf("ack=%u\n", ack);
		printf("token=%u\n", token);
		body.print();
	}
protected:
	void encode(rapidjson::JsonStreamType& strm, rapidjson::Value& json)
	{
		strm.encode("id", json, id);
		strm.encode("type", json, type);
		strm.encode("size", json, size);
		strm.encode("version", json, version);
		strm.encode("ack", json, ack);
		strm.encode("token", json, token);
		strm.encode("body", json, body);
	}
	void decode(rapidjson::JsonStreamType& strm, rapidjson::Value& json)
	{
		strm.decode("id", json, id);
		strm.decode("type", json, type);
		strm.decode("size", json, size);
		strm.decode("version", json, version);
		strm.decode("ack", json, ack);
		strm.decode("token", json, token);
		strm.decode("body", json, body);
	}
public:
    enum { kPacketType = _type };
    T body;
    Packet()
    {
        id   = 0;
        type = _type;
        size = 0;
        version = 0;
        ack = 0;
        token = 0;
    }

    Packet(const T& value)
    {
        id   = 0;
        type = _type;
        size = 0;
        body = value;
        version = 0;
        ack = 0;
        token = 0;
    }
};

#endif // PACKET_H_

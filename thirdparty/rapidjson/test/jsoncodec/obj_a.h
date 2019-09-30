#ifndef OBJ_A_H_
#define OBJ_A_H_

#include <type.h>
#include <packet.h>

class A : public rapidjson::JsonInterface
{
public:
	A() : bool_(false), char_(0), int8_t_(0), uint8_t_(0), int16_t_(0), uint16_t_(0), int32_t_(0), uint32_t_(0),
		int64_t_(0), uint64_t_(0), float_(0), double_(0), string_("") {}
	A(bool b, char c, int8_t i8, uint8_t u8, int16_t i16, uint16_t u16, int32_t i32, uint32_t u32, int64_t i64, uint64_t u64,
		float f, double d, std::string s) : bool_(b), char_(c), int8_t_(i8), uint8_t_(u8), int16_t_(i16), uint16_t_(u16),
		int32_t_(i32), uint32_t_(u32), int64_t_(i64), uint64_t_(u64), float_(f), double_(d), string_(s) {}
	void print()
	{
		printf("bool_=%s\n", bool_ ? "true" : "false");
		printf("char_=%c\n", char_);
		printf("int8_t_=%d\n", int8_t_);
		printf("uint8_t_=%u\n", uint8_t_);
		printf("int16_t_=%d\n", int16_t_);
		printf("uint16_t_=%u\n", uint16_t_);
		printf("int32_t_=%d\n", int32_t_);
		printf("uint32_t_=%u\n", uint32_t_);
		printf("int64_t_=%ld\n", int64_t_);
		printf("uint64_t_=%lu\n", uint64_t_);
		printf("float_=%f\n", float_);
		printf("double_=%lf\n", double_);
		printf("string_=%s\n", string_.c_str());
		printf("vector_char_=");
		for (size_t i=0; i<vector_char_.size(); ++i)
		{
			printf("%c ", vector_char_[i]);
		}
		printf("\nvector_string_=");
		for (size_t i=0; i<vector_string_.size(); ++i)
		{
			printf("%s ", vector_string_[i].c_str());
		}
		printf("\n");
	}
protected:
	void encode(rapidjson::JsonStreamType& strm, rapidjson::Value& json)
	{
		strm.encode("bool_", json, bool_);
		strm.encode("char_", json, char_);
		strm.encode("int8_t_", json, int8_t_);
		strm.encode("uint8_t_", json, uint8_t_);
		strm.encode("int16_t_", json, int16_t_);
		strm.encode("uint16_t_", json, uint16_t_);
		strm.encode("int32_t_", json, int32_t_);
		strm.encode("uint32_t_", json, uint32_t_);
		strm.encode("int64_t_", json, int64_t_);
		strm.encode("uint64_t_", json, uint64_t_);
		strm.encode("float_", json, float_);
		strm.encode("double_", json, double_);
		strm.encode("string_", json, string_);
		strm.encode("vector_char_", json, vector_char_);
		strm.encode("vector_string_", json, vector_string_);
	}
	void decode(rapidjson::JsonStreamType& strm, rapidjson::Value& json)
	{
		strm.decode("bool_", json, bool_);
		strm.decode("char_", json, char_);
		strm.decode("int8_t_", json, int8_t_);
		strm.decode("uint8_t_", json, uint8_t_);
		strm.decode("int16_t_", json, int16_t_);
		strm.decode("uint16_t_", json, uint16_t_);
		strm.decode("int32_t_", json, int32_t_);
		strm.decode("uint32_t_", json, uint32_t_);
		strm.decode("int64_t_", json, int64_t_);
		strm.decode("uint64_t_", json, uint64_t_);
		strm.decode("float_", json, float_);
		strm.decode("double_", json, double_);
		strm.decode("string_", json, string_);
		strm.decode("vector_char_", json, vector_char_);
		strm.decode("vector_string_", json, vector_string_);
	}

public:
	bool bool_;
	char char_;
	int8_t int8_t_;
	uint8_t uint8_t_;
	int16_t int16_t_;
	uint16_t uint16_t_;
	int32_t int32_t_;
	uint32_t uint32_t_;
	int64_t int64_t_;
	uint64_t uint64_t_;
	float float_;
	double double_;
	std::string string_;
	std::vector<char> vector_char_;
	std::vector<std::string> vector_string_;
};

typedef Packet<A, kSendA> PacketA;

#endif // OBJ_A_H_

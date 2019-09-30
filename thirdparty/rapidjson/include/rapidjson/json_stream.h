#ifndef RAPIDJSON_JSON_STREAM_H_
#define RAPIDJSON_JSON_STREAM_H_

#include <memory>
#include <string>
#include <vector>

#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

RAPIDJSON_NAMESPACE_BEGIN

class JsonInterface;
template<typename Allocator>
class JsonStream
{
public:
	JsonStream(std::shared_ptr<Allocator>& allocator)
	{
		assert(allocator != NULL);
		allocator_ = allocator;
	}

public:
	void encode(const char* name, Value& json, bool in);
	void encode(const char* name, Value& json, char in);
	void encode(const char* name, Value& json, int8_t in);
	void encode(const char* name, Value& json, uint8_t in);
	void encode(const char* name, Value& json, int16_t in);
	void encode(const char* name, Value& json, uint16_t in);
	void encode(const char* name, Value& json, int32_t in);
	void encode(const char* name, Value& json, uint32_t in);
	void encode(const char* name, Value& json, int64_t in);
	void encode(const char* name, Value& json, uint64_t in);
	void encode(const char* name, Value& json, float in);
	void encode(const char* name, Value& json, double in);
	void encode(const char* name, Value& json, std::string& in);
	void encode(const char* name, Value& json, JsonInterface& in);
	template<typename T>
	void encode(const char* name, Value& json, std::vector<T>& in);

	void decode(const char* name, Value& json, bool& out);
	void decode(const char* name, Value& json, char& out);
	void decode(const char* name, Value& json, int8_t& out);
	void decode(const char* name, Value& json, uint8_t& out);
	void decode(const char* name, Value& json, int16_t& out);
	void decode(const char* name, Value& json, uint16_t& out);
	void decode(const char* name, Value& json, int32_t& out);
	void decode(const char* name, Value& json, uint32_t& out);
	void decode(const char* name, Value& json, int64_t& out);
	void decode(const char* name, Value& json, uint64_t& out);
	void decode(const char* name, Value& json, float& out);
	void decode(const char* name, Value& json, double& out);
	void decode(const char* name, Value& json, std::string& out);
	void decode(const char* name, Value& json, JsonInterface& out);
	template<typename T>
	void decode(const char* name, Value& json, std::vector<T>& out);

private:
	void obj_2_json(Value& json, bool t) { json.SetBool(t); }
	void obj_2_json(Value& json, char t) { json.SetInt(t); }
	void obj_2_json(Value& json, int8_t t) { json.SetInt(t); }
	void obj_2_json(Value& json, uint8_t t) { json.SetUint(t); }
	void obj_2_json(Value& json, int16_t t) { json.SetInt(t); }
	void obj_2_json(Value& json, uint16_t t) { json.SetUint(t); }
	void obj_2_json(Value& json, int32_t t) { json.SetInt(t); }
	void obj_2_json(Value& json, uint32_t t) { json.SetUint(t); }
	void obj_2_json(Value& json, int64_t t) { json.SetInt64(t); }
	void obj_2_json(Value& json, uint64_t t) { json.SetUint64(t); }
	void obj_2_json(Value& json, float t) { json.SetFloat(t); }
	void obj_2_json(Value& json, double t) { json.SetDouble(t); }
	void obj_2_json(Value& json, std::string& t) { json.SetString(t.c_str(), t.size()); }
	void obj_2_json(Value& json, JsonInterface& t);

	void json_2_obj(Value& json, bool& t) { t = json.GetBool(); }
	void json_2_obj(Value& json, char& t) { t = json.GetInt(); }
	void json_2_obj(Value& json, int8_t& t) { t = json.GetInt(); }
	void json_2_obj(Value& json, uint8_t& t) { t = json.GetUint(); }
	void json_2_obj(Value& json, int16_t& t) { t = json.GetInt(); }
	void json_2_obj(Value& json, uint16_t& t) { t = json.GetUint(); }
	void json_2_obj(Value& json, int32_t& t) { t = json.GetInt(); }
	void json_2_obj(Value& json, uint32_t& t) { t = json.GetUint(); }
	void json_2_obj(Value& json, int64_t& t) { t = json.GetInt64(); }
	void json_2_obj(Value& json, uint64_t& t) { t = json.GetUint64(); }
	void json_2_obj(Value& json, float& t) { t = json.GetFloat(); }
	void json_2_obj(Value& json, double& t) { t = json.GetDouble(); }
	void json_2_obj(Value& json, std::string& t) { t = json.GetString(); }
	void json_2_obj(Value& json, JsonInterface& t);

private:
	std::shared_ptr<Allocator> allocator_;
};

typedef JsonStream<Document::AllocatorType> JsonStreamType;
class JsonInterface
{
	friend class JsonStream<Document::AllocatorType>;
public:
	virtual ~JsonInterface() {}
public:
	virtual bool serialize(std::string& out, std::string& errmsg)
	{
		StringBuffer strbuf;
		auto allocator = std::make_shared<Document::AllocatorType>();
		Writer<StringBuffer> writer(strbuf);

		JsonStreamType jstrm(allocator);
		Value ret_obj(kObjectType);
		try
		{
			encode(jstrm, ret_obj);
		}
		catch (const std::string& msg)
		{
			errmsg = msg;
			return false;
		}
		catch (...)
		{
			errmsg = "serialize error: unkown error";
			return false;
		}
		ret_obj.Accept(writer);
		out = strbuf.GetString();
		return true;
	}

	virtual bool deserialize(const std::string& in, std::string& errmsg)
	{
		Document document;
		if (document.Parse<0>(in.c_str()).HasParseError())
		{
			errmsg = "deserialize error: json parse fail";
			return false;
		}

		auto allocator = std::make_shared<Document::AllocatorType>();
		JsonStreamType jstrm(allocator);
		try
		{
			decode(jstrm, document);
		}
		catch (const std::string& msg)
		{
			errmsg = msg;
			return false;
		}
		catch (...)
		{
			errmsg = "deserialize error: unkown error";
			return false;
		}
		return true;
	}

protected:
	virtual void encode(JsonStreamType& strm, Value& json) = 0;
	virtual void decode(JsonStreamType& strm, Value& json) = 0;
};

#define POST_ENCODE(name, json, in) \
if (!json.IsObject()) \
{ \
	throw std::string("encode error: json is not a object, key is ") + name; \
} \
json.AddMember(StringRef(name), in, *allocator_);

#define PRE_DECODE(name, json) \
if (!(name && json.IsObject() && json.HasMember(name))) \
{ \
	throw std::string("decode error: json has not a key of ") + name; \
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, bool in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, char in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, int8_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, uint8_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, int16_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, uint16_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, int32_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, uint32_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, int64_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, uint64_t in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, float in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, double in)
{
	POST_ENCODE(name, json, in);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, std::string& in)
{
	Value t_json;
	t_json.SetString(in.c_str(), *allocator_);
	POST_ENCODE(name, json, t_json);
}

template<typename Allocator>
void JsonStream<Allocator>::encode(const char* name, Value& json, JsonInterface& in)
{
	Value t_json(kObjectType);
	in.encode(*this, t_json);
	POST_ENCODE(name, json, t_json);
}

template<typename Allocator>
template<typename T>
void JsonStream<Allocator>::encode(const char* name, Value& json, std::vector<T>& in)
{
	Value t_json(kArrayType);
	for (typename std::vector<T>::size_type i = 0; i < in.size(); ++i)
	{
		Value t;
		obj_2_json(t, in[i]);
		t_json.PushBack(t, *allocator_);
	}
	POST_ENCODE(name, json, t_json);
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, bool& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsBool())
	{
		throw "decode error: " + std::string(name) + " is not a bool type";
	}
	out = json[name].GetBool();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, char& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsInt())
	{
		throw "decode error: " + std::string(name) + " is not a char type";
	}
	out = json[name].GetInt();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, int8_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsInt())
	{
		throw "decode error: " + std::string(name) + " is not a int8_t type";
	}
	out = json[name].GetInt();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, uint8_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsUint())
	{
		throw "decode error: " + std::string(name) + " is not a uint8_t type";
	}
	out = json[name].GetUint();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, int16_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsInt())
	{
		throw "decode error: " + std::string(name) + " is not a int16_t type";
	}
	out = json[name].GetInt();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, uint16_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsUint())
	{
		throw "decode error: " + std::string(name) + " is not a uint16_t type";
	}
	out = json[name].GetUint();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, int32_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsInt())
	{
		throw "decode error: " + std::string(name) + " is not a int32_t type";
	}
	out = json[name].GetInt();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, uint32_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsUint())
	{
		throw "decode error: " + std::string(name) + " is not a uint32_t type";
	}
	out = json[name].GetUint();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, int64_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsInt64())
	{
		throw "decode error: " + std::string(name) + " is not a int64_t type";
	}
	out = json[name].GetInt64();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, uint64_t& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsUint64())
	{
		throw "decode error: " + std::string(name) + " is not a uint64_t type";
	}
	out = json[name].GetUint64();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, float& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsFloat())
	{
		throw "decode error: " + std::string(name) + " is not a float type";
	}
	out = json[name].GetFloat();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, double& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsDouble())
	{
		throw "decode error: " + std::string(name) + " is not a double type";
	}
	out = json[name].GetDouble();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, std::string& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsString())
	{
		throw "decode error: " + std::string(name) + " is not a string type";
	}
	out = json[name].GetString();
}

template<typename Allocator>
void JsonStream<Allocator>::decode(const char* name, Value& json, JsonInterface& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsObject())
	{
		throw "decode error: " + std::string(name) + std::string("is not a object type");
	}
	out.decode(*this, json[name]);
}

template<typename Allocator>
template<typename T>
void JsonStream<Allocator>::decode(const char* name, Value& json, std::vector<T>& out)
{
	PRE_DECODE(name, json);
	if (!json[name].IsArray())
	{
		throw "decode error: " + std::string(name) + " is not a array type";
	}
	for (SizeType i = 0; i < json[name].Size(); ++i)
	{
		T t;
		json_2_obj(json[name][i], t);
		out.push_back(t);
	}
}

template<typename Allocator>
void JsonStream<Allocator>::obj_2_json(Value& json, JsonInterface& t)
{
	json.SetObject();
	t.encode(*this, json);
}

template<typename Allocator>
void JsonStream<Allocator>::json_2_obj(Value& json, JsonInterface& t)
{
	t.decode(*this, json);
}

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_JSON_STREAM_H_

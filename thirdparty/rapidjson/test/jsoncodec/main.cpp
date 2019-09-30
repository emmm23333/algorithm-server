#include <obj_b.h>

int main()
{
	std::string out, err;

	PacketA pkg_a;
	pkg_a.id = 10;
	A a(true, '0', (int8_t)-127, (uint8_t)255, (int16_t)-32768, (uint16_t)65535, (int32_t)-2147483647, (uint32_t)4294967295,
		(int64_t)-9223372036854775807, (uint64_t)18446744073709551615, -2147483647, -9223372036854775807, "shunwang");
	pkg_a.body = std::move(a);
	pkg_a.body.vector_char_.push_back('A');
	pkg_a.body.vector_char_.push_back('B');
	pkg_a.body.vector_char_.push_back('C');
	pkg_a.body.vector_string_.push_back("hello");
	pkg_a.body.vector_string_.push_back("world");
	if (!pkg_a.serialize(out, err))
	{
		printf("A serialize err:%s\n", err.c_str());
	}
	printf("A serialize:\nout=%s\n", out.c_str());

	PacketA pkg_ad;
	if (!pkg_ad.deserialize(out, err))
	{
		printf("A deserialize err:%s\n", err.c_str());
	}
	printf("A deserialize:\n");
	pkg_ad.print();

	printf("------------------------------------------------------------------\n");

	PacketB pkg_b;
	pkg_b.id = 20;
	B b(true, '0', (int8_t)-127, (uint8_t)255, (int16_t)-32768, (uint16_t)65535, (int32_t)-2147483647, (uint32_t)4294967295,
		(int64_t)-9223372036854775807, (uint64_t)18446744073709551615, 4294967295, 18446744073709551615, "shunwang");
	pkg_b.body = std::move(b);
	pkg_b.body.vector_char_.push_back('A');
	pkg_b.body.vector_char_.push_back('B');
	pkg_b.body.vector_char_.push_back('C');
	pkg_b.body.vector_string_.push_back("hello");
	pkg_b.body.vector_string_.push_back("world");
	pkg_b.body.obj_ = pkg_a.body;
	for (int i=0; i<2; ++i)
	{
		pkg_b.body.vector_obj_.push_back(pkg_a.body);
	}
	if (!pkg_b.serialize(out, err))
	{
		printf("B serialize err:%s\n", err.c_str());
	}
	printf("B serialize:\nout=%s\n", out.c_str());

	PacketB pkg_bd;
	if (!pkg_bd.deserialize(out, err))
	{
		printf("B deserialize err:%s\n", err.c_str());
	}
	printf("B deserialize:\n");
	pkg_bd.print();
	return 0;
}

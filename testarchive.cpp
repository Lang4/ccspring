#include "ccarchive.h"
class OBJECT :public Archive<OBJECT>{
public:

	std::string b;
	DEC_ARCHIVE_OBJECT();
};

IMP_ARCHIVE_OBJECT(OBJECT)
{
	BIND_FIELD(b);
}

class TObject : public Archive<TObject>{
public:
	int a;
	OBJECT o;
	DEC_ARCHIVE_OBJECT();
};

IMP_ARCHIVE_OBJECT(TObject)
{
	BIND_FIELD(a);
	BIND_FIELD(o);
}




void testStream()
{
	TObject object;
	object.a = 10;
	object.o.b = "hello world 1111 [[]]{}{}  121[] jijijijin";
	IStream stream;
	stream << object;

	std::string str = stream.print();
	printf("%s\n", str.c_str());

	TObject o1;
	IStream istream;
	istream.parse(str);
	istream >> o1;
	printf("%d,%s", o1.a, o1.o.b.c_str());

}
 

 
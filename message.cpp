#include "Message.h"
fstream& operator >>(fstream& is, Message& obj)
{
	is >> obj._text;
	is >> obj._sender;
	is >> obj._receiver;
	return is;
}
ostream& operator <<(ostream& os, const Message& obj)
{
	os << obj._text;
	os << ' ';
	os << obj._sender;
	os << ' ';
	os << obj._receiver;
	return os;
}
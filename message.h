#pragma once
#include<iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class Message
{
	string _text;
	string _sender;
	string _receiver;
public:
	Message(string text, string sender, string receiver) :_text(text), _sender(sender), _receiver(receiver) {}

	friend fstream& operator >>(fstream& is, Message& obj);
	friend ostream& operator <<(ostream& os, const Message& obj);
	string get_text() { return _text; }
	string get_sender() { return _sender; }
	string get_receiver() { return _receiver; }
};
class Message_data
{
public:
	Message_data()
	{
		count = 0;
	}

	inline int count_Message() { return count; }

	void add_Message(Message new_Message)
	{
		count++;
		data.push_back(new_Message);
	}

	Message& operator[](int i) { return data[i]; }

protected:
	int count; // counter of Messages
	vector<Message> data;

};
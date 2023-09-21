#include <mysql.h>
#include <thread>
#include <mutex>
#include "chat_menu.h"


#include <cstdlib>
using namespace std;
#include <fstream>



int main()
{
	setlocale(LC_ALL, "rus");

	cout << "---------------------------------------" << endl;
	cout << "	Welcome to the chat app! " << endl;
	cout << "---------------------------------------\n" << endl;

	MYSQL mysql;

	int i = 0;

	// Getting the connection descriptor
	mysql_init(&mysql);
	if (&mysql == nullptr) {
		// If the descriptor is not received, we display an error message
		cout << "Error: can't create MySQL-descriptor" << endl;
	}
	// Connecting to the server
	if (mysql_real_connect(&mysql, "localhost", "root", "12345asdzxc12345-", NULL, 0, NULL, 0))
	{
		mysql_query(&mysql, "create database if not exists chatdb");
		mysql_select_db(&mysql, "chatdb");
		//cout << "Success!" << endl;
	}
	else
	{
		// If it is not possible to establish a connection to the database, we display an error message
		cout << "Error: can't connect to database " << mysql_error(&mysql) << endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	//Let's see if the encoding has changed to the desired one, the default is latin1
	//cout << "connection characterset: " << mysql_character_set_name(&mysql) << endl;


	//We make a request to the database to add a table with account data
	mysql_query(&mysql, "create table IF NOT EXISTS accs_data ("
		"id serial primary key,"
		"login character varying(255) NOT NULL,"
		"pass character varying(255) NOT NULL, "
		"name_of_user character varying(200) NOT NULL,"
		"second_name_of_user character varying(255),"
		"special_unique_name character varying(255) NOT NULL,"
		"email character varying(200) NOT NULL,"
		"online_status character varying(200) default 'online' CHECK(online_status IN('online', 'offline')),"
		"delete_status character varying(200) default 'active' CHECK(delete_status IN('active', 'deleted')))");


	//We make a request to the database to add a table with message data
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS messages ("
		"id serial primary key,"
		"message_text character varying(5000),"
		"id_sender integer REFERENCES accs_data(id),"
		"id_receiver integer default 0,"
		"date_and_time_of_message TIMESTAMP default CURRENT_TIMESTAMP,"
		"delete_status character varying(200) default 'active' CHECK (delete_status IN ('active', 'deleted')),"
		"sending_reading_status character varying(200) default 'not sent' CHECK (sending_reading_status IN ('not sent', 'delivered', 'read')))");


	while (true)
	{
		// Launching the login menu
		for_menu rezult = login_menu(mysql);  // in the rezult variable we store the id of the user who logged in successfully
		if (!rezult.res) break;	// if you exited the function with false, then exit the while loop and end the program

		// Launching the chat menu
		if (chat_menu(mysql, rezult.id)) break;		// if you left the function with false, then exit the while loop
	}

	return 0;
}
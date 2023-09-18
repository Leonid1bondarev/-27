#include <mysql.h>
#include <thread>
#include <mutex>
#include "Message.h"

#include <cstdlib>
using namespace std;
#include <fstream>
void clear_screen()
{
#ifdef _WIN32
	std::system("cls");
#else
	std::system("clear");
#endif
}

struct for_menu // structure to return value from menu
{
	string id;  // we know who logged in
	bool res = false; // this is if selection error \ number of inputs exceeded \ just exit
};

for_menu login_menu(MYSQL& mysql);
bool chat_menu(MYSQL& mysql, string id);



int main()
{
	setlocale(LC_ALL, "rus");

	cout << "---------------------------------------" << endl;
	cout << "	Welcome to the chat app! " << endl;
	cout << "---------------------------------------\n" << endl;

	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	int i = 0;

	// Получаем дескриптор соединения
	mysql_init(&mysql);
	if (&mysql == nullptr) {
		// Если дескриптор не получен — выводим сообщение об ошибке
		cout << "Error: can't create MySQL-descriptor" << endl;
	}
	// Подключаемся к серверу
	if (mysql_real_connect(&mysql, "localhost", "root", "12345asdzxc12345-", NULL, 0, NULL, 0))
	{
		mysql_query(&mysql, "create database if not exists chatdb");
		mysql_select_db(&mysql, "chatdb");
		//cout << "Success!" << endl;
	}
	else
	{
		// Если нет возможности установить соединение с БД выводим сообщение об ошибке
		cout << "Error: can't connect to database " << mysql_error(&mysql) << endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	//Смотрим изменилась ли кодировка на нужную, по умолчанию идёт latin1
	//cout << "connection characterset: " << mysql_character_set_name(&mysql) << endl;


	mysql_query(&mysql, "create table IF NOT EXISTS accs_data ("
		"id serial primary key,"
		"login character varying(255) NOT NULL,"
		"pass character varying(255) NOT NULL, "
		"name_of_user character varying(200) NOT NULL,"
		"second_name_of_user character varying(255),"
		"special_unique_name character varying(255) NOT NULL,"
		"email character varying(200) NOT NULL,"
		"online_status character varying(200) default 'online' CHECK(online_status IN('online', 'offline')),"
		"delete_status character varying(200) default 'active' CHECK(delete_status IN('active', 'deleted')))"); //Делаем запрос к таблице

	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS messages ("
		"id serial primary key,"
		"message_text character varying(5000),"
		"id_sender integer REFERENCES accs_data(id),"
		"id_receiver integer default 0,"
		"date_and_time_of_message TIMESTAMP default CURRENT_TIMESTAMP,"
		"delete_status character varying(200) default 'active' CHECK (delete_status IN ('active', 'deleted')),"
		"sending_reading_status character varying(200) default 'not sent' CHECK (sending_reading_status IN ('not sent', 'delivered', 'read')))"); //Делаем запрос к таблице


	while (true)
	{
		// Launching the login menu
		for_menu rezult = login_menu(mysql);  // в переменной rezult храним id пользователя, который залогинился успешно
		if (!rezult.res) break;	// if you exited the function with false, then exit the while loop and end the program

		// Launching the chat menu
		if (chat_menu(mysql, rezult.id)) break;		// if you left the function with false, then exit the while loop
	}

	return 0;
}

for_menu login_menu(MYSQL& mysql)
{
	for_menu rezult;

	MYSQL_RES* res;
	MYSQL_ROW row;

	bool first_while = true; //bool type to control the cycle
	while (first_while)
	{
		cout << "----------------------------------\n";
		cout << " Make a choice: " << endl
			<< "	1 - registration" << endl
			<< "	2 - log in to your account" << endl
			<< "	q - exit the chat " << endl;
		cout << "----------------------------------\n";


		char choice;
		cin >> choice;

		try
		{
			if (!(choice == '1' || choice == '2' || choice == 'q'))
			{
				throw "Error choice!";
			}

		}
		catch (const char* exception)
		{
			cout << exception << endl;
			continue;
		}

		if (choice == 'q')
		{
			return rezult;
		}

		switch (choice)

		{
		case '1':
		{
			string login;
			string pass;
			string first_name;
			string second_name;
			string special_unique_name;
			string email;
			bool log_busy = true;
			while (log_busy)
			{
				log_busy = false;
				cout << "Enter the login for your account: " << endl;
				cin >> login;
				string query = "SELECT login FROM accs_data where login = '" + login + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == login)         //login busy test
							{
								cout << "Login is busy\n";
								log_busy = true;
							}
							else
							{
								log_busy = false;
								cout << "Login created!\n";
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
			}

			cout << "Enter the password for your account: " << endl;
			while (true)
			{
				cin >> pass;

				if (pass.size() > 5) // password longer than 5 characters
				{
					cout << "pass created!\n";
					break;
				}
				else
				{
					cout << "Enter a password longer than 5 symbols!" << endl;
				}
			}

			cout << "Come up with a first name for your account: " << endl;
			cin >> first_name;

			cout << "Come up with a second name for your account: " << endl;
			cin >> second_name;

			bool special_unique_name_busy = true;
			while (special_unique_name_busy)
			{
				special_unique_name_busy = false;
				cout << "Enter the special_unique_name for your account: " << endl;
				cin >> special_unique_name;
				string query = "SELECT special_unique_name FROM accs_data where special_unique_name = '" + special_unique_name + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == special_unique_name)         //special_unique_name busy test
							{
								cout << "special_unique_name is busy\n";
								special_unique_name_busy = true;
							}
							else
							{
								special_unique_name_busy = false;
								cout << "special_unique_name created!\n";
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
			}

			bool email_busy = true;
			while (email_busy)
			{
				email_busy = false;
				cout << "Enter the email for your account: " << endl;
				cin >> email;
				string query = "SELECT email FROM accs_data where email = '" + email + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == email)         //email busy test
							{
								cout << "email is busy\n";
								email_busy = true;
							}
							else
							{
								email_busy = false;
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
			}
			string query = "INSERT ignore INTO accs_data(id,login, pass, name_of_user, second_name_of_user, special_unique_name, email)"
				"values(default, '" + login + "', '" + pass + "', '" + first_name + "', '" + second_name + "', '" + special_unique_name + "', '" + email + "')";


			mysql_query(&mysql, query.c_str()); //added the created account
			cout << "Account created!\n";
			cout << "-------------------\n";
		}
		case '2':
		{
			string login;
			string pass;

			bool noAccs;
			string id = "0";
			mysql_query(&mysql, "SELECT COUNT(*) FROM accs_data");

			if (res = mysql_store_result(&mysql))
			{
				while (row = mysql_fetch_row(res))
				{
					for (int i = 0; i < mysql_num_fields(res); i++)
					{
						if (row[i] == id) // if there is no account, then we will ask you to restart the chat and create an account
						{
							cout << "There is not a single registration! Register to get started!" << endl;
							noAccs = true;
						}
						else
						{
							noAccs = false;
						}
					}
				}
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);

			if (noAccs)
			{
				break;
			}

			int attempt = 2;//input attempts counter
			bool enter_log = true;
			while (enter_log)
			{
				cout << "Enter login: " << endl;
				cin >> login;
				string query = "SELECT login FROM accs_data where login = '" + login + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == login)         //login verification
							{
								enter_log = false;      //to continue work of programm
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
				if (enter_log)
				{
					if (attempt == 0) return rezult;
					cout << "Wrong login! There are still attempts left: " << attempt-- << endl;
				}
			}

			attempt = 2;
			bool enter_pass = true;
			while (enter_pass)
			{
				cout << "Enter password: " << endl;;
				cin >> pass;
				string query = "SELECT pass FROM accs_data where login = '" + login + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == pass)         //pass verification
							{
								rezult.res = true;   //to continue work of programm
								first_while = false; //to continue work of programm
								enter_pass = false;     //to continue work of programm
								clear_screen();
								cout << "Authentication was successful!" << endl;
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
				if (enter_pass)
				{
					if (attempt == 0) return rezult;
					cout << "Wrong password! There are still attempts left: " << attempt-- << endl;
				}
			}
			string query = "SELECT id FROM accs_data where login = '" + login + "'";
			mysql_query(&mysql, query.c_str());
			if (res = mysql_store_result(&mysql))
			{
				while (row = mysql_fetch_row(res))
				{
					for (int i = 0; i < mysql_num_fields(res); i++)
					{
						id = row[i];
					}
				}
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);
			rezult.id = id;
			break;
		}

		default:
		{
			cout << "Wrong choice " << endl;
			break;
		}
		}

	}
	return rezult;
}



bool chat_menu(MYSQL& mysql, string id)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	string zero = "0"; //for checking count of message in next part of code(switch '1' and '2'

	while (true)
	{
		cout << "\n------------------------------------\n";
		cout << "Select one of following actions: " << endl
			<< "1 - View received messages " << endl
			<< "2 - View sent messages " << endl
			<< "3 - Write a new letter " << endl
			<< "4 - Write a new letter to all members of chat " << endl
			<< "q - Sign out" << endl;
		cout << "------------------------------------\n";

		char choice;
		cin >> choice;

		try
		{
			if (!(choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == 'q'))
			{
				throw "Error choice!";
			}

		}
		catch (const char* exception)
		{
			cout << exception << endl;
			continue;
		}


		if (choice == 'q')
		{
			cout << "You are logged out! " << endl;
			return false;
		}

		switch (choice)
		{
		case '1':
		{
			clear_screen();
			cout << "You have received the following messages: " << endl;
			bool have_rec_mess = false;

			string query = "SELECT COUNT(*) FROM messages where id_receiver = '" + id + "'";
			mysql_query(&mysql, query.c_str());
			if (res = mysql_store_result(&mysql))
			{
				while (row = mysql_fetch_row(res))
				{
					for (int i = 0; i < mysql_num_fields(res); i++)
					{
						if (row[i] == zero)
						{
							have_rec_mess = false;
						}
						else
						{
							have_rec_mess = true;
						}
					}
				}
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);

			if (!have_rec_mess)
			{
				cout << "No messages received!" << endl;
			}
			else
			{
				string query = "SELECT mess.message_text, acc.special_unique_name, mess.date_and_time_of_message FROM messages as mess join accs_data as acc on acc.id = mess.id_receiver where mess.id_receiver = '" + id + "' or mess.id_receiver = '" + zero + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							cout << row[i] << " ";
						}
						cout << endl;
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
			}
			break;
		}
		case '2':
		{
			clear_screen();
			cout << "You sent the following messages: " << endl;
			bool have_send_mess = false;
			string query = "SELECT COUNT(*) FROM messages where id_sender = '" + id + "'";
			mysql_query(&mysql, query.c_str());
			if (res = mysql_store_result(&mysql))
			{
				while (row = mysql_fetch_row(res))
				{
					for (int i = 0; i < mysql_num_fields(res); i++)
					{
						if (row[i] == zero)
						{
							have_send_mess = false;
						}
						else
						{
							have_send_mess = true;
						}
					}
				}
			}
			else
				cout << "Ошибка MySql номер " << mysql_error(&mysql);

			if (!have_send_mess)
			{
				cout << "No messages sent!" << endl;
			}
			else
			{
				string query = "SELECT mess.message_text, acc.special_unique_name, mess.date_and_time_of_message FROM messages as mess join accs_data as acc on acc.id = mess.id_sender where mess.id_sender = '" + id + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							cout << row[i] << " ";
						}
						cout << endl;
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
			}
			break;
		}
		case '3':
		{
			int attempt = 2;
			bool special_unique_name_is_actual = false;
			string special_unique_name;
			string id_reciever;
			while (true)
			{
				cout << "Enter the special_unique_name you want to write to: " << endl; // user special_unique_name are not repeated, so we can send either by special_unique_name
				cin >> special_unique_name;
				string query = "SELECT special_unique_name FROM accs_data where special_unique_name = '" + special_unique_name + "'";
				mysql_query(&mysql, query.c_str());
				if (res = mysql_store_result(&mysql))
				{
					while (row = mysql_fetch_row(res))
					{
						for (int i = 0; i < mysql_num_fields(res); i++)
						{
							if (row[i] == special_unique_name)
							{
								special_unique_name_is_actual = true;
							}
						}
					}
				}
				else
					cout << "Ошибка MySql номер " << mysql_error(&mysql);
				if (special_unique_name_is_actual)         //occupancy test name
				{
					string msg; //message
					cout << "Enter you massage: " << endl;
					cin.ignore();

					getline(cin, msg);


					// find out id
					string query = "SELECT id FROM accs_data where special_unique_name = '" + special_unique_name + "'";
					mysql_query(&mysql, query.c_str());
					if (res = mysql_store_result(&mysql))
					{
						while (row = mysql_fetch_row(res))
						{
							for (int i = 0; i < mysql_num_fields(res); i++)
							{
								id_reciever = row[i];
							}
						}
					}
					Message_data messages;
					fstream message_file = fstream("messages.txt", ios::in | ios::out);
					if (!message_file)
						message_file = fstream("messages.txt", ios::in | ios::out | ios::trunc);
					if (message_file)
					{
						Message new_Message(msg, id, id_reciever);
						messages.add_Message(new_Message);
						std::mutex m;
						m.lock();
						ofstream b_file("messages.txt", ios::app);
						b_file << endl;
						b_file << new_Message;
						m.unlock();
					}
					else
					{
						cout << "Could not open file messages.txt !" << '\n';
					}

					// save in received from the recipient
					query = "insert ignore into messages(message_text, id_sender, id_receiver) values('" + msg + "', '" + id + "', '" + id_reciever + "')";
					mysql_query(&mysql, query.c_str());

					break;
				}
				else
				{

					if (attempt == 0)
					{
						clear_screen();
						break;

					}
					cout << "User with this name not found!\nThere are still attempts left: " << attempt-- << endl;
				}
			}
			break;
		}
		case '4':
		{
			string msg; //message
			cout << "Enter you massage: " << endl;
			cin.ignore();
			getline(cin, msg);

			string query = "insert ignore into messages(message_text, id_sender) values('" + msg + "', '" + id + "')";
			mysql_query(&mysql, query.c_str());
			clear_screen();
			break;
		}
		default:
		{
			cout << "Operator specified incorrectly" << endl;
			break;
		}
		}
	}

	return true;
}
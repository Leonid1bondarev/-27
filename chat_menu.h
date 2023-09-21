#pragma once
#include "login_menu.h"

bool chat_menu(MYSQL& mysql, string id)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	string zero = "0"; //for checking count of message in next part of code(switch '1' and '2')

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
				string query = "SELECT mess.message_text, sender.special_unique_name, mess.date_and_time_of_message FROM messages as mess join accs_data as receiver on receiver.id = mess.id_receiver join accs_data as sender on sender.id = mess.id_sender where mess.id_receiver = '" + id + "' or mess.id_receiver = '" + zero + "'";
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
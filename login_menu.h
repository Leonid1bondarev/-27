#pragma once
#include <mysql.h>
#include "message.h"

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
			//create variables to add parameters to the accounts table
			string login;
			string pass;
			string first_name;
			string second_name;
			string special_unique_name;
			string email;
			bool log_busy = true;  //bool type to control the cycle
			while (log_busy)
			{
				log_busy = false;
				cout << "Enter the login for your account: " << endl;
				cin >> login;
				string query = "SELECT login FROM accs_data where login = '" + login + "'";
				mysql_query(&mysql, query.c_str()); //we make a query to the table to check it for the presence of the entered data
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

			bool special_unique_name_busy = true;  //bool type to control the cycle
			while (special_unique_name_busy)
			{
				special_unique_name_busy = false;
				cout << "Enter the special_unique_name for your account: " << endl;
				cin >> special_unique_name;
				string query = "SELECT special_unique_name FROM accs_data where special_unique_name = '" + special_unique_name + "'";
				mysql_query(&mysql, query.c_str()); //we make a query to the table to check it for the presence of the entered data
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

			bool email_busy = true;  //bool type to control the cycle
			while (email_busy)
			{
				email_busy = false;
				cout << "Enter the email for your account: " << endl;
				cin >> email;
				string query = "SELECT email FROM accs_data where email = '" + email + "'";
				mysql_query(&mysql, query.c_str());  //we make a query to the table to check it for the presence of the entered data
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
			//make a query to the table to check if there is data in it
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
			bool enter_log = true;  //bool type to control the cycle
			while (enter_log)
			{
				cout << "Enter login: " << endl;
				cin >> login;
				string query = "SELECT login FROM accs_data where login = '" + login + "'";
				mysql_query(&mysql, query.c_str());  //we make a query to the table to check it for the presence of the entered data
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
			mysql_query(&mysql, query.c_str());  //taking the ID from the account from which the login was made
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

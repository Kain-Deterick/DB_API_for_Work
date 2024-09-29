#include "MySQL_DB.h"

MySQL_API::MySQL_API()
{
	try
	{
		Session sess("localhost", 33060, user_name, password);
	}
	catch (const mysqlx::Error& err)
	{
		std::cout << "ERROR: " << err << std::endl;
	}
}

int MySQL_API::get_table(std::string nameTable)
{

}


  
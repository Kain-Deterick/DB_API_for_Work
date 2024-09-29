#pragma once
#ifndef MySQL_DB_H
#define MySQL_DB_H

//#ifdef _MSC_VER
//// Если используется Microsoft Visual C++
//#ifdef MY_DLL_BUILD
//// Если файл подключается из проекта библиотеки
//#define MySQL_DB_API __declspec (dllexport)
//#else // MY_DLL_BUILD
//// Если файл подключается из проекта клиента
//#define MySQL_DB_API __declspec (dllimport)
//#endif // MY_DLL_BUILD (else)
//#else // _MSC_VER
//// Если используется любой другой компилятор
//#define MySQL_DB_API
//#endif // _MSC_VER (else)


#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>

#include "immutable_variables.h"

using namespace mysqlx;

class MySQL_API
{
public:
	MySQL_API();

	int get_table(std::string nameTable);
private:
};

#endif //DATABASEAPI_H
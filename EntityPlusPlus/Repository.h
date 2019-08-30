#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <typeinfo>
#include <sql.h>
#include <typeinfo>
#include <string>
#include "TableStruct.h"

using namespace std;

#pragma once
class Repository
{
#define SQL_RETURN_CODE_LEN 2048
#define	SQL_RESULT_LEN 240

private:
	SQLHANDLE sqlConnHandle;
	SQLHANDLE sqlStmtHandle;
	SQLHANDLE sqlEnvHandle;
	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
	SQLWCHAR* connetionString;

private:
	void CloseConnection();
	void OpenConnection();
	std::string GetTableName(const char* className);
	std::wstring GetQueryColumnsNames(const char* className);
	TableStruct GetTableStruct(const char* className);

public:
	Repository();

	template<class Entity>
	void ExecuteQuery()
	{
		TableStruct tableStruct;
		tableStruct = this->GetTableStruct(typeid(Entity).name());
		cout << tableStruct.jsonStruct;
		cout << '\n';
		cout << tableStruct.columnsNumber;
	}
};
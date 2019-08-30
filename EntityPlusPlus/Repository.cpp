#include "Repository.h"
#include "TableStruct.h"

Repository::Repository()
{
	this->sqlConnHandle = NULL;
	this->sqlStmtHandle = NULL;
	this->sqlEnvHandle = NULL;
	this->retconstring[SQL_RETURN_CODE_LEN] = NULL;

	// Connection string with user and password
	this->connetionString = (SQLWCHAR*)L"DRIVER=SQL Server;SERVER=localhost, portNumber;DATABASE=master;UID=user;PWD=password;";

	// Connection string with trusted connection
	//this->connetionString = (SQLWCHAR*)L"DRIVER=SQL Server;SERVER=localhost, portNumber;DATABASE=master;Trusted_Connection=yes;";
}

void Repository::CloseConnection()
{
	SQLFreeHandle(SQL_HANDLE_STMT, this->sqlStmtHandle);
	SQLDisconnect(this->sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, this->sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, this->sqlEnvHandle);
}

void Repository::OpenConnection()
{
	int resultAllocation = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->sqlEnvHandle);
	resultAllocation = SQLSetEnvAttr(this->sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	resultAllocation = SQLAllocHandle(SQL_HANDLE_DBC, this->sqlEnvHandle, &this->sqlConnHandle);

	if (resultAllocation != SQL_SUCCESS)
		this->CloseConnection();

	SQLDriverConnect(this->sqlConnHandle, NULL, this->connetionString, SQL_NTS, this->retconstring, 2048, NULL, SQL_DRIVER_NOPROMPT);

	resultAllocation = SQLAllocHandle(SQL_HANDLE_STMT, this->sqlConnHandle, &this->sqlStmtHandle);

	if (resultAllocation != SQL_SUCCESS)
		this->CloseConnection();
}

std::string Repository::GetTableName(const char* className)
{
	std::string classNameString(className);
	std::string tableNameString = classNameString.substr(6, 200);

	return tableNameString;
}

std::wstring Repository::GetQueryColumnsNames(const char* className)
{
	std::string tableNameString = this->GetTableName(className) + "'";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &tableNameString[0], (int)tableNameString.size(), NULL, 0);
	std::wstring tableName(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &tableNameString[0], (int)tableNameString.size(), &tableName[0], size_needed);

	wstring queryColumnsNames(L"SELECT COLUMN_NAME FROM information_schema.columns WHERE table_name = '");
	queryColumnsNames = queryColumnsNames + tableName;

	return queryColumnsNames;
}

TableStruct Repository::GetTableStruct(const char* className)
{
	SQLINTEGER ptrCountColumns;
	SQLSCHAR countColumns[SQL_RESULT_LEN] = "";
	TableStruct result;

	wstring queryCountColumnsString = this->GetQueryColumnsNames(className);
	SQLWCHAR* queryCountColumns = (SQLWCHAR*)queryCountColumnsString.c_str();

	this->OpenConnection();

	int resultExecution = SQLExecDirect(sqlStmtHandle, queryCountColumns, SQL_NTS);
	if (resultExecution != SQL_SUCCESS)
	{
		this->CloseConnection();
		return result;
	}

	int columnNumber = 1;
	while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
	{
		SQLGetData(sqlStmtHandle, 1, SQL_CHAR, countColumns, SQL_RESULT_LEN, &ptrCountColumns);
		string data((char*)countColumns);

		result.jsonStruct = result.jsonStruct + data + ": {" + std::to_string(columnNumber) + "}, ";
		columnNumber++;
	}

	this->CloseConnection();

	std::size_t found = result.jsonStruct.find_last_of(",");
	result.jsonStruct = result.jsonStruct.substr(0, found);
	result.jsonStruct = "{ " + result.jsonStruct + " }";
	result.columnsNumber = columnNumber - 1;

	return result;
}
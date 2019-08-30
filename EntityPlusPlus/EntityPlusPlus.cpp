#include <iostream>
#include <windows.h>
#include <sqltypes.h>
#include "Repository.h"

using namespace std;

class SystemParameter
{
public:
	SystemParameter();
	~SystemParameter();

private:

};

SystemParameter::SystemParameter()
{
}

SystemParameter::~SystemParameter()
{
}

int main()
{
	Repository repo;
	repo.ExecuteQuery<SystemParameter>();
}
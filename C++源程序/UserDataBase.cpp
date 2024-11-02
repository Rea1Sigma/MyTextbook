#include "UserDataBase.h"

UserData::UserData(LPTSTR Account_in, LPTSTR Password_in)
{
	Account = TEXT("");
	Password = TEXT("");
	Set_UserData_Account(Account_in);
	Set_UserData_Password(Password_in);
	return;
}

void UserData::Set_UserData_Account(LPTSTR Account_Name)
{
	Account = Account_Name;
	return;
}

void UserData::Set_UserData_Password(LPTSTR Account_Password)
{
	Password = Account_Password;
	return;
}

LPTSTR UserData::Get_Account() 
{
	return Account;
}

LPTSTR UserData::Get_Password() 
{
	return Password;
}

UserData::~UserData()
{
	return;
}


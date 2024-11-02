#pragma once
#include "BForm.h"
#include <vector>

class UserData {
public:
	UserData(LPTSTR Account_in, LPTSTR Password_in);
	void Set_UserData_Account(LPTSTR Account_Name);
	void Set_UserData_Password(LPTSTR Account_Password);
	LPTSTR Get_Account();
	LPTSTR Get_Password();
	~UserData();
private:
	LPTSTR Account;
	LPTSTR Password;
protected:
};
//////////////////////////////////////////////////////////////////////
// mdlRegOperation.cpp: ʵ��ע��������ģ�麯��ʵ��
//
// ֧�֣�
//     ��Ҫ BWindows ģ���֧��
//////////////////////////////////////////////////////////////////////

#include "mdlRegOperation.h"


// ��������·���Ļ�������ÿ�� MakeSubKey ���޸Ĵ�ֵΪ����·���ַ���
TCHAR m_szSubKeyBuff[2048];	

// ���ַ�������
TCHAR m_szEmptyString[2]={0};	

// �������� szSubKey ��Ĭ��·��
// ������·��Ϊ��Software\<mcSubKey>\<szSubKey>
LPTSTR MakeSubKey(LPCTSTR szSubKey)
{
	*m_szSubKeyBuff = 0;
	_tcscpy(m_szSubKeyBuff, TEXT("Software\\"));
	_tcscat(m_szSubKeyBuff, mcSubKey);
	if ( *m_szSubKeyBuff )	// �� m_szSubKeyBuff ��Ϊ�մ�
		_tcscat(m_szSubKeyBuff, TEXT("\\"));	// ����ټ� ��\��
	_tcscat(m_szSubKeyBuff, szSubKey);	// ���������ַ���
	return m_szSubKeyBuff; 
}

// �� <mcRootKey>\<szSubKey> ���潨��һ������
// szSubKey �����Ƕ༶·������һ���Խ���·���е����������"Software\abc\de\fg"
bool RegCreate(LPCTSTR szSubKey, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	HKEY hKey; 
	LONG ret = RegCreateKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
	  0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	RegCloseKey(hKey);

	return (ret==ERROR_SUCCESS);
}


// ɾ�� <mcRootKey>\Software\<mcSubKey> �����һ������
// szSubKey �����Ƕ༶·������ֻɾ�����һ������
// ��� subKey ��ָ�������һ�������л�������һ��������ܱ�ɾ��
//   ֻ�в�������һ�������������ܱ�ɾ��
bool RegDel(LPCTSTR szSubKey, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	return RegDeleteKey((HKEY)eRootKey, MakeSubKey(szSubKey))==ERROR_SUCCESS; 
}

// ��ȡע����ַ������͵ļ�ֵ���ɺ������أ��������������""
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// ����ַ����ռ��ɱ�ģ���Զ����٣��� HM �Զ�����
LPTSTR RegGetValueString(LPCTSTR szSubKey, LPCTSTR szKeyName, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	LPTSTR szResult = NULL;		// ����ַ���ָ��
	LONG ret;

	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
		0, KEY_ALL_ACCESS64, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		*m_szEmptyString = 0;
		return m_szEmptyString;	// ���ؿ��ַ���
	}
	
	// ��������м�ֵ����Ϊ keyName �ļ�ֵ���������ͺͻ���������
	DWORD dwType=0, dwLen=0; 
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, NULL, &dwLen);
	if (dwLen<0 || ret!= ERROR_SUCCESS)
		{ *m_szEmptyString = 0; return m_szEmptyString;	} // ���ؿ��ַ���
	// ׼�������� szResult
	szResult = new TCHAR[dwLen+1];
	HM.AddPtr(szResult);
	// �û����� szResult ��ȡ��ֵ
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, (LPBYTE)szResult, &dwLen);
	if (ret != ERROR_SUCCESS) 
		{ *m_szEmptyString = 0; return m_szEmptyString;	} // ���ؿ��ַ���
	// �ر�������
	RegCloseKey(hKey);		
	// �����ַ���
	return szResult;
}


// ��ȡע����������͵ļ�ֵ���ɺ������أ��������������0
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
long RegGetValueLong(LPCTSTR szSubKey, LPCTSTR szKeyName, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	long lResult=0;
	LONG ret;

	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
		0, KEY_ALL_ACCESS64, &hKey);
	if (ret != ERROR_SUCCESS) return 0;	// ���� 0 ��ʾʧ��

	// ��ȡ�������ļ�ֵ���� => dwType������ => dwLen
	DWORD dwType=0, dwLen=0; 
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, NULL, &dwLen);

	// ��ȡ�������ļ�ֵ������ lResult
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, (LPBYTE)&lResult, &dwLen);
	if (ret != ERROR_SUCCESS) lResult=0;	// ����׼������ 0
	
	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ lResult
	return lResult;
}


// ��һ������������һ���ַ����͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ szValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueString(LPCTSTR szSubKey, LPCTSTR szKeyName, LPCTSTR szValue, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	LONG ret;
	
	// ������� RegCreateKeyEx �򿪣�����������򴴽���
	HKEY hKey=NULL; 
	ret = RegCreateKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
	  0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	if (ret != ERROR_SUCCESS) return false;	// ���� false ��ʾʧ��

	// ���������ֵ
	ret = RegSetValueEx(hKey, szKeyName, 0, REG_SZ, 
		(CONST BYTE *)szValue, (lstrlen(szValue)+1) * sizeof(TCHAR) );
	
	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}

// ��һ������������һ���������͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ lValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueLong(LPCTSTR szSubKey, LPCTSTR szKeyName, long lValue, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	LONG ret;
	
	// ������� RegCreateKeyEx �򿪣�����������򴴽���
	HKEY hKey=NULL; 
	ret = RegCreateKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
		0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	if (ret != ERROR_SUCCESS) return false;	// ���� false ��ʾʧ��
	
	// ���������ֵ
	ret = RegSetValueEx(hKey, szKeyName, 0, REG_DWORD, 
		(CONST BYTE *)&lValue, sizeof(lValue) );
	
	// �ر�������
	RegCloseKey(hKey);
	
	// ����ֵ
	return ret==ERROR_SUCCESS;	
}


// ��һ������������һ���������͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <eRootKey>\software\<mcSubKey>\subKey
// ��ֵ��Ϊ szKeyName��="" ��ʾĬ�ϼ�ֵ����ֵΪ pBuff ��ַ��ʼ�ġ�iBuffLen ���ֽ�
// �ɹ����� True��ʧ�ܷ��� False
bool RegSetValueBin( LPCTSTR szSubKey, LPCTSTR szKeyName, const char * pBuff, const int iBuffLen, ERegControlKeys eRootKey/*=mcRootKey*/ )
{
	LONG ret;

	// ������� RegCreateKeyEx �򿪣�����������򴴽���
	HKEY hKey=NULL; 
	ret = RegCreateKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
		0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	if (ret != ERROR_SUCCESS) return false;	// ���� false ��ʾʧ��

	// ���������ֵ
	ret = RegSetValueEx(hKey, szKeyName, 0, REG_BINARY, 
		(CONST BYTE *)pBuff, (DWORD)iBuffLen );

	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}


// ɾ�� szSubKey �����µ�һ��ע����ֵ
bool RegDelValue(LPCTSTR szSubKey, LPCTSTR szKeyName, ERegControlKeys eRootKey/*=mcRootKey*/)
{
	LONG ret;
	
	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)eRootKey, MakeSubKey(szSubKey), 
		0, KEY_ALL_ACCESS64, &hKey);
	if (ret != ERROR_SUCCESS) return 0;	// ���� 0 ��ʾʧ��

	// ɾ����ֵ
	ret = RegDeleteValue(hKey, szKeyName);

	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}


// ɾ�� ע��������λ�� �����µ�һ��ע����ֵ
bool RegDelValueCtrl( ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName )
{
	LONG ret;

	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)controlKey, szSubKey, 
		0, KEY_ALL_ACCESS64, &hKey);
	if (ret != ERROR_SUCCESS) return 0;	// ���� 0 ��ʾʧ��

	// ɾ����ֵ
	ret = RegDeleteValue(hKey, szKeyName);

	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}

// ɾ�� ע��������λ�� ��<controlKey>\<szSubKey> �����һ������
// szSubKey �����Ƕ༶·������ֻɾ�����һ������
// ��� subKey ��ָ�������һ�������л�������һ��������ܱ�ɾ��
//   ֻ�в�������һ�������������ܱ�ɾ��
bool RegDelCtrl( ERegControlKeys controlKey, LPCTSTR szSubKey )
{
	return RegDeleteKey((HKEY)controlKey, szSubKey)==ERROR_SUCCESS; 
}


// �� ע��������λ�� ��ȡ�ַ������͵ļ�ֵ���ɺ������أ��������������""
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// ����ַ����ռ��ɱ�ģ���Զ����٣��� HM �Զ�����
LPTSTR RegGetValueStringCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName)
{
	LPTSTR szResult = NULL;		// ����ַ���ָ��
	LONG ret;
	
	// �������ʹ�ö���ֵ����Ȩ�� KEY_QUERY_VALUE
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)controlKey, szSubKey, 
		0, KEY_QUERY_VALUE | 256, &hKey);	// | 256������ 64 λ����ϵͳ
	if (ret != ERROR_SUCCESS)
	{
		*m_szEmptyString = 0;
		return m_szEmptyString;	// ���ؿ��ַ���
	}
	
	// ��������м�ֵ����Ϊ keyName �ļ�ֵ���������ͺͻ���������
	DWORD dwType=0, dwLen=0; 
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, NULL, &dwLen);
	if (dwLen<0 || ret!= ERROR_SUCCESS)
		{ *m_szEmptyString = 0; return m_szEmptyString;	} // ���ؿ��ַ���
	// ׼�������� szResult
	szResult = new TCHAR[dwLen+1];
	HM.AddPtr(szResult);
	// �û����� szResult ��ȡ��ֵ
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, (LPBYTE)szResult, &dwLen);
	if (ret != ERROR_SUCCESS) 
		{ *m_szEmptyString = 0; return m_szEmptyString;	} // ���ؿ��ַ���
	// �ر�������
	RegCloseKey(hKey);		
	// �����ַ���
	return szResult;
}


// �� ע��������λ�� ��ȡ���������͵ļ�ֵ���ɺ������أ��������������0
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
long RegGetValueLongCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName)
{
	long lResult=0;
	LONG ret;
	
	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)controlKey, szSubKey, 
		0, KEY_QUERY_VALUE | 256, &hKey);	// | 256������ 64 λ����ϵͳ
	if (ret != ERROR_SUCCESS) return 0;		// ���� 0 ��ʾʧ��
	
	// ��ȡ�������ļ�ֵ���� => dwType������ => dwLen
	DWORD dwType=0, dwLen=0; 
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, NULL, &dwLen);
	
	// ��ȡ�������ļ�ֵ������ lResult
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, (LPBYTE)&lResult, &dwLen);
	if (ret != ERROR_SUCCESS) lResult=0;	// ����׼������ 0
	
	// �ر�������
	RegCloseKey(hKey);
	
	// ����ֵ lResult
	return lResult;
}


// ��ȡע�����������͵ļ�ֵ����ȡ���Ķ������ֽڿռ�
//   �ɱ������Զ����١��� HM �Զ������� ptrBuff ���ش˿ռ��׵�ַ
// �������ش˿ռ���ֽڸ����������� 0
// λ��Ϊ <eRootKey>\software\<mcSubKey>\subKey
// szKeyName Ϊ��ֵ���ƣ�=""��ʾ��ȡĬ��ֵ
long RegGetValueBin( LPCTSTR szSubKey, LPCTSTR szKeyName, char * ptrBuff, ERegControlKeys eRootKey/*=mcRootKey*/ )
{
	long lResult=0;
	LONG ret;

	// ������ 
	HKEY hKey=NULL; 
	ret = RegOpenKeyEx((HKEY)eRootKey, szSubKey, 
		0, KEY_QUERY_VALUE | 256, &hKey);	// | 256������ 64 λ����ϵͳ
	if (ret != ERROR_SUCCESS) return 0;		// ���� 0 ��ʾʧ��

	// ��������м�ֵ����Ϊ szKeyName �ļ�ֵ���������ͺͻ���������
	DWORD dwType=0, dwLen=0; 
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, NULL, &dwLen);
	if (dwLen<0 || ret!= ERROR_SUCCESS) { RegCloseKey(hKey); return 0;}
	// ׼�������� bytBuff
	char * bytBuff = new char [dwLen+1];
	HM.AddPtr(bytBuff);
	// �û����� bytBuff ��ȡ��ֵ
	ret = RegQueryValueEx(hKey, szKeyName, 0, &dwType, (LPBYTE)bytBuff, &dwLen);
	if (ret != ERROR_SUCCESS) { RegCloseKey(hKey); return 0;}

	// �ر�������
	RegCloseKey(hKey);		

	// �����ֽڸ���
	return dwLen;
}


// ����һ���ļ�����
bool RegCreateAssoFile(LPTSTR szFileExp, 
					   LPTSTR szClassName /*= NULL*/, 
					   LPTSTR szDescrip /*= NULL*/,
					   LPTSTR szIconFile /*= NULL*/, 
					   LPTSTR szOpenCommand /*= NULL*/, 
					   LPTSTR szPrintCommand /*= NULL*/)
{
	// Ĭ�Ϸ���ֵ
	bool retVal = true;	

	// ׼����չ���ı��� szFileExp
	if (szFileExp == NULL) return false;
	if ( *szFileExp != TEXT('.') )
		szFileExp = StrAppend(TEXT("."), szFileExp);

	// ׼�������ı��� szClassName �������������ƣ�
	if (szClassName == NULL)
		szClassName = StrAppend(szFileExp+1, TEXT("file"));	// ȥ���㣬���磺txtfile
	if ( *(szClassName+lstrlen(szClassName)-1)==TEXT('\\') )// �����һ���ַ�Ϊ ��\����ȥ����
		*(szClassName+lstrlen(szClassName)-1)=TEXT('\0');	

	// ������չ������
	if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
		szFileExp, TEXT(""), szClassName) ) retVal=false;

	// ���� shell\open\command �����ʾ���ļ�����
	if (szOpenCommand)
	{
		if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
			StrAppend(szClassName, TEXT("\\shell\\open\\command")),
			TEXT(""), szOpenCommand) ) retVal=false;
	}

	// ���� shell\print\command �����ʾ��ӡ�ļ�����
	if (szPrintCommand)
	{
		if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
			StrAppend(szClassName, TEXT("\\shell\\print\\command")),
			TEXT(""), szPrintCommand) ) retVal=false;
	}

	// ���� descrip ��ֵ
	if (szDescrip)
	{
		if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
			szClassName, TEXT(""), szDescrip) ) retVal=false;
	}

	// ���� ͼ���ļ� ����
	if (szIconFile)
	{
		if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
			StrAppend(szClassName, TEXT("\\DefaultIcon")), 
			TEXT(""), szIconFile) ) retVal=false;
	}

	// ���� retVal ��ֵ
	return retVal;
}	

// ɾ��һ���ļ�������ע�����Ϣ
// ֻ��ɾ��ͨ�� RegCreateAssoFile �������ļ�������Ϣ��
//   ��������ɾ��ע����е�������Ϣ
bool RegDelAssoFile(LPCTSTR szFileExp)
{
	LPTSTR szClassName;
	
	// Ĭ�Ϸ���ֵ
	bool retVal = true;	

	if (szFileExp == NULL) return false;
	// ׼����չ���ı��� szFileExp
	if ( *szFileExp != TEXT('.') )
		szFileExp = StrAppend(TEXT("."), szFileExp);
	
	// ��������ı� => szClassName �������������ƣ�
	szClassName = RegGetValueStringCtrl(eHKEY_CLASSES_ROOT, szFileExp, TEXT(""));


	// ɾ����չ������
	if (! RegDelCtrl(eHKEY_CLASSES_ROOT, szFileExp)) retVal=false;

	// ɾ��ͼ������
	if (! RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\DefaultIcon")))) retVal=false;
	
	// ɾ���ļ��򿪹�������
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\shell\\open\\command")));
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\shell\\open")));

	// ɾ���ļ���ӡ��������
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\shell\\print\\command")));
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\shell\\print")));

	// ɾ�� shell ����
	if (! RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(szClassName,TEXT("\\shell")))) retVal=false;

	// ɾ����������
	if (! RegDelCtrl(eHKEY_CLASSES_ROOT,szClassName)) retVal=false;

	// ���� retVal ��ֵ
	return retVal;
}

bool RegSetValueStringCtrl( ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName, LPCTSTR szValue )
{
	LONG ret;

	// ������� RegCreateKeyEx �򿪣�����������򴴽���
	HKEY hKey=NULL; 
	ret = RegCreateKeyEx((HKEY)controlKey, szSubKey, 
		0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	if (ret != ERROR_SUCCESS) return false;	// ���� false ��ʾʧ��

	// ���������ֵ
	ret = RegSetValueEx(hKey, szKeyName, 0, REG_SZ, 
		(CONST BYTE *)szValue, (lstrlen(szValue)+1) * sizeof(TCHAR) );

	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}

bool RegSetValueLongCtrl( ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName, long lValue )
{
	LONG ret;

	// ������� RegCreateKeyEx �򿪣�����������򴴽���
	HKEY hKey=NULL; 
	ret = RegCreateKeyEx((HKEY)controlKey, szSubKey, 
		0, NULL, 0, KEY_ALL_ACCESS64, 0, &hKey, NULL); 
	if (ret != ERROR_SUCCESS) return false;	// ���� false ��ʾʧ��

	// ���������ֵ
	ret = RegSetValueEx(hKey, szKeyName, 0, REG_DWORD, 
		(CONST BYTE *)&lValue, sizeof(lValue) );

	// �ر�������
	RegCloseKey(hKey);

	// ����ֵ
	return ret==ERROR_SUCCESS;	
}

// ����һ���롰�����ļ������ܴ򿪵Ĺ���
// szClassName Ϊע����� HKEY_CLASSES_ROOT\*\shell\className �� className
// szDescrip Ϊ���á����򿪡�������˵��
// szCmdExe Ϊ��������
bool RegCreateAllAssoFile( LPCTSTR szClassName, LPCTSTR szDescrip, LPCTSTR szCmdExe )
{
	if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(TEXT("*\\shell\\"), szClassName), 
		TEXT(""), szDescrip)) return false;
	if (! RegSetValueStringCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(TEXT("*\\shell\\"), szClassName, TEXT("\\Command")),
		TEXT(""), szCmdExe)) return false;
}

// ɾ��һ���롰�����ļ������ܴ򿪵Ĺ���
// szClassName Ϊע����� HKEY_CLASSES_ROOT\*\shell\className �� className
bool RegDelAllAssoFile( LPCTSTR szClassName )
{
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(TEXT("*\\shell\\"), szClassName, TEXT("\\Command")));
	RegDelCtrl(eHKEY_CLASSES_ROOT, 
		StrAppend(TEXT("*\\shell\\"), szClassName));
	return true;
}





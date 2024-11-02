//////////////////////////////////////////////////////////////////////
// mdlRegOperation.h: ʵ��ע��������ģ�麯��ԭ�ͼ�������������
//
// ֧�֣�
//     ��Ҫ BWindows ģ���֧��
//////////////////////////////////////////////////////////////////////

#include "BWindows.h"

// �������ע������ʹ�ã�<mcRootKey>\Software\<mcSubKey>
// Ҫʹ������������޸Ĵ˳���ֵ
#define mcSubKey	TEXT("NZKey")		// δ�޸Ĵ˴��۷�
#define mcRootKey	eHKEY_CURRENT_USER		// Ĭ�ϱ����ڵĸ���

// ����64λ����ϵͳ
#define KEY_ALL_ACCESS64 (KEY_ALL_ACCESS | 256)

enum ERegControlKeys		// ע�����
{
	eHKEY_CLASSES_ROOT = (int)HKEY_CLASSES_ROOT,
	eHKEY_CURRENT_CONFIG = (int)HKEY_CURRENT_CONFIG, 
	eHKEY_CURRENT_USER = (int)HKEY_CURRENT_USER, 
	eHKEY_LOCAL_MACHINE = (int)HKEY_LOCAL_MACHINE, 
	eHKEY_USERS = (int)HKEY_USERS, 
	eHKEY_PERFORMANCE_DATA = (int)HKEY_PERFORMANCE_DATA, 
    eHKEY_DYN_DATA = (int)HKEY_DYN_DATA
};

//////////////////////////////////////////////////////////////////////////
// Ĭ�ϱ�����洢λ���µĲ�����
//		<mcRootKey>\Software\<mcSubKey>

// �������� szSubKey ��Ĭ��·��
// ������·��Ϊ��Software\<mcSubKey>\<szSubKey>
LPTSTR MakeSubKey(LPCTSTR szSubKey);

// �� <mcRootKey>\<szSubKey> ���潨��һ������
// szSubKey �����Ƕ༶·������һ���Խ���·���е����������"Software\abc\de\fg"
bool RegCreate(LPCTSTR szSubKey, ERegControlKeys eRootKey=mcRootKey);

// ��ȡע����ַ������͵ļ�ֵ���ɺ������أ��������������""
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ��ֵ
// ����ַ����ռ��ɱ�ģ���Զ����٣��� HM �Զ�����
LPTSTR RegGetValueString(LPCTSTR szSubKey, 
						 LPCTSTR szKeyName, 
						 ERegControlKeys eRootKey=mcRootKey);


// ��ȡע����������͵ļ�ֵ���ɺ������أ��������������0
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
long RegGetValueLong(LPCTSTR szSubKey, 
					 LPCTSTR szKeyName, 
					 ERegControlKeys eRootKey=mcRootKey);


// ��ȡע�����������͵ļ�ֵ����ȡ���Ķ������ֽڿռ�
//   �ɱ������Զ����١��� HM �Զ������� ptrBuff ���ش˿ռ��׵�ַ
// �������ش˿ռ���ֽڸ����������� 0
// λ��Ϊ <eRootKey>\software\<mcSubKey>\subKey
// szKeyName Ϊ��ֵ���ƣ�=""��ʾ��ȡĬ��ֵ
long RegGetValueBin(LPCTSTR szSubKey, 
					LPCTSTR szKeyName, 
					char * ptrBuff, 
					ERegControlKeys eRootKey=mcRootKey);

// ��һ������������һ���ַ����͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ szValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueString(LPCTSTR szSubKey, 
					   LPCTSTR szKeyName, 
					   LPCTSTR szValue, 
					   ERegControlKeys eRootKey=mcRootKey);


// ��һ������������һ���������͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <mcRootKey>\software\<mcSubKey>\<szSubKey>
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ lValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueLong(LPCTSTR szSubKey, 
					 LPCTSTR szKeyName, 
					 long lValue, 
					 ERegControlKeys eRootKey=mcRootKey);

// ��һ������������һ���������͵ļ�ֵ���������������ȴ�����
// λ��Ϊ <eRootKey>\software\<mcSubKey>\subKey
// ��ֵ��Ϊ szKeyName��="" ��ʾĬ�ϼ�ֵ����ֵΪ pBuff ��ַ��ʼ�ġ�iBuffLen ���ֽ�
// �ɹ����� True��ʧ�ܷ��� False
bool RegSetValueBin(LPCTSTR szSubKey, 
					LPCTSTR szKeyName, 
					const char * pBuff,
					const int iBuffLen,
					ERegControlKeys eRootKey=mcRootKey);


// ɾ�� szSubKey �����µ�һ��ע����ֵ
bool RegDelValue(LPCTSTR szSubKey, LPCTSTR szKeyName, ERegControlKeys eRootKey=mcRootKey);

// ɾ�� <mcRootKey>\Software\<mcSubKey> �����һ������
// szSubKey �����Ƕ༶·������ֻɾ�����һ������
// ��� subKey ��ָ�������һ�������л�������һ��������ܱ�ɾ��
//   ֻ�в�������һ�������������ܱ�ɾ��
bool RegDel(LPCTSTR szSubKey, ERegControlKeys eRootKey=mcRootKey);


//////////////////////////////////////////////////////////////////////////
// ע�������λ���µĲ�����
//		<controlKey>\<szSubKey>


// �� ע��������λ�� ��ȡ�ַ������͵ļ�ֵ���ɺ������أ��������������""
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// ����ַ����ռ��ɱ�ģ���Զ����٣��� HM �Զ�����
LPTSTR RegGetValueStringCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName);

// �� ע��������λ�� ��ȡ���������͵ļ�ֵ���ɺ������أ��������������0
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
long RegGetValueLongCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName);

// �� ע��������λ�� ����һ���ַ����͵ļ�ֵ���������������ȴ�����
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ szValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueStringCtrl( ERegControlKeys controlKey, 
							LPCTSTR szSubKey, 
							LPCTSTR szKeyName, 
							LPCTSTR szValue); 

// �� ע��������λ�� ����һ���������͵ļ�ֵ���������������ȴ�����
// szKeyName Ϊ��ֵ���ƣ�== "" �� ָ��==0 ��ʾ��ȡĬ�ϼ�ֵ
// Ҫ���õ�ֵΪ lValue
// ����ֵ���ɹ����� true��ʧ�ܷ��� false
bool RegSetValueLongCtrl( ERegControlKeys controlKey, 
						  LPCTSTR szSubKey, 
						  LPCTSTR szKeyName, 
						  long lValue);

// ɾ�� ע��������λ�� �����µ�һ��ע����ֵ
bool RegDelValueCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey, LPCTSTR szKeyName);

// ɾ�� ע��������λ�� ��<controlKey>\<szSubKey> �����һ������
// szSubKey �����Ƕ༶·������ֻɾ�����һ������
// ��� subKey ��ָ�������һ�������л�������һ��������ܱ�ɾ��
//   ֻ�в�������һ�������������ܱ�ɾ��
bool RegDelCtrl(ERegControlKeys controlKey, LPCTSTR szSubKey);


//////////////////////////////////////////////////////////////////////////
// �ļ���������

// ����һ���ļ�����
bool RegCreateAssoFile(LPTSTR szFileExp, 
					   LPTSTR szClassName = NULL, 
					   LPTSTR szDescrip = NULL,
					   LPTSTR szIconFile = NULL, 
					   LPTSTR szOpenCommand = NULL, 
					   LPTSTR szPrintCommand = NULL);


// ɾ��һ���ļ�������ע�����Ϣ
// ֻ��ɾ��ͨ�� RegCreateAssoFile �������ļ�������Ϣ��
//   ��������ɾ��ע����е�������Ϣ
bool RegDelAssoFile(LPCTSTR szFileExp);

// ����һ���롰�����ļ������ܴ򿪵Ĺ���
// szClassName Ϊע����� HKEY_CLASSES_ROOT\*\shell\className �� className
// szDescrip Ϊ���á����򿪡�������˵��
// szCmdExe Ϊ��������
bool RegCreateAllAssoFile(LPCTSTR szClassName, LPCTSTR szDescrip, LPCTSTR szCmdExe);

// ɾ��һ���롰�����ļ������ܴ򿪵Ĺ���
// szClassName Ϊע����� HKEY_CLASSES_ROOT\*\shell\className �� className
bool RegDelAllAssoFile(LPCTSTR szClassName);
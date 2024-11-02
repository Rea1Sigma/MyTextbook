//////////////////////////////////////////////////////////////////////
// ���ļ�����������Ķ��壺
// CBRecycledArr��������վ��������
// CBRecycledArrInt�����Ͱ������վ��������
//
// ����֯һ�����ݣ��������Ϳ�ͬʱΪ��
//    һ���ַ���������������һ������������һ��˫������
// ������ Add��Remove���Զ�ά�� index Ϊ 1��Count ��Χ���Ա�ͨ������
//   �ķ�ʽ���ʸ�Ԫ�أ�Ҳ֧��ͨ�������ķ������ʸ�Ԫ��
//////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>



//////////////////////////////////////////////////////////////////////
// CBRecycledArr��������վ��������
//
// ����֯һ�����ݣ��������Ϳ�ͬʱΪ��
//    һ���ַ���������������һ������������һ��˫������
// ������ Add��Remove���Զ�ά�� index Ϊ 1��Count ��Χ���Ա�ͨ������
//   �ķ�ʽ���ʸ�Ԫ�أ�Ҳ֧��ͨ�������ķ������ʸ�Ԫ��
//////////////////////////////////////////////////////////////////////


// ���� throw �Ĵ���
// throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
class CBRecycledArr  
{
private:
	typedef struct _MemType
	{
		LPTSTR DataString;	// �ַ�������
		int DataInt;		// ������������1
		int DataInt2;		// ������������2
		float DataFloat;	// ���ӵ���������
		double DataDouble;	// ����˫��������
		bool Used;
	} MemType;
	
	static const int	mcIniMemSize;  			// ��ʼ mem[] �Ĵ�С
	static const int	mcMaxItemCount; 		// ���Ԫ�ظ������������ֵ�� long ��ʾ�ķ�Χ֮�ڣ�
	static const float	mcExpandMaxPort;  		// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
	static const int	mcExpandCountThres; 	// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
	static const int	mcExpandCountThresMax;	// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcExpandBigPer; 		// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcRecyExpandPer;		// ���� recycles[] �ռ�ʱ��ÿ������Ĵ�С
	
private:
	MemType * mem;					// ��̬����ָ�룬�����鲻ʹ�� [0] ��Ԫ��
	int memCount, memUsedCount;		// ��̬��������±꣬mem[] �����±�Ϊ [0] �� [memCount]������[0] �� [memUsedCount]�����п�����ɾ����Ԫ�أ�ɾ����Ԫ���±��� recycles �б��棩
	int * recycles;					// ָ��һ���������飬�����Ԫ�ر��� mem[] ����ɾ����Ԫ�ص��±ꣻ�����鱻ά����С��������
									// recycles[] �����±�Ϊ [0] �� [recyclesCount]������ [0] ���ã��� [1] - [recyUsedCount]������Ϊ���ÿ���ռ�
	int recyCount, recyUsedCount;	// recycles[] ����Ԫ���±����ֵ
	int mTravIdxCurr;				// �� NextXXX ���������ĵ�ǰ index
	
public:
	CBRecycledArr(int initSize = 0);	// initSize = 0 ʱ����ʼ���� mcIniMemSize ��С�Ŀռ䣬���򿪱� initSize ��С�Ŀռ�
	~CBRecycledArr(); 
	
	int Add(LPCTSTR dataString = 0, int dataInt = 0, int dataInt2 = 0, float dataFloat = 0.0, double dataDouble = 0.0);  // ��������ݣ����������ݱ����浽�� mem[] �е��±꣨>0����������0
	bool Remove(int index);				// ɾ��һ�����ݣ�index ΪҪɾ�����ݵ��±ꣻɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
	LPTSTR Item(int index, bool bRaiseErrIfNotHas=false);		// �����±� index������һ���ַ������ݣ�ɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count���ַ������ڴ�ռ��ɱ����Զ������������򲻱ظ�Ԥ�������������޸��˷���ָ���ָ�����ݣ���������ڲ�����Ҳͬʱ�����仯
	int ItemInt(int index, bool bRaiseErrIfNotHas=false);		// �����±� index������һ�����͸������ݣ�ɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
	int ItemInt2(int index, bool bRaiseErrIfNotHas=false);		// �����±� index������һ�����͸�������2��ɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
	float ItemFloat(int index, bool bRaiseErrIfNotHas=false);	// �����±� index������һ�� float �͸������ݣ�ɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
	double ItemDouble(int index, bool bRaiseErrIfNotHas=false);	// �����±� index������һ�� double �͸������ݣ�ɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
	int Count();						// ���ص�ǰӵ�е����ݸ���
	void Clear();						// ɾ����������
	
	void StartTraversal();				// ��ʼһ�α���
	LPTSTR NextItem( bool &bRetNotValid );			// ��������һ�����ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int NextDataInt( bool &bRetNotValid );			// ��������һ�������������ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int NextDataInt2( bool &bRetNotValid );			// ��������һ�������������ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	float NextDataFloat( bool &bRetNotValid );		// ��������һ������ float �����ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	double NextDataDouble( bool &bRetNotValid );	// ��������һ�������� double ���ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	
private:
	void ExpandMem(void);
	int RedimArrMemType(MemType * &arr, int toUBound=-1, int uboundCurrent=-1, bool preserve=false);		// �ض��� һ�� MemType ���͵����飨������� mem[]���Ĵ�С���¶���ռ��Զ�����
	int RedimArrInt( int * &arr, int toUBound=-1, int uboundCurrent=-1, bool preserve=false );				// �ض��� һ�� int ���͵����飨������� recycles���Ĵ�С���¶���ռ��Զ�����
	int FindPosInSortedRecycle(int itemToFind);		// �ö��ֲ��ҷ������� recycles �в���Ԫ�� itemToFind ��λ��
	int UserIndexToMemIndex(int index);				// �����û� index������� mem[] �е��±ꣻɾ�����ݺ󣬺������ݵ��û� index ���Զ�������ʹ�û� index ��Ϊ 1��Count
};




//////////////////////////////////////////////////////////////////////
// CBRecycledArrInt�����Ͱ������վ��������
//
// ����֯һ�����ݣ��������Ϳ�ͬʱΪ��
//    ����������һ������������һ��˫������
// ������ Add��Remove���Զ�ά�� index Ϊ 1��Count ��Χ���Ա�ͨ������
//   �ķ�ʽ���ʸ�Ԫ�أ�Ҳ֧��ͨ�������ķ������ʸ�Ԫ��
//////////////////////////////////////////////////////////////////////

// ���� throw �Ĵ���
// throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
class CBRecycledArrInt  
{
private:
	typedef int DataType;			// ���ݵ�����
	typedef int DataIntType;		// �����������ݵ�����
	typedef float DataFloatType;	// ���ӵ��������ݵ�����
	typedef double DataDoubleType;	// ����˫�������ݵ�����
	
	typedef struct _MemType
	{
		DataType Data;
		DataIntType DataInt;
		DataFloatType DataFloat;
		DataDoubleType DataDouble;
		bool Used;
	} MemType;
	
	static const int	mcIniMemSize;  			// ��ʼ mem[] �Ĵ�С
	static const int	mcMaxItemCount; 		// ���Ԫ�ظ������������ֵ�� long ��ʾ�ķ�Χ֮�ڣ�
	static const float	mcExpandMaxPort;  		// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
	static const int	mcExpandCountThres; 	// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
	static const int	mcExpandCountThresMax;	// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcExpandBigPer; 		// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcRecyExpandPer;		// ���� recycles[] �ռ�ʱ��ÿ������Ĵ�С
	
private:
	MemType * mem;					// ��̬����ָ�룬�����鲻ʹ�� [0] ��Ԫ��
	int memCount, memUsedCount;		// ��̬��������±꣬mem[] �����±�Ϊ [0] �� [memCount]������[0] �� [memUsedCount]�����п�����ɾ����Ԫ�أ�ɾ����Ԫ���±��� recycles �б��棩
	int * recycles;					// ָ��һ���������飬�����Ԫ�ر��� mem[] ����ɾ����Ԫ�ص��±�
	// recycles[] �����±�Ϊ [0] �� [recyclesCount]������ [0] ���ã��� [1] - [recyUsedCount]������Ϊ���ÿ���ռ�
	int recyCount, recyUsedCount;	// recycles[] ����Ԫ���±����ֵ
	int mTravIdxCurr;				// �� NextXXX ���������ĵ�ǰ index
	
public:
	CBRecycledArrInt(int initSize = 0);	// initSize = 0 ʱ����ʼ���� mcIniMemSize ��С�Ŀռ䣬���򿪱� initSize ��С�Ŀռ�
	~CBRecycledArrInt(); 
	
	int Add(DataType data, DataIntType dataInt = 0, DataFloatType dataFloat = 0.0, DataDoubleType dataDouble = 0.0);  // ��������ݣ����������ݱ����浽�� mem[] �е��±꣨>0����������0
	bool Remove(int index);				// ɾ��һ�����ݣ�index ΪҪɾ�����ݵ��±ꡣ
	DataType Item(int index, bool bRaiseErrIfNotHas=false);				// �����±� index������һ������
	DataIntType ItemInt(int index, bool bRaiseErrIfNotHas=false);		// �����±� index������һ�����͸�������
	DataFloatType ItemFloat(int index, bool bRaiseErrIfNotHas=false);	// �����±� index������һ�� float �͸�������
	DataDoubleType ItemDouble(int index, bool bRaiseErrIfNotHas=false);	// �����±� index������һ�� double �͸�������
	int Count();						// ���ص�ǰӵ�е����ݸ���
	void Clear();						// ɾ����������
	
	void StartTraversal();				// ��ʼһ�α���
	DataType NextItem( bool &bRetNotValid );				// ��������һ�����ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	DataIntType NextDataInt( bool &bRetNotValid );			// ��������һ�������������ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	DataFloatType NextDataFloat( bool &bRetNotValid );		// ��������һ������ float �����ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	DataDoubleType NextDataDouble( bool &bRetNotValid );	// ��������һ�������� double ���ݣ��� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	
private:
	void ExpandMem(void);
	int RedimArrMemType(MemType * &arr, int toUBound=-1, int uboundCurrent=-1, bool preserve=false);		// �ض��� һ�� MemType ���͵����飨������� mem[]���Ĵ�С���¶���ռ��Զ�����
	int RedimArrInt( int * &arr, int toUBound=-1, int uboundCurrent=-1, bool preserve=false );				// �ض��� һ�� int ���͵����飨������� recycles���Ĵ�С���¶���ռ��Զ�����
	
};


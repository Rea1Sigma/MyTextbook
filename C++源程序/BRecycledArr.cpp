//////////////////////////////////////////////////////////////////////
// ���ļ�����2�����ʵ�֣�
// CBRecycledArr ������վ������
// CBRecycledArrInt ���Ͱ������վ������
//
//////////////////////////////////////////////////////////////////////

#include "BRecycledArr.h"
#include <memory.h>
#include <string.h>
#include <tchar.h>


//////////////////////////////////////////////////////////////////////
// CBRecycledArr ���ʵ�֣�������վ������
//
//////////////////////////////////////////////////////////////////////

// ===================================================================================
// �ַ������ݣ�
//   �� Add ʱ�� new �ռ䣬���� mem[] �Ŀռ�ʱ���� mem[] Ԫ�ص��ַ���ָ�벻 = new
//   �� Remove ʱ��delete �ռ䣬Ȼ�󱣴浽������վ��������վ�е������ַ���ָ�붼ΪNULL
//   ������ʱ��delete �ռ�
// ===================================================================================

//////////////////////////////////////////////////////////////////////
// Static ������ֵ
//////////////////////////////////////////////////////////////////////
const int   CBRecycledArr::mcIniMemSize = 7;				// ��ʼ mem[] �Ĵ�С
const int   CBRecycledArr::mcMaxItemCount = 100000000;		// ���Ԫ�ظ����������ڴ�ԭ��2G�ڴ�֮�ڣ�sizeof(MemType)*��ֵ���ó���2G��
const float CBRecycledArr::mcExpandMaxPort = 0.75;			// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
const int   CBRecycledArr::mcExpandCountThres = 10000;		// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
const int   CBRecycledArr::mcExpandCountThresMax = 10000000;// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int   CBRecycledArr::mcExpandBigPer = 1000000;		// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int	CBRecycledArr::mcRecyExpandPer = 100;			// ���� recycles[] �ռ�ʱ��ÿ������Ĵ�С
		

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBRecycledArr::CBRecycledArr(int initSize/*=0*/)
{
	// ��ʼ����Ա������ֵ
	mem = 0;
	memCount = 0; memUsedCount = 0;
	recycles = 0;
	recyCount = 0; recyUsedCount = 0;
	mTravIdxCurr = 0;

	// ���ٳ�ʼ�ռ�
	if (initSize)
	{
		// ���� initSize ��С�Ŀռ�
		RedimArrMemType(mem, initSize);
		memCount = initSize;
	}
	else
	{
		// ���� mcIniMemSize ��С�Ŀռ�
		RedimArrMemType(mem, mcIniMemSize);
		memCount = mcIniMemSize;
	}
}

CBRecycledArr::~CBRecycledArr()
{
	Clear();
	// Clear() �������Զ������� mcIniMemSize ��С�Ŀռ䣬����ɾ��������
	delete []mem; memCount=0;
} 





//////////////////////////////////////////////////////////////////////
// ���к���
//////////////////////////////////////////////////////////////////////

// ��������ݣ����������ݱ����浽�� mem[] �е��±꣨>0����������0
int CBRecycledArr::Add( LPCTSTR dataString /*= NULL*/, int dataInt /*= 0*/, int dataInt2 /*= 0*/, float dataFloat /*= 0.0*/, double dataDouble /*= 0.0*/ )
{
	int idx = 0;  // Ҫ�������ݴ��� mem[idx]

	// ======== Ϊ�����ݷ����¿ռ䣬��ÿռ��±� idx ========
	if (recycles!=0 && recyUsedCount>0)
	{
		// ����վ���пռ䣬ʹ�û���վ�е����һ���ռ䣻����վʣ�ಿ����ά���Ŵ�С��������
		idx = recycles [recyUsedCount];
		recyUsedCount--;
	}
	else
	{
		// ����վ��û�пռ䣬ʹ�� mem[idx] �е���һ�����ÿռ�
		if (memUsedCount+1 <= memCount)
		{
			memUsedCount++;
			idx = memUsedCount;
		}
		else
		{
			// Redim preserve mem[1 to memCount + һ��] 
			ExpandMem();
			// ����ռ���ٴ��ж�
			if (memUsedCount+1 <= memCount) 
			{
				memUsedCount++;
				idx = memUsedCount;
			}
			else
			{
				// ����ռ�󻹲��� memUsedCount <= memCount���޷�����ռ䣬���س���
				return 0;		
			}
		}
	}

	// ======== ���������� ========
	if (dataString)
	{
		mem[idx].DataString = new TCHAR [_tcslen(dataString) + 1];
		_tcscpy(mem[idx].DataString, dataString);
	}
	else
	{
		mem[idx].DataString = NULL;
	}
	mem[idx].DataInt = dataInt;
	mem[idx].DataInt2 = dataInt2;
	mem[idx].DataFloat = dataFloat;
	mem[idx].DataDouble = dataDouble;
	mem[idx].Used = true;

	// ���������ݱ����浽�� mem[] �е��±�
	return idx;
}


bool CBRecycledArr::Remove( int index )
{
	int idx, i;

	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount) return false;	// index ���Ϸ�
	if (! mem[index].Used) return false;			// �����ѱ�ɾ��

	// ======== ��������ֵΪ�� ========
	if (mem[index].DataString)	// ɾ�������ݵ��ַ�����ռ�õĿռ䣨����еĻ���
	{
		delete [] mem[index].DataString;
		mem[index].DataString = NULL;
	}
	mem[index].DataInt = 0;
	mem[index].DataInt2 = 0;
	mem[index].DataFloat = 0.0;
	mem[index].DataDouble = 0.0;
	mem[index].Used = false;

	// ======== �ڻ���վ�м�¼�ÿռ��ѱ�ɾ�� ========
	// �ڻ���վ�б����¼ index��ά������վ recycles[] �����С��������

	if (recycles==0 || recyUsedCount + 1 > recyCount)
	{
		// �������վ�еĿռ�
		RedimArrInt(recycles, recyCount+mcRecyExpandPer, recyCount, true);
		recyCount = recyCount+mcRecyExpandPer;
	}
	
	// ʹ�ö��ֲ����ҵ� index Ӧ�����浽�� recycles[] �±�ĸ���
	idx=FindPosInSortedRecycle(index);	
	
	// ���� + 1
	recyUsedCount++;

	// ����������
	if (idx<0)
	{
		// ���������index Ӧ�����浽�� recycles[-idx]
		for(i=recyUsedCount; i>-idx; i++)
			recycles[i]=recycles[i-1];
		recycles[-idx] = index;
	}
	else
	{
		// �������������վ���Ѿ�����ֵΪ index �����Ԫ�أ�����ֲ��ҳ���
		// �ݴ���ʹ�� recycles[idx] ���� index
		recycles[idx] = index;
	}

	// ���سɹ�
	return true;
}


LPTSTR CBRecycledArr::Item( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;	
	}

	return mem[index].DataString;
}


int CBRecycledArr::ItemInt( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;	
	}

	return mem[index].DataInt;
}

int CBRecycledArr::ItemInt2( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;	
	}
	
	return mem[index].DataInt2;
}

float CBRecycledArr::ItemFloat( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;	
	}
	
	return mem[index].DataFloat;
}

double CBRecycledArr::ItemDouble( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	index=UserIndexToMemIndex(index);
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;	
	}
	
	return mem[index].DataDouble;
}



void CBRecycledArr::Clear()
{
	// ��ɾ�������ַ����Ŀռ�
	int i;
	for(i=1; i<=memCount; i++)
		if (mem[i].DataString)
		{
			delete [] mem[i].DataString;
			mem[i].DataString = NULL;
		}

	// ɾ�� mem ����Ŀռ�� ����վ ����Ŀռ�
	if (mem) delete []mem;
	if (recycles) delete []recycles;

	mem = 0; memCount = 0; memUsedCount = 0;
	recycles = 0; recyCount = 0; recyUsedCount = 0;
	mTravIdxCurr = 0;

	// ���� mcIniMemSize ��С�Ŀռ�
	RedimArrMemType(mem, mcIniMemSize);
	memCount = mcIniMemSize;
}



void CBRecycledArr::StartTraversal()
{
	mTravIdxCurr = 1;
}


LPTSTR CBRecycledArr::NextItem( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataString;}
	}

	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0;
}


int CBRecycledArr::NextDataInt( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataInt;}
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0;
}

int CBRecycledArr::NextDataInt2( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataInt2;}
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0;
}

float CBRecycledArr::NextDataFloat( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataFloat;}
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0.0;
}

double CBRecycledArr::NextDataDouble( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataDouble; }
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0.0;
}


// ���ص�ǰӵ�е����ݸ���
int CBRecycledArr::Count()
{
	int ct = memUsedCount-recyUsedCount;
	if (ct<0) ct = 0;
	return ct;
}



//////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////

void CBRecycledArr::ExpandMem( void )
{
	int toCount = 0;
	toCount=memCount;

	if (toCount < 1) toCount = 1;	// ���� toCount Ϊ0�޷�����ռ�
	if (toCount < mcExpandCountThres)
	{
		// ��������������Ƚ��١����������ռ�Ϊԭ����2��
		toCount = toCount * 2;
	}
	else if (toCount < mcExpandCountThresMax)
	{
		// ������������Ѿ����е�ࡱ���������ռ�Ϊԭ����1.5��
		toCount = toCount * 3 / 2;
	}
	else
	{
		// ��������������ܶࡱ�������� mcExpandBigPer ���ռ�
		toCount = toCount + mcExpandBigPer;
	}
	
	// �ض��������С
	// ReDim Preserve mem(1 To toCount)
	RedimArrMemType(mem, toCount, memCount, true);
	memCount = toCount;
}




// �ض��� һ�� MemType ���͵����飨������� recycles���Ĵ�С���¶���ռ��Զ�����
// arr��Ϊ����ָ�룬�ɴ��ݣ�recycles�����������޸Ĵ�ָ���ָ��
// toUBound��ΪҪ�ض����������Ͻ磬����Ϊ��[0] to [toUBound]��Ϊ -1 ʱ�����ٿռ䣬������ɾ��ԭ
//	 �ռ䣬�� arr �ᱻ��Ϊ0
// uboundCurrent��Ϊ�ض���ǰ������Ͻ� [0] to [uboundCurrent]��Ϊ -1 ��ʾ��δ���ٹ��ռ�Ϊ��һ�ε���
// preserve����������ԭʼ���ݷ��򲻱���
// �����¿ռ��ϱ꣬�� toUBound
int CBRecycledArr::RedimArrMemType( MemType * &arr, int toUBound/*=-1*/, int uboundCurrent/*=-1*/, bool preserve/*=false*/ )
{
	// �����¿ռ䣺[0] to [toUBound]
	if (toUBound >= 0)
	{
		MemType * ptrNew = new MemType [toUBound + 1];		// +1 Ϊʹ�����±���� toUBound
		// �¿ռ�����
		memset(ptrNew, 0, sizeof(MemType)*(toUBound + 1));
		
		// ��ԭ�пռ����ݿ������¿ռ�
		if (preserve && arr!=0 && uboundCurrent>=0)
		{
			int ctToCpy;										// ����ԭ�����ݣ���Ҫ�����ڴ�� MemType Ԫ�ظ���
			ctToCpy = uboundCurrent;
			if (uboundCurrent>toUBound) ctToCpy = toUBound;		// ȡ uboundCurrent �� toUBound ����Сֵ
			ctToCpy = ctToCpy + 1;								// ���� +1����Ϊ uboundCurrent �� toUBound ���������Ͻ�
			memcpy(ptrNew, arr, sizeof(MemType)*ctToCpy); 
		}
		
		// ɾ��ԭ�пռ�
		if (arr!=0 && uboundCurrent>=0) delete [] arr;
		
		// ָ��ָ���¿ռ�
		arr = ptrNew;
		return toUBound;
	}
	else		// if (toUBound < 0)�������ٿռ䣬ɾ��ԭ�пռ�
	{
		if(arr!=0 && uboundCurrent>=0) delete [] arr;
		arr = 0;
		return 0;
	}
	
}


// �ض��� һ�� int ���͵����飨������� mem[]���Ĵ�С���¶���ռ��Զ�����
// arr��Ϊ����ָ�룬�ɴ��ݣ�mem�����������޸Ĵ�ָ���ָ��
// toUBound��ΪҪ�ض����������Ͻ磬����Ϊ��[0] to [toUBound]��Ϊ -1 ʱ�����ٿռ䣬������ɾ��ԭ
//	 �ռ䣬�� arr �ᱻ��Ϊ0
// uboundCurrent��Ϊ�ض���ǰ������Ͻ� [0] to [uboundCurrent]��Ϊ -1 ��ʾ��δ���ٹ��ռ�Ϊ��һ�ε���
// preserve����������ԭʼ���ݷ��򲻱���
// �����¿ռ��ϱ꣬�� toUBound
int CBRecycledArr::RedimArrInt( int * &arr, int toUBound/*=-1*/, int uboundCurrent/*=-1*/, bool preserve/*=false*/ )
{
	// �����¿ռ䣺[0] to [toUBound]
	if (toUBound >= 0)
	{
		int * ptrNew = new int [toUBound + 1];		// +1 Ϊʹ�����±���� toUBound
		// �¿ռ�����
		memset(ptrNew, 0, sizeof(int)*(toUBound + 1));
		
		// ��ԭ�пռ����ݿ������¿ռ�
		if (preserve && arr!=0 && uboundCurrent>=0)
		{
			int ctToCpy;										// ����ԭ�����ݣ���Ҫ�����ڴ�� int Ԫ�ظ���
			ctToCpy = uboundCurrent;
			if (uboundCurrent>toUBound) ctToCpy = toUBound;		// ȡ uboundCurrent �� toUBound ����Сֵ
			ctToCpy = ctToCpy + 1;								// ���� +1����Ϊ uboundCurrent �� toUBound ���������Ͻ�
			memcpy(ptrNew, arr, sizeof(int)*ctToCpy); 
		}
		
		// ɾ��ԭ�пռ�
		if (arr!=0 && uboundCurrent>=0) delete [] arr;
		
		// ָ��ָ���¿ռ�
		arr = ptrNew;
		return toUBound;
	}
	else		// if (toUBound < 0)�������ٿռ䣬ɾ��ԭ�пռ�
	{
		if(arr!=0 && uboundCurrent>=0) delete [] arr;
		arr = 0;
		return 0;
	}
	
}



// �ö��ֲ��ҷ������� recycles �в���Ԫ�� itemToFind ��λ��
// ���� recycles �Ѱ���С�������������±�Ҫ��1��ʼ, recyUsedCount ͬʱΪ�±��Ͻ�
// ��������ֵ��
//   1. ���ҵ�Ԫ���������д��ڣ����ش��ڵ��±꣨>0����
//   2. �粻����Ҫ�ҵ�Ԫ�أ��������ظ�����<0���������ֵΪά�����������Ҫ�������λ��
//      (��Ҫ�����λ�������һ��Ԫ��֮�󣬺������صĸ����ľ���ֵΪԭ����±�+1)
//   3. ������ 0
int CBRecycledArr::FindPosInSortedRecycle( int itemToFind )
{
	int iStart=1, iEnd=recyUsedCount;
	int i;
	
	if (recyUsedCount<1) return -1;	// ���黹û��Ԫ�أ�����-1����ʾ��һ������Ҫ�����뵽�±�Ϊ [1] ��λ�ã������±��1��ʼ��
	while(iStart<=iEnd)
	{
		i = iStart+(iEnd-iStart)/2;
		if (itemToFind > recycles[i])
			iStart = i + 1;
		else if(itemToFind < recycles[i])
			iEnd = i - 1;
		else
			return i;		// �ҵ�Ԫ�أ��±�Ϊ i
	}
	
	// û���ҵ�������Ҫ�����λ�ã�ֱ�ӷ��� iEnd+1 �ĸ�������
	return -(iEnd+1);
}


// �����û� index������� mem[] �е��±ꣻɾ�����ݺ󣬺������� index ���Զ�������ʹ index ��Ϊ 1��Count
// ������ 0
int CBRecycledArr::UserIndexToMemIndex( int index )
{
	// ����� ����վ �м�¼����Щ�±��У��� index С���м��� => i
	int i=FindPosInSortedRecycle(index);
	if (i<0) 
		i=-i-1;
	// else ֱ�ӷ��� index+i
	return index+i;
}








//////////////////////////////////////////////////////////////////////
// CBRecycledArrInt ���ʵ�֣����Ͱ������վ������
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Static ������ֵ
//////////////////////////////////////////////////////////////////////
const int   CBRecycledArrInt::mcIniMemSize = 7;					// ��ʼ mem[] �Ĵ�С
const int   CBRecycledArrInt::mcMaxItemCount = 100000000;		// ���Ԫ�ظ����������ڴ�ԭ��2G�ڴ�֮�ڣ�sizeof(MemType)*��ֵ���ó���2G��
const float CBRecycledArrInt::mcExpandMaxPort = 0.75;			// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
const int   CBRecycledArrInt::mcExpandCountThres = 10000;		// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
const int   CBRecycledArrInt::mcExpandCountThresMax = 10000000;// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int   CBRecycledArrInt::mcExpandBigPer = 1000000;			// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int	CBRecycledArrInt::mcRecyExpandPer = 100;			// ���� recycles[] �ռ�ʱ��ÿ������Ĵ�С
		

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBRecycledArrInt::CBRecycledArrInt(int initSize/*=0*/)
{
	// ��ʼ����Ա������ֵ
	mem = 0;
	memCount = 0; memUsedCount = 0;
	recycles = 0;
	recyCount = 0; recyUsedCount = 0;
	mTravIdxCurr = 0;

	// ���ٳ�ʼ�ռ�
	if (initSize)
	{
		// ���� initSize ��С�Ŀռ�
		RedimArrMemType(mem, initSize);
		memCount = initSize;
	}
	else
	{
		// ���� mcIniMemSize ��С�Ŀռ�
		RedimArrMemType(mem, mcIniMemSize);
		memCount = mcIniMemSize;
	}
	
}

CBRecycledArrInt::~CBRecycledArrInt()
{
	if (mem) delete []mem;
	mem = 0; memCount = 0; memUsedCount = 0;
	if (recycles) delete []recycles;
	recycles = 0; recyCount = 0; recyUsedCount = 0;

	mTravIdxCurr = 0;
} 





//////////////////////////////////////////////////////////////////////
// ���к���
//////////////////////////////////////////////////////////////////////

// ��������ݣ����������ݱ����浽�� mem[] �е��±꣨>0����������0
int CBRecycledArrInt::Add( DataType data, DataIntType dataInt /*= 0*/, DataFloatType dataFloat /*= 0.0*/, DataDoubleType dataDouble /*= 0.0*/ )
{
	int idx = 0;  // Ҫ�������ݴ��� mem[idx]

	// Ϊ�����ݷ����¿ռ䣬��ÿռ��±� idx
	if (recycles!=0 && recyUsedCount>0)
	{
		// ����վ���пռ䣬ʹ�û���վ�е����һ���ռ�
		idx = recycles [recyUsedCount];
		recyUsedCount--;
	}
	else
	{
		// ����վ��û�пռ䣬ʹ�� mem[idx] �е���һ�����ÿռ�
		if (memUsedCount+1 <= memCount)
		{
			memUsedCount++;
			idx = memUsedCount;
		}
		else
		{
			// Redim preserve mem[1 to memCount + һ��] 
			ExpandMem();
			// ����ռ���ٴ��ж�
			if (memUsedCount+1 <= memCount) 
			{
				memUsedCount++;
				idx = memUsedCount;
			}
			else
			{
				// ����ռ�󻹲��� memUsedCount <= memCount���޷�����ռ䣬���س���
				return 0;		
			}
		}
	}

	// ����������
	mem[idx].Data = data;
	mem[idx].DataInt = dataInt;
	mem[idx].DataFloat = dataFloat;
	mem[idx].DataDouble = dataDouble;
	mem[idx].Used = true;

	// ���������ݱ����浽�� mem[] �е��±�
	return idx;
}


bool CBRecycledArrInt::Remove( int index )
{
	if (index<1 || index>memCount) return false;	// index ���Ϸ�
	if (! mem[index].Used) return false;			// �����ѱ�ɾ��

	// ��������ֵΪ��
	mem[index].Data = 0;
	mem[index].DataInt = 0;
	mem[index].DataFloat = 0.0;
	mem[index].DataDouble = 0.0;
	mem[index].Used = false;

	// �ڻ���վ�м�¼�ÿռ��ѱ�ɾ��
	if (recycles==0 || recyUsedCount + 1 > recyCount)
	{
		// �������վ�еĿռ�
		RedimArrInt(recycles, recyCount+mcRecyExpandPer, recyCount, true);
		recyCount = recyCount+mcRecyExpandPer;
	}
	recyUsedCount++;
	recycles[recyUsedCount] = index;

	// ���سɹ�
	return true;
}


CBRecycledArrInt::DataType CBRecycledArrInt::Item( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;	
	}

	return mem[index].Data;
}

CBRecycledArrInt::DataIntType CBRecycledArrInt::ItemInt( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0;	
	}

	return mem[index].DataInt;
}

CBRecycledArrInt::DataFloatType CBRecycledArrInt::ItemFloat( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;	
	}
	
	return mem[index].DataFloat;
}

CBRecycledArrInt::DataDoubleType CBRecycledArrInt::ItemDouble( int index, bool bRaiseErrIfNotHas/*=false*/ )
{
	if (index<1 || index>memCount)	// index ���Ϸ�
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;
	}
	if (! mem[index].Used) 				// �����ѱ�ɾ��
	{
		if (bRaiseErrIfNotHas) throw (unsigned char)5;			// ��Ч�Ĺ��̵��û����
		return 0.0;	
	}
	
	return mem[index].DataDouble;
}



void CBRecycledArrInt::Clear()
{
	if (mem) delete []mem;
	if (recycles) delete []recycles;

	mem = 0; memCount = 0; memUsedCount = 0;
	recycles = 0; recyCount = 0; recyUsedCount = 0;
	mTravIdxCurr = 0;

	// ���� mcIniMemSize ��С�Ŀռ�
	RedimArrMemType(mem, mcIniMemSize);
	memCount = mcIniMemSize;
}



void CBRecycledArrInt::StartTraversal()
{
	mTravIdxCurr = 1;
}


CBRecycledArrInt::DataType CBRecycledArrInt::NextItem( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].Data;}
	}

	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0;
}


CBRecycledArrInt::DataIntType CBRecycledArrInt::NextDataInt( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataInt;}
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0;
}

CBRecycledArrInt::DataFloatType CBRecycledArrInt::NextDataFloat( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataFloat;}
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0.0;
}

CBRecycledArrInt::DataDoubleType CBRecycledArrInt::NextDataDouble( bool &bRetNotValid )
{
	bRetNotValid = true;	// Ԥ�践�����ݳɹ�
	for (; mTravIdxCurr<=memCount; mTravIdxCurr++)
	{
		if (mem[mTravIdxCurr].Used) {mTravIdxCurr++; return mem[mTravIdxCurr-1].DataDouble; }
	}
	
	// �������ݽ�������ʱ��������ֵ��Ч
	bRetNotValid = false;	
	return 0.0;
}


// ���ص�ǰӵ�е����ݸ���
int CBRecycledArrInt::Count()
{
	int ct = memUsedCount-recyUsedCount;
	if (ct<0) ct = 0;
	return ct;
}



//////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////

void CBRecycledArrInt::ExpandMem( void )
{
	int toCount = 0;
	if (memCount < mcExpandCountThres)
	{
		// ��������������Ƚ��١����������ռ�Ϊԭ����2��
		toCount = memCount * 2;
	}
	else if (memCount < mcExpandCountThresMax)
	{
		// ������������Ѿ����е�ࡱ���������ռ�Ϊԭ����1.5��
		toCount = memCount * 3 / 2;
	}
	else
	{
		// ��������������ܶࡱ�������� mcExpandBigPer ���ռ�
		toCount = memCount + mcExpandBigPer;
	}
	
	// �ض��������С
	// ReDim Preserve mem(1 To toCount)
	RedimArrMemType(mem, toCount, memCount, true);
	memCount = toCount;
}




// �ض��� һ�� MemType ���͵����飨������� recycles���Ĵ�С���¶���ռ��Զ�����
// arr��Ϊ����ָ�룬�ɴ��ݣ�recycles�����������޸Ĵ�ָ���ָ��
// toUBound��ΪҪ�ض����������Ͻ磬����Ϊ��[0] to [toUBound]��Ϊ -1 ʱ�����ٿռ䣬������ɾ��ԭ
//	 �ռ䣬�� arr �ᱻ��Ϊ0
// uboundCurrent��Ϊ�ض���ǰ������Ͻ� [0] to [uboundCurrent]��Ϊ -1 ��ʾ��δ���ٹ��ռ�Ϊ��һ�ε���
// preserve����������ԭʼ���ݷ��򲻱���
// �����¿ռ��ϱ꣬�� toUBound
int CBRecycledArrInt::RedimArrMemType( MemType * &arr, int toUBound/*=-1*/, int uboundCurrent/*=-1*/, bool preserve/*=false*/ )
{
	// �����¿ռ䣺[0] to [toUBound]
	if (toUBound >= 0)
	{
		MemType * ptrNew = new MemType [toUBound + 1];		// +1 Ϊʹ�����±���� toUBound
		// �¿ռ�����
		memset(ptrNew, 0, sizeof(MemType)*(toUBound + 1));
		
		// ��ԭ�пռ����ݿ������¿ռ�
		if (preserve && arr!=0 && uboundCurrent>=0)
		{
			int ctToCpy;										// ����ԭ�����ݣ���Ҫ�����ڴ�� MemType Ԫ�ظ���
			ctToCpy = uboundCurrent;
			if (uboundCurrent>toUBound) ctToCpy = toUBound;		// ȡ uboundCurrent �� toUBound ����Сֵ
			ctToCpy = ctToCpy + 1;								// ���� +1����Ϊ uboundCurrent �� toUBound ���������Ͻ�
			memcpy(ptrNew, arr, sizeof(MemType)*ctToCpy); 
		}
		
		// ɾ��ԭ�пռ�
		if (arr!=0 && uboundCurrent>=0) delete [] arr;
		
		// ָ��ָ���¿ռ�
		arr = ptrNew;
		return toUBound;
	}
	else		// if (toUBound < 0)�������ٿռ䣬ɾ��ԭ�пռ�
	{
		if(arr!=0 && uboundCurrent>=0) delete [] arr;
		arr = 0;
		return 0;
	}
	
}


// �ض��� һ�� int ���͵����飨������� mem[]���Ĵ�С���¶���ռ��Զ�����
// arr��Ϊ����ָ�룬�ɴ��ݣ�mem�����������޸Ĵ�ָ���ָ��
// toUBound��ΪҪ�ض����������Ͻ磬����Ϊ��[0] to [toUBound]��Ϊ -1 ʱ�����ٿռ䣬������ɾ��ԭ
//	 �ռ䣬�� arr �ᱻ��Ϊ0
// uboundCurrent��Ϊ�ض���ǰ������Ͻ� [0] to [uboundCurrent]��Ϊ -1 ��ʾ��δ���ٹ��ռ�Ϊ��һ�ε���
// preserve����������ԭʼ���ݷ��򲻱���
// �����¿ռ��ϱ꣬�� toUBound
int CBRecycledArrInt::RedimArrInt( int * &arr, int toUBound/*=-1*/, int uboundCurrent/*=-1*/, bool preserve/*=false*/ )
{
	// �����¿ռ䣺[0] to [toUBound]
	if (toUBound >= 0)
	{
		int * ptrNew = new int [toUBound + 1];		// +1 Ϊʹ�����±���� toUBound
		// �¿ռ�����
		memset(ptrNew, 0, sizeof(int)*(toUBound + 1));
		
		// ��ԭ�пռ����ݿ������¿ռ�
		if (preserve && arr!=0 && uboundCurrent>=0)
		{
			int ctToCpy;										// ����ԭ�����ݣ���Ҫ�����ڴ�� int Ԫ�ظ���
			ctToCpy = uboundCurrent;
			if (uboundCurrent>toUBound) ctToCpy = toUBound;		// ȡ uboundCurrent �� toUBound ����Сֵ
			ctToCpy = ctToCpy + 1;								// ���� +1����Ϊ uboundCurrent �� toUBound ���������Ͻ�
			memcpy(ptrNew, arr, sizeof(int)*ctToCpy); 
		}
		
		// ɾ��ԭ�пռ�
		if (arr!=0 && uboundCurrent>=0) delete [] arr;
		
		// ָ��ָ���¿ռ�
		arr = ptrNew;
		return toUBound;
	}
	else		// if (toUBound < 0)�������ٿռ䣬ɾ��ԭ�пռ�
	{
		if(arr!=0 && uboundCurrent>=0) delete [] arr;
		arr = 0;
		return 0;
	}
	
}



//////////////////////////////////////////////////////////////////////
// BBMPLoad.cpp: CBBMPLoad ���ʵ��
//
//////////////////////////////////////////////////////////////////////

#include "BBMPLoad.h"
#include <memory.h>
#include "mdlFileSys.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBBMPLoad::CBBMPLoad( LPCTSTR szBmpFile/*=NULL*/ )
{
	if (szBmpFile)
	{
		m_szBmpFile = new TCHAR[lstrlen(szBmpFile) + 1];
		lstrcpy(m_szBmpFile, szBmpFile);
	}
	else
	{
		m_szBmpFile = NULL;
	}

	// ������ʼ��
	m_ErrDesp = 0;
	m_hBitmap = 0;
	m_ptrPixels = 0;
	m_pLines = 0;
	memset(&m_bm, 0, sizeof(m_bm));
}

CBBMPLoad::~CBBMPLoad()
{
	ReleaseResource();
	delete [] m_szBmpFile;	m_szBmpFile = 0;	// �ͷű��� BMP �ļ����� fileBmp �ַ����ռ�
	delete [] m_ErrDesp;	m_ErrDesp = 0;		// �ͷű��������Ϣ�Ŀռ�
}


//////////////////////////////////////////////////////////////////////
// ���к���
//////////////////////////////////////////////////////////////////////


bool CBBMPLoad::LoadBitmapFile( LPCTSTR szBmpFile/*=NULL*/ )
{
	int r=0, i=0;

	// ȷ�� bmp �ļ��������� m_fileBmp
	if (szBmpFile==NULL)
	{
		if (m_szBmpFile==NULL) return false;	// û��ָ���ļ���
		// else ����ʹ�� m_szBmpFile ��Ϊ�ļ���
	}
	else
	{
		// ������ szBmpFile �滻 m_szBmpFile
		if (m_szBmpFile) delete [] m_szBmpFile;
		m_szBmpFile = new TCHAR[lstrlen(szBmpFile) + 1];
		lstrcpy(m_szBmpFile, szBmpFile);
	}
	
	// �ж��ļ��Ƿ����
	if (FMFileExist(m_szBmpFile) != 1)
	{
		SetErrDesp(TEXT("�ļ������ڡ�"));		// ���ñ�����Ĵ�����Ϣ
		return false;
	}
	
	// ����������Ѿ��򿪹�λͼ���������ͷ�����Ȼ���ٴ���λͼ
	if (m_hBitmap) ReleaseResource(false);	// ������Ϊ false ��ʾ���ͷ� m_szBmpFile �Ŀռ�

	// ���� bmp ͼƬ�ļ���ʹ��ԭʼͼƬ��С��ʹ���豸�޹�λͼ�������λͼ���
	m_hBitmap = (HBITMAP)LoadImage(0, m_szBmpFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (m_hBitmap == 0)
	{
		SetErrDesp(TEXT("���� bmp ͼƬ�ļ�ʧ�ܡ�"));	// ���ñ�����Ĵ�����Ϣ
		return false;
	}


    // ���λͼ��Ϣ
	if (GetObject(m_hBitmap, sizeof(m_bm), &m_bm) == 0)
	{
		SetErrDesp(TEXT("���λͼ��Ϣʧ�ܡ�"));	// ���ñ�����Ĵ�����Ϣ
		ReleaseResource();						// �ͷ��Ѽ��ص�λͼ��Դ
		return false;
	}


	// ֻ�ܴ���ÿ����8λ���µ�λͼ
	if (m_bm.bmBitsPixel > 8)
	{
		TCHAR strtmp[300];
		wsprintf(strtmp, TEXT("��������ֻ�ܴ���λ�����8λ���£���8λ����λͼ��λ�����ָÿ����ռ�ı���λ��������λͼλ���Ϊ %d λ���ݲ��ܱ�����"), m_bm.bmBitsPixel);
		SetErrDesp(strtmp);			// ���ñ�����Ĵ�����Ϣ
		ReleaseResource();			// �ͷ��Ѽ��ص�λͼ��Դ
		return false;
	}


	// ���ٴ洢�� m_ptrPixels����λͼ���ݿ����� m_ptrPixels
	// �Ƚ�λͼ���ݿ����� buffBytes��Ȼ���ٸ��ݲ�ͬ��λͼ��ʽ�������� m_ptrPixels
	int buffBytesCount = m_bm.bmWidthBytes * m_bm.bmHeight;
	unsigned char * buffBytes = new unsigned char [buffBytesCount];
	buffBytesCount = GetBitmapBits(m_hBitmap, buffBytesCount, buffBytes);
	if (buffBytesCount == 0)
	{
		SetErrDesp(TEXT("�޷���ȡλͼͼ�����ݡ�"));		// ���ñ�����Ĵ�����Ϣ
		ReleaseResource();							// �ͷ��Ѽ��ص�λͼ��Դ
		return false;
	}
	
	// ���� m_Pixels �洢��������λͼ��ʽ��m_Pixels һ��һ���ֽڣ���һ������Ԫ�أ���һ�����أ��ʱ�����ֻ��8λ���������µ�λͼ
	m_ptrPixels = new unsigned char [m_bm.bmWidth * m_bm.bmHeight];
	// ���� m_pLines ָ�����飬��ʹ�����Ԫ��ָ��λͼ���صĸ��У�ʵ��Ϊ m_ptrPixels ָ��Ŀռ���м�ĳ�����֣�
	m_pLines = new unsigned char * [m_bm.bmHeight];
	for (i=0; i<m_bm.bmHeight; i++)
		m_pLines[i] = m_ptrPixels + m_bm.bmWidth*i;

	
	// �� buffBytes ������ m_ptrPixels ����
	if (m_bm.bmBitsPixel >= 8)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 8λλͼ�����ϣ�ÿ����ռ1�ֽڻ�����
		// m_bm.bmWidthBytes Ϊһ��ɨ���е��ֽ���������ɨ�������ֽڶ��룬��Щ�ֽڲ�һ������һ����Ч���ص��ֽ�
		// m_bm.bmWidth * m_bm.bmBitsPixel / 8 ����һ������ʵ�����õ��ֽ���
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (m_bm.bmWidthBytes == m_bm.bmWidth * m_bm.bmBitsPixel / 8)
		{
			// ======== ÿ���ֽ���������Ч���أ�ֱ�ӿ����� m_ptrPixels ���� ========
			memcpy(m_ptrPixels, buffBytes, buffBytesCount);
		}
		else		// if (m_bm.bmWidthBytes == m_bm.bmWidth)
		{
			// ======== ÿ���ֽ��� ��һ��������Ч���أ�����п��� ========
			int iBytesPerLine = buffBytesCount / m_bm.bmHeight;	// ���� m_bm.bmWidthBytes��buffBytesCount ����Ϊ GetBitmapBits ʵ�ʻ�õ��ֽ���
			for (r=0; r<m_bm.bmHeight; r++)
			{
				// ÿ��ѭ������һ��
				memcpy(m_ptrPixels + r*m_bm.bmWidth, (void *)(buffBytes + r*iBytesPerLine), m_bm.bmWidth* m_bm.bmBitsPixel / 8);
			}	// end for (int r=1; r<=m_bm.bmHeight; r++)

		}			// end if (m_bm.bmWidthBytes == m_bm.bmWidth)
	}
	else				// if (m_bm.bmBitsPixel == 8)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 8λλͼ���£�ÿ����ռ�ֽ������� 1 ���ֽ�
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (r=0; r<m_bm.bmHeight; r++)
		{
			// ============ ÿ��ѭ������һ�� ============
			int c=0;		// �� c ��0��ʼ
			// ��ǰ��Ҫ�ĵ� r �е���ʼ�������ڵ��ֽ�Ϊ buffBytes[i]����� i ֵ
			i = r * buffBytesCount / m_bm.bmHeight;     // ���� mbm.bmWidthBytes��buffBytesCount ����Ϊ GetBitmapBits ʵ�ʻ�õ��ֽ���
			
			// ��õ� r �е������е�����ֵ => m_ptrPixels[r][ ]
			while(c < m_bm.bmWidth)
			{
				int rk = ((1 << m_bm.bmBitsPixel) - 1) << 8;	// �� & ��λ��������һ�����صġ�λ�����ݵ�����
				int rkMoveRight=8;								// �� And ��λ��������һ�����صġ�λ�����ݺ󣬻�Ҫ��֮�����ơ������λ�����ƶ���λ���������� unsigned char
				while(c < m_bm.bmWidth)
				{
					rkMoveRight -= m_bm.bmBitsPixel; 
					if (rkMoveRight < 0) break;		// ���ֽ�ʣ��λ������
					rk >>= m_bm.bmBitsPixel; 
					*(m_ptrPixels + r*m_bm.bmWidth + c) = (buffBytes[i] & rk) >> rkMoveRight;	// m_ptrPixels[r][c]=...
					c = c + 1;
				}

				i++;
			}	// end do while(c<=m_bm.bmWidth)
		}			// end for (r=0; r<m_bm.bmHeight; r++)
	}					// end if (m_bm.bmBitsPixel == 8)

	return true;
}



// �ͷ���Դ
// bFreeSzBmpFile==true ʱ��Ҳ�ͷű����ļ����� m_szBmpFile ����Դ�������ͷ� m_szBmpFile�������ͷ�������Դ
void CBBMPLoad::ReleaseResource(bool bFreeSzBmpFile/*=true*/)
{
	// ɾ��λͼ����
	if (m_hBitmap) 
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = 0;
	}

	// ɾ������λͼ�ļ��ַ����� m_szBmpFile
	if (bFreeSzBmpFile)
	{
		if (m_szBmpFile) delete []m_szBmpFile;
		m_szBmpFile = 0;
	}

	// �ͷ��ڴ���Դ
	delete [] m_pLines;		 m_pLines = 0;		// ɾ��λͼÿ�����ص���ʼ��ַ���ڵ� ָ������
	delete [] m_ptrPixels;	 m_ptrPixels = 0;	// ɾ��λͼ��������
	memset(&m_bm, 0, sizeof(m_bm));				// ��� m_bm
}















//////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////

// ���ô�����Ϣ�ַ������Զ����ٿռ䣬���浽 m_ErrDesp
void CBBMPLoad::SetErrDesp( LPCTSTR errDesp )
{ 
	if (m_ErrDesp) delete [] m_ErrDesp;				// ���ԭ�пռ�

	m_ErrDesp = new TCHAR [lstrlen(errDesp) + 1];		// ���ٱ��������Ϣ�Ŀռ䣨�ÿռ佫�ڶ�������ʱ�ͷţ�
	lstrcpy(m_ErrDesp, errDesp);						// ��������Ϣ���浽�¿��ٵĿռ�
}

LPTSTR CBBMPLoad::GetErrDesp()
{
	if (m_ErrDesp==0) 
	{
		// ��Ϊ���ַ���
		m_ErrDesp = new TCHAR [2];
		*m_ErrDesp = 0;
	}
	return m_ErrDesp;
}

unsigned char ** CBBMPLoad::GetBMPBytes()
{
	return m_pLines;	
}


// ����λͼ��ȣ��������� LoadBitmapFile �����Ч
long CBBMPLoad::Width()
{
	return m_bm.bmWidth;
}

// ����λͼ�߶ȣ��������� LoadBitmapFile �����Ч
long CBBMPLoad::Height()
{
	return m_bm.bmHeight;
}

long CBBMPLoad::Type()
{
	return m_bm.bmType;
}

long CBBMPLoad::WidthBytes()
{
	return m_bm.bmWidthBytes;
}

WORD CBBMPLoad::Planes()
{
	return m_bm.bmPlanes;
}

WORD CBBMPLoad::BitsPixel()
{
	return m_bm.bmBitsPixel;
}












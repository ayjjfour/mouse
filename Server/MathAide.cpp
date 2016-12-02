#include "StdAfx.h"
#include "Mathaide.h"
#include "math.h"


//���캯��
CMathAide::CMathAide(void)
{
}

//��������
CMathAide::~CMathAide(void)
{
}


//����׳�
LONG CMathAide::Factorial(int nNumber)
{
	int nFactorial=1;
	int nTemp=nNumber;
	for(int nIndex=0; nIndex<nNumber; nIndex++)
	{
		nFactorial*=nTemp;
		nTemp--;
	}
	return nFactorial;
}

//�������
LONG CMathAide::Combination(int nCount,int nR)
{
	LONG lResult;
	lResult=Factorial(nCount)/((Factorial(nR))*Factorial(nCount-nR));

	return lResult;
}

//�������
LONG CMathAide::CalcDistance(int nX1, int nY1, int nX2, int nY2)
{
	LONG lSqure=(nX1-nX2)*(nX1-nX2)+(nY1-nY2)*(nY1-nY2);
	float fTmpValue=sqrt(float(lSqure));

	return LONG(fTmpValue);
}

//����ֱ��
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound)
{
	//���ñ���
	nLinearCount=0;

	//�Ϸ��ж�
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//����Ƕ�
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//��������
	nLinearCount=1;
	ptLinear[0].x=nInitX[0];
	ptLinear[0].y=nInitY[0];
	LONG lDis=0;
	float fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[0].x-sin(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].x+sin(fAngle)*(nDistance*nLinearCount);

		if (LONG(fTmpValue)>=rcBound.right || LONG(fTmpValue)<=rcBound.left) break;
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[0].y-cos(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].y+cos(fAngle)*(nDistance*nLinearCount);

		if (LONG(fTmpValue)>=rcBound.bottom || LONG(fTmpValue)<=rcBound.top) break;
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}
}

//����ֱ��
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//���ñ���
	nLinearCount=0;

	//�Ϸ��ж�
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//����Ƕ�
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//��������
	nLinearCount=1;
	ptLinear[0].x=nInitX[0];
	ptLinear[0].y=nInitY[0];
	LONG lDis=0;
	float fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[0].x-sin(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].x+sin(fAngle)*(nDistance*nLinearCount);
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[0].y-cos(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].y+cos(fAngle)*(nDistance*nLinearCount);
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}

	/*while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[nLinearCount-1].x-sin(fAngle)*nDistance;            
		else
			fTmpValue=ptLinear[nLinearCount-1].x+sin(fAngle)*nDistance;
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[nLinearCount-1].y-cos(fAngle)*nDistance;
		else
			fTmpValue=ptLinear[nLinearCount-1].y+cos(fAngle)*nDistance;
		ptLinear[nLinearCount].y=LONG(fTmpValue);
		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}*/
}

//��������
void CMathAide::BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance)
{

	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//��ʼ����
	nBezierCount=0;

	//��������
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;

	//���ɱ�����
	while(t<=1)
	{
		dTempX=0.0;
		dTempY=0.0;
		i=0;
		while(i<=n)
		{
			dTempX+=(nInitX[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			dTempY+=(nInitY[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			i++;
		}

		//�Ϸ��ж�
		if (nBezierCount>=nBuffertCount) return;

		// * /add for angle
	/*	dTempX *= 10;
		dTempY *= 10;
		nDistance = nExDistance * 10;
		*/
		//�жϾ���
		int nPtSpace=0;
		if (nBezierCount>0) nPtSpace=CalcDistance(ptBezier[nBezierCount-1].x, ptBezier[nBezierCount-1].y, int(dTempX), int(dTempY));
		if (nPtSpace>=nDistance || 0==nBezierCount)
		{
			// * /LeonLv add for angle, 13Feb.2011

			// * /End
			ptBezier[nBezierCount].x=(long)(dTempX);
			ptBezier[nBezierCount].y=(long)(dTempY);
			++nBezierCount;		// * /LeonLv modify for coding rule, 11Feb.2011
		}
		// * /t+=0.00001;
		t += 0.002;		// * /LeonLv modify step as 500, 12Feb.2011
		// * /t += 0.01;
	}
	return ;
}

// * /LeonLv add:���Լ���Ƕȵľ�ȷ��������
void CMathAide::BuildExactBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance)
{

	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//��ʼ����
	nBezierCount=0;

	//��������
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;

	if (n < 1)
	{	// * /����, 09May.2011
		return;
	}

	//���ɱ�����
	while(t<=1)
	{
		dTempX=0.0;
		dTempY=0.0;
		i=0;
		while(i<=n)
		{
			dTempX+=(nInitX[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			dTempY+=(nInitY[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			i++;
		}

		//�Ϸ��ж�
		if (nBezierCount>=nBuffertCount) return;

		// * /add for angle
		/*	dTempX *= 10;
		dTempY *= 10;
		nDistance = nExDistance * 10;
		*/
		//�жϾ���
		int nPtSpace=0;
		if (nBezierCount>0) nPtSpace=CalcDistance(ptBezier[nBezierCount-1].x, ptBezier[nBezierCount-1].y, int(dTempX), int(dTempY));
		if (nPtSpace>=nDistance || 0==nBezierCount)
		{
			// * /LeonLv add for angle, 13Feb.2011
			ptAngle[nBezierCount].x=(long)(dTempX*10);
			ptAngle[nBezierCount].y=(long)(dTempY*10);
			// * /End
			ptBezier[nBezierCount].x=(long)(dTempX);
			ptBezier[nBezierCount].y=(long)(dTempY);
			++nBezierCount;		// * /LeonLv modify for coding rule, 11Feb.2011
		}
		// * /t+=0.00001;
		t += 0.002;		// * /LeonLv modify step as 500, 12Feb.2011
		// * /t += 0.002;
		// * /t += 0.01;
	}
	return ;
}

// * /LeonLv add: ������������ϣ�06May.2011
void CMathAide::BuildFittingBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance, CRect rcBound)
{
	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//��������
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;
	int  arKeyPointX[32] = {0};				// * /���ߵ����п��Ƶ�
	int  arKeyPointY[32] = {0};				// * /���ߵ����п��Ƶ�
	int nKeyCount = (2 * nInitCount - 1);
	int nBzNum = 2;							// * /���������ߴ���
	int nDivNum = 1;						// * /
	int nBzBias = 0;
	int nDivCount = 0;
	int nDivMaxCount = 0;

	if (n < 1 || 32 <= nKeyCount)
	{
		return;
	}

/*	if (1 == n)
	{	// * /ֱ�߹켣
		BuildLinear(nInitX, nInitY, nInitCount, ptBezier, nBuffertCount, nBezierCount, nDistance, rcBound);
		return;
	} */
	// * /��չ��ϱ��������ߵĹؼ��㣬����ԭ���ǣ���һ���ؼ�������һ����һ���ؼ��㣨����������ã�����ڶ��ؼ��㹹��һ�����α��������ߣ�
	// * /Ȼ���һ���ؼ�����ڶ��ؼ���Ⱦ��ӳ��󹹳ɵڶ����ؼ��㣬�������������ؼ��㹹��һ�����������ߣ���������Ӧ�����Զ���ϵģ���������
	arKeyPointX[0] = nInitX[0];
	arKeyPointY[0] = nInitY[0];
/*	if (3 >= nInitCount)
	{
		arKeyPointX[1] = (nInitX[n] + nInitX[0]) / 2;
		arKeyPointY[1] = (nInitY[n] + nInitY[0]) / 2;
	} 
	else
	{
		arKeyPointX[1] = (nInitX[2] + nInitX[0]) / 2;
		arKeyPointY[1] = (nInitY[2] + nInitY[0]) / 2;
	}
	/*/
	if (1 == n)
	{
		arKeyPointX[1] = (nInitX[n] + nInitX[0]) / 2;
		arKeyPointY[1] = (nInitY[n] + nInitY[0]) / 2;
	} 
	else
	{
		arKeyPointX[1] = (nInitX[2] + nInitX[0]) / 2;
		arKeyPointY[1] = (nInitY[2] + nInitY[1]) / 2;
	}

	for (i = 2; i < nKeyCount; ++i)
	{
		if (0 == i % 2)
		{
			arKeyPointX[i] = nInitX[i/2];
			arKeyPointY[i] = nInitY[i/2];
		} 
		else
		{	// * /ǰ����Ƶ���ӳ���
			arKeyPointX[i] = arKeyPointX[i - 1] + (arKeyPointX[i - 1] - arKeyPointX[i - 2]);
			arKeyPointY[i] = arKeyPointY[i - 1] + (arKeyPointY[i - 1] - arKeyPointY[i - 2]);
			if (arKeyPointX[i] < rcBound.left)
			{
				arKeyPointX[i] = rcBound.left;
			}
			else if (rcBound.right < arKeyPointX[i])
			{
				arKeyPointX[i] = rcBound.right;
			}
			if (arKeyPointY[i] < rcBound.top)
			{
				arKeyPointY[i] = rcBound.top;
			} 
			else if (rcBound.bottom < arKeyPointY[i])
			{
				arKeyPointY[i] = rcBound.bottom;
			}
		}
	}

	// * /��ʼ���ܹ켣����
	nBezierCount = 0;
	nDivNum = (nInitCount * 2 - 1) / 2;		// * /������Ҫ��ϵ�����
	nDivMaxCount = nBuffertCount / nDivNum;
	nBzBias = 0;

	while (nDivNum > 0)
	{
		nDivCount = 0;
		t = 0.0f;
		//���ɱ�����
		while(t <= 1)
		{
			dTempX=0.0;
			dTempY=0.0;
			i = 0;
			while(i <= nBzNum)
			{
				dTempX+=(arKeyPointX[nBzBias + i] * pow(t, i) * pow((1-t),(nBzNum - i)) * Combination(nBzNum, i));
				dTempY+=(arKeyPointY[nBzBias + i] * pow(t, i) * pow((1-t),(nBzNum - i)) * Combination(nBzNum, i));
				++i;
			}

			//�Ϸ��ж�
			if (nBezierCount>=nBuffertCount/* || nDivCount >= nDivMaxCount*/)
				break;

			//�жϾ���
			int nPtSpace=0;
			if (nBezierCount>0) nPtSpace=CalcDistance(ptBezier[nBezierCount-1].x, ptBezier[nBezierCount-1].y, int(dTempX), int(dTempY));
			if (nPtSpace>=nDistance || 0==nBezierCount)
			{
				// * /LeonLv add for angle, 13Feb.2011
				ptAngle[nBezierCount].x=(long)(dTempX*100);
				ptAngle[nBezierCount].y=(long)(dTempY*100);
				// * /End
				ptBezier[nBezierCount].x=(long)(dTempX);
				ptBezier[nBezierCount].y=(long)(dTempY);
				++nBezierCount;		// * /LeonLv modify for coding rule, 11Feb.2011
				++nDivCount;
			}
			// * /t+=0.00001;
			t += 0.004;		// * /LeonLv modify step as 500, 12Feb.2011
			// * /t += 0.002;
			// * /t += 0.01;

		}
		--nDivNum;
		nBzBias += 2;

		if (nBezierCount >= nBuffertCount)
			return;
	}
	return ;
}


// * /LeonLv add:��ֱֱ���ζ���ˮĸ�������ɵ�
void CMathAide::BuildReturnLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//���ñ���
	nLinearCount=0;

	//�Ϸ��ж�
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	// * /�����һ������λ�ã�
	//����Ƕ�
	LONG lDisTotal=CalcDistance(nInitX[1],nInitY[1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;

	nLinearCount=1;
	ptLinear[0].x=nInitX[0];
	ptLinear[0].y=nInitY[0];
	LONG lDis=0;
	float fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
/*		if (nInitX[1]<nInitX[0])
			fTmpValue=ptLinear[0].x;	// * /-sin(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].x;	// * /+sin(fAngle)*(nDistance*nLinearCount);
*/
		ptLinear[nLinearCount].x = nInitX[0];//ptLinear[0].x;	//LONG(fTmpValue);

		if (nInitY[1]<nInitY[0])
			fTmpValue=(float)(ptLinear[0].y-/* cos(fAngle)* */(nDistance*nLinearCount));
		else
			fTmpValue=(float)(ptLinear[0].y+/* cos(fAngle)* */(nDistance*nLinearCount));
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}
	
	if (nInitCount < 3)	// * /ֻ��һ��
		return;

	// * /����ڶ�������λ�ã�
	//����Ƕ�
	int nReturnPos = nLinearCount;		// * /LeonLv add:�۵����꣬28Feb.2011
	lDisTotal=CalcDistance(nInitX[2],nInitY[2],nInitX[1],nInitY[1]);
	if (lDisTotal<=0) return;
// * /	fCos=(labs(nInitY[2]-nInitY[1])*1.f)/lDisTotal;
// * /	fAngle=float(acos(double(fCos)));

	//��������
	nLinearCount += 1;
	ptLinear[nLinearCount-1].x=nInitX[1];
	ptLinear[nLinearCount-1].y=nInitY[1];
	lDis=0;
	fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
/*		if (nInitX[2]<nInitX[1])
			fTmpValue=ptLinear[nReturnPos].x;	// -sin(fAngle)*(nDistance*(nLinearCount - nReturnPos));
		else
			fTmpValue=ptLinear[nReturnPos].x;	//+sin(fAngle)*(nDistance*(nLinearCount - nReturnPos));
*/
		ptLinear[nLinearCount].x = nInitX[1];//ptLinear[nReturnPos].x;//LONG(fTmpValue);

		if (nInitY[2]<nInitY[1])
			fTmpValue=(float)(ptLinear[nReturnPos].y-/*cos(fAngle)* */(nDistance*(nLinearCount - nReturnPos)));
		else
			fTmpValue=(float)(ptLinear[nReturnPos].y+/*cos(fAngle)* */(nDistance*(nLinearCount - nReturnPos)));
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[nReturnPos].x,ptLinear[nReturnPos].y);
	}

}

// * /LeonLv add:��ֹ���������鱴�������ɵ�
void CMathAide::BuildStayPoint(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//����Ƕ�
	float fAngle = 0;

	//���ñ���
	nLinearCount=0;

	//�Ϸ��ж�
	if (nInitCount < 2) 
		return ;

	//��������
	nLinearCount = 1;
	ptLinear[0].x = nInitX[0];
	ptLinear[0].y = nInitY[0];
	while (nLinearCount < nBuffertCount)
	{
		ptLinear[nLinearCount].x = nInitX[0];
		ptLinear[nLinearCount].y = nInitY[0];

		++nLinearCount;
	}
}

// * /������ĺ����п���ʵ�ֲ���һ����һ������֮�ڵ�ƽ���ֲ����������������4λС����
double CMathAide::AverageRandom(double min, double max)
{
	return (int)((genrand_real2() * (max - min) + min) * 10000) / 10000;

	/*int minInteger = (int)(min*10000);

	int maxInteger = (int)(max*10000);

	int randInteger = rand()*rand();

	int diffInteger = maxInteger - minInteger;

	int resultInteger = randInteger % diffInteger + minInteger;

	return resultInteger/10000.0;*/
}

// * /������ĺ����п���ʵ�ֲ���һ����һ������֮�ڵ�ƽ���ֲ����������
int CMathAide::nAverageRandom(int min, int max)
{
	return (int)(genrand_real2() * (max - min) + min);

	//int diffInteger = max - min;

	//int randInteger = (rand());//(rand()%diffInteger);

	//int resultInteger = randInteger % diffInteger + min;

	//return resultInteger;
}

// * /V2012 Add, 12Sep.2012
// * /��ȡ����Ƕ�
float CMathAide::GetReflectAngle(BYTE byStepDistance, int nPosX, int nPosY, double fInAngle, int nButtom, int nRightLimit)
{
	// * /comment:���ӵ�����Ƕȼ���ʱ���ӵ��Ƕ�Ϊ�����ڿ�-Y����ʱ��нǦ�/3�����ӵ��нǼ�����Ϊ7��/6��
	// * /�����ӵ�ʱ�ô˽ǶȻ��ƣ�ԭ�����ӵ��زģ��պñ��ڽǶ�˳ʱ��ƫ�Ʀ�/2��
	double fReflectAngle = 0;	// * /����Ƕ�
	bool bTwoSide = false;			// * /�����Ե
	bool bUpAndDown = false;		// * /���±�
	// * /��ײ�ϲ�ͬ�ı�Ե���Ƕȼ�����ȫ��ͬ��������ֲ�ͬ��Ե�ͺ���Ҫ��1st&2nd quadrant
	// * /���ײ�ϵ��ǽǣ���Ҫ���⴦��Ϊ����

	// * /�������ұڷ������϶��ǵ�һ���������໥ƫ�ƣ����������������໥ƫ�ƣ�ƫ�ƾ��ԽǶ�Ϊ2��(��/2 - X)
	if (nPosX <= byStepDistance || (nRightLimit - byStepDistance) <= nPosX)
	{//���ұڱ�׼�ķ���ƫ�Ƽ��㹫ʽΪ��X+2*(��/2 -X)����һ�����޺��������޼��㷽��һ��
		fReflectAngle = fInAngle +  2*(M_PI/2 - fInAngle);	//��-X
		bTwoSide = true;
	} 

	// * /�������±ڷ������϶��ǵ�һ���������໥ƫ�ƣ����߶����������໥ƫ�ƣ�ƫ�ƾ��ԽǶ�Ϊ2��X
	if ((nPosY <= byStepDistance || (nButtom -byStepDistance) <= nPosY))
	{		//���㹫ʽΪ��X - 2��X
		fReflectAngle = fInAngle -  2*fInAngle;
		bUpAndDown = true;
	}
	if (!bTwoSide && !bUpAndDown)
	{
		return INVALID_ANGLE;	// * /�޷����
	}

	// * /���λ�ڽ����򣬽Ƕȷ�ת���
	if (bTwoSide && bUpAndDown)
	{
		fReflectAngle = M_PI + fInAngle;
	}
	// * /�Ƕȹ淶��2����
/*	if (fReflectAngle > 2*M_PI){
		int nMultip = (int)(fReflectAngle / 2*M_PI);
		fReflectAngle -= nMultip * 2 * M_PI;
	}
	if (fReflectAngle < 0){
		int nMultip = (int)(-fReflectAngle / 2*M_PI) + 1;
		fReflectAngle += nMultip * 2 * M_PI;
	}*/
	return fReflectAngle;
}
// * /V2012 End

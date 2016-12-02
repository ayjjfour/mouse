#include "StdAfx.h"
#include "Mathaide.h"
#include "math.h"


//构造函数
CMathAide::CMathAide(void)
{
}

//析构函数
CMathAide::~CMathAide(void)
{
}


//计算阶乘
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

//计算组合
LONG CMathAide::Combination(int nCount,int nR)
{
	LONG lResult;
	lResult=Factorial(nCount)/((Factorial(nR))*Factorial(nCount-nR));

	return lResult;
}

//计算距离
LONG CMathAide::CalcDistance(int nX1, int nY1, int nX2, int nY2)
{
	LONG lSqure=(nX1-nX2)*(nX1-nX2)+(nY1-nY2)*(nY1-nY2);
	float fTmpValue=sqrt(float(lSqure));

	return LONG(fTmpValue);
}

//建立直线
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound)
{
	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//计算角度
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//计算坐标
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

//建立直线
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//计算角度
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//计算坐标
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

//贝塞尔点
void CMathAide::BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance)
{

	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//初始变量
	nBezierCount=0;

	//变量定义
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;

	//生成贝塞尔
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

		//合法判断
		if (nBezierCount>=nBuffertCount) return;

		// * /add for angle
	/*	dTempX *= 10;
		dTempY *= 10;
		nDistance = nExDistance * 10;
		*/
		//判断距离
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

// * /LeonLv add:可以计算角度的精确贝塞尔点
void CMathAide::BuildExactBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance)
{

	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//初始变量
	nBezierCount=0;

	//变量定义
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;

	if (n < 1)
	{	// * /检验, 09May.2011
		return;
	}

	//生成贝塞尔
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

		//合法判断
		if (nBezierCount>=nBuffertCount) return;

		// * /add for angle
		/*	dTempX *= 10;
		dTempY *= 10;
		nDistance = nExDistance * 10;
		*/
		//判断距离
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

// * /LeonLv add: 贝塞尔曲线拟合，06May.2011
void CMathAide::BuildFittingBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance, CRect rcBound)
{
	// * /LeonLv add for test, 11Feb.2011
	//nInitCount = 2;
	int nExDistance = nDistance;

	//变量定义
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;
	int  arKeyPointX[32] = {0};				// * /曲线的所有控制点
	int  arKeyPointY[32] = {0};				// * /曲线的所有控制点
	int nKeyCount = (2 * nInitCount - 1);
	int nBzNum = 2;							// * /贝塞尔曲线次数
	int nDivNum = 1;						// * /
	int nBzBias = 0;
	int nDivCount = 0;
	int nDivMaxCount = 0;

	if (n < 1 || 32 <= nKeyCount)
	{
		return;
	}

/*	if (1 == n)
	{	// * /直线轨迹
		BuildLinear(nInitX, nInitY, nInitCount, ptBezier, nBuffertCount, nBezierCount, nDistance, rcBound);
		return;
	} */
	// * /扩展拟合贝塞尔曲线的关键点，基本原则是：第一个关键点生成一个第一副关键点（规则可以设置），与第二关键点构造一个二次贝塞尔曲线，
	// * /然后第一副关键点与第二关键点等距延长后构成第二副关键点，二、二副与三关键点构造一个贝塞尔曲线，两个曲线应该是自动拟合的，依次类推
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
		{	// * /前面控制点的延长线
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

	// * /初始化总轨迹点数
	nBezierCount = 0;
	nDivNum = (nInitCount * 2 - 1) / 2;		// * /几个需要拟合的曲线
	nDivMaxCount = nBuffertCount / nDivNum;
	nBzBias = 0;

	while (nDivNum > 0)
	{
		nDivCount = 0;
		t = 0.0f;
		//生成贝塞尔
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

			//合法判断
			if (nBezierCount>=nBuffertCount/* || nDivCount >= nDivMaxCount*/)
				break;

			//判断距离
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


// * /LeonLv add:垂直直线游动的水母坐标生成点
void CMathAide::BuildReturnLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	// * /计算第一段坐标位置，
	//计算角度
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
	
	if (nInitCount < 3)	// * /只有一段
		return;

	// * /计算第二段坐标位置，
	//计算角度
	int nReturnPos = nLinearCount;		// * /LeonLv add:折点坐标，28Feb.2011
	lDisTotal=CalcDistance(nInitX[2],nInitY[2],nInitX[1],nInitY[1]);
	if (lDisTotal<=0) return;
// * /	fCos=(labs(nInitY[2]-nInitY[1])*1.f)/lDisTotal;
// * /	fAngle=float(acos(double(fCos)));

	//计算坐标
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

// * /LeonLv add:禁止不动的珍珠贝坐标生成点
void CMathAide::BuildStayPoint(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//计算角度
	float fAngle = 0;

	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount < 2) 
		return ;

	//计算坐标
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

// * /在下面的函数中可以实现产生一个在一个区间之内的平均分布的随机数，精度是4位小数。
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

// * /在下面的函数中可以实现产生一个在一个区间之内的平均分布的随机整数
int CMathAide::nAverageRandom(int min, int max)
{
	return (int)(genrand_real2() * (max - min) + min);

	//int diffInteger = max - min;

	//int randInteger = (rand());//(rand()%diffInteger);

	//int resultInteger = randInteger % diffInteger + min;

	//return resultInteger;
}

// * /V2012 Add, 12Sep.2012
// * /获取反射角度
float CMathAide::GetReflectAngle(BYTE byStepDistance, int nPosX, int nPosY, double fInAngle, int nButtom, int nRightLimit)
{
	// * /comment:在子弹发射角度计算时，子弹角度为：当炮口-Y轴逆时针夹角π/3，，子弹夹角计算结果为7π/6。
	// * /绘制子弹时用此角度绘制（原因是子弹素材，刚好比炮角度顺时针偏移π/2）
	double fReflectAngle = 0;	// * /反射角度
	bool bTwoSide = false;			// * /两侧边缘
	bool bUpAndDown = false;		// * /上下边
	// * /对撞上不同的边缘，角度计算完全不同；因此区分不同边缘就很重要；1st&2nd quadrant
	// * /如果撞上的是角，需要特殊处理为反弹

	// * /对于左右壁反弹，肯定是第一二象限内相互偏移，或者三四象限内相互偏移：偏移绝对角度为2×(π/2 - X)
	if (nPosX <= byStepDistance || (nRightLimit - byStepDistance) <= nPosX)
	{//左右壁标准的反射偏移计算公式为：X+2*(π/2 -X)，第一二象限和三四象限计算方法一样
		fReflectAngle = fInAngle +  2*(M_PI/2 - fInAngle);	//π-X
		bTwoSide = true;
	} 

	// * /对于上下壁反弹，肯定是第一四象限内相互偏移，或者二三象限内相互偏移：偏移绝对角度为2×X
	if ((nPosY <= byStepDistance || (nButtom -byStepDistance) <= nPosY))
	{		//计算公式为：X - 2×X
		fReflectAngle = fInAngle -  2*fInAngle;
		bUpAndDown = true;
	}
	if (!bTwoSide && !bUpAndDown)
	{
		return INVALID_ANGLE;	// * /无反射角
	}

	// * /如果位于角区域，角度反转射回
	if (bTwoSide && bUpAndDown)
	{
		fReflectAngle = M_PI + fInAngle;
	}
	// * /角度规范入2π内
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

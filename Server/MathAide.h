#ifndef MATH_AIDE_HEAD_FILE
#define MATH_AIDE_HEAD_FILE
#pragma once

#define INVALID_ANGLE	65536		// * /约定无效角

// * /V2012 Add:使用math.h中的数据定义，21Sep.2012
#ifndef  _USE_MATH_DEFINES
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#endif


//算术辅助
class CMathAide
{
	//函数定义
public:
	//构造函数
	CMathAide(void);
	//析构函数
	~CMathAide(void);

	//辅助计算
public:
	//贝塞尔点
	static void BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance);
	//建立直线
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);
	//建立直线
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound);
	//计算距离
	static LONG CalcDistance(int nX1, int nY1, int nX2, int nY2);
	//计算组合
	static LONG Combination(int nCount,int nR);
	//计算阶乘
	static LONG Factorial(int nNumber);
	// * /LeonLv add:可以计算角度的精确贝塞尔点
	static void BuildExactBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance);

	// * /LeonLv add: 贝塞尔曲线拟合，06May.2011
	static void BuildFittingBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance, CRect rcBound);

	// * /LeonLv add:垂直直线游动的水母坐标生成点
	static void BuildReturnLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);

	// * /LeonLv add:禁止不动的珍珠贝坐标生成点
	static void BuildStayPoint(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);


	// * /在下面的函数中可以实现产生一个在一个区间之内的平均分布的随机数，精度是4位小数。
	static double AverageRandom(double min, double max);

	static int nAverageRandom(int min, int max);

	// * /V2012 Add, 12Sep.2012
	// * /获取反射角度
	static float GetReflectAngle(BYTE byStepDistance, int nPosX, int nPosY, double fInAngle, int nButtom, int nRightLimit);
	// * /V2012 End
};

#endif
#ifndef MATH_AIDE_HEAD_FILE
#define MATH_AIDE_HEAD_FILE
#pragma once

#define INVALID_ANGLE	65536		// * /Լ����Ч��

// * /V2012 Add:ʹ��math.h�е����ݶ��壬21Sep.2012
#ifndef  _USE_MATH_DEFINES
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#endif


//��������
class CMathAide
{
	//��������
public:
	//���캯��
	CMathAide(void);
	//��������
	~CMathAide(void);

	//��������
public:
	//��������
	static void BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance);
	//����ֱ��
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);
	//����ֱ��
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound);
	//�������
	static LONG CalcDistance(int nX1, int nY1, int nX2, int nY2);
	//�������
	static LONG Combination(int nCount,int nR);
	//����׳�
	static LONG Factorial(int nNumber);
	// * /LeonLv add:���Լ���Ƕȵľ�ȷ��������
	static void BuildExactBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance);

	// * /LeonLv add: ������������ϣ�06May.2011
	static void BuildFittingBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], POINT ptAngle[], int nBuffertCount, WORD &nBezierCount, int nDistance, CRect rcBound);

	// * /LeonLv add:��ֱֱ���ζ���ˮĸ�������ɵ�
	static void BuildReturnLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);

	// * /LeonLv add:��ֹ���������鱴�������ɵ�
	static void BuildStayPoint(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);


	// * /������ĺ����п���ʵ�ֲ���һ����һ������֮�ڵ�ƽ���ֲ����������������4λС����
	static double AverageRandom(double min, double max);

	static int nAverageRandom(int min, int max);

	// * /V2012 Add, 12Sep.2012
	// * /��ȡ����Ƕ�
	static float GetReflectAngle(BYTE byStepDistance, int nPosX, int nPosY, double fInAngle, int nButtom, int nRightLimit);
	// * /V2012 End
};

#endif
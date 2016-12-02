#ifndef __GAME_UTILITY_H__
#define __GAME_UTILITY_H__
#include <time.h>

/*
感谢Matsumoto和L'Ecuyer两位先生提供算法。算法出处：<<Game Programming 7>>


*/

#define RandomInitInterval (60*60*1000)
#define RandomMaxInterval (30*60*1000)

class CRand;

class Counter
{
public:
	static Counter& GetSinglton()
	{
		static Counter count;
		return count;
	}

	int GetNum()
	{
		m_num += 20;
		if (m_num >=100)
		{
			m_num =  (int)(genrand_real2() * 20);
		}
		return m_num;
	}

	void add(int add)
	{
		m_num += add;
	}
private:
	Counter()
	{
		m_num = 0;
	}

private:
	int m_num;
};

class /*APPLES_LIB_SYMBOLS*/ CRand
{
public:

	~CRand()
	{

	}

	static CRand& GetSinglton()
	{
		static CRand rand;
		return rand;
	}

	int GetProb( int nMax )
	{
		if(nMax <=0)
		{
			return 0;
		}

		unsigned int nRandNumber = WELLRNG512();
		//            unsigned int x = nRandNumber % 127773;//127773是个较大的质数
		//	 unsigned int x = nRandNumber % 1000003;//1000003是个较大的质数,作此变更的原因的是因为策划的掉落概率需求要改成百万分之X
		//以后需求再有变更请到该网址选择一个相对合适的质数 
		//http://www.buiosch.edu.hk/subjects/maths/PimeGenerator/prime.html
		return nRandNumber%((unsigned int)nMax);//x * nMax /127773;
	}

	void RandomInit(unsigned int tick)
	{
		/*if (tick - m_LastRandomInitTick > GetRandomInitInterval())*/
		{
			m_LastRandomInitTick = tick;
			ProphasePrepare();
			
		}
		
	}

	int GetRandomInitInterval()
	{
		return RandomInitInterval + RandomInterval;
	}



	public:
	CRand()
	{
		index = 0;
		//state[16] = {0, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
		for ( int i = 0 ; i < 16; i++)
		{
			state[i] = i;
		}
		num  = Counter::GetSinglton().GetNum();
		ProphasePrepare();

		m_LastRandomInitTick = 0;
	}
public:
	/*static */void ProphasePrepare()
	{
		initRNG();
		for (int i = 0 ; i < num ; i++)
		{
			WELLRNG512();
		}

		for (int i = 0 ; i < 16; i ++)
		{
			state[i] = WELLRNG512();
		}

		RandomInterval = GetProb(RandomMaxInterval);
	}

	/*static */void initRNG()
	{
		for (int i = 0 ; i < 16; i++)
		{  
			unsigned int  seed = genrand_int32();      
			//seed = seed*127773-1;
			if (seed != 0)
			{
				state[i] = seed;
			}              
		}
	}

	//返回32位随机数
	/*static */unsigned int WELLRNG512()
	{
		unsigned int a, b , c, d;
		a = state[index];
		c = state[(index + 13 ) & 15];
		b = a^c^(a<<16)^(c<<15);
		c = state[(index + 9)&15]; 
		c^= (c>>11);
		a = state[index] = b^c;
		d = a^ ((a <<5) & 0xDA442D20UL);
		index = (index + 15) & 15;
		a = state[index];
		state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
		return state[index];
	}




private:
	//初始化状态到随即位
	/*static */unsigned int state[16];
	//初始化必须为0
	/* static */unsigned int index;

	 int num;

	unsigned int m_LastRandomInitTick;

	int RandomInterval;
};

#endif
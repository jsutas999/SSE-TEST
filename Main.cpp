#include <iostream>
#include <iomanip>
#include <math.h>
#include <ctime>

void checkTripletASM(float* aptr,float* bptr,float* cptr,int& o)
{

	_asm {
		push eax
			push ebx

			mov eax, aptr
			mov ebx, bptr
			movaps xmm0, [eax]
			movaps xmm1, [ebx]

			movaps xmm7, xmm0
			mulps xmm7, xmm0
			movaps xmm2, xmm7

			movaps xmm7, xmm1
			mulps xmm7, xmm1
			movaps xmm3, xmm7

			movaps xmm4, xmm2
			addps xmm4, xmm3

			movaps xmm5, xmm4
			sqrtps xmm5, xmm5

			roundps xmm5, xmm5, 1
			mulps xmm5, xmm5

			subps xmm5, xmm4

			mov eax, cptr
			movaps[eax], xmm5

			pop ebx
			pop eax
	}

	for (int i = 0; i < 4;i++)
		if (*(cptr + i) == 0.0)
		{
			o++;
			//std::cout << *(aptr + i) << " " << *(bptr + i) << " " << *(aptr + i) * *(aptr + i) +
			//*(bptr + i) * *(bptr + i) << std::endl;
		}

}

void checkTripletC(float* aptr, float* bptr, float* cptr, int& o)
{

	for (int i = 0 ; i < 4;i++)
	{
		float a = aptr[i] * aptr[i] + bptr[i] * bptr[i];
		if (sqrt(a) == round(sqrt(a)))
		{
			o++;
		//std::cout << aptr[i] << " " << bptr[i] <<" "<<  a << std::endl;
		}
			
	}


}

float Experiment( void(*fun)(float*, float*, float*, int&), int& countas,int pr)
{
	int n = pr;
	__declspec(align(16)) float pa[4] = { 1,1,6,1 };
	__declspec(align(16)) float pb[4] = { 4096,1,8,1 };
	__declspec(align(16)) float ts[4] = { 0,0,0,0 };
	float* aptr = &pa[0];
	float* bptr = &pb[0];
	float* cptr = &ts[0];
	int c = 0;
	int o = 0;

	std::clock_t b = std::clock();

	for (int i = 1; i <= n; i++)
		for (int j = i; j <= n;j++)
		{
		
			pa[c] = i; pb[c] = j; c++;
			
			if (c == 4)
			{
				c = 0;
				fun(aptr, bptr, cptr, o);
			}
		}

	std::clock_t end = std::clock();
	float ats = (float)(end - b) / CLOCKS_PER_SEC ;
	countas = o;
	return ats;

}


int main()
{
	int n = 100;
	int pr = 2000;
	float m = 0;
	void(*fun)(float*,float*,float*,int&);
	fun = &checkTripletASM;
	int c = 0;
	std::cout << "RUNS: " << n << " PER RUN: " << pr << std::endl;
	std::cout << "TEST BEGINS! \n";
	for (int i = 0; i < n;i++)
		m += Experiment(fun,c,pr);

	std::cout <<"SSE/FLOAT TIME: "<< m / (float)n << " ENTRYS: " << c << std::endl;
	m = 0;
	fun = &checkTripletC;
	for (int i = 0; i < n;i++)
		m += Experiment(fun,c,pr);
	std::cout << "C++/FLOAT TIME: " << m / (float)n << " ENTRYS: " << c << std::endl;


	std::cout << "TEST END!";
	std::cin.ignore();
			
}

	

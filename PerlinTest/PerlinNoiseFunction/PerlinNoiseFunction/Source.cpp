#include <iostream>
#include <fstream>

float noise(int x);
float lerp(float val1, float val2, float t);
float coslerp(float val1, float val2, float t);
constexpr int TILESIZE{ 1 << 8 };
constexpr int startStride{ 1 << 6};
constexpr int StrideStep{ 2 };
constexpr int endStride{ 1<< 1 };
constexpr int NumberTiles{ 3 };
constexpr int LENGTH{ TILESIZE * NumberTiles };
float persistance{ 0.5f };
void writefile(char* name, float * values, int length);

void interpolateStrideValues(float * start, int startindex, float startval, int lastP1, float endVal, float amp);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		throw std::runtime_error("No filename inputted");
	}
	float values[LENGTH];
	// initialize to zero
	float* curr = values;
	float * end = values + LENGTH;
	while (curr < end) {
		*curr++ = 0.0f;
	}
	float amp {2.};
	for (int i{ startStride }; i >= endStride; i /= StrideStep)
	{
		int si{ 0 };
		float noisea{ noise(si) };
		int endoffset{ 0 };
		amp *= persistance;
		for (int j = { si }; j < LENGTH - i; j += i)
		{
			int noiseindex = j + i - endoffset;
     		if (noiseindex >= TILESIZE) {
				endoffset += TILESIZE;
				noiseindex -= TILESIZE;
			}
			float noiseb{ noise(noiseindex) };
			interpolateStrideValues(values, j, noisea, j + i, noiseb, amp);
			noisea = noiseb;
		}


	}
	writefile(argv[1], values, LENGTH);
	
}





void writefile(char * filename, float* values, int length)
{
		std::ofstream thisfile(filename);
		float* endptr{ values + length };
		while (values < endptr)
		{
			thisfile << *values++ << std::endl;
		}

}
float noise(int x) {
	x = (x << 13) ^ x;
	return (1 - ((x * (x * x * 15731 + 789221) + 1376312589)
		& 0x7fffffff) / 1073741824.0);
}
float coslerp(float val1, float val2, float  tL)
{
	//float t = (xnew - indx1) / (float) (indx2 - indx1);
	tL *= 3.1415926f;
	float t = (1.f - std::cos(tL)) * 0.5f;
	return (val1 * (1.f - t) + val2 * t);
}
float lerp(float val1, float val2, float  t)
{
	//float t = (xnew - indx1) / (float) (indx2 - indx1);
	return (val1 * (1 - t) + val2 * t);
}

void interpolateStrideValues(float * start, int startindex, float startval, int lastP1, float endVal, float amp)
{
	float diffVal{static_cast<float>( lastP1 - startindex)};
	for (int i{ startindex }; i < lastP1; ++i)
	{
		float t{ (i - startindex) / diffVal };
		*(start + i) += amp * coslerp(startval, endVal, t);
	}
}

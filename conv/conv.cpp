// conv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "cs477.h"
#include "image.h"



void conv(matrix &bmp, const matrix &kernal)
{
	matrix y;
	y.create(bmp.rows + kernal.rows, bmp.cols + kernal.cols);

	for (unsigned row = 0; row < bmp.rows; row++)
	{
		for (unsigned col = 0; col < bmp.cols; col++)
		{
			auto yrow = row + kernal.rows / 2;
			auto ycol = col + kernal.cols / 2;
			y(yrow, ycol) = bmp(row, col);
		}
	}

	// Compute sum of k
	int weight = 0;
	for (unsigned row = 0; row < kernal.rows; row++)
	{
		for (unsigned col = 0; col < kernal.cols; col++)
		{
			weight += kernal(row, col);
		}
	}

	int maxThreads = 16;
	std::vector<cs477::thread> threads;
	threads.reserve(maxThreads);
	
	//If the height of bmp is not divisible by maxThreads, divide by one less so our chunks are big enough to deal with the whole image
	int chunkSize = bmp.rows / (maxThreads - !(bmp.rows % maxThreads == 0)); 
	
	// Do the convolution
	for (unsigned c = 0; c < maxThreads; c++) {
		auto t = cs477::create_thread([&bmp, &kernal, &y, c, chunkSize, weight]() {
			int start = chunkSize * c;
			int end = start + chunkSize;

			for (unsigned row = start; row < end && row < bmp.rows; row++) {
				for (unsigned col = 0; col < bmp.cols; col++) {
					int t = 0;

					auto yrow = row;
					for (int krow = kernal.rows - 1; krow >= 0; krow--, yrow++) {
						auto ycol = col;
						for (int kcol = kernal.cols - 1; kcol >= 0; kcol--, ycol++) {
							t += y(yrow, ycol) * kernal(krow, kcol);
						}
					}

					if (weight != 0) {
						t /= weight;
					}

					bmp(row, col) = t;
				}
			}
		});

		threads.push_back(t);
	}

	for (auto &t : threads) {
		cs477::join(t);
	}

}


int binomial_coefficient(int n, int k)
{
	if (n <= 1 || k == 0)
	{
		return 1;
	}
	else
	{
		return binomial_coefficient(n - 1, k - 1) * n / k;
	}
}

matrix binomial(int n)
{
	if ((n & 1) == 0)
	{
		throw std::invalid_argument("n must be odd");
	}

	matrix x, y;
	x.create(1, n);
	y.create(n, 1);

	for (int i = 0; i < n / 2; i++)
	{

		x(0, i) = x(0, n - i - 1) = binomial_coefficient(n - 1, i);
		y(i, 0) = y(n - i - 1, 0) = binomial_coefficient(n - 1, i);
	}

	x(0, n / 2) = binomial_coefficient(n - 1, n / 2);
	y(n / 2, 0) = binomial_coefficient(n - 1, n / 2);

	return y * x;
}




int main()
{
	auto bmp = load_image("kittens.png");
	auto orig = bmp;

	matrix kernel = binomial(11);

	auto start = now();
	conv(bmp, kernel);
	auto stop = now();
	
	printf("%g\n", to_seconds(start, stop));

	save_png(bmp, "kittens.bmp");

    return 0;
}

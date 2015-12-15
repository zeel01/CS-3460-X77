// Sort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "..\include\cs477.h"

void quickSort(int*, const int, const int);
void p_quickSort(int*, const int, const int, int);
int split(int*, const int, const int);

const int S = 15000000;
	
int data[S];
int data2[S];

int main()
{
	auto g1 = now();
	for (int i = 0; i < S; i++) {
		data[i] = std::rand() % S;
		data2[i] = std::rand() % S;
//		std::cout << data[i] << " ";
	}
	auto g2 = now();
	std::cout << std::endl << to_seconds(g1, g2) << std::endl << std::endl;

	auto t1 = now();
	quickSort(data, 0, S - 1);
	auto t2 = now();;

//	for (int i = 0; i < S; i++) {
//		std::cout << data[i] << " ";
//	}

	std::cout << std::endl << to_seconds(t1, t2) << std::endl << std::endl;
	auto t3 = now();
	p_quickSort(data2, 0, S - 1, 7); // 7^2 threads, more didn't seem to improve performance by much if any
	auto t4 = now();;
	
//	for (int i = 0; i < S; i++) {
//		std::cout << data[i] << " ";
//	}
	std::cout << std::endl << to_seconds(t2, t4) << std::endl;

    return 0;
}

void p_quickSort(int* arr, const int start, const int end, int depth) {
	if (start >= end) return;

	int mid = split(arr, start, end);

	cs477::future<void> left;

	if (depth > 0) {
		left = cs477::queue_work([=] {
			p_quickSort(arr, start, mid - 1, depth - 1);
		});

		p_quickSort(arr, mid + 1, end, depth - 1);
		left.wait();
	}
	else {
		quickSort(arr, start, mid - 1);
		quickSort(arr, mid + 1, end);
	}
}

void quickSort(int* arr, const int start, const int end) {
	if (start >= end) return;

	int mid = split(arr, start, end);

	quickSort(arr, start, mid - 1);
	quickSort(arr, mid + 1, end);
}

int split(int* arr, const int start, const int end) {
	const int mid = start + (end - start) / 2;
	const int pivot = arr[mid];

	std::swap(arr[start], arr[mid]);
	

	int left = start + 1;
	int right = end;
	while (left <= right) {
		while (left <= right && arr[left] <= pivot) left++;
		while (left <= right && arr[right] > pivot) right--;

		if (left < right) std::swap(arr[left], arr[right]);
	}
	
	std::swap(arr[left - 1], arr[start]);
	return left - 1;
}
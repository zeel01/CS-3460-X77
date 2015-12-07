# CS-3460--4|5]77 - Introduction to Parallel Processing

- [Syllabus](https://github.com/tosutton/CS-3460/blob/master/syllabus.md)

Homework Assignments

0.	Hello World
	Set up a github account and write a C++ program that says "Hello, world."  
	a. Completeness: 1pt  
	b. Performance: 0pts  
	c. Style: 0pts  
	
1.	Prime Numbers
	Parallelize primes.  
	a. Completeness: 5pt  
	b. Performance: 3pts  
	c. Style: 2pts  

2.	Convolution
	Parallelize conv.  
	a. Completeness: 2pt  
	b. Performance: 5pts  
	c. Style: 3pts  

3.	Parallel Sort
	Implement a Parallel sort algorithm (quick or merge).  
	a. Completeness: 5pt  
	b. Performance: 5pts  
	c. Style: 3pts  

3.	Web server
	Write a web server to server static files.  The networking and file I/O be entirely asynchronous.
	a. Completeness: 5pt  
	b. Performance: ?pts  
	c. Style: ?pts  
	
4.  Distributed primes
	Write a distributed version of the prime numbers problem.  The solution
	should consist of two programs: a driver and a processor.  
	
	The driver should distribute ranges of integers to the processor and receive a list of prime numbers.
	
	The processor should receive a range of integers from the driver, 
	find all the prime numbers in that range (in parallel), then write those into shared memory and signal the driver.
	
	You will need two shared_queue's and a shared memory pool to accomplish this.
	
	a. Completeness: ?pt  
	b. Performance: ?pts  
	c. Style: ?pts  
	
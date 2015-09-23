
namespace cs477
{

	template <typename T>
	queue<T>::queue()
	{
	}

	template <typename T>
	void queue<T>::push_back(T &&t)
	{
		lock_guard<> lock(mtx);
		list.push_back(std::move(t));
		cv.notify_all();
	}

	template <typename T>
	void queue<T>::push_back(const T &t)
	{
		lock_guard<> lock(mtx);
		list.push_back(t);
		cv.notify_all();
	}

	template <typename T>
	T queue<T>::pop_back()
	{
		lock_guard<> lock(mtx);
		while (list.empty())
		{
			cv.wait(mtx);
		}

		auto t = list.front();
		list.pop_front();
		return t;

	}


}

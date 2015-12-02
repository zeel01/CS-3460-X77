
#pragma once



// Message represents a buffer in a form that can be passed into 
// a bounded queue.
struct message
{
	char pool[16];
	cs477::shm_pool::pointer ptr;
	uint32_t len;
};


// Buffer represents a chunk of shared memory.
struct buffer
{
public:
	buffer()
		: _pool(nullptr), _size(0)
	{
	}

	buffer(std::shared_ptr<cs477::shm_pool> pool)
		: _pool(pool), _size(0)
	{
	}

	buffer(std::shared_ptr<cs477::shm_pool> pool, const message &msg)
		: _pool(pool)
	{
		_ptr = msg.ptr;
		_size = msg.len;
	}

public:
	void allocate(uint32_t len)
	{
		auto ptr = _pool->allocate(len);

		if (_ptr != nullptr)
		{
			auto src = (*_pool)(_ptr);
			auto dst = (*_pool)(ptr);
			memcpy(dst, src, _size);

			_pool->deallocate(_ptr);
		}

		_ptr = ptr;
		_size = len;
	}

	void deallocate()
	{
		if (_ptr != nullptr)
		{
			_pool->deallocate(_ptr);
		}
	}

	message make_message()
	{
		message msg;
		strncpy_s(msg.pool, _pool->name().c_str(), 16);
		msg.ptr = _ptr;
		msg.len = _size;
		return msg;
	}

public:
	const char *data() const
	{
		return (*_pool)(_ptr);
	}

	char *data()
	{
		return (*_pool)(_ptr);
	}

	uint32_t size() const
	{
		return _size;
	}

	void shrink(uint32_t size)
	{
		assert(size <= _size);
		_size = size;
	}


private:
	std::shared_ptr<cs477::shm_pool> _pool;
	cs477::shm_pool::pointer _ptr;
	uint32_t _size;
};



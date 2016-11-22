#pragma once
#include <vector>

template<typename OBJECT, int POOL_MAXSIZE>
class MemPool{
public:
	static const int OBJECT_COUNT = POOL_MAXSIZE;
	MemPool()
	{
		_offset = 0;
		_freed.reserve(POOL_MAXSIZE);
	}
	OBJECT* create()
	{
		OBJECT *o = 0;
		if (_freed.empty())
		{
			if (_offset >= POOL_MAXSIZE)
			{
				o = new OBJECT();
			}
			else
			{
				o = &_objects[_offset];
				_offset++;
			}
		}
		else
		{
			o = _freed.back();
			_freed.pop_back();
		}
		return o;
	}
	void destroy(OBJECT *object)
	{
		_freed.push_back(object);
	}

	static MemPool * instance()
	{
		return &_memPool;
	}

private:
	unsigned int _offset;
	std::vector<OBJECT*> _freed;
	OBJECT _objects[POOL_MAXSIZE];
	static MemPool _memPool;
};
template<typename OBJECT, int POOL_MAXSIZE>
MemPool<OBJECT, POOL_MAXSIZE> MemPool<OBJECT, POOL_MAXSIZE>::_memPool;

template<int BLOCK_COUNT, int BLOCK_SIZE>
class MemBufferPool{
private:
	struct BLOCK{
		BLOCK *next;
	} *_freehead;
public:
	MemBufferPool()
	{
		_offset = 0;
		_freehead = 0;
	}
	void *get()
	{
		BLOCK *ab = _freehead;
		if (ab != 0)
		{
			_freehead = ab->next;
		}
		else
		{
			if (_offset < BLOCK_COUNT)
			{
				ab = (BLOCK*)&_buffer[_offset];
			}
			else
			{
				ab = (BLOCK*)malloc(BLOCK_SIZE + sizeof(BLOCK));
			}
		}


		return (char*)(ab + 1);
	}
	void put(void *data)
	{
		BLOCK * ab = (BLOCK*)data - 1;
		ab->next = _freehead;
		_freehead = ab;
	}

	static MemBufferPool * instance()
	{
		return &_memPool;
	}

	static const int BUFFER_SIZE = BLOCK_SIZE;
private:
	static MemBufferPool _memPool;
	int _offset;
	char _buffer[BLOCK_COUNT][BLOCK_SIZE + sizeof(BLOCK)];
};
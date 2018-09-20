#include "BitArray.h"
#include <memory>


BitArray::BitArray(int size)
{
	if (size <= 0)
	{
		_arraySize = DefaultBitArraySize;
	}else
	{
		_arraySize = size;
	}

	_data = new unsigned char[_arraySize];
	memset(_data, 0 , _arraySize);
}

BitArray::~BitArray()
{
	if (_data != 0)
	{
		delete _data;
		_data = 0;
	}
}

BitArray::BitArray(const BitArray &ba)
{
	_arraySize = ba._arraySize;
	_data = new unsigned char[_arraySize];

	memcpy(_data, ba._data, _arraySize);
}


BitArray &BitArray::operator =(const BitArray &ba)
{
	if (this == &ba)
	{
		return *this;
	}

	if (_data != NULL)
	{
		delete _data;
		_data = 0;
		_arraySize = 0;
	}

	_arraySize = ba._arraySize;
	_data = new unsigned char[_arraySize];
	memcpy(_data, ba._data, _arraySize);
	return *this;
}

void BitArray::setValue(int pos,unsigned char value)
{
	if (pos <= 0 || pos > _arraySize)
	{
		return;
	}

	_data[pos - 1] = value > 0 ? 1 : 0;
}

unsigned char BitArray::getValue(int pos) const 
{
	if (pos <= 0 && pos > _arraySize)
	{
		return 0;
	}
	return _data[pos - 1];
}


int BitArray::getArraySize() const
{
	return _arraySize;
}

unsigned char BitArray::toUChar()
{
	int count = 0;
	unsigned char result = 0;


	if (sizeof(unsigned char) * 8 > _arraySize)
	{
		count = _arraySize;
	}else
	{
		count = sizeof(unsigned char) * 8; 
	}

	for (int i = 0; i < count; i++)
	{

		result += _data[i] << i; 
	}

	return result;
}


unsigned short BitArray::toUShort()
{

	int count = 0;
	unsigned  short result = 0;


	if (sizeof(unsigned short) * 8 > _arraySize)
	{
		count = _arraySize;
	}else
	{
		count = sizeof(unsigned short) * 8; 
	}


	for (int i = 0; i < count; i++)
	{

		result += ((unsigned short)_data[i]) << i; 
	}

	return result;
}

unsigned int BitArray::toUInt()
{
	int count = 0;
	unsigned  int result = 0;


	if (sizeof(unsigned int) * 8 > _arraySize)
	{
		count = _arraySize;
	}else
	{
		count = sizeof(unsigned int) * 8; 
	}

	for (int i = 0; i < count; i++)
	{
		result += ((unsigned int)_data[i]) << i;
	}

	return result;
}


int BitArray::toBitArray(unsigned char *data, int maxSize)
{
	int count = 0;
	if (maxSize * 8 < _arraySize )
	{
		count =  maxSize * 8;
	}else
	{
		count = _arraySize;
	}

	int bytePos = 0;
	int bitPos = 0;

	memset(data, 0, maxSize);

	for (int i = 0; i < count; i++)
	{
		bytePos = i / 8;
		bitPos = i % 8;

		data[bytePos] += _data[i] << bitPos;
	}

	return (count % 8 > 0) ? (count / 8 + 1) : (count / 8);
}


void BitArray::setContent(const unsigned char *data, int size)
{
	if (data == NULL || size <= 0)
	{
		return;
	}

	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
		_arraySize = 0;
	}

	if (size * 8 > MaxBitArraySize)
	{
		_arraySize = MaxBitArraySize;
	}else
	{
		_arraySize = size * 8;
	}
	
	_data = new unsigned char[_arraySize];
	memset(_data, 0, _arraySize);

	int bytePost = 0;
	int bitPost = 0;
	for (int i = 0; i < _arraySize / 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			_data[i * 8 + j] = (data[i] & ((unsigned char)0x01 << j)) > 0 ? 1 : 0;
			
		}
	}
}

int BitArray::getFirst()
{
	int pos = 0;

	for(int i = 0; i < _arraySize; i++)
	{
		if (_data[i] != 0)
		{
			pos = i + 1;
			break;
		}
	}

	return pos;
}

void BitArray::clear()
{
	for(int i = 0; i < _arraySize; i++)
	{
		_data[i] = 0;
	}

}



void BitArray::fromVector(const vector<int> &vec)
{
	clear();
	for (int i = 0; i < vec.size(); i++)
	{
		setValue(vec[i], 1);
	}
}


vector<int> BitArray::toVector()
{
	vector<int > vec;
	for (int i = 0; i < _arraySize; i++)
	{
		if (_data[i] > 0)
		{
			vec.push_back(i + 1);
		}
	}

	return vec;
}
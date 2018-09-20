#ifndef BITARRAY_H
#define BITARRAY_H
#include <vector>

using std::vector;

//位设置类
//索引从0开始, 但传入参数索引应该从1开始
//

class BitArray
{
public:
	static const int DefaultBitArraySize = 32;

	static const int MaxBitArraySize = 4096;
	
	BitArray(int size = DefaultBitArraySize);
	
	~BitArray();

	BitArray(const BitArray &ba);
	
	BitArray &operator =(const BitArray &ba);

	void resize(int size, bool useOld = false) {}
	
	void setValue(int pos,unsigned char value);
	
	void setContent(const unsigned char *data, int size);
	
	unsigned char getValue(int pos) const ;
	
	int getArraySize() const;
	
	unsigned int toUInt();
	
	unsigned short toUShort();
	
	unsigned char toUChar();
	
	int toBitArray(unsigned char *data, int maxSize);
	
	int getFirst();
	
	void clear();

	
	void fromVector(const vector<int> &vec);
	
	vector<int> toVector();


	template<typename T>
	void setData(T t)
	{
		int size = sizeof(T);


		unsigned char *data = new unsigned char[size];
		memset(data, 0, size);
		for (int i = 0; i < size; i++)
		{
			data[i] = (unsigned char)((t >> i * 8) & 0x0FF );
		}

		setContent(data, size);
		delete data;
	}

private:
	unsigned char *_data;
	int _arraySize;
};

#endif // BITARRAY_H

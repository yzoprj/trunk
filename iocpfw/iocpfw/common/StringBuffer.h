#ifndef __STRINGBUFFER_H__
#define __STRINGBUFFER_H__



class  StringBuffer
{
public:

	StringBuffer();

	StringBuffer(const char *buf);

	StringBuffer(const StringBuffer &rhs);

	StringBuffer& operator=(const StringBuffer &rhs);

	StringBuffer& operator=(const char *rhs);

#if _MSC_VER >= 1600
	// rvalue reference
	StringBuffer(StringBuffer&& sb);
#endif

	bool operator==(const StringBuffer &rhs);

	bool operator==(const char *rhs);
	//operator const char* () const {return data;}

	~StringBuffer();

	void		append(const StringBuffer &buf);

	void		append(const char *buf);

	const char* getData() const {return data;}

	int			getLength() const { return size;}

protected:

	void defaultAlloc();

	void clear();
private:
	int		size;
	char	*data;
};






#endif

#ifndef __ORNG_OBJECTPROXY_HPP__
#define __ORNG_OBJECTPROXY_HPP__

namespace JKProtocol
{
namespace Common
{
	template <class T>
	class ObjectProxy
	{
	public:
		ObjectProxy()
		{
			refCountObj = NULL;
			obj = NULL;
		}


		ObjectProxy(T *refObj)
		{
			refCountObj = NULL;
			obj = NULL;
			setValue(refObj);
		}


		ObjectProxy(const ObjectProxy &rhs)
		{
			refCountObj = NULL;
			obj = NULL;
			setValue(rhs.obj, rhs.refCountObj);
		}

		ObjectProxy &operator=(const ObjectProxy &rhs)
		{
			if (this == &ths)
			{
				return *this;
			}
			setValue(rhs.obj, rhs.refCountObj);
			return *this;
		}

		T* getObject()
		{
			return obj;
		}

		T* operator->()
		{
			return obj;
		}

		T& operator*()
		{
			return *obj;
		}

		~ObjectProxy()
		{
			unRefObj();

		}

		ObjectProxy<T> createNewObject()
		{
			return ObjectProxy<T>(new T);
		}

	protected:


		/////////////////////////
		//引用计数对象
		////////////////////////
		class ReferenceObject
		{

		public:
			ReferenceObject() {refCount = 0;}
			void addReference() { InterlockedIncrement(&refCount);}
			void release() {if(refCount == 0) return; InterlockedDecrement(&refCount);}
			bool isRemovable() { return refCount == 0 ? true : false;}
			long refCount;
		private:

		};

		void setValue(T *value)
		{
			unRefObj();

			refCountObj = new ReferenceObject;
			refCountObj->addReference();
			obj = value;

		}

		void setValue(T *value, ReferenceObject *refCount)
		{
			if (this->refCountObj != NULL 
				&& this->refCountObj == refCount 
				&& this->obj == value)
			{
				return;
			}
			unRefObj();

			refCountObj = refCount;
			obj = value;
			refCountObj->addReference();

		}

		void unRefObj()
		{

			if (refCountObj != NULL)
			{
				refCountObj->release();
				printf("refCount %d\n", refCountObj->refCount);
				if (refCountObj->isRemovable())
				{
					if (obj != NULL)
					{
						delete obj;
						delete refCountObj;
						obj = NULL;
						refCountObj = NULL;
					}
				}
			}
		}
	private:
		ReferenceObject *refCountObj;
		T				*obj;
	};


}


}
/////////////////////////
//代理对象
////////////////////////

#endif
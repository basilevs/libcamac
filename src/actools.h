#ifndef ACTOOLS_H__
#define ACTOOLS_H__

template<class T>
class Sizer
{
public:
	static  int count()
	{
		return 1;
	}
};


template<class D, int size>
class Sizer<D[size]>
{
public:
	static int count()
	{
		return size;
	}
};

template<typename T,int N> 
int arraySize(T (&arr1)[N]) {
	return N;
}

template<class T>
void deleteFunction(T * toDelete) {
	delete toDelete;
}

template <class T, int N>
void deleteArrayOfPointers(T (&array)[N]) {
	for (int i = arraySize(array) - 1; i >=0 ; --i) {
		delete array[i];
	}
}

template <class T, int N, class X>
void arrayFill(T (&array)[N], const X x ) {
	for (int i = arraySize(array) - 1; i >=0 ; --i) {
		array[i] = x;
	}
}

template<class T, int N, class X>
void fillArrayOfPointersWithIndexed(T* (&array)[N], X & x) {
	arrayFill(array, static_cast<T*>(0));
	try {
		for (int i =0; i < arraySize(array); ++i) {
			array[i] = new T(x, i);
		}
	} catch (...) {
		deleteArrayOfPointers(array);
		throw;
	}
}

template <class T, int N>
T getArrayItemOrNull(const T (&array)[N], int idx) {
	if (idx < 0 || idx >= arraySize(array))
		return 0;
	return array[idx];
}

#endif


template<class T>
const T& Resource<T>::Data() const
{
	return data_;
}

template <class T>
T& Resource<T>::Data()
{
	return data_;
}
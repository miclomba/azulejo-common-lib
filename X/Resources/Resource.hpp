#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
template<typename T> \
template<typename U, typename std::enable_if_t<is_arithmetic_container<U>::value, int>> 

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource() 
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(const T& data) : data_(data)
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(T&& data) : data_(std::move(data))
{
}

template<typename T>
Resource<T>::~Resource() = default;

template<typename T>
const T& Resource<T>::Data() const
{
	return data_;
}

template<typename T>
T& Resource<T>::Data()
{
	return data_;
}

template<typename T>
void Resource<T>::Assign(const char* buff, const size_t n)
{
	T vBuff;
	vBuff.push_back(0);
	int size = sizeof(decltype(vBuff[0]));
	vBuff.clear();

	for (auto i = 0; i < n/size; ++i)
		vBuff.push_back(buff[i*size]);

	Data().assign(vBuff.begin(), vBuff.end());
}

template<typename T>
int Resource<T>::Checksum() const
{
	boost::crc_32_type result;
	size_t size = sizeof(*data_.data()) * data_.size();
	result.process_bytes(data_.data(), size);
	return result.checksum();
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DEF

#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
template<typename T> \
template<typename U, typename std::enable_if_t<is_arithmetic_container<U>::value, int>> 


ENABLE_IF_CONTAINER_TEMPLATE_DEF
int IResource<T>::Checksum() const
{
	boost::crc_32_type result;
	size_t size = sizeof(*data_.data()) * data_.size();
	result.process_bytes(data_.data(), size);
	return result.checksum();
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
IResource<T>::IResource(const T& data) : data_(data)
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
IResource<T>::IResource(T&& data) : data_(std::move(data))
{
}

template<typename T>
IResource<T>::~IResource() = default;

template<typename T>
bool IResource<T>::IsDirty() const
{
	int check = Checksum();
	if (check != checkSum_)
	{
		checkSum_ = check;
		return true;
	}
	return false;
}

template<typename T>
const T& IResource<T>::Data() const
{
	return data_;
}

template<typename T>
T& IResource<T>::Data()
{
	return data_;
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DEF

#define TEMPLATE_T template<typename T>
#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
TEMPLATE_T \
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

TEMPLATE_T Resource<T>::Resource(const Resource<T>&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(const Resource<T>&) = default;
TEMPLATE_T Resource<T>::Resource(Resource<T>&&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(Resource<T>&&) = default;

TEMPLATE_T
Resource<T>::~Resource() = default;

TEMPLATE_T
const T& Resource<T>::Data() const
{
	return data_;
}

TEMPLATE_T
T& Resource<T>::Data()
{
	return data_;
}

TEMPLATE_T
void Resource<T>::Assign(const char* buff, const size_t n)
{
	if (!buff)
		throw std::runtime_error("Buffer cannot be NULL during Resource::Assign");
	if (n < 1)
		throw std::runtime_error("N cannot be 0 during Resource::Assign");

	T vBuff;
	vBuff.push_back(0);
	size_t size = sizeof(decltype(vBuff[0]));
	vBuff.clear();

	for (size_t i = 0; i < n/size; ++i)
		vBuff.push_back(buff[i*size]);

	Data().assign(vBuff.begin(), vBuff.end());
}

TEMPLATE_T
int Resource<T>::Checksum() const
{
	boost::crc_32_type result;
	size_t size = sizeof(*data_.data()) * data_.size();
	result.process_bytes(data_.data(), size);
	return result.checksum();
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DEF
#undef TEMPLATE_T

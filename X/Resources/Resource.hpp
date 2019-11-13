#define TEMPLATE_T template<typename T>
#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
TEMPLATE_T \
template<typename U, typename std::enable_if_t<std::is_arithmetic<U>::value, int>> 

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource() 
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(const std::vector<T>& data) :
	data_(data)
{
	SetRowSize(data_.size());
	SetColumnSize(1);
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(std::vector<T>&& data) :
	data_(std::move(data))
{
	SetRowSize(data_.size());
	SetColumnSize(1);
}

TEMPLATE_T Resource<T>::Resource(const Resource<T>&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(const Resource<T>&) = default;
TEMPLATE_T Resource<T>::Resource(Resource<T>&&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(Resource<T>&&) = default;

TEMPLATE_T
Resource<T>::~Resource() = default;

TEMPLATE_T
const T* Resource<T>::Data() const
{
	return data_.data();
}

TEMPLATE_T
T* Resource<T>::Data()
{
	return data_.data();
}

TEMPLATE_T
void Resource<T>::Assign(const char* buff, const size_t n)
{
	if (!buff)
		throw std::runtime_error("Buffer cannot be NULL during Resource::Assign");
	if (n < 1)
		throw std::runtime_error("N cannot be 0 during Resource::Assign");

	size_t size = sizeof(T);
	if (n % size != 0)
		throw std::runtime_error("n is not a multiple of sizeof(T) during Resource<T>::Assign");

	data_.resize(n / size);
	for (size_t i = 0; i < n / size; ++i)
		data_[i] = *reinterpret_cast<const T*>(buff + i*size);
}

TEMPLATE_T
int Resource<T>::Checksum() const
{
	size_t size = sizeof(T) * data_.size();
	boost::crc_32_type crc;
	crc.process_bytes(data_.data(), size);
	return crc.checksum();
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DEF
#undef TEMPLATE_T

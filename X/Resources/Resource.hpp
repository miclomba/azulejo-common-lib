#define TEMPLATE_T template<typename T>
#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
TEMPLATE_T \
template<typename U, typename std::enable_if_t<std::is_arithmetic<U>::value, int>> 

TEMPLATE_T
void Resource<T>::LoadInput(const std::vector<std::vector<T>>& data)
{
	SetColumnSize(data.size());
	if (!data.empty())
		SetRowSize(data[0].size());
	data_.resize(GetColumnSize() * GetRowSize());

	size_t i = 0;
	for (const std::vector<T>& row : data)
	{
		if (GetRowSize() != row.size())
			throw std::invalid_argument("Non-square matrix provided as input during Resource construction");
		for (const T& col : row)
			data_[i++] = col;
	}
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource() 
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(const std::vector<std::vector<T>>& data) 
{
	LoadInput(data);
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(std::vector<std::vector<T>>&& data) 
{
	LoadInput(data);
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
T& Resource<T>::Data(const size_t i, const size_t j)
{
	if (i < 0 || i >= GetColumnSize() || j < 0 || j >= GetRowSize())
		throw std::invalid_argument("Resource indices " + std::to_string(i) + "," + std::to_string(j) + " are out of bounds");
	return data_[i*GetRowSize() + j];
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

	if (GetColumnSize() == 0 || GetRowSize() == 0)
		throw std::runtime_error("Resource data row/column dimensions are not set during Resource<T>::Assign");

	data_.resize(GetColumnSize() * GetRowSize());

	int x = 0;
	int y = -1;
	for (size_t i = 0; i < n / size; ++i)
	{
		if (i % GetRowSize() == 0)
		{
			x = 0;
			++y;
		}
		data_[y*GetRowSize() + x++] = *reinterpret_cast<const T*>(buff + i*size);
	}
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

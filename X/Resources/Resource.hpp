#define TEMPLATE_T template<typename T>
#define ENABLE_IF_CONTAINER_TEMPLATE_DEF \
TEMPLATE_T \
template<typename U, typename std::enable_if_t<std::is_arithmetic<U>::value, int>> 

TEMPLATE_T
void Resource<T>::ValidateInput()
{
	size_t m = data_.size();
	for (const std::vector<T>& row : data_)
	{
		if (m != row.size())
			throw std::invalid_argument("Non-square matrix provided as input during Resource construction");
	}
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource() 
{
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(const std::vector<std::vector<T>>& data) : data_(data)
{
	ValidateInput();
}

ENABLE_IF_CONTAINER_TEMPLATE_DEF
Resource<T>::Resource(std::vector<std::vector<T>>&& data) : data_(std::move(data))
{
	ValidateInput();
}

TEMPLATE_T Resource<T>::Resource(const Resource<T>&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(const Resource<T>&) = default;
TEMPLATE_T Resource<T>::Resource(Resource<T>&&) = default;
TEMPLATE_T Resource<T>& Resource<T>::operator=(Resource<T>&&) = default;

TEMPLATE_T
Resource<T>::~Resource() = default;

TEMPLATE_T
const std::vector<std::vector<T>>& Resource<T>::Data() const
{
	return data_;
}

TEMPLATE_T
std::vector<std::vector<T>>& Resource<T>::Data()
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

	size_t size = sizeof(T);
	if (n % size != 0)
		throw std::runtime_error("n is not a multiple of sizeof(T) during Resource<T>::Assign");

	int N = std::sqrt(n/size);
	std::vector<std::vector<T>>& data = Data(); 
	data.resize(N, std::vector<T>(N));

	int x = 0;
	int y = -1;
	for (size_t i = 0; i < n / size; ++i)
	{
		if (i % N == 0) ++y;
		data[y][x++] = buff[i*size];
	}
}

TEMPLATE_T
std::vector<int> Resource<T>::Checksum() const
{
	size_t i = 0;
	std::vector<int> results;
	results.resize(data_.size());

	boost::crc_32_type crc;
	for (const std::vector<T>& row : data_)
	{
		size_t size = sizeof(T) * row.size();
		crc.process_bytes(row.data(), size);
		results[i++] = crc.checksum();
	}

	return results;
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DEF
#undef TEMPLATE_T

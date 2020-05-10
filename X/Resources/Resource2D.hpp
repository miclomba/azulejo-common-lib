#define TEMPLATE_T template<typename T>

TEMPLATE_T
void Resource2D<T>::LoadInput(const std::vector<std::vector<T>>& data)
{
	this->SetColumnSize(data.size());
	if (!data.empty())
		this->SetRowSize(data[0].size());
	this->data_.resize(this->GetColumnSize() * this->GetRowSize());

	size_t i = 0;
	for (const std::vector<T>& row : data)
	{
		if (this->GetRowSize() != row.size())
			throw std::invalid_argument("Non-square matrix provided as input during Resource construction");
		for (const T& col : row)
			this->data_[i++] = col;
	}
}

TEMPLATE_T
Resource2D<T>::Resource2D() 
{
}

TEMPLATE_T
Resource2D<T>::Resource2D(const std::vector<std::vector<T>>& data) 
{
	LoadInput(data);
}

TEMPLATE_T
Resource2D<T>::Resource2D(std::vector<std::vector<T>>&& data) 
{
	LoadInput(data);
}

TEMPLATE_T Resource2D<T>::Resource2D(const Resource2D<T>&) = default;
TEMPLATE_T Resource2D<T>& Resource2D<T>::operator=(const Resource2D<T>&) = default;
TEMPLATE_T Resource2D<T>::Resource2D(Resource2D<T>&&) = default;
TEMPLATE_T Resource2D<T>& Resource2D<T>::operator=(Resource2D<T>&&) = default;

TEMPLATE_T
Resource2D<T>::~Resource2D() = default;

TEMPLATE_T
T& Resource2D<T>::GetData(const size_t i, const size_t j)
{
	if (i < 0 || i >= this->GetColumnSize() || j < 0 || j >= this->GetRowSize())
		throw std::invalid_argument("Resource indices " + std::to_string(i) + "," + std::to_string(j) + " are out of bounds");
	return this->data_[i*this->GetRowSize() + j];
}

TEMPLATE_T
const T& Resource2D<T>::GetData(const size_t i, const size_t j) const
{
	if (i < 0 || i >= this->GetColumnSize() || j < 0 || j >= this->GetRowSize())
		throw std::invalid_argument("Resource indices " + std::to_string(i) + "," + std::to_string(j) + " are out of bounds");
	return this->data_[i*this->GetRowSize() + j];
}

TEMPLATE_T
void Resource2D<T>::Assign(const char* buff, const size_t n)
{
	if (!buff)
		throw std::runtime_error("Buffer cannot be NULL during Resource::Assign");
	if (n < 1)
		throw std::runtime_error("N cannot be 0 during Resource::Assign");

	size_t size = sizeof(T);
	if (n % size != 0)
		throw std::runtime_error("n is not a multiple of sizeof(T) during Resource<T>::Assign");

	if (this->GetColumnSize() == 0 || this->GetRowSize() == 0)
		throw std::runtime_error("Resource data row/column dimensions are not set during Resource<T>::Assign");

	this->data_.resize(this->GetColumnSize() * this->GetRowSize());

	int x = 0;
	int y = -1;
	for (size_t i = 0; i < n / size; ++i)
	{
		if (i % this->GetRowSize() == 0)
		{
			x = 0;
			++y;
		}
		this->data_[y*this->GetRowSize() + x++] = *reinterpret_cast<const T*>(buff + i * size);
	}
}

TEMPLATE_T
void* Resource2D<T>::Data()
{
	return data_.data();
}

TEMPLATE_T
const void* Resource2D<T>::Data() const
{
	return data_.data();
}

TEMPLATE_T
size_t Resource2D<T>::GetElementSize() const
{
	return sizeof(T);
}

TEMPLATE_T
int Resource2D<T>::Checksum() const
{
	size_t size = sizeof(T) * data_.size();
	boost::crc_32_type crc;
	crc.process_bytes(data_.data(), size);
	return crc.checksum();
}

#undef TEMPLATE_T

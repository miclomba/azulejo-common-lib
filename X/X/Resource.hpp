
template<typename T>
int Resource<T>::Checksum() const
{
	boost::crc_32_type result;
	size_t size = sizeof(*data_.data()) * data_.size();
	result.process_bytes(data_.data(), size);
	return result.checksum();
}

template<typename T>
Resource<T>::~Resource() = default;

template<typename T>
bool Resource<T>::IsDirty() const
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
const T& Resource<T>::Data() const
{
	return data_;
}

template<typename T>
T& Resource<T>::Data()
{
	return data_;
}


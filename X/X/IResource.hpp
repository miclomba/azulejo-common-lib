
template<typename T>
int IResource<T>::Checksum() const
{
	boost::crc_32_type result;
	size_t size = sizeof(*data_.data()) * data_.size();
	result.process_bytes(data_.data(), size);
	return result.checksum();
}

template<typename T>
IResource<T>::IResource(const T& data) : data_(data)
{
}

template<typename T>
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

template<typename T>
void IResource<T>::Save(const std::string& path) const
{
	if (!IsDirty())
		return;
	SaveImpl(path);
}

template<typename T>
void IResource<T>::Load(const std::string& path)
{
	LoadImpl(path);
}

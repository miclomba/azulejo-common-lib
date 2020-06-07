#ifndef container_resource
#define container_resource

#include <stdexcept>
#include <vector>

#include "DatabaseAdapters/ITabularizableResource.h"
#include "FilesystemAdapters/ISerializableResource.h"

template<typename T = int>
class ContainerResource : 
	public filesystem_adapters::ISerializableResource,
	public database_adapters::ITabularizableResource
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<T>&& values) :
		data_(std::move(values))
	{
		SetRowSize(data_.size());
		SetColumnSize(1);
	}

	ContainerResource(const std::vector<T>& values) :
		data_(values)
	{
		SetRowSize(data_.size());
		SetColumnSize(1);
	}

	bool UpdateChecksumProtected() { return UpdateChecksum(); }
	int ChecksumProtected() { return Checksum(); }

	void Assign(const char* buff, const size_t n) override
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
			data_[i] = *reinterpret_cast<const T*>(buff + i * size);
	}

	void* Data() override
	{
		return data_.data();
	}

	const void* Data() const override
	{
		return data_.data();
	}

	size_t GetElementSize() const override
	{
		return sizeof(T);
	}

private:
	std::vector<T> data_;
};

#endif
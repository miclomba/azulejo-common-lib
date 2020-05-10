#ifndef container_resource_2d
#define container_resource_2d

#include <stdexcept>
#include <vector>

#include "FilesystemAdapters/ISerializableResource.h"

template<typename T = int>
class ContainerResource2D : public filesystem_adapters::ISerializableResource
{
public:
	ContainerResource2D() = default;
	ContainerResource2D(const std::vector<std::vector<T>>& values)
	{
		LoadInput(values);
	}

	ContainerResource2D(std::vector<std::vector<T>>&& values)
	{
		LoadInput(values);
	}

	void LoadInput(const std::vector<std::vector<T>>& data)
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

	T& GetData(const size_t i, const size_t j)
	{
		if (i < 0 || i >= this->GetColumnSize() || j < 0 || j >= this->GetRowSize())
			throw std::invalid_argument("Resource indices " + std::to_string(i) + "," + std::to_string(j) + " are out of bounds");
		return this->data_[i*this->GetRowSize() + j];
	}

	const T& GetData(const size_t i, const size_t j) const
	{
		if (i < 0 || i >= this->GetColumnSize() || j < 0 || j >= this->GetRowSize())
			throw std::invalid_argument("Resource indices " + std::to_string(i) + "," + std::to_string(j) + " are out of bounds");
		return this->data_[i*this->GetRowSize() + j];
	}

	bool IsDirtyProtected() { return IsDirty(); }
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
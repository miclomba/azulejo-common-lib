#include "DatabaseAdapters/EntityHierarchyBlob.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "DatabaseAdapters/ITabularizableResource.h"

namespace pt = boost::property_tree;
using database_adapters::EntityHierarchyBlob;
using database_adapters::ITabularizableResource;

namespace
{
	pt::ptree MakeHierarchyTree(const char *data)
	{
		std::string hierarchyJSON(data);
		std::stringstream jsonStream(hierarchyJSON);

		pt::ptree hierarchyTree;

		try
		{
			pt::json_parser::read_json(jsonStream, hierarchyTree);
		}
		catch (const pt::json_parser_error &e)
		{
			throw std::runtime_error("Could not MakeHierarchyTree from buffer: " + std::string(e.what()));
		}

		return hierarchyTree;
	}
}

EntityHierarchyBlob::EntityHierarchyBlob() = default;

EntityHierarchyBlob::EntityHierarchyBlob(const pt::ptree &hierarchyTree)
{
	hierarchyTree_ = hierarchyTree;

	std::stringstream buffer;
	try
	{
		pt::json_parser::write_json(buffer, hierarchyTree);
	}
	catch (const pt::json_parser_error &e)
	{
		throw std::runtime_error("Could not create EntityHierarchyBlob from ptree: " + std::string(e.what()));
	}

	data_ = std::move(*buffer.rdbuf()).str();
	SetRowSize(data_.size());
	SetColumnSize(1);
}

EntityHierarchyBlob::EntityHierarchyBlob(const std::string &buff)
{
	data_ = buff;
	SetRowSize(data_.size());
	SetColumnSize(1);

	hierarchyTree_ = MakeHierarchyTree(reinterpret_cast<const char *>(Data()));
}

EntityHierarchyBlob::EntityHierarchyBlob(std::string &&buff)
{
	data_ = std::move(buff);
	SetRowSize(data_.size());
	SetColumnSize(1);

	hierarchyTree_ = MakeHierarchyTree(reinterpret_cast<const char *>(Data()));
}

EntityHierarchyBlob::~EntityHierarchyBlob() = default;

const pt::ptree &EntityHierarchyBlob::GetHierarchyTree()
{
	return hierarchyTree_;
}

void EntityHierarchyBlob::Assign(const char *buff, const size_t n)
{
	if (!buff)
		throw std::runtime_error("Buffer cannot be NULL during EntityHierarchyBlob::Assign");
	if (n < 1)
		throw std::runtime_error("N cannot be 0 during EntityHierarchyBlob::Assign");

	data_.resize(n);
	for (size_t i = 0; i < n; ++i)
		data_[i] = *(buff + i);

	hierarchyTree_ = MakeHierarchyTree(reinterpret_cast<const char *>(Data()));
}

void *EntityHierarchyBlob::Data()
{
	return data_.data();
}

const void *EntityHierarchyBlob::Data() const
{
	return data_.data();
}

size_t EntityHierarchyBlob::GetElementSize() const
{
	return sizeof(char);
}

#include "Resources/IResource.h"
#include "Resources/Resource.h"

#include <vector>

#include "config.h"

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/pure_virtual.hpp>
#include <boost/python/ptr.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

namespace
{
std::vector<double>& (resource::Resource<double>::*f_ptr_vec_d)() = &resource::Resource<double>::Data;
std::vector<float>& (resource::Resource<float>::*f_ptr_vec_f)() = &resource::Resource<float>::Data;
std::vector<int>& (resource::Resource<int>::*f_ptr_vec_i)() = &resource::Resource<int>::Data;
std::vector<unsigned int>& (resource::Resource<unsigned int>::*f_ptr_vec_ui)() = &resource::Resource<unsigned int>::Data;
}

BOOST_PYTHON_MODULE(ResourcesModule)
{
	class_<std::vector<double>>("VecDouble").def(vector_indexing_suite<std::vector<double>>());
	class_<std::vector<float>>("VecFloat").def(vector_indexing_suite<std::vector<float>>());
	class_<std::vector<int>>("VecInt").def(vector_indexing_suite<std::vector<int>>());
	class_<std::vector<unsigned int>>("VecUInt").def(vector_indexing_suite<std::vector<unsigned int>>());
	
	class_<resource::Resource<double>>("ResourceVecDouble")
		.def(init<const std::vector<double>&>())
		.def("data", f_ptr_vec_d, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<float>>("ResourceVecFloat")
		.def(init<const std::vector<float>&>())
		.def("data", f_ptr_vec_f, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<int>>("ResourceVecInt")
		.def(init<const std::vector<int>&>())
		.def("data", f_ptr_vec_i, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<unsigned int>>("ResourceVecUInt")
		.def(init<const std::vector<unsigned int>&>())
		.def("data", f_ptr_vec_ui, return_value_policy<reference_existing_object>())
	;
}
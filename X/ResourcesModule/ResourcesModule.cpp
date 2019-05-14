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
typedef std::vector<double> vec_d;
typedef std::vector<float> vec_f;
typedef std::vector<int> vec_i;
typedef std::vector<unsigned int> vec_ui;

vec_d& (resource::Resource<vec_d>::*f_ptr_vec_d)() = &resource::Resource<vec_d>::Data;
vec_f& (resource::Resource<vec_f>::*f_ptr_vec_f)() = &resource::Resource<vec_f>::Data;
vec_i& (resource::Resource<vec_i>::*f_ptr_vec_i)() = &resource::Resource<vec_i>::Data;
vec_ui& (resource::Resource<vec_ui>::*f_ptr_vec_ui)() = &resource::Resource<vec_ui>::Data;
}

BOOST_PYTHON_MODULE(ResourcesModule)
{
	class_<vec_d>("VecDouble").def(vector_indexing_suite<vec_d>());
	class_<vec_f>("VecFloat").def(vector_indexing_suite<vec_f>());
	class_<vec_i>("VecInt").def(vector_indexing_suite<vec_i>());
	class_<vec_ui>("VecUInt").def(vector_indexing_suite<vec_ui>());
	
	class_<resource::Resource<vec_d>>("ResourceVecDouble")
		.def(init<const vec_d&>())
		.def("data", f_ptr_vec_d, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<vec_f>>("ResourceVecFloat")
		.def(init<const vec_f&>())
		.def("data", f_ptr_vec_f, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<vec_i>>("ResourceVecInt")
		.def(init<const vec_i&>())
		.def("data", f_ptr_vec_i, return_value_policy<reference_existing_object>())
	;

	class_<resource::Resource<vec_ui>>("ResourceVecUInt")
		.def(init<const vec_ui&>())
		.def("data", f_ptr_vec_ui, return_value_policy<reference_existing_object>())
	;
}
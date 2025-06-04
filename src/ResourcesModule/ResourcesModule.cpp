#include "Resources/IResource.h"

#include <vector>

#include "ResourcesModule/config.h"

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/pure_virtual.hpp>
#include <boost/python/ptr.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using MatrixDouble = std::vector<std::vector<double>>;
using MatrixFloat = std::vector<std::vector<float>>;
using MatrixInt = std::vector<std::vector<int>>;
using MatrixUInt = std::vector<std::vector<unsigned int>>;
using VecDouble = std::vector<double>;
using VecFloat = std::vector<float>;
using VecInt = std::vector<int>;
using VecUInt = std::vector<unsigned int>;

namespace
{
	// double& (resource::Resource<double>::*f_ptr_vec_d)() = &resource::Resource<double>::Data;
	// float& (resource::Resource<float>::*f_ptr_vec_f)() = &resource::Resource<float>::Data;
	// int& (resource::Resource<int>::*f_ptr_vec_i)() = &resource::Resource<int>::Data;
	// unsigned int& (resource::Resource<unsigned int>::*f_ptr_vec_ui)() = &resource::Resource<unsigned int>::Data;
}

BOOST_PYTHON_MODULE(ResourcesModule)
{
	class_<MatrixDouble>("MatrixDouble").def(vector_indexing_suite<MatrixDouble>());
	class_<MatrixFloat>("MatrixFloat").def(vector_indexing_suite<MatrixFloat>());
	class_<MatrixInt>("MatrixInt").def(vector_indexing_suite<MatrixInt>());
	class_<MatrixUInt>("MatrixUInt").def(vector_indexing_suite<MatrixUInt>());
	class_<VecDouble>("VecDouble").def(vector_indexing_suite<VecDouble>());
	class_<VecFloat>("VecFloat").def(vector_indexing_suite<VecFloat>());
	class_<VecInt>("VecInt").def(vector_indexing_suite<VecInt>());
	class_<VecUInt>("VecUInt").def(vector_indexing_suite<VecUInt>());

	// class_<resource::Resource2D<double>>("Resource2DDouble")
	//	.def(init<const MatrixDouble&>())
	//.def("data", f_ptr_vec_d, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource2D<float>>("Resource2DFloat")
	//.def(init<const MatrixFloat&>())
	//.def("data", f_ptr_vec_f, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource2D<int>>("Resource2DInt")
	//.def(init<const MatrixInt&>())
	//.def("data", f_ptr_vec_i, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource2D<unsigned int>>("Resource2DUInt")
	//.def(init<const MatrixUInt&>())
	//.def("data", f_ptr_vec_ui, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource<double>>("ResourceDouble")
	//.def(init<const VecDouble&>())
	//.def("data", f_ptr_vec_d, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource<float>>("ResourceFloat")
	//.def(init<const VecFloat&>())
	//.def("data", f_ptr_vec_f, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource<int>>("ResourceInt")
	//.def(init<const VecInt&>())
	//.def("data", f_ptr_vec_i, return_value_policy<reference_existing_object>())
	//;

	// class_<resource::Resource<unsigned int>>("ResourceUInt")
	//.def(init<const VecUInt&>())
	//.def("data", f_ptr_vec_ui, return_value_policy<reference_existing_object>())
	//;
}
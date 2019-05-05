#include "Resources/Resource.h"

#include "config.h"

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(ResourcesModule)
{
/*

	const T& Data() const;
	T& Data();

	void Assign(const char* buff, const size_t n) override;
*/

	//class_<resource::Resource<double>>("ResourceDouble")
		//.def("assign", &resource::Resource<double>::Assign)
		//.def("data", &resource::Resource<double>::Data)
	//	;
}
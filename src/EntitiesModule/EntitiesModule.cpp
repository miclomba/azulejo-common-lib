#include "Entities/Entity.h"

#include "EntitiesModule/config.h"

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(EntitiesModule)
{
	class_<entity::Entity>("Entity", no_init)
		.def("get_key", &entity::Entity::GetKey)
		.def("set_key", &entity::Entity::SetKey);
}
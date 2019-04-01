#include "IEventEmitter.h"

#include <boost/signals2/connection.hpp>

namespace events 
{

IEventEmitter::IEventEmitter() = default;
IEventEmitter::IEventEmitter(const IEventEmitter&) = default;
IEventEmitter& IEventEmitter::operator=(const IEventEmitter&) = default;
IEventEmitter::IEventEmitter(IEventEmitter&&) = default;
IEventEmitter& IEventEmitter::operator=(IEventEmitter&&) = default;

IEventEmitter::~IEventEmitter()
{
	for (auto& keyNodePair : subscriberMap_)
	{
		boost::signals2::connection& conn = keyNodePair.second;
		conn.disconnect();
	}
}

void IEventEmitter::Disconnect(const std::string& key)
{
	if (subscriberMap_.find(key) != subscriberMap_.cend())
	{
		subscriberMap_[key].disconnect();
		subscriberMap_.erase(key);
	}
	else
	{
		throw std::runtime_error("Cannot remove event subscriber with key=" + key + " from EventEmitter");
	}
}

} // end namespace events
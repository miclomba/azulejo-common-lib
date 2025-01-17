/**
 * @file IEventEmitter.h
 * @brief Declaration of the IEventEmitter interface for emitting events to event consumers.
 */

#ifndef events_ievent_emitter_h
#define events_ievent_emitter_h

#include <memory>
#include <string>

#include "config.h"

#include <boost/signals2/connection.hpp>

namespace events
{

class IEventConsumer;

/**
 * @class IEventEmitter
 * @brief An interface for emitting events to subscribed event consumers.
 *
 * The IEventEmitter interface provides methods to manage subscriptions and emit events to connected consumers.
 */
class EVENTS_DLL_EXPORT IEventEmitter
{
public:
    /**
     * @brief Default constructor for the IEventEmitter interface.
     */
    IEventEmitter();

    /**
     * @brief Virtual destructor for the IEventEmitter interface.
     */
    virtual ~IEventEmitter();

    /**
     * @brief Copy constructor.
     * @param other The IEventEmitter instance to copy from.
     */
    IEventEmitter(const IEventEmitter& other);

    /**
     * @brief Copy assignment operator.
     * @param other The IEventEmitter instance to copy from.
     * @return Reference to the updated IEventEmitter instance.
     */
    IEventEmitter& operator=(const IEventEmitter& other);

    /**
     * @brief Move constructor.
     * @param other The IEventEmitter instance to move from.
     */
    IEventEmitter(IEventEmitter&& other);

    /**
     * @brief Move assignment operator.
     * @param other The IEventEmitter instance to move from.
     * @return Reference to the updated IEventEmitter instance.
     */
    IEventEmitter& operator=(IEventEmitter&& other);

    /**
     * @brief Connect a subscriber to the event emitter.
     * @param subscriber A shared pointer to the event consumer to subscribe.
     * @return A connection object that manages the subscription.
     */
    virtual boost::signals2::connection Connect(const std::shared_ptr<IEventConsumer> subscriber) = 0;

    /**
     * @brief Get the type of subscriber the event emitter supports.
     * @return A string representing the subscriber type.
     */
    virtual std::string GetSubscriberType() const = 0;
};

} // end namespace events

#endif // events_ievent_emitter_h

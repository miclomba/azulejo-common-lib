/**
 * @file IEventConsumer.h
 * @brief Declaration of the IEventConsumer interface for consuming events.
 */

#ifndef events_ievent_consumer_h
#define events_ievent_consumer_h

#include <string>

#include "config.h"

namespace events
{

/**
 * @class IEventConsumer
 * @brief An interface for consuming events emitted by an event emitter.
 *
 * The IEventConsumer interface provides a method to identify the type of subscriber for compatibility with event emitters.
 */
class EVENTS_DLL_EXPORT IEventConsumer
{
public:
    /**
     * @brief Default constructor for the IEventConsumer interface.
     */
    IEventConsumer();

    /**
     * @brief Virtual destructor for the IEventConsumer interface.
     */
    virtual ~IEventConsumer();

    /**
     * @brief Copy constructor.
     * @param other The IEventConsumer instance to copy from.
     */
    IEventConsumer(const IEventConsumer& other);

    /**
     * @brief Copy assignment operator.
     * @param other The IEventConsumer instance to copy from.
     * @return Reference to the updated IEventConsumer instance.
     */
    IEventConsumer& operator=(const IEventConsumer& other);

    /**
     * @brief Move constructor.
     * @param other The IEventConsumer instance to move from.
     */
    IEventConsumer(IEventConsumer&& other);

    /**
     * @brief Move assignment operator.
     * @param other The IEventConsumer instance to move from.
     * @return Reference to the updated IEventConsumer instance.
     */
    IEventConsumer& operator=(IEventConsumer&& other);

    /**
     * @brief Get the type of subscriber the event consumer represents.
     * @return A string representing the subscriber type.
     */
    virtual std::string GetSubscriberType() const = 0;
};

} // end namespace events

#endif // events_ievent_consumer_h
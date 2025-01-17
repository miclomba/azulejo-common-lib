/**
 * @file EventChannel.h
 * @brief Declaration of the EventChannel class for managing event communication between emitters and consumers.
 */

#ifndef events_event_channel_h
#define events_event_channel_h

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "config.h"

namespace events
{

class IEventEmitter;
class IEventConsumer;

/**
 * @class EventChannel
 * @brief A class for managing event communication between event emitters and consumers.
 *
 * The EventChannel class provides methods to register and unregister emitters and consumers, facilitating event-based communication.
 */
class EVENTS_DLL_EXPORT EventChannel
{
public:
    /**
     * @brief Default constructor for the EventChannel class.
     */
    EventChannel();

    /**
     * @brief Destructor for the EventChannel class.
     */
    virtual ~EventChannel();

    /**
     * @brief Copy constructor.
     * @param other The EventChannel instance to copy from.
     */
    EventChannel(const EventChannel& other);

    /**
     * @brief Copy assignment operator.
     * @param other The EventChannel instance to copy from.
     * @return Reference to the updated EventChannel instance.
     */
    EventChannel& operator=(const EventChannel& other);

    /**
     * @brief Move constructor.
     * @param other The EventChannel instance to move from.
     */
    EventChannel(EventChannel&& other);

    /**
     * @brief Move assignment operator.
     * @param other The EventChannel instance to move from.
     * @return Reference to the updated EventChannel instance.
     */
    EventChannel& operator=(EventChannel&& other);

    /**
     * @brief Register an event emitter with a unique key.
     * @param emitterKey The key associated with the emitter.
     * @param emitter A shared pointer to the event emitter.
     */
    void RegisterEmitter(const std::string& emitterKey, const std::shared_ptr<IEventEmitter> emitter);

    /**
     * @brief Unregister an event emitter using its key.
     * @param emitterKey The key of the emitter to unregister.
     */
    void UnregisterEmitter(const std::string& emitterKey);

    /**
     * @brief Register an event consumer to a specific emitter.
     * @param consumerKey The key associated with the consumer.
     * @param emitterKey The key of the emitter to connect the consumer to.
     * @param consumer A shared pointer to the event consumer.
     */
    void RegisterConsumer(const std::string& consumerKey, const std::string& emitterKey, const std::shared_ptr<IEventConsumer> consumer);

    /**
     * @brief Unregister an event consumer from a specific emitter.
     * @param consumerKey The key associated with the consumer.
     * @param emitterKey The key of the emitter to disconnect the consumer from.
     */
    void UnregisterConsumer(const std::string& consumerKey, const std::string& emitterKey);

protected:
    /**
     * @brief Check if an emitter is registered with a specific key.
     * @param emitterKey The key of the emitter to check.
     * @return True if the emitter is registered, false otherwise.
     */
    bool IsEmitterRegistered(const std::string& emitterKey) const;

    /**
     * @brief Check if a consumer is registered to a specific emitter.
     * @param consumerKey The key of the consumer to check.
     * @param emitterKey The key of the emitter to check.
     * @return True if the consumer is registered to the emitter, false otherwise.
     */
    bool IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const;

private:
    /** @brief Map of emitter keys to weak pointers of event emitters. */
    std::map<std::string, std::weak_ptr<IEventEmitter>> emitterMap_;

    /** @brief Map of consumer-emitter pairs to weak pointers of event consumers. */
    std::map<std::pair<std::string, std::string>, std::weak_ptr<IEventConsumer>> consumerMap_;
};

} // end namespace events

#endif // events_event_channel_h
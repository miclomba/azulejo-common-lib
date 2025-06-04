/**
 * @file EventConsumer.h
 * @brief Declaration of the EventConsumer class template for consuming events from an event emitter.
 */

#ifndef events_event_consumer_h
#define events_event_consumer_h

#include <functional>
#include <string>
#include <typeinfo>

#include "Events/config.h"

#include "Events/IEventConsumer.h"

namespace events
{

    /**
     * @class EventConsumer
     * @brief A class template for consuming events emitted by an event emitter.
     *
     * This class implements the IEventConsumer interface, allowing subscription to events of type T.
     *
     * @tparam T The event type to be consumed.
     */
    template <typename T>
    class EVENTS_DLL_EXPORT EventConsumer : public IEventConsumer
    {
    public:
        /**
         * @brief Constructor for the EventConsumer class.
         * @param subscriber A function object representing the event handling logic.
         */
        EventConsumer(const std::function<T> &subscriber);

        /**
         * @brief Destructor for the EventConsumer class.
         */
        virtual ~EventConsumer();

        /**
         * @brief Copy constructor.
         * @param other The EventConsumer instance to copy from.
         */
        EventConsumer(const EventConsumer &other);

        /**
         * @brief Copy assignment operator.
         * @param other The EventConsumer instance to copy from.
         * @return Reference to the updated EventConsumer instance.
         */
        EventConsumer &operator=(const EventConsumer &other);

        /**
         * @brief Move constructor.
         * @param other The EventConsumer instance to move from.
         */
        EventConsumer(EventConsumer &&other);

        /**
         * @brief Move assignment operator.
         * @param other The EventConsumer instance to move from.
         * @return Reference to the updated EventConsumer instance.
         */
        EventConsumer &operator=(EventConsumer &&other);

        /**
         * @brief Get the subscriber function for handling events.
         * @return A function object representing the subscriber.
         */
        std::function<T> GetSubscriber() const;

        /**
         * @brief Get the type of subscriber.
         * @return A string representing the subscriber type.
         */
        std::string GetSubscriberType() const override;

    private:
        /** @brief The function object representing the event handling logic. */
        std::function<T> subscriber_;
    };

#include "Events/EventConsumer.hpp"

} // end namespace events

#endif // events_event_consumer_h

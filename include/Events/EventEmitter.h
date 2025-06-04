/**
 * @file EventEmitter.h
 * @brief Declaration of the EventEmitter class template for emitting events to subscribed consumers.
 */

#ifndef events_event_emitter_h
#define events_event_emitter_h

#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "Events/config.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>

#include "Events/IEventConsumer.h"
#include "Events/IEventEmitter.h"
#include "Events/EventConsumer.h"

namespace events
{

    /**
     * @class EventEmitter
     * @brief A class template for managing and emitting events to subscribed consumers.
     *
     * This class implements the IEventEmitter interface, allowing subscribers to connect and receive events of type T.
     *
     * @tparam T The event type to be emitted.
     */
    template <typename T>
    class EVENTS_DLL_EXPORT EventEmitter : public IEventEmitter
    {
    public:
        /**
         * @brief Default constructor for the EventEmitter class.
         */
        EventEmitter();

        /**
         * @brief Virtual destructor for the EventEmitter class.
         */
        virtual ~EventEmitter();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        EventEmitter(const EventEmitter &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        EventEmitter &operator=(const EventEmitter &) = delete;

        /**
         * @brief Move constructor.
         * @param other The EventEmitter instance to move from.
         */
        EventEmitter(EventEmitter &&other);

        /**
         * @brief Move assignment operator.
         * @param other The EventEmitter instance to move from.
         * @return Reference to the updated instance.
         */
        EventEmitter &operator=(EventEmitter &&other);

        /**
         * @brief Connect a subscriber to the event emitter.
         * @param subscriber A shared pointer to the event consumer to subscribe.
         * @return A connection object that manages the subscription.
         */
        boost::signals2::connection Connect(const std::shared_ptr<IEventConsumer> subscriber) override;

        /**
         * @brief Get the type of subscriber the event emitter supports.
         * @return A string representing the subscriber type.
         */
        std::string GetSubscriberType() const override;

        /**
         * @brief Get the signal object for emitting events.
         * @return A constant reference to the signal object.
         */
        const boost::signals2::signal<T> &Signal() const;

    private:
        /** @brief The signal object used to emit events. */
        boost::signals2::signal<T> emitter_;
    };

#include "Events/EventEmitter.hpp"
} // end namespace events

#endif // events_event_emitter_h

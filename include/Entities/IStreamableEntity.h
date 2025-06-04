/**
 * @file IStreamableEntity.h
 * @brief Declaration of the IStreamableEntity interface for streamable entities.
 */

#ifndef entity_istreamableentity_h
#define entity_istreamableentity_h

#include <fstream>
#include <string>

#include "Entities/config.h"
#include "Entities/Entity.h"

namespace entity
{

    /**
     * @class IStreamableEntity
     * @brief An interface for entities that can be serialized to and deserialized from strings or streams.
     *
     * Inherits from Entity.
     */
    class ENTITY_DLL_EXPORT IStreamableEntity : public virtual Entity
    {
    public:
        /**
         * @brief Default constructor for the IStreamableEntity class.
         */
        IStreamableEntity();

        /**
         * @brief Destructor for the IStreamableEntity class.
         */
        virtual ~IStreamableEntity();

        /**
         * @brief Copy constructor.
         * @param other The IStreamableEntity instance to copy from.
         */
        IStreamableEntity(const IStreamableEntity &other);

        /**
         * @brief Copy assignment operator.
         * @param other The IStreamableEntity instance to copy from.
         * @return Reference to the updated IStreamableEntity instance.
         */
        IStreamableEntity &operator=(const IStreamableEntity &other);

        /**
         * @brief Move constructor.
         * @param other The IStreamableEntity instance to move from.
         */
        IStreamableEntity(IStreamableEntity &&other);

        /**
         * @brief Move assignment operator.
         * @param other The IStreamableEntity instance to move from.
         * @return Reference to the updated IStreamableEntity instance.
         */
        IStreamableEntity &operator=(IStreamableEntity &&other);

        /**
         * @brief Serialize the entity to a string representation.
         * @return A string representing the serialized entity.
         */
        virtual std::string ToString() const = 0;

        /**
         * @brief Deserialize the entity from a string representation.
         * @param serialization A string containing the serialized entity data.
         */
        virtual void FromString(const std::string &serialization) = 0;

        /**
         * @brief Get the delimiter used for serialization and deserialization.
         * @return A string representing the delimiter.
         */
        static std::string GetDelimeter();

        /**
         * @brief Stream insertion operator for serializing the entity.
         * @param to The output stream.
         * @param from The IStreamableEntity instance to serialize.
         * @return The output stream after writing the serialized data.
         */
        friend ENTITY_DLL_EXPORT std::ostream &operator<<(std::ostream &to, const IStreamableEntity &from);

        /**
         * @brief Stream extraction operator for deserializing the entity.
         * @param from The input stream containing serialized data.
         * @param to The IStreamableEntity instance to deserialize into.
         * @return The input stream after reading the serialized data.
         */
        friend ENTITY_DLL_EXPORT std::istream &operator>>(std::istream &from, IStreamableEntity &to);
    };

} // end namespace entity

#endif // entity_istreamableentity_h

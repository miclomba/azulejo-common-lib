/**
 * @file EntityDeserializer.h
 * @brief Declaration of the EntityDeserializer class for deserializing entities.
 */

#ifndef filesystem_adapters_entitydeserializer_h
#define filesystem_adapters_entitydeserializer_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "Config/filesystem.h"

#include "FilesystemAdapters/config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "FilesystemAdapters/ISerializableEntity.h"

namespace filesystem_adapters
{

    /**
     * @class EntityDeserializer
     * @brief A singleton class responsible for deserializing entities.
     *
     * Provides methods for entity registration, hierarchy management, and deserialization.
     */
    class FILESYSTEM_ADAPTERS_DLL_EXPORT EntityDeserializer
    {
    public:
        /**
         * @brief Destructor for the EntityDeserializer class.
         */
        virtual ~EntityDeserializer();

        /**
         * @brief Get the singleton instance of the EntityDeserializer.
         * @return Pointer to the EntityDeserializer instance.
         */
        static EntityDeserializer *GetInstance();

        /**
         * @brief Reset the singleton instance of the EntityDeserializer.
         */
        static void ResetInstance();

        /**
         * @brief Get the registry of serializable entities.
         * @return Reference to the EntityRegistry for ISerializableEntity.
         */
        entity::EntityRegistry<ISerializableEntity> &GetRegistry();

        /**
         * @brief Get the hierarchy of serialized entities.
         * @return Reference to the EntityHierarchy.
         */
        entity::EntityHierarchy &GetHierarchy();

        /**
         * @brief Load and deserialize an entity.
         * @param entity The serializable entity to load data into.
         */
        void LoadEntity(ISerializableEntity &entity);

    private:
        /**
         * @brief Default constructor for the EntityDeserializer class.
         *
         * Private to enforce the singleton pattern.
         */
        EntityDeserializer();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        EntityDeserializer(const EntityDeserializer &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        EntityDeserializer &operator=(const EntityDeserializer &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        EntityDeserializer(EntityDeserializer &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        EntityDeserializer &operator=(EntityDeserializer &&) = delete;

        /**
         * @brief Load an entity with an optional parent key.
         * @param entity The entity to load data into.
         * @param parentKey The key of the parent entity (default is an empty string).
         */
        void LoadWithParentKey(ISerializableEntity &entity, const entity::Entity::Key &parentKey = "");

        /** @brief Pointer to the singleton instance of the EntityDeserializer. */
        static EntityDeserializer *instance_;

        /** @brief Registry of serializable entities. */
        entity::EntityRegistry<ISerializableEntity> registry_;

        /** @brief Hierarchy of serialized entities. */
        entity::EntityHierarchy hierarchy_;
    };

} // end namespace filesystem_adapters

#endif // filesystem_adapters_entitydeserializer_h
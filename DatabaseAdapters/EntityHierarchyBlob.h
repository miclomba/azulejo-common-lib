/**
 * @file EntityHierarchyBlob.h
 * @brief Declaration of the EntityHierarchyBlob class for managing serialized entity hierarchy data.
 */

#ifndef database_adapters_entityhierarchyblob_h
#define database_adapters_entityhierarchyblob_h

#include <string>
#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "DatabaseAdapters/ITabularizableResource.h"

namespace database_adapters {

/**
 * @class EntityHierarchyBlob
 * @brief A class for handling serialized entity hierarchy data as a tabularizable resource.
 *
 * Inherits from `database_adapters::ITabularizableResource`.
 */
class DATABASE_ADAPTERS_DLL_EXPORT EntityHierarchyBlob : public database_adapters::ITabularizableResource
{
public:
    /**
     * @brief Default constructor for the EntityHierarchyBlob class.
     */
    EntityHierarchyBlob();

    /**
     * @brief Constructor for creating an EntityHierarchyBlob from a property tree.
     * @param hierarchyTree The property tree representing the entity hierarchy.
     */
    EntityHierarchyBlob(const boost::property_tree::ptree& hierarchyTree);

    /**
     * @brief Constructor for creating an EntityHierarchyBlob from a string buffer.
     * @param buff The string containing serialized hierarchy data.
     */
    EntityHierarchyBlob(const std::string& buff);

    /**
     * @brief Constructor for creating an EntityHierarchyBlob from a temporary string buffer.
     * @param buff The temporary string containing serialized hierarchy data.
     */
    EntityHierarchyBlob(std::string&& buff);

    /**
     * @brief Destructor for the EntityHierarchyBlob class.
     */
    virtual ~EntityHierarchyBlob();

    /**
     * @brief Get the property tree representing the entity hierarchy.
     * @return A reference to the property tree.
     */
    const boost::property_tree::ptree& GetHierarchyTree();

    /**
     * @brief Assign new serialized data to the EntityHierarchyBlob.
     * @param buff Pointer to the data buffer.
     * @param n The size of the data in bytes.
     */
    void Assign(const char* buff, const size_t n) override;

    /**
     * @brief Access the serialized data as a mutable pointer.
     * @return A pointer to the serialized data.
     */
    void* Data() override;

    /**
     * @brief Access the serialized data as a constant pointer.
     * @return A constant pointer to the serialized data.
     */
    const void* Data() const override;

    /**
     * @brief Get the size of an individual element in the serialized data.
     * @return The size of an individual element in bytes.
     */
    size_t GetElementSize() const override;

private:
    /** @brief The serialized data buffer. */
    std::string data_;

    /** @brief The property tree representing the entity hierarchy. */
    boost::property_tree::ptree hierarchyTree_;
};

} // namespace database_adapters

#endif // database_adapters_entityhierarchyblob_h
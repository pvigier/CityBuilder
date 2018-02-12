#pragma once

// STL
#include <vector>
// My includes
#include "util/Id.h"

/**
 * \brief Container that provides fast-access to elements using Ids
 *
 * \author Pierre Vigier
 */
template<typename T>
class IdManager
{
public:
    /**
     * \brief Default constructor
     */
    IdManager()
    {

    }

    /**
     * \brief Destructor
     */
    ~IdManager()
    {

    }

    /**
     * \brief Add an element
     *
     * The element is copied in the array.
     *
     * \param x Element to add in the container
     *
     *  \return Id of the added element
     */
    Id add(const T x)
    {
        // Add object
        std::size_t i = mObjects.size();
        mObjects.push_back(x);
        // Get the correct id and set the links
        Id id;
        if (mFreeIds.empty())
        {
            id = mIdToIndex.size();
            mIdToIndex.push_back(i);
        }
        else
        {
            id = mFreeIds.back();
            mFreeIds.pop_back();
            mIdToIndex[id] = i;
        }
        mIndexToId.push_back(id);
        return id;
    }

    /**
     * \brief Get an element
     *
     * \param id Id of the element
     *
     * \return Reference on the element which is associated to id
     */
    inline T& get(const Id id)
    {
        return mObjects[mIdToIndex[id]];
    }

    /**
     * \brief Get an element
     *
     * \param id Id of the element
     *
     * \return Const reference element which is associated to id
     */
    inline const T& get(const Id id) const
    {
        return mObjects[mIdToIndex[id]];
    }

    /**
     * \brief Erase an element
     *
     * \param id Id of the element
     */
    void erase(const Id id)
    {
        // Get the index of the object to destroy
        std::size_t i = mIdToIndex[id];
        // Swap with the last object and update its index
        mObjects[i] = mObjects.back();
        Id lastObjectId = mIndexToId.back();
        mIdToIndex[lastObjectId] = i;
        mIndexToId[i] = lastObjectId;
        // Erase the last object and its index
        mObjects.pop_back();
        mIndexToId.pop_back();
        // Add the deleted Id to the free Ids
        mFreeIds.push_back(id);
    }

    /**
     * \brief Return the elements
     *
     * \return Reference on the std::vector where the elements are stored in
     */
    inline std::vector<T>& getObjects()
    {
        return mObjects;
    }

    /**
     * \brief Return the elements
     *
     * \return Const reference on the std::vector where the elements are stored in
     */
    inline const std::vector<T>& getObjects() const
    {
        return mObjects;
    }

    /**
     * \brief Return the number of elements
     *
     * \return Number of elements in the array
     */
    inline std::size_t getSize() const
    {
        return mObjects.size();
    }

private:
    std::vector<std::size_t> mIdToIndex; /**< std::vector that maps the Id of an element to the index of its index in mObjects */
    std::vector<std::size_t> mFreeIds; /**< std::vector that contains free Ids */
    std::vector<T> mObjects; /**< std::vector which contains the elements */
    std::vector<Id> mIndexToId; /**< std::vector that maps the index of an element to its Id */
};
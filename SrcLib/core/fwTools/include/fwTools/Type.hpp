/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_TYPE_HPP_
#define _FWTOOLS_TYPE_HPP_

#include <limits>
#include <map>
#include <string>
#include <typeinfo>

#include <boost/cstdint.hpp>

#include <boost/type_traits/is_signed.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include "fwTools/config.hpp"
#include "fwTools/Stringizer.hpp"


namespace fwTools
{

/**
 * @brief   Class defining an elementary C++ type aka unsigned char, signed char, .... signed long, float, double
 * @class   Type
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 **/
class FWTOOLS_CLASS_API Type
{

public:
    struct FWTOOLS_CLASS_API ToolBase
    {
        FWTOOLS_API ToolBase();
        FWTOOLS_API ToolBase(const std::type_info &typeinfo);
        FWTOOLS_API  virtual std::string toString( ::boost::any value ) const;
        FWTOOLS_API virtual std::string toString( const void * ) const;

        const std::type_info &m_typeinfo;
    };

    template<typename T>
    struct  Tool : public ToolBase
    {
        Tool();
        virtual std::string toString( ::boost::any value ) const;
        virtual std::string toString( const void * ) const;
    };

    typedef std::map<std::string, Type> TypeMapType;

    /// Default constructor
    FWTOOLS_API Type();
    FWTOOLS_API Type(const std::string &type);

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator==(const Type &) const;

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator!=(const Type &) const;


    /**
     * @brief  define an order (lexicographic) for Type
     */
    FWTOOLS_API bool operator<( const Type& ) const;

    /**
     * @brief   Set Type value according given template
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    void setType();

    /**
     * @brief   Return true iff the Type value represents the TYPE
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    bool isOfType() const;

    /**
     * @brief   Return the sizeof of the type
     */
    FWTOOLS_API unsigned char sizeOf() const;

    /// Return a human readable string
    FWTOOLS_API const std::string &string() const;

    /// return type_info of represented type. If type is unspecified, return typeid(void)
    FWTOOLS_API const std::type_info &typeId() const;

    /**
     * @brief return the min and max storable in the Type. take care that min/max value are casted into template T
     */
    template<class T>
    const std::pair<T,T> minMax() const;

    /// return true iff the type use a fixed precision
    FWTOOLS_API bool isFixedPrecision() const;

    /// return true iff the type is signed
    FWTOOLS_API bool isSigned() const;

    FWTOOLS_API std::string toString( const void * ) const;

    template <typename T>
    static const std::string &typeToString();

    template <typename T>
    static Type create();

    FWTOOLS_API static Type create(std::string name);

protected :

    std::string m_name;
    unsigned char m_sizeof;
    bool m_isSigned;
    bool m_isFixedPrecision;
    ::boost::any m_min;
    ::boost::any m_max;

    SPTR(ToolBase) m_tool;

    /// Value for not specified type

    FWTOOLS_API static const TypeMapType s_TYPEMAP;

public :

    FWTOOLS_API static const Type s_UNSPECIFIED_TYPE;

    FWTOOLS_API static const Type s_INT8;
    FWTOOLS_API static const Type s_INT16;
    FWTOOLS_API static const Type s_INT32;
    FWTOOLS_API static const Type s_INT64;

    FWTOOLS_API static const Type s_UINT8;
    FWTOOLS_API static const Type s_UINT16;
    FWTOOLS_API static const Type s_UINT32;
    FWTOOLS_API static const Type s_UINT64;

    FWTOOLS_API static const Type s_FLOAT;
    FWTOOLS_API static const Type s_DOUBLE;

    FWTOOLS_API static const std::string s_UNSPECIFIED_TYPENAME;

    FWTOOLS_API static const std::string s_INT8_TYPENAME;
    FWTOOLS_API static const std::string s_INT16_TYPENAME;
    FWTOOLS_API static const std::string s_INT32_TYPENAME;
    FWTOOLS_API static const std::string s_INT64_TYPENAME;

    FWTOOLS_API static const std::string s_UINT8_TYPENAME;
    FWTOOLS_API static const std::string s_UINT16_TYPENAME;
    FWTOOLS_API static const std::string s_UINT32_TYPENAME;
    FWTOOLS_API static const std::string s_UINT64_TYPENAME;

    FWTOOLS_API static const std::string s_FLOAT_TYPENAME;
    FWTOOLS_API static const std::string s_DOUBLE_TYPENAME;

    typedef ::boost::int8_t   Int8Type;
    typedef ::boost::int16_t  Int16Type;
    typedef ::boost::int32_t  Int32Type;
    typedef ::boost::int64_t  Int64Type;

    typedef ::boost::uint8_t  UInt8Type;
    typedef ::boost::uint16_t UInt16Type;
    typedef ::boost::uint32_t UInt32Type;
    typedef ::boost::uint64_t UInt64Type;

    typedef float             FloatType;
    typedef double            DoubleType;


};

//-----------------------------------------------------------------------------

template< typename T >
std::string Type::Tool<T>::toString(::boost::any value) const
{
    return ::fwTools::getString( boost::any_cast<const T> (value));
}

//-----------------------------------------------------------------------------

template< typename T >
std::string Type::Tool<T>::toString(const void *value) const
{
    const T &v = *(static_cast< const T* > (value));
    return ::fwTools::getString( v );
}
//-----------------------------------------------------------------------------

template< typename T >
Type::Tool<T>::Tool() : Type::ToolBase(typeid(T))
{}


//-----------------------------------------------------------------------------

template <typename T>
Type Type::create()
{
    Type t;
    t.setType<T>();
    return t;
}

//-----------------------------------------------------------------------------

template <typename T>
bool Type::isOfType() const
{
    return *this == create<T>();
}

//-----------------------------------------------------------------------------

template <typename T>
void Type::setType()
{
    m_name = Type::typeToString<T>();
    m_sizeof = sizeof(T);
    m_isSigned = ::boost::is_signed<T>::value;
    m_isFixedPrecision = ::boost::is_integral<T>::value;

    m_tool = SPTR(ToolBase)(new Type::Tool<T>());

    T min = static_cast< T >( std::numeric_limits< T >::min() );
    T max = static_cast< T >( std::numeric_limits< T >::max() );

    if(!m_isFixedPrecision)
    {
        // std::numeric_limits::min() returns the smallest positive value for floating types
        min = max * -1;
    }

    m_min = min;
    m_max = max;
}

//-----------------------------------------------------------------------------

template <>
FWTOOLS_API void Type::setType< void >();


//-----------------------------------------------------------------------------

template <>
FWTOOLS_API void Type::setType< char >();

//-----------------------------------------------------------------------------

template <typename T>
const std::string &Type::typeToString()
{
    return Type::s_UNSPECIFIED_TYPENAME;
}

//-----------------------------------------------------------------------------

template <typename T>
const std::pair<T,T> Type::minMax() const
{
    return std::pair<T,T>(
            boost::any_cast< T >(m_min),
            boost::any_cast< T >(m_max)
            );
}


//-----------------------------------------------------------------------------

template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::Int8Type  > ();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::Int16Type > ();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::Int32Type > ();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::Int64Type > ();

template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::UInt8Type  >();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::UInt16Type >();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::UInt32Type >();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::UInt64Type >();

template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::FloatType  >  ();
template<> FWTOOLS_API const std::string &Type::typeToString< ::fwTools::Type::DoubleType >  ();

} //end namespace fwTools

namespace std
{
    FWTOOLS_API std::ostream& operator<< (std::ostream& os, const ::fwTools::Type& type);
} // namespace std


#endif /*_FWTOOLS_TYPE_H_*/

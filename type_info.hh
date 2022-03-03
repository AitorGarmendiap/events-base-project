#pragma once

#include <typeinfo>
#include <string>

namespace cs225
{
    class TypeInfo
    {
    public:
        template <typename T>
        TypeInfo(const T& object) : TypeInfo{typeid(object)}
        {}
        TypeInfo(const std::type_info& ti) : info{ti}
        {}
        const char* get_name()
        {
            return info.name();
        }
    private:
        const std::type_info& info;
    };

    bool operator==(const TypeInfo& a, const TypeInfo& b);
    bool operator!=(const TypeInfo& a, const TypeInfo& b);

    bool operator<(const TypeInfo& a, const TypeInfo& b);

    template <typename T>
    TypeInfo type_of(const T& object);
    template <typename T>
    TypeInfo type_of();
}
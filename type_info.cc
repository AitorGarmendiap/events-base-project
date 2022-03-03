#include "type_info.hh"

bool operator==(const TypeInfo& a, const TypeInfo& b)
{
    return a.get_name() == b.get_name();
}
bool operator!=(const TypeInfo& a, const TypeInfo& b)
{
     return !(a == b);
}

bool operator<(const TypeInfo& a, const TypeInfo& b)
{
    return a.get_name() < b.get_name();
}

template <typename T>
TypeInfo type_of(const T& object)
{
    return TypeInfo{object};
}
template <typename T>
TypeInfo type_of()
{
    return TypeInfo(typeid(T));
}
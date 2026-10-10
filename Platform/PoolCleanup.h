#pragma once

namespace Platform {
template<class Object>
inline void DeletePoolObject(Object* object)
{
    if (object) object->deleteInstance();
}
}

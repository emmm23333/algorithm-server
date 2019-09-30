#ifndef COMMON_H_
#define COMMON_H_
namespace json_data
{
    #define STRING_FROM_VECTOR(STR, VEC) STR.assign((VEC).begin(), (VEC).end())
    #define VECTOR_FROM_STRING(VEC, STR) VEC.resize((STR).size());VEC.assign((STR).begin(), (STR).end())
}
#endif
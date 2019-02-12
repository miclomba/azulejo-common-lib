#ifndef resourcetypetraits_hpp
#define resourcetypetraits_hpp

// Is an arithmetic stl container
template<typename T>
struct is_arithmetic_container { static const bool value = false; };
template<typename T, size_t N>
struct is_arithmetic_container<std::array<T, N>> { static const bool value = std::is_arithmetic_v<T>; };
template<typename T, typename Alloc>
struct is_arithmetic_container<std::vector<T, Alloc>> { static const bool value = std::is_arithmetic_v<T>; };

#endif // end resourcetypetraits_hpp
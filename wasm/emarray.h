#include <emscripten/bind.h>
#include <vector>

// auto bind std::vector<T> as JS list
// modified from https://github.com/emscripten-core/emscripten/issues/11070#issuecomment-717675128
namespace emscripten {
namespace internal {

template <typename T, typename Allocator>
struct BindingType<std::vector<T, Allocator>> {
  using ValBinding = BindingType<val>;
  using WireType = ValBinding::WireType;

  static WireType toWireType(const std::vector<T, Allocator> &vec, rvp::default_tag tag) {
	  std::vector<val> valVec(vec.begin(), vec.end());
    return BindingType<val>::toWireType(val::array(valVec), tag);
  }

  static std::vector<T, Allocator> fromWireType(WireType value) {
    return vecFromJSArray<T>(ValBinding::fromWireType(value));
  }
};

template <typename T>
struct TypeID<
    T, typename std::enable_if_t<std::is_same<
        typename Canonicalized<T>::type,
        std::vector<typename Canonicalized<T>::type::value_type,
                    typename Canonicalized<T>::type::allocator_type>>::value>> {
  static constexpr TYPEID get() { return TypeID<val>::get(); }
};

}  // namespace internal
}  // namespace emscripten

// helper function to bind std::array type

template <class T>
void DeclareArrayImpl(T&& a, std::index_sequence<>) {}

template <class T, std::size_t I, std::size_t... Is>
void DeclareArrayImpl(T&& a, std::index_sequence<I, Is...>) {
  DeclareArrayImpl(a.element(emscripten::index<I>()), std::index_sequence<Is...>{});
}

template <class ArrType>
void DeclareArray(const char* name) {
  constexpr size_t N = ArrType().size();
  DeclareArrayImpl(emscripten::value_array<ArrType>(name), std::make_index_sequence<N>{});
}

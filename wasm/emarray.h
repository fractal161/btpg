#include <emscripten/bind.h>

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
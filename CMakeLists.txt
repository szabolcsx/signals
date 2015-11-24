cmake_minimum_required(VERSION 3.0)
project(signal)

set(SOURCE_FILES main.cpp)

add_executable(signal ${SOURCE_FILES})

target_compile_features(signal PUBLIC cxx_auto_type)
target_compile_features(signal PUBLIC cxx_constexpr)
target_compile_features(signal PUBLIC cxx_decltype)
target_compile_features(signal PUBLIC cxx_generic_lambdas)
target_compile_features(signal PUBLIC cxx_lambdas)
target_compile_features(signal PUBLIC cxx_range_for)
target_compile_features(signal PUBLIC cxx_static_assert)
target_compile_features(signal PUBLIC cxx_variadic_templates)

//
//  types.hpp
//
//  Created by Anton Leuski on 8/5/14.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef __jerome_scripting_javascript_types_hpp__
#define __jerome_scripting_javascript_types_hpp__

#include <string>
#include <functional>
#include <jerome/type/FunctionSignature.hpp>

#define AL_JS_DEBUG

namespace jerome { namespace javascript {
	// assume UTF8 encoding
	typedef std::string		String;
	
	struct Value;
	struct Class;
	struct ContextGroup;
	struct Context;

	template <typename T> struct ClassTraits;

	namespace detail {
		
		template <typename T, class Enable = void>
		struct from_valueRef;
		
		template <typename T, class Enable = void>
		struct to_value;
		
		struct JSString;
		
		template <typename, typename>
		struct AbstractValue;
		
		
		template <typename T>
		struct is_static_object_pointer : public std::integral_constant<bool,
		std::is_pointer<T>::value && !is_static_function_pointer<T>::value
		> {};
		
		template <typename, typename = void>
		struct member_class_type;
		
		template <typename T>
		struct member_class_type<T, typename std::enable_if<std::is_member_pointer<T>::value>::type> {
			typedef typename member_traits_impl<typename std::remove_cv<T>::type>::class_type type;
		};

		template <typename, typename = void>
		struct member_value_type;
		
		template <typename T>
		struct member_value_type<T, typename std::enable_if<std::is_member_pointer<T>::value>::type> {
			typedef typename member_traits_impl<typename std::remove_cv<T>::type>::value_type type;
		};

		template <typename>
		struct is_std_function : public std::false_type {};
		
		template <typename _Fp>
		struct is_std_function<std::function<_Fp>> : public std::true_type {};


		template <typename, typename = void>
		struct is_associative_container : std::false_type {};
		
		template <typename T>
		struct is_associative_container<T, typename std::enable_if<
			sizeof(typename T::mapped_type)!=0
		>::type> : std::true_type {};

		template <typename, typename = void>
		struct is_container : std::false_type {};
		
		template <typename T>
		struct is_container<T, typename std::enable_if<
			sizeof(typename T::value_type)!=0 &&
			sizeof(typename T::iterator)!=0
		>::type> : std::true_type {};

		template <typename T, typename FN, FN F>
		struct jsc_callback {
			T value = nullptr;
		};
		
		template <typename FN, FN F>
		struct jsc_callback<FN, FN, F> {
			FN value = F;
		};
		
		struct __has_callback_tester_base {
			typedef char one;
			typedef long two;
		};
		
		template <typename T, typename FT, FT, typename = void>
		struct __is_static_member { static constexpr T value = nullptr;  };

//		template <typename T, typename C, typename R, typename ...Args, R (C::*F)(Args...)>
//		struct __is_static_member<T, R (C::*)(Args...), F> {  static constexpr T value = nullptr; };

		template <typename T, typename FT, FT F>
		struct __is_static_member<T, FT, F, typename std::enable_if<std::is_convertible<FT, T>::value>::type>
		{ static constexpr T value = F;  };

#define AL_JS_CLASS_DECLARE_MEMBER_TESTER(F) \
		template <typename T> \
		class __has_##F : public __has_callback_tester_base \
		{ \
			template <typename C> static one test(decltype(&C::F) ) ; \
			template <typename C> static two test(...); \
		public: \
			enum { value = sizeof(test<T>(0)) == sizeof(char) }; \
		}; \

		
	}

	// JS makes a distinction between null and undefined. We map nullptr to undefined, we need something to map to null
	static const struct Null_t {
		static const Null_t&	instance() {
      // STATIC
			static auto shared = new Null_t;
			return *shared;
		}
	private:
		Null_t() {}
	}& Null(Null_t::instance());
}}


#endif // defined __jerome_scripting_javascript_types_hpp__

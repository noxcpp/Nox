﻿///	@file	algorithm.h
///	@brief	アルゴリズム群
#pragma once

#include	<xutility>
#include	<functional>
#include	"type_traits/type_traits.h"
#include	"type_traits/concepts.h"
#include	"type_traits/function_signature.h"

#include	"assertion.h"

namespace nox::concepts::detail
{
	template <typename T>
	concept Deletable = requires(T ptr) {
		{ delete ptr };
	};
}

/// @brief ユーティリティ
namespace nox::util
{
#pragma region STL拡張
	template<std::ranges::range T, class FuncType>
		requires(std::is_invocable_r_v<bool, FuncType, typename T::value_type>)
	inline std::add_pointer_t<typename T::value_type> FindIf(const T& range, const FuncType func) noexcept(IsFunctionNoexceptValue<FuncType>)
	{
		decltype(auto) result = std::ranges::find_if(range, func);
		if (result == std::end(range))
		{
			return nullptr;
		}
		else
		{
			return &*result;
		}
	}

	/// @brief 範囲の中から、指定された条件を満たす最初の要素を検索する
	/// @tparam FuncType 関数の型
	/// @tparam T 範囲の型
	/// @param begin 範囲の先頭を指すイテレータ
	/// @param last 範囲の末尾の次を指すイテレータ
	/// @param func 比較関数
	/// @return 見つかった場合はnullptrを返す
	template<nox::concepts::Pointer T, class FuncType> requires(std::is_invocable_r_v<bool, FuncType, std::remove_pointer_t<T>>)
		inline constexpr T FindIf(const T begin, const T last, const FuncType func) noexcept(IsFunctionNoexceptValue<FuncType>)

	{
		decltype(auto) result = std::ranges::find_if(begin, last, func);
		if (result == last)
		{
			return nullptr;
		}
		else
		{
			return &*result;
		}
		
	}

	template<class MapContainer, class ReturnType>
	inline decltype(auto) Find(MapContainer&& container, typename MapContainer::key_type&& key, const ReturnType(*process)(typename MapContainer::value_type&&))
	{
		auto it = container.find(key);
		if (it == container.end())
		{
			return nullptr;
		}

		return process(it->second);
	}

	/// @brief 範囲の中から、指定された条件を満たす最初の要素を検索する
	template <std::ranges::forward_range _Rng, class _Ty, class _Pj = std::identity>
		requires std::permutable<std::ranges::iterator_t<_Rng>>
	&& std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<_Rng>, _Pj>, const _Ty*>
		inline constexpr std::ranges::iterator_t<_Rng> RemoveErase(
			_Rng&& _Range, const _Ty& _Val, _Pj = {}) 
	{
		const auto result = std::ranges::remove(std::forward<_Rng>(_Range), _Val);
		return _Range.erase(result.begin(), _Range.end());
	}

	/// <summary>
	/// 範囲の中から、指定された条件を満たす最初の要素を検索する
	/// </summary>
	/// <typeparam name="_Pj"></typeparam>
	/// <typeparam name="_Rng"></typeparam>
	/// <typeparam name="_Pr"></typeparam>
	/// <param name="_Range"></param>
	/// <param name="_Pred"></param>
	/// <param name=""></param>
	/// <returns></returns>
	template <std::ranges::forward_range _Rng, class _Pj = std::identity,
		std::indirect_unary_predicate<std::projected<std::ranges::iterator_t<_Rng>, _Pj>> _Pr>
		requires std::permutable<std::ranges::iterator_t<_Rng>>
	inline constexpr std::ranges::iterator_t<_Rng> RemoveEraseIf(
		_Rng&& _Range, _Pr _Pred, _Pj = {})
	{
		const auto result = std::ranges::remove_if(std::forward<_Rng>(_Range), _Pred);
		return _Range.erase(result.begin(), _Range.end());
	}
#pragma endregion

#pragma region 配列に対する操作
	namespace detail
	{
		template<concepts::Array T>
		[[nodiscard]] inline	constexpr size_t GetArrayExtentImpl(const size_t array_rank_index, const size_t indexCounter = 0)noexcept
		{
			if (array_rank_index == indexCounter)
			{
				return std::extent_v<T>;
			}
			else
			{
				if (indexCounter >= std::rank_v<T>)
				{
					return 0U;
				}
				if constexpr (std::is_array_v<std::remove_extent_t<T>> == false)
				{
					return 0U;
				}
				else
				{
					return util::detail::GetArrayExtentImpl<std::remove_extent_t<T>>(array_rank_index, indexCounter + 1);
				}
			}
		}
	}

	template<concepts::Array T>
	[[nodiscard]] inline constexpr size_t GetArrayExtent(const size_t array_rank_index)noexcept
	{
		if (array_rank_index >= std::rank_v<T>)
		{
			return 0U;
		}
		else
		{
			return util::detail::GetArrayExtentImpl<T>(array_rank_index);
		}
	}
#pragma endregion
	
#pragma region 文字列操作
	/// @brief 確保済みバッファに対しての文字列コピー
	/// @tparam CharType 
	/// @param source 
	/// @param dest_buffer 
	template <concepts::Char CharType>
	inline void StrCopy(std::basic_string_view<CharType> source, std::span<CharType> dest_buffer)
	{
		NOX_ASSERT(dest_buffer.size() >= source.size(), U"buffer size over");
		std::ranges::copy(source, dest_buffer.data());
		//	buffer[size] = std::char_traits<Char>::eof();	//	終端文字を格納

		//	終端文字を格納
		//	サイズがちょうどの場合は何もしない
		if (dest_buffer.size() < source.size())
		{
			dest_buffer[source.size()] = 0;	
		}
	}
#pragma endregion

//#if false
//	/// @brief 関数ポインタのアドレスをint64で取得する
//	/// @tparam T 
//	/// @param v 
//	/// @return 
//	template<nox::concepts::MemberFunctionPointer T>
//	inline nox::uint64 GetFunctionPointerID(const T v)
//	{
//		const void* const& p = reinterpret_cast<const void* const&>(v);
//		return reinterpret_cast<nox::uint64>(p);
//	}
//
//	/// @brief 関数ポインタのアドレスをint64で取得する
//	/// @tparam T 
//	/// @param v 
//	/// @return 
//	template<typename T> requires(std::is_function_v<T> || nox::concepts::GlobalFunctionPointer<T>)
//	inline nox::uint64 GetFunctionPointerID(const T v)
//	{
//		return reinterpret_cast<nox::uint64>(v);
//	}
//
//	/// @brief	
//	/// @tparam T 
//	/// @param v 
//	/// @return 
//	template<nox::concepts::MemberObjectPointer T>
//	inline nox::uint64 GetObjectPointerID(const T v)
//	{
//		const void* const& p = reinterpret_cast<const void* const&>(v);
//		return reinterpret_cast<nox::uint64>(p);
//	}
//
//	/// @brief 
//	/// @tparam T 
//	/// @param v 
//	/// @return 
//	template<nox::concepts::Pointer T>
//	inline nox::uint64 GetObjectPointerID(const T v)
//	{
//		return reinterpret_cast<nox::uint64>(v);
//	}
//#endif

	template<class T>
	inline constexpr bool IsNullPointer(T&& v)noexcept
	{
		if constexpr (std::is_pointer_v<T> == true)
		{
			return v == nullptr;
		}
		else
		{
			return false;
		}
	}

	template<class T>
	inline constexpr decltype(auto) At(T&& container, size_t length, size_t index)noexcept(false)
	{
		NOX_ASSERT(index < length, nox::assertion::RuntimeAssertErrorType::OutOfRange, U"index over");
		return container[index];
	}

	/// @brief 要素アクセス
	/// @tparam T 
	/// @param container 
	/// @param index 
	/// @return 
	template<class T> requires(std::is_invocable_v<decltype(&T::size), T>)
	inline constexpr decltype(auto) At(T&& container, size_t index)noexcept(false)
	{
		return At(std::forward<T>(container), std::size(container), index);
	}


	/// @brief 要素数の範囲外チェック
	/// @tparam T 型
	/// @param container コンテナ
	/// @param index 要素インデックス
	/// @return 範囲外かどうか
	template<class T> requires(std::is_array_v<T> || std::is_invocable_v<decltype(&T::size), T>)
		[[nodiscard]]	inline	constexpr	bool	IsValidIndex(const T& container, const size_t index)noexcept
	{
		return index < std::size(container);
	}

	/// @brief 列挙型Tの値を基底型に変換する
	/// @tparam T 列挙型
	/// @param value 列挙型の値
	/// @return 基底型の値
	template<concepts::Enum T>
	[[nodiscard]] inline	constexpr	std::underlying_type_t<T> ToUnderlying(const T value)noexcept
	{
#if defined(__clang__)
		return static_cast<std::underlying_type_t<T>>(value);
#else
		return std::to_underlying(value);
#endif
	}

#pragma region bit操作
	template<concepts::Enum T>
	inline	constexpr T BitOr(const std::initializer_list<T> lst)noexcept
	{
		std::underlying_type_t<T> ret = static_cast<std::underlying_type_t<T>>(0); //ValueTraits<std::underlying_type_t<T>>::VALUE0();
		for (typename std::initializer_list<T>::iterator it = lst.begin(); it != lst.end(); ++it)
		{
			ret |= util::ToUnderlying(*it);
		}
		return static_cast<T>(ret);
	}

	template<concepts::ClassOrUnion T>
	[[nodiscard]] inline constexpr T BitOr(const T& a, const T& b)noexcept { return std::bit_or<T>()(a, b); }

	template<concepts::Enum T>
	[[nodiscard]] inline constexpr T BitOr(const T a, const T b)noexcept { return static_cast<T>(std::bit_or<std::underlying_type_t<T>>()(ToUnderlying(a), ToUnderlying(b))); }

	template<std::integral T>
	[[nodiscard]] inline constexpr T BitOr(const T a, const T b)noexcept { return std::bit_or<T>()(a, b); }

	template<nox::concepts::Enum T, nox::concepts::Enum... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitOr(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitOr(e, nox::util::BitOr(rest...));
	}

	template<std::integral T, std::integral... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitOr(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitOr(e, nox::util::BitOr(rest...));
	}

	template<concepts::ClassOrUnion T>
	[[nodiscard]] inline	constexpr	bool IsBitAnd(const T& a, const T& b)noexcept { return std::bit_and<T>()(a, b); }

	template<concepts::Enum T>
	[[nodiscard]] inline	constexpr	bool IsBitAnd(const T a, const T b)noexcept { return std::bit_and<std::underlying_type_t<T>>()(ToUnderlying(a), ToUnderlying(b)); }

	template<std::integral T>
	[[nodiscard]] inline	constexpr	bool IsBitAnd(const T a, const T b)noexcept { return std::bit_and<T>()(a, b); }

	template<concepts::ClassOrUnion T>
	[[nodiscard]] inline	constexpr	T BitAnd(const T& a, const T& b)noexcept { return a & b; }

	template<concepts::Enum T>
	[[nodiscard]] inline	constexpr	T BitAnd(const T a, const T b)noexcept { return static_cast<T>(ToUnderlying(a) & ToUnderlying(b)); }

	template<std::integral T>
	[[nodiscard]] inline	constexpr	T BitAnd(const T a, const T b)noexcept { return a & b; }

	template<nox::concepts::Enum T, nox::concepts::Enum... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitAnd(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitAnd(e, nox::util::BitAnd(rest...));
	}

	template<std::integral T, std::integral... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitAnd(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitAnd(e, nox::util::BitAnd(rest...));
	}

	template<concepts::ClassOrUnion T>
	[[nodiscard]] inline constexpr T BitXor(const T& a, const T& b)noexcept { return std::bit_xor<T>()(a, b); }

	template<std::integral T>
	[[nodiscard]] inline constexpr T BitXor(const T a, const T b)noexcept { return std::bit_xor<T>()(a, b); }

	template<concepts::Enum T>
	[[nodiscard]] inline constexpr T BitXor(const T a, const T b)noexcept { return static_cast<T>(BitXor(ToUnderlying(a), ToUnderlying(b))); }

	template<nox::concepts::Enum T, nox::concepts::Enum... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitXor(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitXor(e, nox::util::BitXor(rest...));
	}

	template<std::integral T, std::integral... Rest>
		requires(sizeof...(Rest) > 1 && std::is_same_v<T, std::tuple_element_t<0, std::tuple<Rest...>>>)
	[[nodiscard]] inline constexpr T BitXor(const T e, const Rest... rest)noexcept
	{
		return nox::util::BitXor(e, nox::util::BitXor(rest...));
	}

	template<std::integral T>
	[[nodiscard]] inline constexpr T BitNot(const T v)noexcept { return std::bit_not<T>()(v); }
	
	template<concepts::Enum T>
	[[nodiscard]] inline constexpr T BitNot(const T v)noexcept { return static_cast<T>(BitNot<std::underlying_type_t<T>>(ToUnderlying(v))); }
#pragma endregion

	

	/// @brief delete呼び出し後nullptrを格納する
	/// @tparam T ポインタの型
	/// @param ptr delete対象のポインタ
	template<nox::concepts::detail::Deletable T>
	inline constexpr void SafeDelete(T&& ptr)
	{
		if (ptr != nullptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	/// @brief 
	/// @param ptr 
	/// @return 
	inline constexpr std::span<void(*)()> GetVTable(nox::not_null<const void*> ptr)
	{
		void (**vt)() = *(void (***)())ptr.get();
		nox::int32 counter = 0;
		while (vt[counter] != nullptr)
		{
			++counter;
		}

		return std::span(vt, counter);
	}

	template<nox::concepts::Class T>
	inline constexpr std::span<void(*)()> GetVTable(const T& ptr)
	{
		return nox::util::GetVTable(&ptr);
	}
}
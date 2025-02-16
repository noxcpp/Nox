﻿///	@file	log_trace.cpp
///	@brief	log_trace
#include	"stdafx.h"
#include	"log_trace.h"
#include	"algorithm.h"
#include	"memory/stl_allocate_adapter.h"
#include	"unicode_converter.h"
#include	"string_format.h"

#if NOX_WINDOWS
#include	"os/windows.h"
#endif // NOX_WIN64


#include	<iostream>

namespace
{
	inline constexpr nox::StringView GetLogCategoryName(nox::dev::LogCategory log_category)noexcept
	{
		constexpr std::array<nox::StringView, nox::util::ToUnderlying(nox::dev::LogCategory::_Max)> table =
		{
			U"Info",
			U"Warning",
			U"Error",
		};

		return table.at(nox::util::ToUnderlying(log_category));
	}

//	constinit std::array<nox::char32, 64> log_buffer_table 
}

void nox::dev::detail::TraceDirect(LogCategory log_category, const StringView category, const StringView message, bool isNewLine, const std::source_location& source_location)
{
	std::array<char16, 2048> buffer = { 0 };
	//source_location;
	if (isNewLine)
	{
		util::Format(buffer, u"[{0}][{1}]{2}\n", GetLogCategoryName(log_category), category.data(), message.data());
	}
	else
	{
		util::Format(buffer, u"[{0}][{1}]{2}", GetLogCategoryName(log_category), category.data(), message.data());
	}

	const wchar_t* converted_str = nox::util::CharCast<wchar_t>(buffer.data());

	//	コンソールへの出力
#if NOX_WINDOWS
	//	デバッグウィンドウに出力
	::OutputDebugStringW(converted_str);
#endif // NOX_WIN64

	std::wcout << converted_str ;
}
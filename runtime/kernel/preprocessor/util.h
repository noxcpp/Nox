﻿//	Copyright (C) 2023 NOX ENGINE All Rights Rserved.

///	@file	util.h
///	@brief	util
#pragma once
#include	"cat.h"

#if defined(__clang__)
/*
*@brief ユニークなローカル変数を定義します
*@note 関数内でのみ使用できます
*/
#define	NOX_LOCAL_SCOPE(Type, ...) static_assert(__PRETTY_FUNCTION__); const Type NOX_PP_CAT_I(__local, __LINE__)(__VA_ARGS__)
#define	NOX_LOCAL_SCOPE_C(Type, ...) static_assert(__PRETTY_FUNCTION__); constexpr Type NOX_PP_CAT_I(__local, __LINE__)(__VA_ARGS__)
#elif defined(_MSC_VER)
/*
*@brief ユニークなローカル変数を定義します
*@note 関数内でのみ使用できます
*/
#define	NOX_LOCAL_SCOPE(Type, ...) static_assert(__FUNCTION__); const Type NOX_PP_CAT_I(__local, __LINE__)(__VA_ARGS__)
#define	NOX_LOCAL_SCOPE_C(Type, ...) static_assert(__FUNCTION__); constexpr Type NOX_PP_CAT_I(__local, __LINE__)(__VA_ARGS__)

#endif
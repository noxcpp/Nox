﻿//	Copyright (C) 2024 NOX ENGINE All Rights Rserved.

///	@file	register.cpp
///	@brief	register
#include	"stdafx.h"
#include	"register.h"

#include	"gen.h"

void	nox::reflection::InitializeGen()
{
#if NOX_DEBUG
#if NOX_WIN64
//	nox::reflection::gen::Register_X64_Debug();
#endif // NOX_WIN64
#endif // NOX_DEBUG

#if NOX_RELEASE
#if NOX_WIN64
//	nox::reflection::gen::RegisterReleaseX64();
#endif // NOX_WIN64
#endif // NOX_RELEASE

}

void	nox::reflection::FinalizeGen()
{
#if NOX_DEBUG
#if NOX_WIN64
//	nox::reflection::gen::UnregisterDebugX64();
#endif // NOX_WIN64
#endif // NOX_DEBUG
}
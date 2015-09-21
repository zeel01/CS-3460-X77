
#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#else

#include <pthread.h>

#endif



#include <exception>
#include <system_error>



#include "thread.h"
#include "mutex.h"
#include "condition_variable.h"
#include "future.h"
#include "timer.h"

#include "details.h"

#include "thread.inl"
#include "mutex.inl"
#include "future.inl"

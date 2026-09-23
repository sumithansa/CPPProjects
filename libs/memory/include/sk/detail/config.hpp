#pragma once

// MSVC accepts [[no_unique_address]] but ignores it for ABI-compatibility
// reasons; the vendor-specific spelling actually applies the optimisation.
#if defined(_MSC_VER) && !defined(__clang__)
#define SK_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define SK_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

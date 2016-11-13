#pragma once

#include <memory>
#include <tuple>

#include "custom_types.h"

#define eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__)

#define exit_eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__); exit(EXIT_FAILURE)

std::unique_ptr<char> readFileToCharBuffer(const char* szFilename, size_t* pOutBufferLength);
u32 makeApiVersionNumber(u32 major, u32 minor, u32 patch);
std::tuple<u32, u32, u32> decomposeApiVersionNumber(u32 apiVersionNumber);

constexpr bool hasBisSet(u32 bitsetVal, u32 bitIndex) {
	return (bitsetVal >> bitIndex) & 1;
}

// Wrapper for calling get std container .size() as a forced uint32_t for Vulkan functions
template <class CONT>
constexpr u32 len32(const CONT& cont) {
	return static_cast<u32>(cont.size());
}

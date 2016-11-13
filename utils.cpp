#include "utils.h"

#include <fstream>
#include <iostream>


std::unique_ptr<char> readFileToCharBuffer(const char* szFilename, size_t* pOutBufferLength) {
	std::ifstream fileSource;
	fileSource.open(szFilename, std::ios::binary | std::ios::in | std::ios::ate);

	if (!fileSource.is_open()) {
		std::cout << "Failed to open source file: " << szFilename << std::endl;
		exit(EXIT_FAILURE);
	}

	*pOutBufferLength = static_cast<size_t>(fileSource.tellg());
	fileSource.seekg(0, std::ios::beg);

	std::unique_ptr<char> memblock(new char[*pOutBufferLength]);
	fileSource.read(memblock.get(), *pOutBufferLength);

	fileSource.close();

	return memblock;
}

u32 makeApiVersionNumber(u32 major, u32 minor, u32 patch) {
	return (major << 22u) | (minor << 12u) | patch;
}

constexpr u32 mask_22_31 = 0xFFC00000u;		// 0b11111111110000000000000000000000u
constexpr u32 mask_21_12 = 0x3FF000u;		// 0b00000000001111111111000000000000u
constexpr u32 mask_11_0 = 0xFFFu;			// 0b00000000000000000000111111111111u

std::tuple<u32, u32, u32> decomposeApiVersionNumber(u32 apiVersionNumber) {
	return std::make_tuple(
		(apiVersionNumber & mask_22_31) >> 22u,
		(apiVersionNumber & mask_21_12) >> 12u,
		(apiVersionNumber & mask_11_0)
	);
}

//constexpr u32 makeApiVersionNumber(u32 major, u32 minor, u32 patch) {	
//	return 
//		(major < 1024 && minor < 1024 && patch < 4096) 
//			? (major << 22u) | (minor << 12u) | patch
//			: throw std::logic_error("x must be > 0");
//}


#pragma once

#include <vector>
#include <assert.h>

#include "custom_types.h"

static constexpr u64 npos64 = ((u64)~0);

struct PagedAlignedFreeList {
	void Init(u64 pageSize, u64 memorySize) {
		mPageSize = pageSize;
		mMemorySize = memorySize;

		assert(memorySize % pageSize == 0);
		mPageCount = memorySize / pageSize;

		mFreePages.resize(mPageCount);

		for (u64 i = 0, pageOffset = 0; pageOffset < memorySize; ++i, pageOffset += pageSize) {
			mFreePages[i] = PageLocation{ pageOffset };
		}

		mRemainingFreeMemory = memorySize;
	}

	u64 GetContiguousFreeMemoryOffset(u64 contiguousSize) {
		assert(contiguousSize > 0);

		if (contiguousSize > mRemainingFreeMemory) { // Early exit
			return npos64;
		}

		u64 neededContiguousPageCount = (contiguousSize + mPageSize - 1) / mPageSize;

		u64 remainingFreePageCount = (u64)mFreePages.size();

		// TODO REMOVE, HACK BEFORE PROPER IMPLEMENTATIOn
		if (neededContiguousPageCount <= remainingFreePageCount) {
			u64 freeOffset = mFreePages[0].offset;

			// Remove pages
			mFreePages.erase(mFreePages.begin(), mFreePages.begin() + neededContiguousPageCount);

			return freeOffset;
		}
		else {
			return npos64;
		}

		for (u64 i = 0; i < remainingFreePageCount; ++i) {
			u64 currentContiguousCount = 1;

			while (currentContiguousCount < neededContiguousPageCount && (i + 1) < remainingFreePageCount) {
				if (mFreePages[i].offset + mPageSize == mFreePages[i + 1].offset) {
					++currentContiguousCount;
					++i;
				}
				else {
					++i;
					break;
				}
			}

			if (currentContiguousCount == neededContiguousPageCount) {
				u64 matchIdx = i - (neededContiguousPageCount - 1);
				u64 freeOffset = mFreePages[matchIdx].offset;

				// Remove pages
				mFreePages.erase(mFreePages.begin() + matchIdx, mFreePages.begin() + matchIdx + neededContiguousPageCount);

				return freeOffset;
			}
		}

		return npos64;
	}

	u64 mPageSize;
	u64 mMemorySize;

	u64 mPageCount;

	u64 mRemainingFreeMemory;

	struct PageLocation {
		u64 offset;
	};

	std::vector<PageLocation> mFreePages;
};

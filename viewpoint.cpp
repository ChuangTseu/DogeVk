#include "viewpoint.h"

//
//void ViewPoint::Bind() const
//{
//	PerViewPointCB& cb = SystemCBuffersManager::GetMutablePerViewPointCB();
//
//	DogeAssert(sizeof(*this) + /*pad*/ 4 == sizeof(cb));
//
//	std::copy_n(AS_U8_ADDRESSOF(*this), sizeof(*this), AS_U8_ADDRESSOF(cb));
//
//	SystemCBuffersManager::CommitPerViewPointCB();
//}

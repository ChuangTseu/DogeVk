#pragma once

#include <glm/glm.hpp>

#define MAX_NUM_VIEWPOINTPLANES 8 // Maybe cascades go up to that ?

#define CONCAT_MACRO_INTERNAL(x, y) x##y
#define CONCAT_MACRO(x, y) CONCAT_MACRO_INTERNAL(x, y)

#define CB_PAD_BYTE(n) char CONCAT_MACRO(_pad_, __COUNTER__)[n];
#define CB_PAD_FLOAT(n) float CONCAT_MACRO(_pad_, __COUNTER__)[n];
#define CB_PAD_VEC4(n) vec4 CONCAT_MACRO(_pad_, __COUNTER__)[n];

struct PerFrameCB {
	glm::vec3 g_randRgbColor;
	float m_time;
};

struct PerViewPointCB {
	glm::mat4 g_view[MAX_NUM_VIEWPOINTPLANES];
	glm::mat4 g_proj[MAX_NUM_VIEWPOINTPLANES];
	glm::mat4 g_viewProj[MAX_NUM_VIEWPOINTPLANES];

	glm::vec3 eyePosition;
	CB_PAD_FLOAT(1);
};

#define MAX_MODEL_WORLDS 4

struct PerObjectCB {
	glm::mat4 g_world[MAX_MODEL_WORLDS];
};

struct PerMaterialCB {
	glm::vec3 ka;
	CB_PAD_FLOAT(1);
	glm::vec3 kd;
	CB_PAD_FLOAT(1);
	glm::vec3 ks;
	float shininess;
};

struct PerFboCB {
	glm::vec4 FboDimensions[16];
};

#define MAX_SHADER_LIGHTS 16

struct DirLightCBStruct
{
	glm::vec3 m_direction;
	CB_PAD_FLOAT(1);
	glm::vec3 m_color;
	CB_PAD_FLOAT(1);
};

struct LightsCB {
	DirLightCBStruct dirLights[16];
	int numDirLights;
	CB_PAD_FLOAT(3);
};

struct AppConfigCB {
	glm::vec3 g_globalColor;
	CB_PAD_FLOAT(1);
};

#include "doge_vulkan.h"
#include "vulkan_helpers.h"
#include "dedicated_buffer.h"

#include <tuple>
#include <array>

template <class TUPLE, size_t... Is>
constexpr std::array<size_t, std::tuple_size<TUPLE>::value> arrTupleSizes_impl(std::index_sequence<Is...>) {
	return std::array<size_t, std::tuple_size<TUPLE>::value>{
		sizeof(std::tuple_element<Is, TUPLE>::type)...
	};
}

template <class TUPLE>
constexpr std::array<size_t, std::tuple_size<TUPLE>::value> arrTupleSizes() {
	return arrTupleSizes_impl<TUPLE>(std::make_index_sequence<std::tuple_size<TUPLE>::value>());
}

template <class T, class Tuple>
struct TupleTypeIndex;

template <class T, class... Types>
struct TupleTypeIndex<T, std::tuple<T, Types...>> {
	static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct TupleTypeIndex<T, std::tuple<U, Types...>> {
	static const std::size_t value = 1 + TupleTypeIndex<T, std::tuple<Types...>>::value;
};

class GlobalDescriptorSets
{
public:
	// A bit of template trickery to automatize the setup of all the Constant Buffers
	typedef std::tuple<
		PerFrameCB, 
		PerViewPointCB, 
		PerObjectCB, 
		PerMaterialCB,
		PerFboCB,
		LightsCB,
		AppConfigCB
	> UsedCStructs;

	//enum ECBuffers {
	//	kPerFrameCB,
	//	kPerViewPointCB,
	//	kPerObjectCB,
	//	kPerMaterialCB,
	//	kPerFboCB,
	//	kLightsCB,
	//	kAppConfigCB
	//};

	static UsedCStructs usedCStructs;

	template <class CB>
	static CB& GetMutableCBuffer() {
		return std::get<CB>(usedCStructs);
	}

	template <class CB>
	static void CommitCBuffer() {
		constexpr size_t idx = TupleTypeIndex<CB, UsedCStructs>::value;

		uniformBuffers[idx].UploadData(
			VkDeviceSize{ 0 },
			VkDeviceSize{ USED_CSTRUCT_SIZES[idx] },
			&std::get<idx>(usedCStructs));
	}

	//template <ECBuffers Index>
	//static typename std::tuple_element<Index, UsedCStructs>::type& GetMutableCBuffer() {
	//	return std::get<Index>(usedCStructs);
	//}

	//template <ECBuffers Index>
	//static void CommitCBuffer() {
	//	uniformBuffers[Index].UploadData(
	//		VkDeviceSize{ 0 }, 
	//		VkDeviceSize{ USED_CSTRUCT_SIZES[Index] },
	//		&std::get<Index>(usedCStructs));
	//}

	static constexpr u32 CB_COUNT = (u32)std::tuple_size<UsedCStructs>::value;
	static constexpr std::array<size_t, CB_COUNT> USED_CSTRUCT_SIZES = arrTupleSizes<UsedCStructs>();

	static VkDescriptorSetLayoutBinding descripterSetLayoutBindings[CB_COUNT];
	static VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfos[CB_COUNT];
	static VkDescriptorSetLayout descriptorSetLayouts[CB_COUNT];

	static VkDescriptorPool descriptorPool;

	static VkDescriptorSet descriptorSets[CB_COUNT];

	static DedicatedBuffer uniformBuffers[CB_COUNT];	

	static void Initialize();

public:
	/*static PerFrameCB m_perFrameCb;
	static GLuint m_perFrameCbId;
	static PerViewPointCB m_perViewPointCb;
	static GLuint m_perViewPointId;
	static PerObjectCB m_perObjectCb;
	static GLuint m_perObjectCbId;
	static PerLightCB m_perLightCb;
	static GLuint m_perLightCbId;
	static PerMaterialCB m_perMaterialCb;
	static GLuint m_perMaterialCbId;
	static PerFboCB m_perFboCb;
	static GLuint m_perFboCbId;
	static LightsCB m_lightsCb;
	static GLuint m_lightsCbId;
	static UIConfigCB m_UIConfigCb;
	static GLuint m_UIConfigCbId;

public:
	static void CommitPerFrameCB();
	static void CommitPerViewPointCB();
	static void CommitPerObjectCB();
	static void CommitPerLightCB();
	static void CommitPerMaterialCB();
	static void CommitPerFboCB();
	static void CommitLightsCB();
	static void CommitUIConfigCB();

	static PerFrameCB& GetMutablePerFrameCB() { return m_perFrameCb; }
	static PerViewPointCB& GetMutablePerViewPointCB() { return m_perViewPointCb; }
	static PerObjectCB& GetMutablePerObjectCB() { return m_perObjectCb; }
	static PerLightCB& GetMutablePerLightCB() { return m_perLightCb; }
	static PerMaterialCB& GetMutablePerMaterialCB() { return m_perMaterialCb; }
	static PerFboCB& GetMutablePerFboCB() { return m_perFboCb; }
	static LightsCB& GetMutableLightsCB() { return m_lightsCb; }
	static UIConfigCB& GetMutableUIConfigCB() { return m_UIConfigCb; }*/
};

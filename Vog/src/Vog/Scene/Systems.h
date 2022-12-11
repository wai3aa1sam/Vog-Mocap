#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Math/MyMath.h"

#include "Components.h"

#include <memory>

namespace vog {

	class Scene;
	class PhysicsWorld;

	namespace Test { class PhysicsEngine; }		// TODO: remove

	struct TransformSystem
	{
		static void update(Scene* pScene_);
	};

	struct TransformHierarchySystem
	{
		static void update(Scene* pScene_);
	};

	struct MeshRendererSystem
	{
		static void uploadLights(Scene* pScene_);
		static void update(Scene* pScene_);
	};

	struct AnimationSystem
	{
		static void update(float timestep_, Scene* pScene_);
	};

	struct BoneTransformSystem		// TODO: remove
	{
		static void update(Scene* pScene_);
	};

	struct PhysicsSystem
	{
		static void onStart(Scene* pScene_);

		static void uploadData(Scene* pScene_);

		static void update(PhysicsWorld* pPhysicsWorld_, float timestep_, Scene* pScene_);
	};

	struct TestPhysicsSystem
	{
		static void update(Test::PhysicsEngine* pPhysicsEngine_, float timestep_, Scene* pScene_);
	};

	struct TestRenderSystem
	{
		static void update(Scene* pScene_);
	};

	struct NativeScriptSystem
	{
		static void onAwake(Scene* pScene_);

		static void onStart(Scene* pScene_);

		static void update(Scene* pScene_, float timestep_);

		static void onDestroy(Scene* pScene_);
	};
}
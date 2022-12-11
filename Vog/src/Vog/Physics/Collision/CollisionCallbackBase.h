#pragma once

namespace vog {

	// TODO: temporary collision callback 
	// should have id / collision id to perform the trigger stay and exit and pass his pair to the function!
	// maybe change to listener pattern?
	class CollisionCallbackBase
	{
	public:
		CollisionCallbackBase() = default;
		virtual ~CollisionCallbackBase() = default;

		virtual void onTriggerEnter(uint32_t handle_) {};
		virtual void onTriggerStay(uint32_t handle_) {};
		virtual void onTriggerExit(uint32_t handle_) {};

		[[deprecated]]virtual void onCollisionEnter(/*const Collision& other_*/) {};
		[[deprecated]]virtual void onCollisionStay(/*const Collision& other_*/) {};
		[[deprecated]]virtual void onCollisionExit(/*const Collision& other_*/) {};
	};
}
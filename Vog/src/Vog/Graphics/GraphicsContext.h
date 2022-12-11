#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {

	class VOG_API GraphicsContext : public NonCopyable
	{
	public:
		GraphicsContext() = default;
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;
		virtual void makeContextCurrent() = 0;

		inline virtual void* getHandle() const = 0;

		virtual void enableVsync(bool isVsync_) = 0;
	};
}
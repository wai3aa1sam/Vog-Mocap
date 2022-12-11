#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Scene/Scene.h"
#include "Vog/Scene/Entity.h"

namespace vog {

	class VOG_API ScenePanel : public NonCopyable
	{
	public:
		ScenePanel() = default;
		ScenePanel(const RefPtr<Scene>& pScene_);
		~ScenePanel();

		void onImGuiRender();

		void setScene(const RefPtr<Scene>& pScene_);

		void setSelectedEntity(Entity entity_);
		Entity getSelectedEntity() const { return m_selectedEntity; }

		Entity createLight(Entity parentEntity_ = {}, const std::string& name_ = "Light");

		Entity createAABB(Entity parentEntity_ = {}, const std::string& name_ = "AABB");
		Entity createCapsule(Entity parentEntity_ = {}, const std::string& name_ = "Capsule");
		Entity createOBB(Entity parentEntity_ = {}, const std::string& name_ = "OBB");
		Entity createSphere(Entity parentEntity_ = {}, const std::string& name_ = "Sphere");
		Entity createPlane(Entity parentEntity_ = {}, const std::string& name_ = "Plane");
		Entity createTriangle(Entity parentEntity_ = {}, const std::string& name_ = "Triangle");

	private:
		void _drawEntity(Entity entity_);

		void _drawAddComponent();
		void _drawComponents(Entity entity_);

	private:
		RefPtr<Scene> m_pScene;
		Entity m_selectedEntity;
	};
}
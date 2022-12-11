#pragma once

#include <vector>
#include <queue>

// References: Game Physics Cookbook
// this class is not my work!
// use for reference only!

namespace vog::Test {

	struct QuadTreeData {
		void* object;
		//Rectangle2D bounds;
		bool flag;
		inline QuadTreeData(void* o) :
			object(o), flag(false) { }
	};


	class QuadTreeNode {
	protected:
		std::vector<QuadTreeNode> children;
		std::vector<QuadTreeData*> contents;
		int currentDepth;
		static constexpr int maxDepth = 5;
		static constexpr int maxObjectsPerNode = 15;
		//Rectangle2D nodeBounds;
	public:
		inline QuadTreeNode(/*const Rectangle2D& bounds*/) :
			/*nodeBounds(bounds),*/ currentDepth(0) { }
		bool IsLeaf();
		int NumObjects();
		void Insert(QuadTreeData& data);
		void Remove(QuadTreeData& data);
		void Update(QuadTreeData& data);
		void Shake();
		void Split();
		void Reset();
		std::vector<QuadTreeData*>Query(/*const Rectangle2D& area*/);
	};
	typedef QuadTreeNode QuadTree;

	class Scene
	{
		/*Most games will have some kind of a scene class
		 this scene class will have an initialize function */
	protected:
		QuadTree* quadTree;
	public:
		inline void Scene::Initialize() {
			//quadTree = new QuadTree( Rectangle2D(0, 0, sceneWidth, sceneHeight);
			struct CollisionData { };
			struct GameObject { };

			std::vector<QuadTreeData> colData;

			std::vector<GameObject> gameObjects;

			/* Usually the scene is read out of a text file
			or some resource on disk. From that resource,
			some game object array (or tree) is populated */
			for (int i = 0; i < gameObjects.size(); ++i) {
			//2. As we loop through each game object, insert all of them into the quad tree :
				//colData.push_back(QuadTreeData());
				QuadTreeData* collisionData = &colData[colData.size() - 1];
				//collisionData->object = gameObjects[i];
				//collisionData->bounds = gameObjects[i].bounds;
				//gameObjects[i]->cData = collisionData;
				quadTree->Insert(*collisionData);
			}
		}

		// 3. The update function for a scene might look something like this:
		void Update(float deltaTime) {
			struct GameObject {};
			//GameObject* player = FindObject("Player");
			//UpdatePlayerBasedOnInput(player);
			//quadTree->Update(player->cData);
			// 
			//4. Get a list of objects near the player :
			std::vector<QuadTreeData*>collisionObjects = quadTree->Query(/*player->cData->bounds*/);

			/* Loop trough the objects the player has
			collided with and perform actions or collision resolution */
		}
	};
}

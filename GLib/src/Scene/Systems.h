#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Renderer/Render.h"
#include "Physics/Bullet.h"
#include "btBulletDynamicsCommon.h"

namespace GLib {

    class Scene;
    class Entity;

    struct System {
        System() = default;
        ~System() = default;

        virtual void OnInit(Scene& scene) {}
        virtual void OnUpdate(Scene& scene, double dt) {}
        virtual void OnCleanup(Scene& scene) {}

        virtual int GetPriority() const { return 0; } // lower = earlier
    };

    struct RenderSystem : public System {
        //void OnInit(Scene& scene) override;

        void OnUpdate(Scene& scene, double dt) override;

        //void OnCleanup(Scene& scene) override;

        int GetPriority() const override {
            return INT_MAX - 100;
        }
    };

    struct PhysicsSystem : public System {
        void OnInit(Scene& scene) override {
            ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
            m_CollisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            m_OverlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            m_Solver = new btSequentialImpulseConstraintSolver;

            m_DebugDrawer = new BulletDebugDrawer(btIDebugDraw::DBG_DrawWireframe);

            m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_OverlappingPairCache, m_Solver, m_CollisionConfiguration);
            m_DynamicsWorld->setGravity(ToBullet(scene.GetGravity()));
            m_DynamicsWorld->setDebugDrawer(m_DebugDrawer);

            //keep track of the shapes, we release memory at exit.
            //make sure to re-use collision shapes among rigid bodies whenever possible!
            btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;

            scene.GetRegistry().on_construct<RigidBodyComponent>()
            .connect<&PhysicsSystem::InitBody>(this);
        }

        void OnUpdate(Scene& scene, double dt) override {
            auto view = scene.GetRegistry().view<RigidBodyComponent>();
            for (entt::entity entity : view){
                auto& rb = view.get<RigidBodyComponent>(entity);
                rb.SyncTransform();
            }

            m_DynamicsWorld->stepSimulation(dt, 10);
            m_DynamicsWorld->debugDrawWorld();

        }

        void OnCleanup(Scene& scene) override {
            auto view = scene.GetRegistry().view<RigidBodyComponent>();
            for (entt::entity entity : view) {
                auto& rigidBody = view.get<RigidBodyComponent>(entity);
                if (rigidBody.m_Body && rigidBody.m_Body->getMotionState())
                {
                    delete rigidBody.m_Body->getMotionState();
                }
                m_DynamicsWorld->removeRigidBody(rigidBody.m_Body);
                delete rigidBody.m_Body;
                rigidBody.m_Body = nullptr;
            }

            //delete collision shapes
            for (int j = 0; j < m_CollisionShapes.size(); j++)
            {
                btCollisionShape* shape = m_CollisionShapes[j];
                m_CollisionShapes[j] = 0;
                delete shape;
            }

            delete m_DynamicsWorld;
            delete m_Solver;
            delete m_OverlappingPairCache;
            delete m_Dispatcher;
            delete m_CollisionConfiguration;
        }

        int GetPriority() const override {
            return INT_MAX - 200;
        }

    private:
        btDefaultCollisionConfiguration* m_CollisionConfiguration = nullptr;
        btCollisionDispatcher* m_Dispatcher = nullptr;
        btBroadphaseInterface* m_OverlappingPairCache = nullptr;
        btSequentialImpulseConstraintSolver* m_Solver = nullptr;
        BulletDebugDrawer* m_DebugDrawer = nullptr;
        btDiscreteDynamicsWorld* m_DynamicsWorld = nullptr;

        btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
    private:
        void InitBody(entt::registry& registry, entt::entity entity) {//(RigidBodyComponent& rigidBody, TransformComponent& transform) {
            auto& rigidBody = registry.get<RigidBodyComponent>(entity);

            m_CollisionShapes.push_back(rigidBody.m_Shape);
            std::cout << "rigidbody created: " << rigidBody.m_Shape->getName() << "\n";

            rigidBody.m_Shape->setLocalScaling(ToBullet(registry.get<TransformComponent>(entity).Scale));

            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            btVector3 localInertia(0.0f, 0.0f, 0.0f);
            if (rigidBody.m_Mass != 0.f)
                rigidBody.m_Shape->calculateLocalInertia(rigidBody.m_Mass, localInertia);

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            BulletMotionState* motionState = new BulletMotionState(registry, entity);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(rigidBody.m_Mass, motionState, rigidBody.m_Shape, localInertia);
            rigidBody.m_Body = new btRigidBody(rbInfo);

            //add the body to the dynamics world
            m_DynamicsWorld->addRigidBody(rigidBody.m_Body);
        }
    };

}
# Dynamic Third-Person Camera (Master's Thesis) | WIP

Read more about the project [here](https://www.neesarg.me/third-person-camera-system).

### Quick links
* Camera System [directory](Engine/Code/Engine/CameraSystem/) 
	* Camera Manager [.hpp](Engine/Code/Engine/CameraSystem/CameraManager.hpp?ts=4) [.cpp](Engine/Code/Engine/CameraSystem/CameraManager.cpp?ts=4) 
	* Camera Behaviors [directory](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/), [abstract class](Engine/Code/Engine/CameraSystem/CameraBehaviour.hpp?ts=4) 
		* Degrees of Freedom [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_DegreesOfFreedom.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_DegreesOfFreedom.cpp?ts=4)
		* Follow [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_Follow.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_Follow.cpp?ts=4)
		* Freelook [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_FreeLook.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Behaviours/CB_FreeLook.cpp?ts=4)
	* Camera Constraints [directory](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/), [abstract class](Engine/Code/Engine/CameraSystem/CameraConstraint.hpp?ts=4)  
		* Modified Cone Raycast [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_ModifiedConeRaycast.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_ModifiedConeRaycast.cpp?ts=4)
		* Cone Raycast [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_ConeRaycast.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_ConeRaycast.cpp?ts=4)
		* Line-of-sight [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_LineOfSight.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_LineOfSight.cpp?ts=4)
		* Collision Check [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_CameraCollision.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Camera%20Constraints/CC_CameraCollision.cpp?ts=4)
	* Camera Motion Controllers [directory](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Motion%20Controllers/), [abstract class](Engine/Code/Engine/CameraSystem/CameraMotionController.hpp?ts=4) 
		* Proportional Controller [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Motion%20Controllers/CMC_ProportionalController.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Camera%20System/Motion%20Controllers/CMC_ProportionalController.cpp?ts=4)
* Game Scenes [directory](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Game%20States/) 
	* Collision Avoidance [.hpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Game%20States/Scene_CollisionAvoidance.hpp?ts=4) [.cpp](Thesis/Dynamic%20Third-Person%20Camera/Code/Game/Game%20States/Scene_CollisionAvoidance.cpp?ts=4) 
* Quaternion [.hpp](Engine/Code/Engine/Math/Quaternion.hpp?ts=4) [.cpp](Engine/Code/Engine/Math/Quaternion.cpp?ts=4) 
* Transform [.hpp](Engine/Code/Engine/Math/Transform.hpp?ts=4) [.cpp](Engine/Code/Engine/Math/Transform.cpp?ts=4)
* Game Object [.hpp](Engine/Code/Engine/Core/GameObject.hpp?ts=4) [.cpp](Engine/Code/Engine/Core/GameObject.cpp?ts=4) 

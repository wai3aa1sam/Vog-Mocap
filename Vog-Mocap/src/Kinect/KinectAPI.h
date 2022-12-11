#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog.h"

#ifdef VOG_ENABLE_KINECT
#include <KinectJointFilter.h>
#include <Kinect.h>
#include <stdint.h>
#endif // VOG_ENABLE_KINECT

// References: Microsoft Kinect v2 Native sample

namespace vog {

#ifdef VOG_ENABLE_KINECT

//#define enable_prev_data

	struct KinectData
	{
		bool isTrakced = false;
		Vector3f translation = { 0.0f, 0.0f, 0.0f };
		Quaternion rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
	};

	struct KinectDataRange
	{
		Vector3f min_translation = { 999999.9f, 999999.9f, 999999.9f };
		Vector3f max_translation = { -999999.9f, -999999.9f, -999999.9f };
	};

	static const std::unordered_map<std::string, _JointType> s_kinectBoneNameMap =
	{
		{"Spine_base",		JointType_SpineBase		},
		{"Spine_mid",		JointType_SpineMid		},
		{"Neck",			JointType_Neck			},
		{"Head",			JointType_Head			},
		{"Shoulder.L",		JointType_ShoulderLeft	},
		{"Elbow.L",			JointType_ElbowLeft		},
		{"Wrist.L",			JointType_WristLeft		},
		{"Hand.L",			JointType_HandLeft		},
		{"Shoulder.R",		JointType_ShoulderRight	},
		{"Elbow.R",			JointType_ElbowRight	},
		{"Wrist.R",			JointType_WristRight	},
		{"Hand.R",			JointType_HandRight		},
		{"Hip.L",			JointType_HipLeft		},
		{"Knee.L",			JointType_KneeLeft		},
		{"Ankle.L",			JointType_AnkleLeft		},
		{"Foot.L",			JointType_FootLeft		},
		{"Hip.R",			JointType_HipRight		},
		{"Knee.R",			JointType_KneeRight		},
		{"Ankle.R",			JointType_AnkleRight	},
		{"Foot.R",			JointType_FootRight		},
		{"Spine_shoulder",	JointType_SpineShoulder	},
		{"Hand_tip.L",		JointType_HandTipLeft	},
		{"Thumb.L",			JointType_ThumbLeft		},
		{"Hand_tip.R",		JointType_HandTipRight	},
		{"Thumb.R",			JointType_ThumbRight	},
	};

	static void toKinectBoneCoordinate(_JointType type_, Quaternion& quaternion_)
	{
		Quaternion result;
		if (type_ == JointType_SpineBase)
		{

		}
		else if (type_ == JointType_SpineMid)
		{

		}
		else if (type_ == JointType_Neck)
		{

		}
		else if (type_ == JointType_Head)
		{

		}
		else if (type_ == JointType_ShoulderLeft)
		{
			result.w = quaternion_.w;
			result.x = quaternion_.y;
			result.y = -quaternion_.x;
			result.z = quaternion_.z;
		}
		else if (type_ == JointType_Neck)
		{

		}
		else if (type_ == JointType_Neck)
		{

		}
	}

	static bool isKinectBoneName(const std::string& boneName_)
	{
		return s_kinectBoneNameMap.find(boneName_) != s_kinectBoneNameMap.end();
	}

	static const char* KinectJointTypeToString(_JointType type_)
	{
		switch (type_)
		{
		case JointType_SpineBase:			return "Spine_base";
		case JointType_SpineMid:			return "Spine_mid";
		case JointType_Neck:				return "Neck";
		case JointType_Head:				return "Head";
		case JointType_ShoulderLeft:		return "Shoulder.L";
		case JointType_ElbowLeft:			return "Elbow.L";
		case JointType_WristLeft:			return "Wrist.L";
		case JointType_HandLeft:			return "Hand.L";
		case JointType_ShoulderRight:		return "Shoulder.R";
		case JointType_ElbowRight:			return "Elbow.R";
		case JointType_WristRight:			return "Wrist.R";
		case JointType_HandRight:			return "Hand.R";
		case JointType_HipLeft:				return "Hip.L";
		case JointType_KneeLeft:			return "Knee.L";
		case JointType_AnkleLeft:			return "Ankle.L";
		case JointType_FootLeft:			return "Foot.L";
		case JointType_HipRight:			return "Hip.R";
		case JointType_KneeRight:			return "Knee.R";
		case JointType_AnkleRight:			return "Ankle.R";
		case JointType_FootRight:			return "Foot.R";
		case JointType_SpineShoulder:		return "Spine_shoulder";
		case JointType_HandTipLeft:			return "Hand_tip.L";
		case JointType_ThumbLeft:			return "Thumb.L";
		case JointType_HandTipRight:		return "Hand_tip.R";
		case JointType_ThumbRight:			return "Thumb.R";
		}

		return nullptr;
		//VOG_CORE_ASSERT(0, "Invalid joint type!");
	}

	static JointType toKinectJointType(int pinNum_)
	{
		switch (pinNum_)
		{
			case 22: return JointType_WristRight;
			case 24: return JointType_ElbowRight;
			case 26: return JointType_SpineBase;
			case 28: return JointType_Head;
			case 30: return JointType_ElbowLeft;
			case 32: return JointType_WristLeft;
		}
		VOG_CORE_ASSERT(0, "");
	}

	static bool isJointTypeSupported(JointType type_)
	{
		return type_ == JointType_Head
			|| type_ == JointType_SpineBase
			|| type_ == JointType_ElbowLeft
			|| type_ == JointType_WristLeft
			|| type_ == JointType_HandLeft
			|| type_ == JointType_HandRight
			|| type_ == JointType_ElbowRight
			|| type_ == JointType_WristRight;
	}

	

	class VOG_API KinectAPI : public NonCopyable
	{
	public:
		static const uint32_t bodyVerticesCount = (JointType::JointType_Count - 1) * 2;
		static const int        cDepthWidth = 512;
		static const int        cDepthHeight = 424;
	public:
		KinectAPI();
		~KinectAPI();

		void onUpdate();
		void onResize(float width_, float height_);
		void onImGuiRender();
		void drawSkeleton();

		void onUpdateBonesTransfom(RefPtr<Model>& pModel_);

		const std::array<Vector3f, bodyVerticesCount>& getBodyVertices() const { return m_joints; }
		const std::array<Vector3f, bodyVerticesCount>& getBodyScreenVertices() const { return m_jointScreen; }		// TODO: remove

		const std::array<KinectData, JointType::JointType_Count>& getJointUpdatedData() const { return m_jointUpdatedData; }
		const std::array<KinectData, JointType::JointType_Count>& getJointResetZeroData() const { return m_jointRestZeroData; }
		const std::array<KinectDataRange, JointType::JointType_Count>& getJointDataRanges() const { return m_jointDataRanges; }

		/*const Vector3f& getFinalJoint_Poisiton(JointType type_) const 
		{ 
			KinectDataRange range;
			float length = s_kinectMappingScales[type_];
			range.min_translation = { -length, -length, -length };
			range.max_translation = { length, length, length };

			return mapTranslationTo(getJointUpdatedData()[type_].translation,
			getJointResetZeroData()[JointType_HandRight].translation,
			getJointDataRanges()[JointType_HandRight], range); 
		}*/

		Vector3f getFinalData_Poisition(JointType type_) const {
			int index = type_;

			VOG_ASSERT(isJointTypeSupported(type_), "");

			float scale = m_kinectMappingScales[index];
			//VOG_LOG_INFO("scale: {0}", scale);

			KinectDataRange range;
			range.min_translation = { -scale, -scale, -scale };
			range.max_translation = { scale, scale, scale };


			Vector3f ret = KinectAPI::mapTranslationTo(getJointUpdatedData()[index].translation,
				getJointResetZeroData()[index].translation,
				getJointDataRanges()[index], range);

			return ret;
		}

		std::pair<Vector2f, Vector2f> getHandScreenPosition();
		std::pair<bool, bool> getIsHandClosed();

		Vector2f cameraToScreen(const CameraSpacePoint& bodyPoint_, int width_, int height_);

		static Vector3f mapTranslationTo(const Vector3f& src_translation_, const Vector3f& translation_setZero_, const KinectDataRange& src_ranges_, const KinectDataRange& target_ranges_);
	private:
		void _initKinectObject();

		void _getKinectBodyData(int nBodyCount_, IBody** ppBodies_);
		void _processBodyData(std::array<Vector3f, bodyVerticesCount>& joints_, const Joint* pJoints_);
		void _processBodyData(std::array<Vector3f, bodyVerticesCount>& joints_, const DirectX::XMVECTOR* pJoints_);

		void _initRendererObject();

		void _setHandIsClosed(bool& isHandClosed_, HandState handState_);


	private:
		IKinectSensor* m_pSensor = nullptr;
		IBodyFrameSource* m_pFrameSource = nullptr;
		IBody* m_ppBodies[BODY_COUNT] = { 0 };
		IBodyFrameReader* m_pFrameReader = nullptr;
		IBodyFrame* m_pBodyFrame = nullptr;
		ICoordinateMapper* m_pCoordinateMapper = nullptr;

		std::array<Vector3f, bodyVerticesCount> m_jointScreen;		// TODO: remove
		std::array<Vector3f, bodyVerticesCount> m_joints;

#ifdef enable_prev_data
		std::array<KinectData, JointType::JointType_Count> m_jointPrevData;
#endif // enable_prev_data

		std::array<KinectData, JointType::JointType_Count> m_jointUpdatedData;
		std::array<KinectData, JointType::JointType_Count> m_jointRestZeroData;

		std::array<KinectDataRange, JointType::JointType_Count> m_jointDataRanges;

		bool m_isResetZero = false;
		bool m_isCalibrating = false;

		bool m_isLeftHandClosed = false;
		bool m_isRightHandClosed = false;
		Vector2f m_leftHandScreenPosition = { 0.0f, 0.0f };
		Vector2f m_rightHandScreenPosition = { 0.0f, 0.0f };

		Vector2f m_viewportSize = {0.0f, 0.0f};

		Sample::FilterDoubleExponential m_filter;

		float m_transformScaleFactor = 30.0f;

		std::array<float, JointType::JointType_Count> m_kinectMappingScales =
		{
			1.0f, // JointType_SpineBase		
			1.0f, // JointType_SpineMid		
			1.0f, // JointType_Neck			
			1.0f, // JointType_Head			
			1.0f, // JointType_ShoulderLeft	
			1.0f, // JointType_ElbowLeft		
			1.0f, // JointType_WristLeft		
			8.0f, // JointType_HandLeft		
			1.0f, // JointType_ShoulderRight	
			1.0f, // JointType_ElbowRight	
			1.0f, // JointType_WristRight	
			8.0f, // JointType_HandRight		
			1.0f, // JointType_HipLeft		
			1.0f, // JointType_KneeLeft		
			1.0f, // JointType_AnkleLeft		
			1.0f, // JointType_FootLeft		
			1.0f, // JointType_HipRight		
			1.0f, // JointType_KneeRight		
			1.0f, // JointType_AnkleRight	
			1.0f, // JointType_FootRight		
			1.0f, // JointType_SpineShoulder	
			1.0f, // JointType_HandTipLeft	
			1.0f, // JointType_ThumbLeft		
			1.0f, // JointType_HandTipRight	
			1.0f, // JointType_ThumbRight	
		};

		//Renderer object
		RefPtr<VertexArray> m_pSkeletonVA;
		RefPtr<Shader> m_pVertexPosShader;
	};
#endif // 0


	
}
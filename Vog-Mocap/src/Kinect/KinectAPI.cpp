#include "KinectAPI.h"

#include "Vog/Math/MyMath.h"

#include "Vog/ImGui/ImGuiLibrary.h"

#include "../Serial/ArduinoSerial.h"

#include <ImGui/imgui.h>

namespace vog {

#ifdef VOG_ENABLE_KINECT
	static bool tracked = false;

	// Safe release for interfaces
	template<class Interface>
	inline void SafeRelease(Interface*& pInterfaceToRelease)
	{
		if (pInterfaceToRelease != NULL)
		{
			pInterfaceToRelease->Release();
			pInterfaceToRelease = NULL;
		}
	}

	KinectAPI::KinectAPI()
	{
		_initKinectObject();

		SafeRelease(m_pFrameSource);

		// skeleton render obj
		_initRendererObject();
	}

	KinectAPI::~KinectAPI()
	{
		SafeRelease(m_pFrameReader);

		SafeRelease(m_pCoordinateMapper);

		m_pSensor->Close();
		SafeRelease(m_pSensor);
	}

	void KinectAPI::_initKinectObject()
	{
		// kinext init
		HRESULT ret = GetDefaultKinectSensor(&m_pSensor);
		VOG_ASSERT(ret == S_OK, "Get Sensor failed");

		ret = m_pSensor->Open();
		VOG_ASSERT(ret == S_OK, "Can't open sensor");

		ret = m_pSensor->get_BodyFrameSource(&m_pFrameSource);
		VOG_ASSERT(ret == S_OK, "Can't get body frame source");

		ret = m_pFrameSource->OpenReader(&m_pFrameReader);
		VOG_ASSERT(ret == S_OK, "Can't get body frame reader");

		ret = m_pSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		VOG_ASSERT(ret == S_OK, "Can't get Coordinate Mapper");

		Sample::TRANSFORM_SMOOTH_PARAMETERS smoothingParams;

		smoothingParams.fSmoothing = 0.75f; // [0..1], lower values closer to raw data
		smoothingParams.fCorrection = 0.25f * 2; // [0..1], lower values slower to correct towards the raw data
		smoothingParams.fPrediction = 0.5f; // [0..n], the number of frames to predict into the future
		smoothingParams.fJitterRadius = 0.05f; // The radius in meters for jitter reduction
		smoothingParams.fMaxDeviationRadius = 0.05f; // The maximum radius in meters that filtered positions are allowed to deviate from raw data
		
		m_filter.Init(smoothingParams.fSmoothing, smoothingParams.fCorrection, smoothingParams.fPrediction, smoothingParams.fJitterRadius, smoothingParams.fMaxDeviationRadius);
	}

#pragma region toVector_func
	Vector3f toVector3f(const CameraSpacePoint& vec3_)
	{
		return Vector3f{ vec3_.X, vec3_.Y, vec3_.Z };
	}

	Vector3f toVector3f( const DirectX::XMVECTOR& v4_)
	{
		DirectX::XMFLOAT4 v2F;
		DirectX::XMStoreFloat4(&v2F, v4_);
		return Vector3f{ v2F.x, v2F.y, v2F.z };
	}

	Vector4f toVector4f(const Vector4& vec4_)
	{
		return Vector4f{ vec4_.x, vec4_.y, vec4_.z, vec4_.w };
	}

	void toVector3f(Vector3f& vec3_, const CameraSpacePoint& csp3_)
	{
		vec3_.x = csp3_.X;
		vec3_.y = csp3_.Y;
		vec3_.z = csp3_.Z;
	}
	
	void toVector3f(Vector3f& vec3_, const DirectX::XMVECTOR& v4_)
	{
		DirectX::XMFLOAT4 v2F;
		DirectX::XMStoreFloat4(&v2F, v4_);
		vec3_.x = v2F.x;
		vec3_.y = v2F.y;
		vec3_.z = v2F.z;
	}

	void toVector4f(Vector4f& vec4_, const Vector4& kVec4_)
	{
		vec4_.x = kVec4_.x;
		vec4_.y = kVec4_.y;
		vec4_.z = kVec4_.z;
		vec4_.w = kVec4_.w;
	}
#pragma endregion

	void KinectAPI::onUpdate()
	{
		if (m_pFrameReader->AcquireLatestFrame(&m_pBodyFrame) == S_OK)
		{
			if (m_pBodyFrame->GetAndRefreshBodyData(_countof(m_ppBodies), m_ppBodies) == S_OK)
			{
				_getKinectBodyData(BODY_COUNT, m_ppBodies);
			}

			for (int i = 0; i < _countof(m_ppBodies); ++i)
			{
				SafeRelease(m_ppBodies[i]);
			}

			m_pBodyFrame->Release();
		}
	}

	void KinectAPI::onResize(float width_, float height_)
	{
		m_viewportSize.x = width_;
		m_viewportSize.y = height_;
	}

	void KinectAPI::onImGuiRender()
	{
		ImGui::Begin("Kinect Setting");

		ImGuiLibrary::drawCheckbox("Reset Zero", m_isResetZero);
		ImGuiLibrary::drawCheckbox("Calibrate", m_isCalibrating);

		//m_isResetZero = getIsHandClosed().first;

		for (int i = 0; i < m_kinectMappingScales.size(); i++)
		{
			if (isJointTypeSupported((JointType)i))
			{
				float scale = m_kinectMappingScales[i];
				bool isChanged = ImGui::DragFloat(KinectJointTypeToString((JointType)i), &m_kinectMappingScales[i], 0.1f);
				if (isChanged)
				{
					VOG_LOG_INFO("{0}, scale = {1}", KinectJointTypeToString((JointType)i), m_kinectMappingScales[i]);
				}
			}
		}
		ImGui::End();
	}

	void KinectAPI::drawSkeleton()
	{
		//m_pVertexPosShader->bind();
		//m_pVertexPosShader->setMat4("u_viewProjection", camera_.getViewProjectionMatrix());
		Renderer::submitLines(m_pVertexPosShader, m_pSkeletonVA);
	}

	static void _updateBonesTransfom(AssimpNodeData& node_, RefPtr<Model>& pModel_, KinectAPI* pKinectAPI)
	{
		auto& boneInfoMap = pModel_->getBoneInfoMap();
		auto& nodeName = node_.name;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			JointType type_;
			if (auto it = s_kinectBoneNameMap.find(nodeName); it != s_kinectBoneNameMap.end())
				type_ = it->second;

			if (isKinectBoneName(nodeName) && isJointTypeSupported(type_))
			{
				uint32_t boneIndex = s_kinectBoneNameMap.at(nodeName);
				const KinectData& kinectData = pKinectAPI->getJointUpdatedData()[boneIndex];
				if (kinectData.isTrakced)
				{
					/*const KinectData& kinectResetZeroData = pKinectAPI->getJointResetZeroData()[boneIndex];
					const KinectDataRange& kinectRangeData = pKinectAPI->getJointDataRanges()[boneIndex];

					float length = s_kinectMappingScales[boneIndex];

					KinectDataRange range;
					range.min_translation = { -length, -length, -length };
					range.max_translation = { length, length, length };

					Vector3f ret = KinectAPI::mapTranslationTo(pKinectAPI->getJointUpdatedData()[boneIndex].translation,
						pKinectAPI->getJointResetZeroData()[boneIndex].translation,
						pKinectAPI->getJointDataRanges()[boneIndex], range);*/

					//node_.transform.translation = pKinectAPI->getFinalData_Poisition((JointType)boneIndex);

					if (type_ == JointType::JointType_Head)
					{
						node_.transform.rotation = ArduinoSerial::get().getFinalRotation(IMUJointType::Head);
					}

					//Vector3f ret = KinectAPI::mapTranslationTo(kinectData.translation, kinectResetZeroData.translation, kinectRangeData, range);

					//node_.transform.rotation = MyMath::toEulerAngles(kinectData.rotation);
					//node_.transform.rotation.x = kinectData.eulerAngle.x;
					//node_.transform.rotation.y = kinectData.eulerAngle.y;
					//node_.transform.rotation.z = kinectData.eulerAngle.z;

					node_.transform.updateTransformation();
				}
			}
		}

		for (uint32_t i = 0; i < node_.nChildren; i++)
		{
			_updateBonesTransfom(node_.children[i], pModel_, pKinectAPI);
		}
	}

	void KinectAPI::onUpdateBonesTransfom(RefPtr<Model>& pModel_)
	{
		_updateBonesTransfom(pModel_->getRootNode(), pModel_, this);
	}

	static float pitch(const Vector4& quaternion_)
	{
		float value1 = 2.0f * (quaternion_.w * quaternion_.x + quaternion_.y * quaternion_.z);
		float value2 = 1.0f - 2.0f * (quaternion_.x * quaternion_.x + quaternion_.y * quaternion_.y);
		float roll = glm::atan(value1, value2);
		return roll /** (180.0 / glm::pi<double>())*/;
	}

	static float yaw(const Vector4& quaternion_)
	{
		float value = 2.0f * (quaternion_.w * quaternion_.y - quaternion_.z * quaternion_.x);
		value = value > 1.0f ? 1.0f : value;
		value = value < -1.0f ? -1.0f : value;
		float pitch = glm::asin(value);
		return pitch /** (180.0 / glm::pi<double>())*/;
	}

	static float roll(const Vector4& quaternion_)
	{
		float value1 = 2.0f * (quaternion_.w * quaternion_.z + quaternion_.x * quaternion_.y);
		float value2 = 1.0f - 2.0f * (quaternion_.y * quaternion_.y + quaternion_.z * quaternion_.z);
		float yaw = glm::atan(value1, value2);
		return yaw /** (180.0 / glm::pi<double>())*/;
	}

	void KinectAPI::_getKinectBodyData(int nBodyCount_, IBody** ppBodies_)
	{
		for (int i = 0; i < nBodyCount_; ++i)
		{
			IBody* pBody = ppBodies_[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					Joint joints[JointType::JointType_Count];
					// get joint orientation
					JointOrientation jointOrientations[JointType::JointType_Count];
					VOG_ASSERT(pBody->GetJointOrientations(JointType::JointType_Count, jointOrientations) == S_OK, "Get joints fail");

					//D2D1_POINT_2F jointPoints[JointType_Count];
					HandState leftHandState = HandState_Unknown;
					HandState rightHandState = HandState_Unknown;

					pBody->get_HandLeftState(&leftHandState);
					pBody->get_HandRightState(&rightHandState);

					hr = pBody->GetJoints(_countof(joints), joints);
					if (SUCCEEDED(hr))
					{
						m_filter.Update(joints);
						_processBodyData(m_joints, m_filter.GetFilteredJoints());

#if screenBodyData
						for (int j = 0; j < _countof(joints); ++j)
						{
							joints[i].Position.X = cameraToScreen(joints[j].Position, 1280, 720).x;
							joints[i].Position.Y = cameraToScreen(joints[j].Position, 1280, 720).y;
							joints[i].Position.Z = 0.0f;
						}
						_processBodyData(m_jointScreen, joints);
#endif
						//m_pSkeletonVA->getVertexBuffer()->setData(m_joints.data(), sizeof(m_joints));
						//DrawBody(joints, jointPoints);
						//DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
						//DrawHand(rightHandState, jointPoints[JointType_HandRight]);

						HandState leftHandState = HandState_Unknown;
						HandState rightHandState = HandState_Unknown;

						pBody->get_HandLeftState(&leftHandState);
						pBody->get_HandRightState(&rightHandState);

						_setHandIsClosed(m_isLeftHandClosed, leftHandState);
						_setHandIsClosed(m_isRightHandClosed, rightHandState);

						//VOG_LOG_INFO("===Start===");

						// set kinect animation
						for (uint32_t i = 0; i < m_jointUpdatedData.size(); i++)
						{
							if (joints[i].TrackingState != TrackingState::TrackingState_Tracked)
							{
								m_jointUpdatedData[i].isTrakced = false;
								continue;
							}

							KinectData data;

							data.isTrakced = true;
							data.translation = Vector3f{ joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z };
							data.rotation = Quaternion{ jointOrientations[i].Orientation.w, jointOrientations[i].Orientation.x,
														jointOrientations[i].Orientation.y, jointOrientations[i].Orientation.z };

							m_jointUpdatedData[i] = data;

							//VOG_LOG_INFO(KinectJointTypeToString((JointType)i));
							//VOG_LOG_INFO("Translation: <{0}, {1}, {2}>", data.translation.x, data.translation.y, data.translation.z);
							//VOG_LOG_INFO("Rotation: <{0}, {1}, {2}, {3}>", data.rotation.w, data.rotation.x, data.rotation.y, data.rotation.z);
						}

						//VOG_LOG_INFO("===End===");

						if (m_isCalibrating)
						{
							// calculate the max and min range for translation for further mapping
							for (uint32_t i = 0; i < m_jointDataRanges.size(); i++)
							{
								if (joints[i].TrackingState != TrackingState::TrackingState_Tracked)
									continue;

								if (joints[i].Position.X < m_jointDataRanges[i].min_translation.x)
									m_jointDataRanges[i].min_translation.x = joints[i].Position.X;
								
								if (joints[i].Position.X > m_jointDataRanges[i].max_translation.x)
									m_jointDataRanges[i].max_translation.x = joints[i].Position.X;

								if (joints[i].Position.Y < m_jointDataRanges[i].min_translation.y)
									m_jointDataRanges[i].min_translation.y = joints[i].Position.Y;

								if (joints[i].Position.Y > m_jointDataRanges[i].max_translation.y)
									m_jointDataRanges[i].max_translation.y = joints[i].Position.Y;

								if (joints[i].Position.Z < m_jointDataRanges[i].min_translation.z)
									m_jointDataRanges[i].min_translation.z = joints[i].Position.Z;

								if (joints[i].Position.Z > m_jointDataRanges[i].max_translation.z)
									m_jointDataRanges[i].max_translation.z = joints[i].Position.Z;

								/*if (i == JointType_HandRight && getJointUpdatedData()[JointType_HandRight].isTrakced)
								{
									VOG_LOG_INFO("=====Start");
									VOG_LOG_INFO("Min Range: <{0}, {1}, {2}>", m_jointDataRanges[i].min_translation.x, m_jointDataRanges[i].min_translation.y, m_jointDataRanges[i].min_translation.z);
									VOG_LOG_INFO("Max Range: <{0}, {1}, {2}>", m_jointDataRanges[i].max_translation.x, m_jointDataRanges[i].max_translation.y, m_jointDataRanges[i].max_translation.z);
									VOG_LOG_INFO("=====End");
								}*/
							}
						}

						if (m_isResetZero)
						{
							for (uint32_t i = 0; i < m_jointUpdatedData.size(); i++)
							{
								if (joints[i].TrackingState != TrackingState::TrackingState_Tracked)
								{
									m_jointRestZeroData[i].isTrakced = false;
									continue;
								}
								KinectData data;

								data.translation = Vector3f{ joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z };
								data.rotation = Quaternion{ jointOrientations[i].Orientation.w, jointOrientations[i].Orientation.x,
															jointOrientations[i].Orientation.y, jointOrientations[i].Orientation.z };
								data.isTrakced = true;
								m_jointRestZeroData[i] = data;
							}

							// reset ranges, could use memset!
							for (uint32_t i = 0; i < m_jointDataRanges.size(); i++)
							{
								m_jointDataRanges[i].min_translation = { 999999.9f, 999999.9f, 999999.9f };
								m_jointDataRanges[i].max_translation = { -999999.9f, -999999.9f, -999999.9f };
							}

							m_isResetZero = false;
						}

						if (!tracked)
						{
#ifdef enable_prev_data
							// init kinect data
							for (uint32_t i = 0; i < m_jointPrevData.size(); i++)
							{
								if (joints[i].TrackingState == TrackingState::TrackingState_NotTracked)
								{
									m_jointPrevData[i].isTrakced = false;
									continue;
								}

								KinectData data;

								data.isTrakced = true;
								data.translation = Vector3f{ joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z };
								data.rotation = Quaternion{ jointOrientations[i].Orientation.w, jointOrientations[i].Orientation.x,
															jointOrientations[i].Orientation.y, jointOrientations[i].Orientation.z };

								m_jointPrevData[i] = data;
							}
#endif // enable_prev_data

#ifndef enable_prev_data
							VOG_LOG_INFO("tracked!, no prev data now!, uncomment to enable");
#else
							VOG_LOG_INFO("tracked!");
#endif // 1
							tracked = true;
						}

						//VOG_LOG_TRACE("Right Hand Position : <{0}, {1}, {2}>", joints[JointType_HandRight].Position.X, joints[JointType_HandRight].Position.Y, joints[JointType_HandRight].Position.Z);
						//VOG_LOG_TRACE("m_rightHandScreenPosition : <{0}, {1}>", m_rightHandScreenPosition.x, m_rightHandScreenPosition.y);

						m_leftHandScreenPosition = cameraToScreen(joints[JointType_HandLeft].Position, (int)m_viewportSize.x, (int)m_viewportSize.y);
						m_rightHandScreenPosition = cameraToScreen(joints[JointType_HandRight].Position, (int)m_viewportSize.x, (int)m_viewportSize.y);

						//VOG_LOG_TRACE("m_leftHandScreenPosition : <{0}, {1}>", m_leftHandScreenPosition.x, m_leftHandScreenPosition.y);
						//VOG_LOG_TRACE("m_rightHandScreenPosition : <{0}, {1}>", m_rightHandScreenPosition.x, m_rightHandScreenPosition.y);
					}
				}
			}
		}
	}

	void KinectAPI::_initRendererObject()
	{
		struct vertexLayout
		{
			Vector3f position;
		};
		auto pSkeltonVertexBuffer = VertexBuffer::create(nullptr, sizeof(m_joints));
		VertexLayout layout = {
			{ VertexDataType::Float3, "a_position",	},
		};
		pSkeltonVertexBuffer->setVertexLayout(layout);

		uint32_t indices[KinectAPI::bodyVerticesCount];
		for (int i = 0; i < KinectAPI::bodyVerticesCount; i++)
			indices[i] = i;
		auto pSkeltonIndexBuffer = IndexBuffer::create(indices, KinectAPI::bodyVerticesCount);

		m_pSkeletonVA = VertexArray::create(pSkeltonVertexBuffer, pSkeltonIndexBuffer);
		m_pVertexPosShader = Shader::create("assets/shaders/vertexPos.glsl");
	}

	void KinectAPI::_setHandIsClosed(bool& isHandClosed_, HandState handState_)
	{
		switch (handState_)
		{
		case HandState_Closed:
		{
			isHandClosed_ = true;
		}break;

		case HandState_Open:
		{
			isHandClosed_ = false;
		}break;

		/*case HandState_Lasso:
			m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
			break;*/
		}
	}

	Vector3f KinectAPI::mapTranslationTo(const Vector3f& src_translation_, const Vector3f& translation_setZero_, const KinectDataRange& src_ranges_, const KinectDataRange& target_ranges_)
	{
		//Vector3f relative_translation = src_translation_ - translation_setZero_;
		Vector3f src_full_range = src_ranges_.max_translation - src_ranges_.min_translation;
		Vector3f center_ratio = (translation_setZero_ - src_ranges_.min_translation) / src_full_range;
		Vector3f translation_ratio = (src_translation_ - src_ranges_.min_translation) / src_full_range;
		//translation_ratio = glm::clamp(translation_ratio, Vector3f(0.0f), Vector3f(1.0f));

		Vector3f target_center = MyMath::lerp(target_ranges_.min_translation, target_ranges_.max_translation, center_ratio);
		Vector3f target_translation = MyMath::lerp(target_ranges_.min_translation, target_ranges_.max_translation, translation_ratio);

		/*VOG_LOG_INFO("=====Start");
		VOG_LOG_INFO("src_translation_: <{0}, {1}, {2}>", src_translation_.x, src_translation_.y, src_translation_.z);
		VOG_LOG_INFO("ser zero center: <{0}, {1}, {2}>", translation_setZero_.x, translation_setZero_.y, translation_setZero_.z);

		VOG_LOG_INFO("center ratio: <{0}, {1}, {2}>",			center_ratio.x, center_ratio.y, center_ratio.z);
		VOG_LOG_INFO("translation ratio: <{0}, {1}, {2}>",		translation_ratio.x, translation_ratio.y, translation_ratio.z);
		VOG_LOG_INFO("target_center: <{0}, {1}, {2}>",			target_center.x, target_center.y, target_center.z);
		VOG_LOG_INFO("target_translation: <{0}, {1}, {2}>",		target_translation.x, target_translation.y, target_translation.z);
		VOG_LOG_INFO("=====End");*/


		return target_translation - target_center;

		/*float x_ratio = relative_translation.x / (src_full_range.x);
		float y_ratio = relative_translation.y / (src_full_range.y);
		float z_ratio = relative_translation.z / (src_full_range.z);
		Vector3f ret;
		if (x_ratio > 0.0f)
			ret.x = target_ranges_.max_translation.x * x_ratio;
		else
			ret.x = target_ranges_.min_translation.x * -x_ratio;

		if (y_ratio > 0.0f)
			ret.y = target_ranges_.max_translation.y * y_ratio;
		else
			ret.y = target_ranges_.min_translation.y * -y_ratio;

		if (z_ratio > 0.0f)
			ret.z = target_ranges_.max_translation.z * z_ratio;
		else
			ret.z = target_ranges_.min_translation.z * -z_ratio;
		return ret;*/
	}

	std::pair<Vector2f, Vector2f> KinectAPI::getHandScreenPosition()
	{
		return { m_leftHandScreenPosition, m_rightHandScreenPosition };
	}

	std::pair<bool, bool> KinectAPI::getIsHandClosed()
	{
		return{m_isLeftHandClosed, m_isRightHandClosed};
	}

	Vector2f KinectAPI::cameraToScreen(const CameraSpacePoint& bodyPoint_, int width_, int height_)
	{
		// Calculate the body's position on the screen
		DepthSpacePoint depthPoint = { 0 };
		m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint_, &depthPoint);

		float screenPointX = static_cast<float>(depthPoint.X * width_) / cDepthWidth;
		float screenPointY = static_cast<float>(depthPoint.Y * height_) / cDepthHeight;

		return Vector2f(screenPointX, screenPointY);
	}

	void KinectAPI::_processBodyData(std::array<Vector3f, bodyVerticesCount>& joints_, const Joint* pJoints_)
	{
		int i = 0;
		// Torso
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Head].Position);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Neck].Position);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Neck].Position);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineMid].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineMid].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderRight].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipRight].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipLeft].Position);			i++;

		// Right Arm    
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderRight].Position);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandTipRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ThumbRight].Position);		i++;

		// Left Arm
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandLeft].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandLeft].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandTipLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ThumbLeft].Position);		i++;

		// Right Leg
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipRight].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleRight].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_FootRight].Position);		i++;

		// Left Leg
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipLeft].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeLeft].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeLeft].Position);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleLeft].Position);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_FootLeft].Position);			i++;
	}
	void KinectAPI::_processBodyData(std::array<Vector3f, bodyVerticesCount>& joints_, const DirectX::XMVECTOR* pJoints_)
	{
		int i = 0;
		// Torso
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Head]);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Neck]);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_Neck]);				i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineMid]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineMid]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderRight]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineShoulder]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipRight]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_SpineBase]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipLeft]);			i++;

		// Right Arm    
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderRight]);	i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandTipRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ThumbRight]);		i++;

		// Left Arm
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ShoulderLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ElbowLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandLeft]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandLeft]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HandTipLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_WristLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_ThumbLeft]);		i++;

		// Right Leg
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipRight]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleRight]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_FootRight]);		i++;

		// Left Leg
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_HipLeft]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeLeft]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_KneeLeft]);			i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_AnkleLeft]);		i++;
		joints_[i] = /* m_transformScaleFactor * */ toVector3f(pJoints_[JointType_FootLeft]);			i++;
	}
#endif // 
	
}

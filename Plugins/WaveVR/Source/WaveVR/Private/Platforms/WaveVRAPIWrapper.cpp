// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRAPIWrapper.h"
#include "WaveVRPrivatePCH.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

class OpenglCoordinate {
public:
	static WVR_Vector3f_t GetVector3(const FVector& ue_pos) {
		WVR_Vector3f_t vec;

		vec.v[0] = ue_pos.Y / 100;
		vec.v[1] = ue_pos.Z / 100;
		vec.v[2] = -ue_pos.X / 100;
		return vec;
	}

	static WVR_Quatf_t GetQuaternion(const FRotator& ue_rot)
	{
		WVR_Quatf_t qat;
		FQuat ue_ori = ue_rot.Quaternion();
		qat.w = -ue_ori.W;
		qat.x = ue_ori.Y;
		qat.y = ue_ori.Z;
		qat.z = -ue_ori.X;
		return qat;
	}

	static WVR_Matrix4f_t GetMatrix44(const FVector& ue_pos, const FRotator& ue_rot)
	{
		WVR_Vector3f_t pos;
		pos.v[0] = ue_pos.Y / 100;
		pos.v[1] = ue_pos.Z / 100;
		pos.v[2] = -ue_pos.X / 100;

		FQuat ue_ori = ue_rot.Quaternion();
		WVR_Quatf_t rot;
		rot.w = -ue_ori.W;
		rot.x = ue_ori.Y;
		rot.y = ue_ori.Z;
		rot.z = -ue_ori.X;

		WVR_Matrix4f_t mat;

		// m[0, 0] = 1 - 2 * y^2 - 2 * z^2
		mat.m[0][0] = 1 - (2 * (rot.y * rot.y)) - (2 * (rot.z * rot.z));
		// m[0, 1] = 2xy - 2zw
		mat.m[0][1] = (2 * rot.x * rot.y) - (2 * rot.z * rot.w);
		// m[0, 2] = 2xz + 2yw
		mat.m[0][2] = (2 * rot.x * rot.z) + (2 * rot.y * rot.w);
		// m[0, 3] = X
		mat.m[0][3] = pos.v[0];
		// m[1, 0] = 2xy + 2zw
		mat.m[1][0] = (2 * rot.x * rot.y) + (2 * rot.z * rot.w);
		// m[1, 1] = 1 - 2 * x^2 - 2 * z^2
		mat.m[1][1] = 1 - (2 * (rot.x * rot.x)) - (2 * (rot.z * rot.z));
		// m[1, 2] = 2yz -2xw
		mat.m[1][2] = (2 * rot.y * rot.z) - (2 * rot.x * rot.w);
		// m[1, 3] = Y
		mat.m[1][3] = pos.v[1];
		// m[2, 0] = 2xz - 2yw
		mat.m[2][0] = (2 * rot.x * rot.z) - (2 * rot.y * rot.w);
		// m[2, 1] = 2yz + 2xw
		mat.m[2][1] = (2 * rot.y * rot.z) + (2 * rot.x * rot.w);
		// m[2, 2] = 1 - 2 * x^2 - 2 * y^2
		mat.m[2][2] = 1 - (2 * rot.x * rot.x) - (2 * rot.y * rot.y);
		// m[2, 3] = Z
		mat.m[2][3] = pos.v[2];
		// m[3, 0] = 0
		mat.m[3][0] = 0;
		// m[3, 1] = 0
		mat.m[3][1] = 0;
		// m[3, 2] = 0
		mat.m[3][2] = 0;
		// m[3, 3] = 1
		mat.m[3][3] = 1;

		return mat;
	}
};

FVector BONE_L_OFFSET = FVector(15, 0, 2);
// Left wrist.
FVector BONE_HAND_WRIST_L_POS		= FVector(20.0f, -9, 0);
FRotator BONE_HAND_WRIST_L_ROT		= FRotator(165, 0, -7);
// Left thumb.
FVector BONE_THUMB_JOINT2_L_POS		= FVector(20.0f, -5, 2);
FVector BONE_THUMB_JOINT3_L_POS		= FVector(20.0f, -4, 3);
FVector BONE_THUMB_TIP_L_POS		= FVector(20.0f, -3, 4);
// Left index.
FVector BONE_INDEX_JOINT1_L_POS		= FVector(20.0f, -6, 4);
FVector BONE_INDEX_JOINT2_L_POS		= FVector(20.0f, -5.6f, 5);
FVector BONE_INDEX_JOINT3_L_POS		= FVector(20.0f, -5.2f, 6);
FVector BONE_INDEX_TIP_L_POS		= FVector(20.0f, -4.8f, 7);
// Left middle.
FVector BONE_MIDDLE_JOINT1_L_POS	= FVector(20.0f, -7.5f, 4.5f);
FVector BONE_MIDDLE_JOINT2_L_POS	= FVector(20.0f, -7.4f, 5.5f);
FVector BONE_MIDDLE_JOINT3_L_POS	= FVector(20.0f, -7.3f, 6.5f);
FVector BONE_MIDDLE_TIP_L_POS		= FVector(20.0f, -7.2f, 7.5f);
// Left ring.
FVector BONE_RING_JOINT1_L_POS		= FVector(20.0f, -8.7f, 4);
FVector BONE_RING_JOINT2_L_POS		= FVector(20.0f, -8.9f, 5);
FVector BONE_RING_JOINT3_L_POS		= FVector(20.0f, -9.1f, 6);
FVector BONE_RING_TIP_L_POS			= FVector(20.0f, -9.3f, 7);
// Left pinky.
FVector BONE_PINKY_JOINT1_L_POS		= FVector(20.0f, -9.9f, 3);
FVector BONE_PINKY_JOINT2_L_POS		= FVector(20.0f, -10.3f, 4);
FVector BONE_PINKY_JOINT3_L_POS		= FVector(20.0f, -10.6f, 5);
FVector BONE_PINKY_TIP_L_POS		= FVector(20.0f, -10.9f, 6);

FVector BONE_R_OFFSET = FVector(15, -0, 2);
// Right wrist.
FVector BONE_HAND_WRIST_R_POS		= FVector(20.0f, 9, 0);
FRotator BONE_HAND_WRIST_R_ROT		= FRotator(165, 0, 7);
// Right thumb.
FVector BONE_THUMB_JOINT2_R_POS		= FVector(20.0f, 5, 2);
FVector BONE_THUMB_JOINT3_R_POS		= FVector(20.0f, 4, 3);
FVector BONE_THUMB_2_DISTAL_R_POS	= FVector(20.0f, 3, 4);
// Right index.
FVector BONE_INDEX_JOINT1_R_POS		= FVector(20.0f, 6, 4);
FVector BONE_INDEX_JOINT2_R_POS		= FVector(20.0f, 5.6f, 5);
FVector BONE_INDEX_JOINT3_R_POS		= FVector(20.0f, 5.2f, 6);
FVector BONE_INDEX_TIP_R_POS		= FVector(20.0f, 4.8f, 7);
// Right middle.
FVector BONE_MIDDLE_JOINT1_R_POS	= FVector(20.0f, 7.5f, 4.5f);
FVector BONE_MIDDLE_JOINT2_R_POS	= FVector(20.0f, 7.4f, 5.5f);
FVector BONE_MIDDLE_JOINT3_R_POS	= FVector(20.0f, 7.3f, 6.5f);
FVector BONE_MIDDLE_TIP_R_POS		= FVector(20.0f, 7.2f, 7.5f);
// Right ring.
FVector BONE_RING_JOINT1_R_POS		= FVector(20.0f, 8.7f, 4);
FVector BONE_RING_JOINT2_R_POS		= FVector(20.0f, 8.9f, 5);
FVector BONE_RING_JOINT3_R_POS		= FVector(20.0f, 9.1f, 6);
FVector BONE_RING_TIP_R_POS			= FVector(20.0f, 9.3f, 7);
// Right pinky.
FVector BONE_PINKY_JOINT1_R_POS		= FVector(20.0f, 9.9f, 3);
FVector BONE_PINKY_JOINT2_R_POS		= FVector(20.0f, 10.3f, 4);
FVector BONE_PINKY_JOINT3_R_POS		= FVector(20.0f, 10.6f, 5);
FVector BONE_PINKY_TIP_R_POS		= FVector(20.0f, 10.9f, 6);

FVector PINCH_DIRECTION_R = FVector(1, 0, 0.5f);

FWaveVRAPIWrapper * FWaveVRAPIWrapper::instance = nullptr;

FWaveVRAPIWrapper::FWaveVRAPIWrapper()
	: focusController(WVR_DeviceType::WVR_DeviceType_Controller_Right)
{
	handTrackingData.left.wrist.isValidPose = true;
	handTrackingData.left.confidence = 1;
	// Left wrist.
	handTrackingData.left.wrist.poseMatrix			= OpenglCoordinate::GetMatrix44(BONE_HAND_WRIST_L_POS + BONE_L_OFFSET, BONE_HAND_WRIST_L_ROT);
	// Left thumb.
	handTrackingData.left.thumb.joint2	= OpenglCoordinate::GetVector3(BONE_THUMB_JOINT2_L_POS + BONE_L_OFFSET);
	handTrackingData.left.thumb.joint3	= OpenglCoordinate::GetVector3(BONE_THUMB_JOINT3_L_POS + BONE_L_OFFSET);
	handTrackingData.left.thumb.tip		= OpenglCoordinate::GetVector3(BONE_THUMB_TIP_L_POS + BONE_L_OFFSET);
	// Left index.
	handTrackingData.left.index.joint1	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT1_L_POS + BONE_L_OFFSET);
	handTrackingData.left.index.joint2	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT2_L_POS + BONE_L_OFFSET);
	handTrackingData.left.index.joint3	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT3_L_POS + BONE_L_OFFSET);
	handTrackingData.left.index.tip		= OpenglCoordinate::GetVector3(BONE_INDEX_TIP_L_POS + BONE_L_OFFSET);
	// Left middle.
	handTrackingData.left.middle.joint1	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT1_L_POS + BONE_L_OFFSET);
	handTrackingData.left.middle.joint2	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT2_L_POS + BONE_L_OFFSET);
	handTrackingData.left.middle.joint3	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT3_L_POS + BONE_L_OFFSET);
	handTrackingData.left.middle.tip		= OpenglCoordinate::GetVector3(BONE_MIDDLE_TIP_L_POS + BONE_L_OFFSET);
	// Left ring.
	handTrackingData.left.ring.joint1	= OpenglCoordinate::GetVector3(BONE_RING_JOINT1_L_POS + BONE_L_OFFSET);
	handTrackingData.left.ring.joint2	= OpenglCoordinate::GetVector3(BONE_RING_JOINT2_L_POS + BONE_L_OFFSET);
	handTrackingData.left.ring.joint3	= OpenglCoordinate::GetVector3(BONE_RING_JOINT3_L_POS + BONE_L_OFFSET);
	handTrackingData.left.ring.tip		= OpenglCoordinate::GetVector3(BONE_RING_TIP_L_POS + BONE_L_OFFSET);
	// Left pinky.
	handTrackingData.left.pinky.joint1	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT1_L_POS + BONE_L_OFFSET);
	handTrackingData.left.pinky.joint2	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT2_L_POS + BONE_L_OFFSET);
	handTrackingData.left.pinky.joint3	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT3_L_POS + BONE_L_OFFSET);
	handTrackingData.left.pinky.tip		= OpenglCoordinate::GetVector3(BONE_PINKY_TIP_L_POS + BONE_L_OFFSET);

	handTrackingData.right.wrist.isValidPose = true;
	handTrackingData.right.confidence = 0.5f;
	// Right wrist.
	handTrackingData.right.wrist.poseMatrix			= OpenglCoordinate::GetMatrix44(BONE_HAND_WRIST_R_POS + BONE_R_OFFSET, BONE_HAND_WRIST_R_ROT);
	// Right thumb.
	handTrackingData.right.thumb.joint2	= OpenglCoordinate::GetVector3(BONE_THUMB_JOINT2_R_POS + BONE_R_OFFSET);
	handTrackingData.right.thumb.joint3	= OpenglCoordinate::GetVector3(BONE_THUMB_JOINT3_R_POS + BONE_R_OFFSET);
	handTrackingData.right.thumb.tip		= OpenglCoordinate::GetVector3(BONE_THUMB_2_DISTAL_R_POS + BONE_R_OFFSET);
	// Right index.
	handTrackingData.right.index.joint1	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT1_R_POS + BONE_R_OFFSET);
	handTrackingData.right.index.joint2	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT2_R_POS + BONE_R_OFFSET);
	handTrackingData.right.index.joint3	= OpenglCoordinate::GetVector3(BONE_INDEX_JOINT3_R_POS + BONE_R_OFFSET);
	handTrackingData.right.index.tip		= OpenglCoordinate::GetVector3(BONE_INDEX_TIP_R_POS + BONE_R_OFFSET);
	// Right middle.
	handTrackingData.right.middle.joint1	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT1_R_POS + BONE_R_OFFSET);
	handTrackingData.right.middle.joint2	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT2_R_POS + BONE_R_OFFSET);
	handTrackingData.right.middle.joint3	= OpenglCoordinate::GetVector3(BONE_MIDDLE_JOINT3_R_POS + BONE_R_OFFSET);
	handTrackingData.right.middle.tip	= OpenglCoordinate::GetVector3(BONE_MIDDLE_TIP_R_POS + BONE_R_OFFSET);
	// Right ring.
	handTrackingData.right.ring.joint1	= OpenglCoordinate::GetVector3(BONE_RING_JOINT1_R_POS + BONE_R_OFFSET);
	handTrackingData.right.ring.joint2	= OpenglCoordinate::GetVector3(BONE_RING_JOINT2_R_POS + BONE_R_OFFSET);
	handTrackingData.right.ring.joint3	= OpenglCoordinate::GetVector3(BONE_RING_JOINT3_R_POS + BONE_R_OFFSET);
	handTrackingData.right.ring.tip		= OpenglCoordinate::GetVector3(BONE_RING_TIP_R_POS + BONE_R_OFFSET);
	// Right pinky.
	handTrackingData.right.pinky.joint1	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT1_R_POS + BONE_R_OFFSET);
	handTrackingData.right.pinky.joint2	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT2_R_POS + BONE_R_OFFSET);
	handTrackingData.right.pinky.joint3	= OpenglCoordinate::GetVector3(BONE_PINKY_JOINT3_R_POS + BONE_R_OFFSET);
	handTrackingData.right.pinky.tip	= OpenglCoordinate::GetVector3(BONE_PINKY_TIP_R_POS + BONE_R_OFFSET);

	// Static Gesture
	gestureData.left = WVR_HandGestureType::WVR_HandGestureType_IndexUp;
	gestureData.right = WVR_HandGestureType::WVR_HandGestureType_OK;
}

FWaveVRAPIWrapper::~FWaveVRAPIWrapper()
{
	if (FWaveVRAPIWrapper::instance == this)
		FWaveVRAPIWrapper::instance = nullptr;
}

void FWaveVRAPIWrapper::SetInstance(FWaveVRAPIWrapper * inst)
{
	FWaveVRAPIWrapper::instance = inst;
}

int32 tickCount = 0, tickAdder = 1;
FRotator leftYawRotator = FRotator(0, 0.1f, 0), rightYawRotator = FRotator(0, -0.1f, 0);
FQuat leftYawOrientation = FQuat::Identity, rightYawOrientation = FQuat::Identity;
int32 gestureCount = 0;
void FWaveVRAPIWrapper::Tick()
{
	tickCount += tickAdder;
	if (tickCount == 100 || tickCount == -100)
	{
		tickAdder *= -1;
	}
	leftYawRotator.Yaw += 0.1f * tickAdder;
	rightYawRotator.Yaw += -0.1f * tickAdder;
	leftYawOrientation = leftYawRotator.Quaternion();
	rightYawOrientation = rightYawRotator.Quaternion();

	// Left wrist.
	handTrackingData.left.wrist.poseMatrix = OpenglCoordinate::GetMatrix44(leftYawOrientation * (BONE_HAND_WRIST_L_POS + BONE_L_OFFSET), BONE_HAND_WRIST_L_ROT);
	// Left thumb.
	handTrackingData.left.thumb.joint2
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_JOINT2_L_POS + BONE_L_OFFSET));
	handTrackingData.left.thumb.joint3
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_JOINT3_L_POS + BONE_L_OFFSET));
	handTrackingData.left.thumb.tip
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_TIP_L_POS + BONE_L_OFFSET));
	// Left index.
	handTrackingData.left.index.joint1
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT1_L_POS + BONE_L_OFFSET));
	handTrackingData.left.index.joint2
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT2_L_POS + BONE_L_OFFSET));
	handTrackingData.left.index.joint3
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT3_L_POS + BONE_L_OFFSET));
	handTrackingData.left.index.tip
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_TIP_L_POS + BONE_L_OFFSET));
	// Left middle.
	handTrackingData.left.middle.joint1
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT1_L_POS + BONE_L_OFFSET));
	handTrackingData.left.middle.joint2
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT2_L_POS + BONE_L_OFFSET));
	handTrackingData.left.middle.joint3
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT3_L_POS + BONE_L_OFFSET));
	handTrackingData.left.middle.tip
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_TIP_L_POS + BONE_L_OFFSET));
	// Left ring.
	handTrackingData.left.ring.joint1
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT1_L_POS + BONE_L_OFFSET));
	handTrackingData.left.ring.joint2
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT2_L_POS + BONE_L_OFFSET));
	handTrackingData.left.ring.joint3
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT3_L_POS + BONE_L_OFFSET));
	handTrackingData.left.ring.tip
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_TIP_L_POS + BONE_L_OFFSET));
	// Left pinky.
	handTrackingData.left.pinky.joint1
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT1_L_POS + BONE_L_OFFSET));
	handTrackingData.left.pinky.joint2
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT2_L_POS + BONE_L_OFFSET));
	handTrackingData.left.pinky.joint3
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT3_L_POS + BONE_L_OFFSET));
	handTrackingData.left.pinky.tip
		= OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_TIP_L_POS + BONE_L_OFFSET));

	// Right wrist.
	handTrackingData.right.wrist.poseMatrix = OpenglCoordinate::GetMatrix44(rightYawOrientation * (BONE_HAND_WRIST_R_POS + BONE_R_OFFSET), BONE_HAND_WRIST_R_ROT);
	// Right thumb.
	handTrackingData.right.thumb.joint2
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_JOINT2_R_POS + BONE_R_OFFSET));
	handTrackingData.right.thumb.joint3
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_JOINT3_R_POS + BONE_R_OFFSET));
	handTrackingData.right.thumb.tip
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_2_DISTAL_R_POS + BONE_R_OFFSET));
	// Right index.
	handTrackingData.right.index.joint1
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT1_R_POS + BONE_R_OFFSET));
	handTrackingData.right.index.joint2
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT2_R_POS + BONE_R_OFFSET));
	handTrackingData.right.index.joint3
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT3_R_POS + BONE_R_OFFSET));
	handTrackingData.right.index.tip
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_TIP_R_POS + BONE_R_OFFSET));
	// Right middle.
	handTrackingData.right.middle.joint1
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT1_R_POS + BONE_R_OFFSET));
	handTrackingData.right.middle.joint2
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT2_R_POS + BONE_R_OFFSET));
	handTrackingData.right.middle.joint3
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT3_R_POS + BONE_R_OFFSET));
	handTrackingData.right.middle.tip
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_TIP_R_POS + BONE_R_OFFSET));
	// Right ring.
	handTrackingData.right.ring.joint1
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT1_R_POS + BONE_R_OFFSET));
	handTrackingData.right.ring.joint2
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT2_R_POS + BONE_R_OFFSET));
	handTrackingData.right.ring.joint3
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT3_R_POS + BONE_R_OFFSET));
	handTrackingData.right.ring.tip
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_TIP_R_POS + BONE_R_OFFSET));
	// Right pinky.
	handTrackingData.right.pinky.joint1
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT1_R_POS + BONE_R_OFFSET));
	handTrackingData.right.pinky.joint2
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT2_R_POS + BONE_R_OFFSET));
	handTrackingData.right.pinky.joint3
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT3_R_POS + BONE_R_OFFSET));
	handTrackingData.right.pinky.tip
		= OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_TIP_R_POS + BONE_R_OFFSET));

	// Static Gesture
	gestureCount++;
	gestureCount %= 300;
	if (gestureCount == 0)
	{
		if (gestureData.left == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
			gestureData.left = WVR_HandGestureType::WVR_HandGestureType_Five;
		else
			gestureData.left = WVR_HandGestureType::WVR_HandGestureType_IndexUp;

		if (gestureData.right == WVR_HandGestureType::WVR_HandGestureType_OK)
			gestureData.right = WVR_HandGestureType::WVR_HandGestureType_Five;
		else
			gestureData.right = WVR_HandGestureType::WVR_HandGestureType_OK;
	}
}

WVR_Result FWaveVRAPIWrapper::GetHandTrackingData(WVR_HandSkeletonData_t *skeleton, WVR_HandPoseData_t* pose, WVR_PoseOriginModel type)
{
	*skeleton = handTrackingData;
	pose->right.pinch.base.type = WVR_HandPoseType::WVR_HandPoseType_Pinch;
	pose->right.pinch.finger = WVR_FingerType::WVR_FingerType_Index;
	pose->right.pinch.strength = 0.8f;
	pose->right.pinch.origin = OpenglCoordinate::GetVector3(BONE_HAND_WRIST_R_POS + BONE_R_OFFSET);
	pose->right.pinch.direction = OpenglCoordinate::GetVector3(PINCH_DIRECTION_R);
	return WVR_Result::WVR_Success;
}

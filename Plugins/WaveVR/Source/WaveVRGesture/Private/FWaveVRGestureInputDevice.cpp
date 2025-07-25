// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "FWaveVRGestureInputDevice.h"
#include "IWaveVRGesture.h"
#include "WaveVRStaticGestureComponent.h"
#include "WaveVRUtils.h"

using namespace wvr::utils;

FWaveVRGestureInputDevice::FWaveVRGestureInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) :
	MessageHandler(InMessageHandler),
	DeviceIndex(0),
	bHandGestureDataUpdated(false),
	prevStaticGestureRight(EWaveVRStaticGesture::TYPE_INVALID),
	prevStaticGestureLeft(EWaveVRStaticGesture::TYPE_INVALID),
	currStaticGestureRight(EWaveVRStaticGesture::TYPE_INVALID),
	currStaticGestureLeft(EWaveVRStaticGesture::TYPE_INVALID),
	bHandTrackingDataUpdated(false),
	pinchStrengthLeft(0),
	pinchStrengthRight(0),
	pinchOriginLeft(FVector::ZeroVector),
	pinchOriginRight(FVector::ZeroVector),
	pinchDirectionLeft(FVector::ZeroVector),
	pinchDirectionRight(FVector::ZeroVector),
	World(nullptr),
	BONE_OFFSET_L(FVector(0, 0, 0)),
	BONE_OFFSET_R(FVector(0, 0, 0))
{
	Skeleton = new WaveVRGestureSkeleton();
	bGestureInputDeviceInitialized = true;
	gestureThread = FWaveVRGestureThread::JoyInit();
	UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("FWaveVRGestureInputDevice()"));
}


FWaveVRGestureInputDevice::~FWaveVRGestureInputDevice()
{
	FWaveVRGestureThread::Shutdown();
	gestureThread = NULL;
	UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("~FWaveVRGestureInputDevice()"));
}


void FWaveVRGestureInputDevice::Tick(float DeltaTime)
{
	FWaveVRHMD* HMD = GetWaveVRHMD();

	if (!bGestureInputDeviceInitialized
		|| (gestureThread == NULL)
		|| (HMD == nullptr))
	{
		bHandGestureDataUpdated = false;
		bHandTrackingDataUpdated = false;
		return;
	}

	/* ----------------------- Hand Gesture -----------------------*/
	WVR_EventType gesture_event = HMD->GetGestureEvent();

	if (gesture_event == WVR_EventType::WVR_EventType_HandGesture_Abnormal)
	{
		gestureThread->RestartHandGesture();
		HMD->SetGestureEvent(WVR_EventType::WVR_EventType_HandGesture_Changed);
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("Tick() hand gesture is abnormal, restart the feature."));
	}

	if (gestureThread->IsHandGestureAvailable())
	{
		bHandGestureDataUpdated = FWaveVRAPIWrapper::GetInstance()->GetHandGestureData(&handGestureData) == WVR_Result::WVR_Success ? true : false;
		if (bHandGestureDataUpdated)
		{
			UpdateLeftHandGestureData();
			UpdateRightHandGestureData();
		}
	}
	else
	{
		bHandGestureDataUpdated = false;
	}



	/* ----------------------- Hand Tracking -----------------------*/
	if (gesture_event == WVR_EventType::WVR_EventType_HandTracking_Abnormal)
	{
		gestureThread->RestartHandTracking();
		HMD->SetGestureEvent(WVR_EventType::WVR_EventType_HandGesture_Changed);
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("Tick() hand tracking is abnormal, restart the feature."));
	}

	if (gestureThread->IsHandTrackingAvailable())
	{
		UpdateParametersFromPawn();

		bHandTrackingDataUpdated = FWaveVRAPIWrapper::GetInstance()->GetHandTrackingData(&handSkeletonData, &handPoseData) == WVR_Result::WVR_Success ? true : false;
		if (bHandTrackingDataUpdated)	// Get the tracking data from the shmem.
		{
			if (handSkeletonData.left.wrist.isValidPose)
				UpdateLeftHandTrackingData();

			if (handSkeletonData.right.wrist.isValidPose)
				UpdateRightHandTrackingData();

			UpdateLeftHandPoseData();
			UpdateRightHandPoseData();
		}
	}
	else
	{
		bHandTrackingDataUpdated = false;
	}
}


void FWaveVRGestureInputDevice::SendControllerEvents()
{
}


void FWaveVRGestureInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}


bool FWaveVRGestureInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
	return false;
}


void FWaveVRGestureInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
}


void FWaveVRGestureInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
}

#pragma region
bool FWaveVRGestureInputDevice::IsGestureInputDeviceInitialized()
{
	return bGestureInputDeviceInitialized;
}

void FWaveVRGestureInputDevice::StartHandGesture()
{
	if (gestureThread != NULL)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("StartHandGesture()"));
		gestureThread->StartHandGesture();
	}
}

void FWaveVRGestureInputDevice::StopHandGesture()
{
	if (gestureThread != NULL)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("StopHandGesture()"));
		gestureThread->StopHandGesture();
	}
}

bool FWaveVRGestureInputDevice::IsHandGestureAvailable()
{
	if (gestureThread != NULL)
		return gestureThread->IsHandGestureAvailable();

	return false;
}

EWaveVRStaticGesture FWaveVRGestureInputDevice::GetStaticGestureType(EWaveVRGestureHandType DevType)
{
	if (!bHandGestureDataUpdated)
		return EWaveVRStaticGesture::TYPE_INVALID;

	if (DevType == EWaveVRGestureHandType::LEFT)
		return currStaticGestureLeft;
	if (DevType == EWaveVRGestureHandType::RIGHT)
		return currStaticGestureRight;

	return EWaveVRStaticGesture::TYPE_INVALID;
}

EWaveVRHandGestureStatus FWaveVRGestureInputDevice::GetHandGestureStatus()
{
	if (gestureThread != NULL)
		return gestureThread->GetHandGestureStatus();

	return EWaveVRHandGestureStatus::UNSUPPORT;
}

void FWaveVRGestureInputDevice::StartHandTracking()
{
	if (gestureThread != NULL)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("StartHandTracking()"));
		gestureThread->StartHandTracking();
	}
}

void FWaveVRGestureInputDevice::StopHandTracking()
{
	if (gestureThread != NULL)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("StopHandTracking()"));
		gestureThread->StopHandTracking();
	}
}

bool FWaveVRGestureInputDevice::IsHandTrackingAvailable()
{
	if (gestureThread != NULL)
		return gestureThread->IsHandTrackingAvailable();

	return false;
}

bool FWaveVRGestureInputDevice::GetBonePositionAndRotation(int32 DevId, EWaveVRGestureBoneType BoneId, FVector& OutPosition, FRotator& OutRotation)
{
	if (!bHandTrackingDataUpdated
		|| DevId < 0
		|| ((int32)BoneId >= (int32)EWaveVRGestureBoneType::BONES_COUNT)
		|| ((int32)BoneId <= (int32)EWaveVRGestureBoneType::BONE_ROOT)
		)
		return false;

	OutPosition = Skeleton->Bones[(int32)BoneId]->GetPosition();
	OutRotation = Skeleton->Bones[(int32)BoneId]->GetRotation();
	return Skeleton->Bones[(int32)BoneId]->IsValidPose();
}

EWaveVRHandTrackingStatus FWaveVRGestureInputDevice::GetHandTrackingStatus()
{
	if (gestureThread != NULL)
		return gestureThread->GetHandTrackingStatus();

	return EWaveVRHandTrackingStatus::UNSUPPORT;
}

float FWaveVRGestureInputDevice::GetHandConfidence(EWaveVRGestureHandType hand)
{
	if (!bHandTrackingDataUpdated)
		return 0;

	return (hand == EWaveVRGestureHandType::LEFT ? handSkeletonData.left.confidence : handSkeletonData.right.confidence);
}

float FWaveVRGestureInputDevice::GetHandPinchStrength(EWaveVRGestureHandType hand)
{
	if (!bHandTrackingDataUpdated)
		return 0;

	return (hand == EWaveVRGestureHandType::LEFT ? pinchStrengthLeft : pinchStrengthRight);
}

FVector FWaveVRGestureInputDevice::GetHandPinchOrigin(EWaveVRGestureHandType hand)
{
	if (!bHandTrackingDataUpdated)
		return FVector::ZeroVector;

	return (hand == EWaveVRGestureHandType::LEFT ? pinchOriginLeft : pinchOriginRight);
}

FVector FWaveVRGestureInputDevice::GetHandPinchDirection(EWaveVRGestureHandType hand)
{
	if (!bHandTrackingDataUpdated)
		return FVector::ZeroVector;

	return (hand == EWaveVRGestureHandType::LEFT ? pinchDirectionLeft : pinchDirectionRight);
}
#pragma endregion GestureBPLibrary related functions.

#pragma region
void FWaveVRGestureInputDevice::UpdateLeftHandPoseData()
{
	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	if (handPoseData.left.pinch.base.type == WVR_HandPoseType::WVR_HandPoseType_Invalid)
	{
		pinchStrengthLeft = 0;
		pinchOriginLeft = FVector::ZeroVector;
		pinchDirectionLeft = FVector::ZeroVector;
	}

	if (handPoseData.left.pinch.base.type == WVR_HandPoseType::WVR_HandPoseType_Pinch)
	{
		pinchStrengthLeft = handPoseData.left.pinch.strength;
		pinchOriginLeft = CoordinateUtil::GetVector3(handPoseData.left.pinch.origin, HMD->GetWorldToMetersScale());
		pinchDirectionLeft = CoordinateUtil::GetVector3(handPoseData.left.pinch.direction, HMD->GetWorldToMetersScale());
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("UpdateLeftHandPoseData() %f"), pinchStrengthLeft);
	}
}

void FWaveVRGestureInputDevice::UpdateRightHandPoseData()
{
	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	if (handPoseData.right.pinch.base.type == WVR_HandPoseType::WVR_HandPoseType_Invalid)
	{
		pinchStrengthRight = 0;
		pinchOriginRight = FVector::ZeroVector;
		pinchDirectionRight = FVector::ZeroVector;
	}

	if (handPoseData.right.pinch.base.type == WVR_HandPoseType::WVR_HandPoseType_Pinch)
	{
		pinchStrengthRight = handPoseData.right.pinch.strength;
		pinchOriginRight = CoordinateUtil::GetVector3(handPoseData.right.pinch.origin, HMD->GetWorldToMetersScale());
		pinchDirectionRight = CoordinateUtil::GetVector3(handPoseData.right.pinch.direction, HMD->GetWorldToMetersScale());
	}
}
#pragma endregion Hand Pose

#pragma region
void FWaveVRGestureInputDevice::UpdateLeftHandTrackingData()
{
	if (Skeleton == nullptr) return;

	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	for (int i = (int32)EWaveVRGestureBoneType::BONE_UPPERARM_L; i < (int32)EWaveVRGestureBoneType::BONE_UPPERARM_R; i++)
	{
		Skeleton->Bones[i]->SetPoseValid(handSkeletonData.left.wrist.isValidPose);
		Skeleton->Bones[i]->SetConfidence(handSkeletonData.left.confidence);
	}

	// Left arm wrist - BONE_HAND_WRIST_L.
	FVector BONE_HAND_WRIST_L_Pos = FVector::ZeroVector;
	FQuat BONE_HAND_WRIST_L_Quat = FQuat::Identity;
	CoordinateUtil::MatrixToPose(handSkeletonData.left.wrist.poseMatrix, BONE_HAND_WRIST_L_Quat, BONE_HAND_WRIST_L_Pos, HMD->GetWorldToMetersScale());

	BONE_HAND_WRIST_L_Pos += BONE_OFFSET_L;
	FRotator BONE_HAND_WRIST_L_Rot = BONE_HAND_WRIST_L_Quat.Rotator();

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_L]->SetPosition(BONE_HAND_WRIST_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_L]->SetRotation(BONE_HAND_WRIST_L_Rot);

	// Left arm thumb joint1 bone - BONE_THUMB_JOINT1_L.
	FVector BONE_THUMB_JOINT1_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.thumb.joint1, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_THUMB_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_L]->SetPosition(BONE_THUMB_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_L]->SetRotation(BONE_THUMB_JOINT1_L_Rot);

	// Left arm thumb joint2 bone - BONE_THUMB_JOINT2_L.
	FVector BONE_THUMB_JOINT2_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.thumb.joint2, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_THUMB_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_L]->SetPosition(BONE_THUMB_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_L]->SetRotation(BONE_THUMB_JOINT2_L_Rot);

	// Left arm thumb joint3 bone - BONE_THUMB_JOINT3_L.
	FVector BONE_THUMB_JOINT3_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.thumb.joint3, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT2_L_Pos, BONE_THUMB_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_L]->SetPosition(BONE_THUMB_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_L]->SetRotation(BONE_THUMB_JOINT3_L_Rot);

	// Left arm thumb tip bone - BONE_THUMB_TIP_L.
	FVector BONE_THUMB_TIP_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.thumb.tip, HMD->GetWorldToMetersScale());
	BONE_THUMB_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT3_L_Pos, BONE_THUMB_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_L]->SetPosition(BONE_THUMB_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_L]->SetRotation(BONE_THUMB_TIP_L_Rot);

	// Left arm index joint1 bone - BONE_INDEX_JOINT1_L.
	FVector BONE_INDEX_JOINT1_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.index.joint1, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_INDEX_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_L]->SetPosition(BONE_INDEX_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_L]->SetRotation(BONE_INDEX_JOINT1_L_Rot);

	// Left arm index joint2 bone - BONE_INDEX_JOINT2_L.
	FVector BONE_INDEX_JOINT2_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.index.joint2, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT1_L_Pos, BONE_INDEX_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_L]->SetPosition(BONE_INDEX_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_L]->SetRotation(BONE_INDEX_JOINT2_L_Rot);

	// Left arm index joint3 bone - BONE_INDEX_JOINT3_L.
	FVector BONE_INDEX_JOINT3_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.index.joint3, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT2_L_Pos, BONE_INDEX_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_L]->SetPosition(BONE_INDEX_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_L]->SetRotation(BONE_INDEX_JOINT3_L_Rot);

	// Left arm index tip bone - BONE_INDEX_TIP_L.
	FVector BONE_INDEX_TIP_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.index.tip, HMD->GetWorldToMetersScale());
	BONE_INDEX_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT3_L_Pos, BONE_INDEX_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_L]->SetPosition(BONE_INDEX_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_L]->SetRotation(BONE_INDEX_TIP_L_Rot);

	// Left arm middle joint1 bone - BONE_MIDDLE_JOINT1_L.
	FVector BONE_MIDDLE_JOINT1_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.middle.joint1, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_MIDDLE_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_L]->SetPosition(BONE_MIDDLE_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_L]->SetRotation(BONE_MIDDLE_JOINT1_L_Rot);

	// Left arm middle joint2 bone - BONE_MIDDLE_JOINT2_L.
	FVector BONE_MIDDLE_JOINT2_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.middle.joint2, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT1_L_Pos, BONE_MIDDLE_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_L]->SetPosition(BONE_MIDDLE_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_L]->SetRotation(BONE_MIDDLE_JOINT2_L_Rot);

	// Left arm middle joint3 bone - BONE_MIDDLE_JOINT3_L.
	FVector BONE_MIDDLE_JOINT3_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.middle.joint3, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT2_L_Pos, BONE_MIDDLE_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_L]->SetPosition(BONE_MIDDLE_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_L]->SetRotation(BONE_MIDDLE_JOINT3_L_Rot);

	// Left arm middle tip bone - BONE_MIDDLE_TIP_L.
	FVector BONE_MIDDLE_TIP_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.middle.tip, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT3_L_Pos, BONE_MIDDLE_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_L]->SetPosition(BONE_MIDDLE_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_L]->SetRotation(BONE_MIDDLE_TIP_L_Rot);

	// Left arm ring joint1 bone - BONE_RING_JOINT1_L.
	FVector BONE_RING_JOINT1_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.ring.joint1, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_RING_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_L]->SetPosition(BONE_RING_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_L]->SetRotation(BONE_RING_JOINT1_L_Rot);

	// Left arm ring joint2 bone - BONE_RING_JOINT2_L.
	FVector BONE_RING_JOINT2_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.ring.joint2, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT1_L_Pos, BONE_RING_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_L]->SetPosition(BONE_RING_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_L]->SetRotation(BONE_RING_JOINT2_L_Rot);

	// Left arm ring joint3 bone - BONE_RING_JOINT3_L.
	FVector BONE_RING_JOINT3_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.ring.joint3, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT2_L_Pos, BONE_RING_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_L]->SetPosition(BONE_RING_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_L]->SetRotation(BONE_RING_JOINT3_L_Rot);

	// Left arm ring tip bone - BONE_RING_TIP_L.
	FVector BONE_RING_TIP_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.ring.tip, HMD->GetWorldToMetersScale());
	BONE_RING_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT3_L_Pos, BONE_RING_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_L]->SetPosition(BONE_RING_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_L]->SetRotation(BONE_RING_TIP_L_Rot);

	// Left arm pinky joint1 bone - BONE_PINKY_JOINT1_L.
	FVector BONE_PINKY_JOINT1_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.pinky.joint1, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_PINKY_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_L]->SetPosition(BONE_PINKY_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_L]->SetRotation(BONE_PINKY_JOINT1_L_Rot);

	// Left arm pinky joint2 bone - BONE_PINKY_JOINT2_L.
	FVector BONE_PINKY_JOINT2_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.pinky.joint2, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT1_L_Pos, BONE_PINKY_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_L]->SetPosition(BONE_PINKY_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_L]->SetRotation(BONE_PINKY_JOINT2_L_Rot);

	// Left arm pinky joint3 bone - BONE_PINKY_JOINT3_L.
	FVector BONE_PINKY_JOINT3_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.pinky.joint3, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT2_L_Pos, BONE_PINKY_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_L]->SetPosition(BONE_PINKY_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_L]->SetRotation(BONE_PINKY_JOINT3_L_Rot);

	// Left arm pinky tip bone - BONE_PINKY_TIP_L.
	FVector BONE_PINKY_TIP_L_Pos = CoordinateUtil::GetVector3(handSkeletonData.left.pinky.tip, HMD->GetWorldToMetersScale());
	BONE_PINKY_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT3_L_Pos, BONE_PINKY_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_L]->SetPosition(BONE_PINKY_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_L]->SetRotation(BONE_PINKY_TIP_L_Rot);

}

void FWaveVRGestureInputDevice::UpdateRightHandTrackingData()
{
	if (Skeleton == nullptr) return;

	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	for (int i = (int32)EWaveVRGestureBoneType::BONE_UPPERARM_R; i < (int32)EWaveVRGestureBoneType::BONES_COUNT; i++)
	{
		Skeleton->Bones[i]->SetPoseValid(handSkeletonData.right.wrist.isValidPose);
		Skeleton->Bones[i]->SetConfidence(handSkeletonData.right.confidence);
	}

	// Right arm wrist - BONE_HAND_WRIST_R.
	FVector BONE_HAND_WRIST_R_Pos = FVector::ZeroVector;
	FQuat BONE_HAND_WRIST_R_Quat = FQuat::Identity;
	CoordinateUtil::MatrixToPose(handSkeletonData.right.wrist.poseMatrix, BONE_HAND_WRIST_R_Quat, BONE_HAND_WRIST_R_Pos, HMD->GetWorldToMetersScale());

	BONE_HAND_WRIST_R_Pos += BONE_OFFSET_R;
	FRotator BONE_HAND_WRIST_R_Rot = BONE_HAND_WRIST_R_Quat.Rotator();

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_R]->SetPosition(BONE_HAND_WRIST_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_R]->SetRotation(BONE_HAND_WRIST_R_Rot);

	// Right arm thumb joint1 bone - BONE_THUMB_JOINT1_R.
	FVector BONE_THUMB_JOINT1_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.thumb.joint1, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_THUMB_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_R]->SetPosition(BONE_THUMB_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_R]->SetRotation(BONE_THUMB_JOINT1_R_Rot);

	// Right arm thumb joint2 bone - BONE_THUMB_JOINT2_R.
	FVector BONE_THUMB_JOINT2_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.thumb.joint2, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_THUMB_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_R]->SetPosition(BONE_THUMB_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_R]->SetRotation(BONE_THUMB_JOINT2_R_Rot);

	// Right arm thumb joint2 bone - BONE_THUMB_JOINT3_R.
	FVector BONE_THUMB_JOINT3_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.thumb.joint3, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT2_R_Pos, BONE_THUMB_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_R]->SetPosition(BONE_THUMB_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_R]->SetRotation(BONE_THUMB_JOINT3_R_Rot);

	// Right arm thumb tip bone - BONE_THUMB_TIP_R.
	FVector BONE_THUMB_TIP_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.thumb.tip, HMD->GetWorldToMetersScale());
	BONE_THUMB_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT3_R_Pos, BONE_THUMB_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_R]->SetPosition(BONE_THUMB_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_R]->SetRotation(BONE_THUMB_TIP_R_Rot);

	// Right arm index joint1 bone - BONE_INDEX_JOINT1_R.
	FVector BONE_INDEX_JOINT1_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.index.joint1, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_INDEX_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_R]->SetPosition(BONE_INDEX_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_R]->SetRotation(BONE_INDEX_JOINT1_R_Rot);

	// Right arm index joint2 bone - BONE_INDEX_JOINT2_R.
	FVector BONE_INDEX_JOINT2_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.index.joint2, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT1_R_Pos, BONE_INDEX_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_R]->SetPosition(BONE_INDEX_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_R]->SetRotation(BONE_INDEX_JOINT2_R_Rot);

	// Right arm index joint3 bone - BONE_INDEX_JOINT3_R.
	FVector BONE_INDEX_JOINT3_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.index.joint3, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT2_R_Pos, BONE_INDEX_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_R]->SetPosition(BONE_INDEX_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_R]->SetRotation(BONE_INDEX_JOINT3_R_Rot);

	// Right arm index tip bone - BONE_INDEX_TIP_R.
	FVector BONE_INDEX_TIP_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.index.tip, HMD->GetWorldToMetersScale());
	BONE_INDEX_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT3_R_Pos, BONE_INDEX_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_R]->SetPosition(BONE_INDEX_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_R]->SetRotation(BONE_INDEX_TIP_R_Rot);

	// Right arm middle joint1 bone - BONE_MIDDLE_JOINT1_R.
	FVector BONE_MIDDLE_JOINT1_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.middle.joint1, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_MIDDLE_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_R]->SetPosition(BONE_MIDDLE_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_R]->SetRotation(BONE_MIDDLE_JOINT1_R_Rot);

	// Right arm middle joint2 bone - BONE_MIDDLE_JOINT2_R.
	FVector BONE_MIDDLE_JOINT2_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.middle.joint2, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT1_R_Pos, BONE_MIDDLE_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_R]->SetPosition(BONE_MIDDLE_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_R]->SetRotation(BONE_MIDDLE_JOINT2_R_Rot);

	// Right arm middle joint3 bone - BONE_MIDDLE_JOINT3_R.
	FVector BONE_MIDDLE_JOINT3_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.middle.joint3, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT2_R_Pos, BONE_MIDDLE_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_R]->SetPosition(BONE_MIDDLE_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_R]->SetRotation(BONE_MIDDLE_JOINT3_R_Rot);

	// Right arm middle tip bone - BONE_MIDDLE_TIP_R.
	FVector BONE_MIDDLE_TIP_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.middle.tip, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT3_R_Pos, BONE_MIDDLE_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_R]->SetPosition(BONE_MIDDLE_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_R]->SetRotation(BONE_MIDDLE_TIP_R_Rot);

	// Right arm ring joint1 bone - BONE_RING_JOINT1_R.
	FVector BONE_RING_JOINT1_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.ring.joint1, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_RING_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_R]->SetPosition(BONE_RING_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_R]->SetRotation(BONE_RING_JOINT1_R_Rot);

	// Right arm ring joint2 bone - BONE_RING_JOINT2_R.
	FVector BONE_RING_JOINT2_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.ring.joint2, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT1_R_Pos, BONE_RING_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_R]->SetPosition(BONE_RING_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_R]->SetRotation(BONE_RING_JOINT2_R_Rot);

	// Right arm ring joint3 bone - BONE_RING_JOINT3_R.
	FVector BONE_RING_JOINT3_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.ring.joint3, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT2_R_Pos, BONE_RING_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_R]->SetPosition(BONE_RING_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_R]->SetRotation(BONE_RING_JOINT3_R_Rot);

	// Right arm ring tip bone - BONE_RING_TIP_R.
	FVector BONE_RING_TIP_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.ring.tip, HMD->GetWorldToMetersScale());
	BONE_RING_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT3_R_Pos, BONE_RING_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_R]->SetPosition(BONE_RING_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_R]->SetRotation(BONE_RING_TIP_R_Rot);

	// Right arm pinky joint1 bone - BONE_PINKY_JOINT1_R.
	FVector BONE_PINKY_JOINT1_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.pinky.joint1, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_PINKY_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_R]->SetPosition(BONE_PINKY_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_R]->SetRotation(BONE_PINKY_JOINT1_R_Rot);

	// Right arm pinky joint2 bone - BONE_PINKY_JOINT2_R.
	FVector BONE_PINKY_JOINT2_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.pinky.joint2, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT1_R_Pos, BONE_PINKY_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_R]->SetPosition(BONE_PINKY_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_R]->SetRotation(BONE_PINKY_JOINT2_R_Rot);

	// Right arm pinky joint3 bone - BONE_PINKY_JOINT3_R.
	FVector BONE_PINKY_JOINT3_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.pinky.joint3, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT2_R_Pos, BONE_PINKY_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_R]->SetPosition(BONE_PINKY_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_R]->SetRotation(BONE_PINKY_JOINT3_R_Rot);

	// Right arm pinky tip bone - BONE_PINKY_TIP_R.
	FVector BONE_PINKY_TIP_R_Pos = CoordinateUtil::GetVector3(handSkeletonData.right.pinky.tip, HMD->GetWorldToMetersScale());
	BONE_PINKY_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT3_R_Pos, BONE_PINKY_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_R]->SetPosition(BONE_PINKY_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_R]->SetRotation(BONE_PINKY_TIP_R_Rot);

}
#pragma endregion Hand Skeleton

#pragma region
void FWaveVRGestureInputDevice::UpdateLeftHandGestureData()
{
	// Update the left gesture.
	prevStaticGestureLeft = currStaticGestureLeft;
	currStaticGestureLeft = EWaveVRStaticGesture::TYPE_INVALID;

	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_Unknown)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_UNKNOWN;
	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_Fist)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_FIST;
	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_Five)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_FIVE;
	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_OK)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_OK;
	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_ThumbUp)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_LIKE;
	if (handGestureData.left == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_POINT;

	if (currStaticGestureLeft != prevStaticGestureLeft)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("UpdateLeftHandGestureData() broadcast left static gesture %d"), (uint8)currStaticGestureLeft);
		UWaveVRStaticGestureComponent::OnStaticGestureNative_Left.Broadcast(currStaticGestureLeft);
	}
}

void FWaveVRGestureInputDevice::UpdateRightHandGestureData()
{
	// Update the right gesture.
	prevStaticGestureRight = currStaticGestureRight;
	currStaticGestureRight = EWaveVRStaticGesture::TYPE_INVALID;

	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_Unknown)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_UNKNOWN;
	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_Fist)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_FIST;
	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_Five)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_FIVE;
	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_OK)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_OK;
	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_ThumbUp)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_LIKE;
	if (handGestureData.right == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_POINT;

	if (currStaticGestureRight != prevStaticGestureRight)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("UpdateRightHandGestureData() broadcast right static gesture %d"), (uint8)currStaticGestureRight);
		UWaveVRStaticGestureComponent::OnStaticGestureNative_Right.Broadcast(currStaticGestureRight);
	}
}
#pragma endregion Hand Gesture

#pragma region
void FWaveVRGestureInputDevice::UpdateParametersFromPawn()
{
	if (!World)
		World = GWorld->GetWorld();
	if (World)
	{
		FString WaveVRPawnName = FString(TEXT("WaveVR_Pawn_C"));
		bool IsWaveVRPawnExist = false;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->IsPendingKill())
			{
				FString str = Actor->GetClass()->GetName();

				if (str == WaveVRPawnName) {
					IsWaveVRPawnExist = true;
					FName CameraHeight = FName(TEXT("CameraHeight"));
					FProperty* Property = Actor->GetClass()->FindPropertyByName(CameraHeight);
					if (Property)
					{
						FVector* ptrValue = Property->ContainerPtrToValuePtr<FVector>(Actor);
						if (ptrValue)
						{
							BONE_OFFSET_L = *ptrValue;
							BONE_OFFSET_R = *ptrValue;
						}
					}
				}
			}
		}
	}
}
#pragma endregion WaveVR Pawn Related Interface

// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#ifndef wvr_hand_h_
#define wvr_hand_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The gesture type
 */
typedef enum {
    WVR_HandGestureType_Invalid     = 0,    /**< The gesture is invalid. */
    WVR_HandGestureType_Unknown     = 1,    /**< Unknow gesture type. */
    WVR_HandGestureType_Fist        = 2,    /**< Represent fist gesture. */
    WVR_HandGestureType_Five        = 3,    /**< Represent five gesture. */
    WVR_HandGestureType_OK          = 4,    /**< Represent OK gesture. */
    WVR_HandGestureType_ThumbUp     = 5,    /**< Represent thumb up gesture. */
    WVR_HandGestureType_IndexUp     = 6,    /**< Represent index up gesture. */
} WVR_HandGestureType;

/**
 * @brief The gesture information
 */
typedef struct WVR_HandGestureData {
    int64_t             timestamp;  /**< The current time in milliseconds. */
    WVR_HandGestureType right;      /**< Gesture type of right hand. */
    WVR_HandGestureType left;       /**< Gesture type of left hand. */
}  WVR_HandGestureData_t;

/**
 * @brief The finger name
 */
typedef enum {
    WVR_FingerType_Thumb    = 1,    /**< Represent thumb finger. */
    WVR_FingerType_Index    = 2,    /**< Represent index finger. */
    WVR_FingerType_Middle   = 3,    /**< Represent middle finger. */
    WVR_FingerType_Ring     = 4,    /**< Represent ring finger. */
    WVR_FingerType_Pinky    = 5     /**< Represent pinky finger. */
} WVR_FingerType;

/**
 * @brief The hand pose type definition
 */
typedef enum {
    WVR_HandPoseType_Invalid  = 0,  /**< The hand pose type is invalid. */
    WVR_HandPoseType_Pinch    = 1,  /**< The hand pose type is pinch. */
} WVR_HandPoseType;

/**
 * @brief The common pose state information
 */
typedef struct WVR_HandPoseStateBase {
    WVR_HandPoseType  type;         /**< The current hand pose type. */
} WVR_HandPoseStateBase_t;

/**
 * @brief The pinch state
 */
typedef struct WVR_HandPosePinchState {
    WVR_HandPoseStateBase_t base;       /**< Refer to @ref WVR_HandPoseStateBase */
    WVR_FingerType          finger;     /**< Move someone finger and thumb apart or bring them together.*/
    float                   strength;   /**< The value of ranges between 0 to 1 for each finger pich, 1 means pinch with the finger touching the thumb fully. */
    WVR_Vector3f_t          origin;     /**< The pinch origin. */
    WVR_Vector3f_t          direction;  /**< The pinch direction. */
} WVR_HandPosePinchState_t;

/**
 * @brief The hand pose state
 */
typedef union WVR_HandPoseState {
    WVR_HandPoseStateBase_t     base;   /**< Refer to @ref WVR_HandPoseStateBase */
    WVR_HandPosePinchState_t    pinch;  /**< Refer to @ref WVR_HandPosePinchState */
} WVR_HandPoseState_t;

/**
 * @brief The hands pose information
 */
typedef struct WVR_HandPoseData {
    int64_t                 timestamp;  /**< The current time in milliseconds. */
    WVR_HandPoseState_t     right;      /**< The pose state of right hand, refer to @ref WVR_HandPoseState. */
    WVR_HandPoseState_t     left;       /**< The pose state of left hand, refer to @ref WVR_HandPoseState. */
} WVR_HandPoseData_t;

/**
 * @brief Information of finger pose
 *
 * Separates a finger into 4 trackable parts.
 */
typedef struct WVR_FingerState {
    WVR_Vector3f_t joint1;  /**< The position value of joint between trapezium and metracarpal in thumb, the position value of joint between metracarpal and proximal in other fingers. */
    WVR_Vector3f_t joint2;  /**< The position value of joint between metracarpal and proximal in thumb, the position value of joint between proximal and intermediate in other fingers. */
    WVR_Vector3f_t joint3;  /**< The position value of joint between proximal and distal in thumb, the position value of joint between intermediate and distal in other fingers. */
    WVR_Vector3f_t tip;     /**< The position dvalueata of finger tip. */
} WVR_FingerState_t;

/**
 * @brief Information for the five fingers
 */
typedef struct WVR_HandSkeletonState {
    WVR_PoseState_t     wrist;      /**< Tracking value of the wrist. */
    WVR_FingerState_t   thumb;      /**< Finger data of the thumb. */
    WVR_FingerState_t   index;      /**< Finger data of the index. */
    WVR_FingerState_t   middle;     /**< Finger data of the middle. */
    WVR_FingerState_t   ring;       /**< Finger data of the ring. */
    WVR_FingerState_t   pinky;      /**< Finger data of the pinky. */
    float               confidence; /**< The hand confidence value. */
} WVR_HandSkeletonState_t;

/**
 * @brief The hand skeleton state
 */
typedef struct WVR_HandSkeletonData {
    WVR_HandSkeletonState_t  right;  /**< Tracking skeleton state of right hand, refer to @ref WVR_HandSkeletonState. */
    WVR_HandSkeletonState_t  left;   /**< Tracking skeleton state of left hand, refer to @ref WVR_HandSkeletonState. */
} WVR_HandSkeletonData_t;

/**
 * @brief Use this function to start hand gesture feature.
 *
 * Use this API to start hand gesture module
 * This API must be called by main thread.
 *
 * @retval WVR_Success start hand gesture feature successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_StartHandGesture();

/**
 * @brief Use this function to stop hand gesture that you already started.
 *
 * Use this API to stop hand gesture that you already started,
 * and release related hand gesture source.
 * This API must be called by main thread.
 * @version API Level 5
*/
extern WVR_EXPORT void WVR_StopHandGesture();

/**
 * @brief Use this function to get gesture data.
 *
 * Use this API to get hand gesture state from the hand gesture module.
 * Use this API must be called by main thread.
 *
 * @param data The hand gesture data refer to @ref WVR_HandGestureData.
 * @retval WVR_Success Successfully retrieved data.
 * @retval others @ref WVR_Result the mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_GetHandGestureData(WVR_HandGestureData_t *data);

/**
 * @brief Use this function to start hand tracking feature.
 *
 * Use this API to start hand tracking module.
 * This API must be called by main thread.
 * @retval WVR_Success start hand tracking feature successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_StartHandTracking();

/**
 * @brief Use this function to stop the hand tracking that was started.
 *
 * Use this API to stop hand tracking that you already started,
 * and release related hand tracking resources.
 * This API must be called by the main thread.
 * @version API Level 5
*/
extern WVR_EXPORT void WVR_StopHandTracking();

/**
 * @brief Use this function to get hand tracking data.
 *
 * Use this API to get hand tracking data from the hand tracking module.
 * This API must be called by the main thread.
 *
 * @param skeleton The hand skeletion state @ref WVR_HandSkeletonData
 * @param pose The hand pose state refer to @ref WVR_HandPoseData
 * @param type Specify the tracking universe of the origin tracking model. Refer to @ref WVR_PoseOriginModel.
 * @retval WVR_Success Successfully retrieved data.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_GetHandTrackingData(WVR_HandSkeletonData_t *skeleton, WVR_HandPoseData_t* pose=nullptr, WVR_PoseOriginModel type=WVR_PoseOriginModel_OriginOnHead);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_hand_h_ */


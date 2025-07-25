// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_device_h_
#define wvr_device_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

static const uint32_t WVR_DEVICE_COUNT_LEVEL_0 = 1;    /**< Get HMD */
static const uint32_t WVR_DEVICE_COUNT_LEVEL_1 = 3;    /**< Get HMD and 2 controllers */
static const uint32_t WVR_DEVICE_COUNT_LEVEL_2 = 16;   /**< Get HMD, 2 controllers, and other miscellaneous tracking devices */
static const uint32_t WVR_DEVICE_HMD = 0;              /**< Specify device HMD */

/**
 * @brief Input type for the input id.
 */
typedef enum {
    WVR_InputType_Button = 1<<0,    /**< Button input type */
    WVR_InputType_Touch  = 1<<1,    /**< Touch input type */
    WVR_InputType_Analog = 1<<2,    /**< Analog input type */
} WVR_InputType;

/**
 * @brief Analog type
 */
typedef enum {
    WVR_AnalogType_None     = 0,
    WVR_AnalogType_2D       = 1,    /**< Provide analog data with x and y coordinate, @ref WVR_Axis value is -1 ~ 1*/
    WVR_AnalogType_1D       = 2,    /**< Provide analog data with x coordinate, @ref WVR_Axis value is 0 ~ 1*/
} WVR_AnalogType;

/**
 * @brief Intensity of the vibration
 */
typedef enum {
    WVR_Intensity_Weak       = 1,   /**< Vibration intensity is very weak. */
    WVR_Intensity_Light      = 2,   /**< Vibration intensity is weak. */
    WVR_Intensity_Normal     = 3,   /**< Vibration intensity is normal. */
    WVR_Intensity_Strong     = 4,   /**< Vibration intensity is strong. */
    WVR_Intensity_Severe     = 5,   /**< Vibration intensity is very strong. */
} WVR_Intensity;

/**
 * @brief Analog data
 */
typedef struct WVR_Axis {
    float x;  /**< -1<=x<=1 for @ref WVR_AnalogType_2D; 0<=x<=1 for @ref WVR_AnalogType_1D. */
    float y;  /**< -1<=y<=1 for @ref WVR_AnalogType_2D; 0 for @ref WVR_AnalogType_1D. */
} WVR_Axis_t;

/**
 * @brief Analog state structure
 */
typedef struct WVR_AnalogState {
    WVR_InputId     id; /**< indicate the analog state belongs to which WVR_InputId */
    WVR_AnalogType  type; /**< indicate the analog data's type */
    WVR_Axis_t      axis; /**< @ref WVR_Axis, include analog data */
} WVR_AnalogState_t;

/**
 * @brief Input attribute for the designated input id.
 */
typedef struct WVR_InputAttribute {
    WVR_InputId id;             /**< The input id of the device. */
    int32_t capability;         /**< Bitmask of @ref WVR_InputType. The value must be from 1 ~ 7 */
    WVR_AnalogType axis_type;   /**< @ref WVR_AnalogType_2D or @ref WVR_AnalogType_1D if analog bit of capability is 1; otherwise, WVR_AnalogType_None */
} WVR_InputAttribute_t;

/**
 * @brief Input mapping pair from the device to the app.
 */
typedef struct WVR_InputMappingPair {
    WVR_InputAttribute destination; /**< Destination key to the app */
    WVR_InputAttribute source;      /**< Source key from the device */
} WVR_InputMappingPair_t;

/**
 * @brief The remaining battery power status.
 */
typedef enum {
    WVR_BatteryStatus_Unknown  = 0, /**< Error code if the device service cannot get remaining battery power status. */
    WVR_BatteryStatus_Normal   = 1, /**< Remaining battery power status is sufficient. */
    WVR_BatteryStatus_Low      = 2, /**< Remaining battery power status is low. */
    WVR_BatteryStatus_UltraLow = 3, /**< Remaining battery power status is really low. */
} WVR_BatteryStatus;

/**
 * @brief The status of battery temperature.
 */
typedef enum {
    WVR_BatteryTemperatureStatus_Unknown       = 0, /**< an error code if device service cannot get the temperature status of the battery. */
    WVR_BatteryTemperatureStatus_Normal        = 1, /**< battery temperature is normal. */
    WVR_BatteryTemperatureStatus_Overheat      = 2, /**< warning, battery temperature is high. */
    WVR_BatteryTemperatureStatus_UltraOverheat = 3, /**< battery temperature is too high. */
} WVR_BatteryTemperatureStatus;

/**
 * @brief The charging status.
 */
typedef enum {
    WVR_ChargeStatus_Unknown     = 0, /**< Error code if the device service cannot get the charging status of the battery in use. */
    WVR_ChargeStatus_Discharging = 1, /**< Device battery is discharging. */
    WVR_ChargeStatus_Charging    = 2, /**< Device battery is charging. */
    WVR_ChargeStatus_Full        = 3, /**< Device battery is full. */
} WVR_ChargeStatus;

/**
 * @brief Recenter effect of WVR_InAppRecenter
 */
typedef enum {
    WVR_RecenterType_Disabled   = 0,  /**< Make everything back to system coordinate instantly */
    WVR_RecenterType_YawOnly = 1, /**< Only adjust the Yaw angle */
    WVR_RecenterType_YawAndPosition = 2, /**< Adjust the Yaw angle, and also reset user's position to Virtual World's center */
    WVR_RecenterType_RotationAndPosition = 3, /**< Affect all the XYZ and Pitch Yaw Roll. It is sensitive to user's head gesture at recentering. */
} WVR_RecenterType;

/**
 * @brief Arm/Neck Model options of WVR_SetArmModel/WVR_SetNeckModel
 */
typedef enum {
    WVR_SimulationType_Auto   = 0,  /**< Simulates device position while device tracking is only 3DoF. */
    WVR_SimulationType_ForceOn = 1, /**< Always simulates and replaces devices' positions. */
    WVR_SimulationType_ForceOff = 2, /**< Do NOT activate Position Simulation. */
} WVR_SimulationType;

/**
 * @brief Lets the developer know if WVR_DeviceType is connected
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @return True means connected; false means disconnected.
 * @version API Level 1
 */
extern WVR_EXPORT bool WVR_IsDeviceConnected(WVR_DeviceType type);

/**
 * @brief Lets the developer know if WVR_DeviceType is in suspend state(HMD only)
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @return True if in suspend state; otherwise returns false.
 * @version API Level 1
 */
extern WVR_EXPORT bool WVR_IsDeviceSuspend(WVR_DeviceType type);

/**
 * @brief Function to get the Input Device Capability for WVR_InputType
 *
 * @param type Indicate what device to get the capability. (refer to @ref WVR_DeviceType)
 * @param inputType One of @ref WVR_InputType.
 * @return int32_t, bitmask for WVR_InputType. A value of -1 means an error has occured.
 * @version API Level 1
*/
extern WVR_EXPORT int32_t WVR_GetInputDeviceCapability(WVR_DeviceType type, WVR_InputType inputType);

/**
 * @brief Function to get input device analog Type for @ref WVR_InputId
 *
 * @param type This parameter specifies the type of device. (refer to @ref WVR_DeviceType)
 * @param id One of @ref WVR_InputId.
 * @return Analog type for @ref WVR_InputId. (refer to @ref WVR_AnalogType)
 * @version API Level 3
 * @note Effective with Runtime version 3 or higher
*/
extern WVR_EXPORT WVR_AnalogType WVR_GetInputDeviceAnalogType(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get the count for WVR_InputType. This is a calcuated WVR_InputType count
 * based on the WVR_SetInputRequest paried result.
 *
 * @param type Indicates what device to get the count. (refer to @ref WVR_DeviceType)
 * @param inputType One of @ref WVR_InputType
 * @return int32_t, count for each WVR_InputType. A value of -1 means an error has occurred.
 * @version API Level 1
*/
extern WVR_EXPORT int32_t WVR_GetInputTypeCount(WVR_DeviceType type, WVR_InputType inputType);

/**
 * @brief Function to get the WVR_InputType data. InputType is a bitmask that represents the combined input types for WVR_InputType.
 *
 * @param type Indicates what device to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @param inputType Bitmask to represent the combined input type for @ref WVR_InputType. Developers can get a single
 * or combined input status.
 * @param buttons Bitmask to represent the button state. Each bit corresponds to WVR_InputId.
 * @param touches Bitmast to represent the touches state. Each bit corresponds to WVR_InputId.
 * @param analogArray An array of  WVR_AnalogState_t to represent all analog data. (refer to @ref WVR_AnalogState)
 * @param analogArrayCount Developer should the the array count from the WVR_GetInputTypeCount API.
 * @return False when any of the parameter @ref WVR_InputType fails to get data. For example, if inputType is a combination of
 * WVR_InputType_Button and WVR_InputType_Analog but the WVR_InputType_Button data was not retrieved, the API will return false.
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_GetInputDeviceState(WVR_DeviceType type, uint32_t inputType, uint32_t* buttons, uint32_t* touches,
                                                   WVR_AnalogState_t* analogArray, uint32_t analogArrayCount);

/**
 * @brief Function to get the button state of a specific input id
 *
 * @param type Indicates that device type. (refer to @ref WVR_DeviceType)
 * @param id One of @ref WVR_InputId. Indicates which button is pressed.
 * @return bool, true for pressed and false is not pressed (unpressed)
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_GetInputButtonState(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get the touch state of a specific input id
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @param id One of @ref WVR_InputId, indicate which touch
 * @return bool, true for touched and false for untouched
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_GetInputTouchState(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get the analog data of a specific input id
 *
 * @param type Indicates the device type. (refer to @ref WVR_DeviceType)
 * @param id One of @ref WVR_InputId. This parameter specifies the physical touch on the device that retrieves the input data.
 * @return Analog data for @ref WVR_InputId. (refer to @ref WVR_Axis)
 * @version API Level 1
*/
extern WVR_EXPORT WVR_Axis_t WVR_GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id);

/**
 * @brief Function to get the VR device pose with or without prediction
 *
 * The pose that the tracker thinks that the HMD will be in at the specified number of seconds into the
 * future. Pass 0 to get the state at the instant the method is called. Most of the time the application should
 * calculate the time until the photons will be emitted from the display and pass that time into the method.
 *
 * For devices where isValidPose is true the application can use the pose to position the device
 * in question.
 *
 * Seated experiences should call this method with **WVR_PoseOriginModel_OriginOnHead** and receive poses relative
 * to the seated zero pose. Standing experiences should call this method with **WVR_PoseOriginModel_OriginOnGround**
 * and receive poses relative to the Arena Play Area. **WVR_PoseOriginModel_OriginOnTrackingObserver** should
 * probably not be used unless the application is the Arena calibration tool itself, but will provide
 * poses relative to the hardware-specific coordinate system in the driver.
 *
 * You will need to explicitly indicate the correct pose with the parameter 'pose' when calling @ref WVR_SubmitFrame.
 *
 * @param type Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @param originModel Tracking universe that returned poses should be relative to.  (refer to @ref WVR_PoseOriginModel)
 * @param predictedMilliSec Number of milliseconds from now to predict poses for. Positive numbers are in the future.
 * Pass 0 to get the state at the instant the function is called.
 * @param poseState Obtain pose data (refer to @ref WVR_PoseState) of tracked device by @ref WVR_DeviceType.
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_GetPoseState(WVR_DeviceType type, WVR_PoseOriginModel originModel, uint32_t predictedMilliSec, WVR_PoseState_t *poseState);

/**
 * @brief Returns poses to usage for rendering scene.
 *
 * This function gets the poses from runtime for the usage of rendering scene at the specific moment based on the pose origin model.
 * The one-time configuring the tracking universe assumes one kind of origin pose models unless the model setting is not changed.
 * The description of tracking universe can refer to the argument originModel of @ref WVR_GetPoseState.
 *
 * The poses and device type are aggregated together as struct WVR_DevicePosePair_t.
 * The struct pointers of all tracked devices compose a specific size array to keep track of the locomotions of the devices.
 * The specific array size can be substituted with three levels of device count.
 * **WVR_DEVICE_COUNT_LEVEL_0** is assumed to only track HMD.
 * **WVR_DEVICE_COUNT_LEVEL_1** is assumed that the HMD and two controllers are tracked.
 * **WVR_DEVICE_COUNT_LEVEL_2** should track the number of devices up to 16, including HMD, 2 controllers, and other miscellaneous tracking devices.
 *
 * You will need to explicitly indicate the correct pose with the parameter 'pose' when calling @ref WVR_SubmitFrame.
 *
 * To reduce judder, the moment of getting poses should approximate the moment update the new scene on display.
 * WVR_GetSyncPose also makes short time prediction of poses according to different render methods such as ATW and direct mode.
 *
 * @param originModel enum @ref WVR_PoseOriginModel. Specify the tracking universe of the origin pose model.
 * @param pairArray struct pointer WVR_DevicePosePair_t (refer to @ref WVR_DevicePosePair). The aggregation array to keep the pose information and tracked device type.
 * @param pairArrayCount uint32_t, specify the size of the WVR_DevicePosePair_t array. The three levels of device count are optional.
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t * pairArray, uint32_t pairArrayCount);

/**
 * @brief Function to trigger vibration.
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @param id Indicates the @ref WVR_InputId to trigger vibration.
 * @param durationMicroSec The vibration duration.
 * @param frequency Number of vibration based on durationMicroSec.
 * @param intensity Intensity of the vibration. The default value is WVR_Intensity_Normal
 * @version API Level 4
 * @note Effective with Runtime version 4 or higher
*/
extern WVR_EXPORT void WVR_TriggerVibration(WVR_DeviceType type, WVR_InputId id = WVR_InputId_Max, uint32_t durationMicroSec = 65535,
                                           uint32_t frequency = 1, WVR_Intensity intensity = WVR_Intensity_Normal);
/**
 * @brief Function to recenter the tracker coordinate system to the current rotation and/or position of the app's "Virtual World".
 *
 * Sets the zero pose for current tracker coordinate system to the current position and rotation of the Virtual World. After
 * @ref WVR_InAppRecenter all @ref WVR_GetSyncPose and @ref WVR_GetPoseState calls that pass @ref WVR_PoseOriginModel
 * as the origin model will be relative to this new zero pose. Take effects until HMD take-off or App switched.
 * @param recenterType indicates type of recenter in @ref WVR_RecenterType
 * @version API Level 2
*/
extern WVR_EXPORT void WVR_InAppRecenter(WVR_RecenterType recenterType);

/**
 * @brief Function to transform between the rotation matrix and quaternion
 *
 *  A convenient utility to apply the specified transform between the rotation matrix and quaternion.
 *
 * @param mat Input rotation matrix. (refer to @ref WVR_Matrix4f)
 * @param quat Input quaternion. (refer to @ref WVR_Quatf)
 * @param m2q True converts the rotation matrix to quaternion, false converts the quaternion to rotation matrix.
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_ConvertMatrixQuaternion(WVR_Matrix4f_t *mat, WVR_Quatf_t *quat, bool m2q);

/**
 * @brief Function to set a parameter string to the device service.
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @param pchValue Set string to let app communicate with the device service.
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_SetParameters(WVR_DeviceType type, const char *pchValue);

/**
 * @brief Function to get a parameter string from the device service.
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @param pchValue Send the string to let the app communicate with the device service.
 * @param retValue The buffer to store the parameter retrieved from the device service. unBufferSize should be the size of this buffer.
 * @param unBufferSize The size of the buffer pointed to by pchValue.
 * @return The number of bytes necessary to hold the string, including the trailing null. Returns 0 (and 0-length string) on failure.
 * @version API Level 1
*/
extern WVR_EXPORT uint32_t WVR_GetParameters(WVR_DeviceType type, const char *pchValue, char *retValue, uint32_t unBufferSize);

/**
 * @brief Function to get the Degrees of Freedom (DoF) of the device's pose.
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType)
 * @return WVR_NumDoF_3DoF means the device provides a 3 DoF pose (rotation only), WVR_NumDoF_6DoF means the device provides 6 DoF pose (rotation and position). (refer to @ref WVR_NumDoF)
 * @version API Level 1
*/
extern WVR_EXPORT WVR_NumDoF WVR_GetDegreeOfFreedom(WVR_DeviceType type);

/**
 * @brief Function to get the default controller role that determines the role of the first controller that connects.
 * When the controller role changes, the server will send an WVR_EventType_DeviceRoleChanged event to notify all applications.
 * During this time, applications need to check the controller roles again with this API(WVR_GetDefaultControllerRole).
 *
 * @return The type of controller role, WVRDeviceType_Controller_Right or WVRDeviceType_Controller_Left. (refer to @ref WVR_DeviceType)
 * @version API Level 1
*/
extern WVR_EXPORT WVR_DeviceType WVR_GetDefaultControllerRole();

/**
 * @brief Get the remaining battery power in percentage, of specified device.
 * @param type Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @return The percentage of the remaining battery power of the specified device.
 * @version API Level 1
 */
extern WVR_EXPORT float WVR_GetDeviceBatteryPercentage(WVR_DeviceType type);

/**
 * @brief Get the status of remaining battery power.
 * @param type Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @return The status of the remaining battery power. Return WVR_BatteryStatus_Unknown if the device service cannot get the remaining battery power. (refer to @ref WVR_BatteryStatus)
 * @version API Level 1
 */
extern WVR_EXPORT WVR_BatteryStatus WVR_GetBatteryStatus(WVR_DeviceType type);

/**
 * @brief Get the current HMD battery charging status.
 * @param type Type Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @return The HMD battery charging status. Return WVR_ChargeStatus_Unknown if the device service cannot get the battery charging status of the HMD. (refer to @ref WVR_ChargeStatus)
 * @version API Level 1
 */
extern WVR_EXPORT WVR_ChargeStatus WVR_GetChargeStatus(WVR_DeviceType type);

/**
 * @brief Get the current temperature status of the battery.
 * @param type : Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @return The temperature status of the battery. Return WVR_BatteryTemperatureStatus_Unknown if the device service cannot get the temperature status of the HMD battery. (refer to @ref WVR_BatteryTemperatureStatus)
 * @version API Level 1
 */
extern WVR_EXPORT WVR_BatteryTemperatureStatus WVR_GetBatteryTemperatureStatus(WVR_DeviceType type);

/**
 * @brief Get the current temperature of battery.
 * @param type Device type to get the InputDeviceState. (refer to @ref WVR_DeviceType)
 * @return The temperature of the battery.
 *          A value of -1 means it is not supported or failed to get the battery temperature.
 * @version API Level 1
 */
extern WVR_EXPORT float WVR_GetBatteryTemperature(WVR_DeviceType type);

/**
 * @brief Function to set the interaction mode of the application.
 *
 * @param mode Indicates @ref WVR_InteractionMode, @ref WVR_InteractionMode_SystemDefault means to reset @ref WVR_InteractionMode and @ref WVR_GazeTriggerType to the default system values.
 * @return bool, true for when setting is successfully.
 * @version API Level 2
 * @note Effective with Runtime version 2 or higher
 */
extern WVR_EXPORT bool WVR_SetInteractionMode(WVR_InteractionMode mode);

/**
 * @brief Function to get the interaction mode of the application.
 *
 * @return @ref WVR_InteractionMode, the interaction mode of the application.
 * @version API Level 2
 * @note Effective with Runtime version 2 or higher
 */
extern WVR_EXPORT WVR_InteractionMode WVR_GetInteractionMode();

/**
 * @brief Function to set the gaze trigger type of the application.
 *
 * @param type Indicate @ref WVR_GazeTriggerType.
 * @return bool, true when set successfully.
 * @version API Level 2
 * @note Effective with Runtime version 2 or higher
 */
extern WVR_EXPORT bool WVR_SetGazeTriggerType(WVR_GazeTriggerType type);

/**
 * @brief Function to get when gaze is triggered in the application.
 *
 * @return @ref WVR_GazeTriggerType is the gaze trigger type of the application.
 * @version API Level 2
 * @note Effective with Runtime version 2 or higher
 */
extern WVR_EXPORT WVR_GazeTriggerType WVR_GetGazeTriggerType();

/**
 * @brief Function to get the current states of device errors from the specified device.
 *
 * @param type Device type to get the InputDeviceState.
 * @return WVR_DeviceErrorState is the bitmask that records all the of current states of device errors that which has occurred on the specified device.
 * @version API Level 3
 * @note Effective with Runtime version 3 or higher
 */
extern WVR_EXPORT bool WVR_GetDeviceErrorState(WVR_DeviceType dev_type, WVR_DeviceErrorStatus error_type);

/**
 * @brief Function to enables or disables use of the neck model for 3-DOF head tracking.
 *
 * @param true to enable neck model for 3-DOF head tracking, false to disable.
 * @version API Level 2
 * @note Effective with Runtime version 2 or higher
 */
extern WVR_EXPORT void WVR_SetNeckModelEnabled(bool enabled);

/**
 * @brief Function to replace the HMD position with a neck model simulation.
 * If one pose is simulated by Neck Model, its is6DoFPose will be always false.
 *
 * @param type Indicates @ref WVR_SimulationType.
 * @version API Level 3
 */
extern WVR_EXPORT void WVR_SetNeckModel(WVR_SimulationType type);

/**
 * @brief Function to replace controllers' with an arm model simulation or not.
 * If one pose is simulated by an arm model, its is6DoFPose will be always false.
 *
 * @param type Indicates @ref WVR_SimulationType.
 * @version API Level 3
 */
extern WVR_EXPORT void WVR_SetArmModel(WVR_SimulationType type);

/**
 * @brief Function to set the behavior of Arm Model simulation. Default NOT follows head/HMD if a controller is steady (stickyArm=false).
 *
 * @param stickyArm force the simulated controllers' positions always follow HMD constantly, even a controller is steady.
 * @version API Level 3
 */
extern WVR_EXPORT void WVR_SetArmSticky(bool stickyArm);

/**
 * @brief Set what input keys are requested by an app.
 * It must be invoked after WVR_Init . If not, @ref WVR_InputEvent, @ref WVR_GetInputDeviceState, @ref WVR_GetInputButtonState, @ref WVR_GetInputTouchState, and @ref WVR_GetInputAnalogAxis cannot get the input event in an app.
 *
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType).
 * @param request Keys used by an app (refer to @ref WVR_InputAttribute).
 * @param size The size of the input buffer pointed by the request parameter.
 * @return True means request is valid; false means request is invalid.
 * @version API Level 3
 * @note  Key mapping policy excludes 2 cases from setting an input request:
 * -# @ref WVR_InputId_Alias1_System is reserved for system use. The app cannot receive a system key event even if you set an input request for it.
 * -# @ref WVR_InputId_Alias1_Volume_Up and @ref WVR_InputId_Alias1_Volume_Down are exclusively used for volume control by the system. Key mapping policy will not let other keys mapping to volume keys. The app can receive volume key events even if you don’t set input request for them.
 */
extern WVR_EXPORT bool WVR_SetInputRequest(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size);

/**
 * @brief To get the input mapping pair for the designated destination key.
 * @param type Indicates the device type. (refer to @ref WVR_DeviceType).
 * @param destination Indicates the input id that the app requests (refer to @ref WVR_InputId).
 * @param pair Outputs the input mapping pair for the destination input id.
 * @return True means the pair is valid; false means the pair is invalid(cannot find a right source mapping to the destination from the device).
 * @version API Level 3
 */
extern WVR_EXPORT bool WVR_GetInputMappingPair(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair);

/**
 * @brief Get the entire input mapping table.
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType).
 * @param table Outputs the entire input mapping table.
 * @param size The allocated buffer size for the table.
 * @return Retrieves the mapping table from the input id of the hardware device to map the software input events needed by an app.
 * @version API Level 3
 */
extern WVR_EXPORT uint32_t WVR_GetInputMappingTable(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size);

/**
 * @brief Function to enable or disable the position and rotation prediction of the pose.
 * @param type Indicates what device type. (refer to @ref WVR_DeviceType).
 * @param enabled_position_predict Enable or disable the position prediction of the pose.
 * @param enabled_rotation_predict Enable or disable the rotation prediction of the pose.
 * @version API Level 4
 * @note You cannot disable the rotation prediction for the HMD.
 * You can pick either the left or right controller to enable/disable the pose prediction of both controllers.
 * By default, the HMD position prediction and controller pose prediction are disabled.
 * This feature depend on the device's capability of supporting pose prediction.
 */
extern WVR_EXPORT void WVR_SetPosePredictEnabled(WVR_DeviceType type, bool enabled_position_predict, bool enabled_rotation_predict);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* wvr_device_h_ */

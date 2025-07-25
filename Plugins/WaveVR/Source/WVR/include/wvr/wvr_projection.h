// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_projection_h_
#define wvr_projection_h_
#include "wvr_types.h"
#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function to get the projection matrix for the specified eye.
 *
 * @param eye Determines which eye the function should return the projection for.
 * @param near Distance, in meters, to the near clip plane.
 * @param far Distance, in meters, to the far clip plane.
 * @return The projection matrix with the @ref WVR_Matrix4f_t type to use for the specified eye.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_Matrix4f_t WVR_GetProjection(WVR_Eye eye, float near, float far);

/**
 * @brief Function to get the boundary of the clipping plane.
 *
 * Returns the coordinates of the margin of the clipping plane for the specified eye.
 * These coordinates can be used to compose a custom matrix. Most content should use @ref WVR_GetProjection
 * but some content may need to do something complicated with its projection.
 * These content can use these coordinates to compute the projection.
 *
 * @param eye Determines which eye the function should return for the corresponding clipping plane.
 * @param left Coordinate for the left margin of the clipping plane.
 * @param right Coordinate for the right margin of the clipping plane.
 * @param top Coordinate for the top margin of the clipping plane.
 * @param bottom Coordinate for the bottom margin of the clipping plane.
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_GetClippingPlaneBoundary(WVR_Eye eye, float *left, float *right, float *top, float *bottom);

/**
 * @brief Function to return the transform from the eye space to the head space.
 *
 * Eye space is respective to the left or right eye. By applying the returned transform matrix,
 * it adjusts the View matrix for each eye, and therefore creates stereo disparity.
 * Instead of the Model View Projection model, the model is Model View Eye Projection.
 * Usually, View and Eye are multiplied together to get the View in your app.
 *
 * This matrix incorporates the user's interpupillary distance (IPD).
 *
 * @param eye Determines which eye the function should return the eye matrix for.
 * @param dof Specify the DoF of the current content. A 6DoF transform considers the eye to head depth but 3DoF does not.
 * @return The transform between the view space and eye space.
 * @version API Level 1
*/
extern WVR_EXPORT WVR_Matrix4f_t WVR_GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof =  WVR_NumDoF_6DoF);

/**
 * @brief Function to get the suggested size of the render target.
 *
 * This API provides the render target size to fit the display.
 *
 * @param width Suggested width for the offscreen render target.
 * @param height Suggested height for the offscreen render target.
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_GetRenderTargetSize(uint32_t* width, uint32_t * height);

/**
 * @brief Function to set the ratio for overfill.
 *
 * The texture and mesh size may be larger than the actual display size
 * and may result in a decrease in the black area during the ATW process.
 * The implementation treats the x and y directions as equal ratios.
 * If the passed in ratioX and ratioY are different, the smaller value will be used.
 * This function should be invoked after the VR runtime initialization and before the render runtime initialization.
 * This sequence helps prepare the prerequisite data in a suitable environment.
 *
 * @param ratioX The scale ratio of width (≥ 1.0).
 * @param ratioY The scale ratio of height (≥ 1.0).
 * @version API Level 3
 */
extern WVR_EXPORT void WVR_SetOverfillRatio(float ratioX, float ratioY);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* wvr_projection_h_ */

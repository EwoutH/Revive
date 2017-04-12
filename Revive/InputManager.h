#pragma once

#include "HapticsBuffer.h"
#include "OVR_CAPI.h"

#include <openvr.h>
#include <thread>
#include <vector>
#include <Windows.h>
#include <Xinput.h>

class InputManager
{
public:
	class InputDevice
	{
	public:
		// Input
		virtual vr::ETrackedControllerRole GetRole() { return vr::TrackedControllerRole_Invalid; }
		virtual ovrControllerType GetType() = 0;
		virtual bool IsConnected() = 0;
		virtual bool GetInputState(ovrSession session, ovrInputState* inputState) = 0;

		// Haptics
		virtual void SetVibration(float frequency, float amplitude) { }
		virtual void SubmitVibration(const ovrHapticsBuffer* buffer) { }
		virtual void GetVibrationState(ovrHapticsPlaybackState* outState) { }
	};

	class OculusTouch : public InputDevice
	{
	public:
		OculusTouch(vr::ETrackedControllerRole role);
		~OculusTouch();

		HapticsBuffer m_Haptics;

		virtual vr::ETrackedControllerRole GetRole() { return m_Role; }
		virtual ovrControllerType GetType();
		virtual bool IsConnected();
		virtual bool GetInputState(ovrSession session, ovrInputState* inputState);
		virtual void SetVibration(float frequency, float amplitude) { m_Haptics.SetConstant(frequency, amplitude); }
		virtual void SubmitVibration(const ovrHapticsBuffer* buffer) { m_Haptics.AddSamples(buffer); }
		virtual void GetVibrationState(ovrHapticsPlaybackState* outState) { *outState = m_Haptics.GetState(); }

	private:
		bool m_bHapticsRunning;
		vr::ETrackedControllerRole m_Role;
		vr::VRControllerState_t m_LastState;

		bool m_StickTouched;
		ovrVector2f m_ThumbStick;
		ovrTouch AxisToTouch(vr::VRControllerAxis_t axis);

		bool m_Gripped;
		double m_GrippedTime;
		bool IsPressed(vr::VRControllerState_t newState, vr::EVRButtonId button);
		bool IsReleased(vr::VRControllerState_t newState, vr::EVRButtonId button);

		std::thread m_HapticsThread;
		static void HapticsThread(OculusTouch* device);
	};

	class OculusRemote : public InputDevice
	{
	public:
		OculusRemote() { }
		~OculusRemote() { }

		virtual ovrControllerType GetType() { return ovrControllerType_Remote; }
		virtual bool IsConnected();
		virtual bool GetInputState(ovrSession session, ovrInputState* inputState);
	};

	class XboxGamepad : public InputDevice
	{
	public:
		XboxGamepad() { }
		~XboxGamepad() { }

		virtual ovrControllerType GetType() { return ovrControllerType_XBox; }
		virtual bool IsConnected();
		virtual bool GetInputState(ovrSession session, ovrInputState* inputState);
		virtual void SetVibration(float frequency, float amplitude);
	};

	InputManager();
	~InputManager();

	unsigned int GetConnectedControllerTypes();
	ovrTouchHapticsDesc GetTouchHapticsDesc(ovrControllerType controllerType);

	ovrResult SetControllerVibration(ovrControllerType controllerType, float frequency, float amplitude);
	ovrResult GetInputState(ovrSession session, ovrControllerType controllerType, ovrInputState* inputState);
	ovrResult SubmitControllerVibration(ovrControllerType controllerType, const ovrHapticsBuffer* buffer);
	ovrResult GetControllerVibrationState(ovrControllerType controllerType, ovrHapticsPlaybackState* outState);

protected:
	std::vector<InputDevice*> m_InputDevices;
};


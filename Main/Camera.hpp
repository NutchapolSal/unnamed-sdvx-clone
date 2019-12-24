#pragma once

/*
	Camera shake effect 
*/
struct CameraShake
{
	CameraShake() = default;
	CameraShake(float duration);
	CameraShake(float duration, float amplitude);
	float amplitude;
	float duration;
	float time = 0.0f;
};

static const float KSM_PITCH_UNIT_PRE_168 = 7.0f;
static const float KSM_PITCH_UNIT_POST_168 = 180.0f / 12;
// Amount of time roll is ignored
static const float FAST_ROLL_IGNORE_TIMER = 0.1;
static const float SLOW_ROLL_IGNORE_TIMER = 0.2;
static const int FAST_ROLL_IGNORE = 0;
static const int SLOW_ROLL_IGNORE = 1;
// Percent of m_rollIntensity where camera rolls at its slowest rate
static const float SLOWEST_TILT_THRESHOLD = 0.1f;
static const float MAX_ROLL_ANGLE = 10.5 / 360.f;
static const float ROLL_SPEED = 3.8;

/*
	Camera that hovers above the playfield track and can process camera shake and tilt effects
*/
class Camera
{
public:
	Camera();
	~Camera();

	// Updates the camera's shake effects, movement, etc.
	void Tick(float deltaTime, class BeatmapPlayback& playback);

	void AddCameraShake(CameraShake camerShake);
	void AddRollImpulse(float dir, float strength);

	// Changes the amount of roll applied when lasers are controlled, default = 1
	void SetRollIntensity(float val);
	void SetRollKeep(bool rollKeep);
	
	/*
	Sets laser slam amount
	@param index - index of the laser. 0 for blue laser, 1 for red laser
	@param amount - the "strength" of the slam. Should be the position of the slam's tail
	@param slowDecay - whether if the other laser's current position is 0 or if the slam's tail position is less than its head
	slowDecay only matters when there is an incoming laser of the same index within 2 beats
	*/
	void SetSlamAmount(uint32 index, float amount, bool slowDecay);
	
	/*
	Sets slow tilt state
	@param tilt - should be true when rollA && rollB == -1 and 1 respectively, or when rollA && rollB == 0
	*/
	void SetSlowTilt(bool tilt);
	void SetLasersActive(bool lasersActive);
	void SetTargetRoll(float target);
	void SetSpin(float direction, uint32 duration, uint8 type, class BeatmapPlayback& playback);
	void SetXOffsetBounce(float direction, uint32 duration, uint32 amplitude, uint32 frequency, float decay, class BeatmapPlayback &playback);
	float GetRoll() const;
	float GetLaserRoll() const;
	float GetActualRoll() const;
	float GetHorizonHeight();
	Vector2i GetScreenCenter();
	Vector3 GetShakeOffset();
	bool GetRollKeep();
	
	/*
	Gets laser slam timer
	@param index - index of the laser. 0 for blue laser, 1 for red laser
	@return the slam timer for the given laser index
	*/
	float GetSlamTimer(uint32 index);

	/*
	Gets laser slam amount
	@param index - index of the laser. 0 for blue laser, 1 for red laser
	@return the slam amount for the given laser index
	*/
	float GetSlamAmount(uint32 index);

	// Gets the spin angle for the background shader
	float GetBackgroundSpin() const { return m_bgSpin; }

	Vector2 Project(const Vector3& pos);

	// Generates a new render state for drawing from this cameras Point of View
	// the clipped boolean indicates whenether to clip the cameras clipping planes to the track range
	RenderState CreateRenderState(bool clipped);

	// The track being watched
	class Track* track;

	// Zoom values, both can range from -1 to 1 to control the track zoom
	float pLaneOffset = 0.0f;
	float pLaneZoom = 0.0f;
	float pLanePitch = 0.0f;
	float pLaneTilt = 0.0f;
	bool pManualTiltEnabled = false;

	float pitchUnit = KSM_PITCH_UNIT_POST_168;

	float cameraShakeX = 0.0f;
	float cameraShakeY = 0.4f;
	float cameraShakeZ = 0.0f;

	// Camera variables Landscape, Portrait
	float basePitch[2] = { 0.f, 0.f };
	float baseRadius[2] = { 0.3f, 0.275f };

	float pitchOffsets[2] = { 0.05f, 0.25f }; // how far from the bottom of the screen should the crit line be
	float fovs[2] = { 60.f, 90.0f };

	Transform worldNormal;
	Transform worldNoRoll;
	Transform critOrigin;

private:
	float m_ClampRoll(float in) const;
	bool m_ShouldRollDuringKeep(float target, float roll);
	// x offset
	float m_totalOffset = 0.0f;
	float m_spinBounceOffset = 0.0f;
	// roll value
	float m_totalRoll = 0.0f;
	float m_laserRoll = 0.0f;
	float m_actualRoll = 0.0f;
	// Target to roll towards
	float m_targetLaserRoll = 0.0f;
	bool m_targetRollSet = false;
	bool m_lasersActive = false;
	// Roll force
	float m_rollVelocity = 0.0f;
	float m_rollIntensity = MAX_ROLL_ANGLE;
	float m_oldRollIntensity = MAX_ROLL_ANGLE;
	// Checks if camera roll needs to roll quickly due to a new tilt value
	bool m_rollIntensityChanged = false;
	float m_rollIntensityChangedTarget = 0.f;
	bool m_rollIntensityChangedTargetSet = false;
	bool m_rollKeep = false;
	bool m_rollKeepChanged = false;

	// Controls if the camera rolls at a slow rate
	// Activates when blue and red lasers are at the extremeties (-1, 1 or 0, 0)
	bool m_slowTilt = false;

	// Laser slam rolls
	// Does not track slams that have a next segment
	float m_slamRoll[2] = { 0.0f };
	// Keeps track of how roll is ignored
	float m_slamRollTimer[2] = { 0.0f };
	int m_slamRollType[2] = { 0 };

	// Spin variables
	int32 m_spinDuration = 1;
	int32 m_spinStart = 0;
	uint8 m_spinType;
	float m_spinDirection = 0.0f;
	float m_spinRoll = 0.0f;
	float m_spinProgress = 0.0f;
	float m_bgSpin = 0.0f;

	float m_spinBounceAmplitude = 0.0f;
	float m_spinBounceFrequency = 0.0f;
	float m_spinBounceDecay = 0.0f;

	float m_actualCameraPitch = 0.0f;

	RenderState m_rsLast;

	CameraShake m_shakeEffect;
	// Base position with shake effects applied after a frame
	Vector3 m_shakeOffset;
};
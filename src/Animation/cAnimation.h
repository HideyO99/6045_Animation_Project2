#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>

struct BoneInfo
{
	std::string name;
	glm::mat4 boneOffset;
	glm::mat4 globalTransformation;
	glm::mat4 finalTransformation; 
};

struct BoneVertexData
{
	BoneVertexData()
	{
		ids[0] = 0;
		ids[1] = 0;
		ids[2] = 0;
		ids[3] = 0;
		weights[0] = 0.f;
		weights[1] = 0.f;
		weights[2] = 0.f;
		weights[3] = 0.f;
	}

	unsigned int ids[4];
	float weights[4];

	void AddBoneInfo(int id, float weight)
	{
		int numIds = sizeof(ids) / sizeof(ids[0]);
		for (int i = 0; i < numIds; i++)
		{
			if (weights[i] == 0.f)
			{
				ids[i] = id;
				weights[i] = weight;
				return;
			}
		}

		assert(0);
	}
};

enum EasingType
{
	EaseIn,
	EaseOut,
	EaseInOut,
	None
};

struct PositionKeyFrame
{
	PositionKeyFrame(glm::vec3 Pos, float time, EasingType type = None) : Pos(Pos), time(time), type(type) {}
	glm::vec3 Pos;
	float time;
	EasingType type;
};

struct ScaleKeyFrame
{
	ScaleKeyFrame(glm::vec3 Scale, float time, EasingType type = None) : Scale(Scale), time(time), type(type) {}
	glm::vec3 Scale;
	float time;
	EasingType type;
};

struct RotationKeyFrame
{
	RotationKeyFrame(glm::quat Rotation, float time, EasingType type = None) : Rotation(Rotation), time(time), type(type) {}
	glm::quat Rotation;
	float time;
	//bool useSlerp;
	EasingType type;
};

struct AnimationData
{
	std::vector<PositionKeyFrame> PositionKeyFrames;
	std::vector<ScaleKeyFrame> ScaleKeyFrames;
	std::vector<RotationKeyFrame> RotationKeyFrames;
	float Duration;
};


class cAnimation
{
public:
	cAnimation();
	~cAnimation();

	std::string tag;
	std::vector<std::string> seq;
	unsigned int curSeq;
	float AnimationTime;
	bool IsPlaying;
	bool IsLooping;
	float Speed;


};

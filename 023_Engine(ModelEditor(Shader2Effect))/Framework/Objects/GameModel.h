#pragma once
#include "GameRender.h"

#include "./Models/ModelMeshPart.h"
#include "./Models/Model.h"
#include "./Models/ModelMesh.h"
#include "./Models/ModelBone.h"

class GameModel : public GameRender
{
public:
	GameModel() {}
	GameModel(wstring effectFile, wstring matFile, wstring meshFile);
	virtual ~GameModel();

	virtual void Update();
	virtual void Render();

	// --------------------------------------------------------------------- //
	//  Model
	// --------------------------------------------------------------------- //
	Model *GetModel() { return model; }		//	Get model datas

	// --------------------------------------------------------------------- //
	//  Name
	// --------------------------------------------------------------------- //
	void Name(wstring name) { this->name = name; }	//	Set the name of the model
	wstring Name() { return name; }					//	Get the name of the model

	// --------------------------------------------------------------------- //
	//  Effect
	// --------------------------------------------------------------------- //
	void SetEffect(wstring fileName);				//	Set the effect of the model by file name
	void SetEffect(string fileName);				//	Set the effect of the model by file name

	void SetDiffuseMap(wstring fileName);			//	Set diffuse map by file name
	void SetSpecularMap(wstring fileName);			//	Set normal map by file name
	void SetNormalMap(wstring fileName);			//	Set specular map by file name
	void SetDetailMap(wstring fileName);			//	Set detail map by file name

	void SetDiffuse(float r, float g, float b, float a);	//	Set diffuse color
	void SetDiffuse(D3DXCOLOR& color);						//	Set diffuse color
	void SetSpecular(float r, float g, float b, float a);	//	Set specular color
	void SetSpecular(D3DXCOLOR& color);						//	Set specular color
	void SetShininess(float val);							//	Set shininess(exp)

	// --------------------------------------------------------------------- //
	//  Load texture file
	// --------------------------------------------------------------------- //
	void LoadDiffuseMapFromFile(wstring fileName);		//	Load diffuse map from file
	void LoadNormalMapFromFile(wstring fileName);		//	Load normal map from file
	void LoadSpecularMapFromFile(wstring fileName);		//	Load specular map from file

	// --------------------------------------------------------------------- //
	//  Colliders
	// --------------------------------------------------------------------- //
	//DebugLine * FindLine(wstring name);		//	Get collider by its name
	//void LineInit(DebugLine *collider);		//	Initialize colliders
	//void LineUpdate(wstring name);			//	Update collider
	//void LineUpdate();						//	Update collider
	//void LineRender();						//	Render collider
	//void LineDelete();						//	Delete collider

protected:
	void UpdateWorld() override;

protected:
	wstring				name;				//	Name of the model
	Model				*model;				//	Pointer to the model

	vector<D3DXMATRIX>	transforms;			//	Matrix of all bones

											//vector<ILine *>		colliderBoxes;
											//vector<DebugLine *>	colliderBoxes;		//	Collider boxes
											//DebugLine			*pickCollider;		//	Collider box for picking model

	//public:
	/*void SetRim(int val, D3DXCOLOR color)
	{
	rimBuffer->Data.Rim = val;
	rimBuffer->Data.RimColor = color;
	}*/

	//private:

	//class RimBuffer : public ShaderBuffer
	//{
	//public:
	//	RimBuffer() : ShaderBuffer(&Data, sizeof(Data))
	//	{
	//		Data.RimColor = D3DXCOLOR(220.0f / 255.0f, 133.0f / 255.0f, 15.0f / 255.0f, 1);
	//		Data.Rim = 0;
	//	}

	//	struct Struct
	//	{
	//		D3DXCOLOR RimColor;
	//		int Rim;

	//		float Padding[3];
	//	} Data;
	//};
	//RimBuffer* rimBuffer;
};
#include "Framework.h"
#include "Terrain.h"
#include "HeightMap.h"
#include "TerrainRenderer.h"
#include "Systems\Window.h"
#include "Utilities/Xml.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc & desc)
	: desc(desc)
	, isPick(false)
	, terrainName(L"")
	, render(NULL)
	, heightMap(NULL)
	, loadDataFile(L"")
{
	for (int i = 0; i < 4; i++)
		materialValueFile[i] = L"";
}

Terrain::~Terrain()
{
	SAFE_DELETE(render);
	SAFE_DELETE(heightMap);
	SAFE_DELETE(desc.material);

	SAFE_DELETE(materialMap.detailMap);
	SAFE_DELETE(materialMap.diffuseMap);
	SAFE_DELETE(materialMap.normalMap);
	SAFE_DELETE(materialMap.specularMap);
}

void Terrain::Initialize(ScatterSky * sky)
{
	desc.material = new Material(Effects + L"022_Terrain.fx");
	desc.material->ChangeMaterialDesc(&colorDesc);
	desc.material->ChangeMaterialMap(&materialMap);

	if (render == NULL)
		render = new TerrainRenderer(this);

	if (desc.HeightMap != L"")
	{
		if (heightMap == NULL)
			heightMap = new HeightMap((UINT)desc.HeightMapWidth, (UINT)desc.HeightMapHeight, desc.HeightScale);

		heightMap->Load(desc.HeightMap);
		render->Initialize();
	}

	render->GetSky(sky);
}

void Terrain::Ready()
{
	if (materialValueFile[0] != L"")
		desc.material->SetDiffuseMap(materialValueFile[0]);

	if (materialValueFile[1] != L"")
		desc.material->SetSpecularMap(materialValueFile[1]);

	if (materialValueFile[2] != L"")
		desc.material->SetNormalMap(materialValueFile[2]);

	if (materialValueFile[3] != L"")
		desc.material->SetDetailMap(materialValueFile[3]);
	

	render->Ready(desc.material);
}



void Terrain::Update()
{
}

void Terrain::Render()
{

	render->Render();
}


void Terrain::LineUpdate()
{
	desc.material->GetShader()->AsScalar("LineType")->SetInt(tline.LineType);
	desc.material->GetShader()->AsVector("LineColor")->SetFloatVector(tline.LineColor);
	desc.material->GetShader()->AsScalar("Spacing")->SetInt(tline.Spacing);
	desc.material->GetShader()->AsScalar("Thickness")->SetFloat(tline.Thickness);
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * Width()) / desc.CellSpacing;
	float d = (z - 0.5f * Depth()) / desc.CellSpacing;
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float h00 = heightMap->Data(row, col);
	float h01 = heightMap->Data(row, col + 1);
	float h10 = heightMap->Data(row + 1, col);
	float h11 = heightMap->Data(row + 1, col + 1);

	float s = c - col;
	float t = d - row;

	if (s + t <= 1.0f)
	{
		float uy = h01 - h00;
		float vy = h01 - h11;
		return h00 + (1.0f - s) * uy + (1.0f - t) * vy;
	}
	else
	{
		float uy = h10 - h11;
		float vy = h01 - h11;
		return h11 + (1.0f - s) * uy + (1.0f - t) * vy;
	}
}

bool Terrain::Picking(OUT D3DXVECTOR3 * pickPixel, OUT D3DXVECTOR3 * pickWorldPos)
{
	float x, z;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXVECTOR4 result = Texture::ReadPixel128(render->GetRenderTargetTexture(), (UINT)position.x, (UINT)position.y);
	x = result.x;
	z = result.y;

	if (pickPixel != NULL)
	{
		pickPixel->x = x * (desc.HeightMapWidth - 1.0f);
		pickPixel->z = z * (desc.HeightMapHeight - 1.0f);
	}

	if (pickWorldPos != NULL)
	{
		pickWorldPos->x = x * Width() - (Width() * 0.5f);
		pickWorldPos->z = z * Depth() - (Depth() * 0.5f);
	}

	return x + z > 0;
}

bool Terrain::IsPicked()
{
	return isPick;
}

void Terrain::SetPickState(bool val)
{
	isPick = val;
}


void Terrain::SetShader(D3DXMATRIX v, D3DXMATRIX p)
{
	desc.material->GetShader()->AsMatrix("LightView")->SetMatrix(v);
	desc.material->GetShader()->AsMatrix("LightProjection")->SetMatrix(p);
}

void Terrain::SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv)
{
	desc.material->GetShader()->AsMatrix("ShadowTransform")->SetMatrix(shadowTransform);

	if (srv != NULL)
		desc.material->GetShader()->AsShaderResource("ShadowMap")->SetResource(srv);
}

void Terrain::ShadowUpdate()
{

}

void Terrain::NormalRender()
{
	render->Render();
}

void Terrain::ShadowRender(UINT tech, UINT pass)
{
	render->RenderShadow(tech, pass);
}

wstring & Terrain::Name()
{
	return terrainName;
}

wstring & Terrain::Tag()
{
	return tag;
}

void Terrain::Load(wstring fileName)
{
	GameTerrainSpec * spec = NULL;

	if (loadDataFile == fileName)
		spec = (GameTerrainSpec*)GameDataSpecManager::Get()->Find(loadDataFile);
	else
		spec = (GameTerrainSpec*)GameDataSpecManager::Get()->LoadTerrain(fileName);

	desc.HeightMap = spec->HeightMap;
	desc.HeightScale = spec->HeightScale;
	desc.HeightMapWidth = spec->HeightMapWidth;
	desc.HeightMapHeight = spec->HeightMapHeight;
	for (int i = 0; i < 3; i++)
		desc.layerMapFile[i] = spec->LayerMapFile[i];
	desc.blendHeight = spec->BlendHeight;
	desc.activeBlend = spec->ActiveBlend;
	detail.DetailValue = spec->DetailValue;
	detail.DetailIntensity = spec->DetailIntensity;
	desc.material->SetAmbient(spec->AmbientColor);
	for (int i = 0; i < 4; i++)
		materialValueFile[i] = spec->MaterialValueFile[i];
	desc.material->SetDiffuse(spec->DiffuseColor);
	D3DXCOLOR specular = spec->SpecularColor;
	specular.a = spec->Shininess;
	desc.material->SetSpecular(specular);
	render->GetBuffer().TexScale = spec->TexScale;
	render->GetBuffer().FogStart = spec->FogStart;
	render->GetBuffer().FogRange = spec->FogRange;

	if (heightMap == NULL)
		heightMap = new HeightMap((UINT)desc.HeightMapWidth, (UINT)desc.HeightMapHeight, desc.HeightScale);

	heightMap->Load(desc.HeightMap);
	render->Initialize();

	loadDataFile = fileName;
}

void Terrain::Save()
{
	GameTerrainSpec * spec = new GameTerrainSpec();
	spec->HeightMap = desc.HeightMap;
	spec->HeightScale = desc.HeightScale;
	spec->HeightMapWidth = desc.HeightMapWidth;
	spec->HeightMapHeight = desc.HeightMapHeight;
	for (int i = 0; i < 3; i++)
		spec->LayerMapFile[i] = desc.layerMapFile[i];
	spec->BlendHeight = desc.blendHeight;
	spec->ActiveBlend = desc.activeBlend;
	spec->DetailValue = detail.DetailValue;
	spec->DetailIntensity = detail.DetailIntensity;
	spec->AmbientColor = desc.material->GetAmbient();
	for (int i = 0; i < 4; i++)
		spec->MaterialValueFile[i] = materialValueFile[i];
	spec->DiffuseColor = desc.material->GetDiffuse();
	spec->SpecularColor = desc.material->GetSpecular();
	spec->Shininess = desc.material->GetSpecular().a;
	spec->TexScale = render->GetBuffer().TexScale;
	spec->FogStart = render->GetBuffer().FogStart;
	spec->FogRange = render->GetBuffer().FogRange;

	GameDataSpecManager::Get()->WriteTerrain(terrainName, spec);
	SAFE_DELETE(spec);
}

void Terrain::Delete()
{
	if (this != NULL)
	{
		delete this;
	}
	//SAFE_DELETE(render);
	//SAFE_DELETE(heightMap);
}
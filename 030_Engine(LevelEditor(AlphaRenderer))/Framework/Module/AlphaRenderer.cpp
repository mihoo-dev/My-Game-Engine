#include "Framework.h"
#include "AlphaRenderer.h"
#include "Interfaces/IAlpha.h"

AlphaRenderer * AlphaRenderer::instance = NULL;
void AlphaRenderer::Create()
{
	if (instance != NULL)
		return;

	instance = new AlphaRenderer();
}

void AlphaRenderer::Delete()
{
	if (instance == NULL)
		return;

	SAFE_DELETE(instance);
}

AlphaRenderer * AlphaRenderer::Get()
{
	return instance;
}

AlphaRenderer::AlphaRenderer()
{
	alphaMaps.clear();
	alphaMapIter = alphaMaps.begin();
}

AlphaRenderer::~AlphaRenderer()
{
}

void AlphaRenderer::Render()
{
	for (IAlpha * alpha : alphaMaps)
		alpha->Render();
}

void AlphaRenderer::AddAlphaMaps(IAlpha * alphas)
{
	alphaMapIter = alphaMaps.insert(alphaMapIter, alphas);
}


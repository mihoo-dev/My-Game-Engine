#pragma once

class RenderTargetView
{
public:
	RenderTargetView(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTargetView();

	ID3D11Texture2D* Texture() { return texture; }
	ID3D11ShaderResourceView* SRV() { return srv; }
	ID3D11RenderTargetView* RTV() { return rtv; }
	ID3D11UnorderedAccessView* UAV() { return uav; }

private:
	UINT width, height;
	DXGI_FORMAT format;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
	ID3D11UnorderedAccessView* uav;
};
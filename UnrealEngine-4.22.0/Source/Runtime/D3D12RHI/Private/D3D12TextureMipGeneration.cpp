#include "D3D12RHIPrivate.h"

class FDX12MipGenerationCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FDX12MipGenerationCS, Global);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FDX12MipGenerationCS()
	{}

	explicit FDX12MipGenerationCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		MipGenerationParamsParameters.Bind(Initializer.ParameterMap, TEXT("MipGenerationParams"));
		RWTexture0Parameter.Bind(Initializer.ParameterMap, TEXT("Texture0"));
		RWTexture1Parameter.Bind(Initializer.ParameterMap, TEXT("Texture1"));
	}

	bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

		Ar << MipGenerationParamsParameters;
		Ar << RWTexture0Parameter;
		Ar << RWTexture1Parameter;

		return bShaderHasOutdatedParameters;
	}

	void SetParameters(FRHICommandList& RHICmdList)
	{
		//FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
	}

	void SetUAVs(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIParamRef UAV0, FUnorderedAccessViewRHIParamRef UAV1)
	{
		FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

		FUnorderedAccessViewRHIParamRef UAVs[] = {
			UAV0,
			UAV1,
		};

		RHICmdList.TransitionResources(EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, UAVs, 2);
		RWTexture0Parameter.SetTexture(RHICmdList, ComputeShaderRHI, 0, UAV0);
		RWTexture1Parameter.SetTexture(RHICmdList, ComputeShaderRHI, 0, UAV1);
	}

	void UnsetUAVs(FRHICommandList& RHICmdList)
	{
		FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
		RWTexture0Parameter.UnsetUAV(RHICmdList, ComputeShaderRHI);
		RWTexture1Parameter.UnsetUAV(RHICmdList, ComputeShaderRHI);
	}

private:
	FShaderParameter MipGenerationParamsParameters;
	FRWShaderParameter RWTexture0Parameter;
	FRWShaderParameter RWTexture1Parameter;
};

IMPLEMENT_SHADER_TYPE(, FDX12MipGenerationCS, TEXT("/Engine/Private/DX12TextureMipGeneration.usf"), TEXT("GenerateMipsMain"), SF_Compute);

void DX12GenerateMips(class FRHICommandListImmediate& RHICmdList, FTextureRHIParamRef TextureRHI)
{
	if (!TextureRHI)
		return;

	const int BlockSize = 8;

	// We have support only for 2d textures, now
	FRHITexture2D* TextureRHI2D = TextureRHI->GetTexture2D();
	if (!TextureRHI2D)
		return;

	// Need access to UAVs
	FD3D12TextureBase* TextureD3D12 = GetD3D12TextureFromRHITexture(TextureRHI);
	if (!TextureD3D12)
		return;

	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FDX12MipGenerationCS> ComputeShader(ShaderMap);

	RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());

	// We have nNumMips, but we need generate nNumMips - 1
	uint32 nNumMips = TextureRHI->GetNumMips();
	FIntVector SizeXYZ = TextureRHI->GetSizeXYZ();
	for (uint32 i = 0; i < nNumMips - 1; ++i)
	{
		FD3D12UnorderedAccessView* UAV0 = TextureD3D12->GetUnorderedAccessView(i, 0);
		FD3D12UnorderedAccessView* UAV1 = TextureD3D12->GetUnorderedAccessView(i + 1, 0);

		ComputeShader->SetUAVs(RHICmdList, UAV0, UAV1);

		uint32 MipWidth = SizeXYZ.X / FMath::Pow(2, i + 1);
		uint32 MipHeight = SizeXYZ.Y / FMath::Pow(2, i + 1);
		
		uint32 GroupSizeX = (MipWidth + BlockSize - 1) / BlockSize;
		uint32 GroupSizeY = (MipHeight + BlockSize - 1) / BlockSize;
		DispatchComputeShader(RHICmdList, *ComputeShader, GroupSizeX, GroupSizeY, 1);

		ComputeShader->UnsetUAVs(RHICmdList);
	}
}
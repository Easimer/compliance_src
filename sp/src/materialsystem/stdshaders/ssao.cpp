// ----------------------------------------------------------------------------
// MYSHADER.CPP
//
// This file defines the C++ component of the example shader.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Must include this. Contains a bunch of macro definitions along with the
// declaration of CBaseShader.
#include "BaseVSShader.h"

// We're going to be making a screenspace effect. Therefore, we need the
// screenspace vertex shader.
#include "SDK_screenspaceeffect_vs20.inc"

// We also need to include the pixel shader for our own shader.
// Note that the shader compiler generates both 2.0 and 2.0b versions.
// Need to include both.
#include "chromatic_aberration_ps20.inc"
#include "chromatic_aberration_ps20b.inc"

// ----------------------------------------------------------------------------
// This macro defines the start of the shader. Effectively, every shader is
// 
// ----------------------------------------------------------------------------
BEGIN_SHADER(GlobalIllumination, "Screen space shadows")

// ----------------------------------------------------------------------------
// This block is where you'd define inputs that users can feed to your
// shader.
// ----------------------------------------------------------------------------
	BEGIN_SHADER_PARAMS
		SHADER_PARAM(FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_FullFrameFB", "")
		SHADER_PARAM(SUN_ANGLE, SHADER_PARAM_TYPE_VEC2, "[0.0 1.0]", "")
	END_SHADER_PARAMS

// ----------------------------------------------------------------------------
// This is the shader initialization block. This disgusting macro defines
// a bunch of ick that makes this shader work.
// ----------------------------------------------------------------------------
	SHADER_INIT
	{
		if (params[FBTEXTURE]->IsDefined())
		{
			LoadTexture(FBTEXTURE);
		}
		if (!params[SUN_ANGLE]->IsDefined())
		{
			params[SUN_ANGLE]->SetVecValue(0, 1);
		}
	}

// ----------------------------------------------------------------------------
// We want this shader to operate on the frame buffer itself. Therefore,
// we need to set this to true.
// ----------------------------------------------------------------------------
	bool NeedsFullFrameBufferTexture(IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */) const
	{
		return true;
	}

// ----------------------------------------------------------------------------
// This block should return the name of the shader to fall back to if
// we fail to bind this shader for any reason.
// ----------------------------------------------------------------------------
	SHADER_FALLBACK
	{
		// Requires DX9 + above
		if (g_pHardwareConfig->GetDXSupportLevel() < 90)
		{
			Assert(0);
			return "Wireframe";
		}
	return 0;
	}

// ----------------------------------------------------------------------------
// This implements the guts of the shader drawing code.
// ----------------------------------------------------------------------------
	SHADER_DRAW
	{
		// ----------------------------------------------------------------------------
		// This section is called when the shader is bound for the first time.
		// You should setup any static state variables here.
		// ----------------------------------------------------------------------------
		SHADOW_STATE
		{
			pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
			// Setup the vertex format.
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat(fmt, 1, 0, 0);

			// We don't need to write to the depth buffer.
			pShaderShadow->EnableDepthWrites(true);

			float c0[2];

			params[SUN_ANGLE]->GetVecValue(c0, 2);
			pShaderAPI->SetPixelShaderConstant(1, c0, ARRAYSIZE(c0));

			// Precache and set the screenspace shader.
			DECLARE_STATIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);
			SET_STATIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);

			// Precache and set the example shader.
			if (g_pHardwareConfig->SupportsPixelShaders_2_b())
			{
				DECLARE_STATIC_PIXEL_SHADER(chromatic_aberration_ps20b);
				SET_STATIC_PIXEL_SHADER(chromatic_aberration_ps20b);
			}
			else
			{
				DECLARE_STATIC_PIXEL_SHADER(chromatic_aberration_ps20);
				SET_STATIC_PIXEL_SHADER(chromatic_aberration_ps20);
			}
		}

	// ----------------------------------------------------------------------------
	// This section is called every frame.
	// ----------------------------------------------------------------------------
		DYNAMIC_STATE
		{
			BindTexture(SHADER_SAMPLER0, FBTEXTURE, -1);
			// Use the sdk_screenspaceeffect_vs20 vertex shader.
			DECLARE_DYNAMIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);
			SET_DYNAMIC_VERTEX_SHADER(sdk_screenspaceeffect_vs20);

			// Use our custom pixel shader.
			if (g_pHardwareConfig->SupportsPixelShaders_2_b())
			{
				DECLARE_DYNAMIC_PIXEL_SHADER(chromatic_aberration_ps20b);
				SET_DYNAMIC_PIXEL_SHADER(chromatic_aberration_ps20b);
			}
			else
			{
				DECLARE_DYNAMIC_PIXEL_SHADER(chromatic_aberration_ps20);
				SET_DYNAMIC_PIXEL_SHADER(chromatic_aberration_ps20);
			}
		}

			// NEVER FORGET THIS CALL! This is what actually
			// draws your shader!
		Draw();
	}

END_SHADER
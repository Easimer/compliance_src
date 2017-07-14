#include "cbase.h"
#include "materialsystem\itexture.h"
#include "materialsystem/IMaterialProxy.h"
#include "materialsystem/IMaterialVar.h"
#include "pixelwriter.h"
#include "usermessages.h"
#include "dss.h"
#include "fmtstr.h"

#include "memdbgon.h"

/*
00:00 - 04:00 Night
04:00 - 07:00 Dawn
07:00 - 10:00 Morning
10:00 - 17:00 Day
17:00 - 21:00 Twilight
21:00 - 00:00 Night

0.000000000	-	0.166666667	Night
0.166666667	-	0.291666667	Dawn
0.291666667	-	0.416666667	Morning
0.416666667	-	0.708333333	Day
0.708333333	-	0.875000000	Twilight
0.875000000	-	1.000000000	Night

These blend with the prev/next state when they're in the first/last quarter of their phase.

// Night: RGB(5,21,69)
// Dawn: RGB(37,55,95)
// Morning: RGB(91,201,255)
// Day: RGB(155,232,255)
// Twilight: RGB(37,55,95)
// Night: RGB(5,21,69)

Twilight = Dawn on the opposite skybox side
*/
/*
#define DSKY_NIGHT 5,21,69
#define DSKY_DAWN 37,55,95
#define DSKY_MORNING 91,201,255
#define DSKY_DAY 155,232,255
#define DSKY_TWILIGHT DSKY_DAWN
*/
#define DSKY_DAWN 200,64.3,50
#define DSKY_MORNING 200,64.3,70
#define DSKY_DAYLIGHT 200,64.3,100
#define DSKY_EVENING 200,64.3,50
#define DSKY_NIGHT 200,64.3,30

#define DSKY_NIGHT_START	0.000000000
#define DSKY_NIGHT_END		0.166666667
#define DSKY_DAWN_START		DSKY_NIGHT_END
#define DSKY_DAWN_END		0.291666667
#define DSKY_MORNING_START	DSKY_DAWN_END
#define DSKY_MORNING_END	0.416666667
#define DSKY_DAY_START		DSKY_MORNING_END
#define DSKY_DAY_END		0.708333333
#define DSKY_TWILIGHT_START	DSKY_DAY_END
#define DSKY_TWILIGHT_END	0.875000000
#define DSKY_EVENING_START	DSKY_TWILIGHT_END
#define DSKY_EVENING_END	1.000000000

enum eSkyboxSide {
	BACK, DOWN, FRONT, LEFT, RIGHT, UP
};

skyinfo_t g_SkyInfo;

void __MsgFunc_UpdateSky(bf_read& msg)
{
	g_SkyInfo.flLastTimeOfDay = g_SkyInfo.flTimeOfDay;
	g_SkyInfo.flTimeOfDay = msg.ReadFloat();
}

#define M_EULER (0.5772156649015328606065120900824024310421)

double dss_magic_function(double x)
{
	double y0 = 29.69497 + 14.89158*x - 204.1242*pow(x, 2) + 3412.985*pow(x, 3) - 6694.867*pow(x, 5) + 3472.85*pow(x, 5);
	//double y1 = pow(97.26614*M_EULER, (-pow((x - 0.530894), 2) / (2 * 0.2701742 * 0.2701742)));
	return y0;
}

class CSkyGenerator : public ITextureRegenerator {
public:
	CSkyGenerator(eSkyboxSide iSide);
	virtual void RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect);
	virtual void Release(void);
private:
	void GenerateStars(int width, int height);
	bool NeedsStars();
private:
	EHANDLE m_pDynSkyCtl;
	eSkyboxSide m_iSkyboxSide;

	byte* m_pStarBuf;

};

CSkyGenerator::CSkyGenerator(eSkyboxSide iSide)
{
	m_pStarBuf = NULL;
	m_iSkyboxSide = iSide;
	DevMsg("CSkyGenerator::constr\n");
	usermessages->HookMessage("UpdateSky", __MsgFunc_UpdateSky);
}

bool CSkyGenerator::NeedsStars()
{
	return m_iSkyboxSide != DOWN;
}

void CSkyGenerator::GenerateStars(int width, int height)
{
	m_pStarBuf = (byte*)malloc(width * height * sizeof(byte));
	Assert(m_pStarBuf);
	memset(m_pStarBuf, 0, width * height * sizeof(byte));
	for (int y = 0; y < height; y+=4)
	{
		for (int x = 0; x < width; x += 4)
		{
			float r = rand();
			if (r < 0.1)
			{
				for (int y1 = 0; y1 < 4; y1++)
					for (int x1 = 0; x1 < 4; x1++)
					{
						m_pStarBuf[(y+y1) * width + x + x1] = 1;
					}
			}
		}
	}
}

void CSkyGenerator::RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect)
{
	CPixelWriter pixelWriter;
	pixelWriter.SetPixelMemory(pVTFTexture->Format(),
		pVTFTexture->ImageData(0, 0, 0), pVTFTexture->RowSizeInBytes(0));
	int xmax = pSubRect->x + pSubRect->width;
	int ymax = pSubRect->y + pSubRect->height;
	//int x, y;
	/*for (y = pSubRect->y; y < ymax; ++y)
	{
		pixelWriter.Seek(pSubRect->x, y);
		for (x = pSubRect->x; x < xmax; ++x)
		{
			pixelWriter.WritePixel(y % 256, 0, 0, 255);
		}
	}*/

	int nTotalWidth = pTexture->GetActualWidth();
	if (NeedsStars() && !m_pStarBuf)
		GenerateStars(nTotalWidth, pTexture->GetActualHeight());

	float flTimeOfDay = g_SkyInfo.flTimeOfDay;

	Color skycol;

	Vector hsv, rgb;
	rgb.Init(155, 232, 255);
	RGBtoHSV(rgb, hsv);
	float lofasz = sin(flTimeOfDay * (M_PI)) * 0.8 + 0.2;
	hsv.z = lofasz * 100;
	// X is hue
	// Y is saturation
	// Z is value
	HSVtoRGB(hsv, rgb);

	//DevMsg("Lofasz: %f\n", lofasz);

	int x, y;
	for (y = pSubRect->y; y < ymax; ++y)
	{
		pixelWriter.Seek(pSubRect->x, y);
		for (x = pSubRect->x; x < xmax; ++x)
		{
			pixelWriter.WritePixel(rgb.x, rgb.y, rgb.z);
			if (m_pStarBuf)
			{
				if (m_pStarBuf[y * nTotalWidth + x])
				{
					pixelWriter.WritePixel(255, 255, 255, max(min(0, (lofasz - 0.30)) * 2.2222222222, 1));
				}
			}
		}
	}
}

void CSkyGenerator::Release()
{
	DevMsg("CSkyGenerator::Release\n");
	if (m_pStarBuf)
		free(m_pStarBuf);
	delete this;
}

class CSkyMatProxy : public IMaterialProxy {
public:
	CSkyMatProxy();
	virtual ~CSkyMatProxy();
	virtual bool Init(IMaterial* pMaterial, KeyValues* pKv);
	virtual void OnBind(void *pEnt);
	virtual void Release(void) { delete this; }
	virtual IMaterial *GetMaterial() { return m_pTextureVar->GetOwningMaterial(); }
private:
	void InitSkyboxSide(KeyValues* pKv);
private:
	IMaterialVar		*m_pTextureVar;   // The material variable
	ITexture		*m_pTexture;      // The texture
	ITextureRegenerator	*m_pTextureRegen; // The regenerator
	eSkyboxSide m_iSkyboxSide;
};

CSkyMatProxy::CSkyMatProxy()
{
	m_pTextureVar = NULL;
	m_pTexture = NULL;
	m_pTextureRegen = NULL;
}

CSkyMatProxy::~CSkyMatProxy()
{
	if (m_pTexture)
		m_pTexture->SetTextureRegenerator(NULL);
	DevMsg("CSkyMatProxy::destr\n");
}

bool CSkyMatProxy::Init(IMaterial* pMaterial, KeyValues* pKv)
{
	DevMsg("CSkyMatProxy::Init\n");
	bool bFound;

	m_pTextureVar = pMaterial->FindVar("$basetexture", &bFound, false);
	if (!bFound)
	{
		m_pTextureVar = NULL;
		return false;
	}

	InitSkyboxSide(pKv);

	m_pTexture = m_pTextureVar->GetTextureValue();
	if (m_pTexture != NULL)
	{
		m_pTextureRegen = new CSkyGenerator(m_iSkyboxSide);  // Here we create our regenerator
		m_pTexture->SetTextureRegenerator(m_pTextureRegen); // And here we attach it to the texture.
	}
	return true;
}

void CSkyMatProxy::OnBind(void *pEntity)
{
	if (!m_pTexture)  // Make sure we have a texture
		return;

	if (!m_pTextureVar->IsTexture())  // Make sure it is a texture
		return;
	m_pTexture->Download(); // Force the regenerator to redraw
}

void CSkyMatProxy::InitSkyboxSide(KeyValues* pKv)
{
	string_t szOrientation = MAKE_STRING(pKv->GetString("orient", "back"));
	if (!szOrientation)
		m_iSkyboxSide = BACK;
	if (strcmp(szOrientation, "back") == 0)
		m_iSkyboxSide = BACK;
	else if (strcmp(szOrientation, "down") == 0)
		m_iSkyboxSide = DOWN;
	else if (strcmp(szOrientation, "front") == 0)
		m_iSkyboxSide = FRONT;
	else if (strcmp(szOrientation, "left") == 0)
		m_iSkyboxSide = LEFT;
	else if (strcmp(szOrientation, "right") == 0)
		m_iSkyboxSide = RIGHT;
	else
		m_iSkyboxSide = UP;
}

EXPOSE_INTERFACE(CSkyMatProxy, IMaterialProxy, "DynamicSkyRenderer" IMATERIAL_PROXY_INTERFACE_VERSION);
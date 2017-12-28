#pragma once

void PostFXInit();
void PostFXDeinit();
void PostFXPerform(int x, int y, int width, int height);

class postfx {
public:
	postfx(const char* szName);
	~postfx();
	postfx(const postfx&) = delete;
	void operator=(const postfx&) = delete;
	void apply(int x, int y, int w, int h);
private:
	IMaterial* m_pMaterial;
};

extern CUtlLinkedList<postfx*>* postfx_list;

#define DECLARE_POSTFX(name) postfx_list->AddToHead(new postfx(name))

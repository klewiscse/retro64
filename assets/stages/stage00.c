#include <nusys.h>
#include "../../config.h"
#include "../sprites/IGLogo32.h"
#include "../sprites/IGMainWhite200.h"
#include "../sprites/dGasm.h"
#include "../sprites/dOof.h"
#include "../sprites/kappaRide.h"
#include "../sprites/fnt_text.h"
#include "../sprites/fnt_text_outlined.h"
#include "../sprites/fnt_numbers.h"
#include "../sprites/fnt_numbers_outlined.h"
#include "definitions.h"

static u8 r;
static u8 g;
static u8 b;
static int pos_y;
static int scene;
static u8 gasm;

static unsigned char* global_text_sprite = fnt_text;
static char global_text_alpha = 255;
static unsigned short* global_text_tlut = fnt_text_tlut;

void DrawBigImg32(int x, int y, int w, int h, unsigned int img[]);

void DrawLogo(int x, int y, float scale_x, float scale_y);

void ClearBackground(u8 r, u8 g, u8 b);

void DrawScene();

void DrawText(int x, int y, char* text, char halign, char valign);

void stage00_init(void)
{
	scene = 0;
	gasm = 0;
	pos_y = ICON_DEFAULT_Y_POS;
	
	//Initialize Console
	nuDebConWindowPos(0, 25, 25);
}

void stage00_update(void)
{
	nuContDataGetExAll(contData);
	
	if(contData[0].stick_y < 0)
		pos_y = ICON_DEFAULT_Y_POS + MENU_OFFSET_Y;
	else if (contData[0].stick_y > 0)
		pos_y = ICON_DEFAULT_Y_POS;
	
	// Set debug mode
	if(contData[0].trigger & START_BUTTON){
		if (scene > -1)
			scene = -1;
		else
			scene = 0;
	}
	
	if(contData[0].trigger & A_BUTTON){
		if (scene > -1)
			scene++;
	}
}

void stage00_draw(void)
{
	char r_str[3];
	char g_str[3];
	char b_str[3];
	
	// Initialize RCP
	glistp = glist;
	RCPInit(glistp);
	
	// Draw
	DrawScene();

	// Sync
	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);
	nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx),
		NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
		
	//Debug commands
	if (scene == -1) {
		nuDebConClear(NU_DEB_CON_WINDOW0);
		nuDebConCPuts(NU_DEB_CON_WINDOW0, "Debug Menu\n\n");
		nuDebConDisp(NU_SC_SWAPBUFFER);
	}

}

void ClearBackground(u8 r, u8 g, u8 b)
{
	gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetDepthImage(glistp++, nuGfxZBuffer);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, nuGfxZBuffer);
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(glistp++);
	
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                     osVirtualToPhysical(nuGfxCfb_ptr));
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(r, g, b, 1) << 16 |
                               GPACK_RGBA5551(r, g, b, 1)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(glistp++);
}

void DrawBigImg32(int x, int y, int w, int h, unsigned int img[]){
	int i = 0;
	
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 0, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
	
	for (i=0; i<h; i++)
	{
		gDPLoadMultiTile(
			glistp++,
			img,          			  // timg - Our sprite array
			0,                        // tmem - Address to store in TMEM
			G_TX_RENDERTILE,          // rt - Tile descriptor
			G_IM_FMT_RGBA,            // fmt - Our image format
			G_IM_SIZ_32b,             // size - Pixel size
			w, h,                     // width, height of the full image
			0, i,                     // Top left corner of the rectangle
			w-1, i,                   // Bottom right corner
			0,                        // Palette to use (always 0)
			G_TX_WRAP, G_TX_WRAP,     // cms, cmt
			G_TX_NOMASK, G_TX_NOMASK, // masks, maskt
			G_TX_NOLOD, G_TX_NOLOD    // shifts, shiftt
		);
			
		gSPTextureRectangle(glistp++, 
			x << 2, y + i << 2, 
			x + w << 2, y + i + 1 << 2,
			G_TX_RENDERTILE, 
			0 << 5, 0 << 5, 
			1 << 10, 1 << 10);
		
		gDPPipeSync(glistp++);
	}
}

void DrawLogo(int x, int y, float scale_x, float scale_y)
{
    float w = 32*scale_x;
    float h = 32*scale_y;
    s32 sx = (int) ((1<<10) / scale_x + 0.5F);
    s32 sy = (int) ((1<<10) / scale_y + 0.5F);
	
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 0, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
	
    gDPLoadTextureBlock(glistp++, 
        IGLogo32, 
        G_IM_FMT_RGBA, G_IM_SIZ_32b, 
        32, 32, 0, 
        G_TX_WRAP, G_TX_WRAP, 
        G_TX_NOMASK, G_TX_NOMASK, 
        G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, 
		((int)(x-w/2))<<2, ((int)(y-h/2))<<2,
		((int)(x+w/2))<<2, ((int)(y+h/2))<<2, 
        G_TX_RENDERTILE, 
        0 << 5, 0 << 5, 
        sx, sy);
    gDPPipeSync(glistp++);
}

void DrawScene(){
	switch(scene){
		case -1:
			ClearBackground(0, 0, 0);
			break;
		case 0:
			ClearBackground(DEF_BG_R, DEF_BG_G, DEF_BG_B);
			DrawBigImg32(TITLE_X_POS, TITLE_Y_POS, 200, 66, IGMainWhite200);
			DrawText(MENU_START_TXT_POS_X, MENU_START_TXT_POS_Y, "START", 
				TEXT_HALIGN_LEFT, TEXT_VALIGN_TOP);
			DrawText(MENU_OPTIONS_TXT_POS_X, MENU_OPTIONS_TXT_POS_Y, "OPTIONS", 
				TEXT_HALIGN_LEFT, TEXT_VALIGN_TOP);
			DrawLogo(ICON_DEFAULT_X_POS, pos_y, 0.5, 0.5);
			break;
		case 1:
			ClearBackground(DEF_BG_R, DEF_BG_G, DEF_BG_B);
			DrawBigImg32(TITLE_X_POS, TITLE_Y_POS, 112, 107, dGasm);
			break;
		case 2:
			ClearBackground(DEF_BG_R, DEF_BG_G, DEF_BG_B);
			DrawBigImg32(TITLE_X_POS, TITLE_Y_POS, 112, 112, dOof);
			break;
		case 3:
			ClearBackground(DEF_BG_R, DEF_BG_G, DEF_BG_B);
			DrawBigImg32(TITLE_X_POS, TITLE_Y_POS, 116, 128, kappaRide);
			break;
		default:
			ClearBackground(DEF_BG_R, DEF_BG_G, DEF_BG_B);
			DrawBigImg32(TITLE_X_POS, TITLE_Y_POS, 200, 66, IGMainWhite200);
			DrawLogo(ICON_DEFAULT_X_POS, pos_y, 1, 1);
			scene = 0;
	}
}

void DrawText(int x, int y, char* text, char halign, char valign)
{
    int i=0, j=0, posx=-1, posy=0;
    char newline_total = 0;
    char current_line = 0;
    int string_length = strlen(text);
    char TEXT_W = 8;
    char TEXT_H = 8;
    short TEXT_HPAD = 0;
    short TEXT_VPAD = 2;

    // Change some settings if using an outlined text
    if (global_text_sprite == fnt_text_outlined)
    {
        TEXT_H = 10;
        TEXT_VPAD = 0;
        TEXT_HPAD = 0;
    }
    
    // Count the number of newlines
    for (i=0;i<string_length;i++)
        if (text[i] == '\n')
            newline_total++;
            
            
    // Calculate the vertical alignment based on the number of newlines
    if (valign == TEXT_VALIGN_MIDDLE)
        posy -= (newline_total*TEXT_H)/2 + TEXT_H/2  + (TEXT_VPAD*newline_total)/2;
    else if (valign == TEXT_VALIGN_BOTTOM)
        posy -= (newline_total*TEXT_H) + TEXT_H + (TEXT_VPAD*newline_total);
    
    
    // Set drawing mode
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16 );
    gDPSetPrimDepth(glistp++, 10, 0);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, global_text_alpha);
    
    gDPLoadTLUT_pal16(glistp++, 0, global_text_tlut);
    
    
    // Draw the text
    for (i=0;i<string_length;i++)
    {
        char offsetx;
        float offsety;
        
        
        // If on a newline, calculate the horizontal alignment
        if (posx == -1)
        {
            posx = 0;
            if (halign != TEXT_HALIGN_LEFT)
            {
                char chars_to_newline = 0;
                char current_newline = 0;
                
                for (j=0;j<string_length;j++)
                {
                    if (current_newline == current_line)
                        break;
                    if (text[j] == '\n')
                        current_newline++;
                }
                
                for (;j<string_length;j++)
                {
                    if (text[j] == '\n')
                        break;
                    else
                        chars_to_newline++;
                }
                if (halign == TEXT_HALIGN_RIGHT)
                    posx -= (chars_to_newline*TEXT_W)+TEXT_HPAD*chars_to_newline;
                else
                    posx -= ((chars_to_newline)*TEXT_W)/2 + ((chars_to_newline)*TEXT_HPAD)/2;
            }
        }
        
        
        // If we found a new line, move the text position
        if (text[i] == '\n')
        {
            posy+=TEXT_H+TEXT_VPAD;
            posx=-1;
            current_line++;
            continue;
        }
        
        // Don't render if we're out of the screen
        if (x+posx+TEXT_W+TEXT_HPAD > SCREEN_WD)
            continue;
        
        // Offset the font sprite based on what character we're currently drawing.
        if (text[i] >= '0' && text[i] <= '9' && (global_text_sprite == fnt_text || global_text_sprite == fnt_text_outlined))
        {
            char * sprite;
            if (global_text_sprite == fnt_text)
                sprite = fnt_numbers;
            else
                sprite = fnt_numbers_outlined;
            offsety = text[i] - '0';
            gDPLoadTextureTile_4b(glistp++, sprite, G_IM_FMT_CI, TEXT_W, TEXT_H, 0, (int)(offsety*TEXT_H), TEXT_W, TEXT_H + (int)(offsety*TEXT_H), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+posx << 2, y+posy << 2, x+TEXT_W+posx << 2, y+posy+TEXT_H << 2,  G_TX_RENDERTILE,  0 << 5, (int)offsety*TEXT_H << 5,  1 << 10, 1 << 10);
        }
        else
        {
            if (text[i] >= 'a' && text[i] <= 'z')
                text[i] = text[i] + ('A' - 'a');
                
            if (text[i] >= 'A' && text[i] <= 'P')
            {
                offsetx = text[i] - 'A';
                offsety = 0;
            }
            else if (text[i] >= 'Q' && text[i] <= 'Z')
            {
                if (global_text_sprite == fnt_text_outlined)
                    offsety = 0.9;
                else
                    offsety = 0.875; // To fix a hardware rendering bug. Change to 1 to melt your brain
                offsetx = text[i] - 'A';
            }
            else
            {
                switch (text[i])
                {
                    case ',': offsety = 1; offsetx = 10; break;
                    case '.': offsety = 1; offsetx = 11; break;
                    case '!': offsety = 1; offsetx = 12; break;
                    case '?': offsety = 1; offsetx = 13; break;
                    case '\'': offsety = 1; offsetx = 14; break;
                    case '-': offsety = 1; offsetx = 15; break;
                    case ':': offsety = 2; offsetx = 0; break;
                    case '\a': offsety = 2; offsetx = 1; break;
                    case '[': offsety = 2; offsetx = 2; break;
                    case ']': offsety = 2; offsetx = 3; break;
                    case '(': offsety = 2; offsetx = 4; break;
                    case ')': offsety = 2; offsetx = 5; break;
                    case '{': offsety = 2; offsetx = 6; break;
                    case '}': offsety = 2; offsetx = 7; break;
                    default: posx+=TEXT_W+TEXT_HPAD; continue;
                }
            }
            
            gDPLoadTextureTile_4b(glistp++, global_text_sprite, G_IM_FMT_CI, 128, 32, offsetx*TEXT_W, (int)(offsety*TEXT_H), TEXT_W + (offsetx*TEXT_W), TEXT_H + (int)(offsety*TEXT_H), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+posx << 2, y+posy << 2, x+posx+TEXT_W << 2, y+posy+TEXT_H << 2,  G_TX_RENDERTILE,  offsetx*TEXT_W << 5, (int)(offsety*TEXT_H) << 5,  1 << 10, 1 << 10);
        }
        posx+=TEXT_W+TEXT_HPAD;
    }
    gDPPipeSync(glistp++);
}
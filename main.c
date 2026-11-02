#include <gccore.h>
#include <wiiuse/wpad.h>
#include <math.h>
#include <stdlib.h>

static GXRModeObj *rmode;
static void *xfb[2];
static int fb = 0;
static f32 diverX = 0.0f, diverY = 0.0f, diverZ = 0.0f;

static void vertex(f32 x, f32 y, f32 z, u8 r, u8 g, u8 b, u8 a) {
    GX_Position3f32(x, y, z);
    GX_Color4u8(r, g, b, a);
}

static void triangle(
    f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz,
    f32 cx, f32 cy, f32 cz, u8 r, u8 g, u8 b
) {
    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
    vertex(ax, ay, az, r, g, b, 255);
    vertex(bx, by, bz, r, g, b, 255);
    vertex(cx, cy, cz, r, g, b, 255);
    GX_End();
}

static void drawOceanFloor(void) {
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    vertex(-30, -8, -30, 12, 55, 67, 255);
    vertex( 30, -8, -30, 12, 55, 67, 255);
    vertex( 30, -8,  30,  8, 38, 54, 255);
    vertex(-30, -8,  30,  8, 38, 54, 255);
    GX_End();

    /* Simple rocks and coral silhouettes for the first blockout. */
    triangle(-7,-8,-5, -6,-4,-5, -5,-8,-5, 30,105,105);
    triangle( 5,-8,-9,  6,-3,-9,  7,-8,-9, 25,125,115);
    triangle( 8,-8, 2,  9,-5, 2, 10,-8, 2, 40,135,125);
    triangle(-10,-8,7, -9,-5,7, -8,-8,7, 35,115,125);
}

static void drawDiver(void) {
    /* Placeholder low-poly diver, facing toward negative Z. */
    f32 x = diverX, y = diverY, z = diverZ;

    /* torso */
    triangle(x-0.65f,y-0.9f,z, x+0.65f,y-0.9f,z, x+0.48f,y+0.75f,z, 22,45,72);
    triangle(x-0.65f,y-0.9f,z, x+0.48f,y+0.75f,z, x-0.48f,y+0.75f,z, 18,35,62);

    /* head / orange hood */
    triangle(x-0.38f,y+0.75f,z, x+0.38f,y+0.75f,z, x+0.30f,y+1.35f,z, 190,92,38);
    triangle(x-0.38f,y+0.75f,z, x+0.30f,y+1.35f,z, x-0.30f,y+1.35f,z, 215,112,45);

    /* arms */
    triangle(x-0.65f,y+0.55f,z, x-1.05f,y-0.55f,z, x-0.55f,y-0.75f,z, 175,125,95);
    triangle(x+0.65f,y+0.55f,z, x+1.05f,y-0.55f,z, x+0.55f,y-0.75f,z, 175,125,95);

    /* legs and fins */
    triangle(x-0.42f,y-0.85f,z, x-0.08f,y-1.65f,z, x-0.52f,y-2.1f,z-0.35f, 35,45,78);
    triangle(x+0.42f,y-0.85f,z, x+0.08f,y-1.65f,z, x+0.52f,y-2.1f,z-0.35f, 35,45,78);
    triangle(x-0.52f,y-2.1f,z-0.35f, x-0.95f,y-2.05f,z-0.7f, x-0.30f,y-1.85f,z-0.7f, 235,135,45);
    triangle(x+0.52f,y-2.1f,z-0.35f, x+0.95f,y-2.05f,z-0.7f, x+0.30f,y-1.85f,z-0.7f, 235,135,45);
}

static void setupGX(void) {
    GX_Init(GX_Init_ex);
    GX_SetCopyClear((GXColor){ 4, 20, 66, 255 }, 0x00ffffff);
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, rmode->xfbHeight);
    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_TRUE : GX_FALSE));
    GX_SetCullMode(GX_CULL_NONE);
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetAlphaUpdate(GX_TRUE);

    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
}

int main(int argc, char **argv) {
    Mtx view, model;
    Mtx44 projection;

    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb[fb]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    setupGX();
    guPerspective(projection, 48.0f, (f32)rmode->fbWidth / (f32)rmode->efbHeight, 0.1f, 100.0f);
    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

    while (1) {
        WPAD_ScanPads();
        u32 held = WPAD_ButtonsHeld(0);
        const f32 speed = 0.12f;

        if (held & WPAD_BUTTON_LEFT)  diverX -= speed;
        if (held & WPAD_BUTTON_RIGHT) diverX += speed;
        if (held & WPAD_BUTTON_UP)    diverZ -= speed;
        if (held & WPAD_BUTTON_DOWN)  diverZ += speed;
        if (held & WPAD_BUTTON_A)     diverY += speed;
        if (held & WPAD_BUTTON_B)     diverY -= speed;

        if (diverX < -12) diverX = -12;
        if (diverX >  12) diverX =  12;
        if (diverY < -4)  diverY = -4;
        if (diverY >  5)   diverY = 5;
        if (diverZ < -18) diverZ = -18;
        if (diverZ >  12) diverZ = 12;

        guLookAt(view,
            (guVector){diverX, diverY + 3.0f, diverZ + 8.0f},
            (guVector){diverX, diverY, diverZ},
            (guVector){0, 1, 0});
        GX_LoadPosMtxImm(view, GX_PNMTX0);

        GX_InvVtxCache();
        GX_InvalidateTexAll();
        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
        GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
        GX_SetCopyClear((GXColor){ 5, 24, 78, 255 }, 0x00ffffff);
        GX_ClearVtxDesc();
        GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        GX_SetNumChans(1);
        GX_SetNumTexGens(0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

        drawOceanFloor();
        drawDiver();

        GX_DrawDone();
        GX_CopyDisp(xfb[fb], GX_TRUE);
        VIDEO_SetNextFramebuffer(xfb[fb]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
        fb ^= 1;
    }
    return 0;
}

#ifndef _RENDERER
#define _RENDERER

#include <stdint.h>
#include <SDL2/SDL.h>

#include "Vec3d.cpp"
#include "Triangle.cpp"

struct Renderer {
    SDL_Renderer* renderer;
    int width;
    int height;

    Renderer() {
        renderer = NULL;
    }

    Renderer (SDL_Renderer* r, int w, int h) {
        renderer = r;
        width = w;
        height = h;
    }

    void HexToRGB(uint32_t &hex, uint8_t &r, uint8_t &g, uint8_t &b) {
        r = (hex & 0xff0000) >> 16;
        g = (hex & 0x00ff00) >> 8;
        b = hex & 0x0000ff;
    }

    void Fill(uint32_t col) {
        uint8_t r, g, b;
        HexToRGB(col, r, g, b);

        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
    }

    void DrawLine(Vec3d p1, Vec3d p2) {
        int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
        int x1 = p1.x;
        int x2 = p2.x;
        int y1 = height - p1.y;
        int y2 = height - p2.y;

        dx = x2 - x1; dy = y2 - y1;
        dx1 = abs(dx); dy1 = abs(dy);
        px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
        if (dy1 <= dx1)
        {
            if (dx >= 0)
                { x = x1; y = y1; xe = x2; }
            else
                { x = x2; y = y2; xe = x1;}

            SDL_RenderDrawPoint(renderer, x, y);
            
            for (i = 0; x<xe; i++)
            {
                x = x + 1;
                if (px<0)
                    px = px + 2 * dy1;
                else
                {
                    if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
                    px = px + 2 * (dy1 - dx1);
                }
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        else
        {
            if (dy >= 0)
                { x = x1; y = y1; ye = y2; }
            else
                { x = x2; y = y2; ye = y1; }

            SDL_RenderDrawPoint(renderer, x, y);

            for (i = 0; y<ye; i++)
            {
                y = y + 1;
                if (py <= 0)
                    py = py + 2 * dx1;
                else
                {
                    if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
                    py = py + 2 * (dx1 - dy1);
                }
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    void DrawLine(Vec3d p1, Vec3d p2, uint32_t col) {
        uint8_t r, g, b;
        HexToRGB(col, r, g, b);
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        DrawLine(p1, p2);
    }

    void DrawTriangle(Triangle tri, uint32_t col) {
        uint8_t r, g, b;
        HexToRGB(col, r, g, b);
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        DrawLine(tri.p[0], tri.p[1]);
        DrawLine(tri.p[1], tri.p[2]);
        DrawLine(tri.p[2], tri.p[0]);
    }

    void FillTriangle(Triangle tri, uint32_t col) {
        tri.col = col;
        FillTriangle(tri);
    }

    void FillTriangle(Triangle tri) {
        auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
        auto drawline = [&](int sx, int ex, int ny) { 
            uint8_t r, g, b;
            HexToRGB(tri.col, r, g, b);
            SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer, sx, ny, ex, ny);
            //for (int i = sx; i <= ex; i++)
            //    SDL_RenderDrawPoint(renderer, i, ny);
        };
        
        int t1x, t2x, y, minx, maxx, t1xp, t2xp;
        bool changed1 = false;
        bool changed2 = false;
        int signx1, signx2, dx1, dy1, dx2, dy2;
        int e1, e2;
        int x1 = tri.p[0].x;
        int x2 = tri.p[1].x;
        int x3 = tri.p[2].x;
        int y1 = height - tri.p[0].y;
        int y2 = height - tri.p[1].y;
        int y3 = height - tri.p[2].y;
        // Sort vertices
        if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
        if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
        if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

        t1x = t2x = x1; y = y1;   // Starting points
        dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
        else signx1 = 1;
        dy1 = (int)(y2 - y1);

        dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
        else signx2 = 1;
        dy2 = (int)(y3 - y1);

        if (dy1 > dx1) {   // swap values
            SWAP(dx1, dy1);
            changed1 = true;
        }
        if (dy2 > dx2) {   // swap values
            SWAP(dy2, dx2);
            changed2 = true;
        }

        e2 = (int)(dx2 >> 1);
        // Flat top, just process the second half
        if (y1 == y2) goto next;
        e1 = (int)(dx1 >> 1);

        for (int i = 0; i < dx1;) {
            t1xp = 0; t2xp = 0;
            if (t1x<t2x) { minx = t1x; maxx = t2x; }
            else { minx = t2x; maxx = t1x; }
            // process first line until y value is about to change
            while (i<dx1) {
                i++;
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) t1xp = signx1;//t1x += signx1;
                    else          goto next1;
                }
                if (changed1) break;
                else t1x += signx1;
            }
            // Move line
        next1:
            // process second line until y value is about to change
            while (1) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2) t2xp = signx2;//t2x += signx2;
                    else          goto next2;
                }
                if (changed2)     break;
                else              t2x += signx2;
            }
        next2:
            if (minx>t1x) minx = t1x;
            if (minx>t2x) minx = t2x;
            if (maxx<t1x) maxx = t1x;
            if (maxx<t2x) maxx = t2x;
            drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                        // Now increase y
            if (!changed1) t1x += signx1;
            t1x += t1xp;
            if (!changed2) t2x += signx2;
            t2x += t2xp;
            y += 1;
            if (y == y2) break;

        }
    next:
        // Second half
        dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
        else signx1 = 1;
        dy1 = (int)(y3 - y2);
        t1x = x2;

        if (dy1 > dx1) {   // swap values
            SWAP(dy1, dx1);
            changed1 = true;
        }
        else changed1 = false;

        e1 = (int)(dx1 >> 1);

        for (int i = 0; i <= dx1; i++) {
            t1xp = 0; t2xp = 0;
            if (t1x<t2x) { minx = t1x; maxx = t2x; }
            else { minx = t2x; maxx = t1x; }
            // process first line until y value is about to change
            while (i<dx1) {
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                    else          goto next3;
                }
                if (changed1) break;
                else   	   	  t1x += signx1;
                if (i<dx1) i++;
            }
        next3:
            // process second line until y value is about to change
            while (t2x != x3) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2) t2xp = signx2;
                    else          goto next4;
                }
                if (changed2)     break;
                else              t2x += signx2;
            }
        next4:

            if (minx>t1x) minx = t1x;
            if (minx>t2x) minx = t2x;
            if (maxx<t1x) maxx = t1x;
            if (maxx<t2x) maxx = t2x;
            drawline(minx, maxx, y);   										
            if (!changed1) t1x += signx1;
            t1x += t1xp;
            if (!changed2) t2x += signx2;
            t2x += t2xp;
            y += 1;
            if (y>y3) return;
        }
    }
};

#endif
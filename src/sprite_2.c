#include "gba/m4a.h"
#include "data.h"
#include "sprite.h"
#include "main.h"
#include "task.h"

void sub_0815604C(struct Sprite *sprite) {
    OamData *r4;
    s32 sl, sp00, sp04, sp08;
    u8 sp0C, i;
    const u16 *sp10;
    u16 sp14 = 0;
    u32 sp18 = 0, sp1C = 0, sp20 = 0, sp24 = 0;
    u32 sp28;
    union SpriteAttributes spriteAttrs;
    u32 x1, y1;
    u32 r0, r1;

    if (sprite->unk4 != -1) {
        if (!(sprite->unk4 >> 28))
            spriteAttrs.sub = &gSpriteTables->attrs[sprite->animId].sub[sprite->unk4];
        else
            spriteAttrs.full = &gSpriteTables->attrs[sprite->animId].full[sprite->unk4];
        
        sprite->unk1E = spriteAttrs.sub->numSubframes;
        sp00 = sprite->x;
        sl = sprite->y;
        if (sprite->unk8 & 0x20000) {
            sp00 -= gUnk_030023F4.unk0;
            sl -= gUnk_030023F4.unk2;
        }

        sp04 = spriteAttrs.sub->width;
        sp08 = spriteAttrs.sub->height;
        if (sprite->unk8 & 0x20) {
            sp14 |= 0x100;
            sp18 |= (sprite->unk8 & 0x1F) << 9;
            if (sprite->unk8 & 0x40) {
                sp00 -= spriteAttrs.sub->width >> 1;
                sl -= spriteAttrs.sub->height >> 1;
                sp04 <<= 1;
                sp08 <<= 1;
                sp14 |= 0x200;
            }
        } else {
            if (sprite->unk8 & 0x800) {
                sl -= sp08 - spriteAttrs.sub->offsetY;
            } else {
                sl -= spriteAttrs.sub->offsetY;
            }
            if (sprite->unk8 & 0x400) {
                sp00 -= sp04 - spriteAttrs.sub->offsetX;
            } else {
                sp00 -= spriteAttrs.sub->offsetX;
            }
            if (((sprite->unk8 >> 11) & 1) != (spriteAttrs.sub->bitfield >> 15))
                sp20 = 1;
            r1 = sprite->unk8;
            r1 >>= 10;
            r0 = spriteAttrs.sub->bitfield >> 14;
            if ((r0 ^ r1) & 1)
                sp24 = 1;
        }
        if (!sp04
            || (sp00 + sp04 >= 0 && sp00 <= 240 && sp08 + sl >= 0 && sl <= 160)) {
            u32 r1 = (sp1C + (sprite->palId << 12)) << 16;

            sp14 |= (sprite->unk8 & 0x180) * 8;
            sp1C = (((sprite->unk8 & 0x3000) << 14) | r1) >> 16;
            sp10 = gSpriteTables->oamData[sprite->animId];
            sprite->unk1D = gUnk_030024F0;
            for (sp0C = 0; sp0C < spriteAttrs.sub->numSubframes; ++sp0C) {
                r4 = sub_08156D84((sprite->unk14 & 0x7C0) >> 6);
                if (iwram_end == r4) return;
                DmaCopy16(3, &sp10[3 * ((spriteAttrs.sub->bitfield & 0x3FFF) + sp0C)], r4, 6);
                x1 = r4->all.attr1 & 0x1FF;
                y1 = r4->all.attr0 & 0xFF;
                r4->all.attr1 &= 0xFE00;
                r4->all.attr0 &= 0xFE00;
                if (sp20 | sp24) {
                    u32 shape = ((r4->all.attr0 & 0xC000) >> 12);
                    u32 shapeAndSize;

                    shape |= ((r4->all.attr1 & 0xC000) >> 14);
                    shapeAndSize = shape; // required for matching
                    if (sp20) {
                        r4->all.attr1 ^= 0x2000;
                        y1 = sp08 - gUnk_08D6084C[shapeAndSize][1] - y1;
                    }

                    if (sp24) {
                        r4->all.attr1 ^= 0x1000;
                        x1 = sp04 - gUnk_08D6084C[shapeAndSize][0] - x1;
                    }
                }

                r4->all.attr0 += ((sl + y1) & 0xFF);
                r4->all.attr1 += ((sp00 + x1) & 0x1FF);
                r4->all.attr0 |= sp14;
                r4->all.attr1 |= sp18;
                r4->all.attr2 |= sp1C;
                if (r4->all.attr0 & 0x2000)
                    r4->all.attr2 += r4->all.attr2 & 0x3FF;
                r4->all.attr2 += (sprite->tilesVram - 0x6010000u) >> 5;
            }

            if ((sprite->unk4 >> 28) == 1 && sprite->unk8 & 0x4000000) {
                const s32 *sl;
                u16 r8_, r3;
                const u8 *ip, *r2, *r4;
                s32 tilesVram;
                u32 r1;

                sprite->unk8 &= ~0x4000000;
                r1 = spriteAttrs.full->unkC & 0xFFFFFF;
                sp28 = spriteAttrs.full->unkC >> 24;
                sl = gSpriteTables->unk18 + r1;
                if (sl[0] >= 0) {
                    r8_ = 0x20;
                    ip = gSpriteTables->tiles4bpp;
                } else {
                    r8_ = 0x40;
                    ip = gSpriteTables->tiles8bpp;
                }

                r3 = r8_;
                r2 = ip + sl++[0] * r8_;
                tilesVram = sprite->tilesVram;
                for (i = 1; i < sp28; ++i) {
                    r4 = ip + sl++[0] * r8_;
                    if (r2 + r3 == r4)
                        r3 += r8_;
                    else {
                        gUnk_03002EC0[gUnk_030039A4].unk0 = (uintptr_t)r2;
                        gUnk_03002EC0[gUnk_030039A4].unk4 = tilesVram;
                        gUnk_03002EC0[gUnk_030039A4].unk8 = r3;
                        gUnk_030039A4 = (gUnk_030039A4 + 1) & 0x3F;
                        tilesVram += r3;
                        r3 = r8_;
                        r2 = r4;
                    }
                }
                gUnk_03002EC0[gUnk_030039A4].unk0 = (uintptr_t)r2;
                gUnk_03002EC0[gUnk_030039A4].unk4 = tilesVram;
                gUnk_03002EC0[gUnk_030039A4].unk8 = r3;
                gUnk_030039A4 = (gUnk_030039A4 + 1) & 0x3F;
            }
        }
    }
}

void sub_081564D8(struct Sprite *sprite) {
    u16 localOamBuffer[3];
    OamData *localOamBufferPtr;
    s32 sp08, sp0C;
    s32 sp10, sp14;
    u8 sp18, j;
    const u16 *sp1C;
    u16 sp20 = 0;
    u32 sp24 = 0, sp28 = 0, sp2C = 0, sp30 = 0;
    u32 sp34;
    union SpriteAttributes spriteAttrs;
    s32 x1, y1;
    s32 r7, ip;
    u32 r0, r1;

    if (sprite->unk4 != -1) {
        if (!(sprite->unk4 >> 28))
            spriteAttrs.sub = &gSpriteTables->attrs[sprite->animId].sub[sprite->unk4];
        else
            spriteAttrs.full = &gSpriteTables->attrs[sprite->animId].full[sprite->unk4];

        sprite->unk1E = spriteAttrs.sub->numSubframes;
        sp08 = sprite->x;
        sp0C = sprite->y;
        if (sprite->unk8 & 0x20000) {
            sp08 -= gUnk_030023F4.unk0;
            sp0C -= gUnk_030023F4.unk2;
        }

        sp10 = spriteAttrs.sub->width;
        sp14 = spriteAttrs.sub->height;

        if (sprite->unk8 & 0x20) {
            sp20 |= 0x100;
            sp24 |= (sprite->unk8 & 0x1F) << 9;
            if (sprite->unk8 & 0x40) {
                sp08 -= spriteAttrs.sub->width >> 1;
                sp0C -= spriteAttrs.sub->height >> 1;
                sp10 <<= 1;
                sp14 <<= 1;
                sp20 |= 0x200;
            }
        } else {
            if (sprite->unk8 & 0x800)
                sp0C -= sp14 - spriteAttrs.sub->offsetY;
            else
                sp0C -= spriteAttrs.sub->offsetY;

            if (sprite->unk8 & 0x400)
                sp08 -= sp10 - spriteAttrs.sub->offsetX;
            else
                sp08 -= spriteAttrs.sub->offsetX;

            if (((sprite->unk8 >> 11) & 1) != (spriteAttrs.sub->bitfield >> 15))
                sp2C = 1;
            r1 = sprite->unk8;
            r1 >>= 10;
            r0 = spriteAttrs.sub->bitfield >> 14;
            if ((r0 ^ r1) & 1)
                sp30 = 1;
        }

        if (!sp10
            || (sp08 + sp10 >= 0 && sp08 <= 240 && sp0C + sp14 >= 0 && sp0C <= 160)) {
            u32 r1 = (sp28 + (sprite->palId << 12)) << 16;

            sp20 |= (sprite->unk8 & 0x180) * 8;
            sp28 = (((sprite->unk8 & 0x3000) << 14) | r1) >> 16;
            sp1C = gSpriteTables->oamData[sprite->animId];
            sprite->unk1D = gUnk_030024F0;
            for (sp18 = 0; sp18 < spriteAttrs.sub->numSubframes; ++sp18) {
                u32 shapeAndSize;

                DmaCopy16(3, &sp1C[3 * ((spriteAttrs.sub->bitfield & 0x3FFF) + sp18)], localOamBuffer, 6); // excluding affine params
                localOamBufferPtr = (OamData *)localOamBuffer;
                x1 = localOamBufferPtr->all.attr1 & 0x1FF;
                if (x1 >= 0x100)
                    x1 -= 0x200;
                y1 = localOamBufferPtr->all.attr0 & 0xFF;
                if (y1 >= 0x80)
                    y1 -= 0x100;
                localOamBufferPtr->all.attr1 &= 0xFE00;
                localOamBufferPtr->all.attr0 &= 0xFE00;
                shapeAndSize = ((localOamBufferPtr->all.attr0 & 0xC000) >> 12) | ((localOamBufferPtr->all.attr1 & 0xC000) >> 14);
                r7 = gUnk_08D6084C[shapeAndSize][1];
                ip = gUnk_08D6084C[shapeAndSize][0];
#ifndef NONMATCHING
            {
                /* localOamBufferPtr is very likely to be a real variable as the function itself is a
                 * modified (and manually optimized) version of sub_081569A0. 
                 */
                register u32 _sp2C asm("r0") = sp2C;
                register u32 _sp30 asm("r1") = sp30;

                _sp2C = sp2C;
                _sp30 = sp30;
                asm("":::"r2");
                if (_sp2C | _sp30) {
#else
                if (sp2C | sp30) {
#endif
                    if (sp2C) {
                        localOamBufferPtr->all.attr1 ^= 0x2000;
                        y1 = sp14 - r7 - y1;
                    }

                    if (sp30) {
                        localOamBufferPtr->all.attr1 ^= 0x1000;
                        x1 = sp10 - ip - x1;
                    }
                }
#ifndef NONMATCHING
            }
#endif
                if ((sp0C + y1 + r7 >= 0 && sp0C + y1 <= 160)
                    && (sp08 + x1 + ip >= 0 && sp08 + x1 <= 240)) {
                    OamData *oamDst;

                    localOamBufferPtr->all.attr0 += ((sp0C + y1) & 0xFF);
                    localOamBufferPtr->all.attr1 += ((sp08 + x1) & 0x1FF);
                    localOamBufferPtr->all.attr0 |= sp20;
                    localOamBufferPtr->all.attr1 |= sp24;
                    localOamBufferPtr->all.attr2 |= sp28;
                    if (localOamBufferPtr->all.attr0 & 0x2000)
                        localOamBufferPtr->all.attr2 += localOamBufferPtr->all.attr2 & 0x3FF;
                    localOamBufferPtr->all.attr2 += (sprite->tilesVram - 0x6010000u) >> 5;
                    oamDst = sub_08156D84((sprite->unk14 & 0x7C0) >> 6);
                    if (iwram_end == oamDst) return;
                    DmaCopy16(3, localOamBuffer, oamDst, 6);
                }
            }

            if ((sprite->unk4 >> 28) == 1 && sprite->unk8 & 0x4000000) {
                const s32 *ip;
                u16 r8, r3;
                const u8 *r2_, *r2, *r4;
                s32 tilesVram;
                u32 r1;

                sprite->unk8 &= ~0x4000000;
                r1 = spriteAttrs.full->unkC & 0xFFFFFF;
                sp34 = spriteAttrs.full->unkC >> 24;
                ip = gSpriteTables->unk18 + r1;
                if (ip[0] >= 0) {
                    r8 = 0x20;
                    r2_ = gSpriteTables->tiles4bpp;
                } else {
                    r8 = 0x40;
                    r2_ = gSpriteTables->tiles8bpp;
                }

                r3 = r8;
                r2 = r2_ + ip++[0] * r8;
                tilesVram = sprite->tilesVram;
                for (j = 1; j < sp34; ++j) {
                    r4 = r2_ + ip++[0] * r8;
                    if (r2 + r3 == r4)
                        r3 += r8;
                    else {
                        gUnk_03002EC0[gUnk_030039A4].unk0 = (uintptr_t)r2;
                        gUnk_03002EC0[gUnk_030039A4].unk4 = tilesVram;
                        gUnk_03002EC0[gUnk_030039A4].unk8 = r3;
                        gUnk_030039A4 = (gUnk_030039A4 + 1) & 0x3F;
                        tilesVram += r3;
                        r3 = r8;
                        r2 = r4;
                    }
                }
                gUnk_03002EC0[gUnk_030039A4].unk0 = (uintptr_t)r2;
                gUnk_03002EC0[gUnk_030039A4].unk4 = tilesVram;
                gUnk_03002EC0[gUnk_030039A4].unk8 = r3;
                gUnk_030039A4 = (gUnk_030039A4 + 1) & 0x3F;
            }
        }
    }
}

/* unused function */
void sub_081569A0(struct Sprite *sprite, u16 *sp08, u8 sp0C) {
    vs32 sp00, sp04;
    s32 sp10, sp14;
    u8 sp18, i;
    u32 x1, y1, sp24, sp28;
    union SpriteAttributes spriteAttrs;

    if (sprite->unk4 != -1) {
        if (!(sprite->unk4 >> 28))
            spriteAttrs.sub = &gSpriteTables->attrs[sprite->animId].sub[sprite->unk4];
        else
            spriteAttrs.full = &gSpriteTables->attrs[sprite->animId].full[sprite->unk4];

        sprite->unk1E = spriteAttrs.sub->numSubframes;
        sp00 = sprite->x;
        sp04 = sprite->y;
        if (sprite->unk8 & 0x20000) {
            sp00 -= gUnk_030023F4.unk0;
            sp04 -= gUnk_030023F4.unk2;
        }

        sp10 = spriteAttrs.sub->width;
        sp14 = spriteAttrs.sub->height;

        if (sprite->unk8 & 0x20) {
            if (sprite->unk8 & 0x40) {
                sp00 -= spriteAttrs.sub->width >> 1;
                sp04 -= spriteAttrs.sub->height >> 1;
                sp10 <<= 1;
                sp14 <<= 1;
            }
        } else {
            u32 r1;

            if (sprite->unk8 & 0x800) {
                r1 = spriteAttrs.sub->offsetY;
                r1 = sp14 - r1;
            } else {
                r1 = spriteAttrs.sub->offsetY;
            }
            sp04 -= r1;

            if (sprite->unk8 & 0x400) {
                r1 = spriteAttrs.sub->offsetX;
                r1 = sp10 - r1;
            } else {
                r1 = spriteAttrs.sub->offsetX;
            }
            sp00 -= r1;
        }

        sp24 = sp00 - sprite->x;
        sp28 = sp04 - sprite->y;
        if (sp00 + sp10 >= 0 && sp00 <= 240
            && sp04 + sp14 >= 0 && sp04 <= 160) {
            for (sp18 = 0; sp18 < spriteAttrs.sub->numSubframes; ++sp18) {
                const u16 *r4 = gSpriteTables->oamData[sprite->animId];
                OamData *oam = sub_08156D84((sprite->unk14 & 0x7C0) >> 6);

                if (iwram_end == oam) return;
                DmaCopy16(3, &r4[3 * ((spriteAttrs.sub->bitfield & 0x3FFF) + sp18)], oam, 6); // excluding affine params
                x1 = oam->all.attr1 & 0x1FF;
                y1 = oam->all.attr0 & 0xFF;
                oam->all.attr1 &= 0xFE00;
                oam->all.attr0 &= 0xFE00;
                oam->all.attr2 += sprite->palId << 12;
                if (sprite->unk8 & 0x20) {
                    oam->all.attr0 |= 0x100;
                    if (sprite->unk8 & 0x40)
                        oam->all.attr0 |= 0x200;
                    oam->all.attr1 |= (sprite->unk8 & 0x1F) << 9;
                } else {
                    u32 shapeAndSize = ((oam->all.attr0 & 0xC000) >> 12) | ((oam->all.attr1 & 0xC000) >> 14);
                    u32 r1 = (sprite->unk8 >> 11) & 1;
                    u32 r0 = spriteAttrs.sub->bitfield >> 15;

                    if (r1 != r0) {
                        oam->all.attr1 ^= 0x2000;
                        y1 = sp14 - gUnk_08D6084C[shapeAndSize][1] - y1;
                    }

                    r1 = sprite->unk8;
                    r1 >>= 10;
                    r0 = spriteAttrs.sub->bitfield >> 14;
                    if ((r0 ^ r1) & 1) {
                        oam->all.attr1 ^= 0x1000;
                        x1 = sp10 - gUnk_08D6084C[shapeAndSize][0] - x1;
                    }
                }

                oam->all.attr0 |= (sprite->unk8 & 0x180) * 8;
                oam->all.attr2 |= (sprite->unk8 & 0x3000) >> 2;
                oam->all.attr0 += (sp04 + y1) & 0xFF;
                oam->all.attr1 += (sp00 + x1) & 0x1FF;
                if (oam->all.attr0 & 0x2000)
                    oam->all.attr2 += oam->all.attr2 & 0x3FF;
                oam->all.attr2 += (sprite->tilesVram - 0x6010000u) >> 5;
#ifndef NONMATCHING
                asm("":::"r8");
#endif
                for (i = 0; i < sp0C; ++i) {
                    OamData *r5 = sub_08156D84((sprite->unk14 & 0x7C0) >> 6);

                    if (iwram_end == oam) return;
                    DmaCopy16(3, oam, r5, 6);
                    r5->all.attr1 &= 0xFE00;
                    r5->all.attr0 &= 0xFF00;
                    r5->all.attr0 += (sp08[2 * i + 1] + sp28 + y1) & 0xFF;
                    r5->all.attr1 += (sp08[2 * i + 0] + sp24 + x1) & 0x1FF;
                }
            }
        }
    }
}

OamData *sub_08156D84(u8 r5) {
    if (r5 > 0x1f) r5 = 0x1f;
    if ((s8)gUnk_030024F0 < 0) {
        return iwram_end;
    }
    else {
        if (gUnk_03002450[r5] == 0xff) {
            gUnk_030031C0[gUnk_030024F0].split.fractional = 0xff;
            gUnk_03002450[r5] = gUnk_030024F0;
            gUnk_03006080[r5] = gUnk_030024F0;
        }
        else {
            gUnk_030031C0[gUnk_030024F0].split.fractional = 0xff;
            gUnk_030031C0[gUnk_03006080[r5]].split.fractional = gUnk_030024F0;
            gUnk_03006080[r5] = gUnk_030024F0;
        }
        ++gUnk_030024F0;
        return &gUnk_030031C0[gUnk_030024F0 - 1];
    }
}

void DrawToOamBuffer(void) {
    OamData *oam = gOamBuffer;
    u8 j = 0;
    s32 i;
    s8 r0;
    u8 *unused;

    for (i = 0; i < 0x20; i++) {
        for (r0 = gUnk_03002450[i]; r0 != -1; r0 = gUnk_030031C0[r0].all.affineParam) {
            unused = gUnk_030035F0;
            DmaCopy16(3, gUnk_030031C0 + r0, oam, 6);
            oam++;
            gUnk_030035F0[r0] = j++;
            unused++; unused--;
        }
    }

    if (gUnk_03002440 & 0x800) {
        i = gUnk_030024F0;
        oam = gOamBuffer + gUnk_030024F0;
        for (; i < gUnk_03003A00; i++) {
            DmaFill16(3, 0x200, oam, 0x6);
            oam++;
        }
    } else {
        if (gUnk_03002440 & 0x400) {
            s32 j;
            i = gUnk_030024F0 - 1;
            oam = gOamBuffer + i; // not used, but can force oam to be preloaded
            for (j = 0; i >= 0; i--, j++) {
                DmaCopy16(3, gOamBuffer + i, gOamBuffer + (0x7f - j), 6);
            }
            gUnk_03003A00 = 0x80 - gUnk_030024F0;
            for (i = 0; i < gUnk_03003A00; i++) {
                DmaFill16(3, 0x200, gOamBuffer + i, 6);
                unused++; unused--;
            }
        } else {
            gUnk_03003A00 = 0;
        }
    }

    gUnk_030024F0 = 0;
    if (gUnk_03002440 & 0x4000) {
        CpuFill32(~0, gUnk_03002450, 0x20);
        CpuFill32(~0, gUnk_03006080, 0x20);
    }
    else {
        DmaFill32(3, ~0, gUnk_03002450, 0x20);
        DmaFill32(3, ~0, gUnk_03006080, 0x20);
    }
}

s32 AnimCmd_JumpBack(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor -= cursor.jumpBack->offset;
    return 1;
}

s32 AnimCmd_4(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->unk8 |= 0x4000;
    return 0;
}

s32 AnimCmd_PlaySoundEffect(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_PlaySoundEffect) / 4;
    m4aSongNumStart(cursor.playSoundEffect->songId);
    return 1;
}

s32 AnimCmd_TranslateSprite(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_TranslateSprite) / 4;
    sprite->x += cursor.translateSprite->x;
    sprite->y += cursor.translateSprite->y;
    return 1;
}

s32 AnimCmd_8(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_8) / 4;
    return 1;
}

s32 AnimCmd_SetIdAndVariant(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_SetIdAndVariant) / 4;
    sprite->animId = cursor.setIdAndVariant->animId;
    sprite->unk1B = 0xFF;
    sprite->variant = cursor.setIdAndVariant->variant;
    return -1;
}

s32 AnimCmd_10(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_10) / 4;
    return (intptr_t)cursor._10; // TODO: fix no return
}

s32 AnimCmd_SetPriority(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_SetPriority) / 4;
    sprite->unk8 = (sprite->unk8 & ~0x3000) | (cursor.setPriority->priority << 12);
    return 1;
}

s32 AnimCmd_12(union AnimCmd cursor, struct Sprite *sprite) {
    sprite->animCursor += sizeof(struct AnimCmd_12) / 4;
    sprite->unk14 = cursor._12->unk4 << 6;
    return 1;
}

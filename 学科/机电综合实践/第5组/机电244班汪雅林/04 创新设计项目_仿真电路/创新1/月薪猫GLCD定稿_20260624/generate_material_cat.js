const fs = require("fs");
const cp = require("child_process");

const SRC = "E:\\Program Files\\我很臭吗？\\图片素材\\128_64\\WPS图片批量处理\\frame-%d.jpg";
const RAW = "video_frames\\material_128x64.rgb";
const OUT = "yuexin_cat_material.c";
const W = 128;
const H = 64;
const FRAME_DELAY_MS = 0;

function runFfmpeg() {
  fs.mkdirSync("video_frames", { recursive: true });
  cp.execFileSync("ffmpeg", [
    "-y",
    "-framerate", "12",
    "-i", SRC,
    "-vf", "scale=128:64:flags=lanczos",
    "-pix_fmt", "rgb24",
    "-f", "rawvideo",
    RAW
  ], { stdio: "inherit" });
}

function readFrames() {
  const raw = fs.readFileSync(RAW);
  const frameSize = W * H * 3;
  const count = Math.floor(raw.length / frameSize);
  const frames = [];

  for (let f = 0; f < count; f++) {
    const bytes = new Array(1024).fill(0);
    const base = f * frameSize;
    for (let y = 0; y < H; y++) {
      for (let x = 0; x < W; x++) {
        const p = base + (y * W + x) * 3;
        const r = raw[p], g = raw[p + 1], b = raw[p + 2];
        const dark = r < 175 && g < 175 && b < 175;
        const notWhite = (255 - r) + (255 - g) + (255 - b) > 125;
        if (dark && notWhite) {
          bytes[(y >> 3) * 128 + x] |= 1 << (y & 7);
        }
      }
    }
    frames.push(bytes);
  }
  return frames;
}

function makeSixtyFrames(frames) {
  const out = [];
  for (let i = 0; i < frames.length; i++) {
    const cur = frames[i];
    const next = frames[(i + 1) % frames.length];
    out.push(cur);
    out.push(interpolateFrame(cur, next));
  }
  return out;
}

function interpolateFrame(a, b) {
  const out = new Array(1024);
  for (let i = 0; i < 1024; i++) {
    // If pixels disagree, keep the union. On a slow LCD this reads smoother than
    // a hard jump because transitional strokes overlap both poses.
    out[i] = a[i] | b[i];
  }
  return out;
}

function diffOne(prev, cur) {
  const runs = [];
  for (let page = 0; page < 8; page++) {
    let x = 0;
    while (x < W) {
      const idx = page * 128 + x;
      if (prev[idx] === cur[idx]) {
        x++;
        continue;
      }
      const start = x;
      const half = x < 64;
      const vals = [];
      while (x < W && (x < 64) === half && prev[page * 128 + x] !== cur[page * 128 + x] && vals.length < 255) {
        vals.push(cur[page * 128 + x]);
        x++;
      }
      runs.push({ page, start, vals });
    }
  }
  return runs;
}

function makePatches(frames) {
  const blank = new Array(1024).fill(0);
  const patches = [];
  for (let i = 0; i < frames.length; i++) {
    patches.push(diffOne(i === 0 ? blank : frames[i - 1], frames[i]));
  }
  patches.push(diffOne(frames[frames.length - 1], frames[0]));
  return patches;
}

function flatten(patches) {
  const offsets = [0];
  const data = [];
  for (const runs of patches) {
    for (const r of runs) data.push(r.page, r.start, r.vals.length, ...r.vals);
    offsets.push(data.length);
  }
  return { offsets, data };
}

function cBytes(bytes, indent = "    ") {
  const out = [];
  for (let i = 0; i < bytes.length; i += 16) {
    out.push(indent + bytes.slice(i, i + 16).map(n => "0x" + n.toString(16).padStart(2, "0").toUpperCase()).join(","));
  }
  return out.join(",\n");
}

function cWords(words) {
  const out = [];
  for (let i = 0; i < words.length; i += 12) {
    out.push("    " + words.slice(i, i + 12).join(","));
  }
  return out.join(",\n");
}

function generateC(patches) {
  const { offsets, data } = flatten(patches);
  return `/*
 * AT89C52 + LGM12641BS1R / KS0108 128x64 GLCD
 * Generated from E:/Program Files/.../图片素材/128_64 frames.
 * Separate material version, does not overwrite earlier animation sources.
 */

typedef unsigned char u8;
typedef unsigned int u16;

__sfr __at(0x80) P0;
__sfr __at(0xA0) P2;

__sbit __at(0xA0) LCD_DI;
__sbit __at(0xA1) LCD_RW;
__sbit __at(0xA2) LCD_E;
__sbit __at(0xA3) LCD_CS1;
__sbit __at(0xA4) LCD_CS2;
__sbit __at(0xA5) LCD_RST;

#define PATCH_COUNT ${patches.length}
#define FRAME_DELAY_MS ${FRAME_DELAY_MS}

static const u16 __code PATCH_OFFSETS[PATCH_COUNT + 1] = {
${cWords(offsets)}
};

static const u8 __code PATCH_DATA[${data.length}] = {
${cBytes(data)}
};

static void tiny_delay(void)
{
    __asm
        nop
        nop
    __endasm;
}

static void delay_ms(u16 ms)
{
    u16 i;
    while (ms--) {
        for (i = 0; i < 112; i++) tiny_delay();
    }
}

static void glcd_pulse(void)
{
    LCD_E = 1;
    tiny_delay();
    LCD_E = 0;
    tiny_delay();
}

static void glcd_select(u8 half)
{
    LCD_CS1 = (half == 0);
    LCD_CS2 = (half != 0);
}

static void glcd_write(u8 half, u8 is_data, u8 value)
{
    glcd_select(half);
    LCD_DI = is_data;
    LCD_RW = 0;
    P0 = value;
    tiny_delay();
    glcd_pulse();
}

static void glcd_cmd(u8 half, u8 cmd) { glcd_write(half, 0, cmd); }
static void glcd_data(u8 half, u8 value) { glcd_write(half, 1, value); }

static void glcd_set_pos(u8 x, u8 page)
{
    u8 half = (x >= 64);
    glcd_cmd(half, 0xB8 | (page & 0x07));
    glcd_cmd(half, 0x40 | (x & 0x3F));
}

static void glcd_init(void)
{
    P0 = 0x00;
    LCD_E = 0;
    LCD_RW = 0;
    LCD_CS1 = 1;
    LCD_CS2 = 1;
    LCD_RST = 0;
    delay_ms(20);
    LCD_RST = 1;
    delay_ms(20);
    glcd_cmd(0, 0x3F);
    glcd_cmd(1, 0x3F);
    glcd_cmd(0, 0xC0);
    glcd_cmd(1, 0xC0);
}

static void clear_screen(void)
{
    u8 page, half, x;
    for (page = 0; page < 8; page++) {
        for (half = 0; half < 2; half++) {
            glcd_set_pos(half ? 64 : 0, page);
            for (x = 0; x < 64; x++) glcd_data(half, 0);
        }
    }
}

static void write_patch(u8 patch)
{
    u16 i = PATCH_OFFSETS[patch];
    u16 end = PATCH_OFFSETS[patch + 1];
    u8 page, x, count, half;
    while (i < end) {
        page = PATCH_DATA[i++];
        x = PATCH_DATA[i++];
        count = PATCH_DATA[i++];
        half = (x >= 64);
        glcd_set_pos(x, page);
        while (count--) glcd_data(half, PATCH_DATA[i++]);
    }
}

void main(void)
{
    u8 patch = 0;
    glcd_init();
    clear_screen();
    while (1) {
        write_patch(patch);
        patch++;
        if (patch >= PATCH_COUNT) patch = 0;
#if FRAME_DELAY_MS > 0
        delay_ms(FRAME_DELAY_MS);
#endif
    }
}
`;
}

runFfmpeg();
const frames = readFrames();
const playbackFrames = makeSixtyFrames(frames);
const patches = makePatches(playbackFrames);
fs.writeFileSync(OUT, generateC(patches), "ascii");
const flat = flatten(patches);
console.log(`generated ${OUT}, sourceFrames=${frames.length}, playbackFrames=${playbackFrames.length}, patches=${patches.length}, patchBytes=${flat.data.length}`);

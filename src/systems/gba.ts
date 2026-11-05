import type { SystemProfile } from './types';

const base = import.meta.env.BASE_URL;

export const gba: SystemProfile = {
  id: 'gba',
  name: 'Game Boy Advance',
  shortName: 'GBA',

  core: {
    id: 'mgba',
    jsUrl: `${base}cores/mgba/mgba_libretro.js`,
    wasmUrl: `${base}cores/mgba/mgba_libretro.wasm`,
  },

  video: { width: 240, height: 160, fps: 59.7275, aspectRatio: 240 / 160 },
  audio: { sampleRate: 65536 },

  buttons: [
    { id: 'UP', label: '↑' },
    { id: 'DOWN', label: '↓' },
    { id: 'LEFT', label: '←' },
    { id: 'RIGHT', label: '→' },
    { id: 'A', label: 'A' },
    { id: 'B', label: 'B' },
    { id: 'L', label: 'L' },
    { id: 'R', label: 'R' },
    { id: 'START', label: 'Start' },
    { id: 'SELECT', label: 'Select' },
  ],

  extensions: ['gba', 'agb'],
  bios: {
    required: false,
    files: [{ name: 'gba_bios.bin', optional: true }],
  },
  saves: { sram: true, states: true },

  input: {
    keyboard: {
      UP: ['ArrowUp'],
      DOWN: ['ArrowDown'],
      LEFT: ['ArrowLeft'],
      RIGHT: ['ArrowRight'],
      A: ['KeyX'],
      B: ['KeyZ'],
      L: ['KeyA'],
      R: ['KeyS'],
      START: ['Enter'],
      SELECT: ['Backspace'],
    },
    gamepad: {
      UP: [12],
      DOWN: [13],
      LEFT: [14],
      RIGHT: [15],
      A: [1],
      B: [0],
      L: [4],
      R: [5],
      START: [9],
      SELECT: [8],
    },
    touch: {
      portrait: [
        { kind: 'dpad', buttons: ['UP', 'DOWN', 'LEFT', 'RIGHT'], cx: 24, cy: 55, w: 40, h: 40 },
        { kind: 'button', buttons: ['A'], label: 'A', cx: 86, cy: 48, w: 18, h: 18 },
        { kind: 'button', buttons: ['B'], label: 'B', cx: 70, cy: 64, w: 18, h: 18 },
        { kind: 'pill', buttons: ['L'], label: 'L', cx: 16, cy: 10, w: 26, h: 10 },
        { kind: 'pill', buttons: ['R'], label: 'R', cx: 84, cy: 10, w: 26, h: 10 },
        { kind: 'pill', buttons: ['SELECT'], label: 'Select', cx: 38, cy: 90, w: 18, h: 8 },
        { kind: 'pill', buttons: ['START'], label: 'Start', cx: 62, cy: 90, w: 18, h: 8 },
      ],
      landscape: [
        { kind: 'dpad', buttons: ['UP', 'DOWN', 'LEFT', 'RIGHT'], cx: 12, cy: 62, w: 44, h: 44 },
        { kind: 'button', buttons: ['A'], label: 'A', cx: 92, cy: 55, w: 20, h: 20 },
        { kind: 'button', buttons: ['B'], label: 'B', cx: 83, cy: 72, w: 20, h: 20 },
        { kind: 'pill', buttons: ['L'], label: 'L', cx: 8, cy: 14, w: 26, h: 10 },
        { kind: 'pill', buttons: ['R'], label: 'R', cx: 92, cy: 14, w: 26, h: 10 },
        { kind: 'pill', buttons: ['SELECT'], label: 'Select', cx: 43, cy: 92, w: 16, h: 8 },
        { kind: 'pill', buttons: ['START'], label: 'Start', cx: 57, cy: 92, w: 16, h: 8 },
      ],
    },
  },
};

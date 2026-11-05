// Mã nút joypad chuẩn của libretro (RETRO_DEVICE_ID_JOYPAD_*).
export const RETRO_JOYPAD = {
  B: 0,
  Y: 1,
  SELECT: 2,
  START: 3,
  UP: 4,
  DOWN: 5,
  LEFT: 6,
  RIGHT: 7,
  A: 8,
  X: 9,
  L: 10,
  R: 11,
  L2: 12,
  R2: 13,
  L3: 14,
  R3: 15,
} as const;

export type JoypadId = keyof typeof RETRO_JOYPAD;

/** Gộp danh sách nút đang nhấn thành một bitmask 16 bit để gửi vào nhân. */
export function toMask(pressed: Iterable<JoypadId>): number {
  let mask = 0;
  for (const id of pressed) mask |= 1 << RETRO_JOYPAD[id];
  return mask;
}

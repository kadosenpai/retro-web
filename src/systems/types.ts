import type { JoypadId } from './libretro';

export interface SystemButton {
  id: JoypadId;
  label: string;
}

/**
 * Một điều khiển trên touch overlay.
 * cx, cy: tâm, tính bằng % của vùng overlay.
 * w, h: kích thước, tính bằng % cạnh NGẮN của vùng overlay.
 * kind 'dpad': buttons = [UP, DOWN, LEFT, RIGHT].
 */
export interface TouchControl {
  kind: 'dpad' | 'button' | 'pill';
  buttons: JoypadId[];
  label?: string;
  cx: number;
  cy: number;
  w: number;
  h: number;
}

export interface BiosFile {
  name: string;
  optional: boolean;
}

export interface SystemProfile {
  id: string;
  name: string;
  shortName: string;

  core: {
    id: string;
    jsUrl: string;
    wasmUrl: string;
  };

  /** Giá trị danh nghĩa; giá trị thật được đọc từ nhân khi chạy (retro_get_system_av_info). */
  video: {
    width: number;
    height: number;
    fps: number;
    aspectRatio: number;
  };
  audio: {
    sampleRate: number;
  };

  buttons: SystemButton[];
  extensions: string[];
  bios: {
    required: boolean;
    files: BiosFile[];
  };
  saves: {
    sram: boolean;
    states: boolean;
  };

  input: {
    /** Giá trị là KeyboardEvent.code. */
    keyboard: Partial<Record<JoypadId, string[]>>;
    /** Giá trị là chỉ số nút theo bố cục "standard" của Gamepad API. */
    gamepad: Partial<Record<JoypadId, number[]>>;
    touch: {
      portrait: TouchControl[];
      landscape: TouchControl[];
    };
  };
}
